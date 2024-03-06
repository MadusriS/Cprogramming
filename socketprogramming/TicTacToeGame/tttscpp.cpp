#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <cstring>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>


using namespace std;
int f=-1;

#define PORT 3002
#define MAX_GAMES 4
#define BOARD_SIZE 3


class WebSocketHandler;

std::mutex gameMutex;

class Player {
public:
    int socket;
    char symbol;

    Player(int sock, char sym) : socket(sock), symbol(sym) {}
};

class Game {
public:
    char board[BOARD_SIZE][BOARD_SIZE];
    std::vector<Player> players;
    std::mutex mutex;

    Game() {
        initialize();
    }

    void initialize() {
        players.clear();
        memset(board, ' ', sizeof(board));
    }

    bool addPlayer(const Player& player) {
        if (players.size() < 2) {
            players.push_back(player);
            return true;
        }
        return false;
    }

    void broadcastUpdatedBoard();

    // Add the declaration for checkWin and checkDraw
    static bool checkWin(char board[BOARD_SIZE][BOARD_SIZE], char symbol);
    static bool checkDraw(char board[BOARD_SIZE][BOARD_SIZE]);
};

class WebSocketHandler {
public:
    static void sendWebSocketFrame(int socket, const std::string& message);
    static bool performWebSocketHandshake(int socket);
    static int parseWebSocketFrame(char *buffer, int len, char *output);
    static bool checkWin(char board[BOARD_SIZE][BOARD_SIZE], char symbol);
    static bool checkDraw(char board[BOARD_SIZE][BOARD_SIZE]);
};

class ClientHandler {
public:
    static void handlePlayerMoves(Game* currentGame, int playerNumber,int* flag);
    static void handleGame(Game* currentGame);
    static void handleClient(int clientSocket);
};


std::vector<Game> games(MAX_GAMES);//an array called games,having class game in its index.

void Game::broadcastUpdatedBoard() {
   // std::string message = "Current board state:\n";
    //mutex.lock();
   std::string message="";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
        if(board[i][j]=='X' ||board[i][j]=='O'){
        std::string r=to_string(i);
        std::string c=to_string(j);
       
            message += r;
            message += ',';
            message +=c;
            message +=',';
            message +=board[i][j];
            cout<<message<<endl;
            mutex.lock();
             for (const auto& player : players) {
        WebSocketHandler::sendWebSocketFrame(player.socket, message);
            
        }
        mutex.unlock();
        message.clear();
        }
        //message += '\n';
    }

    //mutex.lock();
    /*for (const auto& player : players) {
        WebSocketHandler::sendWebSocketFrame(player.socket, message);
    }*/
    //mutex.unlock();
}
}

void WebSocketHandler::sendWebSocketFrame(int socket, const std::string& message) {
    std::string frame;
        frame.push_back(0x81);  // Fin bit set, opcode for text data
        size_t len = message.size();
        if (len <= 125) {
            frame.push_back(static_cast<char>(len));
        } else {
            frame.push_back(126);
            frame.push_back(static_cast<char>((len >> 8) & 0xFF));
            frame.push_back(static_cast<char>(len & 0xFF));
        }
        frame += message;
        send(socket, frame.c_str(), frame.size(), 0);
}

