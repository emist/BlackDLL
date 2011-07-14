#include "stdafx.h"

using namespace std;

class Logger{

public:
	void elog(string message);
	void elog(string & message);
	void elog(char * message);
};