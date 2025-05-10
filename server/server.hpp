#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

#include <chrono>

class Server {
private:
    int server_fd{};
public:
    Server() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) throw std::runtime_error("Cannot to create socket!");

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8080);
        addr.sin_addr.s_addr = INADDR_ANY;

        if(bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0){
            close(server_fd);
            throw std::runtime_error("Failed to bind!");
        }

        if(listen(server_fd, 5) < 0){
            close(server_fd);
            throw std::runtime_error("Failed to listen!");
        }

        std::cout << "Server listening on 0.0.0.0:8080\n";
    }

    void listening() {
        
        while(true){
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            auto client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);

            
            if(client_fd < 0) throw std::runtime_error("Failed to connection!");

            auto req = get_request(client_fd);

            auto start = std::chrono::system_clock::now();
            
            auto res = request_processing(req);
            send(client_fd, res.c_str(), res.length(), 0);
            
            
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_time = end - start;

            std::cout << "Client: " << inet_ntoa(client_addr.sin_addr) << '\n';
            std::cout << "Time to send answer: " << elapsed_time.count() * 1000 << "ms\n\n";

            close(client_fd);
        }
    }

    std::string get_request(const int& cliend_fd) {
        char buff[1024];
        auto bytes_read = recv(cliend_fd, buff, sizeof(buff) - 1, 0);
        if(bytes_read <= 0)
            throw std::runtime_error("Failed to read req!"); 
        return std::string(buff, bytes_read);
    }

    std::string request_processing(const std::string& req){
        std::cout << "received req: " << req << '\n';

        nlohmann::json data = nlohmann::json::parse(req);
        std::string login = data["login"];
        std::string res;
        res = std::format("Hi, {}!", login);
        return res;

    }

    void exit(){
        close(server_fd);
    }
};

#endif SERVER_HPP