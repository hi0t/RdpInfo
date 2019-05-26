#pragma once

#include <exception>
#include <string>

class Wexception : public std::exception
{
public:
    explicit Wexception(const std::wstring &msg);
    explicit Wexception(int ec, const std::wstring &msg);
    const char *what() const throw();
    const wchar_t *whatW() const throw();
    int code() const;
private:
    int _code;
    std::wstring _msg;
    std::string _msg8;
};
