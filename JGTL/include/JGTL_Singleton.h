// Singleton.h

#ifndef __JGTL_SINGLETON_H__
#define __JGTL_SINGLETON_H__

#include "JGTL_LocatedException.h"

namespace JGTL
{
	/**
	* @class Singleton
	* @brief This class handles Singletons (Global Single-Instance Classes)
	* @author Jason Gauci
	* @Date 2008
	*/

	template <class Type>
	class Singleton
	{
	protected:
		// The instance pointer
		static Type * instance;

		// Constructor
		Singleton () {
			instance = (Type*)this; //For DLL support.
		}

		//Destructor
		virtual ~Singleton() { }

	public:
		// Returns the instance pointer
		static inline Type* getInstance (void)
		{
			return instance;
		}

		//NOTE: It's the derived class' responsibility to make a createInstance() method.
		//  This is because an empty constructor might not exist!

		static inline void destroyInstance()
		{
			if (!instance)
				throw CREATE_LOCATEDEXCEPTION_INFO("Tried to delete a singleton before it was created!");

			delete instance;
            instance = NULL;
		}

	};

	template <class Type>
	Type * Singleton <Type>::instance = NULL;

}

#endif //__JGTL_SINGLETON_H__
