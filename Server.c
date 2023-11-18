#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>// for winsock library or functionalitis
#include <ws2tcpip.h>// extra or more winsock functionalities
#include <process.h>// for multithreading
#include <time.h>// para sa seed sakong bomba mausab2 kada execute 

#define MAX_CLIENTS 10
//kani an array to hold or store every sockets of connected clients
SOCKET connected_clients[MAX_CLIENTS];
//number of clients
int num_clients = 0;
//critical section kuan ni siya segment of code na i execute og dungan sa multiple threads since multi threading mani
// akoa code para daghan player. and also this accesses shared resources and this can only be accessed by one thread or 
//process at a time kani nag declare kog critical section named as client lockk pasabot kada naay new thread na mo enter
// sa critical section
//ang uban threads ang uban thread dpat mo huwat until ang kaning ge execute na thread mahuamn or mo exit
// ang purpose ani kay para ang daghan na threads maka process og dungan while wala nagka interfere sa usag usa.  

CRITICAL_SECTION client_lock;
//diri ako ge declare ang mga functions 
//kaning handle client kani na function mao ni gamiton para paghandle sa each client sa thread
void handle_client(void* param);

int main() {
    //kani ga create kos winsock
    WSADATA wsaData;
    //kani diri ako ge initialize or start ang winsock library bu using or calling the function WSAStartup,
    //  ang winsock or windows Socket API kay Data Link Layer na siya sa ato computer use to communicate windows netwrok Sofware and netweok Services
    //and nag exist na siya 
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return 1;
    }
//initialie anf critical section
    InitializeCriticalSection(&client_lock);
// diri ga create kog ssocket paras server nga ang family protofol is AF_net kay IPv4 man ako i use for protocol sa socket 
//programming nya sock stream kay kay this is associated with TCP or transmission Control Protol
//maong socket stream kay reliable ni for connection oriented communication channel
//pasabot ani mo establish sa og kuan connection sa client og server berfore maka send og data
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    //if fail ang pag creat sa socket the I will print in console failed to create socket and return 1
    if (server_socket == INVALID_SOCKET) {
        perror("Failed to create socket");
        WSACleanup();
        return 1;
    }
//set upping the server address structure for the family protocol, port and IP address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;
//kani ako ge bind ang server socket sa server_address which hold the protocol family, port, and Ip address
//if ma fail mo print aning error and exit
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        perror("Bind failed");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
//kani ako ge call ang listen function to set the server in listening mode pasabot ang server nag listen for incoming connetions
//if ma fail mo print error nya exit
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        perror("Listen failed");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
//if successful ang pag create sa server socket ang pag bind sa server socket sa server address and ang pag listen then ako ni i print
//ma display ni sa console sa server nga listening to port 8080 which is mao ang port na ako ge use
    printf("Server listening on port 8080...\n");
//so ang server naa na sa listening state then create kog infinite loop ron ang server mag sige listen for income connection
    while (1) {
        //diri create kog socket para sa client_socket, which hold the parameters server socket para ig accept 
        //mo bind sa katong associated nga Ip_address og port, then and 2nd na parameter kay optional para if gusto mo retrieve 
        //sa address sa client then ang third na paramete is para sa size sa address
        SOCKET client_socket = accept(server_socket, NULL, NULL);
        //if invalid and socket sa client then accept is failed nya exit.
        if (client_socket == INVALID_SOCKET) {
            perror("Accept failed");
            closesocket(server_socket);
            WSACleanup();
            return 1;
        }
//if naay ma accept na connection print ni

        printf("Accepted connection from client\n");

//so mao ni ang critical Section zone diri protected ang code ani nga segment
// ggetawag na nko and enterCriticalSection para maka acquire og lock sa critical section
//para ma ensure nga usa ra ka thread or process maka execute aning protected code which is mag update sa number of client
//and mo add sa new client kay if mag dungan2 ang tanan threads ang data is ma corrupted.
        EnterCriticalSection(&client_lock);
        
        // Add the client to the list
        connected_clients[num_clients++] = client_socket;
        //diri mo leave na or release and criticial section allowaing other threads to execute the code
        //so by using this critical section the connected clients is updated in a thread safe manner.
        LeaveCriticalSection(&client_lock);

        // diri ako ge call ang begin thread para mo create og new thread or process paras mga newly accepted client
        //ang first parameter is and handle client na function kay kani na function mao ni ang process na mo handle sa mga clients
        // ang third na parameter mao ni parameter na i pass sa handlie clinet which is ang client nga socket akong i pass kay mao ni 
        //parameter need sakong handle socket para maka handle og communication sa designated na client socket
        _beginthread(handle_client, 0, (void*)client_socket);
    }

    //so if disconnected na tanan clients or i shutdown na nako ang socket ako ni icall ang close Scoket
    //to close the serversocket. common practice ni to close the socket everytime i shutdown na or not needed
    closesocket(server_socket);
    //so kani ang WSA cleanup na function ang gamit ani is to remove the initialization sa WSA from the start tong nag initialize
    //kos WSAstart aron ma signalan ang operating system na akong application is mana og use sa winsock library.
    WSACleanup();
//diri delete or remove and critical section
    DeleteCriticalSection(&client_lock);

    return 0;
}


//mao ni akong handle client para mag handle sa each client sa akong server
void handle_client(void* param) {
    SOCKET client_socket = (SOCKET)param;
    //to cahnge the seed value for the srand function to genterer new random number every run
    srand(time(NULL));
    //for the bomb values
    //if 1 ang value then bomb na siya
    int arrayBomb[26];
        for (int i = 0; i < 25; i++) {
            int randomNumber = rand() % 5 + 1;
            if(randomNumber == 1){
                printf("bomb number index: %d", i+1);
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

    //send nako ang matrix sa client ron ma print digtos server side paras UI
    send(client_socket, matrix, strlen(matrix), 0);

    while ((bytes_received = recv(client_socket, data, sizeof(data), 0)) > 0) {
        data[bytes_received] = '\0';
        //nadawat sa client
        printf("nadawat sa from client: %s\n", data);
            //if data is equals to ""R" restart the game by sending the original matrix
            if(data[0] == 'r'){
                send(client_socket, matrix, strlen(matrix), 0);
            }else if(data[0] == 'q'){//if data is equals to q kick out the client from the server to end the game
                // enter critical section to update the list of connected clients after removing a clinet
                EnterCriticalSection(&client_lock);
                // Close the client socket if not receiving any message anymore
                closesocket(client_socket);

                // Remove the client from the connected client list
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

            }else{

        //gehimog interger ang data nadawat gekan sa user para i check sa mga index and if 26 ang value ani pasabot mo send og 26 digto
        //naay function digto sa client na maka stop and mo calculate pasabot iya na ge submit iya socre sa game while wala pa
        // ka click bumba
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
         }
        
       
    }
// enter critical section to update the list of connected clients after removing a clinet
    EnterCriticalSection(&client_lock);
    // Close the client socket if not receiving any message anymore
    closesocket(client_socket);

    // Remove the client from the connected client list
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


