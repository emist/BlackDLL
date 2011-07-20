#ifndef LOGIN
#define LOGIN

#include "Logger.h"
#include "ObjectBuilder.h"


class Login{

public:
	Logger log;
	ObjectBuilder builder;
	

	//char * atLogin(int * size);
	PyObject * getLayer();
	//char * atLogin(int & size);
	
};

#endif