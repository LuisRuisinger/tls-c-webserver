cmake_minimum_required(VERSION 3.26)
project(webserver_c C)

set(CMAKE_C_STANDARD 23)

# Find required OpenSSL package
find_package(OpenSSL REQUIRED)

# Include directories for header files
include_directories(include)

# Add the source files to the executable
add_executable(
        webserver_c
        src/main.c
        src/tpool.c
        src/hashmaps/hashmap.c
        src/filemanager.c
        src/parsing/readhandler.c
        src/parsing/writehandler.c
        src/cache.c
        src/tls.c
        src/setup.c
        src/run.c
        src/requesthandler.c
        src/hashmaps/hashmap_mime.c
        src/parsing/fieldparser.c
)

# Link against OpenSSL
target_link_libraries(webserver_c OpenSSL::SSL OpenSSL::Crypto)
