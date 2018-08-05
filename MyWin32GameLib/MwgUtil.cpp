#include "stdafx.h"
#include "MwgUtil.h"

std::mutex MwgUtil::_mtxPrint;

void MwgUtil::PrintLog(std::wstring wFilePath, int nLineNum, std::wstring wFuncName, std::wstring wFormat, ...)
{
    std::lock_guard<std::mutex> lg(_mtxPrint);

    auto now = std::chrono::system_clock::now();
    auto now_t = std::chrono::system_clock::to_time_t(now);
    tm now_tm_utc;
    _gmtime64_s(&now_tm_utc, &now_t);
    auto ms = (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000).count();
    wchar_t wTime[1024] = { 0, };

    std::wcsftime(
        wTime,
        1024,
        L"%m-%d %H:%M:%S",
        &now_tm_utc
    );

    auto pid = GetCurrentProcessId();
    auto tid = std::this_thread::get_id();

    wchar_t wLog[1024] = { 0, };
    va_list args;
    va_start(args, wFormat);
    vswprintf(wLog, 1024, wFormat.c_str(), args);
    va_end(args);
    auto res = GetConsoleWindow();

    wchar_t wPrettyLog[1024] = { 0, };

    if (wFilePath.empty())
    {
        std::swprintf(
            wPrettyLog,
            1024,
            L"%s.%d %d%d %d D %s \n",
            wTime,
            ms,
            pid,
            tid,
            wLog
        );
    }
    else
    {
        auto iStart = wFilePath.find_last_of(L'/');

        if (iStart == std::string::npos)
        {
            iStart = wFilePath.find_last_of(L'\\');
        }

        auto wFileName = wFilePath.substr(iStart + 1, wFilePath.size() - iStart - 1);

        std::swprintf(
            wPrettyLog,
            1024,
            L"%s.%d %d%d %d D %s:%d:%s %s \n",
            wTime,
            ms,
            pid,
            tid,
            wFileName.c_str(),
            nLineNum,
            wFuncName.c_str(),
            wLog
        );
    }

    OutputDebugString(wPrettyLog);

    if (res == nullptr)
    {
        MessageBox(nullptr, wPrettyLog, L"MwgUtil::PrintLog", MB_OK);
    }
    else
    {
        unsigned long uu = 0;
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), wPrettyLog, lstrlen(wPrettyLog), &uu, NULL);
    }
}

void MwgUtil::PrintGetLastError(const wchar_t * wFilePath, int nLineNum, const wchar_t * wFuncName, int nErrCode)
{
    void* lpMsgBuf = nullptr;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr,
        nErrCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (wchar_t*)&lpMsgBuf,
        0,
        nullptr);

    wchar_t wLog[1024] = { 0, };
    std::swprintf(wLog, 1024, L"GETLASTERROR[%d][%s]", nErrCode, (wchar_t*)lpMsgBuf);
    MwgUtil::PrintLog(wFilePath, nLineNum, wFuncName, wLog);
}