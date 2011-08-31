#include "stdafx.h"
#include "Interfaces.h"
#include <List>
#include "ObjectBuilder.h"
#include <sstream>
#include <iostream>
#include <string>

///_functions should never aquire the GIL, its the caller's responsibility to do so.

//NOtes

/*
scrollHeaders container has the name, ic, distance etc of the overview

*/


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

PyObject * Interfaces::_getLoginItem(string name)
{
	PyObject * login = _getLayer("login");
	if(login == NULL)
	{
		log.elog("Couldn't get login");
		return NULL;
	}

	PyObject * item = _findByNameLayer(login, name);
	if(item == NULL)
	{
		log.elog("Couldn't get item");
		Py_DECREF(login);
		return NULL;
	}

	Py_DECREF(login);
	return item;
}

char * Interfaces::_getLoginBoxesWithText(string name, int & size)
{
	PyObject * item = _getLoginItem(name);
	if(item == NULL)
	{
		log.elog("Couldn't get username");
		return NULL;
	}
	
	PyObject * text = _getAttribute(item, "text");
	if(text == NULL)
	{
		log.elog("couldn't get text");
		Py_DECREF(item);
		return NULL;
	}
	
	PyObject * height = NULL, * width = NULL, *absoluteTop = NULL, *absoluteLeft=NULL;

	bool ok = _populateAttributes(item, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("couldn't populate");
		Py_DECREF(item);
		Py_DECREF(text);
		return NULL;
	}

	char * output = builder.buildInterfaceObject(PyString_AsString(text), PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(item);
	Py_DECREF(text);
	Py_DECREF(height);
	Py_DECREF(width);
	Py_DECREF(absoluteLeft);
	Py_DECREF(absoluteTop);
	return output;
}

char * Interfaces::getUserNameBox(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getLoginBoxesWithText("username", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::getPasswordBox(int & size)
{	
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getLoginBoxesWithText("password", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::getEnterButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * charsel = _getLayer("charsel");
	if(charsel == NULL)
	{
		log.elog("char selection is null");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * enterButton = _findByNameLayer(charsel, "enterBtn");
	if(enterButton == NULL)
	{
		log.elog("couldn't find the enter");
		Py_DECREF(charsel);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, *height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;

	bool ok = _populateAttributes(enterButton, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate correctly");
		Py_DECREF(charsel);
		Py_DECREF(enterButton);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("EnterButton", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(charsel);
	Py_DECREF(enterButton);
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteTop);
	Py_DECREF(absoluteLeft);
	PyGILState_Release(gstate);
	return output;

}


char * Interfaces::isAtCharSel(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	PyObject * charsel = _getLayer("charsel");
	if(charsel == NULL)
	{
		log.elog("char selection is null");
		output = builder.buildBooleanObject(false, size);
		PyGILState_Release(gstate);
		return output;
	}
	
	PyObject * isopen = _getAttribute(charsel, "isopen");
	if(isopen == NULL)
	{
		log.elog("isopen is null");
		output = builder.buildBooleanObject(false, size);
		Py_DECREF(charsel);
		PyGILState_Release(gstate);
		return output;
	}
	
	bool open = false;

	if(PyObject_IsTrue(isopen))
	{
		open = true;
	}
	
	output = builder.buildBooleanObject(open, size);
	Py_DECREF(charsel);
	Py_DECREF(isopen);
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::getConnectButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getLoginBoxesWithText("button", size);
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

char * Interfaces::GetServerMessage(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * abovemain = _getLayer("abovemain");
	if(abovemain == NULL)
	{
		log.elog("couldn't get abovemain");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * message = _findByNameLayer(abovemain, "message");
	if(message == NULL)
	{
		log.elog("Couldn't get message");
		Py_DECREF(abovemain);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * messageattr = _getAttribute(message, "message");
	if(messageattr == NULL)
	{
		log.elog("couldn't get message attr");
		Py_DECREF(abovemain);
		Py_DECREF(message);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * text = _getAttribute(messageattr, "text");
	if(text == NULL)
	{
		log.elog("couldn't get text");
		Py_DECREF(abovemain);
		Py_DECREF(message);
		Py_DECREF(messageattr);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildStringObject(PyString_AsString(text), size);
	Py_DECREF(abovemain);
	Py_DECREF(message);
	Py_DECREF(messageattr);
	Py_DECREF(text);
	PyGILState_Release(gstate);
	return output;
}


char * Interfaces::IsLoading(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	bool isTrue = false;
	PyObject * loading = _getLayer("loading");
	if(loading == NULL)
	{
		log.elog("Can't get loading");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * progresswindow = _findByNameLayer(loading, "progresswindow");
	if(progresswindow == NULL)
	{
		log.elog("can't get the window");
		Py_DECREF(loading);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * sr = _getAttribute(progresswindow, "sr");
	if(sr == NULL)
	{
		log.elog("can't get sr");
		Py_DECREF(loading);
		Py_DECREF(progresswindow);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * tickTimer = _getAttribute(sr, "tickTimer");
	if(tickTimer == NULL)
	{
		log.elog("can't get the timer");
		Py_DECREF(loading);
		Py_DECREF(progresswindow);
		Py_DECREF(sr);
		PyGILState_Release(gstate);
		return NULL;
	}

	if(PyObject_IsTrue(tickTimer))
	{
		isTrue = true;
	}
	else
		isTrue  = false;

	char * output = builder.buildBooleanObject(isTrue, size);
	Py_DECREF(loading);
	Py_DECREF(progresswindow);
	Py_DECREF(sr);
	Py_DECREF(tickTimer);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetModalCancelButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getModalButton("Cancel_Btn", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetSystemInformation(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * neocom = _getLayer("neocom");
	if(neocom == NULL)
	{
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * neocomLeftSide = _findByNameLayer(neocom, "neocomLeftside");
	if(neocomLeftSide == NULL)
	{
		log.elog("Couldn't get the left side");
		Py_DECREF(neocom);
		PyGILState_Release(gstate);
		return NULL;
	}

	
	PyObject * locationInfo = _findByNameLayer(neocomLeftSide,  "locationInfo");
	if(locationInfo == NULL)
	{
		log.elog("locationInfo is null");
		Py_DECREF(neocom);
		Py_DECREF(neocomLeftSide);
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * caption = _findByNameLayer(locationInfo, "caption");
	if(caption == NULL)
	{
		log.elog("caption is null");
		Py_DECREF(neocom);
		Py_DECREF(locationInfo);
		Py_DECREF(neocomLeftSide);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * locationTextobj = _findByNameLayer(locationInfo, "locationText");
	if(locationTextobj == NULL)
	{
		log.elog("locationText is null");
		Py_DECREF(neocom);
		Py_DECREF(locationInfo);
		Py_DECREF(caption);
		Py_DECREF(neocomLeftSide);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * locationText = _getAttribute(locationTextobj, "text");
	if(locationText == NULL)
	{
		log.elog("Couldn't get the text");
		Py_DECREF(neocom);
		Py_DECREF(locationInfo);
		Py_DECREF(caption);
		Py_DECREF(neocomLeftSide);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * captionText = _getAttribute(caption, "text");
	if(captionText == NULL)
	{
		log.elog("couldn't get the caption");
		Py_DECREF(neocom);
		Py_DECREF(locationInfo);
		Py_DECREF(caption);
		Py_DECREF(neocomLeftSide);
		Py_DECREF(locationText);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildSolarSystemObject(PyString_AsString(captionText), PyString_AsString(locationText), size);
	Py_DECREF(neocom);
	Py_DECREF(locationInfo);
	Py_DECREF(caption);
	Py_DECREF(locationTextobj);
	Py_DECREF(neocomLeftSide);
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::_getDroneStatus(int & size)
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		return NULL;
	}
	PyObject * droneChildren = _findByNameLayer(main, "droneview");
	
	if(droneChildren == NULL)
	{
		log.elog("Couldn't get children");
		Py_DECREF(main);
		return NULL;
	}

	string entryTxt = "entry_1";
	int i = 0;
	stringstream os;

entry_lab:
	PyObject * entry = _findByNameLayer(droneChildren, entryTxt);
	if(entry == NULL)
	{
		if(i < 10)
		{
			i++;
			os << "entry_" << i;
			entryTxt = os.str();
			os.str("");
			goto entry_lab;
		}
		log.elog("cOuldn't get the entry");
		Py_DECREF(main);
		Py_DECREF(droneChildren);
		return NULL;
	}

	PyObject * label = _findByNameLayer(entry, "text");
	if(label == NULL)
	{
		log.elog("Couldn't get label");
		Py_DECREF(main);
		Py_DECREF(droneChildren);
		Py_DECREF(entry);
		return NULL;
	}

	PyObject * text = _getAttribute(label,"text");
	if(text == NULL)
	{
		Py_DECREF(main);
		Py_DECREF(droneChildren);
		Py_DECREF(label);
		Py_DECREF(entry);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	bool ok = _populateAttributes(label, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate attributes");
		Py_DECREF(main);
		Py_DECREF(droneChildren);
		Py_DECREF(label);
		Py_DECREF(text);
		Py_DECREF(entry);
		return NULL;
	}

	char * ctext = PyString_AsString(text);
	char * output = builder.buildInterfaceObject(ctext, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(main);
	Py_DECREF(droneChildren);
	Py_DECREF(label);
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteTop);
	Py_DECREF(absoluteLeft);
	Py_DECREF(text);
	Py_DECREF(entry);

	return output;
}

char * Interfaces::CheckDroneStatus(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getDroneStatus(size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::_getDroneLabel(int type, int & size)
{
	char * droneType;
	if(type == 0)
		droneType = "droneOverviewDronesinbay";
	else if(type == 1)
		droneType = "droneOverviewDronesinlocalspace";
	
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		return NULL;
	}
	PyObject * droneChildren = _findByNameLayer(main, droneType);
	
	if(droneChildren == NULL)
	{
		log.elog("Couldn't get children");
		Py_DECREF(main);
		return NULL;
	}

	PyObject * label = _findByNameLayer(droneChildren, "text");
	if(label == NULL)
	{
		log.elog("Couldn't get label");
		Py_DECREF(main);
		Py_DECREF(droneChildren);
		return NULL;
	}

	PyObject * text = _getAttribute(label,"text");
	if(text == NULL)
	{
		Py_DECREF(main);
		Py_DECREF(droneChildren);
		Py_DECREF(label);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	bool ok = _populateAttributes(label, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate attributes");
		Py_DECREF(main);
		Py_DECREF(droneChildren);
		Py_DECREF(label);
		Py_DECREF(text);
		return NULL;
	}

	char * ctext = PyString_AsString(text);
	char * output = builder.buildInterfaceObject(ctext, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(main);
	Py_DECREF(droneChildren);
	Py_DECREF(label);
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteTop);
	Py_DECREF(absoluteLeft);
	Py_DECREF(text);

	return output;
}

char * Interfaces::DronesInFlight(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getDroneLabel(1, size);
	PyGILState_Release(gstate);
	return output;
}
char * Interfaces::DronesInBay(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getDroneLabel(0, size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::IsFleeted(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		PyGILState_Release(gstate);
		return NULL;
	}

	bool fleeted = false;
	PyObject * fleetwindow = _findByNameLayer(main, "fleetwindow");
	if(fleetwindow != NULL)
	{
		fleeted = true;
	}
	char * output = builder.buildBooleanObject(fleeted, size);
	Py_DECREF(main);
	if(fleetwindow != NULL)
		Py_DECREF(fleetwindow);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::FindPlayerInLocal(string name, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * local = _findByNameLayer(main, "chatchannel_solarsystemid2");
	if(local == NULL)
	{
		log.elog("couldn't get local");
		Py_DECREF(main);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * entry = _findByNameLayer(local, "entry_0");
	stringstream os;
	os << "entry_";

	for(int i = 1; i<20; i++)
	{
		
		if(entry == NULL)
		{
			os.str("");
			os << "entry_";
			os << i;
			entry = _findByNameLayer(local, os.str());
			continue;
		}

		log.elog(PyEval_GetFuncName(entry));

		if(strcmp(PyEval_GetFuncName(entry), "SE_EditTextlineCore") == 0)
		{
			Py_DECREF(entry);
			os.str("");
			os << "entry_";
			os << i;
			entry = _findByNameLayer(local, os.str());
			continue;
		}

		os.str("");
		os << "entry_" << i;
		PyObject * sr = _getAttribute(entry, "sr");
		if(sr == NULL)
		{
			log.elog("No sr");
			Py_DECREF(main);
			Py_DECREF(local);
			Py_DECREF(entry);
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * node = _getAttribute(sr, "node");
		if(node == NULL)
		{
			log.elog("no node");
			Py_DECREF(main);
			Py_DECREF(local);
			Py_DECREF(entry);
			Py_DECREF(sr);
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * label = _getAttribute(node, "label");
		if(label == NULL)
		{
			log.elog("no label");
			Py_DECREF(main);
			Py_DECREF(local);
			Py_DECREF(entry);
			Py_DECREF(sr);
			Py_DECREF(node);
			PyGILState_Release(gstate);
			return NULL;
		}

		log.elog(PyString_AsString(label));
		if(strcmp(PyString_AsString(label), name.c_str()) == 0)
		{
			log.elog("Found ");
			log.elog(name);
			PyObject * state = _getAttribute(entry, "state");
			if(state == NULL)
			{
				log.elog("Couldn't get the state");
				Py_DECREF(main);
				Py_DECREF(local);
				Py_DECREF(entry);
				Py_DECREF(sr);
				Py_DECREF(node);
				Py_DECREF(label);
				PyGILState_Release(gstate);
				return NULL;
			}
			
			PyObject * width = NULL, * height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
			bool ok = _populateAttributes(entry, &width, &height, &absoluteTop, &absoluteLeft);
			if(!ok)
			{
				log.elog("issue populating");
				Py_DECREF(main);
				Py_DECREF(local);
				Py_DECREF(entry);
				Py_DECREF(sr);
				Py_DECREF(node);
				Py_DECREF(label);
				Py_DECREF(state);
				PyGILState_Release(gstate);
				return NULL;
			}
			
			int x = 0, y = 0;

			if(PyInt_AsLong(state) == 0)
			{
				x = PyInt_AsLong(absoluteLeft);
				y = PyInt_AsLong(absoluteTop);
			}

			char * output = builder.buildInterfaceObject(PyString_AsString(label), x, y, PyInt_AsLong(width), PyInt_AsLong(height), size);
			Py_DECREF(main);
			Py_DECREF(local);
			Py_DECREF(entry);
			Py_DECREF(sr);
			Py_DECREF(node);
			Py_DECREF(label);
			Py_DECREF(state);
			Py_DECREF(absoluteTop);
			Py_DECREF(absoluteLeft);
			Py_DECREF(width);
			Py_DECREF(height);
			PyGILState_Release(gstate);
			return output;
		}
		
		Py_DECREF(entry);
		Py_DECREF(sr);
		Py_DECREF(node);
		Py_DECREF(label);

		log.elog(os.str());
		entry = _findByNameLayer(local, os.str());
	}
	
	Py_DECREF(main);
	Py_DECREF(local);
	PyGILState_Release(gstate);
	return NULL;
}

char * Interfaces::CheckLocal(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * local = _findByNameLayer(main, "chatchannel_solarsystemid2");
	if(local == NULL)
	{
		log.elog("couldn't get local");
		Py_DECREF(main);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * entry = _findByNameLayer(local, "entry_0");
	stringstream os;
	os << "entry_";

	for(int i = 1; entry != NULL; i++)
	{
		os.str("");
		os << "entry_" << i;
		PyObject * flag = _findByNameLayer(entry, "flag");
		if(flag == NULL)
		{
			log.elog("No flag");
			Py_DECREF(main);
			Py_DECREF(local);
			Py_DECREF(entry);
			PyGILState_Release(gstate);
			return builder.buildBooleanObject(true, size);
		}
		
		PyObject * fill = _findByNameLayer(flag, "fill");
		if(fill == NULL)
		{
			log.elog("fill is null");
			Py_DECREF(main);
			Py_DECREF(local);
			Py_DECREF(entry);
			Py_DECREF(flag);
			PyGILState_Release(gstate);
			return builder.buildBooleanObject(true, size);
		}

		PyObject * color  = _getAttribute(fill, "color");
		if(color == NULL)
		{
			log.elog("color is null");
			Py_DECREF(main);
			Py_DECREF(local);
			Py_DECREF(entry);
			Py_DECREF(fill);
			Py_DECREF(flag);
			PyGILState_Release(gstate);
			return builder.buildBooleanObject(true, size);
		}
		
		PyObject * g = _getAttribute(color, "g");
		if(g == NULL)
		{
			log.elog("g is null");
			Py_DECREF(main);
			Py_DECREF(local);
			Py_DECREF(entry);
			Py_DECREF(fill);
			Py_DECREF(color);
			Py_DECREF(flag);
			PyGILState_Release(gstate);
			return builder.buildBooleanObject(true, size);
		}

		double c = PyFloat_AsDouble(g);
		if(c == 0.7 || c == 0.0 || c == 0.35)
		{
			Py_DECREF(main);
			Py_DECREF(local);
			Py_DECREF(entry);
			Py_DECREF(flag);
			Py_DECREF(fill);
			Py_DECREF(color);
			Py_DECREF(g);
			PyGILState_Release(gstate);
			return builder.buildBooleanObject(true, size);
		}
	
		Py_DECREF(flag);
		Py_DECREF(fill);
		Py_DECREF(color);
		Py_DECREF(g);
		Py_DECREF(entry);
	
		log.elog(os.str());
		entry = _findByNameLayer(local, os.str());
	}
	
	Py_DECREF(main);
	Py_DECREF(local);
	PyGILState_Release(gstate);
	return builder.buildBooleanObject(false, size);
}

char * Interfaces::IsIncursion(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * neocom = _getLayer("neocom");
	if(neocom == NULL)
	{
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * neocomLeftSide = _findByNameLayer(neocom, "neocomLeftside");
	if(neocomLeftSide == NULL)
	{
		log.elog("Couldn't get the left side");
		Py_DECREF(neocom);
		PyGILState_Release(gstate);
		return NULL;
	}

	
	PyObject * locationInfo = _findByNameLayer(neocomLeftSide,  "IncursionInfoContainer");
	if(locationInfo == NULL)
	{
		log.elog("IncursionInfoContainer is null");
		Py_DECREF(neocom);
		Py_DECREF(neocomLeftSide);
		char * output = builder.buildBooleanObject(false, size);
		PyGILState_Release(gstate);
		return output;
	}	

	char * output = builder.buildBooleanObject(true, size);
	Py_DECREF(neocom);
	Py_DECREF(locationInfo);
	Py_DECREF(neocomLeftSide);
	PyGILState_Release(gstate);
	return output;

}



char * Interfaces::GetModalOkButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getModalButton("OK_Btn", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetModalYesButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getModalButton("Yes_Btn", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetModalNoButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getModalButton("No_Btn", size);
	PyGILState_Release(gstate);
	return output;
}


PyObject * Interfaces::_getSysMenuButtonByText(string ctext)
{
	PyObject * modal = _getLayer("modal");
	if(modal == NULL)
	{
		log.elog("Couldn't get modal");
		return NULL;
	}

	PyObject * system_menu = _findByNameLayer(modal, "l_systemmenu");
	if(system_menu == NULL)
	{
		log.elog("Couldn't get system menu");
		Py_DECREF(modal);
		return NULL;
	}
	
	PyObject * btnPar = _findByNameLayer(system_menu, "btnPar");
	if(btnPar == NULL)
	{
		log.elog("Couldn't get buttons");
		Py_DECREF(modal);
		Py_DECREF(system_menu);
		return NULL;
	}
	
	PyObject * children = _getAttribute(btnPar, "children");

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
			Py_DECREF(modal);
			Py_DECREF(system_menu);
			Py_DECREF(children);
			return NULL;
		}
		
		PyObject * text = _getAttribute(pvalue, "text");
		if(text == NULL)
		{
			Py_DECREF(modal);
			Py_DECREF(system_menu);
			Py_DECREF(children);
			Py_DECREF(pvalue);
			return NULL;
		}

		//log.elog(PyEval_GetFuncName(pvalue));
		if(strcmp(PyString_AsString(text), ctext.c_str()) == 0)
		{
			log.elog("Found button");
			Py_DECREF(modal);
			Py_DECREF(system_menu);
			Py_DECREF(children);
			Py_DECREF(text);
			return pvalue;
		}

		Py_DECREF(pvalue);
	}

	return NULL;

}

char * Interfaces::GetLogOffButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * button = _getSysMenuButtonByText("Log off");

	if(button == NULL)
	{
		log.elog("Couldn't get the button");
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;

	bool ok = _populateAttributes(button, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("something went wrong in populating");
		Py_DECREF(button);
		PyGILState_Release(gstate);
		return NULL;
	}
		
	char * output = builder.buildInterfaceObject("logoffButton", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteLeft);
	Py_DECREF(absoluteTop);
	Py_DECREF(button);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetBookMarkFieldName(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * modal = _getLayer("modal");
	if(modal == NULL)
	{
		log.elog("Couldn't get modal");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * newbm = _findByNameLayer(modal, "l_modal_New Bookmark");
	if(newbm == NULL)
	{
		log.elog("Couldn't get new bm");
		Py_DECREF(modal);
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * editcap = _findByNameLayer(newbm, "edit_caption");
	if(editcap == NULL)
	{
		log.elog("Couldn't get caption");
		Py_DECREF(modal);
		Py_DECREF(newbm);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * text = _getAttribute(editcap, "text");
	if(text == NULL)
	{
		log.elog("Couldn't get the text");
		Py_DECREF(modal);
		Py_DECREF(newbm);
		Py_DECREF(editcap);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildStringObject(PyString_AsString(text), size);
	Py_DECREF(modal);
	Py_DECREF(newbm);
	Py_DECREF(editcap);
	Py_DECREF(text);
	PyGILState_Release(gstate);
	return output;

}


char * Interfaces::_getModalButton(string name, int & size)
{
	PyObject * modal = _getLayer("modal");
	if(modal == NULL)
	{
		log.elog("Couldn't get modal");
		return NULL;
	}

	PyObject * button = _findByNameLayer(modal, name);

	if(button == NULL)
	{
		log.elog("Doesn't have button");
		Py_DECREF(modal);
		return NULL;
	}

	PyObject * height = NULL, * width = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	
	bool ok = _populateAttributes(button, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		Py_DECREF(button);
		Py_DECREF(modal);
		return NULL;
	}
	
	char * output = builder.buildInterfaceObject(name, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(button);
	Py_DECREF(modal);

	return output;

}
char * Interfaces::IsSystemMenuOpen(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * modal = NULL;
	char * output = NULL;
	modal = _getLayer("modal");
	if(modal == NULL)
	{
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * sysmenu = NULL;
	sysmenu = _findByNameLayer(modal, "sysmenu");

	if(sysmenu == NULL)
	{
		output = builder.buildBooleanObject(false, size);
	}
	else
	{
		output = builder.buildBooleanObject(true, size);
		Py_DECREF(sysmenu);
	}
	
	Py_DECREF(modal);
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

char * Interfaces::GetShipArmor(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _GetShipUIGauge("armorGauge", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetShipShield(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _GetShipUIGauge("shieldGauge", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetShipStructure(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _GetShipUIGauge("structureGauge", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::_GetShipUIGauge(string name, int & size)
{
	PyObject * shipui = _getLayer("shipui");
	if(shipui == NULL)
	{
		log.elog("Couldn't get shipui");
		return NULL;
	}

	PyObject * underMain = _findByNameLayer(shipui, "underMain");
	if(underMain == NULL)
	{
		log.elog("Couldn't get underMain");
		Py_DECREF(shipui);
		return NULL;
	}

	PyObject * gauge = _findByNameLayer(underMain, name);
	if(gauge == NULL)
	{
		log.elog("Couldn't get structure");
		Py_DECREF(shipui);
		Py_DECREF(underMain);
		return NULL;
	}
	
	PyObject * hint = _getAttribute(gauge, "hint");
	if(hint == NULL)
	{
		log.elog("Couldn't get hint");
		Py_DECREF(shipui);
		Py_DECREF(underMain);
		Py_DECREF(gauge);
		return NULL;
	}
	
	char * output = builder.buildStringObject(PyString_AsString(hint), size);
	Py_DECREF(shipui);
	Py_DECREF(underMain);
	Py_DECREF(gauge);
	Py_DECREF(hint);
	return output;
}


char * Interfaces::GetShipSpeed(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * shipui = _getLayer("shipui");
	if(shipui == NULL)
	{
		log.elog("Couldn't get shipui");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * underMain = _findByNameLayer(shipui, "underMain");
	if(underMain == NULL)
	{
		log.elog("Couldn't get underMain");
		Py_DECREF(shipui);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * label = _findByNameLayer(underMain, "text");
	if(label == NULL)
	{
		log.elog("Couldn't get label");
		Py_DECREF(shipui);
		Py_DECREF(underMain);
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * text = _getText(label);
	if(text == NULL)
	{
		log.elog("Couldn't get text");
		Py_DECREF(shipui);
		Py_DECREF(underMain);
		Py_DECREF(label);
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildStringObject(PyString_AsString(text), size);
	Py_DECREF(shipui);
	Py_DECREF(underMain);
	Py_DECREF(label);
	Py_DECREF(text);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetShipHangar(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * cargoWindow = _GetInflightCargoView();
	if(cargoWindow == NULL)
	{
		log.elog("Couldn't get cargo window");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, *height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributesDisplay(cargoWindow, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate");
		Py_DECREF(cargoWindow);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("shipHangar", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(cargoWindow);
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteLeft);
	Py_DECREF(absoluteTop);
/*
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

*/

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



char * Interfaces::GetShipCapacity(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * cargoView = _GetInflightCargoView();
	
	if(cargoView == NULL)
	{
		log.elog("Couldn't get cargoview");
		PyGILState_Release(gstate);
		return NULL;;
	}

	PyObject * label = _findByNameLayer(cargoView, "capacityText");
	if(label == NULL)
	{
		log.elog("Couldn't get capacityLabel");
		Py_DECREF(cargoView);
		PyGILState_Release(gstate);
		return NULL;;
	}

	PyObject * ptext = _getText(label);
	if(ptext == NULL)
	{
		log.elog("Couldn't pull the text off the label");
		Py_DECREF(cargoView);
		Py_DECREF(label);
		PyGILState_Release(gstate);
		return NULL;;
	}


	char * output = builder.buildStringObject(PyString_AsString(ptext), size);
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::GetInterfaceWindows(int & size)
{
	list<ObjectBuilder::overViewEntry * > labels;
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("main is NULL");
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject *children = _getAttribute(main, "children");
	if(children == NULL)
	{
		log.elog("Has no children");
		Py_DECREF(main);
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * pvalue = NULL, * pkey = NULL;
	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	int csize = PyObject_Size(children);
	for(int i = 0; i < csize; i++)
	{
		pkey = PyInt_FromLong(i);
		pvalue = PyObject_GetItem(children, pkey);
		
		if(pvalue == NULL)
		{
			log.elog("Couldn't get entries_children values");
			Py_DECREF(main);
			Py_DECREF(children);
			PyGILState_Release(gstate);
			return NULL;
		}

		bool ok = _populateAttributes(pvalue, &width, &height, &absoluteTop, &absoluteLeft);
		if(!ok)
		{	
			log.elog("Couldn't populate attributes");
			Py_DECREF(main);
			Py_DECREF(children);
			Py_DECREF(pvalue);
			PyGILState_Release(gstate);
			return NULL;
		}
	
		ObjectBuilder::overViewEntry * entry = new ObjectBuilder::overViewEntry();
		entry->text = "window";
		entry->height = PyInt_AsLong(height);
		entry->width = PyInt_AsLong(width);
		entry->topLeftX = PyInt_AsLong(absoluteLeft);
		entry->topLeftY = PyInt_AsLong(absoluteTop);
		
		labels.push_back(entry);
		Py_DECREF(pvalue);
	}

	char * output = builder.buildOverViewObject(labels, size);
	Py_DECREF(main);
	Py_DECREF(children);
	PyGILState_Release(gstate);
	for(list<ObjectBuilder::overViewEntry *>::iterator it = labels.begin(); it != labels.end(); it++)
	{
		delete (*it);
	}

	return output;
}
char * Interfaces::GetMenuItems(int & size)
{
	list<ObjectBuilder::overViewEntry * > labels;
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * menu = _getLayer("menu");
	if(menu == NULL)
	{
		log.elog("Couldn't get the menu");
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * menuview = _findByNameLayer(menu, "menuview");
	if(menuview == NULL)
	{
		log.elog("Couldn't get menuview");
		Py_DECREF(menu);
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * entries = _findByNameLayer(menuview, "_entries");
	if(entries == NULL)
	{
		log.elog("Couldn't get entries");
		Py_DECREF(menu);
		Py_DECREF(menuview);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject *entries_children = _getAttribute(entries, "children");
	if(entries_children == NULL)
	{
		log.elog("Has no children");
		Py_DECREF(menu);
		Py_DECREF(menuview);
		Py_DECREF(entries);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * pkey = NULL, * pvalue = NULL;
	const char * fname;

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	int csize = PyObject_Size(entries_children);
	for(int i = 0; i < csize; i++)
	{
		pkey = PyInt_FromLong(i);
		pvalue = PyObject_GetItem(entries_children, pkey);
		
		if(pvalue == NULL)
		{
			log.elog("Couldn't get entries_children values");
			Py_DECREF(menu);
			Py_DECREF(menuview);
			Py_DECREF(entries);
			Py_DECREF(entries_children);
			PyGILState_Release(gstate);
			return NULL;
		}
		
		fname = PyEval_GetFuncName(pvalue);
		if(fname == NULL)
		{
			log.elog("Couldn't get function name");
			Py_DECREF(menu);
			Py_DECREF(menuview);
			Py_DECREF(entries);
			Py_DECREF(entries_children);
			Py_DECREF(pvalue);
			PyGILState_Release(gstate);
			return NULL;
		}

		if(strcmp(fname, "MenuEntryView") == 0)
		{
			PyObject * text_child = _findByNameLayer(pvalue, "text");
			if(text_child == NULL)
			{
				log.elog("Couldn't get text child");
				Py_DECREF(menu);
				Py_DECREF(menuview);
				Py_DECREF(entries);
				Py_DECREF(entries_children);
				Py_DECREF(pvalue);
				PyGILState_Release(gstate);
				return NULL;
			}
			PyObject * text = _getAttribute(text_child, "text");
			if(text == NULL)
			{
				log.elog("couldn't get text");
				Py_DECREF(menu);
				Py_DECREF(menuview);
				Py_DECREF(entries);
				Py_DECREF(entries_children);
				Py_DECREF(pvalue);
				Py_DECREF(text_child);
				PyGILState_Release(gstate);
				return NULL;
			}
			
			char * ctext = PyString_AsString(text);
			if(ctext == NULL)
			{
				log.elog("Couldn't convert into c string");
				Py_DECREF(menu);
				Py_DECREF(menuview);
				Py_DECREF(entries);
				Py_DECREF(entries_children);
				Py_DECREF(pvalue);
				Py_DECREF(text_child);
				Py_DECREF(text);
				PyGILState_Release(gstate);
				return NULL;
			}

			bool ok = _populateAttributes(text_child, &width, &height, &absoluteTop, &absoluteLeft);
			if(!ok)
			{
				log.elog("Couldn't populate attributes");
				Py_DECREF(menu);
				Py_DECREF(menuview);
				Py_DECREF(entries);
				Py_DECREF(entries_children);
				Py_DECREF(pvalue);
				Py_DECREF(text_child);
				Py_DECREF(text);
				PyGILState_Release(gstate);
				return NULL;
			}

			ObjectBuilder::overViewEntry * entry = new ObjectBuilder::overViewEntry();
			entry->text = ctext;
			entry->height = PyInt_AsLong(height);
			entry->width = PyInt_AsLong(width);
			entry->topLeftX = PyInt_AsLong(absoluteLeft);
			entry->topLeftY = PyInt_AsLong(absoluteTop);

			labels.push_back(entry);
			Py_DECREF(text);
			Py_DECREF(text_child);
		}

		Py_DECREF(pvalue);
	}

	Py_DECREF(menu);
	Py_DECREF(menuview);
	Py_DECREF(entries);
	PyGILState_Release(gstate);
	char * output = builder.buildOverViewObject(labels, size);
	for(list<ObjectBuilder::overViewEntry *>::iterator it = labels.begin(); it != labels.end(); it++)
	{
		delete (*it);
	}	
	return output;
}

char * Interfaces::_getNeoComItem(string name, int & size)
{
	PyObject * neocom = _getLayer("neocom");
	if(neocom == NULL)
	{
		log.elog("Couldn't get neocom");
		return NULL;
	}

	PyObject * neocomitem = _findByNameLayer(neocom, name);
	if(neocomitem == NULL)
	{
		log.elog("couldn't find neocom item");
		Py_DECREF(neocom);
		return NULL;
	}

	PyObject * width = NULL, *height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(neocomitem, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("error populating");
		Py_DECREF(neocom);
		Py_DECREF(neocomitem);
		return NULL;
	}

	char * output = builder.buildInterfaceObject(name, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(neocom);
	Py_DECREF(neocomitem);
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteTop);
	Py_DECREF(absoluteLeft);
	return output;
}

PyObject * Interfaces::_getAddressBookWindow()
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("couldn't get main");
		return NULL;
	}
	
	PyObject * addressbook = _findByNameLayer(main, "addressbook");
	if(addressbook == NULL)
	{
		log.elog("Couldn't get addressbook");
		Py_DECREF(main);
		return NULL;
	}
	
	return addressbook;

}

char * Interfaces::_getPeopleAndPlacesButton(string name, int & size)
{
	PyObject * addressbookWindow = _getAddressBookWindow();
	if(addressbookWindow == NULL)
	{
		log.elog("couldn't get addressbook window");
		return NULL;
	}

	PyObject * button = _findByNameLayer(addressbookWindow, name);
	if(button == NULL)
	{
		log.elog("Couldn't get button");
		Py_DECREF(addressbookWindow);
		return NULL;
	}
	
	PyObject * width = NULL, * height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(button, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate");
		Py_DECREF(addressbookWindow);
		Py_DECREF(button);
		return NULL;
	}

	char * output = builder.buildInterfaceObject(name, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(addressbookWindow);
	Py_DECREF(button);
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteLeft);
	Py_DECREF(absoluteTop);
	return output;

}

char * Interfaces::GetAddressBookWindow(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * window = _getAddressBookWindow();
	if(window == NULL)
	{
		log.elog("Couldn't get the window");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * height = NULL, * width = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(window, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Trouble populating");
		Py_DECREF(window);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("Addressbook", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(window);
	Py_DECREF(height);
	Py_DECREF(width);
	Py_DECREF(absoluteTop);
	Py_DECREF(absoluteLeft);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetAddressBookBMButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getPeopleAndPlacesButton("Add Bookmark_Btn", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetAddressBookPlacesTab(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * addressbookWindow = _getAddressBookWindow();
	if(addressbookWindow == NULL)
	{
		log.elog("Couldn't get the address book window");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * tabparent = _findByNameLayer(addressbookWindow, "tabparent");
	if(tabparent == NULL)
	{
		log.elog("Couldn't get the tabs");
		Py_DECREF(addressbookWindow);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * children = _getAttribute(tabparent, "children");

	if(children == NULL)
	{
		log.elog("Couldn't get children");
		Py_DECREF(addressbookWindow);
		Py_DECREF(children);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * pkey = NULL, *pvalue = NULL;
	int csize = PyObject_Size(children);
	for(int i = 0; i < csize; i++)
	{
		pkey = PyInt_FromLong(i);
		pvalue = PyObject_GetItem(children, pkey);
		if(pvalue == NULL)
		{
			log.elog("pvalue is null");
			Py_DECREF(addressbookWindow);
			Py_DECREF(children);
			PyGILState_Release(gstate);
			return NULL;
		}

		log.elog(PyEval_GetFuncName(pvalue));
		if(strcmp(PyEval_GetFuncName(pvalue), "Tab"))
		{
			PyObject * tabLabel = _findByNameLayer(pvalue, "tabLabel");
			if(tabLabel == NULL)
			{
				log.elog("couldn't get label");
				Py_DECREF(addressbookWindow);
				Py_DECREF(children);
				Py_DECREF(pvalue);
				PyGILState_Release(gstate);
				return NULL;
			}

			PyObject * ptext = _getAttribute(tabLabel, "text");
			if(ptext == NULL)
			{
				log.elog("Couldn't get text");
				Py_DECREF(addressbookWindow);
				Py_DECREF(children);
				Py_DECREF(pvalue);
				Py_DECREF(tabLabel);
				PyGILState_Release(gstate);
				return NULL;
			}

			if(strcmp(PyString_AsString(ptext), "Places") == 0)
			{
				PyObject * width = NULL, * height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
				bool ok = _populateAttributes(tabLabel, &width, &height, &absoluteTop, &absoluteLeft);
				if(!ok)
				{
					log.elog("Couldn't populate");
					Py_DECREF(addressbookWindow);
					Py_DECREF(children);
					Py_DECREF(pvalue);
					Py_DECREF(tabLabel);
					Py_DECREF(ptext);
					PyGILState_Release(gstate);
					return NULL;
				}

				char * output = builder.buildInterfaceObject(PyString_AsString(ptext), PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
				Py_DECREF(addressbookWindow);
				Py_DECREF(children);
				Py_DECREF(pvalue);
				Py_DECREF(tabLabel);
				Py_DECREF(ptext);
				PyGILState_Release(gstate);
				return output;

			}
			Py_DECREF(tabLabel);
			Py_DECREF(ptext);

		}

		Py_DECREF(pvalue);

	}

	Py_DECREF(addressbookWindow);
	Py_DECREF(children);
	PyGILState_Release(gstate);
	return NULL;

}

char * Interfaces::GetPeopleAndPlaces(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getNeoComItem("addressbook", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetHangarItems(int & size)
{
	list<ObjectBuilder::itemEntry *> labels;
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * hangarView = _findByNameLayer(main, "hangarFloor");
	if(hangarView == NULL)
	{
		log.elog("Couldn't get hangar");
		Py_DECREF(main);
		PyGILState_Release(gstate);
		return NULL;;
	}


	PyObject * entry = _findByNameLayer(hangarView, "entry_0");
	stringstream os;
	os << "entry_";


	for(int i = 1; entry != NULL; i++)
	{
		_IterateThroughEntryAndBuild(entry, labels);
		os.str("");
		os << "entry_" << i;

		log.elog(os.str());
		entry = _findByNameLayer(hangarView, os.str());
	}

	PyGILState_Release(gstate);
	char * output = builder.buildItemObject(labels, size);
	for(list<ObjectBuilder::itemEntry *>::iterator it = labels.begin(); it != labels.end(); it++)
	{
		delete (*it);
	}
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

		if(PyObject_Not(node))
		{
			Py_DECREF(sr);
			Py_DECREF(node);
			Py_DECREF(pvalue);
			continue;
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
	stringstream os;
	os << "inFlightHighSlot";
	os << number;

	output = _isModuleActive(os.str(), size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::getHangar(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * layer = _getLayer("main");
	
	if(layer == NULL)
	{
		log.elog("Couldn't get main");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * hangar = _findByNameLayer(layer, "hangarFloor");
	if(hangar == NULL)
	{
		log.elog("couldn't get hangar");
		Py_DECREF(layer);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(hangar, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("couldn't populate");
		Py_DECREF(layer);
		Py_DECREF(hangar);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("hangar", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(layer);
	Py_DECREF(hangar);
	Py_DECREF(width);
	Py_DECREF(height);
	Py_DECREF(absoluteTop);
	Py_DECREF(absoluteLeft);
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::IsMedSlotActive(int number, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	stringstream os;
	os << "inFlightMediumSlot";
	os << number;
	output = _isModuleActive(os.str(), size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::IsLowSlotActive(int number, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	stringstream os;
	os << "inFlightLowSlot";
	os << number;

	output = _isModuleActive(os.str(), size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetMiningAmount(int number, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	stringstream os;
	os << "inFlightHighSlot";
	os << number;
	output = _getMiningAmount(os.str(), size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetDuration(int number, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	stringstream os;
	os << "inFlightHighSlot";
	os << number;
	output = _getModuleDuration(os.str(), size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetTargetingRange(int number, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	stringstream os;
	os << "inFlightHighSlot";
	os << number;
	output = _getModuleTargetingRange(os.str(), size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::_getModuleTargetingRange(string name, int & size)
{
	return _getModuleAttribute(name, "maxRange", size);
}

char * Interfaces::_getMiningAmount(string name, int & size)
{
	return _getModuleAttribute(name, "miningAmount", size);
}

char * Interfaces::_getModuleDuration(string name, int & size)
{
	return _getModuleAttribute(name, "duration", size);
}

char * Interfaces::_getModuleAttribute(string name, string attr, int & size)
{
	PyObject * module = _findModule(name);
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

	PyObject * moduleInfo = _getAttribute(sragain, "moduleInfo");
	if(moduleInfo == NULL)
	{
		log.elog("Can't get the moduleInfo");
		Py_DECREF(module);
		Py_DECREF(sr);
		Py_DECREF(srmodule);
		Py_DECREF(sragain);
		return NULL;
	}

	PyObject * attribute = _getAttribute(moduleInfo, attr);
	if(attribute == NULL)
	{
		log.elog("Couldn't get maxRange");
		Py_DECREF(module);
		Py_DECREF(sr);
		Py_DECREF(srmodule);
		Py_DECREF(sragain);
		Py_DECREF(moduleInfo);
		return NULL;
	}

	if(PyObject_Not(attribute))
	{
		Py_DECREF(module);
		Py_DECREF(sr);
		Py_DECREF(srmodule);
		Py_DECREF(sragain);
		Py_DECREF(moduleInfo);
		Py_DECREF(attribute);
		return NULL;
	}

	
	PyFloatObject * value = (PyFloatObject*)PyFloat_FromDouble(PyFloat_AsDouble(attribute));
	if(value == NULL)
	{
		Py_DECREF(module);
		Py_DECREF(sr);
		Py_DECREF(srmodule);
		Py_DECREF(sragain);
		Py_DECREF(moduleInfo);
		Py_DECREF(attribute);
		return NULL;
	}
	char buf[200];
	PyFloat_AsString(buf, value);

	char * output = builder.buildStringObject(buf, size);
	Py_DECREF(module);
	Py_DECREF(sr);
	Py_DECREF(srmodule);
	Py_DECREF(sragain);
	Py_DECREF(moduleInfo);
	Py_DECREF(attribute);
	Py_DECREF(value);
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

char * Interfaces::GetLowSlot(int number, int & size)
{
	char * output = NULL;
	PyGILState_STATE gstate = PyGILState_Ensure();
	stringstream os;

	os << "inFlightLowSlot";
	os << number;

	output = _GetSlot(os.str(), os.str(), size);
	PyGILState_Release(gstate);
	return output;
}


char * Interfaces::GetMidSlot(int number, int & size)
{
	char * output = NULL;
	PyGILState_STATE gstate = PyGILState_Ensure();
	stringstream os;

	os << "inFlightMediumSlot";
	os << number;

	output = _GetSlot(os.str(), os.str(), size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetHighSlot(int number, int & size)
{
	char * output = NULL;
	PyGILState_STATE gstate = PyGILState_Ensure();
	stringstream os;

	os << "inFlightHighSlot";
	os << number;

	output = _GetSlot(os.str(), os.str(), size);
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
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildStringObject(label, size);

	Py_DECREF(main);
	Py_DECREF(selectedItemView);
	Py_DECREF(maincontainer);
	Py_DECREF(mainitem);
	Py_DECREF(toparea);
	Py_DECREF(text);
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
			PyObject * label = NULL, * text = NULL, * width =NULL, * height=NULL, *absoluteLeft=NULL, *absoluteTop=NULL;
			char * ctext;
			label = _findByNameLayer(pvalue, "text");
			if(label == NULL)
			{
				log.elog("Couldn't pull the text child off the target");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				PyGILState_Release(gstate);
				return NULL;
			}
			
			text= _getAttribute(label, "text");
			if(text == NULL)
			{
				log.elog("Couldn't pull the text attribute off the label");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				Py_DECREF(label);
				PyGILState_Release(gstate);
				return NULL;
			}

			ctext = PyString_AsString(text);
			if(text == NULL)
			{
				log.elog("Couldn't pull the text off the label");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				Py_DECREF(label);
				Py_DECREF(text);
				PyGILState_Release(gstate);
				return NULL;
			}
			
			width = _getWidth(label);
			if(width == NULL)
			{
				log.elog("Couldn't get width");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				Py_DECREF(label);
				Py_DECREF(text);
				PyGILState_Release(gstate);
				return NULL;
			}
			
			height = _getHeight(label);
			if(height == NULL)
			{
				log.elog("Couldn't get height");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				Py_DECREF(label);
				Py_DECREF(text);
				Py_DECREF(width);
				PyGILState_Release(gstate);
				return NULL;
			}
			
			absoluteLeft = _getAbsoluteLeft(label);
			if(absoluteLeft == NULL)
			{
				log.elog("Couldn't get absoluteLeft");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				Py_DECREF(text);
				Py_DECREF(label);
				Py_DECREF(height);
				Py_DECREF(width);
				PyGILState_Release(gstate);
				return NULL;
			}

			absoluteTop = _getAbsoluteTop(label);
			if(absoluteTop == NULL)
			{
				log.elog("Couldn't get absoluteTop");
				Py_DECREF(children);
				Py_DECREF(target);
				Py_DECREF(pvalue);
				Py_DECREF(label);
				Py_DECREF(height);
				Py_DECREF(width);
				Py_DECREF(text);
				Py_DECREF(absoluteLeft);
				PyGILState_Release(gstate);
				return NULL;
			}
			ObjectBuilder::targetEntry * tEntry = new ObjectBuilder::targetEntry();
			tEntry->name = ctext;
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
			Py_DECREF(text);
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

int Interfaces::_getSize(PyObject * layer)
{
	PyObject * top = _getAttribute(layer, "absoluteTop");
	if(top == NULL)
	{
		log.elog("couldn't get top");
		return -1;
	}

	PyObject * bottom = _getAttribute(layer, "absoluteBottom");
	if(bottom == NULL)
	{
		log.elog("couldn't get bottom");
		Py_DECREF(top);
		return -1;
	}

	int size = PyInt_AsLong(bottom) - PyInt_AsLong(top);
	Py_DECREF(bottom);
	Py_DECREF(top);
	return size;
}


char * Interfaces::_getLocalChatScrollAttribute(string attr, int & size)
{
	PyObject * chatScroll = _getLocalChatScroll();
	if(chatScroll == NULL)
	{
		log.elog("couldn't get chatscroll");
		return NULL;
	}

	PyObject * parent = _getAttribute(chatScroll, "parent");
	if(parent == NULL)
	{
		log.elog("Couldn't get parent");
		Py_DECREF(chatScroll);
		return NULL;
	}

	PyObject * bottom = _getAttribute(parent, attr);
	if(bottom == NULL)
	{
		log.elog("couldn't get attribute");
		Py_DECREF(chatScroll);
		Py_DECREF(parent);
		return NULL;
	}

	stringstream os;
	os << PyInt_AsLong(bottom);

	char * output = builder.buildStringObject(os.str(), size);
	Py_DECREF(chatScroll);
	Py_DECREF(parent);
	Py_DECREF(bottom);
	return output;

}

char * Interfaces::GetLocalChatTop(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getLocalChatScrollAttribute("absoluteTop", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetLocalChatBottom(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getLocalChatScrollAttribute("absoluteBottom", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetOverviewBottom(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * overviewScroll = _getOverviewScroll();
	
	if(overviewScroll == NULL)
	{
		log.elog("couldn't get the overview scroll");
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * bottom = _getAttribute(overviewScroll, "absoluteBottom");
	if(bottom == NULL)
	{
		log.elog("couldn't get bottom");
		Py_DECREF(overviewScroll);
		PyGILState_Release(gstate);
		return NULL;
	}

	stringstream os;
	os << PyInt_AsLong(bottom);

	char * output = builder.buildStringObject(os.str(), size);
	Py_DECREF(overviewScroll);
	Py_DECREF(bottom);
	PyGILState_Release(gstate);
	return output;
}

bool Interfaces::isLoginOpen()
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * login = _getLayer("login");
	if(login == NULL)
	{
		log.elog("Couldn't get login");
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * isopen = _getAttribute(login, "isopen");
	
	if(isopen == NULL)
	{
		log.elog("Couldn't check open");
		Py_DECREF(login);
		PyGILState_Release(gstate);
		return NULL;
	}

	bool open = PyObject_IsTrue(isopen);
	Py_DECREF(login);
	Py_DECREF(isopen);
	PyGILState_Release(gstate);
	return open;
}

string Interfaces::Internal_getVersion()
{
	bool open = isLoginOpen();
	if(!open)
	{
		log.elog("login is not open");
		return "";
	}

	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * login = _getLayer("login");
	if(login == NULL)
	{
		log.elog("Couldn't get login");
		PyGILState_Release(gstate);
		return "";
	}

	PyObject * version = _findByNameLayer(login, "text");
	if(version == NULL)
	{
		log.elog("Couldn't get version");
		Py_DECREF(login);
		PyGILState_Release(gstate);
		return "";
	}

	PyObject *version_text = _getAttribute(version, "text");
	if(version_text == NULL)
	{
		log.elog("Couldn't get version text");
		Py_DECREF(login);
		Py_DECREF(version);
		PyGILState_Release(gstate);
		return "";
	}

	string output(PyString_AsString(version_text));
	Py_DECREF(login);
	Py_DECREF(version);
	Py_DECREF(version_text);
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::GetVersion(int & size)
{
	bool open = isLoginOpen();
	if(!open)
		return NULL;

	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * login = _getLayer("login");
	if(login == NULL)
	{
		log.elog("Couldn't get login");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * version = _findByNameLayer(login, "text");
	if(version == NULL)
	{
		log.elog("Couldn't get version");
		Py_DECREF(login);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject *version_text = _getAttribute(version, "text");
	if(version_text == NULL)
	{
		log.elog("Couldn't get version text");
		Py_DECREF(login);
		Py_DECREF(version);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildStringObject(PyString_AsString(version_text), size);
	Py_DECREF(login);
	Py_DECREF(version);
	Py_DECREF(version_text);
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::GetOverviewTop(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * overviewScroll = _getOverviewScroll();
	
	if(overviewScroll == NULL)
	{
		log.elog("couldn't get the overview scroll");
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * bottom = _getAttribute(overviewScroll, "absoluteTop");
	if(bottom == NULL)
	{
		log.elog("couldn't get bottom");
		Py_DECREF(overviewScroll);
		PyGILState_Release(gstate);
		return NULL;
	}

	stringstream os;
	os << PyInt_AsLong(bottom);

	char * output = builder.buildStringObject(os.str(), size);
	Py_DECREF(overviewScroll);
	Py_DECREF(bottom);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetLocalChatScrollbar(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * scroll = _getLocalChatScroll();
	if(scroll == NULL)
	{
		log.elog("couldn't get scroll");
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(scroll, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("couldn't populate attributes");
		Py_DECREF(scroll);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("localScroll", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(scroll);
	Py_DECREF(height);
	Py_DECREF(width);
	Py_DECREF(absoluteTop);
	Py_DECREF(absoluteLeft);
	PyGILState_Release(gstate);
	return output;
}

PyObject * Interfaces::_getLocalChatScroll()
{
	PyObject * main = _getLayer("main");

	if(main == NULL)
	{
		log.elog("main is null");
		return NULL;
	}

	PyObject * local = _findByNameLayer(main, "chatchannel_solarsystemid2");
	if(local == NULL)
	{
		log.elog("Couldn't get local");
		Py_DECREF(main);
		return NULL;
	}
	
	PyObject * userlist = _findByNameLayer(local, "userlist");
	if(userlist == NULL)
	{
		log.elog("Couldn't get userlist");
		Py_DECREF(main);
		Py_DECREF(local);
		return NULL;
	}


	PyObject * scroll = _findByNameLayer(userlist, "__scrollhandle");
	if(scroll == NULL)
	{
		log.elog("Couldn't get the scroll");
		Py_DECREF(main);
		Py_DECREF(local);
		Py_DECREF(userlist);
		return NULL;
	}

	Py_DECREF(main);
	Py_DECREF(local);
	Py_DECREF(userlist);

	return scroll;

}

PyObject * Interfaces::_getOverviewScroll()
{
	PyObject * main = _getLayer("main");

	if(main == NULL)
	{
		log.elog("main is null");
		return NULL;
	}

	PyObject * overview = _findByNameLayer(main, "overview");
	if(overview == NULL)
	{
		log.elog("Couldn't get overview child");
		Py_DECREF(main);
		return NULL;
	}
	
	PyObject * overviewScroll = _findByNameLayer(overview, "overviewscroll2");
	if(overviewScroll == NULL)
	{
		log.elog("Couldn't get the scrollarea");
		Py_DECREF(main);
		Py_DECREF(overview);
		return NULL;
	}

	Py_DECREF(main);
	Py_DECREF(overview);

	return overviewScroll;

}

char * Interfaces::GetOverviewHeight(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * overviewScroll = _getOverviewScroll();
	
	if(overviewScroll == NULL)
	{
		log.elog("couldn't get the overview scroll");
		PyGILState_Release(gstate);
		return NULL;
	}

	int ssize = _getSize(overviewScroll);
	stringstream os;
	os << ssize;
	char * output = builder.buildStringObject(os.str(), size);
	Py_DECREF(overviewScroll);
	PyGILState_Release(gstate);
	return output;

}

PyObject * Interfaces::_getScrollHandle(PyObject * layer)
{
	PyObject * scrollControls = _findByNameLayer(layer, "__scrollcontrols");
	if(scrollControls == NULL)
	{
		log.elog("scrollControls is null");
		return NULL;
	}

	PyObject * sr = _getAttribute(scrollControls, "sr");
	if(sr == NULL)
	{
		log.elog("Couldn't get sr");
		Py_DECREF(scrollControls);
		return NULL;
	}

	PyObject * scrollHandle = _getAttribute(sr, "scrollhandle");
	if(scrollHandle == NULL)
	{
		log.elog("Couldn't get scrollHandle");
		Py_DECREF(scrollControls);
		Py_DECREF(sr);
		return NULL;
	}
	
	Py_DECREF(scrollControls);
	Py_DECREF(sr);
	return scrollHandle;
}

char * Interfaces::OverviewGetScrollBar(int & size)
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
	
	PyObject * scrollHandle = _getScrollHandle(overview);
	if(scrollHandle == NULL)
	{
		log.elog("Couldn't get the scrollHandle");
		Py_DECREF(main);
		Py_DECREF(overview);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * height = NULL, * width = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	bool ok = _populateAttributes(scrollHandle, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate");
		Py_DECREF(main);
		Py_DECREF(overview);
		Py_DECREF(scrollHandle);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("overviewScrollBar", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_DECREF(main);
	Py_DECREF(overview);
	Py_DECREF(scrollHandle);
	Py_DECREF(height);
	Py_DECREF(width);
	Py_DECREF(absoluteLeft);
	Py_DECREF(absoluteTop);
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

		PyObject * absoluteTop = NULL, * absoluteLeft = NULL, * width = NULL, * height = NULL;

		bool ok = _populateAttributes(pvalue, &width, &height, &absoluteTop, &absoluteLeft);
		if(!ok)
		{
			log.elog("couldn't populate");
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


		PyObject * icon = _findByNameLayer(pvalue, "typeicon");
		if(icon == NULL)
		{
			log.elog("Icon was null");
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
			Py_DECREF(height);
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * color = _getAttribute(icon, "color");
		if(color == NULL)
		{
			log.elog("Couldn't get color");
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
			Py_DECREF(height);
			Py_DECREF(icon);
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * g = _getAttribute(color, "g");
		if(g == NULL)
		{
			log.elog("Couldn't get color");
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
			Py_DECREF(height);
			Py_DECREF(icon);
			Py_DECREF(color);
			PyGILState_Release(gstate);
			return NULL;
		}

		ObjectBuilder::overViewEntry * over = new ObjectBuilder::overViewEntry();
		over->text = PyString_AsString(text);
		over->topLeftX = PyInt_AsLong(absoluteLeft);
		over->topLeftY = PyInt_AsLong(absoluteTop);
		over->width = PyInt_AsLong(width);
		over->height = PyInt_AsLong(height);
		over->color = PyFloat_AsDouble(g);

		labels.push_back(over);
		Py_DECREF(pvalue);
		Py_DECREF(label);
		Py_DECREF(text);
		Py_DECREF(absoluteLeft);
		Py_DECREF(absoluteTop);
		Py_DECREF(width);
		Py_DECREF(icon);
		Py_DECREF(color);
		Py_DECREF(g);

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