bool WebSocketHandler::performWebSocketHandshake(int socket) {
       const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    // Read the client's WebSocket handshake request
    ssize_t bytesRead = recv(socket, buffer, BUFFER_SIZE, 0);
    if (bytesRead <= 0) {
        std::cerr << "Error reading WebSocket handshake request.\n";
        return false;
    }

    // Null-terminate the buffer
    buffer[bytesRead] = '\0';

    // Find the WebSocket key in the client's request
    const char* keyStart = strstr(buffer, "Sec-WebSocket-Key: ") + 19;
    const char* keyEnd = strchr(keyStart, '\r');
    if (!keyStart || !keyEnd) {
        std::cerr << "Invalid WebSocket handshake request.\n";
        return false;
    }

    // Extract the key
    std::string clientKey(keyStart, keyEnd - keyStart);

    // Concatenate the key with the magic string
    const char magicString[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string concatenatedKey = clientKey + magicString;

    // Compute SHA-1 hash
    unsigned char sha1Hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(concatenatedKey.c_str()), concatenatedKey.length(), sha1Hash);

    // Encode the hash as Base64
    BIO* bmem = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_push(b64, bmem);
    BIO_write(b64, sha1Hash, SHA_DIGEST_LENGTH);
    BIO_flush(b64);

    char base64Hash[64];
    BUF_MEM* bptr;
    BIO_get_mem_ptr(b64, &bptr);
    strncpy(base64Hash, bptr->data, sizeof(base64Hash) - 1);
    base64Hash[sizeof(base64Hash) - 1] = '\0';

    // Remove newline character from Base64 encoding
    for (int i = 0; i < sizeof(base64Hash); ++i) {
        if (base64Hash[i] == '\n') {
            base64Hash[i] = '\0';
            break;
        }
    }

    BIO_free_all(b64);

    // Send the WebSocket handshake response to the client
    std::ostringstream response;
    response << "HTTP/1.1 101 Switching Protocols\r\n";
    response << "Upgrade: websocket\r\n";
    response << "Connection: Upgrade\r\n";
    response << "Sec-WebSocket-Accept: " << base64Hash << "\r\n";
    response << "\r\n";

    std::string responseStr = response.str();
    send(socket, responseStr.c_str(), responseStr.length(), 0);

    return true;
}

int WebSocketHandler::parseWebSocketFrame(char *buffer, int len, char *output) {
      if (len < 6) {
        cout << "len < 6\n\n";
        return -1; // Insufficient data for header
    }

    // Check if it's a final frame and opcode is text (0x81)
    if ((buffer[0] & 0x80) == 0x80 && (buffer[0] & 0x0F) == 0x01) {
        int payload_len = buffer[1] & 0x7F;
        int mask_offset = 2;
        int data_offset = mask_offset + 4;

        if (payload_len <= 125) {
            // Extract payload
            for (int i = 0; i < payload_len; i++) {
                output[i] = buffer[data_offset + i] ^ buffer[mask_offset + i % 4];
            }
            output[payload_len] = '\0'; // Null-terminate string
            return payload_len;
        }
    }

    cout << "this will give an invalid frame\n";
    return -1; // Invalid frame
}

void ClientHandler::handlePlayerMoves(Game* currentGame, int playerNumber,int* flag) {
      while (true) {
      
      	if(*flag!=3){
		char buffer[1024];
		//bzero(buffer, sizeof(buffer));
		ssize_t bytesRead = read(currentGame->players[playerNumber].socket, buffer, sizeof(buffer));
		std::cout<<"outside if f "<<f<<" playerNumber"<<playerNumber<<endl;

		if (bytesRead > 0) {
		    char message[1024];
		    int payload_len = WebSocketHandler::parseWebSocketFrame(buffer, bytesRead, message);

		    if (payload_len > 0) {
		        int row, col;
		        sscanf(message, "%d %d", &row, &col);
		        cout<<message<<endl;

		        if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE && currentGame->board[row][col] == ' ' && (f==-1 || f!=playerNumber)) {
				    
				    currentGame->board[row][col] = currentGame->players[playerNumber].symbol;

				    // Broadcast the updated board to all connected clients
				    currentGame->broadcastUpdatedBoard();
				    f=playerNumber;
				    std::cout<<"inside if f "<<f<<" playerNumber"<<playerNumber<<endl;
				
				    if (WebSocketHandler::checkWin(currentGame->board, 'X')) {
				        // Handle win
				        const char* winMessage = "You won!";
				        const char* winMessage2 = "Player 1 wins!";
				        WebSocketHandler::sendWebSocketFrame(currentGame->players[0].socket, winMessage);
				        WebSocketHandler::sendWebSocketFrame(currentGame->players[1].socket, winMessage2);
				        *flag = 3;
				        break;
				    } 
				     else if (WebSocketHandler::checkWin(currentGame->board, 'O')) {
				        // Handle win
				        const char* winMessage = "You won!";
				        const char* winMessage2 = "Player 2 wins!";
				        WebSocketHandler::sendWebSocketFrame(currentGame->players[1].socket, winMessage);
				        WebSocketHandler::sendWebSocketFrame(currentGame->players[0].socket, winMessage2);
				        *flag = 3;
				        break;
				    }else if (WebSocketHandler::checkDraw(currentGame->board)){
				        // Handle draw
				        const char* drawMessage = "It's a draw!";
				        for (const auto& player : currentGame->players) {
				            WebSocketHandler::sendWebSocketFrame(player.socket, drawMessage);
				        }
				        *flag = 3;
				        break;
				    }
				    else {
					*flag = 1-playerNumber;
			    	    } 
			    }
			    else {
				    cout<<"invalidmove"<<endl;
				   
				    const char* invalidMoveMessage = "Invalid move!";
				    WebSocketHandler::sendWebSocketFrame(currentGame->players[playerNumber].socket, invalidMoveMessage);
				    f=playerNumber;
				    std::cout<<"inside else f "<<f<<" playerNumber"<<playerNumber<<endl;
			    }
		      }
		}
	}
    	else if(*flag == 3){
    		break;
    	}
/*for (const auto& player : currentGame->players) {
        close(player.socket);
    }*/
    }
}


