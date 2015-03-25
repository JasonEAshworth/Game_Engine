#include <stdafx.h>
#include <Message.h>
#include <GameObject.h>


ssuge::Message::Message() 
{
	initialize();
}


ssuge::Message::Message(int numChunks) 
{
	mNumParam = 0;
	mNumChunks = 0;
	mMaxChunks = numChunks;
	mParams = new DataItem[mMaxChunks];
}


ssuge::Message::Message(const Message & m)
{
	mNumParam = m.mNumParam;
	mNumChunks = m.mNumChunks;
	mMaxChunks = m.mMaxChunks;
	mParams = new DataItem[mMaxChunks];
	if (mParams)
		memcpy(mParams, m.mParams, sizeof(DataItem) * mMaxChunks);
}



ssuge::Message::~Message()
{
	destroy();
}



void ssuge::Message::initialize()
{
	mNumParam = 0;
	mNumChunks = 0;
	mMaxChunks = START_CHUNK_NUM;
	mParams = new DataItem[mMaxChunks];
}


void ssuge::Message::destroy()
{
	if (mParams)
		delete mParams;
	mParams = NULL;
}


ssuge::Message & ssuge::Message::operator = (const ssuge::Message & m)
{
	if (mParams)
		delete [] mParams;

	mNumParam = m.mNumParam;
	mNumChunks = m.mNumChunks;
	mMaxChunks = m.mMaxChunks;
	mParams = new DataItem[mMaxChunks];
	if (mParams)
		memcpy(mParams, m.mParams, sizeof(DataItem) * mMaxChunks);

	return *this;
}






void ssuge::Message::increaseSize()
{
	while(mNumChunks > mMaxChunks) 
	{
		mMaxChunks = 2 * mMaxChunks;
	}
	DataItem* temp = mParams;
	mParams = new DataItem[mMaxChunks];
	for (unsigned int i = 0; i < mNumChunks - 1; i++) 
		mParams[i] = temp[i];
	
	delete temp;
}


void ssuge::Message::addInt(int i)
{
	mNumChunks++;
	if (mNumChunks > mMaxChunks) 
	{
		increaseSize();
	}
	DataItem di;
	di.mType = 'i';
	di.mData.i = i;
	di.mParamNum = (unsigned char)mNumParam;
	mParams[mNumChunks-1] = di;
	mNumParam++;
}


void ssuge::Message::addFloat(float f)
{
	mNumChunks++;
	if (mNumChunks > mMaxChunks) 
	{
		increaseSize();
	}
	DataItem df;
	df.mType = 'f';
	df.mData.f = f;
	df.mParamNum = (unsigned char)mNumParam;
	mParams[mNumChunks-1] = df;
	mNumParam++;
}

void ssuge::Message::addGameObject(GameObject* g) 
{
	mNumChunks++;
	if (mNumChunks > mMaxChunks) 
	{
		increaseSize();
	}
	DataItem dg;
	dg.mType = 'g';
	dg.mData.g = g;
	dg.mParamNum = (unsigned char)mNumParam;
	mParams[mNumChunks-1] = dg;
	mNumParam++;
}

void ssuge::Message::addString(std::string str)
{
const char * s = str.c_str();
	//Ogre::LogManager::getSingleton().getLog("mLog.log")->logMessage(std::string(s));
	int stringLength = strlen(s);
	mNumChunks += stringLength / 4;
	if (strlen(s) % 4 != 0) 
	{
		mNumChunks++;
	}
	if (mNumChunks > mMaxChunks) 
	{
		increaseSize();
	}
	for (int i = 0; i < (int)strlen(s) / 4; i++)
	{
		DataItem ds;
		ds.mType='s';
		//Ogre::LogManager::getSingleton().getLog("mLog.log")->logMessage("Mem1\n    *" + std::to_string(i));
		memcpy(ds.mData.s, s + (4 * i), 4); 
		//Ogre::LogManager::getSingleton().getLog("mLog.log")->logMessage(std::string(ds.mData.s, 4));
		ds.mParamNum = (unsigned char)mNumParam;
		mParams[mNumParam + i] = ds;
	}
	if (strlen(s) % 4 != 0) 
	{
		DataItem ds;
		ds.mType='s';
		//Ogre::LogManager::getSingleton().getLog("mLog.log")->logMessage("Mem2\n    *EXTRA");
		memcpy(ds.mData.s, s + 4 * (strlen(s) / 4), 4); 
		//ds.mData.s[strlen(s) % 4 - 1] = '\0';
		//Ogre::LogManager::getSingleton().getLog("mLog.log")->logMessage(std::string(ds.mData.s));
		ds.mParamNum = (unsigned char)mNumParam;
		mParams[mNumChunks - 1] = ds;
	}
	mNumParam++;
}



