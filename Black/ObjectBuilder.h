
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
};