#ifndef _BASE_APPLICATION_H_
#define _BASE_APPLICATION_H_

#include <stdafx.h>
#include <Singleton.h>
#include <GameObject.h>
#include <SoundManager.h>
#include <InputManager.h>
#include <ScriptManager.h>
#include <PhysicsManager.h>
#include <OgreOculus.h>

/// A convenience shortcut to access the BaseApplication singleton
#define BASE_APPLICATION static_cast<BaseApplication*>(ssuge::BaseApplication::getSingletonPtr())

/// ssuge is our super-awesome game engine.  This namespace
/// contains a BaseApplication, GameObject, and various
/// Components (which can be added to a GameObject).
namespace ssuge
{
	/// The BaseApplication houses all managers (InputManager, ScriptManager, etc.)
	/// and also manages the master dictionary of all game objects.
	class BaseApplication : public Singleton<BaseApplication>
	{
	/***** CONSTRUCTOR / DESTRUCTOR *****/
	public:
		/// The constructor
		BaseApplication();

		/// The destructor
		virtual ~BaseApplication();

	/***** INITIALIZATION methods *****/
	public:
		/// Calls the other initialize methods.  Fully initializes the application
		int initialize();

	protected:
		/// Creates the ogre root, loads plugins, and creates the ogre window (to which
		/// the SDL window is a chile)
		virtual int initializeRoot();

		/// Initialize Log File support
		virtual int initializeLog();

		/// Creates the render window
		virtual int initializeWindow();

		/// Creates the ogre scene manager (using a basic octree manager).  Override this to create
		/// other types of scene manager
		virtual int initializeSceneManager();

		/// Loads the resource locations and identifies all ogre files (e.g. mesh)
		virtual int initializeResources();

		/// Creates a camera and viewport on the ogre window
		virtual int initializeViewports(unsigned int num_viewports = 1);

		/// Initialize the Overlay System (and creates the debug / console overlay)
		int initializeGUI();

		/// Initialize the Overlay System (and creates the debug / console overlay)
		int createGUI_Img(std::string name, float posx = 0.0f, float posy = 0.0f, float scax = 0.0f, float scay = 0.0f, std::string material = "Examples/OgreLogo");

		///Initializes physics manager
		virtual int initializePhysicsManager();

		/// initialize the sound manager
		int initializeSoundManager();

		/// initialize the input manager
		int initializeInputManager();

		/// Initialize the script manager
		int initializeScriptManager();

		/// Creates the 3d scene
		virtual int initializeScene();		


	/***** GETTER / SETTER methods *****/
	public:
		/// Gets mSceneManager
		Ogre::SceneManager * getSceneManager() { return mSceneManager; }

		/// Returns the time (in milliseconds since the application was started)
		unsigned int getTime() { return mLastTick; }

		/// GameObject map
		std::map<std::string, GameObject *> mObjects;

		/// Returns a given viewport (0 is the main or left, 1 is right)
		Ogre::Viewport * getViewport(unsigned int index);


	/***** GAME OBJECT-related methods *****/
	public:
		/// Adds an existing GameObject to the map.  Note: by calling this, BaseApplication
		/// assumes responsibility for deleting the memory allocated by the new operator.
		int addGameObject(std::string title, ssuge::GameObject * obj);

		/// Creates a game object and adds it to the map
		GameObject * createGameObject(unsigned int tag, std::string title);

		/// Removes a GameObjects from the map (if found)
		int removeGameObject(std::string title); 

		/// Removes GameObjects from the map by tag (if found)
		int removeGameObjectByTag(int tag); 

		/// Finds GameObjects from the map by tag and adds them to a vector passed to the function (if found)
		int findObjectsByTag(std::vector<GameObject*>& v, int tag);

		/// Returns the address of the GameObject with the given name, or NULL if it doesn't exist.
		ssuge::GameObject * findGameObject(std::string title);

		// Creates and Oculus
		Oculus oculus;

	/***** CORE Methods *****/
	protected:
		/// Called at the beginning of each game loop iteration (before we draw)
		virtual int update(float dT);

	public:
		/// Starts the application
		void run();

	/***** MISC Methods *****/
	public:
		/// Makes the camera within the CameraComponent of the given object the "active" camera.
		int setActiveCamera(GameObject* cam);

		/// Get the GameObject that contains the active camera
		GameObject* getActiveCamera() { return mActiveCamera; }

		/// Sets whether the default mouse cursor is visible or not
		void setCursorVisibility(bool isVisible) { SDL_ShowCursor(isVisible); }
	
		/// logs a message in our BaseApplication's default log.
		void logMessage(std::string toPrint, bool toDebug);

		///adds the message to the debug console
		void logToConsole(std::string print);

		/// Sets a string in one of the fixed slots on the debug gui display
		void setDebugString(unsigned int slot, std::string value);

		/// Toggles the debug console on / off
		void toggleDebugConsole();

		
	/***** HANDLERS *****/
	public:
		/// Called by input manager.  This is kind of hackish now.  A better solution would be to make
		/// an interface class that GameObject and Application inherit from.
		void onAction(InputManager::InputActions action, bool actionState);

	/***** ATTRIBUTES *****/
	protected:
		/// The pointer to the Ogre window
		Ogre::RenderWindow * mOgreWindow;

		/// The pointer to the SDL window (which is either the same window as
		/// that created by ogre, or a child of it)
		SDL_Window * mSDLWindow;

		/// The time of the last tick
		unsigned int mLastTick;

		/// Ogre root
		Ogre::Root * mRoot;

		/// Ogre scene manager
		Ogre::SceneManager * mSceneManager;

		/// Create the Ogre Overlay System
		Ogre::OverlaySystem* mOverlaySystem;

		/// Create an irrKlang sound manager
		SoundManager * mSoundManager;

		/// Creates the input manager
		InputManager * mInputManager;

		/// Create the script manager
		ScriptManager * mScriptManager;

		/// Physics Manager
		PhysicsManager * mPhysicsManager;

		/// The maximum number of recent log messages (visible in the console) to show
		const unsigned int mNumRecentLogMessages;

		/// Vector of strings for debug console
		std::vector<std::string> mRecentLogMessages;

		/// Game Object that is the active camera
		GameObject * mActiveCamera;
	public:

		/// Yost Sensor Setup
		TSS_Device_Id  mDevice;
		TSS_Device_Id  mD_device;
		TSS_Error mTss_error;
		TSS_ComPort mComport;

	};

}

#endif
