#pragma once

#include <functional>
#include <vector>
#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"
#include "TerminalServicesSession.h"

class RdpInfo : public IComponentBase
{
public:
    static const std::wstring ExtensionName;

    RdpInfo();
    // IInitDoneBase
    bool ADDIN_API Init(void *pConnection);
    bool ADDIN_API setMemManager(void *mem);
    long ADDIN_API GetInfo();
    void ADDIN_API Done();
    // ILanguageExtenderBase
    bool ADDIN_API RegisterExtensionAs(WCHAR_T **wsExtensionName);
    long ADDIN_API GetNProps();
    long ADDIN_API FindProp(const WCHAR_T *wsPropName);
    const WCHAR_T *ADDIN_API GetPropName(long lPropNum, long lPropAlias);
    bool ADDIN_API GetPropVal(const long lPropNum, tVariant *pvarPropVal);
    bool ADDIN_API SetPropVal(const long lPropNum, tVariant *varPropVal);
    bool ADDIN_API IsPropReadable(const long lPropNum);
    bool ADDIN_API IsPropWritable(const long lPropNum);
    long ADDIN_API GetNMethods();
    long ADDIN_API FindMethod(const WCHAR_T *wsMethodName);
    const WCHAR_T *ADDIN_API GetMethodName(const long lMethodNum, const long lMethodAlias);
    long ADDIN_API GetNParams(const long lMethodNum);
    bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue);
    bool ADDIN_API HasRetVal(const long lMethodNum);
    bool ADDIN_API CallAsProc(const long lMethodNum, tVariant *paParams, const long lSizeArray);
    bool ADDIN_API CallAsFunc(const long lMethodNum, tVariant *pvarRetValue, tVariant *paParams, const long lSizeArray);
    // LocaleBase
    void ADDIN_API SetLocale(const WCHAR_T *loc);
private:
    enum { BASE_ERRNO = 7 };

    struct Property
    {
        std::wstring name;
        std::wstring nameRu;
        std::function<bool(tVariant *propVal)> getter;
        std::function<bool(tVariant *propVal)> setter;
    };

    const std::vector<Property> _properties = {
        { L"IsRemoteSession", L"УдаленнаяСессия", std::bind(&RdpInfo::getIsRemoteSession, this, std::placeholders::_1), { } },
        { L"RemoteAddress", L"УдаленныйАдрес", std::bind(&RdpInfo::getRemoteAddress, this, std::placeholders::_1), { } },
        { L"ClientAddress", L"АдресКлиента", std::bind(&RdpInfo::getClientAddress, this, std::placeholders::_1), { } }
    };

    IAddInDefBase *_connect;
    IMemoryManager *_memory;
    TerminalServicesSession _terminalSession;

    void *wstringToMemory(const std::wstring &str) const;
    void wstringToVariant(const std::wstring &str, tVariant *val) const;
    void addError(unsigned short wcode, const std::wstring &source, const std::wstring &descr, int ec) const;
    bool getIsRemoteSession(tVariant *propVal);
    bool getRemoteAddress(tVariant *propVal);
    bool getClientAddress(tVariant *propVal);
};
