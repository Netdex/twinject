#pragma once

/**
 * \brief Detect changes in DirectInput keyboard state representative of keypresses
 */
class keypress_detect
{
	BYTE last_state[256] = {0};
	
public:
	keypress_detect() {}

	/**
	 * \brief Runs key press detection tick on key input frame
	 * \param state State of keyboard
	 * \return Vector of key presses by virtual key code
	 */
	void tick(const BYTE state[256], BYTE pressed[256]);
};