#ifndef OBJECTBUILDER
#define OBJECTBUILDER

#include <string>
#include <List>
#include "Logger.h"

using namespace std;


class ObjectBuilder{
		Logger log;
		template<typename T>
		char * putToByteArray(T & eveobject, int & size)
		{
			char * output = new char[eveobject.ByteSize()];
			size = eveobject.ByteSize();
			eveobject.SerializeToArray(output, size);
			return output;
		}

public:
	
		struct overViewEntry
		{
			string text;
			double color;
			int topLeftX;
			int topLeftY;
			int width;
			int height;
		};

		struct targetEntry
		{
			string name;
			int topLeftX;
			int topLeftY;
			int width;
			int height;
			list<string> jammers;
		};

		struct itemEntry
		{
			string name;
			int quantity;
			string volume;
			string meta;
			int topLeftX;
			int topLeftY;
			int width;
			int height;
		};
			

		char * ObjectBuilder::buildSolarSystemObject(string name, string other, int & size);
		char * buildBooleanObject( bool value, int & size );
		char * buildInterfaceObject( string name, int posX, int posY, int width, int height, int & size );
		char * buildOverViewObject(list<overViewEntry *> & labels, int & size);
		char * buildItemObject(list<itemEntry *> & labels, int & size);
		char * ObjectBuilder::buildTargetObject(list<targetEntry *> & labels, int & size);
		char * ObjectBuilder::buildStringObject( string value, int & size  );
};

#endif