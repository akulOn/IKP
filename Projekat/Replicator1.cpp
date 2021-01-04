// Replicator1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "..\Common\ReplicatorList.h"
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"
#define GUID_FORMAT "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX"
#define GUID_ARG(guid) guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]

bool InitializeWindowsSockets();
DWORD WINAPI handleSocket(LPVOID lpParam);
NODE_REPLICATOR* head;

int main()
{
    InitReplicatorList(&head);

    while (true)
    {
#pragma region connectRegion

        // Socket used for listening for new clients 
        SOCKET listenSocket = INVALID_SOCKET;
        // Socket used for communication with client
        SOCKET acceptedSocket[10];
        for (int i = 0; i < 10; i++)
        {
            acceptedSocket[i] = INVALID_SOCKET;
        }

        // variable used to store function return value
        int iResult;
        // Buffer used for storing incoming data
        //char recvbuf[DEFAULT_BUFLEN];

        if (InitializeWindowsSockets() == false)
        {
            // we won't log anything since it will be logged
            // by InitializeWindowsSockets() function
            return 1;
        }

        // Prepare address information structures
        addrinfo* resultingAddress = NULL;
        addrinfo hints;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;       // IPv4 address
        hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
        hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
        hints.ai_flags = AI_PASSIVE;     // 

        // Resolve the server address and port
        iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultingAddress);
        if (iResult != 0)
        {
            printf("getaddrinfo failed with error: %d\n", iResult);
            WSACleanup();
            return 1;
        }

        // Create a SOCKET for connecting to server
        listenSocket = socket(AF_INET,      // IPv4 address famly
            SOCK_STREAM,  // stream socket
            IPPROTO_TCP); // TCP

        if (listenSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            freeaddrinfo(resultingAddress);
            WSACleanup();
            return 1;
        }

        // Setup the TCP listening socket - bind port number and local address 
        // to socket
        iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            printf("bind failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(resultingAddress);
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        // Since we don't need resultingAddress any more, free it
        freeaddrinfo(resultingAddress);

        // Set listenSocket in listening mode
        iResult = listen(listenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR)
        {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

#pragma endregion connectRegion

        printf("Server initialized, waiting for clients.\n");
        int numberOfClients = 0;

        do
        {
            acceptedSocket[numberOfClients] = accept(listenSocket, NULL, NULL);

            if (acceptedSocket[numberOfClients] == INVALID_SOCKET)
            {
                printf("accept failed with error: %d\n", WSAGetLastError());
                closesocket(listenSocket);
                WSACleanup();
                return 1;
            }

            //POKRETANJE NITI ZA SVAKOG KLIJENTA(PROCES)

            DWORD funId;
            HANDLE handle;
            PROCESS processAdd;
            GUID Id;
            CoCreateGuid(&Id);
            processAdd = InitProcess(Id, acceptedSocket[numberOfClients]);

            handle = CreateThread(NULL, 0, &handleSocket, &processAdd, 0, &funId);
            numberOfClients++;
            CloseHandle(handle);
            numberOfClients--;

            //OVDE DODATI LOGIKU ZA SLANJE INFORMACIJE O REGISTROVANOM PROCESU NA DRUGI REPLIKATOR

        } while (1);
    }
}

bool InitializeWindowsSockets()
{
    WSADATA wsaData;
    // Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
    return true;
}

DWORD WINAPI handleSocket(LPVOID lpParam)
{
    PROCESS* process = (PROCESS*)lpParam;
    SOCKET acceptedSocket = process->acceptedSocket;
    GUID Id = process->processId;
    int iResult;
    char recvbuf[512];

    //unsigned long mode = 1; 
    //non-blocking mode
    //iResult = ioctlsocket(acceptedSocket, FIONBIO, &mode);
    //if (iResult != NO_ERROR)
    //    printf("ioctlsocket failed with error: %ld\n", iResult);

    do
    {
        // Receive data until the client shuts down the connection
        iResult = recv(acceptedSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult > 0)
        {
            if (recvbuf[0] == 1)    //PUSH PROCESS
            {
                if (PushBack(&head, *process))
                {
                    printf("New process added! ID: {" GUID_FORMAT "}\n", GUID_ARG(process->processId));
                    strcpy(recvbuf, "1");
                }
                else
                {
                    printf("This process is registered already.\n");
                    strcpy(recvbuf, "0");
                }

                iResult = send(acceptedSocket, recvbuf, strlen(recvbuf) + 1, 0);

                if (iResult == SOCKET_ERROR)
                {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(acceptedSocket);
                    WSACleanup();
                    return 1;
                }

                PrintAllProcesses(&head);
            }
            else if (recvbuf[0] == 2)   //PUSH DATA
            {
                
            }
        }
        else if (iResult == 0)
        {
            // connection was closed gracefully
            printf("Connection with process(ID: {" GUID_FORMAT "}) closed.\n", GUID_ARG(process->processId));
            closesocket(acceptedSocket);
            break;
        }
        else
        {
            // there was an error during recv
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(acceptedSocket);
        }
    } while (true);

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
