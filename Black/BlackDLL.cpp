#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include <stdafx.h>
#include <iostream>
#include "python/Python.h"
#include <fstream>



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


	__declspec(dllexport) void startServer()
	{
		Py_Initialize();
		PyGILState_STATE gstate = PyGILState_Ensure();
		
		
		PySys_SetPath("C:\\Users\\emist\\Documents;"
		"C:\\Windows\\system32\\python27.zip;"
		"C:\\Python27\\DLLs;"
		"C:\\Python27\\lib;"
		"C:\\Python27\\lib\\plat-win;"
		"C:\\Python27\\lib\\lib-tk;"
		"C:\\Python27;"
		"C:\\Python27\\lib\\site-packages;");
		
		
		char buf[600];

		
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
	}

	__declspec(dllexport) HANDLE dropServer()
	{
		HANDLE threadHandler;
		DWORD threadId;
		threadHandler = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&cServerThread, NULL, 0, &threadId);
		return threadHandler;
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