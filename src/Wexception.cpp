#include "stdafx.h"

#include "Wexception.h"
#include <codecvt>

Wexception::Wexception(const std::wstring &msg) :
    _code(0),
    _msg(msg)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    _msg8 = converter.to_bytes(msg);
}

Wexception::Wexception(int ec, const std::wstring &msg) :
    _code(ec)
{
    LPWSTR errorText = NULL;

    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        ec,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&errorText,
        0, NULL);

    std::wstring fmtMsg = msg + L" : " + std::wstring(errorText);
    LocalFree(errorText);

    _msg = fmtMsg;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    _msg8 = converter.to_bytes(fmtMsg);
}

const char *Wexception::what() const
{
    return _msg8.c_str();
}

const wchar_t *Wexception::whatW() const
{
    return _msg.c_str();
}

int Wexception::code() const
{
    return _code;
}
