#include <stdafx.h>
#include <BaseApplication.h>
#include <Utility.h>
#include <Message.h>
#include <Exception.h>
#include "../include/OgreOculus.h"


#define YEI_SENSOR_SUPPORT 0
#define OCCULUS_SUPPORT 1


/// Template-specialization of the msSingleton for BaseApplications.  Note:
/// We'll have to do this for every ssuge::Singleton-derived class
ssuge::BaseApplication* ssuge::Singleton<ssuge::BaseApplication>::msSingleton = NULL;


ssuge::BaseApplication::BaseApplication() : mOgreWindow(NULL), mRoot(NULL), mSceneManager(NULL), mNumRecentLogMessages(30),
	  mSDLWindow(NULL), mSoundManager(NULL), mInputManager(NULL), mOverlaySystem(NULL), mScriptManager(NULL), mPhysicsManager(NULL),
	  Singleton(), mActiveCamera(NULL)
{
	// Intentionally left blank -- the "hard" work is all done by the constructor-initializers above
}



ssuge::BaseApplication::~BaseApplication()
{
	// Destroy the recent log messages
	mRecentLogMessages.clear();

	// Destroy all the game objects
	std::map<std::string, GameObject*>::iterator iter;
	for (iter = mObjects.begin(); iter != mObjects.end(); ++iter)
	{
		delete iter->second;
		iter->second = NULL;
	}
	mObjects.clear();

	// Destroy our managers
	if(mSoundManager)
		delete mSoundManager;
	if(mInputManager)
		delete mInputManager;
	if(mScriptManager)
		delete mScriptManager;
	if(mPhysicsManager)
		delete mPhysicsManager;
	if (mOverlaySystem)
		delete mOverlaySystem;

	// Destroy the ogre root (which will destroy all ogre objects)
	if(mRoot)
		delete mRoot;
	
	// Shut down SDL (which will destroy the SDL window)

#if OCCULUS_SUPPORT
	// Clean up the Oculus
	oculus.shutDownOculus();
#endif
	SDL_Quit();
}



int ssuge::BaseApplication::initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO))
		return 1;
	if (initializeRoot())
		return 2;
	if(initializeLog())
		return 3;
	if (initializeWindow())
		return 4;
	if (initializeSceneManager())
		return 5;
	if (initializeResources())
		return 7;
	if (initializeViewports())
		return 6;
	if(initializeGUI())
		return 8;
	if(initializePhysicsManager())
		return 9;
	if(initializeSoundManager())
		return 10;
	if(initializeInputManager())
		return 11;
	if(initializeScriptManager())
		return 12;
	if (initializeScene())
		return 13;

	mLastTick = SDL_GetTicks();

	return 0;
}



int ssuge::BaseApplication::initializeRoot()
{
#ifdef _DEBUG
	mRoot = new Ogre::Root("plugins_d.cfg");
#else
	mRoot = new Ogre::Root();
#endif

	if (!mRoot->showConfigDialog())
		return 1;

	return 0;
}



int ssuge::BaseApplication::initializeLog()
{
	Ogre::LogManager::getSingleton().createLog("mLog.txt", false, true, false);
	
	return 0;
}



int ssuge::BaseApplication::initializeWindow()
{
HWND hwnd;

// Create the window

#if OCCULUS_SUPPORT
	mOgreWindow = mRoot->initialise(true, "Oculus");
	if (mOgreWindow == NULL)
		return 1;


#else
	mOgreWindow = mRoot->initialise(true);
	if (mOgreWindow == NULL)
		return 1;
#endif
	mOgreWindow->getCustomAttribute("WINDOW", &hwnd);
	mSDLWindow = SDL_CreateWindowFrom((void*)hwnd);

	return mOgreWindow == NULL || mSDLWindow == NULL;
}



int ssuge::BaseApplication::initializeSceneManager()
{
	mSceneManager = mRoot->createSceneManager("OctreeSceneManager", "MyOctreeManager");
	return mSceneManager == NULL;
}



