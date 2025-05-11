#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <nlohmann/json.hpp>

class User {
private:
    std::string _login;
public:
    User() : _login(""){ }
    
    User(const std::string& login) : _login(login) {}

    std::string to_json() {
        nlohmann::json res = {
            {"login", _login}
        };
        return res.dump();
    }
};

#endif