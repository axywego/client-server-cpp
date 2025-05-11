#include "server.hpp"

std::atomic<bool> shutdown_requested{false};

void signal_handler(int) {
    if (!shutdown_requested.exchange(true)) {
        std::cout << "\nShutting down server...\n";
    }
}

int main(){
    try {
        struct sigaction sa{};
        sa.sa_handler = signal_handler;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGINT, &sa, nullptr);
        sigaction(SIGTERM, &sa, nullptr);

        Server server{};

        std::thread server_thread([&server](){
            server.listening();
        });

        while (!shutdown_requested.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        server.stop();
        if (server_thread.joinable()) server_thread.join();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown error!" << std::endl;
        return 2;
    }
}