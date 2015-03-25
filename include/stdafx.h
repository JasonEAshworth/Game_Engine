// Windows-specific headers
#ifdef _WIN32
#include <Windows.h>
#endif

// SDL
#include <SDL.h>
#undef main

// Ogre
#include <Ogre.h>
#include <Overlay\OgreOverlay.h>
#include <Overlay\OgreOverlayManager.h>
#include <Overlay\OgreOverlayContainer.h>
#include <Overlay\OgreOverlaySystem.h>
#include <Overlay\OgreTextAreaOverlayElement.h>
#include <Overlay\OgrePanelOverlayElement.h>
#include <Overlay\OgreFontManager.h>
#include <Overlay\OgreOverlayElement.h>
#include <Overlay\OgreOverlayElementCommands.h>
#include <Overlay\OgreOverlayElementFactory.h>
// irrklang
#include <irrKlang.h>

// standard C/C++
#include <map>
#include <string>
#include <cstdio>
#include <vector>
#include <iostream>
#include <cstring>

// Lua
#include <lua.hpp>

// Bullet
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <yei_threespace_api.h>
// Oculus VR
#include <OVR.h>

