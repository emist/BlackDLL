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
	PyObject * Interfaces::_getDisplayWidth(PyObject * result);
	PyObject * Interfaces::_getDisplayHeight(PyObject * result);
	
	char * _findByTextGeneric(string layername, string label, int & size);
	PyObject * Interfaces::_getHeight(PyObject * result);
	PyObject * Interfaces::_getWidth(PyObject * result);
	char * _findByNameGeneric(string layername, string name, int & size);
	char * _getLayerWithAttributes(string layername, int & size);
public:
	PyObject * getLayer(string layername);
	char * isMenuOpen(int & size);
	char * getInflightInterface(int & size);
	char * findByTextMenu(string label, int & size);
	char * findByNameLogin(string name, int & size);
	char * findByTextLogin(string text, int & size);
};
#endif