std::string ssuge::Message::toString() 
{
	std::string toReturn = "ssuge.Message<", sep = "";
	for (unsigned int i = 0; i < mNumParam; i++) 
	{
		char c = getParamType(i);
		if (c == 's')
			toReturn = toReturn + sep + getString(i);
		else if (c == 'i')
			toReturn = toReturn + sep + std::to_string(getInt(i));
		else if (c == 'f')
			toReturn = toReturn + sep + std::to_string(getFloat(i));
		else if (c == 'g')
		{
			GameObject * temp = getGameObject(i);
			toReturn = toReturn + sep + temp->getName();
		} 
		sep = ", ";
	}
	return toReturn + ">";
}



char ssuge::Message::getParamType(unsigned int paramNum) const
{
	for (unsigned int i = 0; i < mNumChunks; i++)
	{
		if (mParams[i].mParamNum == (unsigned char)paramNum)
			return mParams[i].mType;
	}

	return '?';
}

int ssuge::Message::getInt(unsigned int paramNum) const
{
	for (unsigned int i = 0; i < mNumChunks; i++)
	{
		if (mParams[i].mParamNum == (unsigned char)paramNum)
		{
			if (mParams[i].mType == 'i')
				return mParams[i].mData.i;
			else if (mParams[i].mType == 'f')
				return (int)mParams[i].mData.f;
			else
				throw ssuge::Exception(__FILE__, __LINE__, "Message Parameter can't be converted to an int");
		}
	}

	return 0;   // The requested int wasn't found.  Do we raise an exception???
}

float ssuge::Message::getFloat(unsigned int paramNum) const
{
	for (unsigned int i = 0; i < mNumChunks; i++)
	{
		if (mParams[i].mParamNum == (unsigned char)paramNum)
		{
			if (mParams[i].mType == 'f')
				return mParams[i].mData.f;
			else if (mParams[i].mType == 'i')
				return (float)mParams[i].mData.i;
			else
				throw ssuge::Exception(__FILE__, __LINE__, "Message parameter can't be converted to float");
		}
	}

	return 0.0f;  // The requested float wasn't found.  Do we raise an exception???
}

ssuge::GameObject* ssuge::Message::getGameObject(unsigned int paramNum) const
{
	for (unsigned int i = 0; i < mNumChunks; i++)
	{
		if (mParams[i].mParamNum == (unsigned char)paramNum)
		{
			return mParams[i].mData.g;
		}
	}

	return NULL;	// The requested GameObject wasn't found.  Do we raise an exception???
}

std::string ssuge::Message::getString(unsigned int paramNum) const
{
	int stringStart = -1;
	int chunkSize = 0;

	for (unsigned int i = 0; i < mNumChunks; i++)
	{
		if (mParams[i].mParamNum == (unsigned char)paramNum)
		{
			if (stringStart == -1) 
			{
				stringStart = i;
			}
			chunkSize++;
		}
	}
	char* cString = new char [chunkSize * 4 + 1];
	cString[0] = '\0';
	for (int i = 0; i < chunkSize; i++)
	{
		//Ogre::LogManager::getSingleton().getLog("mLog.log")->logMessage("Mem3\n    *" + std::to_string(i) + "\n");
		memcpy(cString + (i * 4), mParams[i + stringStart].mData.s, 4);
	}
	cString[chunkSize * 4] = '\0';		// Needed when string is a multiple of 4.
	return std::string(cString);
}



