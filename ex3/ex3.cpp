#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

const int PORT = 8081;
const int MAX_BUFFER = 1024;

void receiveMessages(SOCKET serverSocket) {
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[MAX_BUFFER];

    while (true) {
        int bytesReceived = recvfrom(serverSocket, buffer, MAX_BUFFER, 0,
            (sockaddr*)&clientAddr, &clientAddrLen);

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));

            std::cout << "Message from " << clientIP << ": "
                << buffer << std::endl;
        }
    }
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "UDP Server listening on port " << PORT << std::endl;

    std::thread receiveThread(receiveMessages, serverSocket);
    receiveThread.detach();

    std::string message;
    sockaddr_in targetAddr;
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_port = htons(8082); // Порт клієнта
    inet_pton(AF_INET, "127.0.0.1", &targetAddr.sin_addr);

    while (true) {
        std::cout << "Enter message to send (or 'exit' to quit): ";
        std::getline(std::cin, message);

        if (message == "exit") break;

        sendto(serverSocket, message.c_str(), message.length(), 0,
            (sockaddr*)&targetAddr, sizeof(targetAddr));
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

