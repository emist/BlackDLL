#ifndef OBJECTBUILDER
#define OBJECTBUILDER

#include <string>

using namespace std;

class ObjectBuilder{
		template<typename T>
		char * putToByteArray(T & eveobject, int & size)
		{
			char * output = new char[eveobject.ByteSize()];
			size = eveobject.ByteSize();
			eveobject.SerializeToArray(output, size);
			return output;
		}

public:
		char * buildBooleanObject( bool value, int & size );
		char * buildInterfaceObject( string name, int posX, int posY, int width, int height, int & size );
};
#endif