#include <stdafx.h>
#include <InputManager.h>
#include <BaseApplication.h>

/// Template-specialization of the msSingleton for InputManager.  Note:
/// We'll have to do this for every ssuge::Singleton-derived class
template<> ssuge::InputManager* ssuge::Singleton<ssuge::InputManager>::msSingleton = NULL;


ssuge::InputManager::InputManager(int w, int h) : Singleton(), mWidth(w), mHeight(h), 
	mLastMouseX(0), mLastMouseY(0), mMouseSensitivity(0.01f), mJoystickSensitivity(1.0f), mJoystickDeadzone(0.2f)
{
	// Initialize actions
	for (int i = 0; i < NUM_ACTIONS; i++)
	{
		mActiveActions[i] = false;
	}
	
	// Initialize lookup arrays
	mMouseButtonStrings[0] = "MOUSE_LEFT";
	mMouseButtonStrings[1] = "MOUSE_MIDDLE";
	mMouseButtonStrings[2] = "MOUSE_RIGHT";

	mJoyButtonStrings[0] = "JOY_UP";
	mJoyButtonStrings[1] = "JOY_DOWN";
	mJoyButtonStrings[2] = "JOY_LEFT";
	mJoyButtonStrings[3] = "JOY_RIGHT";
	mJoyButtonStrings[4] = "JOY_START";
	mJoyButtonStrings[5] = "JOY_BACK";
	mJoyButtonStrings[6] = "JOY_LSTICK";
	mJoyButtonStrings[7] = "JOY_RSTICK";
	mJoyButtonStrings[8] = "JOY_LBUMPER";
	mJoyButtonStrings[9] = "JOY_RBUMPER";
	mJoyButtonStrings[10] = "JOY_A";
	mJoyButtonStrings[11] = "JOY_B";
	mJoyButtonStrings[12] = "JOY_X";
	mJoyButtonStrings[13] = "JOY_Y";
	mJoyButtonStrings[14] = "JOY_CENTER";

	// Initialize joysticks
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	mNumJoysticks = SDL_NumJoysticks();
	if(mNumJoysticks > 0)
	{
		memset(mJoy,0,sizeof(SDL_Joystick*)*4);
		for(unsigned int i = 0; i < mNumJoysticks; i++)
		{
			mJoy[i] = SDL_JoystickOpen(i);
		
			if(!mJoy[i])
				BASE_APPLICATION->logMessage("did not open joy stick", true);
		}
	}
}



ssuge::InputManager::~InputManager()
{
	for(int i = 0; i < SDL_NumJoysticks(); i++)
	{
		if(SDL_JoystickGetAttached(mJoy[i]))
			SDL_JoystickClose(mJoy[1]);
	}
}



int ssuge::InputManager::update(float dt)
{
	SDL_Event evt;
	int messageRead;
	// Handle SDL input
	messageRead = SDL_PollEvent(&evt);
	if (messageRead)
	{
		if (evt.type == SDL_QUIT)
			return 1;
		else if (evt.type == SDL_KEYDOWN && evt.key.repeat == 0)
			onKeyDown(evt.key.keysym.sym);
		else if (evt.type == SDL_KEYUP && evt.key.repeat == 0)
			onKeyUp(evt.key.keysym.sym);
		else if (evt.type == SDL_MOUSEBUTTONDOWN)
			onMouseDown(evt.button.button, evt.button.x, evt.button.y);
		else if (evt.type == SDL_MOUSEBUTTONUP)
			onMouseUp(evt.button.button, evt.button.x, evt.button.y);
		else if (evt.type == SDL_JOYBUTTONDOWN)
			onMouseUp(evt.button.button, evt.button.x, evt.button.y);
		else if (evt.type == SDL_JOYBUTTONUP)
			onJoyButtonUp(evt.jbutton.which,evt.jbutton.button);
		else if (evt.type == SDL_JOYAXISMOTION)
			onJoyAxis(evt.jaxis.which,evt.jaxis.axis,evt.jaxis.value);
	}

	// Update the mouse offsets & positions
	int x, y;
	SDL_GetMouseState(&x, &y);
	mMouseOffsetX = x - mLastMouseX;
	mMouseOffsetY = y - mLastMouseY;
	mLastMouseX = x;
	mLastMouseY = y;

	return 0;
}



