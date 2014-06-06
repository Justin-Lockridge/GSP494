#pragma once

#include "DefinesAndEnums.h"

#include <d3d9.h>
#include <d3dx9.h>

class ClassAbilityAnimator{
	float animationTimer, activeTimer, posX, posY, scaleX, scaleY;
	int type;
	RECT animationRect;
	bool animationActive;
public:
	ClassAbilityAnimator();

	void setClassAbilityAnimation( int ability, float x, float y );
	void updateClassAbilityAnimation( float dt );

	void setAnimationTimer( float t );
	void adjustAnimationTimer( float t );
	float getAnimationTimer();
	
	void setActiveTimer( float t );
	void adjustActiveTimer( float t );
	float getActiveTimer();

	void setPosX( float x );
	void adjustPosX( float x );
	float getPosX();

	void setPosY( float y );
	void adjustPosY( float y );
	float getPosY();

	void setScaleX( float s );
	float getScaleX();

	void setScaleY( float y );
	float getScaleY();

	void setType( int t );
	int getType();

	void setAnimationRect( int top, int left, int right, int bottom  );
	void adjustAnimationRectLeftRight( int leftAdjustment, int rightAdjustment );
	RECT getAnimationRect();

	void setAnimationActive( bool b );
	bool getAnimationActive();
};