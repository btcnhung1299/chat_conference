
// ServerChat.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ServerChat.h"
#include "ServerChatDlg.h"
#include "Server.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// CServerChatApp

BEGIN_MESSAGE_MAP(CServerChatApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CServerChatApp construction

CServerChatApp::CServerChatApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CServerChatApp object

CServerChatApp theApp;


// CServerChatApp initialization

BOOL CServerChatApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Server - NHA Chat"));


	CServerChatDlg dlg;
	m_pMainWnd = &dlg;

	int a = -1;
	User * userCache = new User[UCACHE_LENGTH];
	vector<Group> groupCache;
	loadGroupCache(groupCache);

	bool end = false, coutBlocked = false;
	AfxSocketInit();
	//cout << "AFXSOCKET INITIALIZED." << endl;

	string messageToScreen;

	CSocket mainServer, mainClient;
	CSocket subServer[SUBPORT_LENGTH];
	int * portStatus = new int[SUBPORT_LENGTH] { PORT_STT_FREE };
	DlgLogger logger;
	logger.dlg = &dlg;
	logger.isBlocked = false;
	//thread connector(initConnectResolver, &a, userCache, &end);
	//Connect Resolver to respond free port
	thread connector(connectResolver, userCache, ref(messageToScreen), portStatus, ref(end), ref(logger));
	//2 sub port to handle user's request
	thread messager(receiveListener, 1235, ref(portStatus[0]), userCache, ref(groupCache), ref(end), ref(logger));
	thread messager2(receiveListener, 1236, ref(portStatus[1]), userCache, ref(groupCache), ref(end), ref(logger));
	//thread checker(userConnectionChecker, ref(userCache), ref(end), ref(logger));

	//messager.join();
	//cout << " DONE - - - -" << endl;



	INT_PTR nResponse = dlg.DoModal();
	end = true;
	connector.detach();
	messager.detach();
	messager2.detach();
	std::this_thread::sleep_for(100ms);
	delete[] userCache;
	saveGroupCache(groupCache);

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

