#include "stdafx.h"
#include "Interfaces.h"
#include <List>
#include "ObjectBuilder.h"

///_functions should never aquire the GIL, its the caller's responsibility to do so.

using namespace std;


char * Interfaces::atLogin(int & size)
{
		char * output;

		//Py_DECREF(main);
		//Py_DECREF(uicore);
		//Py_DECREF(layer);
		
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject * login = _getLayer("login");

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


char * Interfaces::getInflightInterface(int & size)
{
	char * output = NULL;
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * layer = _getLayer("inflight");
	if(layer == NULL)
	{
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * absoluteTop = _getAbsoluteTop(layer);
	if(absoluteTop == NULL)
	{
		Py_DECREF(layer);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * absoluteLeft = _getAbsoluteLeft(layer);
	if(absoluteLeft == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(absoluteTop);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * displayWidth = _getDisplayWidth(layer);
	
	if(displayWidth == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(absoluteTop);
		Py_DECREF(absoluteLeft);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * displayHeight = _getDisplayHeight(layer);
	if(displayHeight == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(absoluteTop);
		Py_DECREF(absoluteLeft);
		Py_DECREF(displayWidth);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * name = _getName(layer);
	if(name == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(absoluteTop);
		Py_DECREF(absoluteLeft);
		Py_DECREF(displayWidth);
		Py_DECREF(displayHeight);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * iname = PyString_AsString(name);
	output = builder.buildInterfaceObject(iname,  (int)PyInt_AsLong(absoluteLeft),(int)PyInt_AsLong(absoluteTop), (int)PyInt_AsLong(displayWidth), (int)PyInt_AsLong(displayHeight), size);
	Py_DECREF(layer);
	Py_DECREF(absoluteTop);
	Py_DECREF(absoluteLeft);
	Py_DECREF(displayWidth);
	Py_DECREF(displayHeight);
	Py_DECREF(name);
	PyGILState_Release(gstate);
	return output;
}

PyObject * Interfaces::_getDisplayWidth(PyObject * result)
{
	return _getAttribute(result, "displayWidth");
}

PyObject * Interfaces::_getDisplayHeight(PyObject * result)
{
	return _getAttribute(result, "displayHeight");
}

PyObject * Interfaces::_getWidth(PyObject * result)
{	
	return _getAttribute(result, "width");
}

PyObject * Interfaces::_getHeight(PyObject * result)
{		
	return _getAttribute(result, "height");
}


char * Interfaces::findByTextMenu(string label, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _findByTextGeneric("menu", label, size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::findByTextLogin(string text, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _findByTextGeneric("login", text, size);
	PyGILState_Release(gstate);
	return output;
}


char * Interfaces::findByNameLogin(string name, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _findByNameGeneric("login", name, size);
	PyGILState_Release(gstate);
	return output;
}


PyObject * Interfaces::_getAbsoluteLeft(PyObject * result)
{	
	return _getAttribute(result, "absoluteLeft");
}

PyObject * Interfaces::_getAbsoluteTop(PyObject * result)
{
	return _getAttribute(result, "absoluteTop");
}

PyObject * Interfaces::_getText(PyObject * result)
{
	return _getAttribute(result, "text");
}

PyObject * Interfaces::_getName(PyObject * result)
{
	return _getAttribute(result, "name");
}

char * Interfaces::isMenuOpen(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * menu = _getLayer("menu");
	char * output = NULL;

	if(!PyObject_HasAttrString(menu, "children"))
	{
		Py_DECREF(menu);
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * children = PyObject_GetAttrString(menu, "children");
	if(children == NULL)
	{
		log.elog("Couldn't get the children attribute");
		Py_DECREF(menu);
		PyGILState_Release(gstate);
		return NULL;
	}

	
	int len = PyObject_Size(children);

	if(len < 1)
	{
		log.elog("False");
		output = builder.buildBooleanObject(false, size);
	}
	else
	{
		log.elog("True");
		output = builder.buildBooleanObject(true, size);
	}

	Py_DECREF(menu);
	Py_DECREF(children);
	PyGILState_Release(gstate);
	return output;
}


char * Interfaces::_findByTextGeneric(string layername, string label, int & size)
{

	char * output = NULL;
	PyObject * result = NULL;

	PyObject * layer = _getLayer(layername);


	if(layer == NULL)
	{
		log.elog("menulayer is NULL");
		return NULL;
	}

	_findByText(layer, label, &result);

	if(result == NULL)
	{
		log.elog("Result is NULL");
		Py_DECREF(layer);
		return NULL;
	}

	PyObject * leftPosVal, * topPosVal, * name, * width, * height;
	
	leftPosVal = _getAbsoluteLeft(result);
	
	if(leftPosVal == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(result);
		return NULL;
	}

	topPosVal = _getAbsoluteTop(result);
	if(topPosVal == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(leftPosVal);
		Py_DECREF(result);
		return NULL;
	}

	name = _getName(result);
	if(name == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(leftPosVal);
		Py_DECREF(topPosVal);
		Py_DECREF(result);
		return NULL;
	}

	height = _getHeight(result);
	if(height == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(leftPosVal);
		Py_DECREF(topPosVal);
		Py_DECREF(name);
		Py_DECREF(result);
		return NULL;
	}

	width = _getWidth(result);
	if(height == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(leftPosVal);
		Py_DECREF(topPosVal);
		Py_DECREF(name);
		Py_DECREF(height);
		Py_DECREF(result);
		return NULL;
	}

	log.elog("Found Child");
	char * iname = PyString_AsString(name);
	output = builder.buildInterfaceObject(iname,  (int)PyInt_AsLong(leftPosVal),(int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(width), (int)PyInt_AsLong(height), size);
	Py_DECREF(layer);
	Py_DECREF(result);
	Py_DECREF(leftPosVal);
	Py_DECREF(topPosVal);
	Py_DECREF(name);
	Py_DECREF(width);
	Py_DECREF(height);
	return output;
}




char * Interfaces::_getLayerWithAttributes(string layername, int & size)
{

	PyObject * layer = _getLayer(layername);
	PyObject * leftPosVal, * topPosVal, * width, * height, *name;
	leftPosVal = _getAbsoluteLeft(layer);
	char * output = NULL;
	
	if(leftPosVal == NULL)
	{
		Py_DECREF(layer);
		return NULL;
	}

	topPosVal = _getAbsoluteTop(layer);
	if(topPosVal == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(leftPosVal);
		return NULL;
	}

	name = _getName(layer);
	if(name == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(leftPosVal);
		Py_DECREF(topPosVal);
		return NULL;
	}

	height = _getHeight(layer);
	if(height == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(leftPosVal);
		Py_DECREF(topPosVal);
		Py_DECREF(name);
		return NULL;
	}

	width = _getWidth(layer);
	if(height == NULL)
	{
		Py_DECREF(layer);
		Py_DECREF(leftPosVal);
		Py_DECREF(topPosVal);
		Py_DECREF(name);
		Py_DECREF(height);
		return NULL;
	}

	char * iname = PyString_AsString(name);
	output = builder.buildInterfaceObject(iname,  (int)PyInt_AsLong(leftPosVal),(int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(width), (int)PyInt_AsLong(height), size);
	Py_DECREF(layer);
	Py_DECREF(leftPosVal);
	Py_DECREF(topPosVal);
	Py_DECREF(name);
	Py_DECREF(height);
	return output;
}


char * Interfaces::_findByNameGeneric(string layername, string name, int & size)
{
	log.elog(name);

	PyObject * layer = _getLayer(layername);
	
	char * output = NULL;
	PyObject * leftPosVal;
	PyObject * topPosVal;
	PyObject * width, * height;


	if(layer == NULL)
	{
		log.elog("Login Interface is null");
		return NULL;
	}
	
	if(PyObject_HasAttrString(layer, "FindChild"))
	{
		PyObject * findChild = PyObject_GetAttrString(layer, "FindChild");
		if(findChild != NULL)
		{

			PyObject * args = PyString_FromString(name.c_str());
			if(args == NULL)
			{
				log.elog("Failed to create args with args: ");
				log.elog(name);
				Py_DECREF(findChild);
				Py_DECREF(layer);
				return NULL;
			}
			


			PyObject * param = PyTuple_New(1);

			if(param == NULL)
			{
				log.elog("Failed to build PyTuple");
				Py_DECREF(findChild);
				Py_DECREF(layer);
				Py_DECREF(args);
				return NULL;
			}

			
			if(PyTuple_SetItem(param, 0, args) != 0)
			{
				log.elog("Failed to setitem in tuple");
				Py_DECREF(findChild);
				Py_DECREF(layer);
				Py_DECREF(args);
				Py_DECREF(param);
				return NULL;
			}

			

			if(PyCallable_Check(findChild) == 0)
			{
				log.elog("findChild is not callable");
				Py_DECREF(findChild);
				Py_DECREF(layer);
				Py_DECREF(args);
				Py_DECREF(param);
				return NULL;
			}


			PyObject * soughtInterface = PyObject_CallObject(findChild, param );
			
			if(soughtInterface == NULL)
			{
				log.elog("Error calling FindChild(param)");
				log.elog(PyString_AsString(param));
				Py_DECREF(findChild);
				Py_DECREF(layer);
				Py_DECREF(args);
				Py_DECREF(param);
				return NULL;
			}

			leftPosVal = _getAbsoluteLeft(soughtInterface);

			if(leftPosVal == NULL)
			{

				log.elog("Failed to get leftPosVal");
				Py_DECREF(findChild);
				Py_DECREF(layer);
				Py_DECREF(args);
				Py_DECREF(param);
				Py_DECREF(soughtInterface);
				return NULL;
			}
			

			topPosVal = _getAbsoluteTop(soughtInterface);
			if(topPosVal == NULL)
			{
				log.elog("Failed to get topPosVal");
				Py_DECREF(findChild);
				Py_DECREF(layer);
				Py_DECREF(args);
				Py_DECREF(param);
				Py_DECREF(soughtInterface);
				Py_DECREF(leftPosVal);
				return NULL;
			}
			
			width = _getWidth(soughtInterface);
			if(topPosVal == NULL)
			{
				log.elog("Failed to get width");
				Py_DECREF(findChild);
				Py_DECREF(layer);
				Py_DECREF(args);
				Py_DECREF(param);
				Py_DECREF(soughtInterface);
				Py_DECREF(leftPosVal);
				Py_DECREF(topPosVal);
				return NULL;
			}			


			height = _getHeight(soughtInterface);
			if(topPosVal == NULL)
			{
				log.elog("Failed to get width");
				Py_DECREF(findChild);
				Py_DECREF(layer);
				Py_DECREF(args);
				Py_DECREF(param);
				Py_DECREF(soughtInterface);
				Py_DECREF(leftPosVal);
				Py_DECREF(topPosVal);
				Py_DECREF(width);
				return NULL;
			}

			
			log.elog("Found Child");
			output = builder.buildInterfaceObject(name,  (int)PyInt_AsLong(leftPosVal) ,(int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(width), (int)PyInt_AsLong(height), size);
			Py_DECREF(findChild);
			Py_DECREF(layer);
			Py_DECREF(args);
			Py_DECREF(param);
			Py_DECREF(soughtInterface);
			Py_DECREF(leftPosVal);
			Py_DECREF(topPosVal);
			Py_DECREF(width);
			Py_DECREF(height);
		}
		else
		{
			log.elog("findChild Method not found");
		}
	}
	

	return output;
}


char * Interfaces::OverViewGetMembers(int & size)
{
	
	list<ObjectBuilder::overViewEntry *> labels;
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");

	if(main == NULL)
	{
		log.elog("main is null");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * overview = _findByNameLayer(main, "overview");
	if(overview == NULL)
	{
		log.elog("Couldn't get overview child");
		Py_DECREF(main);
		PyGILState_Release(gstate);
		return NULL;
	}
		

	PyObject * maincontainer = _findByNameLayer(overview, "maincontainer");
	if(maincontainer == NULL)
	{
		log.elog("maincontainer is null");
		Py_DECREF(main);
		Py_DECREF(overview);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * content = _findByNameLayer(maincontainer, "__content");	
	if(content == NULL)
	{
		log.elog("content is null");
		Py_DECREF(main);
		Py_DECREF(maincontainer);
		Py_DECREF(overview);
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * children = PyObject_GetAttrString(content, "children");
	
	if(children == NULL)
	{
		log.elog("Couldn't get CHildren");
		Py_DECREF(main);
		Py_DECREF(maincontainer);
		Py_DECREF(content);
		Py_DECREF(overview);
		PyGILState_Release(gstate);
		return NULL;
	}

	int len = PyObject_Size(children);
	if(len < 1)
	{
		log.elog("Overview is Closed");
		Py_DECREF(main);
		Py_DECREF(maincontainer);
		Py_DECREF(content);
		Py_DECREF(children);
		Py_DECREF(overview);
		PyGILState_Release(gstate);
		return NULL;
	}

	for(int i = 0; i < len; i++)
	{

		PyObject * pkey = PyInt_FromLong(i);
		PyObject * pvalue = PyObject_GetItem(children, pkey);
		if(pvalue == NULL)
		{
			log.elog("Couldn't get the child value");
			Py_DECREF(main);
			Py_DECREF(maincontainer);
			Py_DECREF(content);
			Py_DECREF(children);
			Py_DECREF(overview);
			PyGILState_Release(gstate);
			return NULL;
		}


		PyObject * label = _findByNameLayer(pvalue, "text");
		if(label == NULL)
		{
			log.elog("No label");
			Py_DECREF(main);
			Py_DECREF(maincontainer);
			Py_DECREF(content);
			Py_DECREF(children);
			Py_DECREF(overview);
			Py_DECREF(pvalue);
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * text = _getText(label);
	
		if(text == NULL)
		{
			log.elog("No text in the label");
			Py_DECREF(main);
			Py_DECREF(maincontainer);
			Py_DECREF(content);
			Py_DECREF(children);
			Py_DECREF(overview);
			Py_DECREF(pvalue);
			Py_DECREF(label);
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * absoluteTop = _getAbsoluteTop(label);
		if(absoluteTop == NULL)
		{
			log.elog("No absoluteTop");
			Py_DECREF(main);
			Py_DECREF(maincontainer);
			Py_DECREF(content);
			Py_DECREF(children);
			Py_DECREF(overview);
			Py_DECREF(pvalue);
			Py_DECREF(label);
			Py_DECREF(text);
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * absoluteLeft = _getAbsoluteLeft(label);
		if(absoluteLeft == NULL)
		{
			log.elog("No absoluteLeft");
			Py_DECREF(main);
			Py_DECREF(maincontainer);
			Py_DECREF(content);
			Py_DECREF(children);
			Py_DECREF(overview);
			Py_DECREF(pvalue);
			Py_DECREF(label);
			Py_DECREF(text);
			Py_DECREF(absoluteTop);
			PyGILState_Release(gstate);
			return NULL;
		}
		
		PyObject * width = _getWidth(label);
		if(width == NULL)
		{
			log.elog("No width");
			Py_DECREF(main);
			Py_DECREF(maincontainer);
			Py_DECREF(content);
			Py_DECREF(children);
			Py_DECREF(overview);
			Py_DECREF(pvalue);
			Py_DECREF(label);
			Py_DECREF(text);
			Py_DECREF(absoluteLeft);
			Py_DECREF(absoluteTop);
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * height = _getHeight(label);
		if(height == NULL)
		{
			log.elog("No height");
			Py_DECREF(main);
			Py_DECREF(maincontainer);
			Py_DECREF(content);
			Py_DECREF(children);
			Py_DECREF(overview);
			Py_DECREF(pvalue);
			Py_DECREF(label);
			Py_DECREF(text);
			Py_DECREF(absoluteLeft);
			Py_DECREF(absoluteTop);
			Py_DECREF(width);
			PyGILState_Release(gstate);
			return NULL;
		}

		ObjectBuilder::overViewEntry * over = new ObjectBuilder::overViewEntry();
		over->text = PyString_AsString(text);
		over->topLeftX = PyInt_AsLong(absoluteLeft);
		over->topLeftY = PyInt_AsLong(absoluteTop);
		over->width = PyInt_AsLong(width);
		over->height = PyInt_AsLong(height);
		labels.push_back(over);
		Py_DECREF(pvalue);
		Py_DECREF(label);
		Py_DECREF(text);
		Py_DECREF(absoluteLeft);
		Py_DECREF(absoluteTop);
		Py_DECREF(width);

	}
	log.elog("Building overview object");
	char * output = builder.buildOverViewObject(labels, size);

	for(list<ObjectBuilder::overViewEntry *>::iterator it = labels.begin(); it != labels.end(); it++)
	{
		delete (*it);
	}

	Py_DECREF(main);
	Py_DECREF(maincontainer);
	Py_DECREF(content);
	Py_DECREF(children);
	Py_DECREF(overview);
	PyGILState_Release(gstate);
	return output;
}

PyObject * Interfaces::_findByNameLayer(PyObject * layer, string name)
{

	PyObject * soughtInterface = NULL;


	log.elog(name);
	if(layer == NULL)
	{
		log.elog("Login Interface is null");
		return NULL;
	}
	
	if(PyObject_HasAttrString(layer, "FindChild"))
	{
		PyObject * findChild = PyObject_GetAttrString(layer, "FindChild");
		if(findChild != NULL)
		{

			PyObject * args = PyString_FromString(name.c_str());
			if(args == NULL)
			{
				log.elog("Failed to create args with args: ");
				log.elog(name);
				Py_DECREF(findChild);
				return NULL;
			}
			
			PyObject * param = PyTuple_New(1);

			if(param == NULL)
			{
				log.elog("Failed to build PyTuple");
				Py_DECREF(findChild);
				Py_DECREF(args);
				return NULL;
			}

			
			if(PyTuple_SetItem(param, 0, args) != 0)
			{
				log.elog("Failed to setitem in tuple");
				Py_DECREF(findChild);
				Py_DECREF(args);
				Py_DECREF(param);
				return NULL;
			}

			

			if(PyCallable_Check(findChild) == 0)
			{
				log.elog("findChild is not callable");
				Py_DECREF(findChild);
				Py_DECREF(args);
				Py_DECREF(param);
				return NULL;
			}


			soughtInterface = PyObject_CallObject(findChild, param );
			
			if(soughtInterface == NULL)
			{
				log.elog("Error calling FindChild(param)");
				log.elog(PyString_AsString(param));
				Py_DECREF(findChild);
				Py_DECREF(args);
				Py_DECREF(param);
				return NULL;
			}

			log.elog("Found Child");
			//output = builder.buildInterfaceObject(name,  (int)PyInt_AsLong(leftPosVal) ,(int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(width), (int)PyInt_AsLong(height), size);
			Py_DECREF(findChild);
			Py_DECREF(args);
			Py_DECREF(param);
		}
		else
		{
			log.elog("findChild Method not found");
		}
	}
	
	return soughtInterface;
}


PyObject * Interfaces::_getLayer(string name)
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
	
///Changed for debugging


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


