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
#include "objects.pb.h"
#include <iostream>


using namespace std;

	template<typename T>
	char * putToByteArray(T & eveobject, int & size)
	{
		char * output = new char[eveobject.ByteSize()];
		size = eveobject.ByteSize();
		eveobject.SerializeToArray(output, size);
		return output;

	}

	void elog(string message)
	{
		ofstream myfile;
		myfile.open("C:\\Users\\emist\\log.txt", fstream::app);
		myfile << message << endl;
		myfile.close();
	}

extern "C"
{


typedef struct _INIT_STRUCT {
	LPCWSTR Title;
	LPCWSTR Message;
} INIT_STRUCT, *PINIT_STRUCT;


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




	int atLogin()
	{
		int ret = 0;

		Py_Initialize();
		PyGILState_STATE gstate = PyGILState_Ensure();
		
		
		PyObject * main = PyImport_AddModule("__builtin__");
		if(main == NULL)
		{
			elog("Main failed to load");
			PyGILState_Release( gstate );
			return ret;
		}
		PyObject * maindic = PyModule_GetDict(main);
		
		if(maindic == NULL)
		{
			elog("Couldn't load main dictionary");
			PyGILState_Release( gstate );
			return ret;
		}

		PyObject * uicore = PyDict_GetItemString(maindic, "uicore");

		if(uicore == NULL)
		{
			elog("uicore is null");
			PyGILState_Release( gstate );
			return ret;
		}
		PyObject * layer = PyObject_GetAttrString(uicore, "layer");
		if(layer == NULL)
		{
			elog("layer is null");
			PyGILState_Release( gstate );
			return ret;
		}

		PyObject * login = PyObject_GetAttrString(layer, "login");
		if(login == NULL)
		{
			elog("login is null");
			PyGILState_Release( gstate );
			return ret;
		}

		//Py_DECREF(main);
		//Py_DECREF(uicore);
		//Py_DECREF(layer);

		PyObject * isopen = PyObject_GetAttrString(login, "isopen");
		if(isopen != NULL)
		{
			if(PyObject_IsTrue(isopen))
			{
				elog("Login is open");
				ret = 1;
			}
			else
			{
				elog("Login is false");
			}
		}
		else 
		{
			elog("isopen is null");
			PyGILState_Release( gstate );
			return ret;
		}

		PyGILState_Release( gstate );
		return ret;
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


	char * buildBooleanObject( bool value, int & size  )
	{
		eveobjects::BooleanObject eveobject;
		eveobject.set_istrue(value);
		char * output = putToByteArray(eveobject, size);
		return output;
	}


	__declspec(dllexport) void namedPipeServer()
	{
		

	   HANDLE npipe;
		
	   npipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\TestChannel"),
							   PIPE_ACCESS_DUPLEX,
							   PIPE_TYPE_MESSAGE | PIPE_WAIT,  
							   PIPE_UNLIMITED_INSTANCES ,
							   1024,
							   1024,
							   5000,
							   NULL);
	   if( npipe == NULL ){
		  //cerr<<"Error: cannot create named pipe\n";
		  return ;
	   }
	   #ifdef TRACE_ON
	   cerr<<"Named pipe created successfully"<<endl;
	   cerr<<"Waiting for the first client to connect ...\n";
	   #endif

	   char * output = NULL;

	   
	   while( ConnectNamedPipe(npipe, NULL) )
	   {
		  char  buf[1024];
		  char  * conf = "UNKNOWN FUNCTION";
		  int size = 0;
		  DWORD bread;
		  DWORD bsent;
		  while( ReadFile(npipe, (void*)buf, 1023, &bread, NULL) )
		  {
			 buf[bread] = 0; 
			 //cout<<"Received: '"<<buf<<"'"<<endl;

			 if(strcmp(buf, "atLogin") == 0)
			 {
				int ret = atLogin();
				if(ret == 1)
				{
					//elog("login=TRUE");
					output = buildBooleanObject(true, size);
				}
				else
				{
					output = buildBooleanObject(false, size);
				}
			 }

			 if( !WriteFile(npipe, (void*)output, size, &bsent, NULL) )
			 {
				//cerr<<"Error writing the named pipe\n";
			 }
		  }
		  DisconnectNamedPipe(npipe);
		  #ifdef TRACE_ON
		  cerr<<"Waiting for the next client to connect ...\n";
		  #endif
	   }

	   return ;
	
	}

	__declspec(dllexport) void dropServer()
	{
		HANDLE threadHandler;
		DWORD threadId;
		threadHandler = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&namedPipeServer, NULL, 0, &threadId);
		//return threadHandler;
	}


	__declspec(dllexport) void process_expression()
	{

		Py_Initialize();

		PyGILState_STATE gstate = PyGILState_Ensure();
		
		PyRun_SimpleString("import sys\n");

		PyRun_SimpleString("sys.path.insert(0, 'C:\')");

		PyRun_SimpleString("import byteplay\n");
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

	}



	int run(void * a)
	{
		PyRun_SimpleString( (char *)a);
		return 0;
	}

  __declspec(dllexport) void Initialize()
  {
		
	  ofstream out;
	  out.open("hello.txt");
	  out << GetCurrentProcessId() << endl;

	  process_expression();
  }

  
}