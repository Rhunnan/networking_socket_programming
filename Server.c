#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <time.h>

#define MAX_CLIENTS 10

SOCKET connected_clients[MAX_CLIENTS];
int num_clients = 0;
CRITICAL_SECTION client_lock;

void handle_client(void* param);
void broadcast(const char* message, SOCKET sender);

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return 1;
    }

    InitializeCriticalSection(&client_lock);

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        perror("Failed to create socket");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        perror("Bind failed");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {
        perror("Listen failed");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port 8080...\n");

    while (1) {
        SOCKET client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            perror("Accept failed");
            closesocket(server_socket);
            WSACleanup();
            return 1;
        }

        printf("Accepted connection from client\n");

        EnterCriticalSection(&client_lock);
        // Send the number of connected clients to the new client
        char num_clients_str[10];
        // sprintf(num_clients_str, "%d", num_clients);
        // send(client_socket, num_clients_str, strlen(num_clients_str) + 1, 0);  // +1 for the null terminator

        // Add the client to the list
        connected_clients[num_clients++] = client_socket;
        LeaveCriticalSection(&client_lock);

        // Create a new thread for each client
        _beginthread(handle_client, 0, (void*)client_socket);
    }

    closesocket(server_socket);
    WSACleanup();

    DeleteCriticalSection(&client_lock);

    return 0;
}

void handle_client(void* param) {
    SOCKET client_socket = (SOCKET)param;
    //to cahnge the seed value and the random number every run
    srand(time(NULL));
    //for the bomb values
    //if 1 ang value then bomb na siya
    int arrayBomb[26];
        for (int i = 0; i < 25; i++) {
            int randomNumber = rand() % 5 + 1;
            if(randomNumber == 1){
                printf("bomb number index: %d", i);
            }
            arrayBomb[i] = randomNumber;
    }
    //matrix for my UI
    char data[1024];
    int bytes_received;
    char matrix[26] = {
        '1', '2', '3','4', '5',
        '6', '7', '8','9', 'A',
        'B', 'C', 'D','E', 'F',
        'G', 'H', 'I','J', 'K',
        'L', 'M', 'N','O', 'P'
    };


    send(client_socket, matrix, strlen(matrix), 0);

    while ((bytes_received = recv(client_socket, data, sizeof(data), 0)) > 0) {
        data[bytes_received] = '\0';
        //nadawat sa client
        printf("nadawat sa from client: %s\n", data);
       
            int indexCheck = atoi(data)-1; 
            if(indexCheck == 25){
                char endingArray[2];
                sprintf(endingArray, "%d", indexCheck);
                send(client_socket, endingArray, strlen(endingArray), 0);
            }else{// casttype and value sa data para gamiton index checking sa bomb, minus 1 kay ang index sugod man 0
            //check if ang hatag sa client na value is naa bay bomba if naa minus iya kwarta
        //if wala butang counter ron ma add pila na kabuok iya ge click before siya mo submit ron ma double iya kwarta
        //nya sa UI if wala bomba ang na click kay ilisi og slash na symbol "-" if bomba ilisi asterisk nya minus iya kwarata "*"
            if(arrayBomb[indexCheck] == 1){
            matrix[indexCheck] = '*';
            send(client_socket, matrix, strlen(matrix), 0);
            send(client_socket, data, strlen(data) + 1, 0);  

            }else{
            matrix[indexCheck] = '/';
            send(client_socket, matrix, strlen(matrix), 0);
            send(client_socket, data, strlen(data) + 1, 0);  
            }
            }
    
        
        
       // +1 for the null terminator

        // Broadcast a message to all connected clients
        char broadcast_msg[1024];
        sprintf(broadcast_msg, "New client joined: %s", data);
        broadcast(broadcast_msg, client_socket);
    }

    EnterCriticalSection(&client_lock);
    // Close the client socket
    closesocket(client_socket);

    // Remove the client from the list
    for (int i = 0; i < num_clients; ++i) {
        if (connected_clients[i] == client_socket) {
            for (int j = i; j < num_clients - 1; ++j) {
                connected_clients[j] = connected_clients[j + 1];
            }
            --num_clients;
            break;
        }
    }
    LeaveCriticalSection(&client_lock);
}

void broadcast(const char* message, SOCKET sender) {
    EnterCriticalSection(&client_lock);
    for (int i = 0; i < num_clients; ++i) {
        if (connected_clients[i] != sender) {
            send(connected_clients[i], message, strlen(message) + 1, 0);  // +1 for the null terminator
        }
    }
    LeaveCriticalSection(&client_lock);
}