int ssuge::InputManager::onKeyDown(SDL_Keycode key)
{
	if ( mKeyboardMap.find(key) != mKeyboardMap.end() )
	{
		mActiveActions[ mKeyboardMap[key] ] = true;
		sendActionMessage( mKeyboardMap[key], true );
	}
	sendKeyMessage( std::string(SDL_GetKeyName(key)), true );

	return 0;
}



int ssuge::InputManager::onKeyUp(SDL_Keycode key)
{
	if ( mKeyboardMap.find(key) != mKeyboardMap.end() )
	{
		mActiveActions[ mKeyboardMap[key] ] = false;
		sendActionMessage( mKeyboardMap[key], false );
	}
	//sendKeyMessage( std::string(SDL_GetKeyName(key)), false );

	return 0;
}



int ssuge::InputManager::onMouseDown(unsigned char button, int x, int y)
{
	if ( mMouseButtonMap.find(button) != mMouseButtonMap.end() )
	{
		mActiveActions[ mMouseButtonMap[button] ] = true;
		sendActionMessage( mMouseButtonMap[button], true );
	}
	sendMouseMessage( button, true, x, y );

	return 0;
}



int ssuge::InputManager::onMouseUp(unsigned char button, int x, int y)
{
	if ( mMouseButtonMap.find(button) != mMouseButtonMap.end() )
	{
		mActiveActions[ mMouseButtonMap[button] ] = false;
		sendActionMessage( mMouseButtonMap[button], false );
	}
	sendMouseMessage( button, false, x, y );

	return 0;
}



int ssuge::InputManager::onJoyButtonDown(int joyNum, int button)
{
	if ( mJoyButtonMap.find(button) != mJoyButtonMap.end() )
	{
		mActiveActions[ mJoyButtonMap[button] ] = true;
		sendActionMessage( mJoyButtonMap[button], true );
	}
	sendJoyMessage( button, true, joyNum );

	return 0;
}



int ssuge::InputManager::onJoyButtonUp(int joyNum, int button)
{
	if ( mJoyButtonMap.find(button) != mJoyButtonMap.end() )
	{
		mActiveActions[ mJoyButtonMap[button] ] = false;
		sendActionMessage( mJoyButtonMap[button], false );
	}
	sendJoyMessage( button, false, joyNum );

	return 0;
}



int ssuge::InputManager::onJoyAxis(int joyNum, int axisNum, int axisVal)
{
	if(axisVal < -3200 || axisVal > 3200)
	{
		std::string axis_moved = std::to_string(axisNum);
		BASE_APPLICATION->logMessage(axis_moved, false);

	}
	return 0;
}



int ssuge::InputManager::addListener(GameObject* gObject)
{
	// if object already listening
	if ( std::find(mInputListeners.begin(), mInputListeners.end(), gObject) != mInputListeners.end() )
	{
		return 1;
	}
	mInputListeners.push_back( gObject );
	return 0;
}



int ssuge::InputManager::removeListener(GameObject* gObject)
{
	// only remove object if object is actually in the list of listeners
	int idx = std::find(mInputListeners.begin(), mInputListeners.end(), gObject) - mInputListeners.begin();
	if ( idx < (int)mInputListeners.size() )
	{
		mInputListeners.erase( mInputListeners.begin() + idx );
		return 0;
	}
	return 1;
}



void ssuge::InputManager::sendActionMessage(InputActions action, bool actionState)
{
	for (int i = 0; i < (int)mInputListeners.size(); i++)
	{
		mInputListeners[i]->injectScriptEvent("onAction", createMessage("si", actionEnumToString(action).c_str(), actionState ? 1 : 0));
	}
	BASE_APPLICATION->onAction(action, actionState);
}



