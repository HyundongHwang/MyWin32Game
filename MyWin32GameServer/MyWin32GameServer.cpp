// MyWin32GameServer.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "MyWin32GameServer.h"

//https://docs.microsoft.com/en-us/windows/desktop/winsock/complete-server-code



void _ProcessClient(SOCKET clientSocket)
{
    const static int RECV_BUF_LEN = 1024;
    wchar_t recvbuf[RECV_BUF_LEN];

    while (true)
    {
        auto recvLen = recv(clientSocket, (char*)recvbuf, RECV_BUF_LEN, 0);

        if (recvLen > 0)
        {
            MWGUTIL_PRINTLOG(L"Bytes received: recvLen[%d] recvbuf[%s]", recvLen, recvbuf);

            // Echo the buffer back to the sender
            auto sendLen = send(clientSocket, (char*)recvbuf, recvLen, 0);

            if (sendLen == SOCKET_ERROR)
            {
                MWGUTIL_PRINTLOG(L"send failed with error");
                MWGUTIL_PRINTWSAGETLASTERROR();
                break;
            }

            MWGUTIL_PRINTLOG(L"Bytes sent: sendLen[%d] recvbuf[%s]", sendLen, recvbuf);
        }
        else if (recvLen == 0)
        {
            MWGUTIL_PRINTLOG(L"Connection closing...");
            break;
        }
        else
        {
            MWGUTIL_PRINTLOG(L"recv failed with error");
            MWGUTIL_PRINTWSAGETLASTERROR();
            break;
        }
    }

    shutdown(clientSocket, SD_SEND);
    closesocket(clientSocket);
}



int main()
{
    const static int LISTEN_PORT = 27015;
    WSADATA wsaData = { 0, };
    
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;
    struct addrinfo* pAiRes = NULL;
    struct addrinfo aiHints;
    int iResult = 0;



    // Initialize Winsock
    MWGUTIL_PRINTLOG(L"WSAStartup ...");
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        MWGUTIL_PRINTLOG(L"WSAStartup failed with error");
        MWGUTIL_PRINTWSAGETLASTERROR();
        return 1;
    }



    // Resolve the server address and port
    MWGUTIL_PRINTLOG(L"resolve ... LISTEN_PORT[%d]", LISTEN_PORT);
    ZeroMemory(&aiHints, sizeof(aiHints));
    aiHints.ai_family = AF_INET;
    aiHints.ai_socktype = SOCK_STREAM;
    aiHints.ai_protocol = IPPROTO_TCP;
    aiHints.ai_flags = AI_PASSIVE;
    iResult = getaddrinfo(NULL, std::to_string(LISTEN_PORT).c_str(), &aiHints, &pAiRes);

    if (iResult != 0) {
        MWGUTIL_PRINTLOG(L"getaddrinfo failed with error");
        MWGUTIL_PRINTWSAGETLASTERROR();
        WSACleanup();
        return 1;
    }



    // Create a SOCKET for connecting to server
    MWGUTIL_PRINTLOG(L"create socket ...");
    listenSocket = socket(pAiRes->ai_family, pAiRes->ai_socktype, pAiRes->ai_protocol);

    if (listenSocket == INVALID_SOCKET) {
        MWGUTIL_PRINTLOG(L"socket failed with error");
        MWGUTIL_PRINTWSAGETLASTERROR();
        freeaddrinfo(pAiRes);
        WSACleanup();
        return 1;
    }



    // Setup the TCP listening socket
    MWGUTIL_PRINTLOG(L"bind ...");
    iResult = bind(listenSocket, pAiRes->ai_addr, (int)pAiRes->ai_addrlen);

    if (iResult == SOCKET_ERROR) {
        MWGUTIL_PRINTLOG(L"bind failed with error");
        MWGUTIL_PRINTWSAGETLASTERROR();
        freeaddrinfo(pAiRes);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(pAiRes);



    MWGUTIL_PRINTLOG(L"listen ...");
    iResult = listen(listenSocket, SOMAXCONN);

    if (iResult == SOCKET_ERROR) {
        MWGUTIL_PRINTLOG(L"listen failed with error");
        MWGUTIL_PRINTWSAGETLASTERROR();
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }



    while (true)
    {
        // Accept a client socket
        MWGUTIL_PRINTLOG(L"accept ...");
        clientSocket = accept(listenSocket, NULL, NULL);

        if (clientSocket == INVALID_SOCKET) {
            MWGUTIL_PRINTLOG(L"accept failed with error");
            MWGUTIL_PRINTWSAGETLASTERROR();
            break;
        }

        std::thread t(_ProcessClient, clientSocket);
        t.detach();
    }



    // No longer need server socket
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}