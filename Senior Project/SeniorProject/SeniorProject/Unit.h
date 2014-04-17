#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Direct3D headers && includes
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include "DefinesAndEnums.h"

class Unit{
	int type, damage, currentHealth, maxHealth, whoUnitBelongsTo;
	float posX, posY, animationTimer;

	RECT unitRect;
	bool facingRight, isActive, unitCanTakeAction;
public:
	Unit();

	void	addUnit(int t, int playerNumber);
	void	updateAnimations( float dt );
	
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
};