int ssuge::Message::__lua_register_MessageClass(lua_State * L)
{
	// The methods of the Message "class"
	const struct luaL_Reg msg_funcs[] =
	{
		{"new", __lua_Message_new},
		{"__len", __lua_Message_length},
		{"__tostring", __lua_Message_tostring},
		{"__index", __lua_Message_index},
		{"__gc", __lua_Message_gc},
		{NULL, NULL}			// sentinel
	};

	luaL_newmetatable(L, "ssuge.Message");		// Creates a new metatable at the top of the stack
	lua_pushvalue(L, -1);							// Make a copy of the metatable
	lua_setfield(L, -2, "__index");					// This line and the line before set the __index field
													// of the metatable to be itself (like "x.__index = x" in Lua)
	luaL_register(L, NULL, msg_funcs);				// Pops the metatable off the stack and registers
													// everything in the msg_funcs array as methods of
													// the "testModule.Message" class.
	lua_setfield(L, -2, "Message");				// Makes an entry in the ssuge table for the Message table.

	return 0;
}


int ssuge::Message::__lua_Message_new(lua_State * L)
{
	Message * mptr;
	int num_params, i;
	// STACK = [Message table, value1, value2, ..., valueN]

	num_params = (int)lua_gettop(L);
	mptr = (Message*)lua_newuserdata(L, sizeof(Message));   // creates a new buffer on the top of the stack
															// NOTE: This does NOT call the constructor.
	mptr->initialize();
	
	// STACK = [Message table, value1, value2, ..., valueN, user-datum]
	
	luaL_getmetatable(L, "ssuge.Message");
	lua_setmetatable(L, -2);

	// Figure out how much DataItems we need (in bytes) for the actual elements
	for (i=1; i<num_params; i++)
	{
		if (lua_isnumber(L, i+1))
		{
			float f = (float)lua_tonumber(L, i+1);
			if (f - (int)f == 0.0f)
				mptr->addInt((int)f);
			else
				mptr->addFloat(f);
		}
		else if (lua_isstring(L, i+1))
		{
			mptr->addString((char*)lua_tostring(L, i+1));
		}
		// We might want to raise a warning here -- the user passed an unhandled type (in Lua)
	}
	
	// We're done with the passed parameters -- remove them from the stack
	lua_replace(L, 1);        // Swap the user-data with the first element on the stack  
	lua_settop(L, 1);         // Pop everything but the first element.

	// STACK = [user-datum]

	// We're returning the user-datum.
	return 1;
}


int ssuge::Message::__lua_Message_length(lua_State * L)
{
Message * mptr;

	//printf("[C]: in l_msg_length\n");
	mptr = (Message*)luaL_checkudata(L, 1, "ssuge.Message");
	lua_pushinteger(L, mptr->mNumParam);
	return 1;
}


int ssuge::Message::__lua_Message_tostring(lua_State * L)
{
Message * mptr;
	
	//printf("[C] in l_msg_print\n");
	mptr = (Message*)luaL_checkudata(L, 1, "ssuge.Message");
	lua_pushstring(L, mptr->toString().c_str());
	return 1;
}


int ssuge::Message::__lua_Message_index(lua_State * L)
{
Message * mptr;
unsigned int index;
char t;

	mptr = (Message*)luaL_checkudata(L, 1, "ssuge.Message");
	if (!lua_isnumber(L, -1))
		luaL_error(L, "You must pass an integer index number");
	else
	{
		index = lua_tointeger(L, -1);
		if (index > (unsigned int)mptr->mNumParam || index < 1)
			luaL_error(L, "Invalid index number (%d)", index);
		else
		{
			t = mptr->getParamType(index-1);
			if (t == 'i')
			{
				lua_pushinteger(L, mptr->getInt(index-1));
				return 1;
			}
			else if (t == 'f')
			{
				lua_pushnumber(L, mptr->getFloat(index-1));
				return 1;
			}
			else if (t == 's')
			{
				lua_pushstring(L, mptr->getString(index-1).c_str());
				return 1;
			}
			else if (t == 'g')
			{
				GameObject* g = mptr->getGameObject(index-1);
				lua_getglobal(L, g->getName().c_str());
				return 1;
			}
		}
	}
	lua_pushnil(L);
	return 1;
}


int ssuge::Message::__lua_Message_gc(lua_State * L)
{
Message * mptr;

	mptr = (Message*)luaL_checkudata(L, 1, "ssuge.Message");
	mptr->destroy();
	return 0;
}



