#include "stdafx.h"

#include <locale.h>
#include "RdpInfo.h"
#include "Wexception.h"

long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
    if (!*pInterface) {
        *pInterface= new RdpInfo;
        return (long)*pInterface;
    }
    return 0;
}
long DestroyObject(IComponentBase** pIntf)
{
    if (!*pIntf) {
        return -1;
    }

    delete *pIntf;
    *pIntf = 0;
    return 0;
}
const WCHAR_T* GetClassNames()
{
    return RdpInfo::ExtensionName.c_str();
}

const std::wstring RdpInfo::ExtensionName = L"RdpInfo";

RdpInfo::RdpInfo() :
    _connect(nullptr),
    _memory(nullptr)
{
}

bool RdpInfo::Init(void* pConnection)
{
    _connect = static_cast<IAddInDefBase*>(pConnection);
    return _connect != nullptr;
}

bool RdpInfo::setMemManager(void* mem)
{
    _memory = static_cast<IMemoryManager*>(mem);
    return _memory != nullptr;
}

long RdpInfo::GetInfo()
{
    return 2000;
}

void RdpInfo::Done()
{
}

bool RdpInfo::RegisterExtensionAs(WCHAR_T **wsExtensionName)
{
    *wsExtensionName = static_cast<WCHAR_T *>(wstringToMemory(ExtensionName));

    return *wsExtensionName != nullptr;
}

long RdpInfo::GetNProps()
{
    return _propNames.size();
}

long RdpInfo::FindProp(const WCHAR_T *wsPropName)
{
    auto it = std::find(_propNames.begin(), _propNames.end(), wsPropName);
    if (it != _propNames.end()) {
        return std::distance(_propNames.begin(), it);
    }

    it = std::find(_propNamesRu.begin(), _propNamesRu.end(), wsPropName);
    if (it != _propNamesRu.end()) {
        return std::distance(_propNamesRu.begin(), it);
    }

    return -1;
}

const WCHAR_T* RdpInfo::GetPropName(long lPropNum, long lPropAlias)
{
    if (lPropNum >= static_cast<long>(_propNames.size()))
        return nullptr;

    std::wstring propName;

    switch (lPropAlias) {
    case 0: // First language
        propName = _propNames[lPropNum];
        break;
    case 1: // Second language
        propName = _propNamesRu[lPropNum];
        break;
    default:
        return nullptr;
    }

    return static_cast<const WCHAR_T *>(wstringToMemory(propName));
}

bool RdpInfo::GetPropVal(const long lPropNum, tVariant *pvarPropVal)
{
    switch (lPropNum)
    {
    case PropIsRemoteSession:
        TV_VT(pvarPropVal) = VTYPE_BOOL;
        TV_BOOL(pvarPropVal) = TerminalServicesSession::isRemoteSession();
        break;
    case PropRemoteAddress:
        try {
            wstringToVariant(_terminalSession.remoteAddress(), pvarPropVal);
        } catch (const Wexception &e) {
            addError(ADDIN_E_ATTENTION, ExtensionName, e.whatW(), e.code());
            return false;
        }
        break;
    default:
        return false;
    }

    return true;
}

bool RdpInfo::SetPropVal(const long lPropNum, tVariant *varPropVal)
{
    switch (lPropNum)
    {
    case PropIsRemoteSession:
    case PropRemoteAddress:
    default:
        return false;
    }

    return true;
}

bool RdpInfo::IsPropReadable(const long lPropNum)
{
    switch (lPropNum)
    {
    case PropIsRemoteSession:
    case PropRemoteAddress:
        return true;
    default:
        return false;
    }
}

bool RdpInfo::IsPropWritable(const long lPropNum)
{
    switch (lPropNum)
    {
    case PropIsRemoteSession:
    case PropRemoteAddress:
        return false;
    default:
        return false;
    }
}

long RdpInfo::GetNMethods()
{
    return 0;
}

long RdpInfo::FindMethod(const WCHAR_T *wsMethodName)
{
    return -1;
}

const WCHAR_T *RdpInfo::GetMethodName(const long lMethodNum, const long lMethodAlias)
{
    return nullptr;
}

long RdpInfo::GetNParams(const long lMethodNum)
{
    return 0;
}

bool RdpInfo::GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue)
{
    return false;
}

bool RdpInfo::HasRetVal(const long lMethodNum)
{
    return false;
}

bool RdpInfo::CallAsProc(const long lMethodNum, tVariant *paParams, const long lSizeArray)
{
    return false;
}

bool RdpInfo::CallAsFunc(const long lMethodNum, tVariant *pvarRetValue, tVariant *paParams, const long lSizeArray)
{
    return false;
}

void RdpInfo::SetLocale(const WCHAR_T *loc)
{
    _wsetlocale(LC_ALL, loc);
}

void *RdpInfo::wstringToMemory(const std::wstring &str) const
{
    void *strVal;
    size_t len = (str.length() + 1) * sizeof(wchar_t);

    if (_memory == nullptr) {
        return nullptr;
    }

	_memory->AllocMemory(&strVal, len);
    memcpy(strVal, str.c_str(), len);
    return strVal;
}

void RdpInfo::wstringToVariant(const std::wstring &str, tVariant *val) const
{
	TV_VT(val) = VTYPE_PWSTR;
	val->pstrVal = static_cast<char *>(wstringToMemory(str));
	val->strLen = str.length();
}

void RdpInfo::addError(unsigned short wcode, const std::wstring &source, const std::wstring &descr, int ec) const
{
    if (_connect) {
        _connect->AddError(wcode, source.c_str(), descr.c_str(), RESULT_FROM_ERRNO(ec));
    }
}
