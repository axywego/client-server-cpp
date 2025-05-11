cmake_minimum_required(VERSION 3.14...3.27)
project(MyServerProject)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опция для сборки в режиме Debug/Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Загрузка nlohmann/json с GitHub
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# Серверный исполняемый файл
add_executable(server
    server/main.cpp
    server/server.hpp
)
target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/server
)

# Клиентский исполняемый файл
add_executable(client
    client/main.cpp
    client/client.hpp
    client/user.hpp
)
target_link_libraries(client PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/client
)

# Установка
install(TARGETS server RUNTIME DESTINATION bin)
install(TARGETS client RUNTIME DESTINATION bin)