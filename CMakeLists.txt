cmake_minimum_required(VERSION 3.25.1)
project(pdfs4)

set(CMAKE_CXX_STANDARD 23)


add_executable(pdfs4 main.cpp)
target_link_libraries(pdfs4 pthread)

#find_package(CURL REQUIRED)
#target_link_libraries(pdfs4 curl)
#target_link_libraries(pdfs4 PRIVATE cpprestsdk::cpprest)

# for windows
target_link_libraries(pdfs4 ws2_32)

# for macos
# brew install cpprestsdk
# brew install curl