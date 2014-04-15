#pragma once
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Direct3D headers && includes
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class BoardSpace{
	int posX, posY, spaceNumber, occupiedBy;
	bool highlighted, unitCanTakeAction;
	float animationTimer;
	int unitBelongsTo;
	//////////////////////////////////////////
	//  INFO:  Each game space holds the rect
	//			for drawing the occupied unit
	RECT unitAnimationRect;
public:
	BoardSpace();

	void setPosX(int pX);
	int  getPosX();

	void setPosY(int pY);
	int  getPosY();

	void setSpaceNumber(int space);
	int  getSpaceNumber();

	void setOccupiedBy(int type);
	int  getOccupiedBy();

	void setHighlight(bool b);
	bool isHighlighted();

	void setUnitCanTakeAction(bool b);
	bool canUnitTakeAction();

	void   setAnimationTimer(double time);
	void   adjustAnimationTimer(double time);
	float getAnimationTimer();

	void setAnimationRect(int t, int l, int r, int b);
	RECT getAnimationRect();

	void setWhoUnitBelongsTo(int player);
	int  getWhoUnitBelongsTo();
};