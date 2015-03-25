#ifndef _INPUT_MANAGER_
#define _INPUT_MANAGER_

#include <stdafx.h>
#include <Singleton.h>
#include <GameObject.h>

/// A convenience shortcut to access the Input Manager singleton
#define INPUT_MANAGER static_cast<ssuge::InputManager*>(ssuge::InputManager::getSingletonPtr())



/// This is the number of actions in the InputActions enum below.  Make sure if you change
/// one, you change the other.
#define NUM_ACTIONS 11



namespace ssuge
{
	// Forward-reference to the BaseApplication class
	class BaseApplication;

	/// The InputManager class. This class is responsible for managing all user input for 
	/// the engine from keyboard, mouse, and gamepad.
	class InputManager : public Singleton<InputManager>
	{
	/**** CONSTRUCTOR / DESTRUCTOR *****/
	public:
		/// Constructor for the InputManager
		InputManager(int w = 640, int h = 480);

		/// An enumeration of types of InputActions to use for input 
		/// IMPORTANT: Make sure that the NUM_ACTIONS macro at the top of this file matches the length of this enum!!
		//enum mInputActions {ACTION_UP, ACTION_DOWN, ACTION_LEFT, ACTION_RIGHT, ACTION_JUMP, ACTION_ATTACK};
		//enum mInputActions {ACTION_UP, ACTION_DOWN, ACTION_LEFT, ACTION_RIGHT, ACTION_START, ACTION_BACK, ACTION_CROUCH, ACTION_ZOOM, ACTION_LB, ACTION_RB, ACTION_JUMP, ACTION_ATTACK, ACTION_RELOAD, ACTION_SWITCH, ACTION_HOME};
		enum InputActions {ACTION_UP, ACTION_DOWN, ACTION_LEFT, ACTION_RIGHT, ACTION_JUMP, ACTION_ATTACK, ACTION_CROUCH, FIRST_PERSON, THIRD_PERSON, ACTION_PAUSE, DEBUG, GLOW};

		/// An enumeration of input types to use as arguments in a few of the get methods
		enum InputDevices {D_JOY1, D_JOY2, D_JOY3, D_JOY4, D_KEYBOARD, D_MOUSE, D_ALL};

		/// Destructor for the InputManager 
		~InputManager();

	/***** HANDLERS: These are triggered when SDL detects an event.  DON'T call them directly *****/
	protected:
		/// This function should be called every frame (by the application) to recieve SDL input 
		/// and convert it to the proper actions.  It will also handle sending input-related 
		/// messages to subscribed gameobjects
		int update(float dt);

		/// [handler] called when a key is pressed
		int onKeyDown(SDL_Keycode key);

		/// [handler] called when a key is released
		int onKeyUp(SDL_Keycode key);

		/// [handler] called when a mouse button is pressed
		int onMouseDown(unsigned char button, int x, int y);

		/// [handler] called when a mouse button is released
		int onMouseUp(unsigned char button, int x, int y);

		/// [handler] called when a gamepad button goes down
		int onJoyButtonDown(int joyNum, int button);

		/// [handler] called when a gamepad button goes up
		int onJoyButtonUp(int joyNum, int button);

		/// [handler] called when a gamepad axis is moved
		int onJoyAxis(int joyNum, int axisNum, int axisVal);

	/***** OBSERVER-related methods *****/
	public:
		/// Takes a reference to a GameObject and sends it a message whenever an Action changes states
		/// Returns 0 if successfully added, returns 1 if object already listening
		int addListener(GameObject *gObject);

		/// Takes a reference to a GameObject and removes it from the list of input listeners
		/// Returns 0 on successful remove, returns 1 if object was not a listener
		int removeListener(GameObject *gObject);

	protected:
		/// Called when the state of any action changes. Sends a message to subscribed game objects, letting them
		/// know that the action has changed
		void sendActionMessage(InputActions action, bool actionState);

		/// Called when the state of the given key is changed. Sends a message to subscribed game objects, letting
		/// them know that the keystate has changed
		void sendKeyMessage(std::string keyString, bool keyState);

