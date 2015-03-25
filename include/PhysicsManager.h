
#ifndef _PHYSICS_MANAGER_H_
#define _PHYSICS_MANAGER_H_

#include <stdafx.h>
#include <Singleton.h>
#include <DebugDrawer.h>


/// A convenience macro to access the Physics Manager singleton.
#define PHYSICS_MANAGER static_cast<PhysicsManager*>(ssuge::PhysicsManager::getSingletonPtr())

namespace ssuge
{
    // A forward-reference to the PhysicsComponent class
    class PhysicsComponent;

    /// This manager manages all physics object and contains the central
    /// bullet update code.  Here you can also find methods to add / remove
    /// physics objects (this should be done by the PhysicsComponent)
	class PhysicsManager : public Singleton<PhysicsManager>
	{
    /***** CONSTRUCTOR / DESTRUCTOR *****/
	public:
        /// Constructor
		PhysicsManager();

        /// Destructor
		~PhysicsManager();

    /***** METHODS *****/
	
        /// Adds a new object to the simulator.  
		void addToWorld(btRigidBody* component);

        /// Removes a object from the simulator.
		void removeFromWorld(btRigidBody* component);

		/// Steps the simulator.  dT, and FixedTimeStep must be passed in as seconds, NOT Milliseconds.
		int updateWorld(btScalar dT, int maxSubSteps = 1, btScalar fixedTimeStep=btScalar(1.f)/btScalar(60.f));

	

    /***** ATTRIBUTES *****/
	protected:		
        /// The constraint solver (removes penetrations between objects)
		btSequentialImpulseConstraintSolver* mSolver;

        /// The actual physics world
		btDiscreteDynamicsWorld* mDynamicsWorld;

        /// The broadphase interface *efficiently* finds *potential* intersections between objects
		btBroadphaseInterface* mBroadphase;

        /// Contains details about how to do the detailed collision detection between objects (after they
        /// pass the broadphase test).
		btDefaultCollisionConfiguration* mCollisionConfiguration;

        /// Used to handle callback mechanisms.
		btCollisionDispatcher* mDispatcher;

		OgreDebugDrawer* mDebugDrawer;

    /***** FRIENDS *****/
        /// PhysicsComponent is a friend so they can access the add / remove methods
        friend class PhysicsComponent;
	};

}



#endif