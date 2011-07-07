// mouseDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "mouseDLL.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <time.h>


using namespace std;
// This is an example of an exported variable
MOUSEDLL_API int nmouseDLL=0;

// This is an example of an exported function.
MOUSEDLL_API int fnmouseDLL(void)
{
	return 42;
}

extern "C" __declspec(dllexport) void dllCalcTest(HWND handle)
{
		HWND hWndCalc = handle; //FindWindow(NULL, TEXT("Calculator Plus"));
		
		cout << "handle is null?: " << (handle == NULL) << endl;

	    if(hWndCalc != NULL)
	    {
				 HWND hWndFour = FindWindowEx(hWndCalc, NULL, "BUTTON", "4"),
	             hWndPlus = FindWindowEx(hWndCalc, NULL, "BUTTON", "+"),
	             hWndFive = FindWindowEx(hWndCalc, NULL, "BUTTON", "5"),
	             hWndEquals = FindWindowEx(hWndCalc, NULL, "BUTTON", "=");
	         
	        if(hWndFour != NULL)
	        {
	            SendMessage(hWndFour, WM_LBUTTONDOWN, 0, MAKELPARAM(10,10));  
	            SendMessage(hWndFour, WM_LBUTTONUP, 0, MAKELPARAM(10,10));  
	            Sleep(500);
	            SendMessage(hWndPlus, WM_LBUTTONDOWN, 0, MAKELPARAM(10,10));  
	            SendMessage(hWndPlus, WM_LBUTTONUP, 0, MAKELPARAM(10,10)); 
	            Sleep(500);
	            SendMessage(hWndFive, WM_LBUTTONDOWN, 0, MAKELPARAM(10,10));  
	            SendMessage(hWndFive, WM_LBUTTONUP, 0, MAKELPARAM(10,10)); 
	            Sleep(500);
	            SendMessage(hWndEquals, WM_LBUTTONDOWN, 0, MAKELPARAM(10,10));  
	            SendMessage(hWndEquals, WM_LBUTTONUP, 0, MAKELPARAM(10,10)); 
	        }
	    }
}

extern "C" __declspec(dllexport) void dllMouseClick(HWND handle, int x, int y)
{
		cout << "handle is null?: " << (handle == NULL) << endl;

		srand (time(NULL));

	    if(handle != NULL)
	    {
			SendMessage(handle, WM_LBUTTONDOWN, 0, MAKELPARAM(x,y));  
			Sleep(rand() % 10 + 1);
			SendMessage(handle, WM_LBUTTONUP, 0, MAKELPARAM(x,y)); 
	    }
		
}

extern "C" __declspec(dllexport) void dllMoveMouse(HWND handle, int x, int y)
{
		cout << "handle is null?: " << (handle == NULL) << endl;

	    if(handle != NULL)
	    {
			SendMessage(handle, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
	    }
		
	 
}

// This is the constructor of a class that has been exported.
// see mouseDLL.h for the class definition
CmouseDLL::CmouseDLL()
{
	return;
}

