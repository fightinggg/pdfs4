cmake_minimum_required(VERSION 3.25)
project(pdfs4)

set(CMAKE_CXX_STANDARD 23)



add_executable(pdfs4 main.cpp)

target_link_libraries(pdfs4 ws2_32)