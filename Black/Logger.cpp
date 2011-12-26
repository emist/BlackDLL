#include "stdafx.h"
#include "Logger.h"
#include <fstream>
#include "python/Python.h"

using namespace std;
//
char * f = "C:\\log.txt";

void Logger::elog(char * message)
{

	ofstream myfile;
	myfile.open(f, fstream::app);
	myfile << message << endl;
	myfile.flush();
	myfile.close();
}

void Logger::elog(LPCWSTR message)
{

	wofstream myfile;
	myfile.open(f, fstream::app);
	myfile << message << endl;
	myfile.flush();
	myfile.close();

}

void Logger::elog(LPCSTR * message)
{

	ofstream myfile;
	myfile.open(f, fstream::app);
	myfile << *message << endl;
	myfile.flush();
	myfile.close();

}

void Logger::elog(string message)
{

	ofstream myfile;
	myfile.open(f, fstream::app);
	myfile << message << endl;
	myfile.flush();
	myfile.close();
}

