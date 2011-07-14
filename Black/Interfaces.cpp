#include "stdafx.h"
#include "Interfaces.h"
#include "Logger.h"



char * Interfaces::findByName(string name, int & size)
{

	Py_Initialize();

	PyGILState_STATE gstate = PyGILState_Ensure();
	//Momentary hack, seeing if this is going to work
	log.elog(name);

	PyObject * loginInterface = login.getLayer();
	
	char * output = NULL;
	PyObject * leftPosVal;
	PyObject * topPosVal;
	
	string absoluteLeft("absoluteLeft");
	string absoluteTop("absoluteTop");

	if(loginInterface == NULL)
	{
		log.elog("Login Interface is null");
		PyGILState_Release( gstate );
		return NULL;
	}
	
	if(PyObject_HasAttrString(loginInterface, "FindChild"))
	{
		PyObject * findChild = PyObject_GetAttrString(loginInterface, "FindChild");
		if(findChild != NULL)
		{

			PyObject * args = PyString_FromString(name.c_str());
			if(args == NULL)
			{
				log.elog("Failed to create args with args: ");
				log.elog(name);
				PyGILState_Release( gstate );
				return NULL;
			}
			


			PyObject * param = PyTuple_New(1);

			if(param == NULL)
			{
				log.elog("Failed to build PyTuple");
				PyGILState_Release( gstate );
				return NULL;
			}

			
			if(PyTuple_SetItem(param, 0, args) != 0)
			{
				log.elog("Failed to setitem in tuple");
				PyGILState_Release( gstate );
				return NULL;
			}

			

			if(PyCallable_Check(findChild) == 0)
			{
				log.elog("findChild is not callable");
				PyGILState_Release( gstate );
				return NULL;
			}


			PyObject * soughtInterface = PyObject_CallObject(findChild, param );
			
			if(soughtInterface == NULL)
			{
				log.elog("Error calling FindChild(param)");
				log.elog(PyString_AsString(param));
				PyGILState_Release( gstate );
				return NULL;
			}

			if(PyObject_HasAttrString(soughtInterface, absoluteLeft.c_str()))
			{
				leftPosVal = PyObject_GetAttrString(soughtInterface, absoluteLeft.c_str());
				if(leftPosVal == NULL)
				{
					log.elog("Failed to get leftPosVal");
					PyGILState_Release( gstate );
					return NULL;
				}
			}
			else 
			{
				log.elog("Doesn't have " + absoluteLeft);
				PyGILState_Release( gstate );
				return NULL;
			}

			if(PyObject_HasAttrString(soughtInterface, absoluteTop.c_str()))
			{
				topPosVal = PyObject_GetAttrString(soughtInterface, absoluteTop.c_str());
				if(topPosVal == NULL)
				{
					log.elog("Failed to get topPosVal");
					PyGILState_Release( gstate );
					return NULL;
				}
			}
			else 
			{
				log.elog("Doesn't have " + absoluteTop);
				PyGILState_Release( gstate );
				return NULL;
			}
			
			log.elog("Found Child");
			output = builder.buildInterfaceObject(name, (int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(leftPosVal), size);

		}
		else
		{
			log.elog("findChild Method not found");
		}
	}

	PyGILState_Release( gstate );
	return output;
}