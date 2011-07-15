#include "stdafx.h"
#include "Login.h"
#include "Interfaces.h"

Interfaces interfaces;


PyObject * Login::getLayer()
{	
	return interfaces.getLayer("login");
}


char * Login::atLogin(int & size)
{
		char * output;

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