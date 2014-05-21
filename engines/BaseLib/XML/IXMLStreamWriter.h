#pragma once

#include <string>
#include <map>
#include <list>
#include <vector>

#include "IXMLSerializable.h"

#pragma warning( push )
#pragma warning( disable:4512 ) 


class IXMLStreamWriter
{
public:
	virtual ~IXMLStreamWriter() {}

	virtual void NewNode(const wchar_t* name)=0;
	virtual void Begin(const wchar_t* type)=0;
	virtual void End()=0; 
	virtual void Write(const wchar_t* name, signed short value)=0;
	virtual void Write(const wchar_t* name, unsigned short value)=0;
	virtual void Write(const wchar_t* name, int value)=0;
	virtual void Write(const wchar_t* name, unsigned int value)=0;
	virtual void Write(const wchar_t* name, long value)=0;
	virtual void Write(const wchar_t* name, unsigned long value)=0;
	virtual void Write(const wchar_t* name, double value)=0;
	virtual void Write(const wchar_t* name, float value)=0;
	virtual void Write(const wchar_t* name, const wchar_t* buffer)=0;
	virtual void Write(const wchar_t* name, const char* buffer)=0;
	virtual void Write(const wchar_t* name, const ISerializable& value)=0;
	virtual void Write(const wchar_t* name, const std::string& value)
	{ 
		Write(name,value.c_str());
	}

	virtual void Write(const wchar_t* name, const std::wstring& value) 
	{ 
		Write(name,value.c_str());
	}

	template <typename T> void Write(const wchar_t* name,const T& value) 
	{
		WriterWrapper<const T> helper(value);
		Write(name,static_cast<const ISerializable&>(helper));
	}

	template <typename T> void WriteArray(const wchar_t* name, const T* array, unsigned int size) 
	{
		ArraySerialize<T> helper(array,size);
		Write(name,helper);
	}

	template <typename T> void Write(const wchar_t* name, const std::vector<T>& value)
	{ 
		writeContainer(name,value);	
	}

	template <typename T> void Write(const wchar_t* name, const std::list<T>& value) 
	{ 
		writeContainer(name,value);	
	}

	template <typename T1, typename T2> void Write(const wchar_t* name, const std::map<T1,T2>& value) 
	{ 
		writeKeyedContainer(name,value);	
	}

private:
	template <typename T> void writeContainer(const wchar_t* name, const T& container) 
	{
		ContainerSerialize<T> helper(container);
		Write(name,helper);
	}

	template <typename T> void writeKeyedContainer(const wchar_t* name, const T& container) 
	{
		KeyedContainerSerialize<T> helper(container);
		Write(name,helper);
	}
};

#pragma warning( pop )