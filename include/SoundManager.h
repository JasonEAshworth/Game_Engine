#ifndef _SOUND_MANAGER_H_
#define _SOUND_MANAGER_H_

#include <stdafx.h>
#include <Singleton.h>
#include <GameObject.h>
#include <map>

/// A convenience shortcut to access the SoundManager singleton
#define SOUND_MANAGER static_cast<SoundManager*>(ssuge::SoundManager::getSingletonPtr())


namespace ssuge
{
	/// The SoundManager class. This class is responsible for managing the loading, closing, 
	/// and playing(?) of sound files for the engine.
	class SoundManager : public Singleton<SoundManager>
	{
    /***** CONSTRUCTOR / DESTRUCTOR *****/
    public:
		///Constructor for the SoundManager
		SoundManager();

		///Destructor for the SoundManager 
		~SoundManager();

    /***** SETTERS / GETTERS *****/
	public:
        /// There is one (and only one) listener for the scene (needed for 3d sounds).  This
        /// game object is usually the player character.
		void setListener(GameObject* gObject);

    /***** CORE Methods *****/
	public:
		///Loads all sound files into memory putting the data in one dictionary and length in another
		int loadAllFilesFromMemory();
		
		/// Update the listener position function for every frame.  Return 0 if you want the application to keep running.
		int update(float dt);
		
	/***** BACKGROUND-MUSIC Methods ****/
	public:
		/// Loads background music.  name is the lower-case filename of 
		/// the sound (minus the path).  Returns 0 if successful, 1 if the sound couldn't be loaded.
		/// [temporary] The background music starts playing in loop mode. 
		int loadBackgroundMusic(std::string res_name); 
		
		/// Stops playing the background music.  This also unloads the music.
		void stopBackgroundMusic();
		
		/// Toggles pause-mode of background music
		void toggleBackgroundMusicPause();

	/***** 3D SOUND Methods *****/
	public:
		/// Gets a Sound iterface for a loaded sound file.  name is the lower-case filename 
		/// of the sound (minus the path).  Returns an irrklang ISound pointer.
		/// The sound is always started paused and tracking is set to true (so we can adjust 
		/// the position).  The caller is responsible for adjusting the 3d position AND
		/// for calling drop when done with the interface.  enableSFX should be true if you want
		/// to allow sound effects (e.g. doppler).
		irrklang::ISound* get3DSoundInterface(std::string res_name, bool playLooped = false, bool enableSFX = true);
		

    /***** ATTRIBUTES *****/
	protected:
		/// A pointer to reference the irrKlang engine
		irrklang::ISoundEngine* mSoundEngine;

		/// The game object that is the listener for scene.  If NULL, 3d sounds
        /// won't sound right.
		GameObject* mListener;

		///A dictionary of ISoundSource pointers for each sound file in the resource path
		/// (currently with .ogg or .wav extensions) with the lower-case filename 
		/// (minus path info) as the key.  Note: irrklang manages this memory.  We just need to call
		/// the drop method of each ISoundSource when shutting down.
		std::map<std::string,irrklang::ISoundSource*> mISoundSources;
		
		/// The currently playing (2d) background music (or NULL if there is none).
		irrklang::ISound* mBackgroundMusic;
	};
}
#endif