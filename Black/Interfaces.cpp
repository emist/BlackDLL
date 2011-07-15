#include "stdafx.h"
#include "Interfaces.h"

PyObject * Interfaces::getLayer(string name)
{
	
		
		PyObject * main = PyImport_AddModule("__builtin__");
		if(main == NULL)
		{
			log.elog("Main failed to load");
			return NULL;
		}

		PyObject * maindic = PyModule_GetDict(main);
		
		if(maindic == NULL)
		{
			log.elog("Couldn't load main dictionary");
			return NULL;
		}

		PyObject * uicore = PyDict_GetItemString(maindic, "uicore");

		if(uicore == NULL)
		{
			log.elog("uicore is null");
			return NULL;
		}
		PyObject * layer = PyObject_GetAttrString(uicore, "layer");
		if(layer == NULL)
		{
			log.elog("layer is null");
			Py_DECREF(uicore);
			return NULL;
		}

		PyObject * layeritem = PyObject_GetAttrString(layer, name.c_str());
		if(layeritem == NULL)
		{
			log.elog("layeritem is null");
			Py_DECREF(uicore);
			Py_DECREF(layer);
			return NULL;
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
		//log.elog("Interface has no children");
		return;
	}
	
	PyObject * children = PyObject_GetAttrString(parentInt, "children");
	if(children == NULL)
	{
		log.elog("Couldn't get the children attribute");
		return;
	}

	
	int len = PyObject_Size(children);

	//log.elog(len + "");

	if(len < 1)
	{
		log.elog("End of branch");
		Py_DECREF(children);
		return;
	}
	
	if(len > 1)
	{
		//log.elog("Has many children");
	}

	Py_ssize_t pos = 0;

	PyObject *pkey;
	PyObject *pvalue;
	char buf[30];
	char *buff = buf;
		
	//log.elog("About to enter while loop");

	for(int i = 0; i < len; i++)
	{	//log.elog("Iterating");


		pkey = PyInt_FromLong(i);
		pvalue = PyObject_GetItem(children, pkey);
		if(pvalue == NULL)
		{
			log.elog("Couldn't get children[i]");
			Py_DECREF(children);
			return;
		}

		if(PyObject_HasAttrString(pvalue, "text"))
		{
			PyObject * ptext = PyObject_GetAttrString(pvalue, "text");
			if(ptext != NULL)
			{
				//log.elog("Function has text: ");
				
				char * ctext = PyString_AsString(ptext);
			
				if(ctext == NULL)
				{
					log.elog("Couldn't convert python string");
					Py_DECREF(children);
					Py_DECREF(ptext);
					return;
				}

				if(strcmp(ctext, text.c_str()) == 0)
				{
					//log.elog("Found text");
					*result = pvalue;
					//delete buf;
					//delete ibuf;
					Py_DECREF(children);
					Py_DECREF(ptext);
					return;
				}
			}
			//PyObject * firstChild = PyDict_GetItemString(children, "0");
			/*
			if(firstChild == NULL)
			{
				log.elog("Couldn't get first child");
				Py_DECREF(children);
				return;
			}
			*/
		}
		_findByText(pvalue, text, result); 
		//log.elog("Function ran");
		Py_DECREF(pvalue);
	}
		

}


PyObject * Interfaces::_getAttribute(PyObject * result, string attr)
{
	PyObject * attribute = NULL;
	if(PyObject_HasAttrString(result, attr.c_str()))
	{
		attribute = PyObject_GetAttrString(result, attr.c_str());
		if(attribute == NULL)
		{
			log.elog("Failed to get " + attr);
			Py_DECREF(result);
			return NULL;
		}
	}
	else
	{
		log.elog("Doesn't have name" + attr);
		Py_DECREF(result);
		return NULL;
	}
	return attribute;
}



PyObject * Interfaces::_getAbsoluteLeft(PyObject * result)
{	
	return _getAttribute(result, "absoluteLeft");
}

PyObject * Interfaces::_getAbsoluteTop(PyObject * result)
{
	return _getAttribute(result, "absoluteTop");
}
PyObject * Interfaces::_getName(PyObject * result)
{
	return _getAttribute(result, "name");
}



char * Interfaces::findByTextMenu(string label, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	PyObject * result = NULL;

	PyObject * menulayer = getLayer("menu");


	if(menulayer == NULL)
	{
		log.elog("menulayer is NULL");
		PyGILState_Release( gstate );
		return NULL;
	}

	_findByText(menulayer, label, &result);

	if(result == NULL)
	{
		log.elog("Result is NULL");
		Py_DECREF(menulayer);
		PyGILState_Release( gstate );
		return NULL;
	}

	PyObject * leftPosVal, * topPosVal, * name;
	
	leftPosVal = _getAbsoluteLeft(result);
	
	if(leftPosVal == NULL)
	{
		Py_DECREF(menulayer);
		PyGILState_Release( gstate );
		return NULL;
	}

	topPosVal = _getAbsoluteTop(result);
	if(topPosVal == NULL)
	{
		Py_DECREF(menulayer);
		Py_DECREF(leftPosVal);
		PyGILState_Release( gstate );
		return NULL;
	}

	name = _getName(result);
	if(name == NULL)
	{
		Py_DECREF(menulayer);
		Py_DECREF(leftPosVal);
		Py_DECREF(topPosVal);
		PyGILState_Release( gstate );
		return NULL;
	}

	log.elog("Found Child");
	char * iname = PyString_AsString(name);
	output = builder.buildInterfaceObject(iname, (int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(leftPosVal), size);
	Py_DECREF(menulayer);
	Py_DECREF(result);
	Py_DECREF(leftPosVal);
	Py_DECREF(topPosVal);
	Py_DECREF(name);
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::findByTextLogin(string text, int & size)
{
	
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	PyObject * result = NULL;

	PyObject * loginlayer = login.getLayer();


	if(loginlayer == NULL)
	{
		log.elog("Loginlayer is NULL");
		PyGILState_Release( gstate );
		return NULL;
	}

	_findByText(loginlayer, text, &result);

	if(result == NULL)
	{
		log.elog("Result is NULL");
		Py_DECREF(loginlayer);
	
		PyGILState_Release( gstate );
		return NULL;
	}

	PyObject * leftPosVal, * topPosVal, * name;
	
	leftPosVal = _getAbsoluteLeft(result);
	
	if(leftPosVal == NULL)
	{
		Py_DECREF(loginlayer);
		PyGILState_Release( gstate );
		return NULL;
	}

	topPosVal = _getAbsoluteTop(result);
	if(topPosVal == NULL)
	{
		Py_DECREF(loginlayer);
		Py_DECREF(leftPosVal);
		PyGILState_Release( gstate );
		return NULL;
	}

	name = _getName(result);
	if(name == NULL)
	{
		Py_DECREF(loginlayer);
		Py_DECREF(leftPosVal);
		Py_DECREF(topPosVal);
		PyGILState_Release( gstate );
		return NULL;
	}

	log.elog("Found Child");
	char * iname = PyString_AsString(name);
	output = builder.buildInterfaceObject(iname, (int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(leftPosVal), size);
	Py_DECREF(loginlayer);
	Py_DECREF(result);
	Py_DECREF(leftPosVal);
	Py_DECREF(topPosVal);
	Py_DECREF(name);
	PyGILState_Release(gstate);
	return output;
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