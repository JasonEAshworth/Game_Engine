#ifndef _SCRIPTMANAGER_H_
#define _SCRIPTMANAGER_H_

#include <stdafx.h>
#include <Singleton.h>
#include <Message.h>

/// A convenience shortcut to access the ScriptManager singleton
#define SCRIPT_MANAGER static_cast<ScriptManager*>(ssuge::ScriptManager::getSingletonPtr())

namespace ssuge
{
	// [TO-DO]: Derive from singleton
	class ScriptManager : public Singleton<ScriptManager> 
	{
	/**** Constructor / Destructors *****/
	public:
		/// Constructors
		ScriptManager();

		/// Destructor
		~ScriptManager();

		/// Returns a pointer to the manager's lua state
		lua_State* getLuaState();

	/**** Lua Functions ****/
		static int __lua_register_ScriptManagerClass(lua_State * L);
		static int __lua_log(lua_State* L);
		static int __lua_debug(lua_State* L);
		static int __lua_input_register(lua_State * L);
		static int __lua_input_getAxes(lua_State * L);
		static int __lua_find_gobjects(lua_State * L); 
		static int __lua_destroy_gobject(lua_State * L);
		static int stack_dump(lua_State * L);

		void executeFunction(std::string functionName);
		Message executeMethod(std::string objName,std::string methodName,const ssuge::Message &m);

	/**** Attributes ****/
	protected:
		/// Variables 
		lua_State* mLuaState;
	};

}
#endif