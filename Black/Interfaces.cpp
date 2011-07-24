#include "stdafx.h"
#include "Interfaces.h"
#include <List>
#include "ObjectBuilder.h"
#include <sstream>
#include <iostream>

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

PyObject * Interfaces::_findModule(string module)
{
	PyObject * shipui = _getLayer("shipui");

	if(shipui == NULL)
	{
		log.elog("shipui is null");
		return NULL;
	}

	PyObject * slotsContainer  = _findByNameLayer(shipui, "slotsContainer");
	if(slotsContainer == NULL)
	{
		log.elog("doesn't have a slots container");
		Py_DECREF(shipui);
		return NULL;
	}

	PyObject * mod = _findByNameLayer(slotsContainer, module);
	if(mod == NULL)
	{
		log.elog("Mod not visible");
		Py_DECREF(shipui);
		Py_DECREF(slotsContainer);
		return NULL;
	}

	return mod;
}

char * Interfaces::_buildModule(PyObject * mod, string name, int & size)
{
	if(mod == NULL)
		return NULL;

	PyObject * height, * width, *absoluteTop, *absoluteLeft;

	height = _getHeight(mod);
	if(height == NULL)
	{
		log.elog("Couldn't get height");
		return NULL;
	}

	width = _getWidth(mod);
	if(width == NULL)
	{
		log.elog("Coudn't get width");
		Py_DECREF(height);
		return NULL;
	}
	absoluteTop = _getAbsoluteTop(mod);
	if(absoluteTop == NULL)
	{
		log.elog("Couldn't get absoluteTop");
		Py_DECREF(width);
		Py_DECREF(height);
		return NULL;
	}

	absoluteLeft = _getAbsoluteLeft(mod);
	if(absoluteLeft == NULL)
	{
		log.elog("Couldn't get absoluteLeft");
		Py_DECREF(width);
		Py_DECREF(height);
		Py_DECREF(absoluteTop);
		return NULL;		
	}

	char * output = builder.buildInterfaceObject(name, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteTop);
	Py_DECREF(absoluteLeft);
	return output;

}

PyObject * Interfaces::_GetInflightCargoView()
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("main is null");
		return NULL;
	}

	PyObject * children = _getAttribute(main, "children");
	if(children == NULL)
	{
		log.elog("children is null");
		Py_DECREF(main);
		return NULL;
	}

	int csize = PyObject_Size(children);

	PyObject * pvalue = NULL;
	PyObject * pkey = NULL;
	
	for(int i = 0; i < csize; i++)
	{
		pkey = PyInt_FromLong(i);
		pvalue = PyObject_GetItem(children, pkey);
		
		if(pvalue == NULL)
		{
			log.elog("Couldn't get the value");
			Py_DECREF(main);
			Py_DECREF(children);
			return NULL;
		}
		
		log.elog(PyEval_GetFuncName(pvalue));
		if(strcmp(PyEval_GetFuncName(pvalue), "InflightCargoView") == 0)
		{
			log.elog("Found cargoview");
			Py_DECREF(pvalue);
			return pvalue;
		}
		if(strcmp(PyEval_GetFuncName(pvalue), "DockedCargoView") == 0)
		{
			log.elog("Found docked cargoview");
			Py_DECREF(pvalue);
			return pvalue;
		}
		Py_DECREF(pvalue);
	}

	return NULL;
}


PyObject * Interfaces::_findType(string name, PyObject * children)
{
	int csize = PyObject_Size(children);

	PyObject * pvalue = NULL;
	PyObject * pkey = NULL;

	log.elog("inside findType");
	for(int i = 0; i < csize; i++)
	{
		pkey = PyInt_FromLong(i);
		pvalue = PyObject_GetItem(children, pkey);
	
		log.elog("iterating through neocom children");

		if(pvalue == NULL)
		{
			log.elog("Couldn't get the value");
			return NULL;
		}
		
		log.elog(PyEval_GetFuncName(pvalue));
		if(strcmp(PyEval_GetFuncName(pvalue), name.c_str()) == 0)
		{
			log.elog("Found " + name);
			return pvalue;
		}
		Py_DECREF(pvalue);
	}

	
	return NULL;
}


