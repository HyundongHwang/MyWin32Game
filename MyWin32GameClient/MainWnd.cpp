#include "stdafx.h"
#include "MainWnd.h"





MainWnd* MainWnd::spCurrent = NULL;

MainWnd::MainWnd()
{
    spCurrent = this;
    _LoadLibrary();
    AllocConsole();
}


MainWnd::~MainWnd()
{
    _UnloadLibrary();
}

bool MainWnd::_LoadLibrary()
{
    //m_hDll = ::LoadLibrary(L"EasyZmq.dll");
    //m_pFunc_EASYZMQ_INIT = (LP_EASYZMQ_INIT)GetProcAddress(m_hDll, "EASYZMQ_Init");
    //m_pFunc_EASYZMQ_REQUEST = (LP_EASYZMQ_REQUEST)GetProcAddress(m_hDll, "EASYZMQ_Request");

    return true;
}

void MainWnd::_UnloadLibrary()
{
    //if (m_hDll == NULL)
    //{
    //    ::FreeLibrary(m_hDll);
    //    m_hDll = NULL;
    //}
}

LRESULT MainWnd::_On_WM_CREATE(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    USES_CONVERSION;

    auto x = 0;
    auto y = 0;
    auto w = 500;
    auto h = 30;
    auto wId = 1000;

    //CWindow wndS0;
    //wndS0.Create(L"Static", this->m_hWnd, CRect(0, y, 100, y + h), L"MyPort : ", WS_CHILD | WS_VISIBLE, NULL, wId++);
    //m_edMyPort.Create(L"Edit", this->m_hWnd, CRect(100, y, 200, y + h), L"", WS_CHILD | WS_VISIBLE | WS_BORDER, NULL, wId++);
    //CWindow wndS1;
    //wndS1.Create(L"Static", this->m_hWnd, CRect(200, y, 3000, y + h), L"OtherPort : ", WS_CHILD | WS_VISIBLE, NULL, wId++);
    //m_edOtherPort.Create(L"Edit", this->m_hWnd, CRect(300, y, 400, y + h), L"", WS_CHILD | WS_VISIBLE | WS_BORDER, NULL, wId++);
    //y += h;

    _vLabelFuncSimple.push_back(CSTRING_FUNC_SIMPLE_PAIR(L"send hello", MainWnd::_OnClickBtn_SendHello));
    _vLabelFuncSimple.push_back(CSTRING_FUNC_SIMPLE_PAIR(L"Clear", MainWnd::_OnClickBtn_Clear));

    for (size_t i = 0; i < _vLabelFuncSimple.size(); i++)
    {
        auto pair = _vLabelFuncSimple[i];
        auto strBtnName = pair.strLabel;
        auto nBtnId = i;
        CWindow wndBtn;
        wndBtn.Create(L"Button", this->m_hWnd, (_U_RECT)CRect(x, y, x + w, y + h), strBtnName, WS_CHILD | WS_VISIBLE | BS_LEFT, NULL, nBtnId);
        y += h;
    }

    _edLog.Create(L"Edit", this->m_hWnd, (_U_RECT)CRect(0, 0, 0, 0), L"·Î±×", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN, NULL, wId++);
    return 0;
}

LRESULT MainWnd::_On_WM_DESTROY(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ::PostQuitMessage(0);
    return 0;
}

LRESULT MainWnd::_On_WM_SIZE(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CRect rcClient;
    ::GetClientRect(m_hWnd, &rcClient);
    _edLog.MoveWindow(0, rcClient.Height() / 2, rcClient.Width(), rcClient.Height() / 2);
    return 0;
}

void MainWnd::_WriteLog(LPCWSTR wLog)
{
    USES_CONVERSION;

    CString strEd;
    _edLog.GetWindowText(strEd);
    strEd += L"\r\n";
    strEd += wLog;
    _edLog.SetWindowText(strEd);
}

LRESULT MainWnd::_On_ID_BTN(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    auto nBtnId = wID;
    auto pair = _vLabelFuncSimple[nBtnId];
    pair.pFuncSimple(this);
    return 0;
}

void MainWnd::_OnClickBtn_Clear(MainWnd* pThis)
{
    pThis->_edLog.SetWindowText(L"");
}

//https://docs.microsoft.com/en-us/windows/desktop/winsock/complete-client-code
void MainWnd::_OnClickBtn_SendHello(MainWnd* pThis)
{
    const static std::string SERVER_HOST = "localhost";
    const static int SERVER_PORT = 27015;
    const static int BUF_LEN = 1024;

    WSADATA wsaData = { 0, };
    SOCKET connectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints = { 0, };
    const wchar_t* sendbuf = L"this is a test";
    wchar_t recvbuf[BUF_LEN] = { 0, };
    int iResult = 0;



    // Initialize Winsock
    MWGUTIL_PRINTLOG(L"WSAStartup ...");
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        MWGUTIL_PRINTLOG(L"WSAStartup failed with error");
        MWGUTIL_PRINTWSAGETLASTERROR();
        WSACleanup();
        return;
    }



    MWGUTIL_PRINTLOG(L"Resolve ...");
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(SERVER_HOST.c_str(), std::to_string(SERVER_PORT).c_str(), &hints, &result);

    if (iResult != 0) {
        MWGUTIL_PRINTLOG(L"getaddrinfo failed with error");
        MWGUTIL_PRINTWSAGETLASTERROR();
        WSACleanup();
        return;
    }



    // Attempt to connect to an address until one succeeds
    for (size_t i = 0; i < 10; i++)
    {
        // Create a SOCKET for connecting to server
        MWGUTIL_PRINTLOG(L"create socket ...");
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype,ptr->ai_protocol);

        if (connectSocket == INVALID_SOCKET) {
            MWGUTIL_PRINTLOG(L"socket failed with error");
            MWGUTIL_PRINTWSAGETLASTERROR();
            WSACleanup();
            return;
        }



        // Connect to server.
        MWGUTIL_PRINTLOG(L"Connect ...");
        iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

        if (iResult == SOCKET_ERROR) {
            MWGUTIL_PRINTLOG(L"Connect error retry ...");
            MWGUTIL_PRINTWSAGETLASTERROR();
            closesocket(connectSocket);
            connectSocket = INVALID_SOCKET;
            continue;
        }
    }



    if (connectSocket == INVALID_SOCKET) {
        MWGUTIL_PRINTLOG(L"socket failed with error");
        MWGUTIL_PRINTWSAGETLASTERROR();
        WSACleanup();
        return;
    }




    freeaddrinfo(result);

    if (connectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return;
    }

    // Send an initial buffer
    auto bufSize = (lstrlen(sendbuf) + 1) * sizeof(wchar_t);
    iResult = send(connectSocket, (char*)sendbuf, bufSize, 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(connectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(connectSocket, (char*)recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while (iResult > 0);

    // cleanup
    closesocket(connectSocket);
    WSACleanup();

}

void MainWnd::_ServerLoop(MainWnd * pThis)
{
}
