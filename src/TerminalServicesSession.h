#pragma once

#include <string>
#include <mutex>

class TerminalServicesSession
{
public:
    TerminalServicesSession();
    std::wstring remoteAddress() const;
    static bool isRemoteSession();
    std::wstring clientAddress() const;
private:
    HANDLE _server;
    DWORD _sessionId;
    static HINSTANCE _instWinsta;
    static std::mutex _instWinstaLck;

    static HINSTANCE getInstWinsta();
};
