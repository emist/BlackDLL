#include "stdafx.h"
#include "ObjectBuilder.h"


char * ObjectBuilder::buildBooleanObject( bool value, int & size  )
{
	eveobjects::BooleanObject eveobject;
	eveobject.set_istrue(value);
	char * output = putToByteArray(eveobject, size);
	return output;
}
