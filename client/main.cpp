#include "client.hpp"
#include "user.hpp"

int main(){
    try {
        User user("valdem");
        std::string req = user.to_json();

        Client client("192.168.100.120");

        client.send_req(req);
        
        std::string res = client.get_response();
        
        std::cout << res;
        client.exit();
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