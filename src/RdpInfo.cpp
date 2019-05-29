#include "stdafx.h"

#include <algorithm>
#include <locale>
#include "RdpInfo.h"
#include "Wexception.h"

long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
    if (!*pInterface) {
        *pInterface= new RdpInfo;
        return 1;
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
    return static_cast<long>(_properties.size());
}

long RdpInfo::FindProp(const WCHAR_T *wsPropName)
{
    auto it = std::find_if(_properties.begin(), _properties.end(),
        [wsPropName](const Property &x) { return x.name == wsPropName; }
    );
    if (it != _properties.end()) {
        return static_cast<long>(std::distance(_properties.begin(), it));
    }

    it = std::find_if(_properties.begin(), _properties.end(),
        [wsPropName](const Property &x) { return x.nameRu == wsPropName; }
    );
    if (it != _properties.end()) {
        return static_cast<long>(std::distance(_properties.begin(), it));
    }

    return -1;
}

const WCHAR_T* RdpInfo::GetPropName(long lPropNum, long lPropAlias)
{
    if (lPropNum >= static_cast<long>(_properties.size())) {
        return nullptr;
    }

    std::wstring propName;

    switch (lPropAlias) {
    case 0: // First language
        propName = _properties[lPropNum].name;
        break;
    case 1: // Second language
        propName = _properties[lPropNum].nameRu;
        break;
    default:
        return nullptr;
    }

    return static_cast<const WCHAR_T *>(wstringToMemory(propName));
}

bool RdpInfo::GetPropVal(const long lPropNum, tVariant *pvarPropVal)
{
    if (lPropNum >= static_cast<long>(_properties.size())) {
        return false;
    }
    return _properties[lPropNum].getter(pvarPropVal);
}

bool RdpInfo::SetPropVal(const long lPropNum, tVariant *varPropVal)
{
    if (lPropNum >= static_cast<long>(_properties.size())) {
        return false;
    }
    return _properties[lPropNum].setter(varPropVal);
}

bool RdpInfo::IsPropReadable(const long lPropNum)
{
    if (lPropNum >= static_cast<long>(_properties.size())) {
        return false;
    }
    return static_cast<bool>(_properties[lPropNum].getter);
}

bool RdpInfo::IsPropWritable(const long lPropNum)
{
    if (lPropNum >= static_cast<long>(_properties.size())) {
        return false;
    }
    return static_cast<bool>(_properties[lPropNum].setter);
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

	_memory->AllocMemory(&strVal, static_cast<unsigned long>(len));
    memcpy(strVal, str.c_str(), len);
    return strVal;
}

void RdpInfo::wstringToVariant(const std::wstring &str, tVariant *val) const
{
	TV_VT(val) = VTYPE_PWSTR;
	val->pstrVal = static_cast<char *>(wstringToMemory(str));
	val->strLen = static_cast<uint32_t>(str.length());
}

void RdpInfo::addError(unsigned short wcode, const std::wstring &source, const std::wstring &descr, int ec) const
{
    if (_connect) {
        _connect->AddError(wcode, source.c_str(), descr.c_str(), RESULT_FROM_ERRNO(ec));
    }
}

bool RdpInfo::getIsRemoteSession(tVariant *propVal)
{
    TV_VT(propVal) = VTYPE_BOOL;
    TV_BOOL(propVal) = TerminalServicesSession::isRemoteSession();
    return true;
}

bool RdpInfo::getRemoteAddress(tVariant *propVal)
{
    try {
        wstringToVariant(_terminalSession.remoteAddress(), propVal);
    } catch (const Wexception &) {
        TV_VT(propVal) = VTYPE_EMPTY;
    }
    return true;
}

bool RdpInfo::getClientAddress(tVariant *propVal)
{
    try {
        wstringToVariant(_terminalSession.clientAddress(), propVal);
    } catch (const Wexception &) {
        TV_VT(propVal) = VTYPE_EMPTY;
    }
    return true;
}