		/// Called when the state of the given mouse button is changed. Sends a message to subscribed game objects,
		/// letting them know that the button state has changed
		void sendMouseMessage(unsigned char button, bool buttonState, int x, int y);

		/// Called when the state of the given joy button is changed. Sends a message to subscribed game objects,
		/// letting them know that the joy button state has changed
		void sendJoyMessage(unsigned char button, bool buttonState, int joyNum);

	/**** DEVICE-POLLING-related methods *****/
	public:
		/// Gets the pressed state of the given key
		bool getKeystate(SDL_Keycode key);

		/// Get the mouse's current position
		void getMousePos(int & x, int & y);

		/// Gets the current mouse button state
		unsigned char getMouseButtons();

		/// Returns the state of horizontal input from the specified device(s) with a value from -1 to 1 (left to right)
		float getHorizAxis(InputDevices device);

		/// Returns the state of vertical input from the specified device(s) with a value from -1 to 1 (down to up)
		float getVertAxis(InputDevices device);

		/// Returns the state of horizontal input from the specified device(s) with a value from -1 to 1 (left to right)
		float getRHorizAxis(InputDevices device);

		/// Returns the state of vertical input from the specified device(s) with a value from -1 to 1 (down to up)
		float getRVertAxis(InputDevices device);

		/// Returns true if requested action is currently active, false if not
		/// (Use this in the application's update)
		bool actionIsActive(InputActions action);


	/***** REMAPPABLE ACTIONS methods *****/
	public:
		/// Maps an SDL KeyCode to an Action
		void mapKeyAction(SDL_Keycode key, InputActions action);

		/// Maps an SDL MouseButton char code to an Action
		void mapMouseAction(unsigned char button, InputActions action);

		/// Maps an SDL JoyButton char code to an Action (no support for joy axis just yet)
		void mapJoyAction(unsigned char button, InputActions action);

	/***** MISCELLANEOUS *****/
	public:
		static std::string actionEnumToString(InputActions a);

	/***** ATTRIBUTES *****/
	protected:
		/// An array that corresponds with mInputActions to keep track of which actions are currently active
		bool mActiveActions[NUM_ACTIONS];

		/// A lookup array for converting SDL_MouseButtons to c++ strings
		std::string mMouseButtonStrings[3];

		/// A lookup array for converting joystick buttons to c++ strings
		std::string mJoyButtonStrings[15];

		/// The number of joysticks available.  This number is calculated when the input manager is created.
		unsigned int mNumJoysticks;

		/// Joy stick pointer
		SDL_Joystick *mJoy[4];

		///A map for converting SDL KeyCodes to mInputActions
		std::map<SDL_Keycode, InputActions> mKeyboardMap;

		///A map for converting SDL MouseButton to mInputActions
		std::map<unsigned char, InputActions> mMouseButtonMap;

		///An array maps for converting SDL JoyButton to mInputActions for each player
		std::map<unsigned char, InputActions> mJoyButtonMap;

		/// A vector of listeners that have subscribed to action updates from the input manager
		std::vector<GameObject*> mInputListeners;

		/// Previous mouse pos
		int mLastMouseX, mLastMouseY;

		/// Number of pixels mouse has moved since the last frame
		int mMouseOffsetX, mMouseOffsetY;

		/// window width & heigth
		int mWidth, mHeight;

		/// Mouse sensitivity: The fractional percentage of the screen that needs to be moved to 
		///    have an rHoriz or rVert value of 1.0.  This sensitivity value should be greater than 
		///    0.0 but less than 1.0.  Lower values will increase the value of rHoriz / rVert.
		float mMouseSensitivity;

		/// Gamepad sensitivity (between 0.0 = no joystick and 1.0 = full-response)
		float mJoystickSensitivity;

		/// Gamepad dead zone (between 0.0 = no dead zone and 1.0 = no response)
		float mJoystickDeadzone;

	/***** FRIENDS *****/
		friend class BaseApplication;
	};
}

#endif