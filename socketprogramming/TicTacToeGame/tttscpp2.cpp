#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <vector>
#include <mutex>
#include <atomic>
//#include <pthread.h>
#include<thread>
using namespace std;

#define BUFFER_SIZE 1024
#define BOARD_SIZE 3
#define PORT 3002

std::atomic<int> playerNumberCounter{0};
std::mutex gameMutex;

class Game {
public:
    char board[BOARD_SIZE][BOARD_SIZE];
    int playerCount;
    std::vector<int> clientSockets;

    Game() {
        initialize();
    }

    void initialize() {
        playerCount = 0;
        clientSockets.push_back(0);
    	clientSockets.push_back(0);
        memset(board, ' ', sizeof(board));
    }
};

class WebSocketHandler {
public:
    static void encodeAndSend(const char *message, int len, int new_socket);
    static int parseWebSocketFrame(char *buffer, int len, char *output);
};

void WebSocketHandler::encodeAndSend(const char *message, int len, int new_socket) {
    char frame[BUFFER_SIZE];
    frame[0] = 0x81; // Fin bit set, opcode for text data
    frame[1] = 0x80 | len; // Mask bit set, payload length

    // Generate random mask key
    unsigned char mask_key[4];
    srand(time(NULL));
    for (int i = 0; i < 4; i++) {
        mask_key[i] = rand() % 256;
    }

    // Copy mask key into frame
    memcpy(frame + 2, mask_key, 4);

    // Mask the payload
    for (int i = 0; i < len; i++) {
        frame[i + 6] = message[i] ^ mask_key[i % 4];
    }

    // Send the frame to the client
    std::cout<<"encode and send is succesful"<<std::endl;
    send(new_socket, frame, len + 6, 0);
}

int WebSocketHandler::parseWebSocketFrame(char *buffer, int len, char *output) {
    if (len < 6) {
        std::cout << "len < 6\n\n";
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

    std::cout << "this will give an invalid frame\n";
    return -1; // Invalid frame
}

class TicTacToeGame {
public:
    static std::vector<Game> games;
    //Game* currentGame;
    static void initializeBoard(Game* game);
    //static void initializeBoard(char board[BOARD_SIZE][BOARD_SIZE]);
    static void printBoard(char board[BOARD_SIZE][BOARD_SIZE]);
    static bool checkWin(char board[BOARD_SIZE][BOARD_SIZE], char symbol);
    static bool checkDraw(char board[BOARD_SIZE][BOARD_SIZE]);
    static void broadcastUpdatedBoard(Game *game);
};

void TicTacToeGame::broadcastUpdatedBoard(Game *game) {
    gameMutex.lock(); // Lock the mutex before broadcasting
    char message[1024];
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            snprintf(message, sizeof(message), "%d,%d,%c", i, j, game->board[i][j]);
            std::cout<<"in broadcastUpdatedBoard and the message sent is "<<message<<std::endl;
            if (game->board[i][j] == 'X' || game->board[i][j] == 'O') {
            std::cout<<"game->clientSockets.size()"<<game->clientSockets.size()<<std::endl;
                for (size_t k = 0; k < game->clientSockets.size(); k++) {
                std::cout<<"about to encode and send"<<std::endl;
                    WebSocketHandler::encodeAndSend(message, strlen(message), game->clientSockets[k]);
                }
            }
        }
    }
    gameMutex.unlock(); // Unlock the mutex after broadcasting
}


void TicTacToeGame::initializeBoard(Game* currentGame) {
std::cout<<"currentGame in initializing "<<currentGame<<std::endl;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            currentGame->board[i][j] = ' ';
        }
    }

    std::cout << "after initializing board:\n";
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            std::cout << currentGame->board[i][j];
        }
        std::cout << "\n";
    }
}

void TicTacToeGame::printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            std::cout << board[i][j] << ' ';
        }
        std::cout << "\n";
    }
}

bool TicTacToeGame::checkWin(char board[BOARD_SIZE][BOARD_SIZE], char symbol) {
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

bool TicTacToeGame::checkDraw(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == ' ') {
                return false; // There is an empty space, game not draw
            }
        }
    }
    return true; // Board is full, game draw
}

class ClientHandler {
public:
    static void handleWebSocket(int clientSocket, int playerNumber);
    static void handlePlayerMoves(Game *currentGame, int playerNumber);
   
};
std::vector<Game> TicTacToeGame::games;