int ssuge::BaseApplication::initializeResources()
{
	// NOTE: we should probably have a script file which contains these...
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\bootstrap.zip", "Zip", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\bootstrap", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\sounds", "FileSystem", "Sound");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\overlays", "FileSystem", "Overlays");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\Fonts", "FileSystem", "Fonts");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\scripts", "FileSystem", "Scripts");
	// Needed for Shaders
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\materials\\programs\\Cg", "FileSystem", "Shaders");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\materials\\programs\\GLSL", "FileSystem", "Shaders");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\materials\\scripts", "FileSystem", "Materials");
	// Commented out to prevent breakage
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\materials\\scripts", "FileSystem", "Particles");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\media\\materials\\textures", "FileSystem", "Textures");

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	return 0;
}




int ssuge::BaseApplication::initializeViewports(unsigned int num_viewports)
{
#if OCCULUS_SUPPORT
	// init dummy ogre camera.  This will likely be replaced by a real game
	// camera attached to a game object.
#else
	Ogre::Camera * dummy = mSceneManager->createCamera("_dummy_");
	dummy->setPosition(0,30,100);
	dummy->lookAt(0,0,-1);
	dummy->setNearClipDistance(1.0f);
	Ogre::Viewport * vport = mOgreWindow->addViewport(dummy);
	vport->setBackgroundColour(Ogre::ColourValue(0.3f,0.3f,0.3f));
#endif


	return 0;
}



int ssuge::BaseApplication::initializeGUI()
{
	//Initialize the Overlay System
	mOverlaySystem = new Ogre::OverlaySystem();
	mSceneManager->addRenderQueueListener(mOverlaySystem);

	// Convenience Variables for the scene
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	
	// create font used by the debug console
	Ogre::FontPtr& mFont = Ogre::FontManager::getSingleton().create("_debug_font", "Fonts");
	mFont->setType(Ogre::FT_TRUETYPE);
	mFont->setSource(Ogre::String("micross.ttf"));
	mFont->setTrueTypeSize(24);
	mFont->setTrueTypeResolution(96);
	mFont->addCodePointRange(Ogre::Font::CodePointRange(33, 255));
	mFont->load();

	// Create the debug / console overlay
	Ogre::Overlay* overlay = overlayManager.create( "_debug_console" );
	// ...recent log messages area
	Ogre::BorderPanelOverlayElement* panel = static_cast<Ogre::BorderPanelOverlayElement*>( overlayManager.createOverlayElement( "BorderPanel", "_debug_console/log" ) );
	panel->setMetricsMode(Ogre::GMM_RELATIVE);
    panel->setPosition( 0.0f, 0.0f );
    panel->setDimensions( 0.5f, 1.0f );
	panel->setBorderMaterialName("_debug_console_border");
	panel->setMaterialName("_debug_console_fill");
	overlay->add2D(panel);
	// ...recent log message text boxes
	Ogre::TextAreaOverlayElement* tpanel;
	float inter_spacing = 0.01f, height;
	height = (1.0f - inter_spacing * (mNumRecentLogMessages + 1)) / mNumRecentLogMessages;
	for(int i = 0; i < (int)mNumRecentLogMessages; i++)
	{
		tpanel = static_cast<Ogre::TextAreaOverlayElement*>(overlayManager.createOverlayElement("TextArea", "_debug_console/log/msg" + std::to_string(i)));
		tpanel->setMetricsMode(Ogre::GMM_RELATIVE);
		tpanel->setPosition(inter_spacing, inter_spacing * (i + 1) + height * i);
		tpanel->setDimensions(0.9f, height);
		tpanel->setCaption("");
		tpanel->setCharHeight(height);
		tpanel->setFontName("_debug_font");
		tpanel->setColourBottom(Ogre::ColourValue(0.3f, 0.5f, 0.3f));
		tpanel->setColourTop(Ogre::ColourValue(0.5f, 0.7f, 0.5f));
		panel->addChild(tpanel);
	}
	// ...The fixed "slots" in the debug overlay
	panel = static_cast<Ogre::BorderPanelOverlayElement*>( overlayManager.createOverlayElement( "BorderPanel", "_debug_console/stats" ) );
	panel->setMetricsMode(Ogre::GMM_RELATIVE);
    panel->setPosition( 0.55f, 0.0f );
    panel->setDimensions( 0.45f, 1.0f );
	panel->setBorderMaterialName("_debug_console_border");
	panel->setMaterialName("_debug_console_fill");
	overlay->add2D(panel);
	for(int i = 0; i < (int)mNumRecentLogMessages; i++)
	{
		tpanel = static_cast<Ogre::TextAreaOverlayElement*>(overlayManager.createOverlayElement("TextArea", "_debug_console/stats/slot" + std::to_string(i)));
		tpanel->setMetricsMode(Ogre::GMM_RELATIVE);
		tpanel->setPosition(inter_spacing, inter_spacing * (i + 1) + height * i);
		tpanel->setDimensions(0.9f, height);
		tpanel->setCaption("");
		tpanel->setCharHeight(height);
		tpanel->setFontName("_debug_font");
		tpanel->setColourBottom(Ogre::ColourValue(0.5f, 0.3f, 0.3f));
		tpanel->setColourTop(Ogre::ColourValue(0.7f, 0.5f, 0.5f));
		panel->addChild(tpanel);
	}
	// ... fps text area
	tpanel = static_cast<Ogre::TextAreaOverlayElement*>(overlayManager.createOverlayElement("TextArea", "_debug_console/stats/fps"));
	tpanel->setMetricsMode(Ogre::GMM_RELATIVE);
	tpanel->setPosition(0.05f, 0.05f);
	tpanel->setDimensions(0.9f, 0.9f);
	tpanel->setCaption("Hello, World!");
	tpanel->setCharHeight(16);
	tpanel->setFontName("_debug_font");
	tpanel->setColourBottom(Ogre::ColourValue(0.3f, 0.5f, 0.3f));
	tpanel->setColourTop(Ogre::ColourValue(0.5f, 0.7f, 0.5f));
	panel->addChild(tpanel);

	/*tpanel = static_cast<Ogre::TextAreaOverlayElement*>(overlayManager.createOverlayElement("TextArea", "_debug_console/stats/tri"));
	tpanel->setMetricsMode(Ogre::GMM_RELATIVE);
	tpanel->setPosition(0.05f, 0.1f);
	tpanel->setDimensions(0.9f, 0.9f);
	tpanel->setCaption("Hello, World!");
	tpanel->setCharHeight(16);
	tpanel->setFontName("_debug_font");
	tpanel->setColourBottom(Ogre::ColourValue(0.3f, 0.5f, 0.3f));
	tpanel->setColourTop(Ogre::ColourValue(0.5f, 0.7f, 0.5f));
	panel->addChild(tpanel);*/
	
	return 0;
}


