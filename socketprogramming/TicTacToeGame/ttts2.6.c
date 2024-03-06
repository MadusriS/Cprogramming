#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdbool.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define BOARD_SIZE 3
#define PORT 3002
#define MAX_GAMES 4
#define GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define maxclients 8
int f=0;

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    int playerCount;
    int clientSockets[2];
} Game;

Game games[MAX_GAMES];

int m = 0;
char board[BOARD_SIZE][BOARD_SIZE];
pthread_t playerNumberCounter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int client_sockets[maxclients];

void initializeGame(Game *game) {
    game->playerCount = 0;
    memset(game->board, ' ', sizeof(game->board));
}

void encodeAndSend(const char *message, int len, int new_socket) 
{
    // Construct WebSocket frame
    char frame[BUFFER_SIZE];
    frame[0] = 0x81; // Fin bit set, opcode for text data
    frame[1] = 0x80 | len; // Mask bit set, payload length

    // Generate random mask key
    unsigned char mask_key[4];
    for (int i = 0; i < 4; i++) 
    {
        mask_key[i] = rand() % 256;
    }

    // Copy mask key into frame
    memcpy(frame + 2, mask_key, 4);

    // Mask the payload
    for (int i = 0; i < len; i++) 
    {
        frame[i + 6] = message[i] ^ mask_key[i % 4];
    }
    //printf("Frame: %s\n",frame);

    // Send the frame to the client
     //printf("\n 1)encode and sending the frame :'%s'\n",frame);
    
    send(new_socket, frame, len + 6, 0);
    //printf("\nencode and sending the frame :'%s'\n",frame);
}


// Function to parse WebSocket frame and extract payload
int parseWebSocketFrame(char *buffer, int len, char *output) {
//printf("in parseWebSocketFrame\n \n ");
    if (len < 6){
    printf(" len<6\n \n");
        return -1;} // Insufficient data for header

    // Check if it's a final frame and opcode is text (0x81)
    if ((buffer[0] & 0x80) == 0x80 && (buffer[0] & 0x0F) == 0x01) {
    //printf(" 1)inside if \n \n");
        int payload_len = buffer[1] & 0x7F;
        int mask_offset = 2;
        int data_offset = mask_offset + 4;

        if (payload_len <= 125) {
            // Extract payload
            //printf(" 2)inside if \n \n");
            for (int i = 0; i < payload_len; i++) {
                output[i] = buffer[data_offset + i] ^ buffer[mask_offset + i % 4];
                //printf(" 3)inside if \n \n");
            }
            output[payload_len] = '\0'; // Null-terminate string
            return payload_len;
        }
    }
    printf("this will give invalid frame\n");
    return -1; // Invalid frame
}

// Function to initialize the Tic-Tac-Toe board
void initializeBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = ' ';
        }
    }

    printf("after initializing board:\n");
    for (int i = 0; i < BOARD_SIZE; i++) {

        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}

// Function to print the Tic-Tac-Toe board
void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

// Function to check if a player has won
bool checkWin(char board[BOARD_SIZE][BOARD_SIZE], char symbol) {
    // Check rows and columns
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

// Function to check if the board is full (draw)
bool checkDraw(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == ' ') {
                return false; // There is an empty space, game not draw
            }
        }
    }
    return true; // Board is full, game draw
}

// Function to broadcast the updated board to all connected clients
/*void broadcastUpdatedBoard() {
    char updatedBoard[1024];
    snprintf(updatedBoard, sizeof(updatedBoard), "Current board state:\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            strncat(updatedBoard, &board[i][j], 1);
            strcat(updatedBoard, " ");
        }
        strcat(updatedBoard, "\n");
    }

    pthread_mutex_lock(&mutex); // Lock the mutex before broadcasting
    for (int i = 1; i <= playerNumberCounter; i++) {
     printf("my updated board");
        write(i, updatedBoard, strlen(updatedBoard));
        printf("my updated board");
    }
    pthread_mutex_unlock(&mutex); // Unlock the mutex after broadcasting
}*/

void broadcastUpdatedBoard(Game *game) {
    char updatedBoard[1024];
    snprintf(updatedBoard, sizeof(updatedBoard), "Current board state:\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
           strncat(updatedBoard, &(game->board[i][j]), 1);

            strcat(updatedBoard, " ");
        }
        strcat(updatedBoard, "\n");
    }

    // Convert the board state to the required format
