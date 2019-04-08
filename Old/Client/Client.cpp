// Client.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "Client.h"
#include <afxsock.h>
#include <string>
#include <strstream>
#include <iostream>
#include <thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SERVER_DEF_PORT (UINT) 1234

// The one and only application object

CWinApp theApp;

using namespace std;

void convertStr2WStr(string& str, wstring& wstr) {
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	wstr.clear();
	wstr.resize(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
}

void inboxListener(UINT& inboxPort, bool&end) {
	CSocket inbox,sock;
	inbox.Create();
	CString thisAddr;
	inbox.GetSockNameEx(thisAddr, inboxPort);
	cout << "Inbox Port: " << inboxPort << endl;
	string nickname, message;
	int messageLen;
	char buffer[255];
	while (!end)
	{
		inbox.Listen();
		inbox.Accept(sock);

		//Get Nickname sernt from server
		sock.Receive(&messageLen, sizeof(int), 0);
		sock.Receive(&buffer, messageLen, 0);
		buffer[messageLen] = '\0';
		nickname = buffer;

		//Get Message sernt from server
		sock.Receive(&messageLen, sizeof(int), 0);
		sock.Receive(&buffer, messageLen, 0);
		buffer[messageLen] = '\0';	
		message = buffer;
		cout << "[CHAT] " << nickname << ": " << message << endl;
		sock.Close();
	}
}


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
			BOOL connectStatus = false;
			bool end = false;
			std::wstring sa;
			string tmpString;
			CString abc;
			int contactPort = 0;
			UINT inboxPort = 0;
			thread inboxer(inboxListener, ref(inboxPort), ref(end));
			//Make sure inbox port initialized
			while (inboxPort == 0) {}
			std::this_thread::sleep_for(50ms);
			char ch;
			while (!connectStatus )
			{
				cout << "Server's name: ";
				getline(cin,tmpString);

				//Convert string to wstring

				convertStr2WStr(tmpString, sa);
				connectStatus = Clnt.Connect(sa.c_str(), SERVER_DEF_PORT);
				//Notify that client is ready to receive
				ch = 'r';
				//Clnt.Send(&ch, 1, 0);
				Clnt.Receive(&contactPort, 4, 0);

				if (!connectStatus)
				{
					cout << "Fail to detect server" << endl;
				}
			}
			cout << "Server Detected" << endl;
			//Connected, try to set nickname
			Clnt.Close();
			Clnt.Create();
			Clnt.Connect(sa.c_str(),contactPort);
			ch = 'a';
			Clnt.Send(&ch, 1, 0);
			std::string strn;
			//Read username
			std::cout << "username: ";
			getline(std::cin,strn);
			//Send username to server
			int len = strn.length();
			Clnt.Send(&len, 4, 0);
			Clnt.Send(strn.data(), len, 0);
			Clnt.Send(&inboxPort, 4, 0);
			Clnt.Receive(&ch, 1, 0);
			if (ch == 'O')
			{
				printf("Succeed.\n");
				//Send message
				Clnt.Close();
				while (true)
				{
					std::cout << " - ";
					getline(std::cin, strn);
					len = strn.length();
					Clnt.Create();
					Clnt.Connect(sa.c_str(), 1234);
					Clnt.Receive(&contactPort,sizeof(int),0);
					//cout << "Got port: " << contactPort << endl;
					Clnt.Close();
					Clnt.Create();
					Clnt.Connect(sa.c_str(), contactPort);
					//Indicate action: chat
					ch = 'c';
					Clnt.Send(&ch, 1, 0);
					Clnt.Send(&len, sizeof(int), 0);
					Clnt.Send(strn.data(), len, 0);
					Clnt.Close();
				}
			}
			else
			{
				printf("FAILED.\n");
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
