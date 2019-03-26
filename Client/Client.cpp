// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"
#include <afxsock.h>
#include <string>
#include <strstream>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: code your application's behavior here.
			CSocket Clnt;
			AfxSocketInit();
			Clnt.Create();
			std::wstring sa = L"127.0.0.1";
			Clnt.Connect(sa.c_str(), 1234);
			printf("Server Detected\n");
			char ch;
			Clnt.Receive(&ch, 1, 0);
			if (ch == 's') {
				printf("Connected.\n");
			}
			else
			{
				printf("Error.\n");
				getchar();
				return nRetCode;
			}
			//Connected, try to set nickname
			ch = 'r';
			Clnt.Send(&ch, 1, 0);
			std::string strn;
			//Read and send username to server
			std::cout << "username: ";
			getline(std::cin,strn);
			int len = strn.length();
			Clnt.Send(&len, 4, 0);
			Clnt.Send(strn.data(), len, 0);
			Clnt.Receive(&ch, 1, 0);
			if (ch == 'O')
			{
				printf("Succeed.\n");
			}
			else
			{
				printf("Can't.\n");
			}
			//Send message
			while (true)
			{
				std::cout << " - ";
				getline(std::cin, strn);
				Clnt.Send(&len, 4, 0);
				Clnt.Send(strn.data(), len, 0);
			}
			getchar();
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