void ClientHandler::handlePlayerMoves(Game *currentGame, int playerNumber) {

std::cout<<"in handlePlayerMoves the playerNumber is "<<playerNumber<<std::endl;
std::cout<<"in handlePlayerMoves the ,currentGame of  playerNumber"<<playerNumber<<" is "<<currentGame<<std::endl;

    char buffer[1024];
    ssize_t bytesRead;
    TicTacToeGame::initializeBoard(currentGame);

    while (true) {
        // Read the WebSocket frame
        bzero(buffer, sizeof(buffer));
        std::cout<<"currentGame->clientSockets[playerNumber]"<<currentGame->clientSockets[playerNumber]<<std::endl;
        bytesRead = read(currentGame->clientSockets[playerNumber], buffer, sizeof(buffer));
std::cout<<"the buufer rcvd and bytesread"<<buffer<<bytesRead<<std::endl;
        if (bytesRead > 0) {
            // Parse WebSocket frame and extract payload
            char message[1024];
            int payload_len = WebSocketHandler::parseWebSocketFrame(buffer, bytesRead, message);

            if (payload_len > 0) {
                // Handle WebSocket data (Tic-Tac-Toe moves)
                std::cout << "Move received from Player " << playerNumber << ": " << message << std::endl;

                int row, col;
                sscanf(message, "%d %d", &row, &col);
                std::cout<<"rcvd msg fom the client is "<<message<<std::endl;
                std::cout<<"currentGame->board[row][col] of "<<currentGame<<" is "<<currentGame->board[row][col]<<std::endl;
                //&& currentGame->board[row][col] == ' '

                if (row >= 0 && row < BOARD_SIZE &&
                    col >= 0 && col < BOARD_SIZE && (currentGame->board[row][col] != 'X' || currentGame->board[row][col] != 'O')) {
                    currentGame->board[row][col] = (playerNumber % 2 == 0) ? 'X' : 'O';

                    // Broadcast the updated board to all connected clients
                    TicTacToeGame::broadcastUpdatedBoard(currentGame);

                    // Check for a win or draw
                    if (TicTacToeGame::checkWin(currentGame->board, 'X')) {
                        std::cout << "Player 1 wins!" << std::endl;
                        const char* msg1 = "you won!";
                        const char* msg2 = "Player 1 won!";
                        WebSocketHandler::encodeAndSend(msg1, strlen(msg1), currentGame->clientSockets[1]);
                        WebSocketHandler::encodeAndSend(msg2, strlen(msg2), currentGame->clientSockets[0]);
                        break;
                    } else if (TicTacToeGame::checkWin(currentGame->board, 'O')) {
                        std::cout << "Player 2 wins!" << std::endl;
                        const char* msg1 = "you won!";
                        const char* msg2 = "Player 2 won!";
                        WebSocketHandler::encodeAndSend(msg1, strlen(msg1), currentGame->clientSockets[0]);
                        WebSocketHandler::encodeAndSend(msg2, strlen(msg2), currentGame->clientSockets[1]);
                        break;
                    } else if (TicTacToeGame::checkDraw(currentGame->board)) {
                        std::cout << "It's a draw!" << std::endl;
                        const char* msg = "It's a draw!";
                        WebSocketHandler::encodeAndSend(msg, strlen(msg), currentGame->clientSockets[0]);
                        WebSocketHandler::encodeAndSend(msg, strlen(msg), currentGame->clientSockets[1]);
                        break;
                    }
                } else {
                    // Invalid move
                    std::cout << "Invalid move received from Player " << playerNumber << std::endl;
                }
            } else {
                std::cout << "Invalid WebSocket frame received" << std::endl;
            }
        }
    }
}

