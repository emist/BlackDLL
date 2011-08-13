#include "stdafx.h"
#include "Logger.h"
#include <fstream>
#include "python/Python.h"

using namespace std;


void Logger::elog(char * message)
{
/*
	ofstream myfile;
	myfile.open("C:\\Users\\emist\\log.txt", fstream::app);
	myfile << message << endl;
	myfile.flush();
	myfile.close();
*/
}

void Logger::elog(LPCWSTR message)
{
/*
	wofstream myfile;
	myfile.open("C:\\Users\\emist\\log.txt", fstream::app);
	myfile << message << endl;
	myfile.flush();
	myfile.close();
*/
}

void Logger::elog(LPCSTR * message)
{
/*
	ofstream myfile;
	myfile.open("C:\\Users\\emist\\log.txt", fstream::app);
	myfile << *message << endl;
	myfile.flush();
	myfile.close();
*/
}

void Logger::elog(string message)
{
/*
	ofstream myfile;
	myfile.open("C:\\Users\\emist\\log.txt", fstream::app);
	myfile << message << endl;
	myfile.flush();
	myfile.close();
*/
}

