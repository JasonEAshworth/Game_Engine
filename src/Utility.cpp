#include <stdafx.h>
#include <Utility.h>
#include <BaseApplication.h>

void ssuge::findFiles(Ogre::StringVector &found, std::string ext, std::string type) 
{
	Ogre::StringVectorPtr resorceGroup = Ogre::ResourceGroupManager::getSingleton().listResourceNames(type);
	Ogre::StringVectorPtr TEST = Ogre::ResourceGroupManager::getSingleton().listResourceNames(type, true);

	/*BASE_APPLICATION->d_print("**********************TEST*******************");
	for (std::vector<std::string>::iterator i = resorceGroup->begin(); i != resorceGroup->end(); i++) 
	{
		Ogre::String curFile = *i;
		BASE_APPLICATION->d_print(curFile);
	}
	BASE_APPLICATION->d_print("**********************END********************");
	*/

	for (std::vector<std::string>::iterator i = resorceGroup->begin(); i != resorceGroup->end(); i++) 
	{
		Ogre::String curFile = *i;
		Ogre::String curExt = (curFile).substr((curFile).rfind(".") + 1);
		if (curExt.compare(ext) == 0) 
		{
			found.push_back(curFile);
		}
	}
}


void ssuge::findFilesWithExtension(std::string extension, std::vector<std::string> & v)
{

Ogre::StringVector sv;
Ogre::StringVector::iterator svIter;
Ogre::FileInfoListPtr lv;
Ogre::FileInfoList::iterator lvIter;
std::string ext, path, full_path;
int i;

	for (i=0; i<(int)extension.size(); i++)
		extension[i] = ::tolower(extension[i]);

	sv = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
	for (svIter = sv.begin(); svIter != sv.end(); ++svIter)
	{
		lv = Ogre::ResourceGroupManager::getSingleton().listResourceFileInfo(*svIter, false);
		for (lvIter = lv->begin(); lvIter != lv->end(); ++lvIter)
		{
			ext = getExtension(lvIter->basename);
			
			if (ext == extension)
			{
				path = lvIter->archive->getName();
				if (path[path.size()-1] != '/' && path[path.size()-1] != '\\')
					full_path = path + "/" + lvIter->basename;
				else
					full_path = path + lvIter->basename;

				v.push_back(full_path);
				//rg.push_back(*svIter);
			}
				
		}
	}
}


std::string ssuge::getExtension(std::string fname)
{
int pos;
unsigned int i;
std::string ext;

	pos = fname.rfind(".");
	if (pos < 0 || pos >= (int)fname.size())
		return std::string("");

	ext = fname.substr(pos + 1);
	for (i=0; i<ext.size(); i++)
		ext[i] = ::tolower(ext[i]);
			
	return ext;
}


std::string ssuge::getFileNameNoPath(std::string fname)
{
int index, index2;

	index = fname.rfind("\\");		// windows-style
    index2 = fname.rfind("/");      // *nix-style
	if (index2 > index)     index = index2;	
	if (index == -1)
		return fname;				// no path
	else
		return fname.substr(index + 1, fname.size() - index);
}



std::string ssuge::getFileNameNoExtension(std::string fname)
{
int index;

	index = fname.rfind(".");
	if (index != -1)
		return fname.substr(0, index);
	else
		return fname;     // Didn't have an extension
}



std::string ssuge::toLowerCase(std::string s)
{
unsigned int i;
std::string news;

    news = s;

    for (i=0; i<s.size(); i++)
	{
        if (s[i] >= 'A' && s[i] <= 'Z')
            news[i] = (s[i] - 'A') + 'a';
	}

    return news;
}