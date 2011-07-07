// SendTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <windows.h>
#include <fstream>
#include <iostream>

using namespace std;

	int main ()
	{
	 
//		cout << "entering" << endl;
	    HWND hWndCalc = FindWindow(NULL, TEXT("Calculator Plus"));
		

	    if(hWndCalc != NULL)
	    {
				 cout << "entering" << endl;

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
		
	    return 0;
	}
