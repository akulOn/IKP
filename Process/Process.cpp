// Process.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <combaseapi.h>
#include "..\Common\ProcessList.h"
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27016

bool InitializeWindowsSockets();

int main()
{
    bool registered = false;
    NODE *head = NULL;
    DATA dataAdd;
    char data[100];

#pragma region connectRegion
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    // variable used to store function return value
    int iResult;
    // message to send
    //char* messageToSend = "";

    if (InitializeWindowsSockets() == false)
    {
        // we won't log anything since it will be logged
        // by InitializeWindowsSockets() function
        return 1;
    }

    // create a socket
    connectSocket = socket(AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(DEFAULT_PORT);
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
    }

    unsigned long mode = 1; //non-blocking mode
    iResult = ioctlsocket(connectSocket, FIONBIO, &mode);
    if (iResult != NO_ERROR)
        printf("ioctlsocket failed with error: %ld\n", iResult);

    fd_set readfds;
    FD_ZERO(&readfds);
#pragma endregion connectRegion

    while (true)
    {
        puts("MAIN MENU: ");
        puts("0. Exit.");
        puts("1. Register process.");
        puts("2. Send data.");

        int i;
        scanf_s("%d", &i);

        if (i == 0)
        {
            puts("Client is shuting down...");
            break;
        }
        if (i == 1)
        {
            if (!registered) 
            {
                // Send an prepared message with null terminator included
                iResult = send(connectSocket, (char*)&i, sizeof(i), 0);

                if (iResult == SOCKET_ERROR)
                {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(connectSocket);
                    WSACleanup();
                    return 1;
                }

                registered = true;
            }
            else 
            {
                printf("This process is registered already.\n");
            }   
        }
        else if (i == 2)
        {
            printf("This function is not implemented yet.\n");
        }
    }

    // cleanup
    closesocket(connectSocket);
    WSACleanup();

    return 0;
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
