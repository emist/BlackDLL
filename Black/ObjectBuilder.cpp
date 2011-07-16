#include "stdafx.h"
#include "ObjectBuilder.h"


char * ObjectBuilder::buildBooleanObject( bool value, int & size  )
{
	eveobjects::BooleanObject eveobject;
	eveobject.set_istrue(value);
	char * output = putToByteArray(eveobject, size);
	return output;
}

char * ObjectBuilder::buildInterfaceObject(string name, int posX, int posY, int width, int height, int & size)
{
	eveobjects::Interface interfaceObject;
	interfaceObject.set_name(name);
	interfaceObject.set_topleftx(posX);
	interfaceObject.set_toplefty(posY);
	interfaceObject.set_width(width);
	interfaceObject.set_height(height);
	char * output = putToByteArray(interfaceObject, size);
	return output;
}