int ssuge::BaseApplication::createGUI_Img(std::string name, float posx, float posy, float scax, float scay, std::string material)
{
	// Convenience Variables for the scene
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

	Ogre::Overlay* overlay = overlayManager.create(name);
	Ogre::PanelOverlayElement* panel = static_cast<Ogre::PanelOverlayElement*>( overlayManager.createOverlayElement( "Panel", name + "/log" ) );
	panel->setMetricsMode(Ogre::GMM_RELATIVE);
    panel->setPosition( posx, posy );
    panel->setDimensions( scax, scay );
	panel->setMaterialName(material);
	overlay->add2D(panel);
	overlay->show();
	return 0;
}


int ssuge::BaseApplication::initializePhysicsManager()
{
	mPhysicsManager = new ssuge::PhysicsManager();
	return mPhysicsManager == NULL;
}



int ssuge::BaseApplication::initializeSoundManager()
{
	mSoundManager = new SoundManager();
	return mSoundManager == NULL;
}



int ssuge::BaseApplication::initializeInputManager()
{
	mInputManager = new InputManager();
	if (!mInputManager)
		return 1;
	// We might want to move these to a setDefaultMappings method of the InputManager.
	mInputManager->mapKeyAction(SDLK_6, ssuge::InputManager::FIRST_PERSON);
	mInputManager->mapKeyAction(SDLK_7, ssuge::InputManager::THIRD_PERSON);
	mInputManager->mapKeyAction(SDLK_s,ssuge::InputManager::ACTION_DOWN);
	mInputManager->mapKeyAction(SDLK_w, ssuge::InputManager::ACTION_UP);
	mInputManager->mapKeyAction(SDLK_a, ssuge::InputManager::ACTION_LEFT);
	mInputManager->mapKeyAction(SDLK_d, ssuge::InputManager::ACTION_RIGHT);
	mInputManager->mapKeyAction(SDLK_SPACE, ssuge::InputManager::ACTION_JUMP);
	mInputManager->mapKeyAction(SDLK_LCTRL, ssuge::InputManager::ACTION_ATTACK);
	mInputManager->mapKeyAction(SDLK_RCTRL, ssuge::InputManager::ACTION_CROUCH);
	mInputManager->mapKeyAction(SDLK_RETURN, ssuge::InputManager::ACTION_PAUSE);
	mInputManager->mapKeyAction(SDLK_BACKQUOTE, ssuge::InputManager::DEBUG);
	mInputManager->mapKeyAction(SDLK_3, ssuge::InputManager::GLOW);
	
	return 0;
}



