#define _WIN32_WINNT 0x501 
#define DEFAULT_PORT "27016"
#define DEFAULT_BUFLEN 512

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <codecvt> // for std::codecvt_utf8
#include <locale>  // for std::wstring_convert

#pragma comment (lib, "Ws2_32.lib")



void processMessage(char* message, int size){
    /*
    printf("1?: %02X\n", message[0]);
    printf("2?: %02X\n", message[1]);
    printf("?3: %04X\n", (message[0]& 0xFF)<<8|(message[1]&0xFF));
    */
    
    for(int i = 0; i < size; i++){
        printf("Printing: %c\n", message[i]);
        KEYBDINPUT kb={0};
        INPUT input={0};
        kb.dwFlags = KEYEVENTF_UNICODE;
        input.type = INPUT_KEYBOARD;
        kb.wScan = message[i];
        input.ki = kb;
        SendInput(1, &input, sizeof(input));   
    }
}


int main(){    
    printf("Starting Utility\n");

    // Initialize winsock
    WSADATA wsData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsData);
    int flag = 1;

    if(iResult != 0){
        printf("Socket init failed: %d \n", iResult);
        return 1;
    }

    addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    printf("Getting Socket Access\n");
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

    if(iResult != 0){
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create socket
    printf("Creating Socket\n");
    SOCKET listenSocket = INVALID_SOCKET;
    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    // Activete heartbeat, might not be working
    if(setsockopt(listenSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&flag, sizeof(flag)) == SOCKET_ERROR){
        printf("Failed to set heartbeat TCP option %d", WSAGetLastError());
        closesocket(listenSocket);
        return 1;
    }

    if(listenSocket == INVALID_SOCKET){
        printf("Error at socket(): %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    
    // Bind port to socket
    iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if(iResult == SOCKET_ERROR){
        printf("Bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    
    freeaddrinfo(result);
    
    // Listen for client
    printf("Listening on Port %.*s\n", 5, DEFAULT_PORT);
    if (listen(listenSocket, SOMAXCONN ) == SOCKET_ERROR) {
        printf( "Listen failed with error: %d\n", WSAGetLastError() );
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Keep listening after client disconnection
    while(1){
        SOCKET clientSocket;
        clientSocket = INVALID_SOCKET;
        printf("Waiting Connection...\n");

        
        sockaddr_in saServer;
        int saSize = sizeof(saServer);
        clientSocket = accept(listenSocket, (sockaddr *)&saServer, &saSize);
        

        if (clientSocket == INVALID_SOCKET) {
            printf("accept failed: %c\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        printf("Conencted to %s\n", inet_ntoa(saServer.sin_addr));
        
        flag = 1;
        if(setsockopt(clientSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&flag, sizeof(flag)) == SOCKET_ERROR){
            printf("Failed to set heartbeat TCP option %d", WSAGetLastError());
            closesocket(clientSocket);
            closesocket(listenSocket);
            return 1;
        }

        char recvbuf[DEFAULT_BUFLEN];
        memset(recvbuf, 0, sizeof(*recvbuf) * DEFAULT_BUFLEN);
        int recvbuflen = DEFAULT_BUFLEN;

        // Receive until the peer shuts down the connection
        do {
            printf("%", recvbuf);
            iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0) {
                processMessage(recvbuf, iResult);

            } else if (iResult == 0)
                printf("Connection closing...\n");
            else {
                printf("Recieve failed: %d\n", WSAGetLastError());
                closesocket(clientSocket);
                WSACleanup();
                return 1;
        }

        } while (iResult > 0);

        iResult = shutdown(clientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        closesocket(clientSocket);
    }
    WSACleanup();



    return 0;
}