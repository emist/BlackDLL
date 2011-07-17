// mouseDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "mouseDLL.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <windows.h>


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
		
		HWND handleTmp = FindWindow(NULL, TEXT("Eryan"));

		cout << "Handle = " << handle << endl;
		cout << "FindWindowHandle = " << handleTmp;

		cout << "handle is null?: " << (handle == NULL) << endl;


		
	    if(hWndCalc != NULL)
	    {
				 //HWND hWndFour = FindWindowEx(hWndCalc, NULL, TEXT("Button"), TEXT("4")),
				 POINT pt;
				 pt.x = 300;
				 pt.y = 200;

				 HWND hWndFour = WindowFromPoint(pt),
	             hWndPlus = FindWindowEx(hWndCalc, NULL, TEXT("Button"), TEXT("+")),
	             hWndFive = FindWindowEx(hWndCalc, NULL, TEXT("Button"), TEXT("5")),
	             hWndEquals = FindWindowEx(hWndCalc, NULL, TEXT("Button"), TEXT("="));
	    
				 cout << "4th window is: " << hWndFour << endl;

	        if(hWndFour != NULL)
	        {
				cout << "Fourth button is not null " << endl;

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




extern "C" __declspec(dllexport) void dllMouseClick(HWND handle, bool left, int x, int y)
{

		if(handle == NULL)
			cout << "handle is NULL" << endl;

		srand (time(NULL));

	    if(handle != NULL)
	    {
			if(left)
			{
				SendMessage(handle, WM_LBUTTONDOWN, 0, MAKELPARAM(x,y));  
				Sleep(rand() % 10 + 1);
				SendMessage(handle, WM_LBUTTONUP, 0, MAKELPARAM(x,y)); 
			}
			else
			{
				SendMessage(handle, WM_RBUTTONDOWN, 0, MAKELPARAM(x,y));  
				Sleep(rand() % 10 + 1);
				SendMessage(handle, WM_RBUTTONUP, 0, MAKELPARAM(x,y)); 
			}
	    }
		
}

extern "C" __declspec(dllexport) void dllMoveMouse(HWND handle, int x, int y)
{
		if(handle == NULL)
			cout << "handle is null?: " << endl;

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

