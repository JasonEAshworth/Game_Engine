#include <stdafx.h>
#include <BaseApplication.h>
#include <SoundComponent.h>
#include <SoundManager.h>
#include <Exception.h>

/// Template-specialization of the msInstances for SoundComponents.  Note:
/// We'll have to do this for every ssuge::Factory-derived class
template <> std::vector<ssuge::SoundComponent*> ssuge::Factory<ssuge::SoundComponent>::msInstances;



ssuge::SoundComponent::SoundComponent(GameObject* owner) : Component(owner), mSound(NULL)
{
	mEnable = true;
}



ssuge::SoundComponent::~SoundComponent()
{
    if (mSound)
	{
	    mSound->stop();
	    mSound->drop();
	}
}




void ssuge::SoundComponent::loadSound(std::string res_name, bool looped, bool enableSFX)
{
	if (mSound)
	{
        // Remove the old sound.
        mSound->drop();
        mSound = NULL;
	}
	mSound = SOUND_MANAGER->get3DSoundInterface(res_name, looped, enableSFX);
    if (mSound)
	{
        Ogre::Vector3 pos = mOwner->getSceneNode()->_getDerivedPosition();
        mSound->setPosition(irrklang::vec3df(pos.x, pos.y, pos.z));
        mSound->setIsPaused(false);
	}
	else
        BASE_APPLICATION->logMessage("Error loading sound '" + res_name + "'", true);
	
}

ssuge::Message ssuge::SoundComponent::execute(const char * operation, const Message & args)
{
	Message toLua;
	toLua.addString("A dummy message.");
	if (strcmp(operation, "loadSound") == 0)
	{
		loadSound(args.getString(0), args.getInt(1), args.getInt(2));
	}
	return toLua;
}

void ssuge::SoundComponent::update(float dt)
{
Ogre::Vector3 pos = mOwner->getSceneNode()->_getDerivedPosition();
	if(mSound && mSound->getSoundEffectControl() != NULL)
	{
		Ogre::Vector3 vel = this->mOwner->mVelocity;
		mSound->setVelocity(irrklang::vec3df(vel.x, vel.y, vel.z));
	}
	
	if(mSound)
		mSound->setPosition(irrklang::vec3df(pos.x, pos.y, pos.z));
}

