//#include <stdafx.h>
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <winsock.h>
#include <iostream>
#include "Login.h"
#include "Interfaces.h"
#include <iostream>
#include <sstream>
#include "objects.pb.h"

using namespace std;




	extern "C"
	{

	LPWSTR name = NULL;
		

	typedef struct _INIT_STRUCT {
		LPCWSTR Title;
		LPCWSTR cap;
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


		__declspec(dllexport) void startServer()
		{

			PyGILState_STATE gstate = PyGILState_Ensure();
			
			
			PySys_SetPath("C:\\Windows\\system32\\python27.zip;"
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
			"import threading\n"

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

			"def listen():\n"
			"\twhile True:\n"
			"\t\thandleSocket()\n"
			
			"threading.Thread(group=None, target=listen, name=None, args={}, kwargs={}).start()\n"

			);
		

			PyRun_SimpleString(buf);
			PyGILState_Release( gstate );


		}

		extern "C" __declspec(dllexport) void namedPipeServer()
		{
		   
		   	
//		   Login login;
			
		   
		   
		   Interfaces interfaces;
		   Logger log;


		   if(interfaces.Internal_getVersion() != "")
		   {  
			   if(interfaces.Internal_getVersion().compare("Version: 7.10.281738 (273475)") != 0)
			   {
				   log.elog("Didn't match version");
				   MessageBox(NULL, L"ERYAN IS OUTDATED", L"OUTDATED", 0);
				   return;
			   }
		   }
		   else
		   {
			   MessageBox(NULL, L"NO VERSION INFO AVAILABLE", L"ERROR", 0);
			   log.elog("No version available");
			   return;
		   }

		   HANDLE npipe;
		   log.elog("Inside namedpipe");
		   log.elog(name);
		   

		   npipe = CreateNamedPipe(name,
								   PIPE_ACCESS_DUPLEX,
								   PIPE_TYPE_MESSAGE | PIPE_WAIT,  
								   PIPE_UNLIMITED_INSTANCES ,
								   20024,
								   20024,
								   5000,
								   NULL);
		   if( npipe == NULL ){
			  log.elog("pipe creation error");
			  return ;
		   }
		   #ifdef TRACE_ON
		   cerr<<"Named pipe created successfully"<<endl;
		   cerr<<"Waiting for the first client to connect ...\n";
		   #endif


		   
		   while( ConnectNamedPipe(npipe, NULL) )
		   {
			  char * output = NULL;
			  char  buf[20024];
			  char  * conf = "UNKNOWN FUNCTION";
			  int size = 0;
			  DWORD bread;
			  DWORD bsent;
			  while( ReadFile(npipe, (void*)buf, 20023, &bread, NULL) )
			  {
				 char * output = NULL;
				 buf[bread] = 0; 
				 //cout<<"Received: '"<<buf<<"'"<<endl;

				 eveobjects::functionCall func;
				 func.ParseFromArray(buf, bread);
				 
				 
				 if(func.name().compare("atLogin") == 0)
				 {
					 output = interfaces.atLogin(size);
				 }
				 if(func.name().compare("findByNameLogin") == 0)
				 {
					 log.elog(func.strparameter());
					 output = interfaces.findByNameLogin(func.strparameter(), size);
				 }

				 if(func.name().compare("findByTextLogin") == 0)
				 {
					 log.elog(func.strparameter());
					 output = interfaces.findByTextLogin(func.strparameter(), size);
				 }
					
				 if(func.name().compare("findByTextMenu") == 0)
				 {
					 log.elog(func.strparameter());
					 output = interfaces.findByTextMenu(func.strparameter(), size);
				 }
				 
				 if(func.name().compare("getInflightInterface") == 0)
				 {
					 log.elog(func.strparameter());
					 output = interfaces.getInflightInterface(size);
				 }

				 if(func.name().compare("isMenuOpen") == 0)
				 {
					 log.elog(func.name());
					 output = interfaces.isMenuOpen(size);
				 }

				 
				 if(func.name().compare("getOverViewItems") == 0)
				 {
					 log.elog(func.name());
					 output = interfaces.OverViewGetMembers(size);
				 }

				 if(func.name().compare("getSelectedItem") == 0)
				 {
					 log.elog(func.name());
					 output = interfaces.GetSelectedItem(size);
				 }

				 if(func.name().compare("getTargetList") == 0)
				 {
					 log.elog(func.name());
					 output = interfaces.GetTargetList(size);
				 }

				 if(func.name().compare("getHighSlot") == 0)
				 {
					 log.elog(func.name());
					 log.elog(func.strparameter());
					 output = interfaces.GetHighSlot(atoi(func.strparameter().c_str()), size);
				 }

				 if(func.name().compare("isHighSlotActive") == 0)
				 {
					 log.elog(func.name());
					 log.elog(func.strparameter());
					 output = interfaces.IsHighSlotActive(atoi(func.strparameter().c_str()), size);
				 }

				 if(func.name().compare("getCargoList") == 0)
				 {
					 log.elog(func.name());
					 output = interfaces.GetCargoList(size);
				 }

				if(func.name().compare("getUndockButton") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetUndockButton(size);
				}

				if(func.name().compare("getShipHangar") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetShipHangar(size);
				}

				if(func.name().compare("getStationHangar") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetStationHangar(size);
				}

				if(func.name().compare("getItemsButton") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetStationItemsButton(size);
				}

				if(func.name().compare("getShipArmor") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetShipArmor(size);
				}
				
				if(func.name().compare("getShipShield") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetShipShield(size);
				}
				
				if(func.name().compare("getShipStructure") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetShipStructure(size);
				}
			
				if(func.name().compare("getShipSpeed") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetShipSpeed(size);
				}

				if(func.name().compare("getShipCapacity") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetShipCapacity(size);
				}
			
				if(func.name().compare("getMenuItems") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetMenuItems(size);
				}

				if(func.name().compare("isSystemMenuOpen") == 0)
				{
					log.elog(func.name());
					output = interfaces.IsSystemMenuOpen(size);
				}
				
				if(func.name().compare("getModalCancelButton") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetModalCancelButton(size);
				}

				if(func.name().compare("getModalOkButton") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetModalOkButton(size);
				}

				if(func.name().compare("getSystemInformation") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetSystemInformation(size);
				}

				if(func.name().compare("isLoading") == 0)
				{
					log.elog(func.name());
					output = interfaces.IsLoading(size);
				}
				if(func.name().compare("getServerMessage") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetServerMessage(size);
				}
				if(func.name().compare("getInterfaceWindows") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetInterfaceWindows(size);
				}

				if(func.name().compare("getTargetingRange") == 0)
				{
					log.elog(func.name());
					log.elog(func.strparameter());
					output = interfaces.GetTargetingRange(atoi(func.strparameter().c_str()), size);
				}
				if(func.name().compare("isIncursion") == 0)
				{
					log.elog(func.name());
					output = interfaces.IsIncursion(size);
				}
				if(func.name().compare("dronesInSpace") == 0)
				{
					log.elog(func.name());
					output = interfaces.DronesInFlight(size);
				}
				if(func.name().compare("dronesInBay") == 0)
				{
					log.elog(func.name());
					output = interfaces.DronesInBay(size);
				}
				if(func.name().compare("checkLocal") == 0)
				{
					log.elog(func.name());
					output = interfaces.CheckLocal(size);
				}
				if(func.name().compare("checkDroneStatus") == 0)
				{
					log.elog(func.name());
					output = interfaces.CheckDroneStatus(size);
				}
				if(func.name().compare("getMiningAmount") == 0)
				{
					log.elog(func.name());
					log.elog(func.strparameter());
					output = interfaces.GetMiningAmount(atoi(func.strparameter().c_str()), size);
				}
				if(func.name().compare("getDuration") == 0)
				{
					log.elog(func.name());
					log.elog(func.strparameter());
					output = interfaces.GetDuration(atoi(func.strparameter().c_str()), size);
				}

				if(func.name().compare("overviewGetScrollBar") == 0)
				{
					log.elog(func.name());
					output = interfaces.OverviewGetScrollBar(size);
				}

				if(func.name().compare("getOverviewHeight") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetOverviewHeight(size);
				}

				if(func.name().compare("getOverviewBottom") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetOverviewBottom(size);
				}

				if(func.name().compare("getOverviewTop") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetOverviewTop(size);
				}

				if(func.name().compare("getVersion") == 0)
				{
					log.elog(func.name());
					output = interfaces.GetVersion(size);
				}

				log.elog(func.name());
				//Sleep(300);

				if(output == NULL)
				{	 
					log.elog("Output is null");
					output = "\0";
				}

				if( !WriteFile(npipe, (void*)output, size, &bsent, NULL) )
				{
					 
					char * perr =(char*) malloc(100*sizeof(char));
					log.elog("Couldn't write to client");
					sprintf(perr, "%x", GetLastError());
					log.elog(perr);
					free(perr);
					 //cerr<<"Error writing the named pipe\n";
				}
				else
				{
					if(output != NULL && strcmp(output, "\0") != 0)
						delete output;
				}
				 //NEEDS TO delete output
				 //log.elog("Returning successfully");
			  }
			  DisconnectNamedPipe(npipe);
			  #ifdef TRACE_ON
			  cerr<<"Waiting for the next client to connect ...\n";
			  #endif
		   }

		   
		   return ;
		   
		
		}




		extern "C" __declspec(dllexport) void dropServer( LPVOID message)
		{
			HANDLE threadHandler;
			DWORD threadId;
		
			PINIT_STRUCT messageStruct = reinterpret_cast<PINIT_STRUCT>(message);	  

			Logger log;
			name = (LPWSTR)malloc(300);	

			wcscpy(name, messageStruct->Title);

			log.elog(name);
		
			
			//MessageBox(NULL, L"testing", L"testing", 0);
			threadHandler = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&namedPipeServer, 0, 0, &threadId);
		}

	}
	// Define the DLL's main function

		BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
		{
			// Get rid of compiler warnings since we do not use this parameter

			//UNREFERENCED_PARAMETER(lpReserved);

			// If we are attaching to a process

			if(ulReason == DLL_PROCESS_ATTACH)
			{
				// Do not need the thread based attach/detach messages in this DLL

				//DisableThreadLibraryCalls(hModule);
			}

			// Signal for Loading/Unloading
			
			//dropServer();
			//startServer();
			

			return (TRUE);
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

