#include "stdafx.h"
#include "Login.h"


PyObject * Login::getLayer()
{		
		PyObject * output = NULL;
		
		PyObject * main = PyImport_AddModule("__builtin__");
		if(main == NULL)
		{
			log.elog("Main failed to load");
			return output;
		}

		PyObject * maindic = PyModule_GetDict(main);
		
		if(maindic == NULL)
		{
			log.elog("Couldn't load main dictionary");
			return output;
		}

		PyObject * uicore = PyDict_GetItemString(maindic, "uicore");

		if(uicore == NULL)
		{
			log.elog("uicore is null");
			return output;
		}
		PyObject * layer = PyObject_GetAttrString(uicore, "layer");
		if(layer == NULL)
		{
			log.elog("layer is null");
			return output;
		}

		PyObject * login = PyObject_GetAttrString(layer, "login");
		if(login == NULL)
		{
			log.elog("login is null");
			return output;
		}

		return login;
}


char * Login::atLogin(int & size)
{
		char * output;

		Py_Initialize();

		//Py_DECREF(main);
		//Py_DECREF(uicore);
		//Py_DECREF(layer);
		
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject * login = Login::getLayer();

		if(login == NULL)
		{
			log.elog("Login is NULL");
			PyGILState_Release( gstate );
			return NULL;
		}

		PyObject * isopen = PyObject_GetAttrString(login, "isopen");
		if(isopen != NULL)
		{
			if(PyObject_IsTrue(isopen))
			{
				log.elog("Login is open");
				output = builder.buildBooleanObject(true, size);
			}
			else
			{
				log.elog("Login is false");
				output = builder.buildBooleanObject(false, size);
			}
		}
		else 
		{
			log.elog("isopen is null");
			PyGILState_Release( gstate );
			return NULL;
		}

		PyGILState_Release( gstate );
		return output;
}