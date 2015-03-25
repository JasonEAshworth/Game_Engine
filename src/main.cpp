#include <stdafx.h>
#include <BaseApplication.h>
#include <Exception.h>
#include <Message.h>


class MyApplication : public ssuge::BaseApplication
{
public:
	MyApplication() : BaseApplication()
	{

	}
protected:

	virtual int onKeydown(SDL_Keycode key)
	{
		if (key == SDLK_ESCAPE)
			return 1;

		return 0;
	}
};

int main()
{
MyApplication app;

	try
	{
		if (!app.initialize())
		    app.run();
	}
	catch(Ogre::Exception & e)
	{
		MessageBox(NULL, e.getDescription().c_str(), "Ogre Error", MB_OK);
		Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, Ogre::String("[Ogre] FATAL ERROR: ") + e.getDescription(), false);
	}
	catch(ssuge::Exception & e)
	{
		MessageBox(NULL, e.getDescription().c_str(), "Engine Error", MB_OK);
		Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, Ogre::String("[SSUGE] FATAL ERROR: ") + e.mDescription, false);
	}
	
	return 0;
}
