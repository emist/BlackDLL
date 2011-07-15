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
	PyObject * _getAbsoluteLeft(PyObject * result);
	PyObject * _getAbsoluteTop(PyObject * result);
	PyObject * _getName(PyObject * result);
	PyObject * _getAttribute(PyObject * result, string attr);
public:
	PyObject * getLayer(string layername);
	char * findByTextMenu(string label, int & size);
	char * findByNameLogin(string name, int & size);
	char * findByTextLogin(string text, int & size);
};
#endif