int ssuge::BaseApplication::initializeScriptManager()
{
	mScriptManager = new ScriptManager();
	return mScriptManager == NULL;
}



int ssuge::BaseApplication::initializeScene()
{
	// YOST

	if(tss_getComPorts(&mComport,1,0,TSS_FIND_DNG))
	{
		mD_device =tss_createTSDeviceStr(mComport.com_port, TSS_TIMESTAMP_SENSOR);
		if( mD_device == TSS_NO_DEVICE_ID)
		{
			printf("Failed to create a sensor on %s\n",mComport.com_port);
			
		}
		tss_getSensorFromDongle(mD_device,0,&mDevice);
		if( mDevice == TSS_NO_DEVICE_ID)
		{
			printf("Failed to create a wireless sensor on\n");
			
		}
	}
	else
	{
		printf("No sensors found\n");
		
	}

	// YOST

	// [Move to script] Create a directional light
	// Adding in the Oculus
#if OCCULUS_SUPPORT
	oculus.setupOculus();
	oculus.setupOgre(mSceneManager, mOgreWindow);
#endif
	Ogre::Light * L = mSceneManager->createLight();
	L->setType(Ogre::Light::LT_DIRECTIONAL);
	L->setDirection(0,-1,0);
	L->setDiffuseColour(Ogre::ColourValue(1,1,1,1));
	
	// [Move to script] Create and set up the NINJA
	//GameObject *Ninja = createGameObject(1, "Ninja");
	//Ninja->setScale(Ogre::Vector3(0.1f, 0.1f, 0.1f));
	//Ninja->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
	// .... camera
	//CameraComponent *ninjaCam = Ninja->createCameraComponent();
	//ninjaCam->setFirstPerson(true);
	//ninjaCam->setPosition(Ogre::Vector3(1.2f,16.6f,-0.8f));
	//ninjaCam->setModelOrietation(Ogre::Vector3::UNIT_Z, Ogre::Vector3::NEGATIVE_UNIT_X);
	//ninjaCam->setRotationRates(true,180,180);
	//ninjaCam->setNear(1.0f);

	// ... mesh component
	//MeshComponent *MeshNinja = Ninja->createMeshComponent();
	//MeshNinja->loadMesh("ninja.mesh");
	//MeshNinja->queueAnimation("Idle1",false,0.0);

	// ... Physics Component
	// (JW) This is conflicting with the camera controller code, so I temporarily commented
	// it out.  Long term: if we attach a physics component to the player (which makes sense), we'll
	// probably need to apply torques and linear accelerations to the object to get it to move (since 
	// bullet is in control).
	//PhysicsComponent *ninjaPhysics = Ninja->createPhysicsComponent();
	//Ninja->getSceneNode()->_updateBounds();
	//Ogre::Vector3 NinjaOff = Ninja->getSceneNode()->_getDerivedPosition();
	//std::cout << "Ninja HalfSize: " << NinjaOff << std::endl;
	//ninjaPhysics->loadBox(Ninja->getPosition(), Ogre::Vector3(4.f,6.f, 4.f), 1.f);
	// ... register the ninja as an input listener
	//mInputManager->addListener(Ninja);
	
	// [Delete] Create and set up the MONKEY
	/*GameObject *Monkey = createGameObject(1, "Monkey");
	Monkey->setPosition(Ogre::Vector3(0.0f, 5.0f, 0.0f));
	Monkey->createAnimation(5.0f);
	Monkey->addAnimationKeyframe(0.0f, Ogre::Vector3(0,5, -15));
	Monkey->addAnimationKeyframe(2.5f, Ogre::Vector3(0,5, 15));
	Monkey->addAnimationKeyframe(5.0f, Ogre::Vector3(0,5, -15));
	// ... mesh component
	MeshComponent *MeshMonkey = Monkey->createMeshComponent();
	MeshMonkey->loadMesh("monkey.mesh");
	// ... pyhsics component
	PhysicsComponent *monkeyPhysics = Monkey->createPhysicsComponent();
	monkeyPhysics->loadSphere(Monkey->getPosition(), 1.f, 0.f);*/
	
	//monkeyPhysics->addTorque(Ogre::Vector3(0,40,0));
	// [Create something like this in script to illustrate physics]
	/*
	srand(time(NULL));
	///Creating 20 random spheres for testing of the physics engine.
	for(int i = 0; i < 1; i++)
	{
		//Random portion for the sizes of the sphere to be loaded =D
		float scale = (rand() % 10) + 1.0;
		float posx = rand() % 10 + 1.0;
		float posy = rand() % 10 + 1.0;
		float posz = rand() % 10 + 1.0;
		float mass = (4.0/3.0)*3.14*scale*scale;
		if(i%2 == 1)
		{
			posx*= -1;
			posz*= -1;
		}
		//The actual loading of the sphere.
		std::string SphereName;
		SphereName += "Sphere_" + std::to_string(i);
		//Ogre::StringConverter(i, SphereName);
		GameObject *Sphere = createGameObject(1, SphereName);
		Sphere->setScale(Ogre::Vector3(scale, scale, scale));
		Sphere->setPosition(Ogre::Vector3(posx, posy, posz));
		MeshComponent *SphereMesh = Sphere->createMeshComponent();
		SphereMesh->loadMesh("Earth.mesh");
		PhysicsComponent *SpherePhysics = Sphere->createPhysicsComponent();
		SpherePhysics->loadSphere(Sphere->getPosition(), scale, mass);
		
	}

	*/

	// [C
	///Loading of a huge box for the testing of the physics engine.
	GameObject *Box = createGameObject(3, "Box");
	Box->setScale(Ogre::Vector3(50.0f, 50.0f, 50.0f));
	Box->setPosition(Ogre::Vector3(0.0f, -1.0f,  0.0f));
	MeshComponent *BoxMesh = Box->createMeshComponent();
	BoxMesh->loadMesh("Crate.mesh");
	PhysicsComponent *BoxPhysics = Box->createPhysicsComponent();
	BoxPhysics->loadBox(Box->getPosition(), Ogre::Vector3(50.0f,50.0f,50.0f), 0.f);

	
	// ... sound component
	//SoundComponent *SoundMonkey = Monkey->createSoundComponent();
	//SoundMonkey->loadSound("train.wav", true, true);
	/*
	// Create and set up the STICK (figure?)
	GameObject *Stick = createGameObject(1, "Stick");
	Stick->setPosition(Ogre::Vector3(0,2,-5));
	// ... mesh component
	MeshComponent *MeshStick = Stick->createMeshComponent();
	MeshStick->loadMesh("sf2.mesh");
	Ogre::Entity *e =  MeshStick->getEntity();
	Ogre::AnimationState *as = e->getAnimationState("ani1");
	as->setEnabled(true);
	as->setLoop(true);
	*/

	// Camera 1 (Third Person Cam)
	GameObject *mainCam = createGameObject(2, "mainCam");
	CameraComponent *camComponent = mainCam->createCameraComponent();
	camComponent->setOrientation(Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::UNIT_Y) * Ogre::Quaternion(Ogre::Degree(-20), Ogre::Vector3::UNIT_X));
	camComponent->setPosition(Ogre::Vector3(0,10,-30));
	camComponent->setNear(1.0f);
	
	// Create a gameObject to serve as a temporary listener for the camera.
	// Eventually, this will probably be the same game object the camera is attached to.
	GameObject *Listener = createGameObject(2, "Listener");
	// Note: we need a better way of doing this.  Here's the problem
	Listener->setPosition(Ogre::Vector3(0, 0, -10));
	mSoundManager->setListener(Listener);

	/// Calls the Compositor we are attempting to load
