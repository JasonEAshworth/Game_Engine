#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <stdafx.h>

namespace ssuge
{
	/// An general-pupose exception for any ssuge-related exceptions
	class Exception
	{
	public:
		/// The constructor.
		Exception(std::string f, unsigned int l, std::string d) : mDescription(d), mLine(l), mFile(f) {}

		/// A description of the error
		std::string mDescription;

		/// The name of the file in which the error occurred (use __FILE__ most of the time)
		std::string mFile;

		/// The line on which the error occurred (use __LINE__ most of the time)
		unsigned int mLine;

		/// Returns a formatted string with all the above info
		std::string getDescription() 
		{ 
			char temps[1024];
			sprintf_s(temps, 1024, "Engine Error [%s@%d]: %s", mFile.c_str(), mLine, mDescription.c_str());
			return std::string(temps);
		}
	};

}

#endif