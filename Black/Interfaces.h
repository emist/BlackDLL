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
	PyObject * _getDisplayWidth(PyObject * result);
	PyObject * _getDisplayHeight(PyObject * result);
	PyObject * Interfaces::_getText(PyObject * result);
	PyObject * _getHeight(PyObject * result);
	PyObject * _getWidth(PyObject * result);
	PyObject * _findByNameLayer(PyObject * layer, string name);
	PyObject * _getLayer(string layername);

	char * _findByTextGeneric(string layername, string label, int & size);
	char * _findByNameGeneric(string layername, string name, int & size);
	char * _getLayerWithAttributes(string layername, int & size);

public:
	char * GetTargetList(int & size);
	char * GetSelectedItem(int & size);
	char * isMenuOpen(int & size);
	char * OverViewGetMembers(int & size);
	char * getInflightInterface(int & size);
	char * atLogin(int & size);
	char * findByTextMenu(string label, int & size);
	char * findByNameLogin(string name, int & size);
	char * findByTextLogin(string text, int & size);
};
#endif