#if OCCULUS_SUPPORT
	Ogre::Viewport * mViewPort = mOgreWindow->getViewport(0);
	Ogre::CompositorManager::getSingleton().addCompositor(mViewPort, "Oculus");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewPort, "Oculus", true);
#endif
	/// Set up glow on Ninja
	//findGameObject("Ninja")->getMeshComponent()->getEntity()->getSubEntity(0)->setMaterialName("glowColor");
	/// Sky Box
	mSceneManager->setSkyBox(true, "Examples/SpaceSkyBox", 150, false);	
	/// Set up the Particle System
	/*
	Ogre::ParticleSystem* purpleParticle = mSceneManager->createParticleSystem("myEmmiter", "PurpleFountain");
	// We may need to use 'addTemplate' instead?
	Ogre::SceneNode* particleNode = mSceneManager->getRootSceneNode()->createChildSceneNode("My_Particle");
	particleNode->attachObject(purpleParticle);
	*/
	return 0;
}



int ssuge::BaseApplication::addGameObject(std::string title, ssuge::GameObject * obj)
{
std::map<std::string, GameObject *>::iterator iter;
	iter = mObjects.find(title);
	if (iter == mObjects.end())
		mObjects[title] = obj;
	else
		throw ssuge::Exception(__FILE__, __LINE__, "An object with name '" + title + "' already exists!");

	return 1;
}



