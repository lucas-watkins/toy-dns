cmake_minimum_required(VERSION 3.31)
project(dns_impl)

set(CMAKE_CXX_STANDARD 20)

add_executable(dns_impl
        src/main.cpp
        src/DNSQuery.cpp
)
