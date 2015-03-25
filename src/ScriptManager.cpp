#include <stdafx.h>
#include <ScriptManager.h>
#include <Utility.h>
#include <Message.h>
#include <BaseApplication.h>

/// Template-specialization of the msSingleton for ScriptManager.  Note:
/// We'll have to do this for every ssuge::Singleton-derived class
template<>
ssuge::ScriptManager* ssuge::Singleton<ssuge::ScriptManager>::msSingleton = NULL;

/// Constructors
ssuge::ScriptManager::ScriptManager()
{
	// The Lua State that will be used in this instance of SSUGE
	mLuaState = luaL_newstate();
	luaL_openlibs(mLuaState);

	// Regester high level functions
	ssuge::ScriptManager::__lua_register_ScriptManagerClass(mLuaState);
	ssuge::Message::__lua_register_MessageClass(mLuaState);
	ssuge::GameObject::__lua_GameObject_register_class(mLuaState);

	// Loading up the script files
	std::vector<std::string> toLoad;
	findFilesWithExtension("lua", toLoad);
	for (int i = 0; i < (int)toLoad.size(); i++) {
		if (luaL_dofile(mLuaState, toLoad[i].c_str())) 
		{
			__lua_debug(mLuaState);
		}
	}

	// TEST SHIT
	executeFunction("init");
	// END SHIT

}

lua_State* ssuge::ScriptManager::getLuaState()
{
	return mLuaState;
}

/// Lua Functions
int ssuge::ScriptManager::__lua_register_ScriptManagerClass(lua_State* L)
{
	// The methods of the Message "class"
	const struct luaL_Reg global_funcs[] =
	{
		{"log", __lua_log},
		{"input_register", __lua_input_register},
		{"input_getAxes", __lua_input_getAxes},
		{"findObjectsWithTag", __lua_find_gobjects}, 
		{"destroyObjectWithTag", __lua_destroy_gobject},
		{NULL, NULL}			// sentinel
	};

	luaL_register(L, "ssuge", global_funcs);

	return 0;
}

int ssuge::ScriptManager::__lua_log(lua_State* L) 
{
	if (lua_gettop(L) != 1) 
	{
		// ERROR GOES HERE~!
	}
	if (!lua_isstring(L, -1)) 
	{
		// ERROR GOES HERE~!
	}
	std::string toPrint = luaL_checkstring(L, -1);
	BASE_APPLICATION->logMessage(toPrint, false);
	lua_remove(L, -1);
	return 0;
}

int ssuge::ScriptManager::__lua_debug(lua_State* L) 
{
	std::string toPrint = luaL_checkstring(L, -1);
	BASE_APPLICATION->logMessage(toPrint, false);
	lua_remove(L, -1);

	//stack_dump(L);

	return 0;
}

int ssuge::ScriptManager::__lua_input_register(lua_State * L)
{
	GameObject::LuaGameObject * go = (GameObject::LuaGameObject *) lua_touserdata(L, -1);
	INPUT_MANAGER->addListener(go->mPtr);

	return 1;
}


int ssuge::ScriptManager::__lua_input_getAxes(lua_State * L)
{
float x, y, rx, ry;

	x = INPUT_MANAGER->getHorizAxis(INPUT_MANAGER->D_ALL);
	y = INPUT_MANAGER->getVertAxis(INPUT_MANAGER->D_ALL);
	rx = INPUT_MANAGER->getRHorizAxis(INPUT_MANAGER->D_ALL);
	ry = INPUT_MANAGER->getRVertAxis(INPUT_MANAGER->D_ALL);

	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, rx);
	lua_pushnumber(L, ry);
	return 4;
}


int ssuge::ScriptManager::__lua_find_gobjects(lua_State * L)
{
	std::vector<GameObject*> v;
	int tag = luaL_checkint(L, -1);
	BASE_APPLICATION->findObjectsByTag(v, tag);
	return 0;		// This will change
}


int ssuge::ScriptManager::__lua_destroy_gobject(lua_State * L)
{
	int tag = luaL_checkint(L, -1);
	BASE_APPLICATION->removeGameObjectByTag(tag);
	return 0;
}



