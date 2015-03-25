#include <stdafx.h>
#include <GameObject.h>
#include <BaseApplication.h>
#include <Message.h>


/// A convenience shortcut to access the BaseApplication singleton
#define BASE_APPLICATION static_cast<BaseApplication*>(ssuge::BaseApplication::getSingletonPtr())

ssuge::GameObject::GameObject(unsigned int tag, std::string name) : mTag(tag), mName(name), mHasScript(false)
{
Ogre::SceneManager * sm;
BaseApplication * ba;


	ba = ssuge::BaseApplication::getSingletonPtr();
	sm = ba->getSceneManager();
	mNode = sm->getRootSceneNode()->createChildSceneNode(name);
	mNode->setListener(this);
	mAnimation = false;
	mOldTime = BASE_APPLICATION->getTime();
	for(int i = 0; i < MAX_COMP; i++)
	{
		mComponents[i] = NULL;
	}

	BASE_APPLICATION->logMessage("GameObject Created: " + name, true);
}

ssuge::GameObject::GameObject(unsigned int tag, std::string name, Ogre::SceneNode * parent) : mTag(tag), mName(name)
{
	mNode = parent->createChildSceneNode(name);
	mAnimation = false;
	mOldTime = BASE_APPLICATION->getTime();
	for(int i = 0; i < MAX_COMP; i++)
	{
		mComponents[i] = NULL;
	}

	BASE_APPLICATION->logMessage("GameObject Created: " + name, true);
}

ssuge::GameObject::~GameObject()
{
	mNode->setListener(NULL);
	mNode->removeAndDestroyAllChildren();
	BASE_APPLICATION->getSceneManager()->destroySceneNode(mNode);
	for(int i = 0; i < MAX_COMP; i++)
	{
		if(i == ssuge::Component::MESH)
			ssuge::MeshComponent::deallocate(mComponents[i]);
		else if(i == ssuge::Component::SOUND)
			ssuge::SoundComponent::deallocate(mComponents[i]);
		else if(i == ssuge::Component::CAMERA)
			ssuge::CameraComponent::deallocate(mComponents[i]);
		else if(i == ssuge::Component::PHYSICS)
			ssuge::CameraComponent::deallocate(mComponents[i]);


		delete mComponents[i];
	}
	
}

ssuge::MeshComponent* ssuge::GameObject::createMeshComponent()
{
	MeshComponent * mc = ssuge::MeshComponent::allocate(this);
	mComponents[(int)ssuge::Component::MESH] = mc;
	BASE_APPLICATION->logMessage("\t\tMesh Component created", true);
	return mc;
}

ssuge::PhysicsComponent* ssuge::GameObject::createPhysicsComponent()
{
	PhysicsComponent * pc = ssuge::PhysicsComponent::allocate(this);
	mComponents[(int)ssuge::Component::PHYSICS] = pc;
	BASE_APPLICATION->logMessage("\t\tPhysics Component created", true);
	return pc;
}


ssuge::SoundComponent* ssuge::GameObject::createSoundComponent()
{
	SoundComponent * sc = ssuge::SoundComponent::allocate(this);
	mComponents[(int)ssuge::Component::SOUND] = sc;
	BASE_APPLICATION->logMessage("\t\tSound Component created", true);
	return sc;
}

ssuge::CameraComponent * ssuge::GameObject::createCameraComponent()
{
	CameraComponent * cc = ssuge::CameraComponent::allocate(this);
	mComponents[(int)ssuge::Component::CAMERA] = cc;
	BASE_APPLICATION->logMessage("\t\tCamera Component created", true);
	return cc;
}


ssuge::MeshComponent * ssuge::GameObject::getMeshComponent()
{
	return static_cast<MeshComponent *>(mComponents[(int)ssuge::Component::MESH]);
}

ssuge::SoundComponent * ssuge::GameObject::getSoundComponent()
{
	return static_cast<SoundComponent *>(mComponents[(int)ssuge::Component::SOUND]);
}

ssuge::CameraComponent * ssuge::GameObject::getCameraComponent()
{
	return static_cast<CameraComponent *>(mComponents[(int)ssuge::Component::CAMERA]);
}

