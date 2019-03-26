// Project1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"
#include <afxsock.h>
#include <conio.h>
#include <thread>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

struct User {
	CString _address;
	UINT _port;
	std::string nickname;
	CSocket soc;
};

bool checkNickName(std::string nickname, User* users) {
	for (int i = 0; i < 5; i++)
	{
		if (strcmp(nickname.data(),users[i].nickname.data()) == 0)
		{
			return false;
		}
	}
	return true;
}

void messageListener(User& user, bool * end) {
	int messageLen = 0;
	std::string message;
	char buffer[255];
	while (!(*end))
	{
		try {
			user.soc.Receive(&messageLen, 4, 0);
		}
		catch (exception e) {
			std::cout << e.what();
		}
		user.soc.Receive(&buffer, messageLen, 0);
		buffer[messageLen] = '\0';
		message = buffer;
		std::cout << "[Chat]" << user.nickname << ": " << message << endl;
	}
}

void initConnectResolver(int * connectStatus, User * Users, bool * end) {
	CSocket connectr;
	connectr.Create(1234);
	int stt = 0;
	printf(" - Connector started\n");
	char ch = 's';
	int currentUser = 0;
	//Loop to listen for connection request from client
	while (!(*end))
	{
		//Other listener is using (Multi-connector)
		//if (*connectStatus >= 0) {
		//	continue;
		//}
		while (stt == 0)
		{
			if (*end) {
				std::cout << " - Connector stopped\n";
				return;
			}
			stt = connectr.Listen();
		}
		//connect client to server
		connectr.Accept(Users[currentUser].soc);
		std::cout << "[Connector] Detect request" << endl;
		ch = 's';
		char buffer[255];
		string tempName;
		Users[currentUser].soc.Send(&ch, 1, 0);
		Users[currentUser].soc.Receive(&ch, 1, 0);
		if (ch == 'r') {
			//Read nickname from user
			int leng = 0;
			Users[currentUser].soc.Receive(&leng, 4, 0);
			Users[currentUser].soc.Receive(&buffer, leng, 0);
			buffer[leng] = '\0';
			tempName = buffer;
			//Check nickname
			if (checkNickName(tempName, Users)) {
				//Allow connect
				ch = 'O';
				Users[currentUser].nickname = tempName;
				Users[currentUser].soc.Send(&ch, 1, 0);
				Users[currentUser].soc.GetPeerName(Users[currentUser]._address, Users[currentUser]._port);
				std::cout << "[Connector] User " << currentUser << " connected as " << tempName
					<< " in " << &Users[currentUser].soc << endl;
				//std::thread th(messageListener, &(Users[currentUser]), end);

				//int messageLen = 0;
				//std::string message;
				//char buffer[255];
				//while (!(*end))
				//{
				//	try {
				//		Users[currentUser].soc.Receive(&messageLen, 4, 0);
				//	}
				//	catch (exception e) {
				//		std::cout << e.what();
				//	}
				//	Users[currentUser].soc.Receive(&buffer, messageLen, 0);
				//	buffer[messageLen] = '\0';
				//	message = buffer;
				//	std::cout << "[Chat]" << Users[currentUser].nickname << ": " << message << endl;
				//}

			}
			else
			{
				//Refuse connect
				ch = 'X';
				Users[currentUser].soc.Send(&ch, 1, 0);
				Users[currentUser].soc.Close();
			}
		}	
		
		stt = 0;
		currentUser++;
	}
	std::cout <<" - Connector stopped\n";
}

void terminateListen(bool * end) {
	char ch = ' ';
	while (ch != '\r')
	{
		ch = _getch();
	}
	*end = true;
	return;
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

			int a = -1;
			User * users = new User[5];
			bool end = false;
			AfxSocketInit();
			std::cout << "Listenning." << endl;
			std::thread connector(initConnectResolver, &a, users, &end);
			std::thread terminator(terminateListen,&end);
			//std::thread messager(messageListener, users, &end);
			char ch = ' ';
			//Message processor

			terminator.join();
			connector.join();
			//messager.join();
			std::cout << " DONE - - - -" << endl;
			delete[] users;
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
