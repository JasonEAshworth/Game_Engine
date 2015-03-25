
#ifndef _PHYSICS_COMPONENT_H_
#define _PHYSICS_COMPONENT_H_

#include <stdafx.h>
#include <Component.h>
#include <Factory.h>

namespace ssuge
{
    /// When this component is attached to a GameObject, it adds collision support
    /// (esp. callback functions).  It can optionally include a mass -- if so, this
    /// object is considered a rigid body and then will be "moved" in the physics
    /// simulator.
	class PhysicsComponent : public Factory<PhysicsComponent>, public Component
	{
    /***** CONSTRUCTOR / DESTRUCTOR *****/
	public:
        /// Constructor
        PhysicsComponent(GameObject* owner);
			
		/// Destructor
		~PhysicsComponent();

    /***** GETTERS / SETTERS *****/
	public:
        /// Returns the type of this component (PHYSICS)
        virtual Component::ComponentType ssuge::PhysicsComponent::getType() { return PHYSICS; };
			
		/// [unfinished] Returns the size (in bytes) of this component's memory footprint
		virtual int ssuge::PhysicsComponent::getSize() { return 0; };
			
		virtual void addForce(Ogre::Vector3 force);

		virtual void addTorque(Ogre::Vector3 torque);

		/// Set the Enable/Disable for the component
		void setEnable(bool b) { mEnable = b; }

		/// Get the Enable/Disable for the component
		bool getEnable() { return mEnable; }

	/***** CREATION methods *****/
	public:
        /// Makes this object a box.  To make this object "static" (not affected by other objects), set
		/// the mass to 0.0f.  Offset is the 3d offset (relative to the gameObject's coordinate space)
        /// to the center of the box.  Extents is the dimensions (in the local-x/y/z directions) of the box
        /// (note: this is centered about the center of the box).
		void loadBox(Ogre::Vector3 offset, Ogre::Vector3 extents, float mass);

        /// Makes this object a Sphere.  To make this object "static" (not affected by other objects), set
		/// the mass to 0.0f.  Offset is the 3d offset (relative to the gameObject's coordinate space)
        /// to the center of the sphere.  rad is the radius of this sphere (about the center of mass)
		void loadSphere(Ogre::Vector3 offset, float rad, float mass);

	/**** HANDLERS *****/
	public:
		/// [override] Called every frame by the parent game object
		virtual void update(float dT);

		/// [override] This method will be called BY OGRE when a (scene) node is transformed.  You should
		/// never call this method directly.
		virtual void onTransform(const Ogre::Node *);

    /***** ATTRIBUTES *****/

	public:

		float Cmass;

		Ogre::Vector3 mOffset;

	
        /// The collision shape.  Created by one of the load___ methods above.
        btCollisionShape* mShape;

        /// The rigid body.  This will be set to NULL if the mass in load____ is passed as 0.0.
        btRigidBody* mBody;

        /// Updated by bullet.  Likely includes last position, rotational velocity, etc.
        btDefaultMotionState* mMotionState;

		/// The physicscomponent version of execute, inherited from component
		Message execute(const char * operation, const Message & args);

	private:
		/// Boolean to enable/disable the cameraComponent.
		/// True is enabled, False is disabled.
		/// When disabled, the component should have no functionality.
		bool mEnable;
	};
	
}



#endif