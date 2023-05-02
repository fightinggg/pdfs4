
#pragma once

#include<exception>

class NotFoundError : public std::logic_error {
public:
    explicit NotFoundError(const std::string &s) : logic_error(s) {}
};