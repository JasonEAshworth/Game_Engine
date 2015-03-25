#include <stdafx.h>
#include <BaseApplication.h>
#include <MeshComponent.h>
#include <GameObject.h>


/// Template-specialization of the msInstances for MeshComponents.  Note:
/// We'll have to do this for every ssuge::Factory-derived class
template <> std::vector<ssuge::MeshComponent*> ssuge::Factory<ssuge::MeshComponent>::msInstances;



ssuge::MeshComponent::MeshComponent(GameObject * owner) : Component(owner), mEntity(NULL), 
    mCurrentAnimation(NULL), mNextAnimation(NULL), mPlayAnimationForward(true), 
	mTransitionTimeTotal(0.0f), mTransitionTimeCur(0.0f), mNextLooping(false),
    mAnimationLooping(false)
{
	mEnable = true;
}

ssuge::MeshComponent::~MeshComponent()
{
    if (mEntity)
	{
	    mEntity->detachFromParent();
	    BASE_APPLICATION->getSceneManager()->destroyEntity(mEntity);
	}
}

ssuge::Message ssuge::MeshComponent::execute(const char* operation,  const ssuge::Message & args)
{
	Message toLua;
	toLua.addString("A dummy message.");
	if(strcmp(operation, "loadMesh") == 0)
	{
		loadMesh(args.getString(0));
	}
	else if(strcmp(operation, "queueAnimation") == 0)
	{
		queueAnimation(args.getString(0), args.getInt(1) == 1, args.getFloat(2), args.getInt(3) == 1);
	}

	return toLua;
}

int ssuge::MeshComponent::loadMesh(std::string fileName)
{
	Ogre::SceneManager * sm = BASE_APPLICATION->getSceneManager();
	mEntity = sm->createEntity(mOwner->getName() + "_mesh", fileName);
	Ogre::SceneNode * sn = mOwner->getSceneNode();
	sn->attachObject(mEntity);
	
	



	return 0;
}



int ssuge::MeshComponent::queueAnimation(std::string animName, bool looped, float transitionTime, bool playForward)
{
    // FINISH ME.  Note the comments on attributes in the .h file.  We're mainly going to be setting
    //  attribute values here.  The "hard" work will be done in the update method.
	
	if(mCurrentAnimation == NULL)
	{
		mCurrentAnimation = mEntity->getAnimationState(animName);
		mCurrentAnimation->setEnabled(true);
		mCurrentAnimation->setLoop(looped);
		return 0;
	}

	
	if(transitionTime > 0.0 && mCurrentAnimation != NULL)
	{
	

		Ogre::AnimationState *tmp = mCurrentAnimation;
		if(mEntity->hasAnimationState(animName))
		{
			//BASE_APPLICATION->logMessage("YEP");
			mNextAnimation = mEntity->getAnimationState(animName);
			mNextLooping = looped;
			mNextAnimation->setLoop(looped);
			//mNextAnimation->setEnabled(true);
			mNextAnimation->setWeight(0.0);
			mNextAnimation->setTimePosition(0.0);
			mPlayAnimationForward = playForward;
			mTransitionTimeTotal = transitionTime;
			mTransitionTimeCur = 0.0;
			
			return 0;
		}
		
	}
    else return 1;   // Temporary.  This indicates it *wasn't* queued.  Replace this with a 0 (and possibly
					 // add another return 1 statement elsewhere if an error occurrs).
}




void ssuge::MeshComponent::update(float dT)  
{
	static float accum = 0.0f;

		//=================YOST STUFF==========================
	float gyro[3];
	float accel[3];
	float compass[3];
	accum += dT;
	GameObject * yost = BASE_APPLICATION->findGameObject("testGhost");
	if(BASE_APPLICATION->mD_device == TSS_NO_DEVICE_ID)
	{
		if (accum >  .25)
		{
			BASE_APPLICATION->mTss_error= tss_getAllCorrectedComponentSensorData(BASE_APPLICATION->mDevice, gyro, accel, compass, NULL);
			Ogre::Matrix3 m;
			Ogre::Vector3 axis;
			Ogre::Degree angle;
			m.FromEulerAnglesXYZ(Ogre::Degree(gyro[0]+compass[0]), Ogre::Degree(gyro[1]+compass[1]), Ogre::Degree(gyro[2]+compass[2]));
			m.ToAngleAxis(axis, angle);
			Ogre::Quaternion q(angle, axis);
	
			yost->rotate(q);
			accum = 0;
		}
	}
/*	Ogre::Skeleton * yostSkel = yost->getMeshComponent()->getEntity()->getSkeleton();
	Ogre::Bone * sword = yostSkel->getBone("Joint22");
	sword->setManuallyControlled(true);
	sword->setOrientation(1,gyro[0],gyro[1],gyro[2]);
	*/
	

	
//	if (!mTss_error)
//		printf("Gyro:  %f, %f, %f\n", gyro[0],gyro[1],gyro[2]);

	//else
		//printf("TSS_Error: %s\n", TSS_Error_String[mTss_error]);
	//=================YOST STUFF==========================
    // Update the current animation and the transition to the next (if there is one).


	if(mCurrentAnimation)
	{
        bool atEnd = false;

        if (!mCurrentAnimation->getLoop())
		{
            // We're not looping, which means there *could* be a next animation queued up.
            // Here, though, just see if we'll reach the end of the current animation
            if (mPlayAnimationForward)
		    {
                if (mCurrentAnimation->getTimePosition() + dT >= mCurrentAnimation->getLength())
                   atEnd = true;
			    else
                    mCurrentAnimation->addTime(dT);
		    }
		    else
		    {
                if (mCurrentAnimation->getTimePosition() - dT <= 0.0f)
                    atEnd = true;
				else
                    mCurrentAnimation->addTime(-dT);
		    }
		}
		else
		{
            // We *are* looping.  Just add time to the animation state.  It will properly
            // wrap around.
            mCurrentAnimation->addTime((mPlayAnimationForward ? 1.0f : -1.0f) * dT);
		}

        if (mNextAnimation != NULL)
		{
			if(mCurrentAnimation->getLoop())
			{
				Ogre::AnimationState *tmp = mCurrentAnimation;
				float stopTime = mCurrentAnimation->getTimePosition();
				//mCurrentAnimation->setEnabled(false);
				//mNextAnimation->setEnabled(true);
				//mCurrentAnimation = mNextAnimation;
			    //mNextAnimation = tmp;
				//mCurrentAnimation->setWeight(1-((dT+mTransitionTimeCur)/mTransitionTimeTotal));
				//mNextAnimation->setWeight((dT+mTransitionTimeCur)/mTransitionTimeTotal);
				mNextAnimation->setEnabled(true);
				mCurrentAnimation->setWeight(0.0);
				mNextAnimation->setWeight(1.0);
			}
			
			if (atEnd)
			{

				// FINISH ME.  Just turn off the current animation and make next animation
				// the current.
				//mCurrentAnimation->setEnabled(false);
				//mCurrentAnimation = mNextAnimation;
			}
			/*else
			{
				// FINISH ME.  Adjust the transition time (and the weights on the
				// current and next animation).
				mCurrentAnimation->setWeight(1-((dT+mTransitionTimeCur)/mTransitionTimeTotal));
				mNextAnimation->setWeight((dT+mTransitionTimeCur)/mTransitionTimeTotal);
			}*/
		}
		else
		{
		//	mCurrentAnimation->setEnabled(false);
			
		}
	}
}


