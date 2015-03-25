#ifndef _PARTICLE_COMPONENT_H_
#define _PARTICLE_COMPNENT_H_
#include <stdafx.h>
#include <Factory.h>
#include <Component.h>

namespace ssuge
{
	class BaseApplication;

	class ParticleComponent : public Factory<ParticleComponent>, public Component
	{
	public:
	/******* Constructor / Deconstructor ********/
		ParticleComponent(GameObject * owner);

		virtual ~ParticleComponent();

	public:
	/******* Get / Set ********/
		virtual int getSize() {return 0;}

		ComponentType getType() {return PARTICLES;}

		// Returns enity of the emmiter
		Ogre::ParticleEmitter * getParticle() {return mEmitter;}
		Ogre::Entity * getEntity() {return mEntity;}

		void ssuge::ParticleComponent::setPosition(Ogre::Vector3 v);
	/******* Methods ********/
		// modeled after Ogre::ParticleSystemManager->createTemplate("myEmmiter", "PurpleFountain");
		int loadParticleEmitter(std::string, std::string);

		Message execute(const char *operation, const Message & args);
	/******* Handlers *******/
		void update(float dt);

		void setEnable(bool b) {mEnable = b;}

	/******* Attributes ******/
	protected:

		Ogre::ParticleEmitter *mEmitter;
		Ogre::Entity *mEntity;

		bool mEnable;
	
	
	};

}


#endif