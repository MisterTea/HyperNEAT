#ifndef __JGTL_SERIALIZATION_H__
#define __JGTL_SERIALIZATION_H__

#include "JGTL_LocatedException.h"

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

namespace JGTL
{
	typedef unsigned char uchar;

	/*
	* This function packs data into a buffer using the assignment operator
	* NOTE: The pointer to the buffer and the bufferSize are modified to 
	* represent the data being consumed by the function
	* @param buffer The buffer
	* @param bufferSize The unused size of the buffer
	* @param data The data to pack
	*/
	template<class Data>
	inline void packBuffer(uchar* &buffer,int &bufferSize,const Data &data)
	{
		if(bufferSize<sizeof(Data))
		{
			throw CREATE_LOCATEDEXCEPTION_INFO("Buffer Overflow!");
		}

		(*((Data*)buffer)) = data;
		buffer += sizeof(Data);
		bufferSize -= sizeof(Data);
	}

	/*
	* This function unpacks data from a buffer using the assignment operator
	* NOTE: The pointer to the buffer and the bufferSize are modified to 
	* represent the data being consumed by the function
	* @param buffer The buffer
	* @param bufferSize The used size of the buffer
	* @param data The data to unpack
	*/
	template<class Data>
	inline void unpackBuffer(uchar* &buffer,int &bufferSize,Data &data)
	{
		if(bufferSize<sizeof(Data))
		{
			throw CREATE_LOCATEDEXCEPTION_INFO("Buffer Overflow!");
		}

		data = (*((Data*)buffer));
		buffer += sizeof(Data);
		bufferSize -= sizeof(Data);
	}

	/*
	* This function packs data into a buffer using a shallow memory copy
	* NOTE: The pointer to the buffer and the bufferSize are modified to 
	* represent the data being consumed by the function
	* @param buffer The buffer
	* @param bufferSize The unused size of the buffer
	* @param data The data to pack
	*/
	template<class Data>
	inline void packBufferShallow(uchar* &buffer,int &bufferSize,const Data &data)
	{
		if(bufferSize<int(sizeof(Data)))
		{
			throw CREATE_LOCATEDEXCEPTION_INFO("Buffer Overflow!");
		}

		memcpy(buffer,&data,sizeof(Data));
		buffer += sizeof(Data);
		bufferSize -= sizeof(Data);
	}

	/*
	* This function unpacks data into a buffer using a shallow memory copy
	* NOTE: The pointer to the buffer and the bufferSize are modified to 
	* represent the data being consumed by the function
	* @param buffer The buffer
	* @param bufferSize The used size of the buffer
	* @param data The data to pack
	*/
	template<class Data>
	inline void unpackBufferShallow(uchar* &buffer,int &bufferSize,Data &data)
	{
		if(bufferSize<int(sizeof(Data)))
		{
			throw CREATE_LOCATEDEXCEPTION_INFO("Buffer Overflow!");
		}

		memcpy(&data,buffer,sizeof(Data));
		buffer += sizeof(Data);
		bufferSize -= sizeof(Data);
	}

	template<class Data>
	inline void packOrUnpackBufferShallow(uchar* &buffer,int &bufferSize,Data &data,bool pack)
	{
		if(pack)
		{
			packBufferShallow(buffer,bufferSize,data);
		}
		else
		{
			unpackBufferShallow(buffer,bufferSize,data);
		}
	}

	/*
	* This function packs strings so that the actual string data is copied
	* NOTE: The pointer to the buffer and the bufferSize are modified to 
	* represent the data being consumed by the function
	* @param buffer The buffer
	* @param bufferSize The unused size of the buffer
	* @param s the string to pack
	*/
	inline void packBufferString(uchar* &buffer,int &bufferSize,const char *s)
	{
		int sizeToCopy = int(sizeof(int)+strlen(s));
		if(bufferSize<sizeToCopy)
		{
			throw CREATE_LOCATEDEXCEPTION_INFO("Buffer Overflow!");
		}

		int strLength = (int)strlen(s);
		packBufferShallow(buffer,bufferSize,strLength);
		memcpy(buffer,s,strLength);
		buffer += strLength;
		bufferSize -= strLength;
	}

	/*
	* This function packs strings so that the actual string data is copied
	* NOTE: The pointer to the buffer and the bufferSize are modified to 
	* represent the data being consumed by the function
	* @param buffer The buffer
	* @param bufferSize The unused size of the buffer
	* @param s the string to pack
	*/
	inline void packBufferString(uchar* &buffer,int &bufferSize,const std::string &s)
	{
		packBufferString(buffer,bufferSize,s.c_str());
	}

	/*
	* This function unpacks strings so that the actual string data is copied
	* NOTE: The pointer to the buffer and the bufferSize are modified to 
	* represent the data being consumed by the function
	* @param buffer The buffer
	* @param bufferSize The used size of the buffer
	* @param s the string to pack
	*/
	inline void unpackBufferString(uchar* &buffer,int &bufferSize,string &s)
	{
		if(bufferSize<int(sizeof(int)))
		{
			throw CREATE_LOCATEDEXCEPTION_INFO("Buffer Overflow!");
		}

		int stringLength;
		unpackBufferShallow(buffer,bufferSize,stringLength);

		if(bufferSize<stringLength)
		{
			throw CREATE_LOCATEDEXCEPTION_INFO("Buffer Overflow!");
		}

		s = std::string((const char*)buffer,(std::string::size_type)stringLength);
		buffer += stringLength;
		bufferSize -= stringLength;
	}
}

#endif
