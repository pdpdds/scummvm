#pragma once

#include <string>
#include <map>
#include <list>
#include <vector>

#include "IXMLSerializable.h"

#pragma warning( push )
#pragma warning( disable:4512 ) 

class IXMLStreamReader
{
public:
	virtual ~IXMLStreamReader() {}

	virtual bool Exists(const wchar_t* name) const=0;
	virtual size_t GetCount(const wchar_t* name) const=0;
	virtual size_t GetCount() const=0;
	virtual bool FindNode(const wchar_t* name)=0;
	virtual bool Begin(const wchar_t* type)=0;
	virtual void End()=0;
	virtual void Read(const wchar_t* name, signed short& value)=0;
	virtual void Read(const wchar_t* name, unsigned short& value)=0;
	virtual void Read(const wchar_t* name, int& value)=0;
	virtual void Read(const wchar_t* name, unsigned int& value)=0;
	virtual void Read(const wchar_t* name, long& value)=0;
	virtual void Read(const wchar_t* name, unsigned long& value)=0;
	virtual void Read(const wchar_t* name, double& value)=0;
	virtual void Read(const wchar_t* name, float& value)=0;
	virtual unsigned int Read(const wchar_t* name, wchar_t* buffer, unsigned int size)=0;
	virtual unsigned int Read(const wchar_t* name, char* buffer, unsigned int size)=0;
	virtual void Read(const wchar_t* name, ISerializable& value)=0;
	virtual void Read(const wchar_t* name, std::string& value)
	{
		char tmp[MAXSTRING];
		int readed=Read(name,tmp,MAXSTRING);
		//assert(readed < MAXSTRING);
		value=tmp;
	}

	virtual void Read(const wchar_t* name, std::wstring& value)
	{
		wchar_t tmp[MAXSTRING];
		int readed=Read(name,tmp,MAXSTRING);
		//assert(readed < MAXSTRING);
		value=tmp;
	}

	template <typename T> void Read(const wchar_t* name,T& value) 
	{
		ReaderWrapper<T> helper(value);

		Read(name,static_cast<ISerializable&>(helper));
	}

	template <typename T> void ReadArray(const wchar_t* name,T* array, unsigned int size) 
	{
		if ( GetType() == Type::Flat) 
		{
			PrivateFlat::ArrayDeserialize<T> helper(array,size);
			Read(name,helper);
		} 
		else 
		{
			ArrayDeserialize<T> helper(array,size);
			Read(name,helper);
		}
	}

	template <typename T> void Read(const wchar_t* name, std::vector<T>& value) 
	{ 
		readContainer(name,value);	
	}

	template <typename T> void Read(const wchar_t* name, std::list<T>& value) 
	{ 
		readContainer(name,value);	
	}

	template <typename T1, typename T2> void Read(const wchar_t* name, std::map<T1,T2>& value) 
	{ 
		readKeyedContainer(name,value); 
	}

	template <typename T> void Read(const wchar_t* name, T& value, const T& defaultValue) 
	{
		if (Exists(name)) 
		{
			Read(name,value);
		} 
		else 
		{
			value=defaultValue;
		}
	}

private:
	enum 
	{ 
		MAXSTRING = 1024 /// 최대 String 길이
	};

	template <typename T> void readContainer(const wchar_t* name, T& container) 
	{
		ContainerDeserialize<T> helper(container);
		Read(name,helper);
	}

	template <typename T> void readKeyedContainer(const wchar_t* name, T& container) 
	{
		KeyedContainerDeserialize<T> helper(container);
		Read(name,helper);
	}
};

#pragma warning( pop )
