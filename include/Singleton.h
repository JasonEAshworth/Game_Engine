#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <stdafx.h>
#include <Exception.h>

namespace ssuge
{
	/// A General-purpose singleton class.  To use this class, inherit
	/// from it and define the msSingleton attribute (in a .cpp file)
	// [TO-DO]: Make this templatized, most likely (I suspect we'll have
	// problems when we make more than one class derived from Singleton.
	template <class T>
	class Singleton
	{
	public:
		/// The constructor.  Checks for an existing singleton instance and
		/// raises an exception if there is one.  Note: the msSingleton
		/// must be initialized in a .cpp file.
		Singleton()
		{
			if (msSingleton)
				throw Exception(__FILE__, __LINE__, "Singleton is already initialized!");
			msSingleton = static_cast<T*>(this);
		}

		/// The destructor.  Makes sure the destructor has only been called once.
		~Singleton()
		{
			if (!msSingleton)
				throw Exception(__FILE__, __LINE__, "Singleton has already been destroyed!");
			msSingleton = NULL;
		}

		/// Returns a pointer to the singleton
		static T * getSingletonPtr() 
		{ 
			return msSingleton; 
		}

	protected:
		/// A pointer to the one-and-only instance of the singleton.
		static T * msSingleton;
	};
}

#endif