Ogre::SceneNode * ssuge::GameObject::getSceneNode()
{
	return mNode;
}

void ssuge::GameObject::nodeUpdated(const Ogre::Node * n)
{
	mNewTime = BASE_APPLICATION->getTime();
	float dT = (mNewTime - mOldTime)/1000.0f;
	if (dT != 0.0f)
	{
		Ogre::Vector3 CurrentPosition = n->getPosition();
		mVelocity = (CurrentPosition - mOldPosition)/dT;
		/// Goes through all components and if they are not NULL, calls onTransform
		for(int i=0;i<MAX_COMP;i++)
		{
			if(mComponents[i] != NULL)
			{				
				mComponents[i]->onTransform(n);
			}
		}
		mOldPosition = CurrentPosition;
		mOldTime = mNewTime;
	}
}

Ogre::Vector3 ssuge::GameObject::getPosition()
{
	return mNode->getPosition();
}

void ssuge::GameObject::setPosition(Ogre::Vector3 v)
{
	mNode->setPosition(v);
}

void ssuge::GameObject::setScale(Ogre::Vector3 v)
{
	mNode->setScale(v);
}
void ssuge::GameObject::rotate(Ogre::Quaternion q)
{
	mNode->rotate(q);
}
void ssuge::GameObject::createAnimation(float animLength)
{

	//Ogre::SceneNode* animNode = BASE_APPLICATION->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	//animNode->setPosition(0,0,0);

	Ogre::Animation* mAnim = BASE_APPLICATION->getSceneManager()->createAnimation(mName + "an1", animLength);

	mAnim->setInterpolationMode(Ogre::Animation::IM_SPLINE);

	
	Ogre::NodeAnimationTrack* track = mAnim->createNodeTrack(1, mNode);
	
	Ogre::AnimationState * mAnimState = BASE_APPLICATION->getSceneManager()->createAnimationState(mAnim->getName());

	mAnimState->setEnabled(true);
	mAnimation = true;

}

int ssuge::GameObject::get_tag()
{
	return mTag;
}

void ssuge::GameObject::addAnimationKeyframe(float timeOffset, Ogre::Vector3 pos, Ogre::Vector3 scale, Ogre::Quaternion rotation)
{

	Ogre::Animation* mAnim = BASE_APPLICATION->getSceneManager()->getAnimation(mName + "an1");

	Ogre::NodeAnimationTrack* track = mAnim->getNodeTrack(1);

	Ogre::TransformKeyFrame* kf = track->createNodeKeyFrame(timeOffset);	

	kf->setTranslate(pos);
	
	/*

	kf->setRotation(rotation);
	*/
	kf->setScale(scale);
}

void ssuge::GameObject::update(float dT)
{
	if(mAnimation)
		BASE_APPLICATION->getSceneManager()->getAnimationState(mName + "an1")->addTime(dT);
	
	for (int i = 0; i < MAX_COMP; i++)
	{
		if (mComponents[i] && mComponents[i]->getEnable() == true)
			mComponents[i]->update(dT);
	}
	
	if(mHasScript)
		injectScriptEvent("onUpdate", createMessage("f", dT));

	//BASE_APPLICATION->findGameObject("Ninja")->getMeshComponent()->getEntity()->\
		//getAnimationState("Walk")->addTime(dT);
	/*Ogre::AnimationStateSet *AniSet = BASE_APPLICATION->findGameObject("Ninja")->getMeshComponent()->getEntity()->\
		getAllAnimationStates();
	Ogre::ConstEnabledAnimationStateIterator iter = AniSet->getEnabledAnimationStateIterator();
	while(iter.hasMoreElements())
	{
		std::string aName = iter.peekNext()->getAnimationName();
		Ogre::AnimationState *as = BASE_APPLICATION->findGameObject("Ninja")->getMeshComponent()->getEntity()->\
			getAnimationState(aName);
		if(as->getEnabled())
		{
			//as->setWeight(.5); this is kinda derpy looking
			as->addTime(dT/4); // this is done so that the animation can actually be seen
		}
		iter.moveNext();

	}*/
}


