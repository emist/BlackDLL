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

void Interfaces::clearExceptions()
{
	PyErr_Clear();
}

char * Interfaces::atLogin(int & size)
{
		char * output;

		//Py_XDECREF(main);
		//Py_XDECREF(uicore);
		//Py_XDECREF(layer);
		
		PyGILState_STATE gstate = PyGILState_Ensure();
		clearExceptions();
		
		PyObject * login = _getLayer("login");
		
		if(login == NULL)
		{
			log.elog("Login is NULL");
			clearExceptions();
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
			clearExceptions();
			PyGILState_Release( gstate );
			return NULL;
		}

		Py_XDECREF(isopen);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * absoluteTop = _getAbsoluteTop(layer);
	if(absoluteTop == NULL)
	{
		Py_XDECREF(layer);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * absoluteLeft = _getAbsoluteLeft(layer);
	if(absoluteLeft == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(absoluteTop);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * displayWidth = _getDisplayWidth(layer);
	
	if(displayWidth == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(absoluteTop);
		Py_XDECREF(absoluteLeft);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * displayHeight = _getDisplayHeight(layer);
	if(displayHeight == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(absoluteTop);
		Py_XDECREF(absoluteLeft);
		Py_XDECREF(displayWidth);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * name = _getName(layer);
	if(name == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(absoluteTop);
		Py_XDECREF(absoluteLeft);
		Py_XDECREF(displayWidth);
		Py_XDECREF(displayHeight);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * iname = PyString_AsString(name);
	output = builder.buildInterfaceObject(iname,  (int)PyInt_AsLong(absoluteLeft),(int)PyInt_AsLong(absoluteTop), (int)PyInt_AsLong(displayWidth), (int)PyInt_AsLong(displayHeight), size);
	Py_XDECREF(layer);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(displayWidth);
	Py_XDECREF(displayHeight);
	Py_XDECREF(name);
	clearExceptions();
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



char * Interfaces::findByTextMenu(string label, int & size, bool exact)
{
	char * output = NULL;
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * menu = _getLayer("menu");
	if(menu == NULL)
	{
		log.elog("Couldn't get the menu");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * menuview = _findByNameLayer(menu, "menuview");
	if(menuview == NULL)
	{
		log.elog("Couldn't get menuview");
		Py_XDECREF(menu);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * entries = _findByNameLayer(menuview, "_entries");
	if(entries == NULL)
	{
		log.elog("Couldn't get entries");
		Py_XDECREF(menu);
		Py_XDECREF(menuview);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject *entries_children = _getAttribute(entries, "children");
	if(entries_children == NULL)
	{
		log.elog("Has no children");
		Py_XDECREF(menu);
		Py_XDECREF(menuview);
		Py_XDECREF(entries);
		clearExceptions();
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
			Py_XDECREF(menu);
			Py_XDECREF(menuview);
			Py_XDECREF(entries);
			Py_XDECREF(entries_children);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}
		
		fname = PyEval_GetFuncName(pvalue);
		if(fname == NULL)
		{
			log.elog("Couldn't get function name");
			Py_XDECREF(menu);
			Py_XDECREF(menuview);
			Py_XDECREF(entries);
			Py_XDECREF(entries_children);
			Py_XDECREF(pvalue);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		if(strcmp(fname, "MenuEntryView") == 0)
		{
			//crucible changes
			PyObject * text_child = _findByNameLayer(pvalue, "EveLabelSmall");
			if(text_child == NULL)
			{
				log.elog("Couldn't get text child");
				Py_XDECREF(menu);
				Py_XDECREF(menuview);
				Py_XDECREF(entries);
				Py_XDECREF(entries_children);
				Py_XDECREF(pvalue);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}
			PyObject * text = _getAttribute(text_child, "text");
			if(text == NULL)
			{
				log.elog("couldn't get text");
				Py_XDECREF(menu);
				Py_XDECREF(menuview);
				Py_XDECREF(entries);
				Py_XDECREF(entries_children);
				Py_XDECREF(pvalue);
				Py_XDECREF(text_child);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}
			
			char * ctext = PyString_AsString(text);
			if(ctext == NULL)
			{
				log.elog("Couldn't convert into c string");
				Py_XDECREF(menu);
				Py_XDECREF(menuview);
				Py_XDECREF(entries);
				Py_XDECREF(entries_children);
				Py_XDECREF(pvalue);
				Py_XDECREF(text_child);
				Py_XDECREF(text);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}


			log.elog(ctext);
			log.elog(label.c_str());
			string stext(ctext);
			
			stoupper(label);
			stoupper(stext);
			
			bool match = false;

			if(exact)
				match = stext.compare(label);
			else
				match = stext.find(label) != stext.npos;

			if(exact)
			{
				bool ok = _populateAttributes(text_child, &width, &height, &absoluteTop, &absoluteLeft);
				if(!ok)
				{
					log.elog("Couldn't populate attributes");
					Py_XDECREF(menu);
					Py_XDECREF(menuview);
					Py_XDECREF(entries);
					Py_XDECREF(entries_children);
					Py_XDECREF(pvalue);
					Py_XDECREF(text_child);
					Py_XDECREF(text);
					clearExceptions();
					PyGILState_Release(gstate);
					return NULL;
				}
				Py_XDECREF(text);
				Py_XDECREF(text_child);
				Py_XDECREF(pvalue);
				output = builder.buildInterfaceObject(label, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
				break;
			}
			Py_XDECREF(text);
			Py_XDECREF(text_child);
		}

		Py_XDECREF(pvalue);
	}

	Py_XDECREF(menu);
	Py_XDECREF(menuview);
	Py_XDECREF(entries);
	PyGILState_Release(gstate);
	return output;
	/*
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _findByTextGeneric("menu", label, size);
	PyGILState_Release(gstate);
	return output;
	*/
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
		clearExceptions();
		return NULL;
	}

	PyObject * item = _findByNameLayer(login, name);
	if(item == NULL)
	{
		log.elog("Couldn't get item");
		Py_XDECREF(login);
		clearExceptions();
		return NULL;
	}

	Py_XDECREF(login);
	return item;
}

char * Interfaces::_getLoginBoxesWithText(string name, int & size)
{
	PyObject * item = _getLoginItem(name);
	if(item == NULL)
	{
		log.elog("Couldn't get username");
		clearExceptions();
		return NULL;
	}
	
	PyObject * text = _getAttribute(item, "text");
	if(text == NULL)
	{
		log.elog("couldn't get text");
		Py_XDECREF(item);
		clearExceptions();
		return NULL;
	}
	
	PyObject * height = NULL, * width = NULL, *absoluteTop = NULL, *absoluteLeft=NULL;

	bool ok = _populateAttributes(item, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("couldn't populate");
		Py_XDECREF(item);
		Py_XDECREF(text);
		clearExceptions();
		return NULL;
	}

	char * output = builder.buildInterfaceObject(PyString_AsString(text), PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(item);
	Py_XDECREF(text);
	Py_XDECREF(height);
	Py_XDECREF(width);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(absoluteTop);
	clearExceptions();
	return output;
}

char * Interfaces::getUserNameBox(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getLoginBoxesWithText("username", size);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::getPasswordBox(int & size)
{	
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getLoginBoxesWithText("password", size);
	clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * enterButton = _findByNameLayer(charsel, "enterBtn");
	if(enterButton == NULL)
	{
		log.elog("couldn't find the enter");
		Py_XDECREF(charsel);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, *height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;

	bool ok = _populateAttributes(enterButton, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate correctly");
		Py_XDECREF(charsel);
		Py_XDECREF(enterButton);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("EnterButton", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(charsel);
	Py_XDECREF(enterButton);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return output;
	}
	
	PyObject * isopen = _getAttribute(charsel, "isopen");
	if(isopen == NULL)
	{
		log.elog("isopen is null");
		output = builder.buildBooleanObject(false, size);
		Py_XDECREF(charsel);
		clearExceptions();
		PyGILState_Release(gstate);
		return output;
	}
	
	bool open = false;

	if(PyObject_IsTrue(isopen))
	{
		open = true;
	}
	
	output = builder.buildBooleanObject(open, size);
	Py_XDECREF(charsel);
	Py_XDECREF(isopen);
	clearExceptions();
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
		Py_XDECREF(menu);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * children = PyObject_GetAttrString(menu, "children");
	if(children == NULL)
	{
		log.elog("Couldn't get the children attribute");
		Py_XDECREF(menu);
		clearExceptions();
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

	Py_XDECREF(menu);
	Py_XDECREF(children);
	clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * message = _findByNameLayer(abovemain, "message");
	if(message == NULL)
	{
		log.elog("Couldn't get message");
		clearExceptions();
		Py_XDECREF(abovemain);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * messageattr = _getAttribute(message, "message");
	if(messageattr == NULL)
	{
		log.elog("couldn't get message attr");
		Py_XDECREF(abovemain);
		Py_XDECREF(message);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * text = _getAttribute(messageattr, "text");
	if(text == NULL)
	{
		log.elog("couldn't get text");
		Py_XDECREF(abovemain);
		Py_XDECREF(message);
		Py_XDECREF(messageattr);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildStringObject(PyString_AsString(text), size);
	Py_XDECREF(abovemain);
	Py_XDECREF(message);
	Py_XDECREF(messageattr);
	Py_XDECREF(text);
	clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * progresswindow = _findByNameLayer(loading, "progresswindow");
	if(progresswindow == NULL)
	{
		log.elog("can't get the window");
		Py_XDECREF(loading);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * sr = _getAttribute(progresswindow, "sr");
	if(sr == NULL)
	{
		log.elog("can't get sr");
		Py_XDECREF(loading);
		Py_XDECREF(progresswindow);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * tickTimer = _getAttribute(sr, "tickTimer");
	if(tickTimer == NULL)
	{
		log.elog("can't get the timer");
		Py_XDECREF(loading);
		Py_XDECREF(progresswindow);
		Py_XDECREF(sr);
		clearExceptions();
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
	Py_XDECREF(loading);
	Py_XDECREF(progresswindow);
	Py_XDECREF(sr);
	Py_XDECREF(tickTimer);
	clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * neocomLeftSide = _findByNameLayer(neocom, "neocomLeftside");
	if(neocomLeftSide == NULL)
	{
		log.elog("Couldn't get the left side");
		Py_XDECREF(neocom);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	
	PyObject * locationInfo = _findByNameLayer(neocomLeftSide,  "locationInfo");
	if(locationInfo == NULL)
	{
		log.elog("locationInfo is null");
		Py_XDECREF(neocom);
		Py_XDECREF(neocomLeftSide);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * caption = _findByNameLayer(locationInfo, "caption");
	if(caption == NULL)
	{
		log.elog("caption is null");
		Py_XDECREF(neocom);
		Py_XDECREF(locationInfo);
		Py_XDECREF(neocomLeftSide);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * locationText = _getAttribute(caption, "text");
	if(locationText == NULL)
	{
		log.elog("Couldn't get the text");
		Py_XDECREF(neocom);
		Py_XDECREF(locationInfo);
		Py_XDECREF(caption);
		Py_XDECREF(neocomLeftSide);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * captionText = _getAttribute(caption, "text");
	if(captionText == NULL)
	{
		log.elog("couldn't get the caption");
		Py_XDECREF(neocom);
		Py_XDECREF(locationInfo);
		Py_XDECREF(caption);
		Py_XDECREF(neocomLeftSide);
		Py_XDECREF(locationText);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildSolarSystemObject(PyString_AsString(captionText), PyString_AsString(locationText), size);
	Py_XDECREF(neocom);
	Py_XDECREF(locationInfo);
	Py_XDECREF(caption);
	Py_XDECREF(neocomLeftSide);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::getInjuredDrone(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getInjuredDrone(size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::_getInjuredDrone(int & size)
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		clearExceptions();
		return NULL;
	}
	PyObject * droneChildren = _findByNameLayer(main, "droneview");
	
	if(droneChildren == NULL)
	{
		log.elog("Couldn't get children");
		Py_XDECREF(main);
		clearExceptions();
		return NULL;
	}

	string entryTxt = "entry_1";
	int i = 0;
	stringstream os;
	os << "entry_";


entry_lab:
	PyObject * entry = _findByNameLayer(droneChildren, entryTxt);
	if(entry == NULL)
	{
		if(i < 10)
		{
			i++;
			os.str("");
			os << "entry_" << i;
			entryTxt = os.str();
			os.str("");
			goto entry_lab;
		}
		log.elog("cOuldn't get the entry");
		Py_XDECREF(main);
		Py_XDECREF(droneChildren);
		clearExceptions();
		return NULL;
	}

	bool onealloted = false;

	for(; entry != NULL; i++)
	{
		os.str("");
		os << "entry_" << i;

		PyObject * gauge = _findByNameLayer(entry, "gauge_shield");
		if(gauge == NULL)
		{
			log.elog("no gauge");
			clearExceptions();
			entry = _findByNameLayer(droneChildren, os.str());
			continue;
		}
		
		if(PyObject_Not(gauge))
		{
			log.elog("no gauge");
			clearExceptions();
			entry = _findByNameLayer(droneChildren, os.str());
			continue;
		}

		PyObject * children = _getAttribute(gauge, "children");
		if(children == NULL)
		{
			log.elog("no children");
			Py_XDECREF(main);
			Py_XDECREF(droneChildren);
			Py_XDECREF(gauge);
			clearExceptions();
			return NULL;
		}

		if(PyObject_Size(children) < 1)
		{
			log.elog("no children");
			log.elog(os.str());
			Py_XDECREF(gauge);
			clearExceptions();
			entry = _findByNameLayer(droneChildren, os.str());
			continue;
		}

		PyObject * pkey = NULL;
		
		pkey = PyInt_FromLong(0);

		PyObject * frame = PyObject_GetItem(children, pkey);
		if(frame == NULL)
		{
			log.elog("no frame");
			log.elog(os.str());
			log.elog("Evaluation coming");
			log.elog(PyEval_GetFuncName(children));
			Py_XDECREF(main);
			Py_XDECREF(droneChildren);
			Py_XDECREF(gauge);
			Py_XDECREF(children);
			clearExceptions();
			return NULL;
		}

		pkey = PyInt_FromLong(1);
		PyObject * fill = PyObject_GetItem(children, pkey);
		if(fill == NULL)
		{
			log.elog("no data");
			Py_XDECREF(main);
			Py_XDECREF(droneChildren);
			Py_XDECREF(gauge);
			Py_XDECREF(children);
			Py_XDECREF(frame);
			clearExceptions();
			return NULL;
		}
		
		PyObject * fillWidth = _getAttribute(fill, "displayWidth");
		if(fillWidth == NULL)
		{
			log.elog("no fill");
			Py_XDECREF(main);
			Py_XDECREF(droneChildren);
			Py_XDECREF(gauge);
			Py_XDECREF(children);
			Py_XDECREF(frame);
			clearExceptions();
			return NULL;
		}

		PyObject * frameWidth = _getAttribute(frame, "displayWidth");
		if(frameWidth == NULL)	
		{
			log.elog("no frame");
			Py_XDECREF(main);
			Py_XDECREF(droneChildren);
			Py_XDECREF(gauge);
			Py_XDECREF(children);
			Py_XDECREF(frame);
			Py_XDECREF(fillWidth);
			clearExceptions();
			return NULL;
		}

		if(PyInt_AsLong(fillWidth) + 10 < PyInt_AsLong(frameWidth))
		{
			PyObject * width = NULL, * height = NULL, * absoluteLeft = NULL, * absoluteTop = NULL;
			bool ok = _populateAttributesDisplay(entry, &width, &height, &absoluteTop, &absoluteLeft);
			if(!ok)
			{
				log.elog("couldn't populate");
				Py_XDECREF(main);
				Py_XDECREF(droneChildren);
				Py_XDECREF(gauge);
				Py_XDECREF(children);
				Py_XDECREF(frame);
				Py_XDECREF(fillWidth);
				clearExceptions();
				return NULL;
			}
			
			char * output = builder.buildInterfaceObject("drone", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteLeft), PyInt_AsLong(width), PyInt_AsLong(height), size);
			Py_XDECREF(main);
			Py_XDECREF(droneChildren);
			Py_XDECREF(gauge);
			Py_XDECREF(children);
			Py_XDECREF(frame);
			Py_XDECREF(fillWidth);
			Py_XDECREF(width);
			Py_XDECREF(height);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(absoluteTop);
			return output;
		}
			
		entry = _findByNameLayer(droneChildren, os.str());
	}

	return NULL;
	
}

char * Interfaces::_getDroneStatus(int & size)
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		clearExceptions();
		return NULL;
	}
	PyObject * droneChildren = _findByNameLayer(main, "droneview");
	
	if(droneChildren == NULL)
	{
		log.elog("Couldn't get children");
		Py_XDECREF(main);
		clearExceptions();
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
		Py_XDECREF(main);
		Py_XDECREF(droneChildren);
		clearExceptions();
		return NULL;
	}

	PyObject * label = _findByNameLayer(entry, "EveLabelMedium");
	if(label == NULL)
	{
		log.elog("Couldn't get label");
		Py_XDECREF(main);
		Py_XDECREF(droneChildren);
		Py_XDECREF(entry);
		clearExceptions();
		return NULL;
	}

	PyObject * text = _getAttribute(label,"text");
	if(text == NULL)
	{
		Py_XDECREF(main);
		Py_XDECREF(droneChildren);
		Py_XDECREF(label);
		Py_XDECREF(entry);
		clearExceptions();
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	bool ok = _populateAttributes(label, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate attributes");
		Py_XDECREF(main);
		Py_XDECREF(droneChildren);
		Py_XDECREF(label);
		Py_XDECREF(text);
		Py_XDECREF(entry);
		clearExceptions();
		return NULL;
	}

	char * ctext = PyString_AsString(text);
	char * output = builder.buildInterfaceObject(ctext, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(main);
	Py_XDECREF(droneChildren);
	Py_XDECREF(label);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(text);
	Py_XDECREF(entry);
	clearExceptions();

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
		clearExceptions();
		return NULL;
	}
	PyObject * droneChildren = _findByNameLayer(main, droneType);
	
	if(droneChildren == NULL)
	{
		log.elog("Couldn't get children");
		Py_XDECREF(main);
		clearExceptions();
		return NULL;
	}

	PyObject * label = _findByNameLayer(droneChildren, "EveLabelMedium");
	if(label == NULL)
	{
		log.elog("Couldn't get label");
		Py_XDECREF(main);
		Py_XDECREF(droneChildren);
		clearExceptions();
		return NULL;
	}

	PyObject * text = _getAttribute(label,"text");
	if(text == NULL)
	{
		Py_XDECREF(main);
		Py_XDECREF(droneChildren);
		Py_XDECREF(label);
		clearExceptions();
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	bool ok = _populateAttributes(label, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate attributes");
		Py_XDECREF(main);
		Py_XDECREF(droneChildren);
		Py_XDECREF(label);
		Py_XDECREF(text);
		clearExceptions();
		return NULL;
	}

	char * ctext = PyString_AsString(text);
	char * output = builder.buildInterfaceObject(ctext, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(main);
	Py_XDECREF(droneChildren);
	Py_XDECREF(label);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(text);
	clearExceptions();

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
		clearExceptions();
		fleeted = true;
	}
	char * output = builder.buildBooleanObject(fleeted, size);
	Py_XDECREF(main);
	if(fleetwindow != NULL)
		Py_XDECREF(fleetwindow);
	PyGILState_Release(gstate);
	return output;
}

void Interfaces::_strToLower(string & str)
{
	for(int i = 0; i < str.length(); i++)
		str[i] = tolower(str[i]);
}


char * Interfaces::_getAgentButton(string name, int & size)
{
	PyObject * agentWindow = _getAgentWindow();
	if(agentWindow == NULL)
	{
		clearExceptions();
		log.elog("Couldn't get agent window");
		return NULL;
	}

	PyObject * agentButton = _findByNameLayer(agentWindow, name);
	if(agentButton == NULL)
	{
		log.elog("Couldn't get button");
		Py_XDECREF(agentWindow);
		clearExceptions();
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(agentButton, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate");
		Py_XDECREF(agentWindow);
		Py_XDECREF(agentButton);
		clearExceptions();
		return NULL;
	}

	char * output = builder.buildInterfaceObject("AgentButton", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(agentWindow);
	Py_XDECREF(agentButton);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	clearExceptions();
	return output;
}

char * Interfaces::GetAgentReqMissionBtn(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getAgentButton("Request Mission_Btn", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetAgentMissionCloseBtn(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getAgentButton("Close_Btn", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetAgentMissionAcceptBtn(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getAgentButton("Accept_Btn", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetAgentMissionDeclineBtn(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getAgentButton("Decline_Btn", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetAgentMissionQuitBtn(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getAgentButton("Quit Mission_Btn", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetAgentMissionText(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * agentWindow = _getAgentWindow();
	if(agentWindow == NULL)
	{
		log.elog("Couldn't get the agent window");
		clearExceptions();
		PyGILState_Release(gstate);
		
		return NULL;
	}

	PyObject * rightSide = _findByNameLayer(agentWindow, "rightPane");
	if(rightSide == NULL)
	{
		log.elog("Couldn't get the right side");
		Py_XDECREF(agentWindow);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * lines = _findByNameLayer(rightSide, "edit_multiline");
	if(lines == NULL)
	{
		log.elog("Couldn't get the lines");
		Py_XDECREF(agentWindow);
		Py_XDECREF(rightSide);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * sr = _getAttribute(lines, "sr");
	if(sr == NULL)
	{
		log.elog("Couldn't get the rows");
		Py_XDECREF(agentWindow);
		Py_XDECREF(rightSide);
		Py_XDECREF(lines);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * currentTXT = _getAttribute(sr, "currentTXT");
	if(currentTXT == NULL)
	{
		log.elog("Couldn't get the text");
		Py_XDECREF(agentWindow);
		Py_XDECREF(rightSide);
		Py_XDECREF(lines);
		Py_XDECREF(sr);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildStringObject(PyString_AsString(currentTXT), size);
	Py_XDECREF(agentWindow);
	Py_XDECREF(rightSide);
	Py_XDECREF(lines);
	Py_XDECREF(sr);
	Py_XDECREF(currentTXT);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetAgentMissionCompleteBtn(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getAgentButton("Complete Mission_Btn", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetAgentMissionDelayBtn(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getAgentButton("Delay_Btn", size);
	PyGILState_Release(gstate);
	return output;
}

PyObject * Interfaces::_getAgentWindow()
{
	PyObject * main = _getLayer("main");
	
	if(main == NULL)
	{
		log.elog("couldn't get main");
		clearExceptions();
		return NULL;
	}

	PyObject * children = _getAttribute(main, "children");
	if(children == NULL)
	{
		log.elog("no children");
		Py_XDECREF(main);
		clearExceptions();
		return NULL;
	}

	PyObject * pkey = NULL, *pvalue = NULL;
	for(int i = 0; i < PyObject_Size(children); i++)
	{
		pkey = PyInt_FromLong(i);
		pvalue = PyObject_GetItem(children, pkey);

		log.elog(PyEval_GetFuncName(pvalue));

		if(strcmp(PyEval_GetFuncName(pvalue), "AgentDialogueWindow") == 0)
		{
			Py_XDECREF(main);
			Py_XDECREF(children);
			clearExceptions();
			return pvalue;
		}

		Py_XDECREF(pvalue);
	}

	Py_XDECREF(main);
	Py_XDECREF(children);
	clearExceptions();
	return NULL;
}

char * Interfaces::GetAgent(string agentname, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	
	PyObject * bottom = _getStationLobbyBottom();
	if(bottom == NULL)
	{
		log.elog("Couldn't get the lobby");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * agent = _findByNameLayer(bottom, agentname);
	if(agent == NULL)
	{
		log.elog("Couldn't find agent");
		Py_XDECREF(bottom);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * width = NULL, * height = NULL, *absoluteLeft = NULL, *absoluteTop = NULL;

	bool ok = _populateAttributes(agent, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("couldn't populate");
		Py_XDECREF(bottom);
		Py_XDECREF(agent);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;

	}

	char * output = builder.buildInterfaceObject(agentname, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	
	Py_XDECREF(bottom);
	Py_XDECREF(agent);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;

}

void Interfaces::stoupper(std::string& s)
{
	  std::string::iterator i = s.begin();
	  std::string::iterator end = s.end();
	 
	  while (i != end) 
	  {
		
		*i = toupper((unsigned char)*i);
		++i;
	  }
}

char * Interfaces::_getProbeButton(string name, int & size)
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("couldn't get main");
		clearExceptions();
		return NULL;
	}

	PyObject * settings = _findByNameLayer(main, "systemSettings");
	if(settings == NULL)
	{
		log.elog("no settings");
		Py_XDECREF(main);
		clearExceptions();
		return NULL;
	}

	PyObject * children = _getAttribute(settings, "children");
	if(children == NULL)
	{
		log.elog("no children");
		Py_XDECREF(main);
		Py_XDECREF(settings);
		clearExceptions();
		return NULL;
	}

	
	int csize = PyObject_Size(children);

	PyObject * pkey = NULL;
	
	int pos = -1;

	if(name.compare("analyze") == 0)
	{
		pos = 0;
	}
	else if(name.compare("recover") == 0)
	{
		pos = 1;
	}

	if(csize <= pos || pos == -1)
	{
		log.elog("children too small");
		Py_XDECREF(main);
		Py_XDECREF(settings);
		Py_XDECREF(children);
		clearExceptions();
		return NULL;
	}
		
	pkey = PyInt_FromLong(pos);
	
	PyObject * button = PyObject_GetItem(children, pkey);
	if(button == NULL)
	{
		log.elog("couldn't get the button");
		Py_XDECREF(main);
		Py_XDECREF(settings);
		Py_XDECREF(children);
		clearExceptions();
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteLeft = NULL, * absoluteTop = NULL;
	bool ok = _populateAttributes(button, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("error populating");
		Py_XDECREF(main);
		Py_XDECREF(settings);
		Py_XDECREF(children);
		clearExceptions();
		return NULL;
	}

	char * output = builder.buildInterfaceObject(name, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);

	Py_XDECREF(main);
	Py_XDECREF(settings);
	Py_XDECREF(children);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(height);
	Py_XDECREF(width);
	clearExceptions();
	return output;
}

char * Interfaces::GetAnalyzeProbesButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getProbeButton("analyze", size);
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::GetRecoverProbesButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getProbeButton("recover", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetProbe(string name, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("couldn't get main");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * scanner = _findByNameLayer(main, "scanner");
	if(scanner == NULL)
	{
		log.elog("couldn't get scanner");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * system = _findByNameLayer(scanner, "system");
	if(system == NULL)
	{
		log.elog("couldn't get probe window");
		Py_XDECREF(main);
		Py_XDECREF(scanner);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = _findColumnEntryProbeWindow(name, system, size);
	Py_XDECREF(main);
	Py_XDECREF(scanner);
	Py_XDECREF(system);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::GetProbeResult(string name, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("couldn't get main");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * scanner = _findByNameLayer(main, "scanner");
	if(scanner == NULL)
	{
		log.elog("couldn't get scanner");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * resultscroll = _findByNameLayer(scanner, "resultscroll");
	if(resultscroll == NULL)
	{
		log.elog("couldn't get probe window");
		Py_XDECREF(main);
		Py_XDECREF(scanner);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = _findColumnEntryProbeWindow(name, resultscroll, size);
	Py_XDECREF(main);
	Py_XDECREF(scanner);
	Py_XDECREF(resultscroll);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::_getMarketOrders(string type, int & size)
{
	
	list<ObjectBuilder::targetEntry*> entries;
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("couldn't get main");
		clearExceptions();
		return NULL;
	}

	PyObject * marketbase = _findByNameLayer(main, "MarketBase");
	if(marketbase == NULL)
	{
		log.elog("couldn't get market");
		Py_XDECREF(main);
		clearExceptions();
		return NULL;
	}

	PyObject * orders = _findByNameLayer(marketbase,type);
	if(orders == NULL)
	{
		log.elog("couldn't get orders");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		clearExceptions();
		return NULL;
	}

	PyObject * entry = _findByNameLayer(orders, "entry_0");
	stringstream os;
	os << "entry_";

	for(int i = 1; entry != NULL; i++)
	{
		PyObject * ptext = _findByNameLayer(entry, "EveLabelMedium");
		if(ptext == NULL)
		{
			log.elog("couldn't get ptext");
			Py_XDECREF(entry);
			Py_XDECREF(orders);
			Py_XDECREF(main);
			Py_XDECREF(marketbase);
			clearExceptions();
			return NULL;
		}

		PyObject * text = _getAttribute(ptext, "text");
		if(text == NULL)
		{
			log.elog("couldn't get text");
			Py_XDECREF(entry);
			Py_XDECREF(orders);
			Py_XDECREF(main);
			Py_XDECREF(ptext);
			Py_XDECREF(marketbase);
			clearExceptions();
			return NULL;
		}

		PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
		bool ok = _populateAttributes(entry, &width, &height, &absoluteTop, &absoluteLeft);
		if(!ok)
		{
			log.elog("couldn't populate");
			Py_XDECREF(entry);
			Py_XDECREF(orders);
			Py_XDECREF(main);
			Py_XDECREF(ptext);
			Py_XDECREF(marketbase);
			Py_XDECREF(text);
			clearExceptions();
			return NULL;
		}

		ObjectBuilder::targetEntry * tEntry = new ObjectBuilder::targetEntry();
		tEntry->name = PyString_AsString(text);
		tEntry->height = PyInt_AsLong(height);
		tEntry->width = PyInt_AsLong(width);
		tEntry->topLeftX = PyInt_AsLong(absoluteLeft);
		tEntry->topLeftY = PyInt_AsLong(absoluteTop);
		entries.push_back(tEntry);



		Py_XDECREF(entry);
		Py_XDECREF(ptext);
		Py_XDECREF(text);
		Py_XDECREF(absoluteLeft);
		Py_XDECREF(absoluteTop);
		Py_XDECREF(width);
		Py_XDECREF(height);
		

		os.str("");
		os << "entry_" << i;

		log.elog(os.str());
		entry = _findByNameLayer(orders, os.str());
	}

	char * output = builder.buildTargetObject(entries, size);

	Py_XDECREF(main);
	Py_XDECREF(marketbase);
	Py_XDECREF(orders);

	for(list<ObjectBuilder::targetEntry *>::iterator it = entries.begin(); it != entries.end(); it++)
	{
		delete (*it);
	}

	clearExceptions();
	return output;
}

char * Interfaces::GetMarketSearchResult(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getMarketOrders("leftSide", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetBuyOrders(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getMarketOrders("sellParent", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetSellOrders(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getMarketOrders("buyParent", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::_findColumnEntryProbeWindow(string name, PyObject * layer, int & size)
{
	PyObject * clipper = _findByNameLayer(layer, "__clipper");
	if(clipper == NULL)
	{
		log.elog("couldn't get the area");
		clearExceptions();
		return NULL;
	}

	PyObject * entry = _findByNameLayer(clipper, "entry_0");
	stringstream os;
	os << "entry_";

	for(int i = 1; entry != NULL; i++)
	{
		PyObject * column = _findByNameLayer(entry, "column_3");
		if(column == NULL)
		{
			log.elog("no column");
			Py_XDECREF(entry);
			Py_XDECREF(clipper);
			clearExceptions();
			return NULL;
		}
		
		PyObject * ptext = _findByNameLayer(column, "text");
		if(ptext == NULL)
		{
			log.elog("couldn't get ptext");
			Py_XDECREF(entry);
			Py_XDECREF(clipper);
			Py_XDECREF(column);
			clearExceptions();
			return NULL;
		}

		PyObject * text = _getAttribute(ptext, "text");
		if(text == NULL)
		{
			log.elog("couldn't get text");
			Py_XDECREF(entry);
			Py_XDECREF(clipper);
			Py_XDECREF(column);
			Py_XDECREF(ptext);
			clearExceptions();
			return NULL;
		}

		if(strcmp(PyString_AsString(text), name.c_str()) == 0)
		{
			PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
			bool ok = _populateAttributes(entry, &width, &height, &absoluteTop, &absoluteLeft);
			if(!ok)
			{
				log.elog("couldn't populate");
				Py_XDECREF(entry);
				Py_XDECREF(clipper);
				Py_XDECREF(column);
				Py_XDECREF(ptext);
				Py_XDECREF(text);
				clearExceptions();
				return NULL;
			}

			char * output = builder.buildInterfaceObject(name, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
			Py_XDECREF(entry);
			Py_XDECREF(clipper);
			Py_XDECREF(column);
			Py_XDECREF(ptext);
			Py_XDECREF(text);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(absoluteTop);
			Py_XDECREF(width);
			Py_XDECREF(height);
			clearExceptions();
			return output;

		}


		os.str("");
		os << "entry_" << i;

		log.elog(os.str());
		entry = _findByNameLayer(clipper, os.str());
	}

	return NULL;
}

char * Interfaces::GetMarketSearchEditContent(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("no main");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * marketbase = _findByNameLayer(main, "MarketBase");
	if(marketbase == NULL)
	{
		log.elog("no market");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * leftSide = _findByNameLayer(marketbase, "leftSide");
	if(leftSide == NULL)
	{
		log.elog("no search");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * searchparent = _findByNameLayer(leftSide, "searchparent");
	if(searchparent == NULL)
	{
		log.elog("no searcharea");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		Py_XDECREF(leftSide);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * edit = _findByNameLayer(searchparent, "edit");
	if(edit == NULL)
	{
		log.elog("no field");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		Py_XDECREF(leftSide);
		Py_XDECREF(searchparent);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * text = _getAttribute(edit, "text");
	if(text == NULL)
	{
		log.elog("no text");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		Py_XDECREF(leftSide);
		Py_XDECREF(searchparent);
		Py_XDECREF(edit);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	char * output = builder.buildStringObject(PyString_AsString(text), size);
	Py_XDECREF(main);
	Py_XDECREF(marketbase);
	Py_XDECREF(leftSide);
	Py_XDECREF(searchparent);
	Py_XDECREF(edit);
	Py_XDECREF(text);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::GetMarketSearchButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("no main");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * marketbase = _findByNameLayer(main, "MarketBase");
	if(marketbase == NULL)
	{
		log.elog("no market");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * leftSide = _findByNameLayer(marketbase, "leftSide");
	if(leftSide == NULL)
	{
		log.elog("no search");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * searchparent = _findByNameLayer(leftSide, "searchparent");
	if(searchparent == NULL)
	{
		log.elog("no searcharea");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		Py_XDECREF(leftSide);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * button = _findByNameLayer(searchparent, "button");
	if(button == NULL)
	{
		log.elog("no field");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		Py_XDECREF(leftSide);
		Py_XDECREF(searchparent);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width=NULL, *height = NULL, * absoluteTop=NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(button, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("couldn't populate");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		Py_XDECREF(leftSide);
		Py_XDECREF(searchparent);
		Py_XDECREF(button);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildInterfaceObject("SearchButton", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(main);
	Py_XDECREF(marketbase);
	Py_XDECREF(leftSide);
	Py_XDECREF(searchparent);
	Py_XDECREF(button);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetMarketSearchEdit(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("no main");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * marketbase = _findByNameLayer(main, "MarketBase");
	if(marketbase == NULL)
	{
		log.elog("no market");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * leftSide = _findByNameLayer(marketbase, "leftSide");
	if(leftSide == NULL)
	{
		log.elog("no search");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * searchparent = _findByNameLayer(leftSide, "searchparent");
	if(searchparent == NULL)
	{
		log.elog("no searcharea");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		Py_XDECREF(leftSide);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * edit = _findByNameLayer(searchparent, "edit");
	if(edit == NULL)
	{
		log.elog("no field");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		Py_XDECREF(leftSide);
		Py_XDECREF(searchparent);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width=NULL, *height = NULL, * absoluteTop=NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributesDisplay(edit, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("couldn't populate");
		Py_XDECREF(main);
		Py_XDECREF(marketbase);
		Py_XDECREF(leftSide);
		Py_XDECREF(searchparent);
		Py_XDECREF(edit);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildInterfaceObject("SearchArea", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(main);
	Py_XDECREF(marketbase);
	Py_XDECREF(leftSide);
	Py_XDECREF(searchparent);
	Py_XDECREF(edit);
	clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * local = _findByNameLayer(main, "chatchannel_solarsystemid2");
	if(local == NULL)
	{
		log.elog("couldn't get local");
		Py_XDECREF(main);
		clearExceptions();
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
			Py_XDECREF(entry);
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
			Py_XDECREF(main);
			Py_XDECREF(local);
			Py_XDECREF(entry);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * node = _getAttribute(sr, "node");
		if(node == NULL)
		{
			log.elog("no node");
			Py_XDECREF(main);
			Py_XDECREF(local);
			Py_XDECREF(entry);
			Py_XDECREF(sr);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * label = _getAttribute(node, "label");
		if(label == NULL)
		{
			log.elog("no label");
			Py_XDECREF(main);
			Py_XDECREF(local);
			Py_XDECREF(entry);
			Py_XDECREF(sr);
			Py_XDECREF(node);
			clearExceptions();
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
				Py_XDECREF(main);
				Py_XDECREF(local);
				Py_XDECREF(entry);
				Py_XDECREF(sr);
				Py_XDECREF(node);
				Py_XDECREF(label);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}
			
			PyObject * width = NULL, * height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
			bool ok = _populateAttributes(entry, &width, &height, &absoluteTop, &absoluteLeft);
			if(!ok)
			{
				log.elog("issue populating");
				Py_XDECREF(main);
				Py_XDECREF(local);
				Py_XDECREF(entry);
				Py_XDECREF(sr);
				Py_XDECREF(node);
				Py_XDECREF(label);
				Py_XDECREF(state);
				clearExceptions();
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
			Py_XDECREF(main);
			Py_XDECREF(local);
			Py_XDECREF(entry);
			Py_XDECREF(sr);
			Py_XDECREF(node);
			Py_XDECREF(label);
			Py_XDECREF(state);
			Py_XDECREF(absoluteTop);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(width);
			Py_XDECREF(height);
			clearExceptions();
			PyGILState_Release(gstate);
			return output;
		}
		
		Py_XDECREF(entry);
		Py_XDECREF(sr);
		Py_XDECREF(node);
		Py_XDECREF(label);

		log.elog(os.str());
		entry = _findByNameLayer(local, os.str());
	}
	
	Py_XDECREF(main);
	Py_XDECREF(local);
	clearExceptions();
	PyGILState_Release(gstate);
	return NULL;
}

char * Interfaces::GetOverViewSelectIcon(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("couldn't get main");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * headerIcon = _findByNameLayer(main, "overviewHeaderIcon");
	if(headerIcon == NULL)
	{
		log.elog("couldn't get the icon");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteLeft = NULL, *absoluteTop = NULL;
	bool ok = _populateAttributes(headerIcon,&width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("error populating");
		Py_XDECREF(main);
		Py_XDECREF(headerIcon);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("overvIcon", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(main);
	Py_XDECREF(headerIcon);
	Py_XDECREF(height);
	Py_XDECREF(width);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;
}


char * Interfaces::GetOverviewSelectText(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("couldn't get main");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * overview = _findByNameLayer(main, "overview");
	if(overview == NULL)
	{
		log.elog("Couldn't get overview");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * captionP = _findByNameLayer(overview, "captionParent");
	if(captionP == NULL)
	{
		log.elog("Couldn't get caption");
		Py_XDECREF(main);
		Py_XDECREF(overview);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * label = _findByNameLayer(captionP, "EveLabelSmall");
	if(label == NULL)
	{
		log.elog("couldn't get the label");
		Py_XDECREF(main);
		Py_XDECREF(overview);
		Py_XDECREF(captionP);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * text = _getAttribute(label, "text");
	if(text == NULL)
	{
		log.elog("Couldn't get the text");
		Py_XDECREF(main);
		Py_XDECREF(overview);
		Py_XDECREF(captionP);
		Py_XDECREF(label);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildStringObject(PyString_AsString(text), size);
	Py_XDECREF(main);
	Py_XDECREF(overview);
	Py_XDECREF(captionP);
	Py_XDECREF(label);
	Py_XDECREF(text);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::CheckLocal(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * local = _findByNameLayer(main, "chatchannel_solarsystemid2");
	if(local == NULL)
	{
		log.elog("couldn't get local");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * entry = _findByNameLayer(local, "entry_0");
	stringstream os;
	os << "entry_";

	bool onealloted = false;

	for(int i = 1; entry != NULL; i++)
	{
		os.str("");
		os << "entry_" << i;
		PyObject * flag = _findByNameLayer(entry, "flag");
		if(flag == NULL)
		{
			log.elog("No flag");
			Py_XDECREF(main);
			Py_XDECREF(local);
			Py_XDECREF(entry);
			clearExceptions();
			PyGILState_Release(gstate);
			return builder.buildBooleanObject(true, size);
		}
		
		PyObject * fill = _findByNameLayer(flag, "fill");
		if(fill == NULL)
		{
			if(!onealloted)
			{
				onealloted = true;
				Py_XDECREF(flag);
				Py_XDECREF(entry);
				continue;
			}
			log.elog("fill is null");
			Py_XDECREF(main);
			Py_XDECREF(local);
			Py_XDECREF(entry);
			Py_XDECREF(flag);
			clearExceptions();
			PyGILState_Release(gstate);
			return builder.buildBooleanObject(true, size);
		}

		PyObject * color  = _getAttribute(fill, "color");
		if(color == NULL)
		{
			log.elog("color is null");
			Py_XDECREF(main);
			Py_XDECREF(local);
			Py_XDECREF(entry);
			Py_XDECREF(fill);
			Py_XDECREF(flag);
			clearExceptions();
			PyGILState_Release(gstate);
			return builder.buildBooleanObject(true, size);
		}
		
		PyObject * g = _getAttribute(color, "g");
		if(g == NULL)
		{
			log.elog("g is null");
			Py_XDECREF(main);
			Py_XDECREF(local);
			Py_XDECREF(entry);
			Py_XDECREF(fill);
			Py_XDECREF(color);
			Py_XDECREF(flag);
			clearExceptions();
			PyGILState_Release(gstate);
			return builder.buildBooleanObject(true, size);
		}

		double c = PyFloat_AsDouble(g);
		if(c == 0.7 || c == 0.0 || c == 0.35)
		{
			Py_XDECREF(main);
			Py_XDECREF(local);
			Py_XDECREF(entry);
			Py_XDECREF(flag);
			Py_XDECREF(fill);
			Py_XDECREF(color);
			Py_XDECREF(g);
			clearExceptions();
			PyGILState_Release(gstate);
			return builder.buildBooleanObject(true, size);
		}
	
		Py_XDECREF(flag);
		Py_XDECREF(fill);
		Py_XDECREF(color);
		Py_XDECREF(g);
		Py_XDECREF(entry);
	
		log.elog(os.str());
		entry = _findByNameLayer(local, os.str());
	}
	
	Py_XDECREF(main);
	Py_XDECREF(local);
	clearExceptions();
	PyGILState_Release(gstate);
	return builder.buildBooleanObject(false, size);
}

char * Interfaces::IsIncursion(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * neocom = _getLayer("neocom");
	if(neocom == NULL)
	{
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * neocomLeftSide = _findByNameLayer(neocom, "neocomLeftside");
	if(neocomLeftSide == NULL)
	{
		log.elog("Couldn't get the left side");
		Py_XDECREF(neocom);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	
	PyObject * locationInfo = _findByNameLayer(neocomLeftSide,  "IncursionInfoContainer");
	if(locationInfo == NULL)
	{
		log.elog("IncursionInfoContainer is null");
		Py_XDECREF(neocom);
		Py_XDECREF(neocomLeftSide);
		char * output = builder.buildBooleanObject(false, size);
		clearExceptions();
		PyGILState_Release(gstate);
		return output;
	}	

	char * output = builder.buildBooleanObject(true, size);
	Py_XDECREF(neocom);
	Py_XDECREF(locationInfo);
	Py_XDECREF(neocomLeftSide);
	clearExceptions();
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

char * Interfaces::GetModalSubmitButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("couldn't get main");
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * submit = _findByNameLayer(main, "Submit_Btn");
	if(submit == NULL)
	{
		log.elog("couldn't get submit");
		Py_XDECREF(main);
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteLeft = NULL, * absoluteTop = NULL;
	bool ok = _populateAttributes(submit, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate");
		Py_XDECREF(main);
		Py_XDECREF(submit);
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("submit", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
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

char * Interfaces::GetLocalWritingArea(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("couldn't get main");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * chatchannel = _findByNameLayer(main, "chatchannel_solarsystemid2");
	if(chatchannel == NULL)
	{
		log.elog("cant get the channel");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * input = _findByNameLayer(chatchannel, "inputchatchannel_solarsystemid2");
	if(input == NULL)
	{
		log.elog("cant get the input row");
		Py_XDECREF(main);
		Py_XDECREF(chatchannel);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, *height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributesDisplay(input, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Error populating");
		Py_XDECREF(main);
		Py_XDECREF(chatchannel);
		Py_XDECREF(input);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("localinput", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(main);
	Py_XDECREF(chatchannel);
	Py_XDECREF(input);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetLocalChatText(int sysid, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	list<string *> chatlist;
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * chatchannel = _findByNameLayer(main, "chatchannel_solarsystemid2");
	if(chatchannel == NULL)
	{
		log.elog("Couldn't get the channel");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	stringstream os;
	os << "chatoutput_(\'solarsystemid2\', " << sysid << ")";
	 
	PyObject * channelparent = _findByNameLayer(chatchannel, os.str());
	if(channelparent == NULL)
	{
		log.elog("Couldn't get the channel parent");
		Py_XDECREF(main);
		Py_XDECREF(chatchannel);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * content = _findByNameLayer(channelparent, "__content");
	if(content == NULL)
	{
		log.elog("couldn't get the container");
		Py_XDECREF(main);
		Py_XDECREF(chatchannel);
		Py_XDECREF(channelparent);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * children = _getAttribute(content, "children");
	if(children == NULL)
	{
		log.elog("Couldn't get the children");
		Py_XDECREF(main);
		Py_XDECREF(chatchannel);
		Py_XDECREF(channelparent);
		Py_XDECREF(content);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	int csize = PyObject_Size(children);

	PyObject * pvalue = NULL;
	PyObject * pkey = NULL;
	
	int start = 0;

	if(csize > 15)
		start = csize - 15;
		

	for(int i = start; i < csize; i++)
	{
		pkey = PyInt_FromLong(i);
		pvalue = PyObject_GetItem(children, pkey);
		
		if(pvalue == NULL)
		{
			log.elog("Couldn't get the value");
			Py_XDECREF(main);
			Py_XDECREF(chatchannel);
			Py_XDECREF(channelparent);
			Py_XDECREF(content);
			Py_XDECREF(children);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}
		
		PyObject * label = _findByNameLayer(pvalue, "text");
		if(label == NULL)
		{
			log.elog("Couldn't get the label");
			Py_XDECREF(main);
			Py_XDECREF(chatchannel);
			Py_XDECREF(channelparent);
			Py_XDECREF(content);
			Py_XDECREF(children);
			Py_XDECREF(pvalue);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * text = _getAttribute(label, "text");
		if(text == NULL)
		{
			log.elog("Couldn't get the text");
			Py_XDECREF(main);
			Py_XDECREF(chatchannel);
			Py_XDECREF(channelparent);
			Py_XDECREF(content);
			Py_XDECREF(children);
			Py_XDECREF(pvalue);
			Py_XDECREF(label);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		chatlist.push_back(new string(PyString_AsString(text)));
		Py_XDECREF(pvalue);
		Py_XDECREF(label);
		Py_XDECREF(text);

	}

	char * output = builder.buildStringListObject(chatlist, size);
	for(list<string *>::iterator it = chatlist.begin(); it != chatlist.end(); it++)
	{
		delete (*it);		
	}

	Py_XDECREF(main);
	Py_XDECREF(chatchannel);
	Py_XDECREF(channelparent);
	Py_XDECREF(content);
	Py_XDECREF(children);
	clearExceptions();
	PyGILState_Release(gstate);

	return output;

}

char * Interfaces::GetCurrentSolarsystemid(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
		
	PyObject * main = PyImport_AddModule("__builtin__");
	if(main == NULL)
	{
		log.elog("Main failed to load");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * maindic = PyModule_GetDict(main);
	
	if(maindic == NULL)
	{
		log.elog("Couldn't load main dictionary");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * eve = PyDict_GetItemString(maindic, "eve");
	if(eve == NULL)
	{
		log.elog("Couldn't get eve");
		Py_XDECREF(main);
		Py_XDECREF(maindic);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * session = _getAttribute(eve, "session");
	if(session == NULL)
	{
		log.elog("Couldn't get session");
		Py_XDECREF(main);
		Py_XDECREF(maindic);
		Py_XDECREF(eve);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * solarsystemid2 = _getAttribute(session, "solarsystemid2");
	if(solarsystemid2 == NULL)
	{
		log.elog("couldn't get the id");
		Py_XDECREF(main);
		Py_XDECREF(maindic);
		Py_XDECREF(eve);
		Py_XDECREF(session);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	stringstream os;
	os << PyInt_AsLong(solarsystemid2);

	char * output = builder.buildStringObject(os.str(), size);
	Py_XDECREF(main);
	Py_XDECREF(maindic);
	Py_XDECREF(eve);
	Py_XDECREF(session);
	Py_XDECREF(solarsystemid2);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;

}

PyObject * Interfaces::_getSysMenuButtonByText(string ctext)
{
	PyObject * modal = _getLayer("modal");
	if(modal == NULL)
	{
		log.elog("Couldn't get modal");
		clearExceptions();
		return NULL;
	}

	PyObject * system_menu = _findByNameLayer(modal, "l_systemmenu");
	if(system_menu == NULL)
	{
		log.elog("Couldn't get system menu");
		Py_XDECREF(modal);
		clearExceptions();
		return NULL;
	}
	
	PyObject * btnPar = _findByNameLayer(system_menu, "btnPar");
	if(btnPar == NULL)
	{
		log.elog("Couldn't get buttons");
		Py_XDECREF(modal);
		Py_XDECREF(system_menu);
		clearExceptions();
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
			Py_XDECREF(modal);
			Py_XDECREF(system_menu);
			Py_XDECREF(children);
			clearExceptions();
			return NULL;
		}
		
		PyObject * text = _getAttribute(pvalue, "text");
		if(text == NULL)
		{
			Py_XDECREF(modal);
			Py_XDECREF(system_menu);
			Py_XDECREF(children);
			Py_XDECREF(pvalue);
			clearExceptions();
			return NULL;
		}

		//log.elog(PyEval_GetFuncName(pvalue));
		if(strcmp(PyString_AsString(text), ctext.c_str()) == 0)
		{
			log.elog("Found button");
			Py_XDECREF(modal);
			Py_XDECREF(system_menu);
			Py_XDECREF(children);
			Py_XDECREF(text);
			clearExceptions();
			return pvalue;
		}

		Py_XDECREF(pvalue);
	}
	
	clearExceptions();
	return NULL;

}

char * Interfaces::GetLogOffButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * button = _getSysMenuButtonByText("Log off");

	if(button == NULL)
	{
		log.elog("Couldn't get the button");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;

	bool ok = _populateAttributes(button, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("something went wrong in populating");
		Py_XDECREF(button);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
		
	char * output = builder.buildInterfaceObject("logoffButton", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(button);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetBookMarkFieldName(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * newbm = _findByNameLayer(main, "labelEdit");
	if(newbm == NULL)
	{
		log.elog("Couldn't get new bm");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * text = _getAttribute(newbm, "text");
	if(text == NULL)
	{
		log.elog("Couldn't get the text");
		Py_XDECREF(main);
		Py_XDECREF(newbm);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildStringObject(PyString_AsString(text), size);
	Py_XDECREF(main);
	Py_XDECREF(newbm);
	Py_XDECREF(text);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;

}


char * Interfaces::_getModalButton(string name, int & size)
{
	PyObject * modal = _getLayer("modal");
	if(modal == NULL)
	{
		log.elog("Couldn't get modal");
		clearExceptions();
		return NULL;
	}

	PyObject * button = _findByNameLayer(modal, name);

	if(button == NULL)
	{
		log.elog("Doesn't have button");
		Py_XDECREF(modal);
		clearExceptions();
		return NULL;
	}

	PyObject * height = NULL, * width = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	
	bool ok = _populateAttributes(button, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		Py_XDECREF(button);
		Py_XDECREF(modal);
		clearExceptions();
		return NULL;
	}
	
	char * output = builder.buildInterfaceObject(name, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(button);
	Py_XDECREF(modal);
	clearExceptions();

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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * sysmenu = NULL;
	sysmenu = _findByNameLayer(modal, "sysmenu");

	if(sysmenu == NULL)
	{
		clearExceptions();
		output = builder.buildBooleanObject(false, size);
	}
	else
	{
		output = builder.buildBooleanObject(true, size);
		Py_XDECREF(sysmenu);
		clearExceptions();
	}
	
	Py_XDECREF(modal);
	clearExceptions();
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
		clearExceptions();
		return NULL;
	}

	_findByText(layer, label, &result);

	if(result == NULL)
	{
		log.elog("Result is NULL");
		Py_XDECREF(layer);
		clearExceptions();
		return NULL;
	}

	PyObject * leftPosVal, * topPosVal, * name, * width, * height;
	
	leftPosVal = _getAbsoluteLeft(result);
	
	if(leftPosVal == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(result);
		clearExceptions();
		return NULL;
	}

	topPosVal = _getAbsoluteTop(result);
	if(topPosVal == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(leftPosVal);
		Py_XDECREF(result);
		clearExceptions();
		return NULL;
	}

	name = _getName(result);
	if(name == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(leftPosVal);
		Py_XDECREF(topPosVal);
		Py_XDECREF(result);
		clearExceptions();
		return NULL;
	}

	height = _getHeight(result);
	if(height == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(leftPosVal);
		Py_XDECREF(topPosVal);
		Py_XDECREF(name);
		Py_XDECREF(result);
		clearExceptions();
		return NULL;
	}

	width = _getWidth(result);
	if(height == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(leftPosVal);
		Py_XDECREF(topPosVal);
		Py_XDECREF(name);
		Py_XDECREF(height);
		Py_XDECREF(result);
		clearExceptions();
		return NULL;
	}

	log.elog("Found Child");
	char * iname = PyString_AsString(name);
	output = builder.buildInterfaceObject(iname,  (int)PyInt_AsLong(leftPosVal),(int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(width), (int)PyInt_AsLong(height), size);
	Py_XDECREF(layer);
	Py_XDECREF(result);
	Py_XDECREF(leftPosVal);
	Py_XDECREF(topPosVal);
	Py_XDECREF(name);
	Py_XDECREF(width);
	Py_XDECREF(height);
	clearExceptions();
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
		Py_XDECREF(layer);
		clearExceptions();
		return NULL;
	}

	topPosVal = _getAbsoluteTop(layer);
	if(topPosVal == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(leftPosVal);
		clearExceptions();
		return NULL;
	}

	name = _getName(layer);
	if(name == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(leftPosVal);
		Py_XDECREF(topPosVal);
		clearExceptions();
		return NULL;
	}

	height = _getHeight(layer);
	if(height == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(leftPosVal);
		Py_XDECREF(topPosVal);
		Py_XDECREF(name);
		clearExceptions();
		return NULL;
	}

	width = _getWidth(layer);
	if(height == NULL)
	{
		Py_XDECREF(layer);
		Py_XDECREF(leftPosVal);
		Py_XDECREF(topPosVal);
		Py_XDECREF(name);
		Py_XDECREF(height);
		clearExceptions();
		return NULL;
	}

	char * iname = PyString_AsString(name);
	output = builder.buildInterfaceObject(iname,  (int)PyInt_AsLong(leftPosVal),(int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(width), (int)PyInt_AsLong(height), size);
	Py_XDECREF(layer);
	Py_XDECREF(leftPosVal);
	Py_XDECREF(topPosVal);
	Py_XDECREF(name);
	Py_XDECREF(height);
	clearExceptions();
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
		clearExceptions();
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
				Py_XDECREF(findChild);
				Py_XDECREF(layer);
				clearExceptions();
				return NULL;
			}
			


			PyObject * param = PyTuple_New(1);

			if(param == NULL)
			{
				log.elog("Failed to build PyTuple");
				Py_XDECREF(findChild);
				Py_XDECREF(layer);
				Py_XDECREF(args);
				clearExceptions();
				return NULL;
			}

			
			if(PyTuple_SetItem(param, 0, args) != 0)
			{
				log.elog("Failed to setitem in tuple");
				Py_XDECREF(findChild);
				Py_XDECREF(layer);
				Py_XDECREF(args);
				Py_XDECREF(param);
				clearExceptions();
				return NULL;
			}

			

			if(PyCallable_Check(findChild) == 0)
			{
				log.elog("findChild is not callable");
				Py_XDECREF(findChild);
				Py_XDECREF(layer);
				//Py_XDECREF(args);
				Py_XDECREF(param);
				clearExceptions();
				return NULL;
			}


			PyObject * soughtInterface = PyObject_CallObject(findChild, param );
			
			if(soughtInterface == NULL)
			{
				log.elog("Error calling FindChild(param)");
				log.elog(PyString_AsString(param));
				Py_XDECREF(findChild);
				Py_XDECREF(layer);
				//Py_XDECREF(args);
				Py_XDECREF(param);
				clearExceptions();
				return NULL;
			}

			leftPosVal = _getAbsoluteLeft(soughtInterface);

			if(leftPosVal == NULL)
			{

				log.elog("Failed to get leftPosVal");
				Py_XDECREF(findChild);
				Py_XDECREF(layer);
				//Py_XDECREF(args);
				Py_XDECREF(param);
				Py_XDECREF(soughtInterface);
				clearExceptions();
				return NULL;
			}
			

			topPosVal = _getAbsoluteTop(soughtInterface);
			if(topPosVal == NULL)
			{
				log.elog("Failed to get topPosVal");
				Py_XDECREF(findChild);
				Py_XDECREF(layer);
				//Py_XDECREF(args);
				Py_XDECREF(param);
				Py_XDECREF(soughtInterface);
				Py_XDECREF(leftPosVal);
				clearExceptions();
				return NULL;
			}
			
			width = _getWidth(soughtInterface);
			if(topPosVal == NULL)
			{
				log.elog("Failed to get width");
				Py_XDECREF(findChild);
				Py_XDECREF(layer);
				//Py_XDECREF(args);
				Py_XDECREF(param);
				Py_XDECREF(soughtInterface);
				Py_XDECREF(leftPosVal);
				Py_XDECREF(topPosVal);
				clearExceptions();
				return NULL;
			}			


			height = _getHeight(soughtInterface);
			if(topPosVal == NULL)
			{
				log.elog("Failed to get width");
				Py_XDECREF(findChild);
				Py_XDECREF(layer);
				//Py_XDECREF(args);
				Py_XDECREF(param);
				Py_XDECREF(soughtInterface);
				Py_XDECREF(leftPosVal);
				Py_XDECREF(topPosVal);
				Py_XDECREF(width);
				clearExceptions();
				return NULL;
			}

			
			log.elog("Found Child");
			output = builder.buildInterfaceObject(name,  (int)PyInt_AsLong(leftPosVal) ,(int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(width), (int)PyInt_AsLong(height), size);
			Py_XDECREF(findChild);
			Py_XDECREF(layer);
			//Py_XDECREF(args);
			Py_XDECREF(param);
			Py_XDECREF(soughtInterface);
			Py_XDECREF(leftPosVal);
			Py_XDECREF(topPosVal);
			Py_XDECREF(width);
			Py_XDECREF(height);
			clearExceptions();
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
		clearExceptions();
		return NULL;
	}

	PyObject * slotsContainer  = _findByNameLayer(shipui, "slotsContainer");
	if(slotsContainer == NULL)
	{
		log.elog("doesn't have a slots container");
		Py_XDECREF(shipui);
		clearExceptions();
		return NULL;
	}

	PyObject * mod = _findByNameLayer(slotsContainer, module);
	if(mod == NULL)
	{
		log.elog("Mod not visible");
		Py_XDECREF(shipui);
		Py_XDECREF(slotsContainer);
		clearExceptions();
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
		clearExceptions();
		return NULL;
	}

	width = _getWidth(mod);
	if(width == NULL)
	{
		log.elog("Coudn't get width");
		Py_XDECREF(height);
		clearExceptions();
		return NULL;
	}
	absoluteTop = _getAbsoluteTop(mod);
	if(absoluteTop == NULL)
	{
		log.elog("Couldn't get absoluteTop");
		Py_XDECREF(width);
		Py_XDECREF(height);
		clearExceptions();
		return NULL;
	}

	absoluteLeft = _getAbsoluteLeft(mod);
	if(absoluteLeft == NULL)
	{
		log.elog("Couldn't get absoluteLeft");
		Py_XDECREF(width);
		Py_XDECREF(height);
		Py_XDECREF(absoluteTop);
		clearExceptions();
		return NULL;		
	}

	char * output = builder.buildInterfaceObject(name, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	clearExceptions();
	return output;

}



char * Interfaces::GetStationAgentTab(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getLobbyTab("stationInformationTabAgents", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::_getLobbyTab(string name, int & size)
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("can't get main");
		clearExceptions();
		return NULL;
	}

	PyObject * tab = _findByNameLayer(main, name);
	if(tab == NULL)
	{
		log.elog("couldn't get tab");
		Py_XDECREF(main);
		clearExceptions();
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(tab, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("couldn't populate");
		Py_XDECREF(main);
		Py_XDECREF(tab);
		clearExceptions();
		return NULL;
	}

	char * output = builder.buildInterfaceObject("LobbyTab", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(main);
	Py_XDECREF(tab);
	Py_XDECREF(height);
	Py_XDECREF(width);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	clearExceptions();
	return output;
}


PyObject * Interfaces::_getStationLobbyBottom()
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		clearExceptions();
		return NULL;
	}

	PyObject * lobby = _findByNameLayer(main, "lobby");
	if(lobby == NULL)
	{
		log.elog("Couldn't get the lobby");
		Py_XDECREF(main);
		clearExceptions();
		return NULL;
	}

	PyObject * lmain = _findByNameLayer(lobby, "main");
	if(lmain == NULL)
	{
		log.elog("lmain error");
		Py_XDECREF(main);
		Py_XDECREF(lobby);
		clearExceptions();
		return NULL;
	}

	PyObject * bottomparent = _findByNameLayer(lmain, "bottomparent");
	if(bottomparent == NULL)
	{
		log.elog("Couldn't get the bottom");
		Py_XDECREF(main);
		Py_XDECREF(lobby);
		Py_XDECREF(lmain);
		clearExceptions();
		return NULL;
	}

	Py_XDECREF(main);
	Py_XDECREF(lobby);
	Py_XDECREF(lmain);
	clearExceptions();
	return bottomparent;
}


PyObject * Interfaces::_GetInflightCargoView()
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("main is null");
		clearExceptions();
		return NULL;
	}

	PyObject * children = _getAttribute(main, "children");
	if(children == NULL)
	{
		log.elog("children is null");
		Py_XDECREF(main);
		clearExceptions();
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
			Py_XDECREF(main);
			Py_XDECREF(children);
			clearExceptions();
			return NULL;
		}
		
		log.elog(PyEval_GetFuncName(pvalue));
		if(strcmp(PyEval_GetFuncName(pvalue), "InflightCargoView") == 0)
		{
			log.elog("Found cargoview");
			Py_XDECREF(main);
			Py_XDECREF(children);
			clearExceptions();
			return pvalue;
		}
		if(strcmp(PyEval_GetFuncName(pvalue), "DockedCargoView") == 0)
		{
			log.elog("Found docked cargoview");
			Py_XDECREF(main);
			Py_XDECREF(children);
			clearExceptions();
			return pvalue;
		}
		Py_XDECREF(pvalue);
	}

	Py_XDECREF(main);
	Py_XDECREF(children);
	clearExceptions();
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
			clearExceptions();
			return NULL;
		}
		
		log.elog(PyEval_GetFuncName(pvalue));
		if(strcmp(PyEval_GetFuncName(pvalue), name.c_str()) == 0)
		{
			log.elog("Found " + name);
			return pvalue;
		}
		Py_XDECREF(pvalue);
	}

	
	return NULL;
}


PyObject * Interfaces::_GetEntry(string entryname)
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("main is null");
		clearExceptions();
		return NULL;
	}

	PyObject * children = _getAttribute(main, "children");
	if(children == NULL)
	{
		log.elog("children is null");
		Py_XDECREF(main);
		clearExceptions();
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
			Py_XDECREF(main);
			Py_XDECREF(children);
			clearExceptions();
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
				Py_XDECREF(main);
				Py_XDECREF(children);
				Py_XDECREF(pvalue);
				clearExceptions();
				return NULL;
			}
		}
		Py_XDECREF(pvalue);
	}

	if(entry == NULL)
	{
		log.elog("doesn't have an entry " + entryname);
		Py_XDECREF(main);
		Py_XDECREF(children);
		clearExceptions();
		return NULL;
	}

	Py_XDECREF(main);
	Py_XDECREF(children);
	clearExceptions();
	return entry;

}

PyObject * Interfaces::_getNeocomButton(string buttonname)
{
	PyObject * layer = _getLayer("neocom");
	
	if(layer == NULL)
	{	
		clearExceptions();
		return NULL;
	}
	
	PyObject * neocom = _findByNameLayer(layer, "neocom");
	if(neocom == NULL)
	{
		log.elog("Couldn't get neocom layer");
		Py_XDECREF(layer);
		clearExceptions();
		return NULL;
	}

	PyObject * maincontainer = _findByNameLayer(neocom, "maincontainer");
	if(maincontainer == NULL)
	{
		log.elog("Couldn't get maincontainer");
		Py_XDECREF(layer);
		Py_XDECREF(neocom);
		clearExceptions();
		return NULL;
	}

	PyObject * button = _findByNameLayer(maincontainer, buttonname);
	if(button == NULL)
	{
		log.elog("couldn't get button");
		Py_XDECREF(layer);
		Py_XDECREF(neocom);
		Py_XDECREF(maincontainer);
		clearExceptions();
		return NULL;
	}
	return button;

}

char * Interfaces::GetShipArmor(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _GetShipUIGauge("armorGauge", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetOverviewDistanceHeader(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getOverViewHeaders("Distance", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::_getOverViewHeaders(string name, int & size)
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("Couldn't get main");
		clearExceptions();
		return NULL;
	}

	PyObject * overview = _findByNameLayer(main, "overview");
	if(overview == NULL)
	{
		log.elog("couldn't get overview");
		Py_XDECREF(main);
		clearExceptions();
		return NULL;
	}

	PyObject * header = _findByNameLayer(overview, name);
	if(header == NULL)
	{
		log.elog("couldn't get header");
		Py_XDECREF(main);
		Py_XDECREF(overview);
		clearExceptions();
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;

	bool ok = _populateAttributesDisplay(header, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("couldn't populate");
		Py_XDECREF(main);
		Py_XDECREF(overview);
		Py_XDECREF(header);
		clearExceptions();
		return NULL;
	}

	char * output = builder.buildInterfaceObject(name, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(main);
	Py_XDECREF(overview);
	Py_XDECREF(header);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	clearExceptions();
	return output;

}

char * Interfaces::GetShipShield(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _GetShipUIGauge("shieldGauge", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetShipCapacitor(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * shipui = _getLayer("shipui");
	if(shipui == NULL)
	{
		log.elog("couldn't get layer");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * powercore = _findByNameLayer(shipui, "powercore");
	if(powercore == NULL)
	{
		log.elog("Couldn't get the cap");
		Py_XDECREF(shipui);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * hint = _getAttribute(powercore, "hint");
	if(hint == NULL)
	{
		log.elog("Couldn't get the cap text");
		Py_XDECREF(shipui);
		Py_XDECREF(powercore);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildStringObject(PyString_AsString(hint), size);
	Py_XDECREF(shipui);
	Py_XDECREF(powercore);
	Py_XDECREF(hint);
	clearExceptions();
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
		clearExceptions();
		return NULL;
	}

	PyObject * underMain = _findByNameLayer(shipui, "underMain");
	if(underMain == NULL)
	{
		log.elog("Couldn't get underMain");
		Py_XDECREF(shipui);
		clearExceptions();
		return NULL;
	}

	PyObject * gauge = _findByNameLayer(underMain, name);
	if(gauge == NULL)
	{
		log.elog("Couldn't get structure");
		Py_XDECREF(shipui);
		Py_XDECREF(underMain);
		clearExceptions();
		return NULL;
	}
	
	PyObject * hint = _getAttribute(gauge, "hint");
	if(hint == NULL)
	{
		log.elog("Couldn't get hint");
		Py_XDECREF(shipui);
		Py_XDECREF(underMain);
		Py_XDECREF(gauge);
		clearExceptions();
		return NULL;
	}
	
	char * output = builder.buildStringObject(PyString_AsString(hint), size);
	Py_XDECREF(shipui);
	Py_XDECREF(underMain);
	Py_XDECREF(gauge);
	Py_XDECREF(hint);
	clearExceptions();
	return output;
}


char * Interfaces::GetShipSpeed(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * shipui = _getLayer("shipui");
	if(shipui == NULL)
	{
		log.elog("Couldn't get shipui");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * underMain = _findByNameLayer(shipui, "underMain");
	if(underMain == NULL)
	{
		log.elog("Couldn't get underMain");
		Py_XDECREF(shipui);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * label = _findByNameLayer(underMain, "EveLabelSmall");
	if(label == NULL)
	{
		log.elog("Couldn't get label");
		Py_XDECREF(shipui);
		Py_XDECREF(underMain);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * text = _getText(label);
	if(text == NULL)
	{
		log.elog("Couldn't get text");
		Py_XDECREF(shipui);
		Py_XDECREF(underMain);
		Py_XDECREF(label);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildStringObject(PyString_AsString(text), size);
	Py_XDECREF(shipui);
	Py_XDECREF(underMain);
	Py_XDECREF(label);
	Py_XDECREF(text);
	clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, *height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributesDisplay(cargoWindow, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate");
		Py_XDECREF(cargoWindow);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("shipHangar", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(cargoWindow);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(absoluteTop);
	clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * stationHangar = _findByNameLayer(main, "hangarFloor");
	if(stationHangar == NULL)
	{
		log.elog("stationHangar is null");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, *height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;

	bool ok = _populateAttributesDisplay(stationHangar, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		Py_XDECREF(main);
		Py_XDECREF(stationHangar);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("stationHangar", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(main);
	Py_XDECREF(stationHangar);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(absoluteTop);
	clearExceptions();
	PyGILState_Release(gstate);
	return output;
		
}


bool Interfaces::_populateAttributesDisplay(PyObject * item, PyObject ** width, PyObject ** height, PyObject ** absoluteTop, PyObject ** absoluteLeft)
{
			*width = _getAttribute(item, "displayWidth");
			if(*width == NULL)
			{
				log.elog("Couldn't get width");
				clearExceptions();
				return false;
			}
			
			*height = _getAttribute(item, "displayHeight");
			if(*height == NULL)
			{
				log.elog("Couldn't get height");
				Py_XDECREF(width);
				clearExceptions();
				return false;
			}
			
			*absoluteLeft = _getAbsoluteLeft(item);
			if(*absoluteLeft == NULL)
			{
				log.elog("Couldn't get absoluteLeft");
				Py_XDECREF(height);
				Py_XDECREF(width);
				clearExceptions();
				return false;
			}

			*absoluteTop = _getAbsoluteTop(item);
			if(*absoluteTop == NULL)
			{
				log.elog("Couldn't get absoluteTop");
				Py_XDECREF(height);
				Py_XDECREF(width);
				Py_XDECREF(absoluteLeft);
				clearExceptions();
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
				clearExceptions();
				return false;
			}
			
			*height = _getHeight(item);
			if(*height == NULL)
			{
				log.elog("Couldn't get height");
				Py_XDECREF(width);
				clearExceptions();
				return false;
			}
			
			*absoluteLeft = _getAbsoluteLeft(item);
			if(*absoluteLeft == NULL)
			{
				log.elog("Couldn't get absoluteLeft");
				Py_XDECREF(height);
				Py_XDECREF(width);
				clearExceptions();
				return false;
			}

			*absoluteTop = _getAbsoluteTop(item);
			if(*absoluteTop == NULL)
			{
				log.elog("Couldn't get absoluteTop");
				Py_XDECREF(height);
				Py_XDECREF(width);
				Py_XDECREF(absoluteLeft);
				clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	bool ok = _populateAttributesDisplay(buttonIcon, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't get attributes");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildInterfaceObject("UndockButton", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(buttonIcon);
	clearExceptions();
	
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	bool ok = _populateAttributesDisplay(buttonIcon, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't get attributes");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildInterfaceObject("ItemsButton", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(buttonIcon);
	clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;;
	}

	PyObject * label = _findByNameLayer(cargoView, "capacityText");
	if(label == NULL)
	{
		log.elog("Couldn't get capacityLabel");
		Py_XDECREF(cargoView);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;;
	}

	PyObject * ptext = _getText(label);
	if(ptext == NULL)
	{
		log.elog("Couldn't pull the text off the label");
		Py_XDECREF(cargoView);
		Py_XDECREF(label);
		clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject *children = _getAttribute(main, "children");
	if(children == NULL)
	{
		log.elog("Has no children");
		Py_XDECREF(main);
		clearExceptions();
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
			Py_XDECREF(main);
			Py_XDECREF(children);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		bool ok = _populateAttributes(pvalue, &width, &height, &absoluteTop, &absoluteLeft);
		if(!ok)
		{	
			log.elog("Couldn't populate attributes");
			Py_XDECREF(main);
			Py_XDECREF(children);
			Py_XDECREF(pvalue);
			clearExceptions();
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
		Py_XDECREF(pvalue);
	}

	char * output = builder.buildOverViewObject(labels, size);
	Py_XDECREF(main);
	Py_XDECREF(children);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * menuview = _findByNameLayer(menu, "menuview");
	if(menuview == NULL)
	{
		log.elog("Couldn't get menuview");
		Py_XDECREF(menu);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * entries = _findByNameLayer(menuview, "_entries");
	if(entries == NULL)
	{
		log.elog("Couldn't get entries");
		Py_XDECREF(menu);
		Py_XDECREF(menuview);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject *entries_children = _getAttribute(entries, "children");
	if(entries_children == NULL)
	{
		log.elog("Has no children");
		Py_XDECREF(menu);
		Py_XDECREF(menuview);
		Py_XDECREF(entries);
		clearExceptions();
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
			Py_XDECREF(menu);
			Py_XDECREF(menuview);
			Py_XDECREF(entries);
			Py_XDECREF(entries_children);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}
		
		fname = PyEval_GetFuncName(pvalue);
		if(fname == NULL)
		{
			log.elog("Couldn't get function name");
			Py_XDECREF(menu);
			Py_XDECREF(menuview);
			Py_XDECREF(entries);
			Py_XDECREF(entries_children);
			Py_XDECREF(pvalue);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		if(strcmp(fname, "MenuEntryView") == 0)
		{
			//crucible changes
			PyObject * text_child = _findByNameLayer(pvalue, "EveLabelSmall");
			if(text_child == NULL)
			{
				log.elog("Couldn't get text child");
				Py_XDECREF(menu);
				Py_XDECREF(menuview);
				Py_XDECREF(entries);
				Py_XDECREF(entries_children);
				Py_XDECREF(pvalue);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}
			PyObject * text = _getAttribute(text_child, "text");
			if(text == NULL)
			{
				log.elog("couldn't get text");
				Py_XDECREF(menu);
				Py_XDECREF(menuview);
				Py_XDECREF(entries);
				Py_XDECREF(entries_children);
				Py_XDECREF(pvalue);
				Py_XDECREF(text_child);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}
			
			char * ctext = PyString_AsString(text);
			if(ctext == NULL)
			{
				log.elog("Couldn't convert into c string");
				Py_XDECREF(menu);
				Py_XDECREF(menuview);
				Py_XDECREF(entries);
				Py_XDECREF(entries_children);
				Py_XDECREF(pvalue);
				Py_XDECREF(text_child);
				Py_XDECREF(text);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}

			bool ok = _populateAttributes(text_child, &width, &height, &absoluteTop, &absoluteLeft);
			if(!ok)
			{
				log.elog("Couldn't populate attributes");
				Py_XDECREF(menu);
				Py_XDECREF(menuview);
				Py_XDECREF(entries);
				Py_XDECREF(entries_children);
				Py_XDECREF(pvalue);
				Py_XDECREF(text_child);
				Py_XDECREF(text);
				clearExceptions();
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
			Py_XDECREF(text);
			Py_XDECREF(text_child);
		}

		Py_XDECREF(pvalue);
	}

	Py_XDECREF(menu);
	Py_XDECREF(menuview);
	Py_XDECREF(entries);
	PyGILState_Release(gstate);
	char * output = builder.buildOverViewObject(labels, size);
	for(list<ObjectBuilder::overViewEntry *>::iterator it = labels.begin(); it != labels.end(); it++)
	{
		delete (*it);
	}	
	return output;
}

char * Interfaces::GetNeoComItems(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getNeoComItem("items", size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::_getNeoComItem(string name, int & size)
{
	PyObject * neocom = _getLayer("neocom");
	if(neocom == NULL)
	{
		log.elog("Couldn't get neocom");
		clearExceptions();
		return NULL;
	}

	PyObject * neocomitem = _findByNameLayer(neocom, name);
	if(neocomitem == NULL)
	{
		log.elog("couldn't find neocom item");
		Py_XDECREF(neocom);
		clearExceptions();
		return NULL;
	}

	PyObject * text = _findByNameLayer(neocomitem, "EveLabelSmall");
	if(text == NULL)
	{
		log.elog("Couldn't get text");
		Py_XDECREF(neocom);
		Py_XDECREF(neocomitem);
		clearExceptions();
		return NULL;
	}

	PyObject * width = NULL, *height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(text, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("error populating");
		Py_XDECREF(neocom);
		Py_XDECREF(neocomitem);
		Py_XDECREF(text);
		clearExceptions();
		return NULL;
	}

	char * output = builder.buildInterfaceObject(name, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(neocom);
	Py_XDECREF(neocomitem);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(text);
	clearExceptions();
	return output;
}

PyObject * Interfaces::_getAddressBookWindow()
{
	PyObject * main = _getLayer("main");
	if(main == NULL)
	{
		log.elog("couldn't get main");
		clearExceptions();
		return NULL;
	}
	
	PyObject * addressbook = _findByNameLayer(main, "addressbook");
	if(addressbook == NULL)
	{
		log.elog("Couldn't get addressbook");
		Py_XDECREF(main);
		clearExceptions();
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
		clearExceptions();
		return NULL;
	}

	PyObject * button = _findByNameLayer(addressbookWindow, name);
	if(button == NULL)
	{
		log.elog("Couldn't get button");
		Py_XDECREF(addressbookWindow);
		clearExceptions();
		return NULL;
	}
	
	PyObject * width = NULL, * height = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(button, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate");
		Py_XDECREF(addressbookWindow);
		Py_XDECREF(button);
		clearExceptions();
		return NULL;
	}

	char * output = builder.buildInterfaceObject(name, PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(addressbookWindow);
	Py_XDECREF(button);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(absoluteTop);
	return output;

}

char * Interfaces::GetAddressBookWindow(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * window = _getAddressBookWindow();
	if(window == NULL)
	{
		log.elog("Couldn't get the window");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * height = NULL, * width = NULL, *absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(window, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Trouble populating");
		Py_XDECREF(window);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("Addressbook", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(window);
	Py_XDECREF(height);
	Py_XDECREF(width);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetAddressBookBMButton(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = _getPeopleAndPlacesButton("Add Location_Btn", size);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * tabparent = _findByNameLayer(addressbookWindow, "tabparent");
	if(tabparent == NULL)
	{
		log.elog("Couldn't get the tabs");
		Py_XDECREF(addressbookWindow);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * children = _getAttribute(tabparent, "children");

	if(children == NULL)
	{
		log.elog("Couldn't get children");
		Py_XDECREF(addressbookWindow);
		Py_XDECREF(children);
		clearExceptions();
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
			Py_XDECREF(addressbookWindow);
			Py_XDECREF(children);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		log.elog(PyEval_GetFuncName(pvalue));
		if(strcmp(PyEval_GetFuncName(pvalue), "Tab") == 0)
		{
			PyObject * tabLabel = _findByNameLayer(pvalue, "tabLabel");
			if(tabLabel == NULL)
			{
				log.elog("couldn't get label");
				Py_XDECREF(addressbookWindow);
				Py_XDECREF(children);
				Py_XDECREF(pvalue);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}

			PyObject * ptext = _getAttribute(tabLabel, "text");
			if(ptext == NULL)
			{
				log.elog("Couldn't get text");
				Py_XDECREF(addressbookWindow);
				Py_XDECREF(children);
				Py_XDECREF(pvalue);
				Py_XDECREF(tabLabel);
				clearExceptions();
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
					Py_XDECREF(addressbookWindow);
					Py_XDECREF(children);
					Py_XDECREF(pvalue);
					Py_XDECREF(tabLabel);
					Py_XDECREF(ptext);
					clearExceptions();
					PyGILState_Release(gstate);
					return NULL;
				}

				char * output = builder.buildInterfaceObject(PyString_AsString(ptext), PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
				Py_XDECREF(addressbookWindow);
				Py_XDECREF(children);
				Py_XDECREF(pvalue);
				Py_XDECREF(tabLabel);
				Py_XDECREF(ptext);
				clearExceptions();
				PyGILState_Release(gstate);
				return output;

			}
			Py_XDECREF(tabLabel);
			Py_XDECREF(ptext);

		}

		Py_XDECREF(pvalue);

	}

	Py_XDECREF(addressbookWindow);
	Py_XDECREF(children);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * hangarView = _findByNameLayer(main, "hangarFloor");
	if(hangarView == NULL)
	{
		log.elog("Couldn't get hangar");
		Py_XDECREF(main);
		clearExceptions();
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
		clearExceptions();
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
		clearExceptions();
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
			Py_XDECREF(entry_children);
			clearExceptions();
			return;
		}

		sr = _getAttribute(pvalue, "sr");
		if(sr == NULL)
		{
			log.elog("sr null");
			Py_XDECREF(pvalue);
			Py_XDECREF(entry_children);
			clearExceptions();
			return;
		}

		node = _getAttribute(sr, "node");
		if(node == NULL)
		{
			log.elog("node is null");
			Py_XDECREF(pvalue);
			Py_XDECREF(sr);
			Py_XDECREF(entry_children);
			clearExceptions();
			return;
		}

		if(PyObject_Not(node))
		{
			Py_XDECREF(sr);
			Py_XDECREF(node);
			Py_XDECREF(pvalue);
			clearExceptions();
			continue;
		}
			

		name = _getAttribute(node, "name");
		if(name == NULL)
		{
			log.elog("couldn't get the name");
			Py_XDECREF(pvalue);
			Py_XDECREF(sr);
			Py_XDECREF(node);
			Py_XDECREF(entry_children);
			clearExceptions();
			return;
		}
		//Got name, get absoluteTop, absoluteLeft, width, height
		absoluteTop = _getAttribute(pvalue, "absoluteTop");
		if(absoluteTop == NULL)
		{
			log.elog("Couldn't get absoluteTop");
			Py_XDECREF(pvalue);
			Py_XDECREF(sr);
			Py_XDECREF(node);
			Py_XDECREF(name);
			Py_XDECREF(entry_children);
			clearExceptions();
			return;
		}

		absoluteLeft = _getAttribute(pvalue, "absoluteLeft");
		if(absoluteLeft == NULL)
		{
			log.elog("Couldnt' get absoluteleft");
			Py_XDECREF(pvalue);
			Py_XDECREF(sr);
			Py_XDECREF(node);
			Py_XDECREF(name);
			Py_XDECREF(absoluteTop);
			Py_XDECREF(entry_children);
			clearExceptions();
			return;
		}

		width = _getAttribute(pvalue, "width");
		if(width == NULL)
		{
			log.elog("Couldn't get width");
			Py_XDECREF(name);
			Py_XDECREF(pvalue);
			Py_XDECREF(sr);
			Py_XDECREF(node);
			Py_XDECREF(absoluteTop);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(entry_children);
			clearExceptions();
			return;
		}

		height = _getAttribute(pvalue, "height");
		if(height == NULL)
		{
			log.elog("Couldn't get height");
			Py_XDECREF(name);
			Py_XDECREF(pvalue);
			Py_XDECREF(sr);
			Py_XDECREF(node);
			Py_XDECREF(absoluteTop);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(width);
			Py_XDECREF(entry_children);
			clearExceptions();
			return;
		}

		sort_qty = _getAttribute(node, "sort_Quantity");
		if(sort_qty == NULL)
		{
			log.elog("Couldn't get sort_qty");
			Py_XDECREF(name);
			Py_XDECREF(pvalue);
			Py_XDECREF(sr);
			Py_XDECREF(node);
			Py_XDECREF(absoluteTop);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(width);
			Py_XDECREF(entry_children);
			Py_XDECREF(height);
			clearExceptions();
			return;			
		}

		volume = _getAttribute(node, "volume");
		if(volume == NULL)
		{
			log.elog("Couldn't get volume");
			Py_XDECREF(name);
			Py_XDECREF(pvalue);
			Py_XDECREF(sr);
			Py_XDECREF(node);
			Py_XDECREF(absoluteTop);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(width);
			Py_XDECREF(entry_children);
			Py_XDECREF(height);
			Py_XDECREF(sort_qty);
			clearExceptions();
			return;			
		}
		
		meta = _getAttribute(node, "metaLevel");
		if(meta == NULL)
		{
			log.elog("Couldn't get meta");
			Py_XDECREF(name);
			Py_XDECREF(pvalue);
			Py_XDECREF(sr);
			Py_XDECREF(node);
			Py_XDECREF(absoluteTop);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(width);
			Py_XDECREF(entry_children);
			Py_XDECREF(height);
			Py_XDECREF(sort_qty);
			Py_XDECREF(volume);
			clearExceptions();
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
		Py_XDECREF(pvalue);
		Py_XDECREF(height);
		Py_XDECREF(name);
		Py_XDECREF(sr);
		Py_XDECREF(node);
		Py_XDECREF(absoluteLeft);
		Py_XDECREF(absoluteTop);
		Py_XDECREF(width);
		Py_XDECREF(sort_qty);
		Py_XDECREF(volume);

	}///end of for

	Py_XDECREF(entry_children);
		
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * hangar = _findByNameLayer(layer, "hangarFloor");
	if(hangar == NULL)
	{
		log.elog("couldn't get hangar");
		Py_XDECREF(layer);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(hangar, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("couldn't populate");
		Py_XDECREF(layer);
		Py_XDECREF(hangar);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("hangar", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(layer);
	Py_XDECREF(hangar);
	Py_XDECREF(width);
	Py_XDECREF(height);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	PyGILState_Release(gstate);
	return output;

}

char * Interfaces::GetHighSlotModuleInfo(int number, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	stringstream os;
	os << "inFlightHighSlot";
	os << number;
	output = _getModuleInfo(os.str(), size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetMedSlotModuleInfo(int number, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	stringstream os;
	os << "inMediumHighSlot";
	os << number;
	output = _getModuleInfo(os.str(), size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetLowSlotModuleInfo(int number, int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	char * output = NULL;
	stringstream os;
	os << "inFlightLowSlot";
	os << number;
	output = _getModuleInfo(os.str(), size);
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
		clearExceptions();
		return NULL;
	}

	PyObject * sr = _getAttribute(module, "sr");
	if(sr == NULL)
	{
		log.elog("Couldn't get sr");
		Py_XDECREF(module);
		clearExceptions();
		return NULL;
	}
	PyObject * srmodule = _getAttribute(sr, "module");
	if(srmodule == NULL)
	{
		log.elog("Couldn't find srmodule");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		clearExceptions();
		return NULL;
	}

	PyObject * sragain = _getAttribute(srmodule, "sr");
	if(sragain == NULL)
	{
		log.elog("sragain is null");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		clearExceptions();
		return NULL;
	}

	PyObject * moduleInfo = _getAttribute(sragain, "moduleInfo");
	if(moduleInfo == NULL)
	{
		log.elog("Can't get the moduleInfo");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		clearExceptions();
		return NULL;
	}

	PyObject * attribute = _getAttribute(moduleInfo, attr);
	if(attribute == NULL)
	{
		log.elog("Couldn't get maxRange");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		Py_XDECREF(moduleInfo);
		clearExceptions();
		return NULL;
	}

	if(PyObject_Not(attribute))
	{
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		Py_XDECREF(moduleInfo);
		Py_XDECREF(attribute);
		clearExceptions();
		return NULL;
	}

	
	PyFloatObject * value = (PyFloatObject*)PyFloat_FromDouble(PyFloat_AsDouble(attribute));
	if(value == NULL)
	{
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		Py_XDECREF(moduleInfo);
		Py_XDECREF(attribute);
		clearExceptions();
		return NULL;
	}
	char buf[200];
	PyFloat_AsString(buf, value);

	char * output = builder.buildStringObject(buf, size);
	Py_XDECREF(module);
	Py_XDECREF(sr);
	Py_XDECREF(srmodule);
	Py_XDECREF(sragain);
	Py_XDECREF(moduleInfo);
	Py_XDECREF(attribute);
	Py_XDECREF(value);
	clearExceptions();
	return output;
	
}

//This allocates mem, up to the caller to cleanup
char * Interfaces::_getModuleAttributeCA(string name, string attr, int & size)
{
	PyObject * module = _findModule(name);
	if(module == NULL)
	{
		log.elog("Couldn't find the module");
		clearExceptions();
		return NULL;
	}

	PyObject * sr = _getAttribute(module, "sr");
	if(sr == NULL)
	{
		log.elog("Couldn't get sr");
		Py_XDECREF(module);
		clearExceptions();
		return NULL;
	}
	PyObject * srmodule = _getAttribute(sr, "module");
	if(srmodule == NULL)
	{
		log.elog("Couldn't find srmodule");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		clearExceptions();
		return NULL;
	}

	PyObject * sragain = _getAttribute(srmodule, "sr");
	if(sragain == NULL)
	{
		log.elog("sragain is null");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		clearExceptions();
		return NULL;
	}

	PyObject * moduleInfo = _getAttribute(sragain, "moduleInfo");
	if(moduleInfo == NULL)
	{
		log.elog("Can't get the moduleInfo");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		clearExceptions();
		return NULL;
	}

	PyObject * attribute = _getAttribute(moduleInfo, attr);
	if(attribute == NULL)
	{
		log.elog("Couldn't get maxRange");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		Py_XDECREF(moduleInfo);
		clearExceptions();
		return NULL;
	}

	if(PyObject_Not(attribute))
	{
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		Py_XDECREF(moduleInfo);
		Py_XDECREF(attribute);
		clearExceptions();
		return NULL;
	}

	PyFloatObject * value = (PyFloatObject*)PyFloat_FromDouble(PyFloat_AsDouble(attribute));
	if(value == NULL)
	{
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		Py_XDECREF(moduleInfo);
		Py_XDECREF(attribute);
		clearExceptions();
		return NULL;
	}
	char * buf = NULL;
	buf = new char[200];
	PyFloat_AsString(buf, value);

	Py_XDECREF(module);
	Py_XDECREF(sr);
	Py_XDECREF(srmodule);
	Py_XDECREF(sragain);
	Py_XDECREF(moduleInfo);
	Py_XDECREF(attribute);
	Py_XDECREF(value);
	clearExceptions();
	return buf;
	
}

char * Interfaces::_getModuleAttributes(string name, int & size)
{
	list<string*> attributes = list<string*>();
	stringstream os;
	
	char * radius = NULL, *rof = NULL, *tracking = NULL, *damageMult = NULL, *falloff = NULL, *maxRange = NULL, *optsigradius = NULL;

	radius = _getModuleAttributeCA(name, "radius", size);
	rof = _getModuleAttributeCA(name, "speed", size);
	tracking = _getModuleAttributeCA(name, "trackingSpeed", size);
	damageMult = _getModuleAttributeCA(name, "damageMultiplier", size);
	falloff = _getModuleAttributeCA(name, "falloff", size);
	maxRange = _getModuleAttributeCA(name, "maxRange", size);
	optsigradius = _getModuleAttributeCA(name, "optimalSigRadius", size);
	
	if(radius != NULL)
	{
		attributes.push_back(new string(radius));
		clearExceptions();
		delete radius;
	}
	if(rof != NULL)
	{
		attributes.push_back(new string(rof));
		clearExceptions();
		delete rof;
	}
	if(tracking != NULL)
	{
		attributes.push_back(new string(tracking));
		clearExceptions();
		delete tracking;
	}
	if(damageMult != NULL)
	{
		attributes.push_back(new string(damageMult));
		clearExceptions();
		delete damageMult;
	}
	if(falloff != NULL)
	{
		attributes.push_back(new string(falloff));
		clearExceptions();
		delete falloff;
	}
	if(maxRange != NULL)
	{
		attributes.push_back(new string(maxRange));
		clearExceptions();
		delete maxRange;
	}
	if(optsigradius != NULL)
	{
		attributes.push_back(new string(optsigradius));
		clearExceptions();
		delete optsigradius;
	}

	char * output = builder.buildStringListObject(attributes, size);

	for(list<string *>::iterator it = attributes.begin(); it != attributes.end(); it++)
	{
		delete (*it);		
	}

	return output;
}

char * Interfaces::GetHighSlotAttributes(int number, int & size)
{
	char * output = NULL;
	PyGILState_STATE gstate = PyGILState_Ensure();
	stringstream os;
	os << "inFlightHighSlot";
	os << number;
	output = _getModuleAttributes(os.str(), size);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::_getModuleInfo(string name, int & size)
{
	PyObject * module = _findModule(name);
	bool isActive = false;
	if(module == NULL)
	{
		log.elog("Couldn't find the module");
		clearExceptions();
		return NULL;
	}

	PyObject * sr = _getAttribute(module, "sr");
	if(sr == NULL)
	{
		log.elog("Couldn't get sr");
		Py_XDECREF(module);
		clearExceptions();
		return NULL;
	}
	PyObject * srmodule = _getAttribute(sr, "module");
	if(srmodule == NULL)
	{
		log.elog("Couldn't find srmodule");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		clearExceptions();
		return NULL;
	}

	PyObject * sragain = _getAttribute(srmodule, "sr");
	if(sragain == NULL)
	{
		log.elog("sragain is null");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		clearExceptions();
		return NULL;
	}

	PyObject * hint = _getAttribute(sragain, "hint");
	if(hint == NULL)
	{
		log.elog("Can't get the status");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		clearExceptions();
		return NULL;
	}

	if(PyObject_IsTrue(hint) == 0)
	{
		log.elog("no data");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		Py_XDECREF(hint);
		clearExceptions();
		return NULL;
	}

	log.elog("Outputing modinfo");
	char * output = builder.buildStringObject(PyString_AsString(hint), size);
	Py_XDECREF(module);
	Py_XDECREF(sr);
	Py_XDECREF(srmodule);
	Py_XDECREF(sragain);
	Py_XDECREF(hint);
	clearExceptions();
	return output;
}


char * Interfaces::_isModuleActive(string name, int & size)
{
	PyObject * module = _findModule(name);
	bool isActive = false;
	if(module == NULL)
	{
		log.elog("Couldn't find the module");
		clearExceptions();
		return NULL;
	}

	PyObject * sr = _getAttribute(module, "sr");
	if(sr == NULL)
	{
		log.elog("Couldn't get sr");
		Py_XDECREF(module);
		clearExceptions();
		return NULL;
	}
	PyObject * srmodule = _getAttribute(sr, "module");
	if(srmodule == NULL)
	{
		log.elog("Couldn't find srmodule");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		clearExceptions();
		return NULL;
	}

	PyObject * sragain = _getAttribute(srmodule, "sr");
	if(sragain == NULL)
	{
		log.elog("sragain is null");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		clearExceptions();
		return NULL;
	}

	PyObject * glow = _getAttribute(sragain, "glow");
	if(glow == NULL)
	{
		log.elog("Can't get the glow");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		clearExceptions();
		return NULL;
	}

	PyObject * state = _getAttribute(glow, "state");
	if(state == NULL)
	{
		log.elog("Couldn't get state");
		Py_XDECREF(module);
		Py_XDECREF(sr);
		Py_XDECREF(srmodule);
		Py_XDECREF(sragain);
		Py_XDECREF(glow);
		clearExceptions();
		return NULL;
	}

	int istate = PyInt_AsLong(state);
	if(istate == 1)
	{
		isActive = true;
	}

	log.elog("Outputing isActive");
	char * output = builder.buildBooleanObject(isActive, size);
	Py_XDECREF(module);
	Py_XDECREF(sr);
	Py_XDECREF(srmodule);
	Py_XDECREF(sragain);
	Py_XDECREF(glow);
	Py_XDECREF(state);
	return output;


}

char * Interfaces::_GetSlot(string name, string  outputname, int & size)
{
	PyObject * mod = _findModule(name);
	if(mod == NULL)
	{
		log.elog("No module found");
		clearExceptions();
		return NULL;
	}
	char * output = _buildModule(mod, outputname, size);
	Py_XDECREF(mod);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * selectedItemView = _findByNameLayer(main, "selecteditemview");
	if(selectedItemView == NULL)
	{
		log.elog("selectedItemView is null");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * maincontainer = _findByNameLayer(selectedItemView, "__maincontainer");
	if(maincontainer == NULL)
	{
		log.elog("__maincontainer is null");
		Py_XDECREF(main);
		Py_XDECREF(selectedItemView);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * mainitem = _findByNameLayer(maincontainer, "main");
	if(mainitem == NULL)
	{
		log.elog("mainitem is null");
		Py_XDECREF(main);
		Py_XDECREF(selectedItemView);
		Py_XDECREF(maincontainer);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * toparea = _findByNameLayer(mainitem, "toparea");
	if(toparea == NULL)
	{
		log.elog("toparea is null");
		Py_XDECREF(main);
		Py_XDECREF(selectedItemView);
		Py_XDECREF(maincontainer);
		Py_XDECREF(mainitem);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * text = _findByNameLayer(toparea, "EveLabelSmall");
	if(text == NULL)
	{
		log.elog("evelabel is null");
		Py_XDECREF(main);
		Py_XDECREF(selectedItemView);
		Py_XDECREF(maincontainer);
		Py_XDECREF(mainitem);
		Py_XDECREF(toparea);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * result = _getAttribute(text, "text");
	if(result == NULL)
	{
		log.elog("Couldn't get the text attribute");
		Py_XDECREF(main);
		Py_XDECREF(selectedItemView);
		Py_XDECREF(maincontainer);
		Py_XDECREF(mainitem);
		Py_XDECREF(toparea);
		Py_XDECREF(text);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * label = PyString_AsString(result);
	
	if(label == NULL)
	{
		log.elog("Couldn't turn label into string");
		Py_XDECREF(main);
		Py_XDECREF(selectedItemView);
		Py_XDECREF(maincontainer);
		Py_XDECREF(mainitem);
		Py_XDECREF(toparea);
		Py_XDECREF(text);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	char * output = builder.buildStringObject(label, size);

	Py_XDECREF(main);
	Py_XDECREF(selectedItemView);
	Py_XDECREF(maincontainer);
	Py_XDECREF(mainitem);
	Py_XDECREF(toparea);
	Py_XDECREF(text);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * children = _getAttribute(target, "children");
	if(children == NULL)
	{
		log.elog("Target has no children");
		Py_XDECREF(target);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	int len = PyObject_Size(children);

	if(len < 1)
	{
		log.elog("Target has no children");
		Py_XDECREF(children);
		Py_XDECREF(target);
		clearExceptions();
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
			Py_XDECREF(children);
			Py_XDECREF(target);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}
		
		fname = PyEval_GetFuncName(pvalue);
		if(fname == NULL)
		{
			log.elog("Couldn't get type name");
			Py_XDECREF(children);
			Py_XDECREF(target);
			Py_XDECREF(pvalue);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}
		
		if(strcmp(fname, "Target") == 0)
		{
			log.elog("Found target object");
			PyObject * label = NULL, * text = NULL, * width =NULL, * height=NULL, *absoluteLeft=NULL, *absoluteTop=NULL;
			char * ctext;
			label = _findByNameLayer(pvalue, "EveLabelSmall");
			if(label == NULL)
			{
				log.elog("Couldn't pull the text child off the target");
				Py_XDECREF(children);
				Py_XDECREF(target);
				Py_XDECREF(pvalue);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}
			
			text= _getAttribute(label, "text");
			if(text == NULL)
			{
				log.elog("Couldn't pull the text attribute off the label");
				Py_XDECREF(children);
				Py_XDECREF(target);
				Py_XDECREF(pvalue);
				Py_XDECREF(label);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}

			ctext = PyString_AsString(text);
			if(text == NULL)
			{
				log.elog("Couldn't pull the text off the label");
				Py_XDECREF(children);
				Py_XDECREF(target);
				Py_XDECREF(pvalue);
				Py_XDECREF(label);
				Py_XDECREF(text);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}
			
			width = _getWidth(label);
			if(width == NULL)
			{
				log.elog("Couldn't get width");
				Py_XDECREF(children);
				Py_XDECREF(target);
				Py_XDECREF(pvalue);
				Py_XDECREF(label);
				Py_XDECREF(text);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}
			
			height = _getHeight(label);
			if(height == NULL)
			{
				log.elog("Couldn't get height");
				Py_XDECREF(children);
				Py_XDECREF(target);
				Py_XDECREF(pvalue);
				Py_XDECREF(label);
				Py_XDECREF(text);
				Py_XDECREF(width);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}
			
			absoluteLeft = _getAbsoluteLeft(label);
			if(absoluteLeft == NULL)
			{
				log.elog("Couldn't get absoluteLeft");
				Py_XDECREF(children);
				Py_XDECREF(target);
				Py_XDECREF(pvalue);
				Py_XDECREF(text);
				Py_XDECREF(label);
				Py_XDECREF(height);
				Py_XDECREF(width);
				clearExceptions();
				PyGILState_Release(gstate);
				return NULL;
			}

			absoluteTop = _getAbsoluteTop(label);
			if(absoluteTop == NULL)
			{
				log.elog("Couldn't get absoluteTop");
				Py_XDECREF(children);
				Py_XDECREF(target);
				Py_XDECREF(pvalue);
				Py_XDECREF(label);
				Py_XDECREF(height);
				Py_XDECREF(width);
				Py_XDECREF(text);
				Py_XDECREF(absoluteLeft);
				clearExceptions();
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
			Py_XDECREF(pvalue);
			Py_XDECREF(label);
			Py_XDECREF(height);
			Py_XDECREF(width);
			Py_XDECREF(text);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(absoluteTop);
			
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

	Py_XDECREF(target);
	Py_XDECREF(children);
	PyGILState_Release(gstate);
	return output;

}

int Interfaces::_getSize(PyObject * layer)
{
	PyObject * top = _getAttribute(layer, "absoluteTop");
	if(top == NULL)
	{
		log.elog("couldn't get top");
		clearExceptions();
		return -1;
	}

	PyObject * bottom = _getAttribute(layer, "absoluteBottom");
	if(bottom == NULL)
	{
		log.elog("couldn't get bottom");
		Py_XDECREF(top);
		clearExceptions();
		return -1;
	}

	int size = PyInt_AsLong(bottom) - PyInt_AsLong(top);
	Py_XDECREF(bottom);
	Py_XDECREF(top);
	return size;
}


char * Interfaces::_getLocalChatScrollAttribute(string attr, int & size)
{
	PyObject * chatScroll = _getLocalChatScroll();
	if(chatScroll == NULL)
	{
		log.elog("couldn't get chatscroll");
		clearExceptions();
		return NULL;
	}

	PyObject * parent = _getAttribute(chatScroll, "parent");
	if(parent == NULL)
	{
		log.elog("Couldn't get parent");
		Py_XDECREF(chatScroll);
		clearExceptions();
		return NULL;
	}

	PyObject * bottom = _getAttribute(parent, attr);
	if(bottom == NULL)
	{
		log.elog("couldn't get attribute");
		Py_XDECREF(chatScroll);
		Py_XDECREF(parent);
		clearExceptions();
		return NULL;
	}

	stringstream os;
	os << PyInt_AsLong(bottom);

	char * output = builder.buildStringObject(os.str(), size);
	Py_XDECREF(chatScroll);
	Py_XDECREF(parent);
	Py_XDECREF(bottom);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * bottom = _getAttribute(overviewScroll, "absoluteBottom");
	if(bottom == NULL)
	{
		log.elog("couldn't get bottom");
		Py_XDECREF(overviewScroll);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	stringstream os;
	os << PyInt_AsLong(bottom);

	char * output = builder.buildStringObject(os.str(), size);
	Py_XDECREF(overviewScroll);
	Py_XDECREF(bottom);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * isopen = _getAttribute(login, "isopen");
	
	if(isopen == NULL)
	{
		log.elog("Couldn't check open");
		Py_XDECREF(login);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	bool open = PyObject_IsTrue(isopen);
	Py_XDECREF(login);
	Py_XDECREF(isopen);
	PyGILState_Release(gstate);
	return open;
}

string Interfaces::Internal_getVersion()
{
	bool open = isLoginOpen();
	if(!open)
	{
		log.elog("login is not open");
		clearExceptions();
		return "";
	}

	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * login = _getLayer("login");
	if(login == NULL)
	{
		log.elog("Couldn't get login");
		clearExceptions();
		PyGILState_Release(gstate);
		return "";
	}

	PyObject * version = _findByNameLayer(login, "text");
	if(version == NULL)
	{
		log.elog("Couldn't get version");
		Py_XDECREF(login);
		clearExceptions();
		PyGILState_Release(gstate);
		return "";
	}

	PyObject *version_text = _getAttribute(version, "text");
	if(version_text == NULL)
	{
		log.elog("Couldn't get version text");
		Py_XDECREF(login);
		Py_XDECREF(version);
		clearExceptions();
		PyGILState_Release(gstate);
		return "";
	}

	string output(PyString_AsString(version_text));
	Py_XDECREF(login);
	Py_XDECREF(version);
	Py_XDECREF(version_text);
	clearExceptions();
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * version = _findByNameLayer(login, "text");
	if(version == NULL)
	{
		log.elog("Couldn't get version");
		Py_XDECREF(login);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject *version_text = _getAttribute(version, "text");
	if(version_text == NULL)
	{
		log.elog("Couldn't get version text");
		Py_XDECREF(login);
		Py_XDECREF(version);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildStringObject(PyString_AsString(version_text), size);
	Py_XDECREF(login);
	Py_XDECREF(version);
	Py_XDECREF(version_text);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	PyObject * bottom = _getAttribute(overviewScroll, "absoluteTop");
	if(bottom == NULL)
	{
		log.elog("couldn't get bottom");
		Py_XDECREF(overviewScroll);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	stringstream os;
	os << PyInt_AsLong(bottom);

	char * output = builder.buildStringObject(os.str(), size);
	Py_XDECREF(overviewScroll);
	Py_XDECREF(bottom);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * width = NULL, * height = NULL, * absoluteTop = NULL, *absoluteLeft = NULL;
	bool ok = _populateAttributes(scroll, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("couldn't populate attributes");
		Py_XDECREF(scroll);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("localScroll", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(scroll);
	Py_XDECREF(height);
	Py_XDECREF(width);
	Py_XDECREF(absoluteTop);
	Py_XDECREF(absoluteLeft);
	PyGILState_Release(gstate);
	return output;
}

char * Interfaces::GetLocalCount(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * main = _getLayer("main");

	if(main == NULL)
	{
		log.elog("main is null");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * local = _findByNameLayer(main, "chatchannel_solarsystemid2");
	if(local == NULL)
	{
		log.elog("Couldn't get local");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * captionParent = _findByNameLayer(local, "captionParent");
	if(captionParent == NULL)
	{
		log.elog("Couldn't get parent");
		Py_XDECREF(main);
		Py_XDECREF(local);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * label = _findByNameLayer(captionParent, "EveLabelSmall");
	if(label == NULL)
	{
		log.elog("Couldn't get text");
		Py_XDECREF(main);
		Py_XDECREF(local);
		Py_XDECREF(captionParent);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * text = _getAttribute(label, "text");
	if(text == NULL)
	{
		log.elog("Couldn't get parent");
		Py_XDECREF(main);
		Py_XDECREF(local);
		Py_XDECREF(captionParent);
		Py_XDECREF(label);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildStringObject(PyString_AsString(text), size);
	Py_XDECREF(main);
	Py_XDECREF(local);
	Py_XDECREF(captionParent);
	Py_XDECREF(label);
	Py_XDECREF(text);
	PyGILState_Release(gstate);
	return output;


}

PyObject * Interfaces::_getLocalChatScroll()
{
	PyObject * main = _getLayer("main");

	if(main == NULL)
	{
		log.elog("main is null");
		clearExceptions();
		return NULL;
	}

	PyObject * local = _findByNameLayer(main, "chatchannel_solarsystemid2");
	if(local == NULL)
	{
		log.elog("Couldn't get local");
		Py_XDECREF(main);
		clearExceptions();
		return NULL;
	}
	
	PyObject * userlist = _findByNameLayer(local, "userlist");
	if(userlist == NULL)
	{
		log.elog("Couldn't get userlist");
		Py_XDECREF(main);
		Py_XDECREF(local);
		clearExceptions();
		return NULL;
	}


	PyObject * scroll = _findByNameLayer(userlist, "__scrollhandle");
	if(scroll == NULL)
	{
		log.elog("Couldn't get the scroll");
		Py_XDECREF(main);
		Py_XDECREF(local);
		Py_XDECREF(userlist);
		clearExceptions();
		return NULL;
	}

	Py_XDECREF(main);
	Py_XDECREF(local);
	Py_XDECREF(userlist);

	return scroll;

}

PyObject * Interfaces::_getOverviewScroll()
{
	PyObject * main = _getLayer("main");

	if(main == NULL)
	{
		log.elog("main is null");
		clearExceptions();
		return NULL;
	}

	PyObject * overview = _findByNameLayer(main, "overview");
	if(overview == NULL)
	{
		log.elog("Couldn't get overview child");
		Py_XDECREF(main);
		clearExceptions();
		return NULL;
	}
	
	PyObject * overviewScroll = _findByNameLayer(overview, "overviewscroll2");
	if(overviewScroll == NULL)
	{
		log.elog("Couldn't get the scrollarea");
		Py_XDECREF(main);
		Py_XDECREF(overview);
		clearExceptions();
		return NULL;
	}

	Py_XDECREF(main);
	Py_XDECREF(overview);

	return overviewScroll;

}

char * Interfaces::GetOverviewHeight(int & size)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	PyObject * overviewScroll = _getOverviewScroll();
	
	if(overviewScroll == NULL)
	{
		log.elog("couldn't get the overview scroll");
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	int ssize = _getSize(overviewScroll);
	stringstream os;
	os << ssize;
	char * output = builder.buildStringObject(os.str(), size);
	Py_XDECREF(overviewScroll);
	PyGILState_Release(gstate);
	return output;

}

PyObject * Interfaces::_getScrollHandle(PyObject * layer)
{
	PyObject * scrollControls = _findByNameLayer(layer, "__scrollcontrols");
	if(scrollControls == NULL)
	{
		log.elog("scrollControls is null");
		clearExceptions();
		return NULL;
	}

	PyObject * sr = _getAttribute(scrollControls, "sr");
	if(sr == NULL)
	{
		log.elog("Couldn't get sr");
		Py_XDECREF(scrollControls);
		clearExceptions();
		return NULL;
	}

	PyObject * scrollHandle = _getAttribute(sr, "scrollhandle");
	if(scrollHandle == NULL)
	{
		log.elog("Couldn't get scrollHandle");
		Py_XDECREF(scrollControls);
		Py_XDECREF(sr);
		clearExceptions();
		return NULL;
	}
	
	Py_XDECREF(scrollControls);
	Py_XDECREF(sr);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * overview = _findByNameLayer(main, "overview");
	if(overview == NULL)
	{
		log.elog("Couldn't get overview child");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * scrollHandle = _getScrollHandle(overview);
	if(scrollHandle == NULL)
	{
		log.elog("Couldn't get the scrollHandle");
		Py_XDECREF(main);
		Py_XDECREF(overview);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * height = NULL, * width = NULL, * absoluteTop = NULL, * absoluteLeft = NULL;
	bool ok = _populateAttributes(scrollHandle, &width, &height, &absoluteTop, &absoluteLeft);
	if(!ok)
	{
		log.elog("Couldn't populate");
		Py_XDECREF(main);
		Py_XDECREF(overview);
		Py_XDECREF(scrollHandle);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	char * output = builder.buildInterfaceObject("overviewScrollBar", PyInt_AsLong(absoluteLeft), PyInt_AsLong(absoluteTop), PyInt_AsLong(width), PyInt_AsLong(height), size);
	Py_XDECREF(main);
	Py_XDECREF(overview);
	Py_XDECREF(scrollHandle);
	Py_XDECREF(height);
	Py_XDECREF(width);
	Py_XDECREF(absoluteLeft);
	Py_XDECREF(absoluteTop);
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
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * overview = _findByNameLayer(main, "overview");
	if(overview == NULL)
	{
		log.elog("Couldn't get overview child");
		Py_XDECREF(main);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
		

	PyObject * maincontainer = _findByNameLayer(overview, "maincontainer");
	if(maincontainer == NULL)
	{
		log.elog("maincontainer is null");
		Py_XDECREF(main);
		Py_XDECREF(overview);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	PyObject * content = _findByNameLayer(maincontainer, "__content");	
	if(content == NULL)
	{
		log.elog("content is null");
		Py_XDECREF(main);
		Py_XDECREF(maincontainer);
		Py_XDECREF(overview);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}
	
	PyObject * children = PyObject_GetAttrString(content, "children");
	
	if(children == NULL)
	{
		log.elog("Couldn't get CHildren");
		Py_XDECREF(main);
		Py_XDECREF(maincontainer);
		Py_XDECREF(content);
		Py_XDECREF(overview);
		clearExceptions();
		PyGILState_Release(gstate);
		return NULL;
	}

	int len = PyObject_Size(children);
	if(len < 1)
	{
		log.elog("Overview is Closed");
		Py_XDECREF(main);
		Py_XDECREF(maincontainer);
		Py_XDECREF(content);
		Py_XDECREF(children);
		Py_XDECREF(overview);
		clearExceptions();
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
			Py_XDECREF(main);
			Py_XDECREF(maincontainer);
			Py_XDECREF(content);
			Py_XDECREF(children);
			Py_XDECREF(overview);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}
	
		//crucible change text->EveLabelMedium
		PyObject * label = _findByNameLayer(pvalue, "EveLabelMedium");
		if(label == NULL)
		{
			log.elog("No label");
			Py_XDECREF(main);
			Py_XDECREF(maincontainer);
			Py_XDECREF(content);
			Py_XDECREF(children);
			Py_XDECREF(overview);
			Py_XDECREF(pvalue);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * text = _getText(label);
	
		if(text == NULL)
		{
			log.elog("No text in the label");
			Py_XDECREF(main);
			Py_XDECREF(maincontainer);
			Py_XDECREF(content);
			Py_XDECREF(children);
			Py_XDECREF(overview);
			Py_XDECREF(pvalue);
			Py_XDECREF(label);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * absoluteTop = NULL, * absoluteLeft = NULL, * width = NULL, * height = NULL;

		bool ok = _populateAttributes(pvalue, &width, &height, &absoluteTop, &absoluteLeft);
		if(!ok)
		{
			log.elog("couldn't populate");
			Py_XDECREF(main);
			Py_XDECREF(maincontainer);
			Py_XDECREF(content);
			Py_XDECREF(children);
			Py_XDECREF(overview);
			Py_XDECREF(pvalue);
			Py_XDECREF(label);
			Py_XDECREF(text);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}


		PyObject * icon = _findByNameLayer(pvalue, "typeicon");
		if(icon == NULL)
		{
			log.elog("Icon was null");
			Py_XDECREF(main);
			Py_XDECREF(maincontainer);
			Py_XDECREF(content);
			Py_XDECREF(children);
			Py_XDECREF(overview);
			Py_XDECREF(pvalue);
			Py_XDECREF(label);
			Py_XDECREF(text);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(absoluteTop);
			Py_XDECREF(width);
			Py_XDECREF(height);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * color = _getAttribute(icon, "color");
		if(color == NULL)
		{
			log.elog("Couldn't get color");
			Py_XDECREF(main);
			Py_XDECREF(maincontainer);
			Py_XDECREF(content);
			Py_XDECREF(children);
			Py_XDECREF(overview);
			Py_XDECREF(pvalue);
			Py_XDECREF(label);
			Py_XDECREF(text);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(absoluteTop);
			Py_XDECREF(width);
			Py_XDECREF(height);
			Py_XDECREF(icon);
			clearExceptions();
			PyGILState_Release(gstate);
			return NULL;
		}

		PyObject * g = _getAttribute(color, "g");
		if(g == NULL)
		{
			log.elog("Couldn't get color");
			Py_XDECREF(main);
			Py_XDECREF(maincontainer);
			Py_XDECREF(content);
			Py_XDECREF(children);
			Py_XDECREF(overview);
			Py_XDECREF(pvalue);
			Py_XDECREF(label);
			Py_XDECREF(text);
			Py_XDECREF(absoluteLeft);
			Py_XDECREF(absoluteTop);
			Py_XDECREF(width);
			Py_XDECREF(height);
			Py_XDECREF(icon);
			Py_XDECREF(color);
			clearExceptions();
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
		Py_XDECREF(pvalue);
		Py_XDECREF(label);
		Py_XDECREF(text);
		Py_XDECREF(absoluteLeft);
		Py_XDECREF(absoluteTop);
		Py_XDECREF(width);
		Py_XDECREF(icon);
		Py_XDECREF(color);
		Py_XDECREF(g);

	}
	log.elog("Building overview object");
	char * output = builder.buildOverViewObject(labels, size);

	for(list<ObjectBuilder::overViewEntry *>::iterator it = labels.begin(); it != labels.end(); it++)
	{
		delete (*it);
	}

	Py_XDECREF(main);
	Py_XDECREF(maincontainer);
	Py_XDECREF(content);
	Py_XDECREF(children);
	Py_XDECREF(overview);
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
		clearExceptions();
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
				Py_XDECREF(findChild);
				clearExceptions();
				return NULL;
			}
			
			PyObject * param = PyTuple_New(1);

			if(param == NULL)
			{
				log.elog("Failed to build PyTuple");
				Py_XDECREF(findChild);
				Py_XDECREF(args);
				clearExceptions();
				return NULL;
			}

			
			if(PyTuple_SetItem(param, 0, args) != 0)
			{
				log.elog("Failed to setitem in tuple");
				Py_XDECREF(findChild);
				Py_XDECREF(args);
				Py_XDECREF(param);
				clearExceptions();
				return NULL;
			}

			if(PyCallable_Check(findChild) == 0)
			{
				log.elog("findChild is not callable");
				Py_XDECREF(findChild);
				//Py_XDECREF(args);
				Py_XDECREF(param);
				clearExceptions();
				return NULL;
			}

			soughtInterface = PyObject_CallObject(findChild, param );
			
			if(soughtInterface == NULL)
			{
				log.elog("Error calling FindChild(param)");
				log.elog(PyString_AsString(param));
				Py_XDECREF(findChild);
				//Py_XDECREF(args);
				Py_XDECREF(param);
				clearExceptions();
				return NULL;
			}
			if(PyObject_Not(soughtInterface))
			{
				log.elog("FindChild returned blank");
				Py_XDECREF(findChild);
				//Py_XDECREF(args);
				Py_XDECREF(param);
				clearExceptions();
				return NULL;
			}

			log.elog("Found Child");
			//output = builder.buildInterfaceObject(name,  (int)PyInt_AsLong(leftPosVal) ,(int)PyInt_AsLong(topPosVal), (int)PyInt_AsLong(width), (int)PyInt_AsLong(height), size);
			Py_XDECREF(findChild);
			//Py_XDECREF(args);
			Py_XDECREF(param);
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
			clearExceptions();
			return NULL;
		}

		PyObject * maindic = PyModule_GetDict(main);
		
		if(maindic == NULL)
		{
			log.elog("Couldn't load main dictionary");
			clearExceptions();
			return NULL;
		}

		PyObject * uicore = PyDict_GetItemString(maindic, "uicore");

		if(uicore == NULL)
		{
			log.elog("uicore is null");
			clearExceptions();
			return NULL;
		}
		PyObject * layer = PyObject_GetAttrString(uicore, "layer");
		if(layer == NULL)
		{
			log.elog("layer is null");
			clearExceptions();
			return NULL;
		}

		PyObject * layeritem = PyObject_GetAttrString(layer, name.c_str());
		if(layeritem == NULL)
		{
			log.elog("layeritem is null");
			Py_XDECREF(layer);
			clearExceptions();
			return NULL;
		}

		Py_XDECREF(layer);

		return layeritem;
}

void Interfaces::_findByText(PyObject * parentInt, string text, PyObject ** result)
{

	if(parentInt == NULL)
	{
		log.elog("Interface is NULL");
		clearExceptions();
		return;
	}

	if(!PyObject_HasAttrString(parentInt, "children"))
	{
		log.elog("Interface has no children");
		clearExceptions();
		return;
	}
	
///Changed for debugging


	PyObject * children = PyObject_GetAttrString(parentInt, "children");
	if(children == NULL)
	{
		log.elog("Couldn't get the children attribute");
		clearExceptions();
		return;
	}

	
	int len = PyObject_Size(children);

	//log.elog(len + "");

	if(len < 1)
	{
		log.elog("End of branch");
		Py_XDECREF(children);
		clearExceptions();
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
			Py_XDECREF(children);
			clearExceptions();
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
					Py_XDECREF(children);
					Py_XDECREF(ptext);
					clearExceptions();
					return;
				}
				string check(ctext);
				if(check.find(text) != check.npos)
				{
					//log.elog("Found text");
					*result = pvalue;
					//delete buf;
					//delete ibuf;
					Py_XDECREF(children);
					Py_XDECREF(ptext);
					clearExceptions();
					return;
				}
			}

		}
		_findByText(pvalue, text, result); 
		//log.elog("Function ran");
		Py_XDECREF(pvalue);
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
			clearExceptions();
			return NULL;
		}
	}
	else
	{
		log.elog("Doesn't have attribute " + attr);
		clearExceptions();
		return NULL;
	}
	return attribute;
}