ssuge::GameObject * ssuge::BaseApplication::createGameObject(unsigned int tag, std::string title)
{
GameObject * go;

	go = new GameObject(tag, title);
	addGameObject(title, go);
	return go;
}



int ssuge::BaseApplication::removeGameObject(std::string title)
{
std::map<std::string, GameObject *>::iterator iter;

	iter = mObjects.find(title);
	if(iter != mObjects.end())
	{
		delete iter->second;
		mObjects.erase(iter);
		return 1;
	}
	return 0;
}

int ssuge::BaseApplication::removeGameObjectByTag(int tag)
{
	std::map<std::string, GameObject*>::iterator iter;
	for (iter = mObjects.begin(); iter != mObjects.end(); )
	{
		if (iter->second->get_tag() == tag)
		{
			delete iter->second;
			iter = mObjects.erase(iter);
		}
		else
			++iter;
	}
	return 0;
}



int ssuge::BaseApplication::findObjectsByTag(std::vector<GameObject*>& v, int tag)
{
	std::map<std::string, GameObject*>::iterator iter;
	for (iter = mObjects.begin(); iter != mObjects.end(); ++iter)
	{
		if (iter->second->get_tag() == tag)
		{
			v.push_back(iter->second);
		}
	}
	return 0;
}



ssuge::GameObject * ssuge::BaseApplication::findGameObject(std::string title)
{
	std::map<std::string, GameObject *>::iterator iter;
	iter = mObjects.find(title);
	if (iter == mObjects.end())
		return NULL;
	else
		return iter->second;
}



