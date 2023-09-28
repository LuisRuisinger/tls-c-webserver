cmake_minimum_required(VERSION 3.26)
project(webserver_c C)

set(CMAKE_C_STANDARD 23)

add_executable(webserver_c main.c
        client.h
        tpool.c
        tpool.h)
