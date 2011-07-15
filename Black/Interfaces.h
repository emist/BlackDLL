#ifndef INTERFACES
#define INTERFACES

#include <string>
#include "Login.h"

using namespace std; 

class Interfaces{
	
	Login login;

	Logger log;

	ObjectBuilder builder;

	void _findByText(PyObject * parentInt, string text, PyObject ** obj);

public:
	PyObject * getLayer(string layername);
	char * findByNameLogin(string name, int & size);
	char * findByTextLogin(string text, int & size);
};
#endif