int ssuge::BaseApplication::update(float dT)
{

/*
	if(tss_getComPorts(&mComport,1,0,TSS_FIND_DNG)){
		mD_device =tss_createTSDeviceStr(mComport.com_port, TSS_TIMESTAMP_SENSOR);
		if( mD_device == TSS_NO_DEVICE_ID){
			printf("Failed to create a sensor on %s\n",mComport.com_port);
			return 1;
		}
		tss_getSensorFromDongle(mD_device,0,&mDevice);
		if( mDevice == TSS_NO_DEVICE_ID){
			printf("Failed to create a wireless sensor on\n");
			return 1;
		}

	}
	else{
		printf("No sensors found\n");
		//return 1;
	}

	float gyro[3];
	float accel[3];
	float compass[3];
	mTss_error= tss_getAllCorrectedComponentSensorData(mDevice, gyro, accel, compass, NULL);
	if (!mTss_error)
		printf("Gyro:  %f, %f, %f\n", gyro[0],gyro[1],gyro[2]);
	else
		printf("TSS_Error: %s\n", TSS_Error_String[mTss_error]);*/
	//=================YOST STUFF==========================


	Ogre::Overlay* temp = Ogre::OverlayManager::getSingleton().getByName("_debug_console");

	temp->hide();

	unsigned int tc = this->mOgreWindow->getTriangleCount();
	// Update all the game objects.
	for(std::map<std::string, GameObject *>::iterator it = mObjects.begin(); it != mObjects.end(); ++it )
	{
		std::string key = it->first;
		GameObject *obj = it->second;
		obj->update(dT);
	}

	// (temporary) test code.  Eventually this will be done in script.
	/*if(mInputManager->actionIsActive(ssuge::InputManager::ACTION_CROUCH))
	{
		GameObject * ninja = this->findGameObject("Ninja");
		ninja->getMeshComponent()->queueAnimation("Crouch", false, 0.5f, true);
	}*/

	// Update the gui manager texts
	setDebugString(0, "FPS: " + Ogre::StringConverter::toString(mOgreWindow->getAverageFPS()));
	setDebugString(1, "Vert: " + Ogre::StringConverter::toString(INPUT_MANAGER->getHorizAxis(InputManager::D_ALL)));
	setDebugString(2, "Horz: " + Ogre::StringConverter::toString(INPUT_MANAGER->getVertAxis(InputManager::D_ALL)));
	setDebugString(3, "RVert: " + Ogre::StringConverter::toString(INPUT_MANAGER->getRVertAxis(InputManager::D_ALL)));
	setDebugString(4, "RHorz: " + Ogre::StringConverter::toString(INPUT_MANAGER->getRHorizAxis(InputManager::D_ALL)));
	setDebugString(5, "TRI: " + Ogre::StringConverter::toString(tc));
	

/*Ogre::Entity *e = tmp->getEntity();
Ogre::AnimationState *walk = e->getAnimationState("Walk");
Ogre::AnimationState *crouch = e->getAnimationState("Crouch");
Ogre::AnimationState *jump = e->getAnimationState("HighJump");
Ogre::AnimationState *attack = e->getAnimationState("Attack1");
Ogre::Skeleton *NinjaSkel = e->getSkeleton();


NinjaSkel->setBlendMode(Ogre::SkeletonAnimationBlendMode::ANIMBLEND_CUMULATIVE);

MeshComponent *stick = findGameObject("Stick")->getMeshComponent();
Ogre::Entity *en = stick->getEntity();
Ogre::AnimationState * anim = en->getAnimationState("ani1");


if(temp)
	temp->hide();
	
	
	mSoundManager->updateListener();

	// Inputs

	if(mInputManager->actionIsActive(ssuge::InputManager::ACTION_LEFT)) anim->setEnabled(true);
	else anim->setEnabled(false);

	if(mInputManager->actionIsActive(ssuge::InputManager::ACTION_UP)) walk->setEnabled(true);
	else
	{
		
		walk->setEnabled(false);
		walk->setTimePosition(0);
	}
	if(mInputManager->actionIsActive(ssuge::InputManager::ACTION_DOWN))
	{
		crouch->setLength(.4);
		crouch->setLoop(false);
		crouch->setEnabled(true);
	}
	else if(crouch->hasEnded())
	{
		crouch->setEnabled(false);
		crouch->setTimePosition(0);
	}
	if(mInputManager->actionIsActive(ssuge::InputManager::ACTION_JUMP))
	{
		jump->setEnabled(true);
		jump->setLoop(false);
	}
	else if(jump->hasEnded())
	{
		
		jump->setEnabled(false);
		jump->setTimePosition(0);
	}


	else
	{
		as2->setEnabled(false);
		as1->setEnabled(true);

		

	}
	else if(attack->hasEnded())
	{
		attack->setEnabled(false);
		attack->setTimePosition(0);
	}

	*/

	


	// [Temporary] Toggle between camera modes
	if(mInputManager->actionIsActive(ssuge::InputManager::FIRST_PERSON))
	{
		std::cout << " FP " << std::endl;
		//setActiveCamera(findGameObject("Ninja"));
	}
	if(mInputManager->actionIsActive(ssuge::InputManager::THIRD_PERSON))
	{
		std::cout << " TP " << std::endl;
		setActiveCamera(findGameObject("mainCam"));
	}
	/*if(mInputManager->actionIsActive(ssuge::InputManager::GLOW))
	{
		//std::cout << " Glowing " << std::endl;
		findGameObject("Ninja")->getMeshComponent()->getEntity()->getSubEntity(0)->setMaterialName("glowColor");
	}*/

	//if(mInputManager->actionIsActive(ssuge::InputManager::ACTION_ATTACK))
		//findGameObject("Ninja")->getMeshComponent()->queueAnimation("Attack1",false,1.5);
	
	if(mInputManager->actionIsActive(ssuge::InputManager::ACTION_JUMP))
		findGameObject("Ninja")->getMeshComponent()->queueAnimation("Backflip",false,1.5);
	// Toggle between showing the debug overlay and not showing it
	Ogre::OverlayElement* over = Ogre::OverlayManager::getSingleton().getOverlayElement("_debug_console/stats/fps");
	
	//temp->hide();
	if(mInputManager->actionIsActive(ssuge::InputManager::DEBUG))
	{
		over->setCaption(Ogre::StringConverter::toString(mOgreWindow->getAverageFPS()));

		for(int i = 0; i < (int)mRecentLogMessages.size(); i++)
		{
			over = Ogre::OverlayManager::getSingleton().getOverlayElement("_debug_console/log/msg" + std::to_string(i));
			over->setCaption(mRecentLogMessages[i]);
		}

		temp->show();
	}
#if OCCULUS_SUPPORT
	oculus.update();
#endif

	//Testing the applyTorqueImpulse.
	


	return 0;
}

