#pragma once

class th_player;

/**
 * \brief An algorithm that uses game information to control the player 
 * (i.e. the bot's brain)
 */
class th_algorithm
{
protected:
	/**
	 * \brief Pointer to game specific player controller
	 */
	th_player *player;
public:
	/**
	 * \brief Create a th_algorithm
	 * \param player Pointer to player controller
	 */
	th_algorithm(th_player *player) : player(player) {}
	virtual ~th_algorithm() {}
	
	/**
	 * \brief Called when the algorithm is initialized by the player controller
	 */
	virtual void onBegin() {};
	/**
	 * \brief Called every Direct3D frame
	 */
	virtual void onTick() = 0;
	/**
	 * \brief Visualize algorithm functionality by drawing to framebuffer
	 * \param d3dDev Pointer to game's Direct3DDevice
	 */
	virtual void visualize(IDirect3DDevice9* d3dDev){}
	/**
	 * \brief Handle input received from player for algorithm control
	 * \param diKeys Key hold state
	 * \param press Key pressed state
	 */
	virtual void handleInput(const BYTE diKeys[256], const BYTE press[256]) {}
};
