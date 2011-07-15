#include "stdafx.h"
#include "Interfaces.h"

PyObject * Interfaces::getLayer(string name)
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

		PyObject * layeritem = PyObject_GetAttrString(layer, name.c_str());
		if(layeritem == NULL)
		{
			log.elog("layeritem is null");
			return output;
		}

		return layeritem;
}

void Interfaces::_findByText(PyObject * parentInt, string text, PyObject ** result)
{

	if(parentInt == NULL)
	{
		log.elog("Interface is NULL");
		return;
	}

	if(!PyObject_HasAttrString(parentInt, "children"))
	{
		log.elog("Interface has no children");
		return;
	}
	
	PyObject * children = PyObject_GetAttrString(parentInt, "children");
	if(children == NULL)
	{
		log.elog("Couldn't get the children attribute");
		return;
	}

	int len = PyObject_Size(children);

	if(len < 1)
	{
		log.elog("End of branch");
		return;
	}

	Py_ssize_t * pos = 0;

	PyObject *pkey;
	PyObject *pvalue;

	while(PyDict_Next(children, pos, &pkey, &pvalue))
	{
		if(PyObject_HasAttrString(pvalue, "text"))
		{
			PyObject * ptext = PyObject_GetAttrString(pvalue, "text");
			if(ptext != NULL)
			{
				char * ctext = PyString_AsString(ptext);
				if(ctext == NULL)
				{
					log.elog("Couldn't convert python screen");
					return;
				}

				if(strcmp(ctext, text.c_str()) == 0)
				{
					log.elog("Found text");
					*result = pvalue;
					return;
				}
			}
			PyObject * firstChild = PyDict_GetItemString(children, "0");
			if(firstChild == NULL)
			{
				log.elog("Couldn't get first child");
				return;
			}

			_findByText(firstChild, text, result); 
		}
	}
	

}

char * Interfaces::findByText(string text, int & size)
{
	
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * result;
	_findByText(login.getLayer(), text, &result);
	PyGILState_Release(gstate);

	return NULL;
}


char * Interfaces::findByNameLogin(string name, int & size)
{

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
				Py_DECREF(findChild);
				Py_DECREF(loginInterface);
				PyGILState_Release( gstate );
				return NULL;
			}
			


			PyObject * param = PyTuple_New(1);

			if(param == NULL)
			{
				log.elog("Failed to build PyTuple");
				Py_DECREF(findChild);
				Py_DECREF(loginInterface);
				Py_DECREF(args);
				PyGILState_Release( gstate );
				return NULL;
			}

			
			if(PyTuple_SetItem(param, 0, args) != 0)
			{
				log.elog("Failed to setitem in tuple");
				Py_DECREF(findChild);
				Py_DECREF(loginInterface);
				Py_DECREF(args);
				Py_DECREF(param);
				PyGILState_Release( gstate );
				return NULL;
			}

			

			if(PyCallable_Check(findChild) == 0)
			{
				log.elog("findChild is not callable");
				Py_DECREF(findChild);
				Py_DECREF(loginInterface);
				Py_DECREF(args);
				Py_DECREF(param);
				PyGILState_Release( gstate );
				return NULL;
			}


			PyObject * soughtInterface = PyObject_CallObject(findChild, param );
			
			if(soughtInterface == NULL)
			{
				log.elog("Error calling FindChild(param)");
				log.elog(PyString_AsString(param));
				Py_DECREF(findChild);
				Py_DECREF(loginInterface);
				Py_DECREF(args);
				Py_DECREF(param);
				PyGILState_Release( gstate );
				return NULL;
			}

			if(PyObject_HasAttrString(soughtInterface, absoluteLeft.c_str()))
			{
				leftPosVal = PyObject_GetAttrString(soughtInterface, absoluteLeft.c_str());
				if(leftPosVal == NULL)
				{
					log.elog("Failed to get leftPosVal");
					Py_DECREF(findChild);
					Py_DECREF(loginInterface);
					Py_DECREF(args);
					Py_DECREF(param);
					Py_DECREF(soughtInterface);
					PyGILState_Release( gstate );
					return NULL;
				}
			}
			else 
			{
				log.elog("Doesn't have " + absoluteLeft);
				Py_DECREF(findChild);
				Py_DECREF(loginInterface);
				Py_DECREF(args);
				Py_DECREF(param);
				Py_DECREF(soughtInterface);
				PyGILState_Release( gstate );
				return NULL;
			}

			if(PyObject_HasAttrString(soughtInterface, absoluteTop.c_str()))
			{
				topPosVal = PyObject_GetAttrString(soughtInterface, absoluteTop.c_str());
				if(topPosVal == NULL)
				{
					log.elog("Failed to get topPosVal");
					Py_DECREF(findChild);
					Py_DECREF(loginInterface);
					Py_DECREF(args);
					Py_DECREF(param);
					Py_DECREF(soughtInterface);
					Py_DECREF(leftPosVal);
					PyGILState_Release( gstate );
					return NULL;
				}
			}
			else 
			{
				log.elog("Doesn't have " + absoluteTop);
				Py_DECREF(findChild);
				Py_DECREF(loginInterface);
				Py_DECREF(args);
				Py_DECREF(param);
				Py_DECREF(soughtInterface);
				Py_DECREF(leftPosVal);
				PyGILState_Release( gstate );
				return NULL;
			}
			
			log.elog("Found Child");
			output = builder.buildInterfaceObject(name, (int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(leftPosVal), size);
			Py_DECREF(findChild);
			Py_DECREF(loginInterface);
			Py_DECREF(args);
			Py_DECREF(param);
			Py_DECREF(soughtInterface);
			Py_DECREF(leftPosVal);
			Py_DECREF(topPosVal);
		}
		else
		{
			log.elog("findChild Method not found");
		}
	}
	
	PyGILState_Release( gstate );	
	return output;
}