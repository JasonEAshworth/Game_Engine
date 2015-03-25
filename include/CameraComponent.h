#ifndef _CAMERA_COMPONENT_H_
#define _CAMERA_COMPONENT_H_
#include <stdafx.h>
#include <Factory.h>
#include <Component.h>
namespace ssuge
{

	class BaseApplication;

	/// This is a component which adds camera-based support to the containing
	/// GameObject.
	class CameraComponent : public Factory<CameraComponent>, public Component
	{
    /***** CONSTRUCTOR / DESTRUCTOR *****/
	public:
		/// Constructor for camera component
		CameraComponent(GameObject * owner);

        /// Destructor
        virtual ~CameraComponent();

		/*
		Ogre::SceneNode *cameraNode;
		Ogre::SceneNode *cameraYawNode;
		Ogre::SceneNode *cameraPitchNode;
		Ogre::SceneNode *cameraRollNode;
		*/
    /***** GETTERS / SETTERS *****/
	public:
        /// [override, unfinished]: Returns the size (in bytes) of this component
		virtual int getSize() { return 0; }

		/// Returns the type of component this object is
		ComponentType getType() { return CAMERA; }

        /// Returns the Ogre camera
		Ogre::Camera * getCamera() { return mCamera; }

        /// Sets the camera's position (this value is a vector relative to the parent
        /// GameObject's coordinate space)
		void ssuge::CameraComponent::setPosition(Ogre::Vector3 v);

		/// Sets the camera's orientation (this value is a quaternion relative to the parent
		/// GameObject's coordinate space).  Note: if in first-person mode, this value will
		/// be over-written when the mouse / analog stick moves.
		void setOrientation(Ogre::Quaternion q);

        /// Sets the camera's near distance
		void ssuge::CameraComponent::setNear(float d);

        /// Sets the movement speed of the character in world units / s (if any values are negative, they are ignored)
        void setMoveSpeed(float forward = -1.0f, float side = -1.0f, float back = -1.0f);

        /// Sets the rotational parameters.  If is_fps_style is false, all other parameters are ignored and
        /// the mouse will have no effect on the parent game object.  If it is true, the yaw_rate is the number of 
        /// degrees to rotate THE GAME OBJECT for one full screen-width of horizontal mouse movement.  Similarly for the pitch_rate
        /// pitch_rate (although this one chages the pitch of the camera object).  Note: if using an fps-style
        /// camera, the look direction will be changed.
		void setRotationRates(bool is_fps_style, float yaw_rate, float pitch_rate) { mIsFirstPerson = is_fps_style; mYawRate = yaw_rate; mPitchRate = pitch_rate; }

        /// Identifies the forward and right directions (these depend on how the character was modelled).  Usually
        /// both will be one of the cardinal axes.
		void setModelOrietation(Ogre::Vector3 forwardDir, Ogre::Vector3 rightDir) { mForwardDir = forwardDir; mRightDir = rightDir; }

		/// Set whether the camera is a first person camera or not. Pass boolean.
		void setFirstPerson(bool b) { mIsFirstPerson = b; }

		/// The cameracomponent version of execute, inherited from component
		Message execute(const char *operation,  const Message & args);

		/// Set the Enable/Disable for the component
		void setEnable(bool b) { mEnable = b; }

		/// Get the Enable/Disable for the component
		bool getEnable() { return mEnable; }

    /***** HANDLERS *****/
	public:
        /// [override] This method is called by the parent GameObject once per frame
        virtual void update(float dt);

	protected:
		/// The ogre camera
		Ogre::Camera* mCamera;

        /// The model-space forward direction (usually one of the cardinal axes).  This is set by 
        /// setModelOrientation (default of [0,0,-1])
        Ogre::Vector3 mForwardDir;

        /// The model-space right direction (usually one of the cardinal axes).  This is set by
        /// setModelOrientation (default of [1,0,0])
        Ogre::Vector3 mRightDir;

        /// The speed at which the character moves forward (in world units / s).  We'll look at the
        /// vert "axis" of the input_manager.  If it is positive, we'll move the parent game object at this rate in
        /// the forward direction.
        float mForwardRate;

        /// The speed at which the character moves backwards (in world units / s).
        float mBackwardRate;

        /// The speed at which the character strafes.  This amount will be multiplied by the input managers
        /// horiz "axis" and dt.
        float mStrafeRate;

        /// Is this an fps-style camera (where the mouse / gamepad rotate the view)?  If not, it is a 
        /// fixed third-person / top-down / etc camera.
        bool mIsFirstPerson;

        /// The rate at which we yaw the GAME OBJECT if in fps-mode
		/// This is in degrees.  It is based on the RHoriz input axis (which is in the range -1.0 ... +1.0).
		/// This number is the number of degrees to rotate PER SECOND that the RAxis is held down.
        float mYawRate;

        /// The rate at which we pitch the CAMERA if in fps-mode
		/// This is in degrees.  It is based on the RVert input axis (which is in the range -1.0 ... +1.0).
		/// This number is the number of degrees to rotate PER SECOND that the RAxis is held down.
        float mPitchRate;

		/// Boolean to enable/disable the cameraComponent.
		/// True is enabled, False is disabled.
		/// When disabled, the component should have no functionality.
		bool mEnable;
	};

} 

#endif