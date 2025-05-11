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
#include <csignal>
#include <atomic>
#include <thread>

#include <chrono>

class Server {
private:
    int server_fd{-1};

    

    std::atomic<bool> running{false};

    void handle_client(const int& client_fd, const sockaddr_in& client_addr){
        try {
            auto request = get_request(client_fd);
            auto response = request_processing(request);
            send_response(client_fd, response);
            std::cout << "Processed request from: " << inet_ntoa(client_addr.sin_addr) << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
        close(client_fd);
    }

public:
    Server() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) throw std::runtime_error("Cannot to create socket!");

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8080);
        addr.sin_addr.s_addr = INADDR_ANY;

        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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
        
        running.store(true);
        
        while(running.load()){
            fd_set read_fds;
            FD_ZERO(&read_fds);
            FD_SET(server_fd, &read_fds);
            
            timeval tv {.tv_sec = 1, .tv_usec = 0};
            
            auto ready = select(server_fd + 1, &read_fds, nullptr, nullptr, &tv);
            
            if (ready < 0) {
                if (errno == EINTR) {
                    // Прервано сигналом, проверяем флаг running
                    if (!running.load()) break;
                    continue;
                }
                throw std::runtime_error("Select error");
            }
            
            if (ready == 0) continue;

    
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);

            auto client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
            
            if (client_fd < 0) {
                if (errno == EINTR || !running.load()) {
                    // Либо прервано сигналом, либо сервер останавливается
                    break;
                }
                throw std::runtime_error("Failed to accept connection");
            }
        
            handle_client(client_fd, client_addr);
        }
    }

    std::string get_request(const int& cliend_fd) {
        char buff[2048];
        auto bytes_read = recv(cliend_fd, buff, sizeof(buff) - 1, 0);
        if(bytes_read <= 0)
            throw std::runtime_error("Failed to read req!"); 
        return std::string(buff, bytes_read);
    }

    std::string request_processing(const std::string& req){
        try {
            // std::cout << "received req: " << req << '\n';
            nlohmann::json data = nlohmann::json::parse(req);
            std::string login = data["login"];
            return std::format("Hi, {}!", login);
        }
        catch(const nlohmann::json::exception& e){
            return "Error: cannot to parse JSON";
        }
    }

    void send_response(const int& client_fd, const std::string& response) {
        size_t total_sent = 0;
        while (total_sent < response.length()) {
            ssize_t sent = send(client_fd, response.data() + total_sent, response.length() - total_sent, 0);
            if (sent <= 0) {
                throw std::runtime_error("Failed to send response");
            }
            total_sent += sent;
        }
    }

    void stop() {
        running.store(false);
        if (server_fd >= 0)
            shutdown(server_fd, SHUT_RDWR);
    }

    ~Server() {
        stop();
        if (server_fd >= 0)
            close(server_fd);
        std::cout << "Server stopped\n";
    }
};

#endif