#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

class Client {
private:
    int sock_fd;
public:
    Client(const std::string& ip_address){
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0)
            throw std::runtime_error("Failed to create socket");

        sockaddr_in server_addr = {};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);
        if (inet_pton(AF_INET, ip_address.c_str(), &server_addr.sin_addr) <= 0) {
            close(sock_fd);
            throw std::runtime_error("Invalid server address");
        }

        if (connect(sock_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            close(sock_fd);
            throw std::runtime_error("Failed to connect");
        }
    }

    void send_req(const std::string& req){
        if(send(sock_fd, req.c_str(), req.length(), 0) < 0){
            close(sock_fd);
            throw std::runtime_error("Invalid to send request");
        }
    }

    std::string get_response(){
        char buffer[1024];
        int bytes_received = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0) 
            throw std::runtime_error("Invalid to send request");
        
        return std::string(buffer, bytes_received);
    }

    void exit(){
        close(sock_fd);
    }


};

#endif CLIENT_HPP