#include "stdafx.h"
#include "Login.h"



char * Login::atLogin(int & size)
{
		int ret = 0;
		char * output;

		Py_Initialize();
		PyGILState_STATE gstate = PyGILState_Ensure();
		
		
		PyObject * main = PyImport_AddModule("__builtin__");
		if(main == NULL)
		{
			log.elog("Main failed to load");
			PyGILState_Release( gstate );
			output = builder.buildBooleanObject(false, size);			
			return output;
		}
		PyObject * maindic = PyModule_GetDict(main);
		
		if(maindic == NULL)
		{
			log.elog("Couldn't load main dictionary");
			PyGILState_Release( gstate );
			output = builder.buildBooleanObject(false, size);
			return output;
		}

		PyObject * uicore = PyDict_GetItemString(maindic, "uicore");

		if(uicore == NULL)
		{
			log.elog("uicore is null");
			PyGILState_Release( gstate );
			output = builder.buildBooleanObject(false, size);
			return output;
		}
		PyObject * layer = PyObject_GetAttrString(uicore, "layer");
		if(layer == NULL)
		{
			log.elog("layer is null");
			PyGILState_Release( gstate );
			output = builder.buildBooleanObject(false, size);
			return output;
		}

		PyObject * login = PyObject_GetAttrString(layer, "login");
		if(login == NULL)
		{
			log.elog("login is null");
			PyGILState_Release( gstate );
			output = builder.buildBooleanObject(false, size);
			return output;
		}

		//Py_DECREF(main);
		//Py_DECREF(uicore);
		//Py_DECREF(layer);

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
			output = builder.buildBooleanObject(false, size);
			return output;
		}

		PyGILState_Release( gstate );
		return output;
}