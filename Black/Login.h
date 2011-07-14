#include "stdafx.h"
#include "Logger.h"
#include "ObjectBuilder.h"

class Login{

public:
	Logger log;
	ObjectBuilder builder;
	Login(){};
	//char * atLogin(int * size);
	char * atLogin(int & size);
};