void ssuge::BaseApplication::run()
{
float dt;
bool done = false;

	while(!done)
	{
		// Update dT
		dt = (SDL_GetTicks() - mLastTick) / 1000.0f;
		mLastTick = SDL_GetTicks();
		if(dt == 0.0)
		{
			std::cout << "dt was 0\n";
			dt = 1.f/60.f;
		}
		// Update the input manager
		if (mInputManager->update(dt))
			done = true;
		// Update the sound manager
		if (mSoundManager->update(dt))
			done = true;
		//std::cout << dt;
		if (mPhysicsManager->updateWorld(dt))
			done = true;

		// (temporary) Quit the application when escape is pressed
		if (INPUT_MANAGER->getKeystate(SDLK_ESCAPE))
			done = true;

		// Update
		update(dt);

		/// Draws a frame from ogre
		mRoot->renderOneFrame();
	}
}



int ssuge::BaseApplication::setActiveCamera(GameObject* cam)
{
	std::cout << "active camera called" << std::endl;
	if (cam == NULL || cam->getCameraComponent() == NULL || cam->getCameraComponent()->getEnable() == false)
		return 1;
	else
	{	
#if not OCCULUS_SUPPORT
		std::cout << "setting new camera settings" << std::endl;
		Ogre::Viewport * vport = mOgreWindow->getViewport(0);
		vport->setCamera(cam->getCameraComponent()->getCamera());
		mActiveCamera = cam;
#endif
	}
	return 0;
}



void ssuge::BaseApplication::logMessage(std::string toPrint, bool toDebug)
{
	Ogre::LogManager::getSingleton().getLog("mLog.txt")->logMessage(toPrint);
	if(toDebug)
		logToConsole(toPrint);
}

void ssuge::BaseApplication::logToConsole(std::string print)
{
	if (mRecentLogMessages.size() == mNumRecentLogMessages)
		mRecentLogMessages.erase(mRecentLogMessages.cbegin());
	mRecentLogMessages.push_back(print);
}

void ssuge::BaseApplication::setDebugString(unsigned int slot, std::string value)
{
	if (slot < mNumRecentLogMessages)
	{
		Ogre::OverlayElement * elem = Ogre::OverlayManager::getSingleton().getOverlayElement("_debug_console/stats/slot" + Ogre::StringConverter::toString(slot));
		if (elem)
			elem->setCaption(value);
	}
}



void ssuge::BaseApplication::toggleDebugConsole()
{
	Ogre::Overlay* overlay = Ogre::OverlayManager::getSingleton().getByName("_debug_console" );
	if (overlay->isVisible())
		overlay->hide();
	else
		overlay->show();
}


void ssuge::BaseApplication::onAction(InputManager::InputActions action, bool actionState)
{
	if (action == InputManager::ACTION_PAUSE && actionState)
		toggleDebugConsole();
	if (action == InputManager::ACTION_ATTACK && actionState)
	{
		// Toggle between 1 and 2 viewport mode.
		if (mOgreWindow->getNumViewports() == 1)
			initializeViewports(2);
		else
			initializeViewports(1);
	}
}
