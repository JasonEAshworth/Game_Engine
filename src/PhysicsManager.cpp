#include <stdafx.h>
#include <PhysicsManager.h>
#include <Gameobject.h>
#include <baseapplication.h>


template<> ssuge::PhysicsManager* ssuge::Singleton<ssuge::PhysicsManager>::msSingleton = NULL;

ssuge::PhysicsManager::PhysicsManager()
{
	
	mBroadphase = new btDbvtBroadphase();
	mCollisionConfiguration = new btDefaultCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	mSolver = new btSequentialImpulseConstraintSolver;
	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
	mDynamicsWorld->setGravity(btVector3(0,-10,0));	

	//mDebugDrawer = new OgreDebugDrawer( BASE_APPLICATION->getSceneManager() );
	//mDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe );
    //mDynamicsWorld->setDebugDrawer( mDebugDrawer );
}

ssuge::PhysicsManager::~PhysicsManager()	
{
    if (mDynamicsWorld)
	    delete mDynamicsWorld;
    if (mSolver)
        delete mSolver;
    if (mDispatcher)
        delete mDispatcher;
    if (mCollisionConfiguration)
        delete mCollisionConfiguration;
    if (mBroadphase)
        delete mBroadphase;
}

void ssuge::PhysicsManager::addToWorld(btRigidBody* component)
{
	mDynamicsWorld->addRigidBody(component);
}

void ssuge::PhysicsManager::removeFromWorld(btRigidBody* component)
{	
	mDynamicsWorld->removeRigidBody(component);
}

int ssuge::PhysicsManager::updateWorld(btScalar dT, int maxSubSteps, btScalar fixedTimeStep)
{
	///The first and third parameters to stepSimulation are measured in seconds, and not milliseconds.
	mDynamicsWorld->stepSimulation(dT);


	//comment out this next line if you do not want to see the rigid body meshes drawn in the scene.
	mDynamicsWorld->debugDrawWorld();


	int numManifolds = mDynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  mDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
		const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());

		GameObject* a = static_cast<GameObject*>(obA->getCollisionShape()->getUserPointer());
		GameObject* b = static_cast<GameObject*>(obB->getCollisionShape()->getUserPointer());

		//std::cout << "Collision between: ";
		//std::cout << a->getName() << "  And  ";
		//std::cout << b->getName() << std::endl;

		///This part is broken!  Thanks Corey and Random Guy.
		a->injectScriptEvent("onCollide", createMessage("g", b));
		b->injectScriptEvent("onCollide", createMessage("g", a));
		
	}
	
	return 0;
}