void ssuge::GameObject::destroyAnimation()
{
	BASE_APPLICATION->getSceneManager()->destroyAnimationState(mName + "an1");

	BASE_APPLICATION->getSceneManager()->destroyAnimation(mName + "an1");
	mAnimation = false;
}

void ssuge::GameObject::canHaveScript(bool canHave)
{
	mHasScript = canHave;
}

ssuge::Message ssuge::GameObject::injectScriptEvent(std::string method, ssuge::Message m)
{
Message rv;
lua_State* L = SCRIPT_MANAGER->getLuaState();

	//SCRIPT_MANAGER->stack_dump(L);

	//std::cout << mName << std::endl;
	if (!mHasScript)
		return rv; // return empty message if gameobject doesn't have scripts attached

    // TO-DO: Change the following line so that we look for the global object with name mName (I think
	// now we're looking for a *class* with name mName.  If we can't find it, something's wrong 
	// in GameObject::_l_newGameObject. If we do find it, get the field like you are.  
	lua_getglobal(L, mName.c_str());
	if(lua_isuserdata(L,-1) == 0)
	{
		lua_pop(L, 2);
		return rv; // return empty message when we can't find the object in the global table
	}

	lua_getfield(L,-1,method.c_str());
	if(lua_isfunction(L,-1) == 0)
	{
		lua_pop(L, 3);
		return rv; // return empty message if the requested method doesn't exist in the game object table
	}
	lua_pushvalue(L, -2); // makes a copy of the GameObject on top of the stack
    Message::__lua_push_Message(L, m);

    // TO-DO (later): Look at the return value of lua_pcall (it might be a message object).
	int result = lua_pcall(L, 2, 0, 0);
	if (result)
	{
		BASE_APPLICATION->logMessage("[script error]" + std::string(lua_tostring(L, -1)), false);
	}

	lua_settop(L, 0);

	return rv;
}
ssuge::Message ssuge::GameObject::execute(const char * operation, const Message & args)
{
Message toLua;

	if(strcmp(operation, "setPosition") == 0)
	{
		float x = (float) args.getFloat(0);
		float y = (float) args.getFloat(1);
		float z = (float) args.getFloat(2);
		setPosition(Ogre::Vector3(x,y,z));
	}
	else if(strcmp(operation, "createComponent") == 0)
	{
		std::string compType = args.getString(0);
		if (compType == "mesh")
			createMeshComponent();
		else if(compType == "sound")
			createSoundComponent();
		else if(compType == "camera")
			createCameraComponent();
		else if(compType == "physics")
			createPhysicsComponent();
	}
	else if(strcmp(operation, "getTag") == 0)
	{
		int tag = (int)mTag;
		toLua.addInt(tag);
	}
	else if(strcmp(operation, "setScale") == 0)
	{
		float x = (float) args.getFloat(0);
		float y = (float) args.getFloat(1);
		float z = (float) args.getFloat(2);
		setScale(Ogre::Vector3(x,y,z));
	}
	else if(strcmp(operation, "rotate") == 0)
	{
		float angle = (float) args.getFloat(0);
		float x = (float) args.getFloat(1);
		float y = (float) args.getFloat(2);
		float z = (float) args.getFloat(3);
		rotate(Ogre::Quaternion(Ogre::Degree(angle), Ogre::Vector3(x, y, z)));
	}
	else if(strcmp(operation, "createAnimation") == 0)
	{
		float animLen = (float) args.getFloat(0);
		createAnimation(animLen);
	}
	else if(strcmp(operation, "addAnimationKeyframe") == 0)
	{
		float timeOffset = (float) args.getFloat(0);
		float x = (float) args.getFloat(1);
		float y = (float) args.getFloat(2);
		float z = (float) args.getFloat(3);
		float scX = (float) args.getFloat(4);
		float scY = (float) args.getFloat(5);
		float scZ = (float) args.getFloat(6);
		addAnimationKeyframe(timeOffset, Ogre::Vector3(x, y, z), Ogre::Vector3(scX, scY, scZ));
	}
	else if(strcmp(operation, "getPosition") == 0)
	{
		Ogre::Vector3 pos = getPosition();
		float x = pos.x;
		float y = pos.y;
		float z = pos.z;
		toLua.addFloat(x);
		toLua.addFloat(y);
		toLua.addFloat(z);
	}
	else if(strcmp(operation, "getName") == 0)
	{
		toLua.addString(getName());
	}
	else if (strcmp(operation, "getGameObject") == 0)
	{
		std::string objName = args.getString(0);
		std::map<std::string, GameObject *>::iterator iter;
		iter = BASE_APPLICATION->mObjects.find(objName);
		if(iter != BASE_APPLICATION->mObjects.end())
		{
			toLua.addGameObject(BASE_APPLICATION->mObjects[iter->first]);
		}
	}
	else if (strcmp(operation, "getGameObjectsWithTag") == 0)
	{
		int objTag = args.getInt(0); // tag name
		std::vector<GameObject*> v;
		BASE_APPLICATION->findObjectsByTag(v, objTag);
		
		for(unsigned int i = 0; i < v.size(); i++)
		{
			toLua.addGameObject(v[i]);
		}
	}
	else if (strcmp(operation, "lookAt") == 0)
	{
		Ogre::Vector3 pos = Ogre::Vector3(args.getFloat(0),args.getFloat(1),args.getFloat(2));
		Ogre::Vector3 lookAxis = Ogre::Vector3(args.getFloat(3),args.getFloat(4),args.getFloat(5));
		Ogre::SceneNode* objNode = this->getSceneNode();
		objNode->lookAt(pos,Ogre::Node::TS_WORLD, lookAxis);
	}
	else if (strcmp(operation, "moveTo") == 0)
	{
		Ogre::Vector3 point = Ogre::Vector3(args.getFloat(0),args.getFloat(1),args.getFloat(2));
		float speed = args.getFloat(3);
		float dt = args.getFloat(4);
		Ogre::SceneNode* myNode = this->getSceneNode();
		Ogre::Vector3 pos = myNode->getPosition();
		Ogre::Vector3 dir = point - myNode->getPosition();
		Ogre::Vector3 normDir = dir;
		normDir.normalise();
		Ogre::Vector3 moveVec = normDir*speed*dt;
		if (dir.length() > moveVec.length())		 // move towards
			myNode->setPosition(pos + moveVec);
		else									 // set position at
			myNode->setPosition(point);
	}
	else if (strcmp(operation, "moveFoward") == 0) // not currently reliable
	{
		float dt = args.getFloat(0);
		float moveSpeed = args.getFloat(1);
		Ogre::SceneNode* objNode = this->getSceneNode();
		Ogre::Vector3 forward = this->getModelFacing();
		Ogre::Vector3 objPos = objNode->getPosition();
		Ogre::Vector3 moveVec = objPos + (forward * dt * moveSpeed);
		objNode->setPosition(moveVec);
	}
	else if (strcmp(operation, "setModelFacing") == 0)
	{
		Ogre::Vector3 facing = Ogre::Vector3(args.getFloat(0),args.getFloat(1),args.getFloat(2));
		
		this->setModelFacing(facing);
	}
	else if (strcmp(operation, "rotateTowards") == 0)
	{
		// get position and amount passed from lua
		Ogre::Vector3 targetPos = Ogre::Vector3(args.getFloat(0),args.getFloat(1),args.getFloat(2));
		float amt = args.getFloat(3);
		Ogre::Vector3 TD = Ogre::Vector3(targetPos-this->getPosition()).normalisedCopy();
		
		Ogre::SceneNode* objNode = this->getSceneNode();
		Ogre::Vector3 facing = this->getModelFacing();
		
		Ogre::Quaternion myOrient = objNode->getOrientation();
		Ogre::Vector3 worldOrientation = myOrient * facing;

		if((worldOrientation.dotProduct(TD)) < 0.98)
		{
			if(worldOrientation.crossProduct(TD).y > 0)
			{
				objNode->rotate(Ogre::Quaternion(Ogre::Degree(amt), Ogre::Vector3(0, 1, 0)),Ogre::Node::TS_WORLD);
				std::cout<<"rotating"<<std::endl;
			}
			else if(worldOrientation.crossProduct(TD).y < 0)
			{
				objNode->rotate(Ogre::Quaternion(Ogre::Degree(-amt), Ogre::Vector3(0, 1, 0)),Ogre::Node::TS_WORLD);
				std::cout<<"-rotating"<<std::endl;
			}
		}

	}

	return toLua;
}

