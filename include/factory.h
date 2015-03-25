#ifndef _FACTORY_H_
#define _FACTORY_H_	

#include <stdafx.h>


namespace ssuge
{
    /// A templatized, pure-virtual class, which returns instances of that type
    template <class T>
    class Factory
    {
    public:

        /// Static Template for an allocation of a component
        static T * allocate()
        {
	        T * tmp = new T();
	        msInstances.push_back(tmp);
	        return tmp;
        }

		/// Constructor for components
		static T * allocate(GameObject * owner)
		{
			T * tmp = new T(owner);
			msInstances.push_back(tmp);
			return tmp;
		}

		/// Static Template for a de-allocation of a component
		static T * deallocate(void * val)
		{
			T * rv = NULL;
			for(int x = 0; x < (int)msInstances.size(); x++)
			{
				if(msInstances[x] == (T*)val)
				{
					rv = msInstances[x];
					msInstances.erase(msInstances.begin() + x);
					return rv;
				}
			}
			return rv;
		}

    private:
	    /// msInstances is a vector containing all object components
	    static std::vector<T*> msInstances;

    };
}

#endif