PyObject * Interfaces::_GetEntry(string entryname)
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("main is null");
		return NULL;
	}

	PyObject * children = _getAttribute(main, "children");
	if(children == NULL)
	{
		log.elog("children is null");
		Py_DECREF(main);
		return NULL;
	}

	int csize = PyObject_Size(children);

	PyObject * pvalue = NULL;
	PyObject * pkey = NULL;
	PyObject * name = NULL;
	PyObject * width = NULL;
	PyObject * height = NULL;
	PyObject * absoluteTop = NULL;
	PyObject * absoluteLeft = NULL;
	PyObject * entry = NULL;
	PyObject * sr = NULL;
	PyObject * node = NULL;
	
	for(int i = 0; i < csize; i++)
	{
		pkey = PyInt_FromLong(i);
		pvalue = PyObject_GetItem(children, pkey);
		
		if(pvalue == NULL)
		{
			log.elog("Couldn't get the value");
			Py_DECREF(main);
			Py_DECREF(children);
			return NULL;
		}
		
		log.elog(PyEval_GetFuncName(pvalue));
		if(strcmp(PyEval_GetFuncName(pvalue), "InflightCargoView") == 0)
		{
			log.elog("Found cargoview");
			entry = _findByNameLayer(pvalue, entryname);
			if(entry == NULL)
			{
				log.elog("cant find entry " + entryname);
				Py_DECREF(main);
				Py_DECREF(children);
				Py_DECREF(pvalue);
				return NULL;
			}
		}
		Py_DECREF(pvalue);
	}

	if(entry == NULL)
	{
		log.elog("doesn't have an entry " + entryname);
		Py_DECREF(main);
		Py_DECREF(children);
		return NULL;
	}

	Py_DECREF(main);
	Py_DECREF(children);
	return entry;

}

PyObject * Interfaces::_getNeocomButton(string buttonname)
{
	PyObject * layer = _getLayer("neocom");
	
	if(layer == NULL)
	{	
		return NULL;
	}
	
	PyObject * neocom = _findByNameLayer(layer, "neocom");
	if(neocom == NULL)
	{
		log.elog("Couldn't get neocom layer");
		Py_DECREF(layer);
		return NULL;
	}

	PyObject * maincontainer = _findByNameLayer(neocom, "maincontainer");
	if(maincontainer == NULL)
	{
		log.elog("Couldn't get maincontainer");
		Py_DECREF(layer);
		Py_DECREF(neocom);
		return NULL;
	}

	PyObject * button = _findByNameLayer(maincontainer, buttonname);
	if(button == NULL)
	{
		log.elog("couldn't get button");
		Py_DECREF(layer);
		Py_DECREF(neocom);
		Py_DECREF(maincontainer);
		return NULL;
	}

	PyObject * buttonchildren = _getAttribute(button, "children");
	if(buttonchildren == NULL)
	{
		log.elog("Couldn't get button's children");
		Py_DECREF(layer);
		Py_DECREF(neocom);
		Py_DECREF(maincontainer);
		Py_DECREF(button);
		return NULL;
	}

	PyObject * icon = _findType("EveIcon", buttonchildren);
	if(icon == NULL)
	{
		log.elog("Couldn't get the icon");
		Py_DECREF(layer);
		Py_DECREF(neocom);
		Py_DECREF(maincontainer);
		Py_DECREF(button);
		return NULL;
	}

	return icon;

}

