#ifndef INTERFACES
#define INTERFACES

#include <string>
#include "Login.h"
#include "Logger.h"
#include "ObjectBuilder.h"

using namespace std; 

class Interfaces{
	Login login;
	Logger log;
	ObjectBuilder builder;

	PyObject * _findByText(string text, int & size);

public:
	char * findByName(string name, int & size);
	char * findByText(string text, int & size);
};
#endif