void ssuge::Message::__lua_push_Message(lua_State * L, const Message & m)
{
unsigned int i;
char ptype;
Message * mptr;

	mptr = (Message*)lua_newuserdata(L, sizeof(Message));   // creates a new buffer on the top of the stack
															// NOTE: This does NOT call the constructor.
	luaL_getmetatable(L, "ssuge.Message");
	lua_setmetatable(L, -2);

	mptr->initialize();

	// Push the elements of the message onto the stack
	for (i=0; i<m.getNumParams(); i++)
	{
		ptype = m.getParamType(i);
		if (ptype == 'i')
			mptr->addInt(m.getInt(i));
		else if (ptype == 'f')
			mptr->addFloat(m.getFloat(i));
		else if (ptype == 's')
			mptr->addString(m.getString(i));
		else if (ptype == 'g')
		{
			mptr->addGameObject(m.getGameObject(i));
		}
	}
	//SCRIPT_MANAGER->stack_dump(L);
}

		
const ssuge::Message ssuge::Message::__lua_pop_Message(lua_State * L, int index, bool removeFromStack)
{
Message * mptr, m;
unsigned int i;
char ptype;

	mptr = (Message*)luaL_checkudata(L, index, "ssuge.Message");
	for (i=0; i<mptr->getNumParams(); i++)
	{
		ptype = mptr->getParamType(i);
		if (ptype == 'i')
			m.addInt(mptr->getInt(i));
		else if (ptype == 'f')
			m.addFloat(mptr->getFloat(i));
		else if (ptype == 's')
			m.addString(mptr->getString(i));
	}

	return m;
}



const ssuge::Message ssuge::Message::__lua_get_Message(lua_State * L, int startPos, bool removeFromStack)
{
ssuge::Message m;
unsigned int i, endPos;
float f;

	// Get the number of elements in the lua message
	endPos = lua_gettop(L);

	// Extract the elements from the stack
	for (i=startPos; i<=endPos; i++)
	{
		if (lua_isnumber(L, i))
		{
			f = (float)lua_tonumber(L, i);
			if (f - (int)f == 0.0f)
				m.addInt((int)f);
			else
				m.addFloat(f);
		}
		else if (lua_isstring(L, i))
			m.addString(std::string(lua_tostring(L, i)));
	}

	if (removeFromStack)
		lua_settop(L, startPos - 1);

	return m;
}





ssuge::Message ssuge::createMessage(char* initString, ...) 
{
	// Arg variable init
	int numOfArg = strlen(initString);
	va_list argumentList;
	int initNumChunks = 0;

	// Find out how many chunks the input needs
	va_start(argumentList, initString);
	for (int i = 0; i < numOfArg; i++) 
	{
		switch (initString[i])
		{
			case 's':
			case 'S':
			{
				char* valS = va_arg(argumentList, char*);
				//Ogre::LogManager::getSingleton().getLog("mLog.log")->logMessage(std::string(valS));
				initNumChunks += strlen(valS) / 4;
				if (strlen(valS) % 4 != 0) 
				{
					initNumChunks++;
				}
			}
			break;
			case 'i':
			case 'I':
			{
				int valI = va_arg(argumentList, int);
				//Ogre::LogManager::getSingleton().getLog("mLog.log")->logMessage(std::to_string(valI));
				initNumChunks++;
			}
			break;
			case 'f':
			case 'F':
			{
				float valF = va_arg(argumentList, float);
				//Ogre::LogManager::getSingleton().getLog("mLog.log")->logMessage(std::to_string(valF));
				initNumChunks++;
			}
			break;
			case 'g':
			case 'G':
			{
				GameObject* valG = va_arg(argumentList, GameObject*);
				initNumChunks++;
			}
			break;
		}
	}
	va_end(argumentList);
	
	// Create the message we are going to return
	Message toReturn(initNumChunks);

	// Store the sent arguments into the message
	va_start(argumentList, initString);
	for (int i = 0; i < numOfArg; i++) 
	{
		switch (initString[i])
		{
			case 's':
			case 'S':
			{
				char* valS = va_arg(argumentList, char*);
				toReturn.addString(valS);
			}
			break;
			case 'i':
			case 'I':
			{
				int valI = va_arg(argumentList, int);
				toReturn.addInt(valI);
			}
			break;
			case 'f':
			case 'F':
			{
				float valF = (float)va_arg(argumentList, double);
				toReturn.addFloat(valF);
			}
			break;
			case 'g':
			case 'G':
			{
				GameObject* valG = va_arg(argumentList, GameObject*);
				toReturn.addGameObject(valG);
			}
			break;
		}
	}
	va_end(argumentList);
	return toReturn;
}