void ClientHandler::handleWebSocket(int clientSocket, int playerNumber) {

    char buffer[1024];
    ssize_t bytesRead;
    char board[BOARD_SIZE][BOARD_SIZE];
    //TicTacToeGame::initializeBoard(currentGame);

    // Read the WebSocket handshake request
    bytesRead = read(clientSocket, buffer, sizeof(buffer));

    if (bytesRead > 0) {
        // Extract the WebSocket key from the request
        char *keyStart = strstr(buffer, "Sec-WebSocket-Key: ") + 19;
        char *keyEnd = strchr(keyStart, '\r');
        *keyEnd = '\0';

        // Concatenate the key with the magic string
        char concatenatedKey[128];
        const char magicst[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        strcpy(concatenatedKey, keyStart);
        strcat(concatenatedKey, magicst);

        // Compute SHA-1 hash
        static unsigned char sha1Hash[SHA_DIGEST_LENGTH];
        SHA1((const unsigned char *)concatenatedKey, strlen(concatenatedKey), sha1Hash);

        // Encode the hash as Base64
        BIO *bmem = BIO_new(BIO_s_mem());
        BIO *b64 = BIO_new(BIO_f_base64());

        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        BIO_push(b64, bmem);
        BIO_write(b64, sha1Hash, SHA_DIGEST_LENGTH);
        BIO_flush(b64);

        char *base64Hash = (char *)malloc(40);
        int Length;
        BUF_MEM *bptr;
        BIO_get_mem_ptr(b64, &bptr);
        strcpy(base64Hash, bptr->data);
        Length = strlen(base64Hash);

        // Remove newline character from Base64 encoding
        if (Length > 0 && base64Hash[Length - 1] == '\n') {
            Length--;
        }
        std::cout << std::endl;
        std::cout << "the base is:" << base64Hash << std::endl;
        BIO_free_all(b64);

        // Respond with the WebSocket handshake
        const char *responseFormat = "HTTP/1.1 101 Switching Protocols\r\n"
                                     "Upgrade: websocket\r\n"
                                     "Connection: Upgrade\r\n"
                                     "Sec-WebSocket-Accept: %.*s\r\n"
                                     "\r\n";
        char response[256];
        snprintf(response, sizeof(response), responseFormat, Length, base64Hash);
        write(clientSocket, response, strlen(response));
    }
    std::cout<<"after handshake"<<std::endl;

    // Assign the player to a game
    gameMutex.lock();
    //Game *currentGame = nullptr;
    int currentGameIndex = -1;
    
	std::cout<<"TicTacToeGame::games.size()"<<TicTacToeGame::games.size()<<std::endl;
    for (int i=0;i<TicTacToeGame::games.size();i++){
    
        if (TicTacToeGame::games[i].playerCount < 2) {
            //currentGame = &TicTacToeGame::games[i];
             currentGameIndex = i;
            TicTacToeGame::games[i].clientSockets[1] = clientSocket;
            //currentGame = &game;
            break;
        }
    }
   
        // Create a new game if no available game found
        if (currentGameIndex == -1) {
        Game newgame;
        newgame.initialize();
        TicTacToeGame::games.push_back(newgame);
        std::cout<<"after creating newgame TicTacToeGame::games.size()"<<TicTacToeGame::games.size()<<std::endl;
        //currentGame = &TicTacToeGame::games.back();
        currentGameIndex=TicTacToeGame::games.size()-1;
                
        TicTacToeGame::games[currentGameIndex].clientSockets[0] = clientSocket;
        //currentGame->playerCount = 0;
    }
    std::cout<<"currentGameIndex"<<currentGameIndex<<std::endl;
    gameMutex.unlock();
    (&TicTacToeGame::games[currentGameIndex])->playerCount++;
    gameMutex.unlock();
    //currentGame->playerCount++;

    //gameMutex.unlock();
    std::cout<<"the playercount"<<(&TicTacToeGame::games[currentGameIndex])->playerCount<<std::endl;

    //currentGame->playerCount++;
    std::cout<<"&TicTacToeGame::games[currentGameIndex]"<<&TicTacToeGame::games[currentGameIndex]<<std::endl;
    //TicTacToeGame::broadcastUpdatedBoard(&TicTacToeGame::games[currentGameIndex]);
    TicTacToeGame::initializeBoard(&TicTacToeGame::games[currentGameIndex]); 
    ClientHandler::handlePlayerMoves(&TicTacToeGame::games[currentGameIndex], (&TicTacToeGame::games[currentGameIndex])->playerCount - 1);

    close(clientSocket);
}

std::vector<Game> games; // Maintain a vector of active games

void *handleWebSocketThread(void *arg) {
std::cout<<std::endl;
std::cout<<"///////////////////////////////////////////////////////////////"<<std::endl;
std::cout<<"in handleWebSocketThread"<<std::endl;
//int* clientSocketPtr = static_cast<int*>(arg);
//pthread_t threadId = pthread_self();
    //std::cout << "Thread ID: " << threadId << std::endl;
    int clientSocket = *(int *)arg;
    //free(arg);
    //std::unique_lock<std::mutex> lock(gameMutex);
    int currentPlayerNumber = playerNumberCounter++;
    std::cout<<"currentplayernumber"<<currentPlayerNumber<<std::endl;
    gameMutex.unlock();

    ClientHandler::handleWebSocket(clientSocket, currentPlayerNumber);

    close(clientSocket);
    //pthread_exit(NULL);
     //delete clientSocketPtr;
    pthread_exit(NULL);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set the socket option to allow address reuse
    int reuseAddr = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1) {
        perror("Error setting socket option");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 8) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    std::cout << "WebSocket Server listening on port " << PORT << std::endl;

    while (1) {
        // Accept connections from clients
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Error accepting connection");
            continue;
        }

        
        thread th(&handleWebSocketThread,&clientSocket);
        th.detach();
      
    }

    close(serverSocket);
    pthread_exit(NULL);

    return 0;
}

