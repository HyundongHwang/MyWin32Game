#pragma once
class MwgUtil
{
public:
    static void LastErrMsgBox(std::string msg)
    {
        wchar_t lpMsgBuf[1024] = { 0, };
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), lpMsgBuf, 0, NULL);
        //MessageBox(NULL, lpMsgBuf);
    }
};

