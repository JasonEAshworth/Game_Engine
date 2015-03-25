#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <stdafx.h>
#include <Message.h>

namespace ssuge
{
	// A forward-reference to the GameObject class.  You can use this if you're
	// only using GameObject*'s here (not any methods).  This prevents a circular
	// dependency that would occur if we included "GameObject.h" here (since
	// GameObject.h includes Component.h.  We'll still need to include GameObject.h
	// in Component.cpp (or any .cpp files which implement Component-derived classes)
	class GameObject;

	/// This is the base-class for all Components which can be "snapped-in" to
	/// a GameObject.  It is mainly an interface class.  It is up to the derived
	/// class to provide actual implementations of the pure virtual methods.
	class Component
	{
    /***** ENUMERATIONS *****/
	public:
		/// This is the master list of all components types.  As we add more
		/// component types, make sure you add to this enumeration (and add a
		/// create___ method to the GameObject class
		enum ComponentType { MESH, SOUND, CAMERA, PHYSICS, PARTICLES};

    /***** CONSTRUCTOR / DESTRUCTOR *****/
	public:
		/// Constructor
		Component(GameObject * owner) : mOwner(owner) {}

        /// Destructor
		virtual ~Component() {}

    /***** GETTERS / SETTERS *****/
	public:
		/// Returns the type of this component
		virtual ComponentType getType() = 0;

		/// Returns the size (in bytes) of this component.
		virtual int getSize() = 0;

		/// Set the mEnable attribute of the component
		virtual void setEnable(bool b) = 0;

		/// Get the mEnable attribute of the component
		virtual bool getEnable() = 0;

        /// Returns the containing game object
		GameObject *getOwner() { return mOwner; }

    /***** HANDLERS *****/
	public:
		/// This method will be called BY OGRE when a (scene) node is transformed.  You should
		/// never call this method directly.
		virtual void onTransform(const Ogre::Node *) {}

	/***** METHODS *****/	
	public:
		/// This method is called ONCE each frame BY THE OWNER (a GameObject).
		virtual void update(float dT) { }

		/// excutes a method using args (redefine if needed)
		virtual Message execute(const char *operation,  const Message & args) { Message m; return m; }

		/// gets the value of "item" (redefine if needed)
		virtual Message getData(std::string item) { Message m; return m;	}

    /**** ATTRIBUTES *****/
	protected:
		/// The pointer to the containing GameObject
		GameObject *mOwner;
	};
}
#endif