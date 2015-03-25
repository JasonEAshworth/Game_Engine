#ifndef SOUNDCOMPONENT_H
#define SOUNDCOMPONENT_H

#include <stdafx.h>
#include <Component.h>


namespace ssuge
{
	/// This is a component which adds 3d sound-based support to the containing
	/// GameObject.
	class SoundComponent : public Factory<SoundComponent>,public Component
	{
    /***** CONSTRUCTOR / DESTRUCTOR *****/
	public:
		/// Constructor
		SoundComponent(GameObject* owner);

		/// Desctructor
		~SoundComponent();

    /***** GETTERS / SETTERS *****/
	public:
        /// [override] Returns the type of this component
		ComponentType getType() { return SOUND; }

        /// [override, unfinished] Returns the number of bytes used by this component
		virtual int getSize() { return 0; }

		/// Set the Enable/Disable for the component
		void setEnable(bool b) { mEnable = b; }

		/// Get the Enable/Disable for the component
		bool getEnable() { return mEnable; }

    /***** HANDLERS *****/
	public:
		/// [override] Called by the parent game object every frame
        void update(float dT);

    /***** METHODS *****/
	public:
		/// Loads the sound attached to this object.  Pass true for enableSFX if you want to allow
        /// special effects (like doppler)
		void loadSound(std::string res_name, bool looped, bool enableSFX);

		/// The soundcomponent version of execute, inherited from component
		Message execute(const char *operation,  const Message & args);

	/***** ATTRIBUTES *****/
	protected:
		/// pointer to an Irrklang sound object (or NULL) if it hasn't been loaded yet.
		irrklang::ISound* mSound;

		/// Boolean to enable/disable the cameraComponent.
		/// True is enabled, False is disabled.
		/// When disabled, the component should have no functionality.
		bool mEnable;
	};
} 

#endif