#pragma once



#define MWGUTIL_PRINTLOG(wFormat, ...) \
    MwgUtil::PrintLog(__FILEW__, __LINE__, __FUNCTIONW__, wFormat, ##__VA_ARGS__);



#define MWGUTIL_PRINTWSAGETLASTERROR()                                                              \
    auto err = WSAGetLastError();                                                                   \
    MwgUtil::PrintGetLastError(__FILEW__, __LINE__, __FUNCTIONW__, err);



#define MWGUTIL_PRINTGETLASTERROR()                                                                 \
    auto err = GetLastError();                                                                      \
    MwgUtil::PrintGetLastError(__FILEW__, __LINE__, __FUNCTIONW__, err);



class MwgUtil
{
public:
    static void PrintLog(std::wstring wFilePath, int nLineNum, std::wstring wFuncName, std::wstring wFormat, ...);
    static void PrintGetLastError(const wchar_t * wFilePath, int nLineNum, const wchar_t * wFuncName, int nErrCode);
private:
    static std::mutex _mtxPrint;
};