int ssuge::ScriptManager::stack_dump(lua_State * L)
{
int i;
const int size_ts = 255, size_vs = 4095;
char type_str[size_ts+1], value_str[size_vs+1], num_str[8];
std::string s;

	s += std::string("\n===========\n|STACK DUMP\n===========\n"); 
	for (i=1; i<=lua_gettop(L); i++)
	{
		value_str[0] = '\0';
		if (lua_isboolean(L,  i))
		{
			sprintf_s(type_str, size_ts, "boolean");
			if (lua_toboolean(L, i))			
				sprintf_s(value_str, size_vs, "true");
			else										
				sprintf_s(value_str, size_vs, "false");
		}
		else if (lua_iscfunction(L, i))			
			sprintf_s(type_str, size_ts, "cfunction");
		else if (lua_isfunction(L, i))
		{
			lua_Debug ar;
			sprintf_s(type_str, size_ts, "lua-function");
			lua_pushvalue(L, i);
			lua_getinfo(L, ">S", &ar);
			sprintf_s(value_str, size_vs, "type=%s-function, src=%s, line=%d", ar.what, ar.source, ar.linedefined);
		}
		else if (lua_islightuserdata(L, i))		
			sprintf_s(type_str, size_ts, "light-user-data");
		else if (lua_isnil(L, i))				
			sprintf_s(type_str, size_ts, "nil");
		else if (lua_isnone(L, i))				
			sprintf_s(type_str, size_ts, "<none>");
		else if (lua_isnumber(L, i))
		{
			sprintf_s(type_str, size_ts, "number");
			sprintf_s(value_str, size_vs, "%f", lua_tonumber(L, i));
		}
		else if (lua_isstring(L, i))
		{
			sprintf_s(type_str, size_ts, "string");
			sprintf_s(value_str, size_vs, "%s", lua_tostring(L, i));
		}
		else if (lua_istable(L, i))
		{
			lua_pushnil(L);					// Used to start table traversal
			sprintf_s(value_str, size_vs, "{");
			bool first = true;
			while (lua_next(L, i) != 0)
			{
				// current key is at -2, value is at -1
				if (first)
				{
					strcat_s(value_str, size_vs, "key='");
					first = false;
				}
				else
					strcat_s(value_str, size_vs, ", key='");
				strcat_s(value_str, size_vs, lua_tostring(L, -2));
				strcat_s(value_str, size_vs, "'");
				lua_pop(L, 1);		// Removes value.  Keeps the key on the statck for next lua_next call
			}
			strcat_s(value_str, size_vs, "}");
			sprintf_s(type_str, size_ts, "table");
		}
		else if (lua_isuserdata(L, i))			
			sprintf_s(type_str, size_ts, "user-data");
		else
			sprintf_s(type_str, size_ts, "<<UNKNOWN>>");
		
		sprintf_s(num_str, 7, "%d", i);
		s += std::string(num_str) + " [" + std::string(type_str) + "] = '" + std::string(value_str) + "'\n";
		
	}
	s += std::string("\n===========\n|END STACK DUMP\n===========");

	BASE_APPLICATION->logMessage(s, false);
	return 0;
}

/// Destructor
ssuge::ScriptManager::~ScriptManager()
{
	lua_close(mLuaState);
}


void ssuge::ScriptManager::executeFunction(std::string functionName) 
{
	Message rv;
	//get object userdata from lua globals if nil error (Stack +1)
	lua_getglobal(mLuaState, functionName.c_str());
	if (lua_isnil(mLuaState, -1))
	{
		BASE_APPLICATION->logMessage("Error on Lua Function call: No such global function found!", false);
		return;
	}

	// call the method (Stack -1)
	int testCall = lua_pcall(mLuaState,0,0,0);
	BASE_APPLICATION->logMessage(std::to_string(testCall), false);
	if (testCall != 0) 
	{
		BASE_APPLICATION->logMessage("Error on Lua Function call: Error running the lua function!", false);
		std::string errorString = luaL_checkstring(mLuaState, -1);
		BASE_APPLICATION->logMessage(errorString, false);
	}
	BASE_APPLICATION->logMessage("End call!", false);
	return;
}

ssuge::Message ssuge::ScriptManager::executeMethod(std::string objName,std::string methodName,const ssuge::Message &m)
{
	Message rv;


	return rv;
}