char * Interfaces::GetShipHangar(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("main is null");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * shipHangar = _findByNameLayer(main, "shipHangar");
	if(shipHangar == NULL)
	{
		log.elog("shipHangar is null");
		Py_DECREF(main);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, *height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;

	bool ok = _populateAttributesDisplay(shipHangar, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		Py_DECREF(main);
		Py_DECREF(shipHangar);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("shipHangar", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(main);
	Py_DECREF(shipHangar);
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteLeft);
	Py_DECREF(absoluteTop);

	PyGILState_Release(gstate);
	return output;
		
}


char * Interfaces::GetStationHangar(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("main is null");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * stationHangar = _findByNameLayer(main, "hangarFloor");
	if(stationHangar == NULL)
	{
		log.elog("stationHangar is null");
		Py_DECREF(main);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, *height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;

	bool ok = _populateAttributesDisplay(stationHangar, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		Py_DECREF(main);
		Py_DECREF(stationHangar);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("stationHangar", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(main);
	Py_DECREF(stationHangar);
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteLeft);
	Py_DECREF(absoluteTop);

	PyGILState_Release(gstate);
	return output;
		
}


bool Interfaces::_populateAttributesDisplay(PyObject * item, PyObject ** width, PyObject ** height, PyObject ** absoluteTop, PyObject ** absoluteLeft)
{
			*width = _getAttribute(item, "displayWidth");
			if(*width == NULL)
			{
				log.elog("Couldn't get width");
				return false;
			}
			
			*height = _getAttribute(item, "displayHeight");
			if(*height == NULL)
			{
				log.elog("Couldn't get height");
				Py_DECREF(width);
				return false;
			}
			
			*absoluteLeft = _getAbsoluteLeft(item);
			if(*absoluteLeft == NULL)
			{
				log.elog("Couldn't get absoluteLeft");
				Py_DECREF(height);
				Py_DECREF(width);
				return false;
			}

			*absoluteTop = _getAbsoluteTop(item);
			if(*absoluteTop == NULL)
			{
				log.elog("Couldn't get absoluteTop");
				Py_DECREF(height);
				Py_DECREF(width);
				Py_DECREF(absoluteLeft);
				return false;
			}


			return true;
}



bool Interfaces::_populateAttributes(PyObject * item, PyObject ** width, PyObject ** height, PyObject ** absoluteTop, PyObject ** absoluteLeft)
{
			*width = _getWidth(item);
			if(*width == NULL)
			{
				log.elog("Couldn't get width");
				return false;
			}
			
			*height = _getHeight(item);
			if(*height == NULL)
			{
				log.elog("Couldn't get height");
				Py_DECREF(width);
				return false;
			}
			
			*absoluteLeft = _getAbsoluteLeft(item);
			if(*absoluteLeft == NULL)
			{
				log.elog("Couldn't get absoluteLeft");
				Py_DECREF(height);
				Py_DECREF(width);
				return false;
			}

			*absoluteTop = _getAbsoluteTop(item);
			if(*absoluteTop == NULL)
			{
				log.elog("Couldn't get absoluteTop");
				Py_DECREF(height);
				Py_DECREF(width);
				Py_DECREF(absoluteLeft);
				return false;
			}


			return true;
}


char * Interfaces::GetUndockButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * buttonIcon = _getNeocomButton("undock");
	if(buttonIcon == NULL)
	{
		log.elog("Couldn't find the icon");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	bool ok = _populateAttributesDisplay(buttonIcon, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't get attributes");
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildInterfaceObject("UndockButton", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteTop);
	Py_DECREF(absoluteLeft);
	Py_DECREF(buttonIcon);
	
	PyGILState_Release(gstate);
	return output;
}


char * Interfaces::GetStationItemsButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * buttonIcon = _getNeocomButton("items");
	if(buttonIcon == NULL)
	{
		log.elog("Couldn't find the icon");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	bool ok = _populateAttributesDisplay(buttonIcon, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't get attributes");
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildInterfaceObject("ItemsButton", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteTop);
	Py_DECREF(absoluteLeft);
	Py_DECREF(buttonIcon);
	
	PyGILState_Release(gstate);
	return output;
}


char * Interfaces::GetCargoList(int & size)
{
	list<ObjectBuilder::itemEntry *> labels;
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * cargoView = _GetInflightCargoView();
	if(cargoView == NULL)
	{
		log.elog("Couldn't get cargoview");
		PyGILState_Release(gstate);
		return NULL;;
	}

	PyObject * entry = _findByNameLayer(cargoView, "entry_0");
	stringstream os;
	os << "entry_";


	for(int i = 1; entry != NULL; i++)
	{
		_IterateThroughEntryAndBuild(entry, labels);
		os.str("");
		os << "entry_" << i;

		log.elog(os.str());
		entry = _findByNameLayer(cargoView, os.str());
	}

	PyGILState_Release(gstate);
	char * output = builder.buildItemObject(labels, size);
	for(list<ObjectBuilder::itemEntry *>::iterator it = labels.begin(); it != labels.end(); it++)
	{
		delete (*it);
	}
	return output;

}

void Interfaces::_IterateThroughEntryAndBuild(PyObject * entry, list<ObjectBuilder::itemEntry *> & labels)
{	

	PyObject * entry_children = _getAttribute(entry, "children");
	if(entry_children == NULL)
	{
		log.elog("couldn't get entry children");
		return;
	}


	PyObject * pkey = NULL, * pvalue = NULL, *sr = NULL, *node = NULL, *name = NULL, *height = NULL;
	PyObject * absoluteTop = NULL, *absoluteLeft = NULL, *width = NULL, * sort_qty = NULL, * volume = NULL, * meta = NULL;
	int csize = PyObject_Size(entry_children);
	for(int i = 0; i < csize; i++)
	{
		pkey = PyInt_FromLong(i);
		pvalue = PyObject_GetItem(entry_children, pkey);
		if(pvalue == NULL)
		{
			log.elog("pvalue is null");
			Py_DECREF(entry_children);
			return;
		}

		sr = _getAttribute(pvalue, "sr");
		if(sr == NULL)
		{
			log.elog("sr null");
			Py_DECREF(pvalue);
			Py_DECREF(entry_children);
			return;
		}

		node = _getAttribute(sr, "node");
		if(node == NULL)
		{
			log.elog("node is null");
			Py_DECREF(pvalue);
			Py_DECREF(sr);
			Py_DECREF(entry_children);
			return;
		}

		name = _getAttribute(node, "name");
		if(name == NULL)
		{
			log.elog("couldn't get the name");
			Py_DECREF(pvalue);
			Py_DECREF(sr);
			Py_DECREF(node);
			Py_DECREF(entry_children);
			return;
		}
		//Got name, get absoluteTop, absoluteLeft, width, height
		absoluteTop = _getAttribute(pvalue, "absoluteTop");
		if(absoluteTop == NULL)
		{
			log.elog("Couldn't get absoluteTop");
			Py_DECREF(pvalue);
			Py_DECREF(sr);
			Py_DECREF(node);
			Py_DECREF(name);
			Py_DECREF(entry_children);
			return;
		}

		absoluteLeft = _getAttribute(pvalue, "absoluteLeft");
		if(absoluteLeft == NULL)
		{
			log.elog("Couldnt' get absoluteleft");
			Py_DECREF(pvalue);
			Py_DECREF(sr);
			Py_DECREF(node);
			Py_DECREF(name);
			Py_DECREF(absoluteTop);
			Py_DECREF(entry_children);
			return;
		}

		width = _getAttribute(pvalue, "width");
		if(width == NULL)
		{
			log.elog("Couldn't get width");
			Py_DECREF(name);
			Py_DECREF(pvalue);
			Py_DECREF(sr);
			Py_DECREF(node);
			Py_DECREF(absoluteTop);
			Py_DECREF(absoluteLeft);
			Py_DECREF(entry_children);
			return;
		}

		height = _getAttribute(pvalue, "height");
		if(height == NULL)
		{
			log.elog("Couldn't get height");
			Py_DECREF(name);
			Py_DECREF(pvalue);
			Py_DECREF(sr);
			Py_DECREF(node);
			Py_DECREF(absoluteTop);
			Py_DECREF(absoluteLeft);
			Py_DECREF(width);
			Py_DECREF(entry_children);
			return;
		}

		sort_qty = _getAttribute(node, "sort_qty");
		if(sort_qty == NULL)
		{
			log.elog("Couldn't get sort_qty");
			Py_DECREF(name);
			Py_DECREF(pvalue);
			Py_DECREF(sr);
			Py_DECREF(node);
			Py_DECREF(absoluteTop);
			Py_DECREF(absoluteLeft);
			Py_DECREF(width);
			Py_DECREF(entry_children);
			Py_DECREF(height);
			return;			
		}

		volume = _getAttribute(node, "volume");
		if(volume == NULL)
		{
			log.elog("Couldn't get volume");
			Py_DECREF(name);
			Py_DECREF(pvalue);
			Py_DECREF(sr);
			Py_DECREF(node);
			Py_DECREF(absoluteTop);
			Py_DECREF(absoluteLeft);
			Py_DECREF(width);
			Py_DECREF(entry_children);
			Py_DECREF(height);
			Py_DECREF(sort_qty);
			return;			
		}
		
		meta = _getAttribute(node, "meta");
		if(meta == NULL)
		{
			log.elog("Couldn't get meta");
			Py_DECREF(name);
			Py_DECREF(pvalue);
			Py_DECREF(sr);
			Py_DECREF(node);
			Py_DECREF(absoluteTop);
			Py_DECREF(absoluteLeft);
			Py_DECREF(width);
			Py_DECREF(entry_children);
			Py_DECREF(height);
			Py_DECREF(sort_qty);
			Py_DECREF(volume);
			return;			
		}


		
		ObjectBuilder::itemEntry * over = new ObjectBuilder::itemEntry();
		if(PyObject_Not(meta))
		{
			log.elog("Meta is blank");
			over->meta = "0";
		}
		else
		{
			over->meta = PyString_AsString(meta);
		}

		over->quantity = PyInt_AsLong(sort_qty);
		over->volume = PyString_AsString(volume);

		over->name = PyString_AsString(name);
		over->topLeftX = PyInt_AsLong(absoluteLeft);
		over->topLeftY = PyInt_AsLong(absoluteTop);
		over->width = PyInt_AsLong(width);
		over->height = PyInt_AsLong(height);
		
		labels.push_back(over);
		Py_DECREF(pvalue);
		Py_DECREF(height);
		Py_DECREF(name);
		Py_DECREF(sr);
		Py_DECREF(node);
		Py_DECREF(absoluteLeft);
		Py_DECREF(absoluteTop);
		Py_DECREF(width);
		Py_DECREF(sort_qty);
		Py_DECREF(volume);

	}///end of for

	Py_DECREF(entry_children);
		
}

char * Interfaces::IsHighSlotActive(int number, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	switch(number)
	{
		case 1:
		{
			output = _isModuleActive("inFlightHighSlot1", size);
			break;
		}
		case 2:
		{
			output = _isModuleActive("inFlightHighSlot2", size);
			break;
		}
		case 3:
		{
			output = _isModuleActive("inFlightHighSlot3", size);
			break;
		}
		case 4:
		{
			output = _isModuleActive("inFlightHighSlot4", size);
			break;
		}
		case 5:
		{
			output = _isModuleActive("inFlightHighSlot5", size);
			break;
		}
		case 6:
		{
			output = _isModuleActive("inFlightHighSlot6", size);
			break;
		}
		case 7:
		{
			output = _isModuleActive("inFlightHighSlot7", size);
			break;
		}
		case 8:
		{
			output = _isModuleActive("inFlightHighSlot8", size);
			break;
		}
		case 9:
		{
			output = _isModuleActive("inFlightHighSlot9", size);
			break;
		}
	}

	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::_isModuleActive(string name, int & size)
{
	PyObject * module = _findModule(name);
	bool isActive = false;
	if(module == NULL)
	{
		log.elog("Couldn't find the module");
		return NULL;
	}

	PyObject * sr = _getAttribute(module, "sr");
	if(sr == NULL)
	{
		log.elog("Couldn't get sr");
		Py_DECREF(module);
		return NULL;
	}
	PyObject * srmodule = _getAttribute(sr, "module");
	if(srmodule == NULL)
	{
		log.elog("Couldn't find srmodule");
		Py_DECREF(module);
		Py_DECREF(sr);
		return NULL;
	}

	PyObject * sragain = _getAttribute(srmodule, "sr");
	if(sragain == NULL)
	{
		log.elog("sragain is null");
		Py_DECREF(module);
		Py_DECREF(sr);
		Py_DECREF(srmodule);
		return NULL;
	}

	PyObject * glow = _getAttribute(sragain, "glow");
	if(glow == NULL)
	{
		log.elog("Can't get the glow");
		Py_DECREF(module);
		Py_DECREF(sr);
		Py_DECREF(srmodule);
		Py_DECREF(sragain);
		return NULL;
	}

	PyObject * state = _getAttribute(glow, "state");
	if(state == NULL)
	{
		log.elog("Couldn't get state");
		Py_DECREF(module);
		Py_DECREF(sr);
		Py_DECREF(srmodule);
		Py_DECREF(sragain);
		Py_DECREF(glow);
		return NULL;
	}

	int istate = PyInt_AsLong(state);
	if(istate == 1)
	{
		isActive = true;
	}

	log.elog("Outputing isActive");
	char * output = builder.buildBooleanObject(isActive, size);
	Py_DECREF(module);
	Py_DECREF(sr);
	Py_DECREF(srmodule);
	Py_DECREF(sragain);
	Py_DECREF(glow);
	Py_DECREF(state);
	return output;


}

char * Interfaces::_GetSlot(string name, string  outputname, int & size)
{
	PyObject * mod = _findModule(name);
	if(mod == NULL)
	{
		log.elog("No module found");
		return NULL;
	}
	char * output = _buildModule(mod, outputname, size);
	Py_DECREF(mod);
	return output;
}

char * Interfaces::GetHighSlot(int number, int & size)
{
	char * output = NULL;
	PyGILState_STATE gstate = PyGILState_Ensure();
	switch(number)
	{
		case 1:
		{
			output = _GetSlot("inFlightHighSlot1", "FirstHighSlot", size);
			break;
		}
		case 2:
		{
			output = _GetSlot("inFlightHighSlot2", "SecondHighSlot", size);
			break;
		}
		case 3:
		{
			output = _GetSlot("inFlightHighSlot3", "ThirdHighSlot", size);
			break;
		}
		case 4:
		{
			output = _GetSlot("inFlightHighSlot4", "FourthHighSlot", size);
			break;
		}
		case 5:
		{
			output = _GetSlot("inFlightHighSlot5", "FifthHighSlot", size);
			break;
		}
		case 6:
		{
			output = _GetSlot("inFlightHighSlot6", "SixthHighSlot", size);
			break;
		}
		case 7:
		{
			output = _GetSlot("inFlightHighSlot7", "SeventhHighSlot", size);
			break;
		}
		case 8:
		{
			output = _GetSlot("inFlightHighSlot8", "EigthHighSlot", size);
			break;
		}
		case 9:
		{
			output = _GetSlot("inFlightHighSlot9", "NinenthHighSlot", size);
			break;
		}
	}
	PyGILState_Release(gstate);
	return output;
}


char * Interfaces::GetSelectedItem(int & size)
{
	
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");

	if(main == NULL)
	{
		log.elog("main is null");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * selectedItemView = _findByNameLayer(main, "selecteditemview");
	if(selectedItemView == NULL)
	{
		log.elog("selectedItemView is null");
		Py_DECREF(main);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * maincontainer = _findByNameLayer(selectedItemView, "__maincontainer");
	if(maincontainer == NULL)
	{
		log.elog("__maincontainer is null");
		Py_DECREF(main);
		Py_DECREF(selectedItemView);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * mainitem = _findByNameLayer(maincontainer, "main");
	if(mainitem == NULL)
	{
		log.elog("mainitem is null");
		Py_DECREF(main);
		Py_DECREF(selectedItemView);
		Py_DECREF(maincontainer);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * toparea = _findByNameLayer(mainitem, "toparea");
	if(toparea == NULL)
	{
		log.elog("toparea is null");
		Py_DECREF(main);
		Py_DECREF(selectedItemView);
		Py_DECREF(maincontainer);
		Py_DECREF(mainitem);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * text = _findByNameLayer(toparea, "text");
	if(text == NULL)
	{
		log.elog("text is null");
		Py_DECREF(main);
		Py_DECREF(selectedItemView);
		Py_DECREF(maincontainer);
		Py_DECREF(mainitem);
		Py_DECREF(toparea);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * result = _getAttribute(text, "text");
	if(result == NULL)
	{
		log.elog("Couldn't get the text attribute");
		Py_DECREF(main);
		Py_DECREF(selectedItemView);
		Py_DECREF(maincontainer);
		Py_DECREF(mainitem);
		Py_DECREF(toparea);
		Py_DECREF(text);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * label = PyString_AsString(result);
	if(label == NULL)
	{
		log.elog("Couldn't turn label into string");
		Py_DECREF(main);
		Py_DECREF(selectedItemView);
		Py_DECREF(maincontainer);
		Py_DECREF(mainitem);
		Py_DECREF(toparea);
		Py_DECREF(text);
		Py_DECREF(label);
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildInterfaceObject(label,  0 ,0, 0, 0, size);

	Py_DECREF(main);
	Py_DECREF(selectedItemView);
	Py_DECREF(maincontainer);
	Py_DECREF(mainitem);
	Py_DECREF(toparea);
	Py_DECREF(text);
	Py_DECREF(label);
	PyGILState_Release(gstate);
	return output;
}


char * Interfaces::GetTargetList(int & size)
{
	list<ObjectBuilder::targetEntry *> targets;
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * target = _getLayer("target");
	if(target == NULL)
	{
		log.elog("Can't get the target layer");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * children = _getAttribute(target, "children");
	if(children == NULL)
	{
		log.elog("Target has no children");
		Py_DECREF(target);
		PyGILState_Release(gstate);
		return NULL;
	}
	
	int len = PyObject_Size(children);

	if(len < 1)
	{
		log.elog("Target has no children");
		Py_DECREF(children);
		Py_DECREF(target);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * pkey, *pvalue;
	const char * fname;
	for(int i = 0; i < len; i++)
	{
		pkey = PyInt_FromLong(i);
		pvalue = PyObject_GetItem(children, pkey);
		if(pvalue == NULL)
		{
			log.elog("Couldn't get the child value");
			Py_DECREF(children);
			Py_DECREF(target);
			PyGILState_Release(gstate);
			return NULL;
		}
		
		fname = PyEval_GetFuncName(pvalue);
		if(fname == NULL)
		{
			log.elog("Couldn't get type name");
			Py_DECREF(children);
			Py_DECREF(target);
			Py_DECREF(pvalue);
			PyGILState_Release(gstate);
			return NULL;
		}
		
		if(strcmp(fname, "Target") == 0)
		{
			log.elog("Found target object");
			PyObject * label, * width, * height, *absoluteLeft, *absoluteTop;
			char * text;
			label = _getAttribute(pvalue, "label");
			if(label == NULL)
			{
				log.elog("Couldn't pull the label attribute off the target");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				PyGILState_Release(gstate);
				return NULL;
			}
			
			text = PyString_AsString(label);
			if(text == NULL)
			{
				log.elog("Couldn't pull the text off the label");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				Py_DECREF(label);
				PyGILState_Release(gstate);
				return NULL;
			}
			
			width = _getWidth(pvalue);
			if(width == NULL)
			{
				log.elog("Couldn't get width");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				Py_DECREF(label);
				PyGILState_Release(gstate);
				return NULL;
			}
			
			height = _getHeight(pvalue);
			if(height == NULL)
			{
				log.elog("Couldn't get height");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				Py_DECREF(label);
				Py_DECREF(width);
				PyGILState_Release(gstate);
				return NULL;
			}
			
			absoluteLeft = _getAbsoluteLeft(pvalue);
			if(absoluteLeft == NULL)
			{
				log.elog("Couldn't get absoluteLeft");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				Py_DECREF(label);
				Py_DECREF(height);
				Py_DECREF(width);
				PyGILState_Release(gstate);
				return NULL;
			}

			absoluteTop = _getAbsoluteTop(pvalue);
			if(absoluteTop == NULL)
			{
				log.elog("Couldn't get absoluteTop");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				Py_DECREF(label);
				Py_DECREF(height);
				Py_DECREF(width);
				Py_DECREF(absoluteLeft);
				PyGILState_Release(gstate);
				return NULL;
			}
			ObjectBuilder::targetEntry * tEntry = new ObjectBuilder::targetEntry();
			tEntry->name = text;
			tEntry->height = PyInt_AsLong(height);
			tEntry->width = PyInt_AsLong(width);
			tEntry->topLeftX = PyInt_AsLong(absoluteLeft);
			tEntry->topLeftY = PyInt_AsLong(absoluteTop);
			targets.push_back(tEntry);
			log.elog("adding target entry");
			Py_DECREF(pvalue);
			Py_DECREF(label);
			Py_DECREF(height);
			Py_DECREF(width);
			Py_DECREF(absoluteLeft);
			Py_DECREF(absoluteTop);
			
		}		
		else
		{
			log.elog("Not a target type");
		}

	}
	log.elog("Building target object");
	char * output = builder.buildTargetObject(targets, size);

	for(list<ObjectBuilder::targetEntry *>::iterator it = targets.begin(); it != targets.end(); it++)
	{
		delete (*it);
	}

	Py_DECREF(target);
	Py_DECREF(children);
	PyGILState_Release(gstate);
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
		log.elog("Interface is null");
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
			if(PyObject_Not(soughtInterface))
			{
				log.elog("FindChild returned blank");
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
		log.elog("Interface has no children");
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
				string check(ctext);
				if(check.find(text) != check.npos)
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
			return NULL;
		}
	}
	else
	{
		log.elog("Doesn't have attribute " + attr);
		return NULL;
	}
	return attribute;
}


