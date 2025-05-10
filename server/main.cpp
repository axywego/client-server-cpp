#include "server.hpp"

int main(){
    try {
        Server server{};
        server.listening();
        server.exit();
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