int ssuge::GameObject::__lua_GameObject_register_class(lua_State * L)
{
	const struct luaL_Reg game_object_methods[] =
	{
		{"exec", __lua_GameObject_exec},
		{"new_class",_l_createGameObjectClass},
		{"new", _l_newGameObject},
		//{"__newindex", __lua_GameObject_newindex},
		{NULL,NULL}
	};

	luaL_newmetatable(L, "ssuge.GameObject");		// Creates a new metatable at the top of the stack
	lua_pushvalue(L, -1);							// Make a copy of the metatable
	lua_setfield(L, -2, "__index");	

	luaL_register(L, NULL, game_object_methods);
	lua_setfield(L, -2, "GameObject");				// Makes an entry in the ssuge table for the GameObject table.

	return 1;
}

int ssuge::GameObject::__lua_GameObject_exec(lua_State * L)
{
	if(lua_gettop(L) < 3)
		luaL_error(L, "__lua_GameObject_exec requires at least 3 paramaters on the lua stack! A LuaGameObject, component, and function.");

	LuaGameObject * gm = (LuaGameObject *) lua_touserdata(L, 1);
	const char * component = lua_tostring(L, 2);
	const char * operation = lua_tostring(L, 3);

	Message *data;
	if(lua_gettop(L) == 4)
		data = (Message *) lua_touserdata(L, 4);
	
	//the execute function will return a Message even if it's just a dummy message which is then pushed to the Lua stack
	if(strcmp(component, "self") == 0)
	{
		Message::__lua_push_Message(L, gm->mPtr->execute(operation, *data));
	}
	else if(strcmp(component, "mesh") == 0)
	{
		Message::__lua_push_Message(L, gm->mPtr->mComponents[Component::MESH]->execute(operation, *data));
	}
	else if(strcmp(component, "sound") == 0)
	{
		Message::__lua_push_Message(L, gm->mPtr->mComponents[Component::SOUND]->execute(operation, *data));
	}
	else if(strcmp(component, "camera") == 0)
	{
		Message::__lua_push_Message(L, gm->mPtr->mComponents[Component::CAMERA]->execute(operation, *data));
	}
	else if(strcmp(component, "physics") == 0)
	{
		Message::__lua_push_Message(L, gm->mPtr->mComponents[Component::PHYSICS]->execute(operation, *data));
	}

	return 1;
}

