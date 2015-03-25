#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <stdafx.h>

#include <Component.h>
#include <MeshComponent.h>
#include <SoundComponent.h>
#include <CameraComponent.h>
#include <PhysicsComponent.h>
#include <Message.h>
#include <Lua.hpp>
#include <ScriptManager.h>
//#include <AnimationBlender.h>



/// the maximum number of compents a GameObject can have
#define MAX_COMP 4




namespace ssuge
{
	/// A general purpose "thing" in the game.  You add functionality
	/// (and eventually scripting callback "hooks") by adding components
	/// (using a create____Component method).
	class GameObject : public Ogre::Node::Listener
	{
	public:

		class LuaGameObject
		{
		public:
			/// LuaGameObject
			/// Pointer to a gameObject to be used inside LuaGameObject
			GameObject * mPtr;
			/// Name of the gameObject 
			char mClassName [32];
			/// Used to 'create' a initalized buffer on the stack
			void initialize();
		};
		/// Constructor for a stand-alone game object
		GameObject(unsigned int tag, std::string name);

		/// Constructor for a game object which is a child of another scene node 
		GameObject(unsigned int tag, std::string name, Ogre::SceneNode * parent);

		///destructor
		~GameObject();

		/// creates a MeshComponent and adds it to mComponents
		MeshComponent* createMeshComponent();

		/// creates a SoundComponent and adds it to mComponents
		SoundComponent* createSoundComponent();

		/// returns a pointer to the MeshComponent in mComponents
		MeshComponent* getMeshComponent();

		/// returns a pointer to the SoundComponent in mComponents
		SoundComponent* getSoundComponent();

		/// returns a pointer to the CameraComponent in mComponents
		CameraComponent* getCameraComponent();

		/// creates a CameraComponent
		CameraComponent * createCameraComponent();

		/// creates a PhysicsComponent
		PhysicsComponent * createPhysicsComponent();

		/// returns a pointer to the SceneNode
		Ogre::SceneNode * getSceneNode();
		
		/// lua push method
		void _l_gameObjectPush(lua_State * L, ssuge::GameObject * gO);

		/// lua pop method
		void _l_gameObjectPop(lua_State * L, ssuge::GameObject * gO);

		/// creates new class based on game object
		static int _l_createGameObjectClass(lua_State *L);

		/// creates a new instance of a game object class
		static int _l_newGameObject(lua_State *L);

		/// Creates a new (custom) field in the user-date for this game object
		static int __lua_GameObject_newindex(lua_State * L);

		int get_tag();

		/// Returns the name of this GameObjects
		std::string getName() { return mName; }

		///Tells if the object has any animation
		bool mAnimation;


		/// Updates this GameObject and all attached components (called every frame)
		void update(float dT);

		/// Creates a pre-scripted animation
		void createAnimation(float animLength);

		/// Destroys the GameObject's animation (no effect if there is none)
		void destroyAnimation();

		/// Adds a keyframe to the animation (no effect if there is currently no animation)
		void addAnimationKeyframe(float timeOffset, Ogre::Vector3 pos = Ogre::Vector3::ZERO, 
			                                        Ogre::Vector3 scale = Ogre::Vector3::UNIT_SCALE,
													Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY);

		///Node Update.  O
		virtual void nodeUpdated(const Ogre::Node * n);

		/// Returns the position of the gameObject as an Ogre Vec3
		Ogre::Vector3 getPosition();
		
		///Set Position
		virtual void setPosition(Ogre::Vector3 v);

		///Get Scale
		virtual void setScale(Ogre::Vector3 v);

		///Do a rotation
		virtual void rotate(Ogre::Quaternion q);

		/// Sets whether the object can have a script (i can haz scriptz?)
		void canHaveScript(bool canHave);

		/// Inject a script Event each frame if object has a script attached to it
		virtual Message injectScriptEvent(std::string method, Message m);

		/// Identifies the forward(these depend on how the character was modelled).  Usually
        /// both will be one of the cardinal axes.
		void setModelFacing(Ogre::Vector3 forwardDir) { mFacing = forwardDir; }

		Ogre::Vector3 getModelFacing() { return mFacing; }

		/// executes a method using args
		Message execute(const char * operation, const Message & args);

		/// gets the value of "item"
		Message getData(const char * item);

        /// Gets the velocity of the object
		Ogre::Vector3 getVelocity() { return mVelocity; }

		/// registers the __lua_GameObject_* methods with lua
		static int __lua_GameObject_register_class(lua_State * L);

		/// executes the given operation on the given object using the data in the Message object
		/// the Message object will be on top of the lua stack followed by the operation, followed by the name of the object to execute on
		static int __lua_GameObject_exec(lua_State * L);

		/// returns the value of the given attribute name
		/// the attribute name will be on top of the lua stack
		static int __lua_GameObject_get(lua_State * L);

		///Things used to get the Velocity of a Game Object		
		int mOldTime;
		int mNewTime;
		Ogre::Vector3 mOldPosition;
		Ogre::Vector3 mVelocity;		

		/// Pass component type and boolean to enable/disable component of the GameObject
		void enableComponent(Component::ComponentType t, bool b);

	protected:
		///a tag that can be used to reference the GameObject
		unsigned int mTag;
		///a unique name for the GameObject
		std::string mName;
		///the Ogre SceneNode for the GameObject in the scene
		Ogre::SceneNode * mNode;
		/// Does this game object have an animation?
		bool mHasAnimation;
		/// Does this game object have a script?
		bool mHasScript;

		/// Facing direction of the object currently
		Ogre::Vector3 mFacing;

		//Component * mComponents[MAX_COMP];
		///an array of pointers to components of the GameObject
		Component * mComponents[MAX_COMP];
	};
}

#endif