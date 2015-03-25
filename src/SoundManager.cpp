#include <stdafx.h>
#include <SoundManager.h>
#include <Exception.h>
#include <Utility.h>
#include <BaseApplication.h>

/// Template-specialization of the msSingleton for SoundManagers.  Note:
/// We'll have to do this for every ssuge::Singleton-derived class
template<> ssuge::SoundManager* ssuge::Singleton<ssuge::SoundManager>::msSingleton = NULL;



ssuge::SoundManager::SoundManager() : Singleton(), mSoundEngine(NULL), mListener(NULL),
      mBackgroundMusic(NULL)
{
	// Attempt to start up the sound engine and throw an exception if initialization failed
    mSoundEngine = irrklang::createIrrKlangDevice();
	mSoundEngine->setDopplerEffectParameters(0.5);
	if (!mSoundEngine)
		throw ssuge::Exception("Could not initialize irrKlang Sound Manager", __LINE__, __FILE__);
	loadAllFilesFromMemory();
}



ssuge::SoundManager::~SoundManager()
{
std::map<std::string, irrklang::ISoundSource*>::iterator iter;

    // Stop and destroy the background music interface
    if (mBackgroundMusic)
        mBackgroundMusic->drop();

    // Free memory held by all sound sources (in irrklang memory)
    for (iter = mISoundSources.begin(); iter != mISoundSources.end(); ++iter)
	{
        mSoundEngine->removeSoundSource(iter->first.c_str());
        iter->second = NULL;
	}
    mISoundSources.clear();

	// Delete engine, deallocate memory
	mSoundEngine->drop();
}



void ssuge::SoundManager::setListener(GameObject* gObject)
{
	mListener = gObject;
}



int ssuge::SoundManager::loadAllFilesFromMemory()
{
std::vector<std::string> file_names;
std::fstream sInput;
std::string fname, res_name;
unsigned int length;
char * buffer = NULL;

    // Find all files (in any resource group)
    findFilesWithExtension("wav", file_names);
    findFilesWithExtension("ogg", file_names);

	for(int i=0; i < (int)file_names.size(); i++)
	{
		fname = file_names[i];
        res_name = getFileNameNoPath(fname);
        res_name = toLowerCase(res_name);

		//log to debug
		BASE_APPLICATION->logMessage("Loaded sound: " + fname, true);

		// checking for file opening
		sInput.open(fname, std::ios::in | std::ios::binary);
		if(!sInput.is_open()) 
			continue;

		// gets file size
		sInput.seekg(0, sInput.end);
		length = (unsigned int)sInput.tellg();
		sInput.seekg(0, sInput.beg);

		// stores the data of file in buffer
		buffer = new char[length];
		sInput.read(buffer, length);
		sInput.close();

        // stores the ISoundSource pointer into a dictionary using the filename as the key
        irrklang::ISoundSource* snd = mSoundEngine->addSoundSourceFromMemory((void*)buffer, length, res_name.c_str(), true);
        if (snd)
		{
		    //snd->setStreamMode(irrklang::ESM_NO_STREAMING);
            //snd->setForcedStreamingThreshold(0);
            mISoundSources[res_name] = snd;
		}
		delete [] buffer;       // irrklang made a copy of the buffer -- we don't need ours.
        buffer = NULL;
	}
	return 0;
}



int ssuge::SoundManager::update(float dt)
{
	if(mListener != NULL)
	{
	    Ogre::Vector3 pos = mListener->getPosition();
	    Ogre::Vector3 vel = mListener->getVelocity();
	    mSoundEngine->setListenerPosition(irrklang::vec3df(pos.x, pos.y, pos.z), 
			                              irrklang::vec3df(vel.x, vel.y, vel.z));
	}
	return 0;
}



int ssuge::SoundManager::loadBackgroundMusic(std::string res_name)
{
std::map<std::string, irrklang::ISoundSource*>::iterator iter;

    iter = mISoundSources.find(res_name);
    if (iter == mISoundSources.end())
        return 1;
    if (mBackgroundMusic)
        mBackgroundMusic->drop();
	irrklang::ISoundSource* snd = iter->second;
	mBackgroundMusic = mSoundEngine->play2D(snd, true, false, true, false);
	return 0;
}



void ssuge::SoundManager::stopBackgroundMusic()
{
    if (mBackgroundMusic)
	{
        mBackgroundMusic->stop();
        mBackgroundMusic->drop();
        mBackgroundMusic = NULL;
	}
}



void ssuge::SoundManager::toggleBackgroundMusicPause()
{
    if (mBackgroundMusic)
	{
        if (mBackgroundMusic->getIsPaused())
            mBackgroundMusic->setIsPaused(false);
		else
            mBackgroundMusic->setIsPaused(true);
	}
}


irrklang::ISound* ssuge::SoundManager::get3DSoundInterface(std::string res_name, bool looped, bool enableSFX)
{
std::map<std::string, irrklang::ISoundSource*>::iterator iter;
    
    iter = mISoundSources.find(res_name);
    if (iter == mISoundSources.end())
        return NULL;
	irrklang::ISoundSource* snd = iter->second;
	irrklang::ISound* temp = mSoundEngine->play3D(snd, irrklang::vec3df(0,0,0), looped, true, true, enableSFX);
	return temp;
}


