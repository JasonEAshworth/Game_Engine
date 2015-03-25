#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stdafx.h>

namespace ssuge
{
	// Looks though the resource group set by type, and returns A StringVectorPtr 
	// of all files with the given extention
	void findFiles(Ogre::StringVector &found, std::string ext, std::string type);

	/// Gets the (lower-case) extension of the given file name
	std::string getFileNameExtension(std::string fname);

	/// Returns the name of the file (and extension) without any path information
	std::string getFileNameNoPath(std::string fname);

	/// Returns the filename (with path, if present) without an extension
	std::string getFileNameNoExtension(std::string fname);

	/// Fills v with a list of file names (with path) of all files (in registered resource
	/// locations with the given extension).  v is NOT emptied before doing this.
	void findFilesWithExtension(std::string extension, std::vector<std::string> & v);

	/// Gets the extension of the given file name
	std::string getExtension(std::string fname);

    /// Converts a string to lower-case
    std::string toLowerCase(std::string s);
}

#endif