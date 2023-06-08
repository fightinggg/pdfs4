cmake_minimum_required(VERSION 3.20)
project(pdfs4)

set(CMAKE_CXX_STANDARD 23)
#find_package(cpprestsdk REQUIRED)


add_executable(pdfs4 main.cpp)
target_link_libraries(pdfs4 PUBLIC pthread)
#target_link_libraries(pdfs4 PRIVATE cpprestsdk::cpprest)
#target_link_libraries(pdfs4 ws2_32)

# for macos
# brew install cpprestsdk