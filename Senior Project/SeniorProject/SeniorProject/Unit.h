#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Direct3D headers && includes
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include "DefinesAndEnums.h"

class Unit
{
	int type, damage, currentHealth, maxHealth, whoUnitBelongsTo, speed, state;
	float posX, posY, animationTimer;

	RECT unitRect;
	bool facingRight, isActive, unitCanTakeAction, isRanged, unitCanUseAbility;
public:
	bool occupied;
	Unit();

	void	addUnit(int t, int playerNumber);
	void	removeUnit();
	void	swapUnit( Unit copy );
	void	updateAnimations( float dt );
	void	setNewColumn( int c );
	//////////////////////////////////////////////////////////
	//  INFO:  Setters, getters, and adjustments
	void	setType(int t);
	int		getType();
	void	setDamage(int d);
	int		getDamage();
	void	setCurrentHealth(int health);
	void	adjustCurrentHealth(int health);
	int		getCurrentHealth();
	void	setMaxHealth(int health);
	int		getMaxHealth();
	void	setWhoUnitBelongsTo( int player );
	int		getWhoUnitBelongsTo();
	void	setSpeed( int s );
	int		getSpeed();
	void	setState( int s );
	int		getState();
	void	setPosX(float x);
	void	adjustPosX(float x);
	float	getPosX();
	void	setPosY(float y);
	void	adjustPosY(float y);
	float	getPosY();
	void	setUnitRect(int top, int left, int right, int bottom);
	RECT	getUnitRect();
	void	setFacingRight(bool b);
	bool	isFacingRight();
	void	setIsActive(bool b);
	bool	checkIfActive();
	void	setUnitCanTakeAction(bool b);
	bool	canUnitTakeAction();
	void	setRanged(bool b);
	bool	checkIfRanged();
	void	setUnitCanUseAbility( bool b );
	bool	canUnitUseAbility();
};