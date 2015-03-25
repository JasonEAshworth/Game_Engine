#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include <stdafx.h>

/// The number of "chunks" a Message starts with (if not specified).
/// The message can grow if it exceeds this initial bound.
#define START_CHUNK_NUM 3

namespace ssuge
{
	// A forward reference to the GameObject class
	class GameObject;

	// A forward reference to the ScriptManager class.
	class ScriptManager;

	/// One element of the message.  Usually an element is a "parameter", except
	/// in the case of strings which are longer than 4 characters.  In this case,
	/// each DataItem will share the same parameter number (for reconstruction).
	class DataItem
	{
	public:
		/// This indicates the type of data being used in the union.
		char mType; //'i','f','s','g'

		/// A union of the possible data types encoded in this DataItem.  As with
		/// all unions, you only ever access one of these at a time (since they
		/// all share the same (here, 4 bytes of) memory.
		union 
		{
			int i;
			float f;
			char s[4];
			GameObject* g;
		} mData;

		/// The monotonically increasing parameter number.  Only in the case of strings
		/// longer than 4 characters will two DataItem's have the same parameter number.
		unsigned char mParamNum;
	};


	/// A Message is a (limited) heterogeneous array of data that can be sent between
	/// GameObjects (and into the script "world").
	class Message
	{
	/***** Constructors / Destructors *****/
	public:
		// Default constructor.  Initializes the list of DataItems to be of size START_CHUNK_NUM
		Message();

		/// Constructor.  Initializes the list of DataItems to be of the given size (initially).
		Message(int numCunks);

		/// [NEW] Copy constructor.  Makes a deep copy of all data within m.
		Message(const Message & m);    

		/// Destructor
		~Message();

		/// C-style "constructor".  Does the same thing as the constructor.  It will NOT deallocate any 
		/// allocated memory!
		void initialize();

		/// C-style "destructor".  Called by the destructor.
		void destroy();

	/***** Operators *****/
	public:
		/// [NEW] Assignment operator.  Does a deep copy of m to this and returns this.
		Message & operator = (const Message & m);   // NEW

		

	/**** Methods *****/
	protected:
		/// Increase Size will double the currently alocated data items.  In addition it will move the 
		/// current array to the new memory.  This is called when adding a new DataItem would exceed our bounds.
		void increaseSize();

		/// temporary
		static void stack_dump(lua_State * L);
		
	public:
		/// Adds a new integer to the end of the message (increasing size if necessary).
		void addInt(int i);

		/// Adds a new float to the end of the message (increasing size if necessary).
		void addFloat(float f);

		/// Adds a new GameObject* to the end of the message (increasing size if necessary).
		void addGameObject(GameObject* g);

		/// Adds a new string to the end of the message (increasing size if necessary).
		void addString(std::string s);

		/// Returns a formatted string of all the items in the Message
		std::string toString();

	/***** Getters *****/
	public:
		/// Get the type of the given parameter number.  The codes are the same as those used
		/// in the DataItem class.
		char getParamType(unsigned int paramNum) const;

		/// Gets an integer from the given parameter number (0 if it can't be converted to an int)
		int getInt(unsigned int paramNum) const;

		/// Gets a float from the given parameter number (0.0f if it can't be converted to a float)
		float getFloat(unsigned int paramNum) const;

		/// Gets a game object * from the given parameter number (NULL if it can't be converted)
		ssuge::GameObject* getGameObject(unsigned int paramNum) const;

		/// Gets a string from the given parameter number ("" if it can't be converted)
		std::string getString(unsigned int paramNum) const;

		/// Returns the number of "things" in the Message (not the number of raw chunks)
		unsigned int getNumParams() const { return mNumParam; }

    /***** LUA Related *****/
	public:
		/// Called to create the Message "class" in Lua (probably called when opening Lua)
		static int __lua_register_MessageClass(lua_State * L);

		/// Creates a new Message object (in Lua).  All values on the stack are inserted into
		/// the new lua message object.  After this method is called the Message object (userdata)
		/// will be the only thing on the stack.  
		static int __lua_Message_new(lua_State * L);

		/// Pushes the length of the message onto the stack
		static int __lua_Message_length(lua_State * L);

		/// Pushes a string-representation of the Message onto the stack
		static int __lua_Message_tostring(lua_State * L);

		/// Returns an element of the Message object as if it were a list
		/// (remember in lua, indicies are 1-based).
		static int __lua_Message_index(lua_State * L);

		/// Called when the message is about to be garbage-collected.
		static int __lua_Message_gc(lua_State * L);

		/// Create a new Lua Message on top of the stack from the given C++ Message object.
		static void __lua_push_Message(lua_State * L, const Message & m);

		/// Extracts values from the lua stack to make a C++ Message object.  The start position
		/// marks the starting stack index; all values from there to the top of the stack
		/// are considered.  If removeFromStack is true, the values are removed from the lua
		/// stack (if not, they remain on the stack).
		static const Message __lua_get_Message(lua_State * L, int startPos, bool removeFromStack);

		/// Converts the (lua) Message object at the given stack position to a C++ Message object.
		/// If removeFromStack is true, it is also removed from the stack.
		static const Message __lua_pop_Message(lua_State * L, int index, bool removeFromStack);

	/***** Attributes *****/
	protected:
		/// The master array of DataItems.
		DataItem* mParams;

		/// The number of parameters in mParams.  Note: this value will be less than
		/// or equal to mNumChunks.
		unsigned int mNumParam;

		/// The number of actual DataItems in mParams.
		unsigned int mNumChunks;

		/// The maximum number of chunks that we have allocated mParams to hold.
		unsigned int mMaxChunks;

	/***** Friends *****/
		friend class ScriptManager;
	};

	/// A function to create a new message from a printf-style format string.
	Message createMessage(char* initString, ...);
}
#endif