void ssuge::InputManager::sendKeyMessage(std::string keyString, bool keyState)
{
	ssuge::Message m;
	m.addString(keyString);
	m.addInt((int)keyState);
	for (int i = 0; i < (int)mInputListeners.size(); i++)
	{
		mInputListeners[i]->injectScriptEvent("onKeydown", m);
	}
}



void ssuge::InputManager::sendMouseMessage(unsigned char button, bool buttonState, int x, int y)
{
	std::string buttonString = mMouseButtonStrings[button-1];
	for (int i = 0; i < (int)mInputListeners.size(); i++)
	{
		//mInputListeners[i]->injectScriptEvent();
	}
}



void ssuge::InputManager::sendJoyMessage(unsigned char button, bool buttonState, int joyNum)
{
	std::string buttonString = mJoyButtonStrings[button];
	for (int i = 0; i < (int)mInputListeners.size(); i++)
	{
		//mInputListeners[i]->injectScriptEvent();
	}
}



bool ssuge::InputManager::getKeystate(SDL_Keycode key)
{
	if (SDL_GetKeyboardState(NULL)[key])
		return true;
	return false;
}



void ssuge::InputManager::getMousePos(int & x, int & y)
{
	SDL_GetMouseState(&x, &y);
}



unsigned char ssuge::InputManager::getMouseButtons()
{
int tempx, tempy;

	return SDL_GetMouseState(&tempx, &tempy);
}



float ssuge::InputManager::getHorizAxis(InputDevices device)
{
	float horiz = 0, test_horiz = 0;
	// Joystick
	if (device != D_KEYBOARD && SDL_NumJoysticks() > 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (mJoy[i] && (device == D_ALL || (int)device == i) )
			{
				test_horiz = ((float)SDL_JoystickGetAxis(mJoy[i], 0) / 32767.0f);
				if (test_horiz > mJoystickDeadzone) horiz = (test_horiz-mJoystickDeadzone)/(1-mJoystickDeadzone);
				else if (test_horiz < -mJoystickDeadzone) horiz = -((-test_horiz)-mJoystickDeadzone)/(1-mJoystickDeadzone);
				else horiz = 0.0f;

				if ((int)device == i) // if we want this joystick input only, return now
				{
					return horiz;
				}
			}
		}
	}
	// Keyboard (will take precedence over joystick in case of D_ALL)
	if (device == D_KEYBOARD || device == D_ALL)
	{
		if (mActiveActions[ACTION_LEFT] && !mActiveActions[ACTION_RIGHT])
		{
			horiz = -1.0f;
		}
		else if (!mActiveActions[ACTION_LEFT] && mActiveActions[ACTION_RIGHT])
		{
			horiz = 1.0f;
		}
		else if (mActiveActions[ACTION_LEFT] && mActiveActions[ACTION_RIGHT])
		{
			horiz = 0.0f;
		}
	}

	return horiz;
}



float ssuge::InputManager::getVertAxis(InputDevices device)
{
float vert = 0, test_vert = 0;

	// Joystick
	if (device != D_KEYBOARD && SDL_NumJoysticks() > 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (mJoy[i] && (device == D_ALL || (int)device == i) )
			{
				test_vert = -((float)SDL_JoystickGetAxis(mJoy[i], 1) / 32767.0f);
				if (test_vert > mJoystickDeadzone) vert = (test_vert-mJoystickDeadzone)/(1-mJoystickDeadzone);
				else if (test_vert < -mJoystickDeadzone) vert = -((-test_vert)-mJoystickDeadzone)/(1-mJoystickDeadzone);
				else vert = 0.0f; 

				if ((int)device == i) // if we want this joystick input only, return now
				{
					return vert;
				}
			}
		}
	}
	// Keyboard (will take precedence over joystick in case of D_ALL)
	if (device == D_KEYBOARD || device == D_ALL)
	{
		if (mActiveActions[ACTION_DOWN] && !mActiveActions[ACTION_UP])
		{
			vert = -1.0f;
		}
		else if (!mActiveActions[ACTION_DOWN] && mActiveActions[ACTION_UP])
		{
			vert = 1.0f;
		}
		else if (mActiveActions[ACTION_DOWN] && mActiveActions[ACTION_UP])
		{
			vert = 0.0f;
		}
	}

	return vert;
}

