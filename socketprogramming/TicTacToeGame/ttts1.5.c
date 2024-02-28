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
#define PORT 3001
#define maxclients 2
#define GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
int m=0;
char board[BOARD_SIZE][BOARD_SIZE];
pthread_t playerNumberCounter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int client_sockets[maxclients];

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
    send(new_socket, frame, len + 6, 0);
}


// Function to parse WebSocket frame and extract payload
int parseWebSocketFrame(char *buffer, int len, char *output) {
    if (len < 6)
        return -1; // Insufficient data for header

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

void broadcastUpdatedBoard(int clientSocket) {
    char updatedBoard[1024];
    snprintf(updatedBoard, sizeof(updatedBoard), "Current board state:\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            strncat(updatedBoard, &board[i][j], 1);
            strcat(updatedBoard, " ");
        }
        strcat(updatedBoard, "\n");
    }

    // Convert the board state to the required format (x,y,symbol)
    pthread_mutex_lock(&mutex); // Lock the mutex before broadcasting
    char message[1024];
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            snprintf(message, sizeof(message), "%d,%d,%c", i, j, board[i][j]);
            if(board[i][j]=='X' || board[i][j]=='O' ){
            for (int k = 0; k < playerNumberCounter; k++) {
                encodeAndSend(message, strlen(message),client_sockets[k]);
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
    printf("WebSocket handshake request received from Player %d\n", playerNumber);
    printf("the buffer is %s", buffer);
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
        printf("the concatenated key is %s\n", concatenatedKey);

        // Compute SHA-1 hash
        static unsigned char sha1Hash[SHA_DIGEST_LENGTH];
        SHA1((const unsigned char *)concatenatedKey, strlen(concatenatedKey), sha1Hash);
        printf("sha1 is %s\n", sha1Hash);

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

    // Send the initial board state to the connected client
    //broadcastUpdatedBoard();

    // WebSocket communication
    while (1) {
        // Read the WebSocket frame
        bzero (buffer, sizeof (buffer));
       // for(int i=0;i<10;i++){
        bytesRead = read(client_sockets[m], buffer, sizeof(buffer));
        if (bytesRead > 0) {
            // Parse WebSocket frame and extract payload
            
            char message[1024];
            int payload_len = parseWebSocketFrame(buffer, bytesRead, message);
            printf ("Mess: %s\n", message);
            if (payload_len > 0) {
                // Handle WebSocket data (Tic-Tac-Toe moves)
                printf("Move received from Player %d: %s\n", playerNumber, message);

                int row, col;
                sscanf(message, "%d %d", &row, &col);
                printf("row and col %d,%d\n", row, col);
                
                
                printf("%c\n", board[row][col]);
                
                
                if (
                    row >= 0 && row < BOARD_SIZE &&
                    col >= 0 && col < BOARD_SIZE &&
                    board[row][col] == ' ') {

                    // Valid move, update the board
                    board[row][col] = (m == 1) ? 'O' : 'X';

                    // Print the updated board
                    printf("Current board state:\n");
                    printBoard(board);
                   
                    //broadcastUpdatedBoard(clientSocket);
                     broadcastUpdatedBoard(client_sockets[m]);

                    // Check for a win or draw
                    if (checkWin(board, 'X')) {
                        printf("Player 1 wins!\n");
                        char *msg1="YOU WON";
                        
                	encodeAndSend(msg1, strlen(msg1),client_sockets[0]);
                	char *msg2="Player 1 wins!";
            
                        encodeAndSend(msg2, strlen(msg2), client_sockets[1]) ;
                        break;
                    } else if (checkWin(board, 'O')) {
                        printf("Player 2 wins!\n");
                        char *msg1="YOU WON";
                        
                	encodeAndSend(msg1, strlen(msg1),client_sockets[1]);
                	char *msg2="Player 1 wins!";
            
                        encodeAndSend(msg2, strlen(msg2), client_sockets[0]) ;
                        
                        break;
                    } else if (checkDraw(board)) {
                        printf("It's a draw!\n");
                        char* mesg="It's a draw!";
                        for (int k = 0; k <2; k++) {
                	encodeAndSend(mesg, strlen(mesg),client_sockets[k]);
            		}
                        break;
                    }
	
                    // Broadcast the updated board to all connected clients
                    //broadcastUpdatedBoard(client_sockets[m]);
                    if(m==0){
                    m++;
                    }
                    else{
                    m--;
                    }

                    // Echo the updated board back to the respective player
                    /*char moveMessage[1024];
                    snprintf(moveMessage, sizeof(moveMessage), "Your move: Row %d, Column %d", row, col);
                    //void encodeAndSend(const char *message, int len, int new_socket)
                    write(clientSocket, moveMessage, strlen(moveMessage));
                    */
                } else {
                    // Invalid move
                    printf("Invalid move received from Player %d\n", playerNumber);
                }
            } else {
                printf("Invalid WebSocket frame received\n");
            }
        }
   // }
    }
    //close(serverSocket);

     for (int i = 0; i < 2 ; i++) 
    {
        close(client_sockets[i]);
        printf("client socket %d getting closed",i);
    }
    
    pthread_exit(NULL); // Terminate child process after handling the connection
}

// Thread function to handle WebSocket communication
void *handleWebSocketThread(void *arg) {

printf("in *handleWebSocketThread\n \n \n ");
    int clientSocket = *(int *)arg;
    pthread_t currentPlayerNumber;
    
    // Atomically increment playerNumberCounter to get a unique player number
    printf("%ld\n is the playercounter",playerNumberCounter);
    
      if (playerNumberCounter < maxclients) 
    {
        client_sockets[playerNumberCounter] = clientSocket;
    } 
    currentPlayerNumber = __sync_add_and_fetch(&playerNumberCounter, 1);

    //int playerNumber = getpid();  // Use thread ID as player number

    handleWebSocket(clientSocket, playerNumberCounter);

    // Cleanup and exit the thread
    //close(clientSocket);
    
    for (int i = 0; i <playerNumberCounter ; i++) 
    {
        close(client_sockets[i]);
         printf("2) client socket %d getting closed",i);
    }
    free(arg);
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
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 2) == -1) {
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

    return 0;
}

