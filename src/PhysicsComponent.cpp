#include <stdafx.h>
#include <PhysicsComponent.h>
#include <PhysicsManager.h>
#include <GameObject.h>

/// Template-specialization of the factory instance vector.
template <> std::vector<ssuge::PhysicsComponent*> ssuge::Factory<ssuge::PhysicsComponent>::msInstances;



ssuge::PhysicsComponent::PhysicsComponent(GameObject* owner) : Component(owner), mShape(NULL),
    mBody(NULL), mMotionState(NULL), mOffset(Ogre::Vector3(0.0f, 0.0f, 0.0f))
{
    mEnable = true;	
}

ssuge::PhysicsComponent::~PhysicsComponent()
{
    if (mBody)
	{
        PHYSICS_MANAGER->removeFromWorld(mBody);
	    delete mBody;
	}
    if (mMotionState)
        delete mMotionState;
    if (mShape)
	    delete mShape;
}

	
void ssuge::PhysicsComponent::loadBox(Ogre::Vector3 offset, Ogre::Vector3 extents, float mass)
{
	Cmass = mass;
	mOffset = offset;
	//extents = extents / 2;
	//Passing in a mass of 0 makes the object have inifinite mass, and it is immovable.
	mShape = new btBoxShape(btVector3(extents[0], extents[1], extents[2]));	
	mShape->setUserPointer(mOwner);
	
    // Note: I (jasonW) added this to compensate for nested / rotated objects.
	Ogre::Vector3 v = mOwner->getSceneNode()->convertLocalToWorldPosition(offset);
	std::cout << v << std::endl;
	
	v += mOwner->getSceneNode()->_getDerivedPosition();
	std::cout << v << std::endl;
	
    Ogre::Quaternion q = mOwner->getSceneNode()->_getDerivedOrientation();
	mMotionState = new btDefaultMotionState(btTransform(btQuaternion(q.x,q.y,q.z,q.w), btVector3(v[0], v[1], v[2])));

    // Construct the moment-of-inertia (I'm assuming it starts at rest -- hence the zero vector).
    btVector3 fallInertia(0,0,0);
	mShape->calculateLocalInertia(mass, fallInertia);

    // Construct the rigid body
	btRigidBody::btRigidBodyConstructionInfo mComponentFallBody(mass, mMotionState, mShape, fallInertia);
	mBody = new btRigidBody(mComponentFallBody);
	mBody->setUserPointer(mOwner);
	// Add this rigid body to the simulator.	
	PHYSICS_MANAGER->addToWorld(mBody);
};


void ssuge::PhysicsComponent::loadSphere(Ogre::Vector3 offset, float rad, float mass)
{
	Cmass = mass;
	///Passing in a mass of 0 makes the object have inifinite mass, and it is immovable.
	mShape = new btSphereShape(rad);
	mShape->setUserPointer(mOwner);

	// Note: I (jasonW) added this to compensate for nested / rotated objects.
	Ogre::Vector3 v = mOwner->getSceneNode()->convertLocalToWorldPosition(offset);
	v += mOwner->getSceneNode()->_getDerivedPosition();
    Ogre::Quaternion q = mOwner->getSceneNode()->_getDerivedOrientation();
	mMotionState = new btDefaultMotionState(btTransform(btQuaternion(q.x,q.y,q.z,q.w), btVector3(v[0], v[1], v[2])));

    // Construct the moment-of-inertia (I'm assuming it starts at rest -- hence the zero vector).
    btVector3 fallInertia(0,0,0);
	mShape->calculateLocalInertia(mass, fallInertia);

    // Constructor the rigid body.
	btRigidBody::btRigidBodyConstructionInfo mComponentFallBody(mass, mMotionState, mShape, fallInertia);
	mBody = new btRigidBody(mComponentFallBody);
	mBody->setUserPointer(mOwner);
    // Add this rigid body to the simulator.
	PHYSICS_MANAGER->addToWorld(mBody);
};


///Need to add a new update function that is called from the Game object
	//ignore if the mass is 0. ---> immovable.


void ssuge::PhysicsComponent::update(float dT)
{	
	if(!mBody)
		return;
	btTransform trans;
	
	mBody->getMotionState()->getWorldTransform(trans);

	// Tell the ogre parent scene node to match trans
	Ogre::SceneNode * sn = mOwner->getSceneNode();
	
	float x  = trans.getOrigin().getX();
	float y = trans.getOrigin().getY();
	float z = trans.getOrigin().getZ();

	btVector3 V = trans.getOrigin();
	
	Ogre::Vector3 vect = Ogre::Vector3(V.x(), V.y(), V.z());

	btQuaternion Q = trans.getRotation();

	Ogre::Quaternion quat = Ogre::Quaternion(Q.w(), Q.x(), Q.y(), Q.z());

	sn->setPosition(vect);
	sn->setOrientation(quat);
}


void ssuge::PhysicsComponent::onTransform(const Ogre::Node * n)
{
	if(Cmass < 1)
	{
		Ogre::Vector3 position = n->_getDerivedPosition();	
		Ogre::Quaternion quat = n->_getDerivedOrientation();

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(position[0] - mOffset.x, position[1] - mOffset.y, position[2] - mOffset.z));
		transform.setRotation(btQuaternion(quat.x, quat.y, quat.z, quat.w));

		mBody->setWorldTransform(transform);		
	}

}

void ssuge::PhysicsComponent::addForce(Ogre::Vector3 force)
{
	Ogre::SceneNode * sn = mOwner->getSceneNode();
	Ogre::Vector3 position = sn->getPosition();	

	mBody->applyForce(btVector3(force.x, force.y, force.z), 
		              btVector3(position.x, position.y, position.z));

}

void ssuge::PhysicsComponent::addTorque(Ogre::Vector3 torque)
{
	Ogre::SceneNode * sn = mOwner->getSceneNode();
	Ogre::Vector3 position = sn->getPosition();	

	mBody->applyTorqueImpulse(btVector3(torque.x, torque.y, torque.z));	

}


ssuge::Message ssuge::PhysicsComponent::execute(const char * operation, const Message & args)
{
	Message toLua;
	toLua.addString("A dummy message.");
	if (strcmp(operation, "addForce") == 0)
	{
		addForce(Ogre::Vector3(args.getFloat(0), args.getFloat(1), args.getFloat(2)));
	}
	else if(strcmp(operation, "addTorque") == 0)
	{
		addTorque(Ogre::Vector3(args.getFloat(0), args.getFloat(1), args.getFloat(2)));
	}
	else if (strcmp(operation, "loadBox") == 0)
	{
		loadBox(Ogre::Vector3(args.getFloat(0), args.getFloat(1), args.getFloat(2)),
				Ogre::Vector3(args.getFloat(3), args.getFloat(4), args.getFloat(5)),
				args.getFloat(6));
	}
	else if (strcmp(operation, "loadSphere") == 0)
	{
		loadSphere(Ogre::Vector3(args.getFloat(0), args.getFloat(1), args.getFloat(2)), args.getFloat(3), args.getFloat(4));
	}
	return toLua;
}