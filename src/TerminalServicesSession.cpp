#include "stdafx.h"

#include <ws2tcpip.h>
#include <wtsapi32.h>
#include "TerminalServicesSession.h"
#include "Wexception.h"

typedef enum _WINSTATIONINFOCLASS {
    WinStationRemoteAddress = 29
} WINSTATIONINFOCLASS;

typedef struct {
    unsigned short sin_family;
    union {
        struct {
            USHORT sin_port;
            ULONG in_addr;
            UCHAR sin_zero[8];
        } ipv4;
        struct {
            USHORT sin6_port;
            ULONG sin6_flowinfo;
            USHORT sin6_addr[8];
            ULONG sin6_scope_id;
        } ipv6;
    };
 } WINSTATIONREMOTEADDRESS, *PWINSTATIONREMOTEADDRESS;

typedef BOOLEAN (WINAPI * PWINSTATIONQUERYINFORMATIONW)(HANDLE, ULONG, WINSTATIONINFOCLASS, PVOID, ULONG, PULONG);

HINSTANCE TerminalServicesSession::_instWinsta = nullptr;
std::mutex TerminalServicesSession::_instWinstaLck;

TerminalServicesSession::TerminalServicesSession() :
    _server(WTS_CURRENT_SERVER_HANDLE),
    _sessionId(WTS_CURRENT_SESSION)
{
}

bool TerminalServicesSession::isRemoteSession()
{
    return GetSystemMetrics(SM_REMOTESESSION);
}

std::wstring TerminalServicesSession::remoteAddress() const
{
    PWINSTATIONQUERYINFORMATIONW WinStationQueryInformationW;
    WINSTATIONREMOTEADDRESS address;
    HINSTANCE instWinSta = getInstWinsta();
    ULONG ReturnLen;
    wchar_t str[INET6_ADDRSTRLEN];

    if (instWinSta == nullptr) {
        throw Wexception(L"winsta.dll not loaded");
    }

    WinStationQueryInformationW = (PWINSTATIONQUERYINFORMATIONW)GetProcAddress(instWinSta, "WinStationQueryInformationW");
    BOOL result = WinStationQueryInformationW(_server,
                                                _sessionId,
                                                WinStationRemoteAddress,
                                                &address,
                                                sizeof(WINSTATIONREMOTEADDRESS),
                                                &ReturnLen);

    if (!result) {
        throw Wexception(GetLastError(), L"query remote address");
    }

    switch (address.sin_family)
    {
    case AF_INET:
        in_addr ipv4;
        ipv4.S_un.S_addr = address.ipv4.in_addr;
        InetNtopW(address.sin_family, &ipv4, str, sizeof(str));
        break;
    case AF_INET6:
        in6_addr ipv6;
        memcpy(&ipv6, address.ipv6.sin6_addr, sizeof(ipv6));
        InetNtopW(address.sin_family, &ipv6, str, sizeof(str));
        break;
    default:
        throw Wexception(L"unknown address");
    }

    return str;
}

HINSTANCE TerminalServicesSession::getInstWinsta()
{
    std::lock_guard<std::mutex> lck(_instWinstaLck);

    if (_instWinsta == nullptr) {
        _instWinsta = LoadLibraryW(L"winsta.dll");
        if (_instWinsta == nullptr) {
            throw Wexception(GetLastError(), L"load winsta.dll");
        }
    }

    return _instWinsta;
}
