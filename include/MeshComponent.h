#ifndef _MESH_COMPONENT_H_
#define _MESH_COMPONENT_H_
#include <stdafx.h>
#include <Factory.h>
#include <Component.h>

namespace ssuge
{

	/// This is a component which adds mesh-based (and possibly skeletal 
	/// animation) support to the containing GameObject.
	class MeshComponent : public Factory<MeshComponent>, public Component
	{
    /***** CONSTRUCTOR / DESTRUCTOR *****/
	public:
		/// Constructor
		MeshComponent(GameObject * owner);
		
		/// Destructor
		~MeshComponent();

    /***** GETTERS / SETTERS *****/
	public:
		/// [override, unfinished] Returns the size (in bytes) of this MeshComponent.
		virtual int getSize() { return 0; }

		/// [override] Returns the type of component this object is
		ComponentType getType() { return MESH; }

        // Returns entity of mesh
		Ogre::Entity * getEntity() { return mEntity; }

		/// Set the Enable/Disable for the component
		void setEnable(bool b) { mEnable = b; mEntity->setVisible(b); }

		/// Get the Enable/Disable for the component
		bool getEnable() { return mEnable; }

    /***** METHODS *****/
	public:
		/// Creates an Entity and attaches it to the SceneNode of the containing GameObject
		int loadMesh(std::string fileName);

		/// Queues a second animation to transition into.  If transitionTime is > 0.0 and
        /// mCurrentAnim is non-NULL, and this animation is different than
		/// the current, then this new animation will gradually transition from
        /// the current to the new animation over transitionTime seconds.  After the
        /// transition time is reached, this new animation will become THE current animation.  If the
		/// current animation is set to loop, this will cancel that.  If the loop parameter here is
		/// set to true, the queued animation will start looping once completely phased in.  The method
        /// returns 0 if the animation was successfully queued. 
        int queueAnimation(std::string animName, bool looped = false, float transitionTime = 0.0, bool playForward = true);

		/// The meshcomponent version of execute, inherited from component
		Message execute(const char *operation,  const Message & args);

    /***** HANDLERS *****/
	public:
        /// [override] Called by the containing GameObject once per frame
		void update(float dT);

    /***** ATTRIBUTES *****/
	protected:

		/// The current animation (NULL if there is none).
        Ogre::AnimationState * mCurrentAnimation;

        /// The animation we're transition to (NULL if there is none).
        Ogre::AnimationState * mNextAnimation;

        /// Are we playing the current animation backwads?
        bool mPlayAnimationForward;

        /// The total number of seconds the transition from mCurrentAnimation to mNextAnimation should take.
        float mTransitionTimeTotal;

        /// The current number of seconds we are through the transition from current to next animation.  Once
        /// mTransitionTimeTotal is reached, mCurrentAnimation will be set to mNextAnimation and mNextAnimation will
        /// be set to NULL.
        float mTransitionTimeCur;

        /// Set to true if mNextAnimation should start looping once phased in.
        bool mNextLooping;

        /// Set to true if the current animation is looped.  Note: if queueAnimation is called, this will be
        /// set to false.
        bool mAnimationLooping;

        /// The Ogre Entity the mesh is based upon. Note: one mesh can be "shared" by multiple entities
		Ogre::Entity *mEntity;

		/// Boolean to enable/disable the cameraComponent.
		/// True is enabled, False is disabled.
		/// When disabled, the component should have no functionality.
		bool mEnable;
	};

} 

#endif