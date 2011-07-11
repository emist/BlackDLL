#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include <stdafx.h>
#include <iostream>
#include "python/Python.h"
#include <fstream>
#include <time.h>
#include <winsock.h>


using namespace std;

extern "C"
{



typedef HMODULE (WINAPI *pGetModuleHandle)(LPCTSTR);
typedef FARPROC (WINAPI *pGetProcAddress)(HMODULE,LPCSTR);
typedef struct
{
  pGetModuleHandle pfGetModuleHandle;
  pGetProcAddress pfGetProcAddress;
  TCHAR szModule[64];
  CHAR szFunctionName1[64];
  CHAR szFunctionName2[64];
  CHAR szFunctionName3[64];
  const char *code;
} pyfuncs;



using namespace std;


////////////////////////////////////////////////////////////////////////
// Constants

const int kBufferSize = 1024;
        

////////////////////////////////////////////////////////////////////////
// Prototypes

SOCKET SetUpListener(const char* pcAddress, int nPort);
SOCKET AcceptConnection(SOCKET ListeningSocket, sockaddr_in& sinRemote);
bool EchoIncomingPackets(SOCKET sd);


//// DoWinsock /////////////////////////////////////////////////////////
// The module's driver function -- we just call other functions and
// interpret their results.

int DoWinsock()
{
	char * pcAddress = "127.0.0.1";
	int nPort = 10000;
	// Begin listening for connections
    //cout << "Establishing the listener..." << endl;
    SOCKET ListeningSocket = SetUpListener(pcAddress, htons(nPort));
    if (ListeningSocket == INVALID_SOCKET) {
        //cout << endl << WSAGetLastErrorMessage("establish listener") << 
        //        endl;
        return 3;
    }

    // Spin forever handling clients
    while (1) {
        // Wait for a connection, and accepting it when one arrives.
        //cout << "Waiting for a connection..." << flush;
        sockaddr_in sinRemote;
        SOCKET sd = AcceptConnection(ListeningSocket, sinRemote);
        if (sd != INVALID_SOCKET) {
            //cout << "Accepted connection from " <<
             //       inet_ntoa(sinRemote.sin_addr) << ":" <<
              //      ntohs(sinRemote.sin_port) << "." << endl;
        }
        else {
            //cout << endl << WSAGetLastErrorMessage(
            //        "accept connection") << endl;
            return 3;
        }
        
        // Bounce packets from the client back to it.
        if (EchoIncomingPackets(sd)) {
            // Successfully bounced all connections back to client, so
            // close the connection down gracefully.
            //cout << "Shutting connection down..." << flush;
            
        }
        else {
            //cout << endl << WSAGetLastErrorMessage(
            //        "echo incoming packets") << endl;
            return 3;
        }
    }

#if defined(_MSC_VER)
    return 0;       // warning eater
#endif
}


//// SetUpListener /////////////////////////////////////////////////////
// Sets up a listener on the given interface and port, returning the
// listening socket if successful; if not, returns INVALID_SOCKET.

SOCKET SetUpListener(const char* pcAddress, int nPort)
{
    u_long nInterfaceAddr = inet_addr(pcAddress);
    if (nInterfaceAddr != INADDR_NONE) {
        SOCKET sd = socket(AF_INET, SOCK_STREAM, 0);
        if (sd != INVALID_SOCKET) {
            sockaddr_in sinInterface;
            sinInterface.sin_family = AF_INET;
            sinInterface.sin_addr.s_addr = nInterfaceAddr;
            sinInterface.sin_port = nPort;
            if (bind(sd, (sockaddr*)&sinInterface, 
                    sizeof(sockaddr_in)) != SOCKET_ERROR) {
                listen(sd, 1);
                return sd;
            }
        }
    }

    return INVALID_SOCKET;
}


//// AcceptConnection //////////////////////////////////////////////////
// Waits for a connection on the given socket.  When one comes in, we
// return a socket for it.  If an error occurs, we return 
// INVALID_SOCKET.

SOCKET AcceptConnection(SOCKET ListeningSocket, sockaddr_in& sinRemote)
{
    int nAddrSize = sizeof(sinRemote);
    return accept(ListeningSocket, (sockaddr*)&sinRemote, &nAddrSize);
}


//// EchoIncomingPackets ///////////////////////////////////////////////
// Bounces any incoming packets back to the client.  We return false
// on errors, or true if the client closed the socket normally.

bool EchoIncomingPackets(SOCKET sd)
{
    // Read data from client
    char acReadBuffer[kBufferSize];
    int nReadBytes;
    do {
        nReadBytes = recv(sd, acReadBuffer, kBufferSize, 0);
        if (nReadBytes > 0) {
            //cout << "Received " << nReadBytes << 
            //        " bytes from client." << endl;
        
            int nSentBytes = 0;
            while (nSentBytes < nReadBytes) {
                int nTemp = send(sd, acReadBuffer + nSentBytes,
                        nReadBytes - nSentBytes, 0);
                if (nTemp > 0) {
                    //cout << "Sent " << nTemp << 
                    //        " bytes back to client." << endl;
                    nSentBytes += nTemp;
                }
                else if (nTemp == SOCKET_ERROR) {
                    return false;
                }
                else {
                    // Client closed connection before we could reply to
                    // all the data it sent, so bomb out early.
                    //cout << "Peer unexpectedly dropped connection!" << 
                    //        endl;
                    return true;
                }
            }
        }
        else if (nReadBytes == SOCKET_ERROR) {
            return false;
        }
    } while (nReadBytes != 0);

    //cout << "Connection closed by peer." << endl;
    return true;
}




// Define the DLL's main function

	BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
	{
		// Get rid of compiler warnings since we do not use this parameter

		UNREFERENCED_PARAMETER(lpReserved);

		// If we are attaching to a process

		if(ulReason == DLL_PROCESS_ATTACH)
		{
			// Do not need the thread based attach/detach messages in this DLL

			DisableThreadLibraryCalls(hModule);
		}

		// Signal for Loading/Unloading
		
		
		

		return (TRUE);
	}

	__declspec(dllexport) void atLogin()
	{
		Py_Initialize();
		PyGILState_STATE gstate = PyGILState_Ensure();
		char buf[400];
		strcpy(buf, "login = uicore.layer.login\n"
		"if login.isopen:\n"
        "\tlogin.usernameEditCtrl.SetValue(loooool)\n"
        "\tlogin.passwordEditCtrl.SetValue(looooool)\n"
        "\tsm.ScatterEventWithoutTheStars = LoggedScatter\n"
        "\tlogin.Connect()");

		PyRun_SimpleString(buf);

		PyGILState_Release( gstate );

	}


	__declspec(dllexport) void waitLoaded()
	{
		Py_Initialize();
		char buf[300];
		PyObject *myMod = NULL;
		PyObject *var = NULL;

		PyGILState_STATE gstate = PyGILState_Ensure();

		PySys_SetPath("C:\\Users\\emist\\Documents;"
		"C:\\Windows\\system32\\python27.zip;"
		"C:\\Python27\\DLLs;"
		"C:\\Python27\\lib;"
		"C:\\Python27\\lib\\plat-win;"
		"C:\\Python27\\lib\\lib-tk;"
		"C:\\Python27;"
		"C:\\Python27\\lib\\site-packages;");
		
		myMod = PyImport_ImportModule("main");

		PyGILState_Release( gstate );

		if(myMod == NULL)
		{
			printf("Couldn't open main\n");
			return;
		}

		var = PyObject_GetAttrString(myMod, "uicore.layer.login");

		

		while(var == NULL)
		{
			Sleep(300);
			gstate = PyGILState_Ensure();
			printf("uicore.layer.login is NULL\n");
			var = PyObject_GetAttrString(myMod, "uicore.layer.login");
			PyGILState_Release(gstate);
		}
		printf("uicore.layer.login is not NULL");
		atLogin();

	}

	__declspec(dllexport) void startServer()
	{
		//Py_Initialize();
		PyGILState_STATE gstate = PyGILState_Ensure();
		
		
		PySys_SetPath("C:\\Users\\emist\\Documents;"
		"C:\\Windows\\system32\\python27.zip;"
		"C:\\Python27\\DLLs;"
		"C:\\Python27\\lib;"
		"C:\\Python27\\lib\\plat-win;"
		"C:\\Python27\\lib\\lib-tk;"
		"C:\\Python27;"
		"C:\\Python27\\lib\\site-packages;");
		
		
		char buf[1600];

		
		strcpy(buf, 
		"import socket\n"
		"import code\n"
		"import sys\n"

		"class MyConsole(code.InteractiveConsole):\n"
			"\tdef __init__(self, rfile, wfile, locals=None):\n"
				"\t\ttry:\n"
				"\t\t\tself.rfile = rfile\n"
				"\t\t\tself.wfile = wfile\n"
				"\t\t\tcode.InteractiveConsole.__init__(self, locals=locals, filename='<MyConsole>')\n"
				"\t\texcept:\n"
				"\t\t\tpass\n"

			"\tdef raw_input(self, prompt=''):\n"
				"\t\ttry:\n"
					"\t\t\tself.wfile.write(prompt)\n"
					"\t\t\treturn self.rfile.readline().rstrip()\n"
				"\t\texcept:\n"
					"\t\t\tpass\n"

			"\tdef write(self, data):\n"
				"\t\ttry:\n"
					"\t\t\tself.wfile.write(data)\n"
				"\t\texcept:\n"
					"\t\t\tpass\n"



		"def handleSocket():\n"
		"\ttry:\n"
			"\t\tnetloc = ('', 7777)\n"
			"\t\tservsock = socket.socket()\n"
			"\t\tservsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, True)\n"
			"\t\tservsock.bind(netloc)\n"
			"\t\tservsock.listen(5)\n"
			"\t\tsock, _ = servsock.accept()\n"

			"\t\trfile = sock.makefile('r', 0)\n"
			"\t\tsys.stdout = wfile = sock.makefile('w', 0)\n"

			"\t\tconsole = MyConsole(rfile, wfile)\n"
			"\t\tconsole.interact()\n"
		
		"\texcept:\n"
			"\t\tservsock.close()\n"
			"\t\tsys.stdout.close()\n"

		"while True:\n"
		"\thandleSocket()\n"
		);
	

		//"utrhead(console.interact())\n");
		

		//strcpy(buf, "exit()");

		/*
		strcpy(buf, 
		"import sys\n"
		"print sys.path\n"
		//"sys.path.insert(0, 'C:\Python27\')\n"
		"import socket\n"
		"serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)\n"
		"serversocket.bind(('127.0.0.1', 10000))\n"
		"serversocket.listen(5)\n"
		"while 1:\n"
			"\t(clientsocket, address) = serversocket.accept()\n"
			"\tct = client_thread(clientsocket)\n"
			"\tct.run()\n");
		
		*/
		/*
		strcpy(buf, "import sys\n"
					"sys.path.insert(0, 'C:\')\n"
					"print sys.path\n");

		*/
		PyRun_SimpleString(buf);

		PyGILState_Release( gstate );

	}

	__declspec(dllexport) void cServerThread()
	{
		// Start Winsock up
		WSAData wsaData;
		int nCode;
		if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
			//cerr << "WSAStartup() returned error code " << nCode << "." <<
			//		endl;
			return ;
		}

		// Call the main example routine.
		int retval = DoWinsock();

		// Shut Winsock back down and take off.
		WSACleanup();
		
	}

	__declspec(dllexport) HANDLE dropServer()
	{
		HANDLE threadHandler;
		DWORD threadId;
		threadHandler = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&cServerThread, NULL, 0, &threadId);
		return threadHandler;
	}


	__declspec(dllexport) void process_expression()
	{

		// Get a reference to the main module
		// and global dictionary
	//	main_module = PyImport_AddModule("__main__");
	//	global_dict = PyModule_GetDict(main_module);

		// Extract a reference to the function "func_name"
		// from the global dictionary


//		PyRun_SimpleString("print hello");

		Py_Initialize();


		PyGILState_STATE gstate = PyGILState_Ensure();

		
		
		PyRun_SimpleString("import sys\n");
		//PyRun_SimpleString("sys.path.append('C:\\Users\\emist\\My Documents')");
		PyRun_SimpleString("sys.path.insert(0, 'C:\')");

		PyRun_SimpleString("import byteplay\n");
		//PyImport_Import(PyString_FromString("byteplay.py'"));
		//PyRun_SimpleString("sys.exit(0)");
		PyRun_SimpleString("from byteplay import *");
		PyRun_SimpleString("from pprint import pprint");
		PyRun_SimpleString("c = Code.from_code(hello.func_code)");
		

		PyRun_SimpleString("c.code[5:5] = [(LOAD_GLOBAL, 'text')]");
		PyRun_SimpleString("c.code[6:6] = [(LOAD_CONST, 'blah')]");
		PyRun_SimpleString("c.code[7:7] = [(CALL_FUNCTION, 1)]");
		PyRun_SimpleString("c.code[8:8] = [(POP_TOP, None)]");
		PyRun_SimpleString("hello.func_code = c.to_code()");

		PyRun_SimpleString("print c.code");
		
		PyRun_SimpleString("print 'hello'");

		PyGILState_Release( gstate );


//		expression = PyDict_GetItemString(global_dict, func_name);

	
		// Make a call to the function referenced
		// by "expression"
//		PyObject * res = PyObject_CallObject(expression, NULL);

		

//		char buf[120];
//		strcpy(buf, PyString_AsString(res));

		

		//MessageBox(0, buf, NULL, 0);
	}



	int run(void * a)
	{
		PyRun_SimpleString( (char *)a);
		return 0;
	}

  __declspec(dllexport) void Initialize()
  {
		
	  //TCHAR s[100];

	  //sprintf(s, "%s", GetCurrentProcessId());
	  //int (*pPyRun_SimpleString)(void * buf);
	  //Py_AddPendingCall(pPyRun_SimpleString, ((void*)"print hello"));

	  ofstream out;
	  out.open("hello.txt");
	  out << GetCurrentProcessId() << endl;

	  process_expression();
	  
	  //Py_AddPendingCall(&run, "print 'hello'");	


  }

  
}