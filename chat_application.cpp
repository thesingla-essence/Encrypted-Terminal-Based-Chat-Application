#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024
#define KEY 5 // Simple Caesar cipher key for demo encryption

std::mutex clients_mutex;
std::vector<SOCKET> clients;

// Simple encryption (Caesar cipher for demo; replace with AES for production)
std::string encrypt(const std::string& msg, int key) {
    std::string result = msg;
    for (char& c : result) {
        if (std::isalpha(c)) {
            char base = std::isupper(c) ? 'A' : 'a';
            c = base + (c - base + key) % 26;
        }
    }
    return result;
}

// Simple decryption
std::string decrypt(const std::string& msg, int key) {
    std::string result = msg;
    for (char& c : result) {
        if (std::isalpha(c)) {
            char base = std::isupper(c) ? 'A' : 'a';
            c = base + (c - base - key + 26) % 26;
        }
    }
    return result;
}

// Broadcast message to all clients except sender
void broadcast(const std::string& msg, SOCKET sender) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (SOCKET client : clients) {
        if (client != sender) {
            send(client, msg.c_str(), msg.length(), 0);
        }
    }
}

// Handle individual client
void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::lock_guard<std::mutex> lock(clients_mutex);
            auto it = std::find(clients.begin(), clients.end(), client_socket);
            if (it != clients.end()) {
                clients.erase(it);
            }
            closesocket(client_socket);
            std::cout << "Client disconnected\n";
            break;
        }
        buffer[bytes_received] = '\0';
        std::string encrypted_msg(buffer);
        std::string decrypted_msg = decrypt(encrypted_msg, KEY);
        std::cout << "Received: " << decrypted_msg << std::endl;

        std::string broadcast_msg = "Message: " + decrypted_msg + "\n";
        broadcast(broadcast_msg, client_socket);
    }
}

// Server function
void server() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(server_socket);
        WSACleanup();
        return;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(server_socket);
        WSACleanup();
        return;
    }
    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        SOCKET client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed\n";
            continue;
        }
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_socket);
        }
        std::cout << "New client connected\n";
        std::thread(handle_client, client_socket).detach();
    }

    closesocket(server_socket);
    WSACleanup();
}

// Client function
void client(const std::string& server_ip) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return;
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server_addr.sin_port = htons(PORT);

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed\n";
        closesocket(client_socket);
        WSACleanup();
        return;
    }

    std::cout << "Connected to server. Type 'exit' to quit.\n";

    // Thread to receive messages
    std::thread([client_socket]() {
        char buffer[BUFFER_SIZE];
        while (true) {
            int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (bytes_received <= 0) {
                std::cout << "Disconnected from server\n";
                break;
            }
            buffer[bytes_received] = '\0';
            std::cout << buffer;
        }
    }).detach();

    // Send messages
    std::string msg;
    while (true) {
        std::getline(std::cin, msg);
        if (msg == "exit") break;
        std::string encrypted_msg = encrypt(msg, KEY);
        send(client_socket, encrypted_msg.c_str(), encrypted_msg.length(), 0);
    }

    closesocket(client_socket);
    WSACleanup();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <server|client> [server_ip]\n";
        return 1;
    }

    std::string mode = argv[1];
    if (mode == "server") {
        server();
    } else if (mode == "client" && argc == 3) {
        client(argv[2]);
    } else {
        std::cerr << "Invalid mode or missing server IP\n";
        return 1;
    }

    return 0;
}