/// Lua Methods

void ssuge::GameObject::_l_gameObjectPush(lua_State * L, ssuge::GameObject * gO) 
{
	lua_pushlightuserdata (L, gO);
	lua_setglobal(L, "ssuge.GameObject");
	if(lua_pcall(L, 0, LUA_MULTRET, 0))
	{
		std::cerr << "Gameobject was invalid" << std::endl;
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	// One does not simply push everything

}

void ssuge::GameObject::_l_gameObjectPop(lua_State * L, ssuge::GameObject * gO) 
{
	lua_getglobal(L, "ssuge.GameObject");
	char * myObject = (char * )lua_touserdata(L, -1);
	lua_pop(L, 1);
}

int ssuge::GameObject::_l_createGameObjectClass(lua_State * L)
{
	//BASE_APPLICATION->logMessage("In GameObject::_l_createGameObjectClass", false);
	//SCRIPT_MANAGER->stack_dump(L);
	std::string name = lua_tostring(L,-1);
	luaL_newmetatable(L,name.c_str());

	// STACK = [..., ssuge.GameObject, string, metatable]

	// Step1. Create a new metatable (e.g. Robot) in ssuge.
	// Get the global "ssuge" table.
	// Create a new field in ssuge (lua_setfield) with the
	lua_getglobal(L, "ssuge");
	// STACK = [..., ssuge.GameObject, string, metatable, ssuge]
	lua_pushvalue(L, -2);
	// STACK = [..., ssuge.GameObject, string, metatable, ssuge, metatable]	
	lua_setfield(L, -2, name.c_str());
	lua_pop(L, 1);
	// STACK = [..., ssuge.GameObject, string, metatable]

	// Step2. Set the metatable of the new metatable (Robot) to ssuge.GameObject.
	//  (I don't think we'll need to do this, but just in case...) AND
	//        set the __index field of the new metatable (Robot)
	lua_pushvalue(L, -3); // makes a copy of ssuge.GameObject at the top of the stack
	// STACK = [..., ssuge.GameObject, string, metatable, ssuge.GameObject]
	lua_setmetatable(L, -2); // consumes ssuge.GameObject and sets it as the metatable of Robot 
	// STACK = [..., ssuge.GameObject, string, metatable]
	lua_pushvalue(L, -1); // create a copy of the metatable
	// STACK = [..., ssuge.GameObject, string, metatable, metatable]
	lua_setfield(L, -2, "__index");	// set the index of the metatable to itself
	// STACK = [..., ssuge.GameObject, string, metatable]
	lua_pushvalue(L, -1);
	// STACK = [..., ssuge.GameObject, string, metatable, metatable]
	lua_setfield(L, -2, "__newindex");
	// STACK = [..., ssuge.GameObject, string, metatable]


	//BASE_APPLICATION->logMessage("Exiting GameObject::_l_createGameObjectClass", false);

	return 1;
}

int ssuge::GameObject::_l_newGameObject(lua_State *L)
{
Message m;

	// StackPos1 = class (table) we're trying to instantiate.
	// StackPos2 = name (string) of object we're trying to create.
	// StackPos3 = tag (int) of object we're trying to create
	// StackPos4 = hasScript (int)
	// StackPos5 - n are extra arguments (later) to be passed onCreate
    if (lua_gettop(L) < 4)
	{
        luaL_error(L, "GameObject:new takes at least 4 arguments (1(self) = class, 2 = name, 3 = tag, 4 = hasScript)");
        return 0;
	}
    
	// Create a message object from stack values 4 - n
    if (lua_gettop(L) >= 5)
        m = Message::__lua_get_Message(L, 5, true);
	
    // Get the name and tag and pop them from the stack
	std::string name = std::string( lua_tostring(L, 2) );
	int tag = lua_tointeger(L, 3);
	bool hasScript = lua_tointeger(L, 4) != 0;
    lua_pop(L, 3);

	// Create the lua version of this game object
	LuaGameObject * lua_gobj = (LuaGameObject*)lua_newuserdata(L, sizeof(LuaGameObject));
	// Get table for our gameobject and set it as the metatable for our new userdata
	lua_pushvalue(L, 1);
	lua_setmetatable(L, -2);

	// Create a global variable in L (with the name of our game object)
    lua_pushvalue(L, -1); // makes a copy for set_global to consume
	lua_setglobal(L, name.c_str()); // creates a new global object with the same name as our gameobject

	// Tell the base application to create this game object
	GameObject * ssuge_gobj = BASE_APPLICATION->createGameObject(tag, name);
	lua_gobj->mPtr = ssuge_gobj;
	strcpy_s(lua_gobj->mClassName, 32, name.c_str());
    ssuge_gobj->mHasScript = hasScript;

    // Inject an 'onCreate' message to the new game object.
	if(ssuge_gobj->mHasScript)
		ssuge_gobj->injectScriptEvent("onCreate", m);

	return 1;
}


int ssuge::GameObject::__lua_GameObject_newindex(lua_State * L)
{
	// Stack = [table (userdata), key, value]
	const char * field_name = luaL_checkstring(L, -2);
	lua_setfield(L, -3, field_name);

	lua_pop(L, 2);

	return 0;
}

void ssuge::GameObject::enableComponent(Component::ComponentType t, bool b)
{
	if (mComponents[t])
	{
			mComponents[t]->setEnable(b);
	}
}