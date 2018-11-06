#pragma once


/**
 * \brief Initialize the external D3D9 renderer and window that 
 * IMGUI will use to render debug information onto
 * \return Whether the initialization succeeded or not
 */
bool imgui_window_init();

/**
 * \brief Execute operations for preparing the current frame
 */
bool imgui_window_preframe();

/**
 * \brief Process the message queue and render buffer to screen
 * \return Whether the operation succeeded or not
 * Will return false if the window was closed
 */
bool imgui_window_render();

bool imgui_window_cleanup();