void ClientHandler::handleGame(Game* currentGame) {
    // Assign symbols to players
    currentGame->players[0].symbol = 'X';
    currentGame->players[1].symbol = 'O';

    // Broadcast initial board state
    currentGame->broadcastUpdatedBoard();
    
    int flag = 0;

    // Handle player moves in separate threads
    std::thread player1Thread(handlePlayerMoves, currentGame, 0,&flag);
    std::thread player2Thread(handlePlayerMoves, currentGame, 1,&flag);

    // Wait for threads to finish (game ends)
    player1Thread.join();
    player2Thread.join();

    // Close sockets and clean up
    for (const auto& player : currentGame->players) {
        close(player.socket);
    }
    currentGame->initialize();  // Reset the game state
}

void ClientHandler::handleClient(int clientSocket) {
    if (!WebSocketHandler::performWebSocketHandshake(clientSocket)) {
            close(clientSocket);
            return;
        }

        Player newPlayer(clientSocket, ' ');//a player got a socket,and symbol space allocated.
        Game* currentGame = nullptr;//currrent game of class game created.

        for (auto& game : games) {
            std::lock_guard<std::mutex> lock(game.mutex);
            if (game.addPlayer(newPlayer)) {
                currentGame = &game;
                break;
            }
        }

        if (!currentGame) {
            // No available games, create a new one
            for (auto& game : games) {
                std::lock_guard<std::mutex> lock(game.mutex);
                if (game.players.empty()) {
                    game.initialize();
                    game.addPlayer(newPlayer);
                    currentGame = &game;
                    break;
                }
            }
        }

        if (!currentGame) {
            // Couldn't join a game, handle accordingly
            std::cerr << "Couldn't join a game.\n";
            close(clientSocket);
            return;
        }

        // Notify the player of their symbol
        std::string symbolMessage = "You are Player " + std::to_string(currentGame->players.size());
        WebSocketHandler::sendWebSocketFrame(clientSocket, symbolMessage);

        if (currentGame->players.size() == 2) {
            // Start the game
            std::thread gameThread(handleGame, currentGame);
            gameThread.detach();
        }
}


bool WebSocketHandler::checkWin(char board[BOARD_SIZE][BOARD_SIZE], char symbol) {
      for (int i = 0; i < BOARD_SIZE; i++) {
        if ((board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) ||
            (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol)) {
            return true;
        }
    }

    // Check diagonals
    if ((board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) ||
        (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol)) {
        return true;
    }

    return false;
}


bool WebSocketHandler::checkDraw(char board[BOARD_SIZE][BOARD_SIZE]) {
        for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == ' ') {
                return false; // There is an empty space, game not draw
            }
        }
    }
    return true; // Board is full, game draw
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating server socket.\n";
        return -1;
    }

	int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Error setting socket options.\n";
        close(serverSocket);
        return -1;
    }
    // Set up server address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the server socket
    if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding server socket.\n";
        close(serverSocket);
        return -1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening for connections.\n";
        close(serverSocket);
        return -1;
    }

    std::cout << "Server listening on port " << PORT << ".\n";

    while (true) {
        // Accept incoming connection
        clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientLen);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection.\n";
            continue;
        }

        // Start a new thread to handle the client
        std::cout<<"im here after listen and accept"<<endl;
        std::thread(ClientHandler::handleClient, clientSocket).detach();/*
This line of code creates a new std::thread object, invokes the handleClient function of the ClientHandler class, and then immediately detaches the thread.*/
    }

    // Close server socket (this part may not be reached in practice)
    close(serverSocket);

    return 0;
}

