#include <stdafx.h>
#include <BaseApplication.h>
#include <CameraComponent.h>
#include <GameObject.h>
#include <InputManager.h>

/// Template-specialization of the msSingleton for MeshComponents.  Note:
/// We'll have to do this for every ssuge::Singleton-derived class
template <> std::vector<ssuge::CameraComponent*> ssuge::Factory<ssuge::CameraComponent>::msInstances;



ssuge::CameraComponent::CameraComponent(GameObject * owner) : Component(owner), mForwardRate(10.0f), 
    mBackwardRate(10.0f), mStrafeRate(10.0f), mIsFirstPerson(true), mYawRate(90.0f), mPitchRate(90.0f)
{
	mCamera = BASE_APPLICATION->getSceneManager()->createCamera(mOwner->getName() + "_camera");
	mOwner->getSceneNode()->attachObject(mCamera);
    mForwardDir = Ogre::Vector3::NEGATIVE_UNIT_Z;
    mRightDir = Ogre::Vector3::UNIT_X;
	mIsFirstPerson = false;
	mEnable = true;
}



ssuge::CameraComponent::~CameraComponent()
{
    if (mCamera)
        BASE_APPLICATION->getSceneManager()->destroyCamera(mOwner->getName());
}



void ssuge::CameraComponent::setNear(float d)
{
	mCamera->setNearClipDistance(d);
}



void ssuge::CameraComponent::setPosition(Ogre::Vector3 v)
{
	mCamera->setPosition(v);
}

void ssuge::CameraComponent::setOrientation(Ogre::Quaternion q)
{
	mCamera->setOrientation(q);
}



ssuge::Message ssuge::CameraComponent::execute(const char *operation,  const Message & args)
{
	Message toLua;
	toLua.addString("A dummy message.");
	if(strcmp(operation, "setFirstPerson") == 0)
	{
		setFirstPerson(args.getInt(0) != 0);
	}
	else if(strcmp(operation, "setPosition") == 0)
	{
		setPosition(Ogre::Vector3(args.getFloat(0), args.getFloat(1), args.getFloat(2)));
	}
	else if(strcmp(operation, "setModelOrietation") == 0)
	{
		setModelOrietation(Ogre::Vector3(args.getFloat(0), args.getFloat(1), args.getFloat(2)), Ogre::Vector3(args.getFloat(3), args.getFloat(4), args.getFloat(5)));
	}
	else if(strcmp(operation, "setRotationRates") == 0)
	{
		setRotationRates(args.getInt(0) != 0, args.getFloat(1), args.getFloat(2));
	}
	else if(strcmp(operation, "setNear") == 0)
	{
		setNear(args.getFloat(0));
	}
	else if(strcmp(operation, "activate") == 0)
		BASE_APPLICATION->setActiveCamera(mOwner);
	return toLua;
}


void ssuge::CameraComponent::update(float dT)
{
	if (mIsFirstPerson && BASE_APPLICATION->getActiveCamera() != NULL && BASE_APPLICATION->getActiveCamera()->getName() == mOwner->getName())
	{
        // Talk to the input manager to get rhoriz movement to rotate the
        // camera and gameobject
        // FINISH ME!

        // Talk to the input manager to get horiz / vert movement to move
        // the parent game object in the forward / right direction.
		// vmove and hmove are negated so the camera moves in the correct
		// direction for first person perception
        float hmove = INPUT_MANAGER->getHorizAxis(INPUT_MANAGER->D_ALL);
	    float vmove = INPUT_MANAGER->getVertAxis(INPUT_MANAGER->D_ALL);
		float mhmove = INPUT_MANAGER->getRHorizAxis(INPUT_MANAGER->D_ALL);
		float mvmove = INPUT_MANAGER->getRVertAxis(INPUT_MANAGER->D_ALL);
		
		mCamera->rotate(Ogre::Quaternion(Ogre::Degree(mPitchRate * mvmove * dT), Ogre::Vector3::UNIT_X));
		if (mCamera->getOrientation().getPitch() > Ogre::Radian(1.0))
			mCamera->setOrientation(Ogre::Quaternion(Ogre::Radian(1.0f), Ogre::Vector3::UNIT_X));
		if (mCamera->getOrientation().getPitch() < Ogre::Radian(-1.0))
			mCamera->setOrientation(Ogre::Quaternion(Ogre::Radian(-1.0f), Ogre::Vector3::UNIT_X));
		
        if (vmove > 0)
            mOwner->getSceneNode()->translate(mForwardDir * mForwardRate * -vmove * dT, Ogre::Node::TS_LOCAL);
		else
            mOwner->getSceneNode()->translate(mForwardDir * mBackwardRate * -vmove * dT, Ogre::Node::TS_LOCAL);
        mOwner->getSceneNode()->translate(mRightDir * mStrafeRate * -hmove * dT, Ogre::Node::TS_LOCAL);
		mOwner->rotate(Ogre::Quaternion(Ogre::Degree(mYawRate * -mhmove * dT), Ogre::Vector3::UNIT_Y));
	}
}