//"(x,y,symbol)
    pthread_mutex_lock(&mutex); // Lock the mutex before broadcasting
    char message[1024];
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            snprintf(message, sizeof(message), "%d,%d,%c", i, j, game->board[i][j]);
            if(game->board[i][j]=='X' || game->board[i][j]=='O' ){
            for (int k = 0; k <= game->playerCount; k++) {
            printf("\nencode and sending for msg:'%s' having player count %d\n ",message,k);
                encodeAndSend(message, strlen(message),game->clientSockets[k]);
            }}
            
            }}
        
    pthread_mutex_unlock(&mutex); // Unlock the mutex after broadcasting
    
}



// Function to handle WebSocket communication for a specific client
void handleWebSocket(int clientSocket, int playerNumber) {
    char buffer[1024];
    ssize_t bytesRead;
    initializeBoard(board);

    // Read the WebSocket handshake request
    bytesRead = read(clientSocket, buffer, sizeof(buffer));
    //printf("WebSocket handshake request received from Player %d\n", playerNumber);
    //printf("the buffer is %s", buffer);
    if (bytesRead > 0) {
        // Extract the WebSocket key from the request
        char *keyStart = strstr(buffer, "Sec-WebSocket-Key: ") + 19;
        char *keyEnd = strchr(keyStart, '\r');
        *keyEnd = '\0';

        // Concatenate the key with the magic string
        char concatenatedKey[128];
        char magicst[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        strcpy(concatenatedKey, keyStart);
        strcat(concatenatedKey, magicst);
        //printf("the concatenated key is %s\n", concatenatedKey);

        // Compute SHA-1 hash
        static unsigned char sha1Hash[SHA_DIGEST_LENGTH];
        SHA1((const unsigned char *)concatenatedKey, strlen(concatenatedKey), sha1Hash);
        //printf("sha1 is %s\n", sha1Hash);

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
        printf("\n");
        printf("the base is:%s\n ", base64Hash);
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
    // Find an available game or create a new one
    pthread_mutex_lock(&mutex);
    int currentGameIndex = -1;
    for (int i = 0; i < MAX_GAMES; i++) {
        if (games[i].playerCount < 2) {
            currentGameIndex = i;
            printf(" 1) now the game %d is running and has playercount %d \n ",i,games[i].playerCount);
            break;
        }
    }

    if (currentGameIndex == -1) {
        // No available games, create a new one
        for (int i = 0; i < MAX_GAMES; i++) {
            if (games[i].playerCount == 0) {
                currentGameIndex = i;
                initializeGame(&games[i]);
                printf(" 2) now the game %d is running and has playercount %d \n ",i,games[i].playerCount);
                break;
            }
        }
    }

    // Add the player to the selected game
    Game *currentGame = &games[currentGameIndex];
    currentGame->clientSockets[currentGame->playerCount] = clientSocket;//
    printf("1)the current game has player count %d\n",currentGame->playerCount);
    
    currentGame->playerCount++;
    printf("2)the current game has player count %d\n",currentGame->playerCount);
    pthread_mutex_unlock(&mutex);

    // Send the initial board state to the connected client
    broadcastUpdatedBoard(currentGame);

    // WebSocket communication
    while (1) {
        // Read the WebSocket frame
        bzero(buffer, sizeof(buffer));
        bytesRead = read(clientSocket, buffer, sizeof(buffer));
        //printf("WebSocket handshake request received from Player %d\n", playerNumber);
    //printf("the buffer is %s", buffer);
        //printf("bytes read =%ld \n",bytesRead);
        if (bytesRead > 0) {
            // Parse WebSocket frame and extract payload
            char message[1024];
            int payload_len = parseWebSocketFrame(buffer, bytesRead, message);
            if (payload_len > 0) {
                // Handle WebSocket data (Tic-Tac-Toe moves)
                printf("Move received from Player %d: %s\n", playerNumber, message);

                int row, col;
                sscanf(message, "%d %d", &row, &col);
                printf("message rcvd from client %s\n",message);

                if (row >= 0 && row < BOARD_SIZE &&
                    col >= 0 && col < BOARD_SIZE &&
                    currentGame->board[row][col] == ' ' && (f==0 || f!=playerNumber)) {
                    printf("now the currrent game's playercount is :%d\n",currentGame->playerCount);

                    // Valid move, update the board
                    currentGame->board[row][col] = ( playerNumber%2 == 0) ? 'O' : 'X';

                    f=playerNumber;// Broadcast the updated board to all connected clients
                    broadcastUpdatedBoard(currentGame);

                    // Check for a win or draw
                    if (checkWin(currentGame->board, 'X')) {
                        printf("Player 1 wins!\n");
                        char*msg1="you won!";
                        char*msg2="Player 1 won!";
                        
                        encodeAndSend(msg1,strlen(msg1),currentGame->clientSockets[1]) ;
                        encodeAndSend(msg2,strlen(msg2),currentGame->clientSockets[0]) ;
                        break;
                    } else if (checkWin(currentGame->board, 'O')) {
                        printf("Player 2 wins!\n");
                        char*msg1="you won!";
                        char*msg2="Player 2 won!";
                        
                        encodeAndSend(msg1,strlen(msg1),currentGame->clientSockets[0]) ;
                        encodeAndSend(msg2,strlen(msg2),currentGame->clientSockets[1]) ;
                        break;
                    } else if (checkDraw(currentGame->board)) {
                        printf("It's a draw!\n");
                        char*msg="It's a draw!";
                        encodeAndSend(msg,strlen(msg),currentGame->clientSockets[0]) ;
                        encodeAndSend(msg,strlen(msg),currentGame->clientSockets[1]) ;
                        
                        
                        break;
                    }
                } else {
                    // Invalid move
                    printf("Invalid move received from Player %d\n", playerNumber);
                }
            } else {
                printf("Invalid WebSocket frame received\n");
            }
        }
    }

   for(int i=0;i<2;i++){
    close(currentGame->clientSockets[i]);}

    // Remove the player from the game
    pthread_mutex_lock(&mutex);
    //currentGame->playerCount--;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL); // Terminate child process after handling the connection
}
// Thread function to handle WebSocket communication
/*void *handleWebSocketThread(void *arg) {
    printf("in *handleWebSocketThread\n\n\n");
    int clientSocket = *(int *)arg;
    free(arg); 
    //pthread_t currentPlayerNumber;

    // Atomically increment playerNumberCounter to get a unique player number
    printf("%ld\n is the playercounter", playerNumberCounter);

    if (playerNumberCounter < maxclients) {
        client_sockets[playerNumberCounter] = clientSocket;
        printf(" client_sockets[playerNumberCounter] = clientSocket; %d\n",clientSocket);
        printf(" client_sockets[playerNumberCounter] = clientSocket; %d\n",client_sockets[playerNumberCounter]);
    }
    

    // Use playerNumberCounter as player number
    handleWebSocket(client_sockets[playerNumberCounter], playerNumberCounter);

    // Cleanup and exit the thread
    //close(client_sockets[playerNumberCounter]);
    currentPlayerNumber = __sync_add_and_fetch(&playerNumberCounter, 1);

    for (int i = 0; i < playerNumberCounter; i++) {
        close(client_sockets[i]);
        printf("2) client socket %d getting closed", i);
    }
    free(arg);
    pthread_exit(NULL);
}*/
void *handleWebSocketThread(void *arg) {
    int clientSocket = *(int *)arg;
    free(arg); // Free the memory allocated for the argument

    pthread_mutex_lock(&mutex);
    int currentPlayerNumber = playerNumberCounter++;
    pthread_mutex_unlock(&mutex);

    handleWebSocket(clientSocket, currentPlayerNumber);

    close(clientSocket);
    pthread_exit(NULL);
}


int main() {
for (int i = 0; i < MAX_GAMES; i++) {
        initializeGame(&games[i]);
    }

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
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, maxclients) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("WebSocket Server listening on port %d\n", PORT);

    while (1) {
        // Accept connections from clients
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Create a new thread to handle the connection
        pthread_t thread;
        int *clientSocketPtr = malloc(sizeof(int));
        *clientSocketPtr = clientSocket;

        if (pthread_create(&thread, NULL, handleWebSocketThread, clientSocketPtr) != 0) {
            perror("Error creating thread");
            close(clientSocket);
            free(clientSocketPtr);
        }
    }

    close(serverSocket);

    return 0;}