float ssuge::InputManager::getRHorizAxis(InputDevices device)
{
float horiz = 0, test_horiz;

	// Joystick
	if (device != D_MOUSE && SDL_NumJoysticks() > 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (mJoy[i] && (device == D_ALL || (int)device == i) )
			{
				test_horiz = (mJoystickSensitivity*((float)SDL_JoystickGetAxis(mJoy[i], 2) / 32767.0f));
				if (test_horiz > mJoystickDeadzone) horiz = (test_horiz-mJoystickDeadzone)/(1-mJoystickDeadzone);
				else if (test_horiz < -mJoystickDeadzone) horiz = -((-test_horiz)-mJoystickDeadzone)/(1-mJoystickDeadzone);
				else horiz = 0.0f;
			}
		}
	}
	
	if (device == D_MOUSE || device == D_ALL)
	{
		test_horiz = ((float)mMouseOffsetX / mWidth) / mMouseSensitivity;
		if (test_horiz < -1.0f)			test_horiz = -1.0f;
		if (test_horiz > 1.0f)			test_horiz = 1.0f;
		if (fabs(test_horiz) > fabs(horiz))
			horiz = test_horiz;
	}

	return horiz;
}

float ssuge::InputManager::getRVertAxis(InputDevices device)
{
float vert = 0, test_vert;

	// Joystick

	if (device != D_MOUSE && SDL_NumJoysticks() > 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (mJoy[i] && (device == D_ALL || (int)device == i) )
			{
				test_vert = -(mJoystickSensitivity*((float)SDL_JoystickGetAxis(mJoy[i], 3) / 32767.0f));
				if (test_vert > mJoystickDeadzone) vert = (test_vert-mJoystickDeadzone)/(1-mJoystickDeadzone);
				else if (test_vert < -mJoystickDeadzone) vert = -((-test_vert)-mJoystickDeadzone)/(1-mJoystickDeadzone);
				else vert = 0.0f;
				//(m-k)/(1-k)
			}
		}
	}
	
	if (device == D_MOUSE || device == D_ALL)
	{
		test_vert=-((float)mMouseOffsetY / mHeight) / mMouseSensitivity;
		if (test_vert < -1.0f)		test_vert = -1.0f;
		if (test_vert > 1.0f)		test_vert = 1.0f;
		if (fabs(test_vert) > fabs(vert))
			vert = test_vert;
	}
	return vert;
}



bool ssuge::InputManager::actionIsActive(InputActions action)
{
	return mActiveActions[action];
}



void ssuge::InputManager::mapKeyAction(SDL_Keycode key, InputActions action)
{
	mKeyboardMap[key] = action;
}



void ssuge::InputManager::mapMouseAction(unsigned char button, InputActions action)
{
	mMouseButtonMap[button] = action;
}



void ssuge::InputManager::mapJoyAction(unsigned char button, InputActions action)
{
	mJoyButtonMap[button] = action;
}



std::string ssuge::InputManager::actionEnumToString(InputActions a)
{
std::string s = "";
	
	switch(a)
	{
	case ACTION_UP:		s = "up";		break;
	case ACTION_DOWN:	s = "down";		break;
	case ACTION_LEFT:	s = "left";		break;
	case ACTION_RIGHT:	s = "right";	break;
	case ACTION_JUMP:	s = "jump";		break;
	case ACTION_ATTACK:	s = "attack";	break;
	case ACTION_CROUCH:	s = "crouch";	break;
	case FIRST_PERSON:	s = "first";	break;
	case THIRD_PERSON:	s = "thrid";	break;
	case ACTION_PAUSE:	s = "pause";	break;
	case DEBUG:			s = "debug";	break;
	case GLOW:			s = "glow";		break;
	}

	return s;
}

