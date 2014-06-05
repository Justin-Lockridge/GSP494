#include "BoardSpace.h"

BoardSpace::BoardSpace()
{
	posX						=	0;
	posY						=	0;
	spaceNumber					=	0;
	occupiedBy					=	0;
	highlighted					=	false;
	unitCanTakeAction			=	false;
	animationTimer				=	0;
	unitAnimationRect.top		=	0;
	unitAnimationRect.left		=	0;
	unitAnimationRect.bottom	=	0;
	unitAnimationRect.right		=	0;
};

void BoardSpace::setPosX(float pX)
{
	posX = pX;
};

float BoardSpace::getPosX()
{
	return posX;
};

void BoardSpace::setPosY(float pY)
{
	posY = pY;
};

float BoardSpace::getPosY(){
	return posY;
};

void BoardSpace::setSpaceNumber(int space){
	spaceNumber = space;
};

int BoardSpace::getSpaceNumber()
{
	return spaceNumber;
};

void BoardSpace::setOccupiedBy(int type)
{
	occupiedBy = type;
};

int BoardSpace::getOccupiedBy()
{
	return occupiedBy;
};

void BoardSpace::setHighlight(bool b)
{
	highlighted = b;
};

bool BoardSpace::isHighlighted()
{
	return highlighted;
};

void BoardSpace::setUnitCanTakeAction(bool b)
{
	unitCanTakeAction  = b;
};

bool BoardSpace::canUnitTakeAction()
{
	return unitCanTakeAction;
};

void BoardSpace::setAnimationTimer(double time)
{
	animationTimer = time;
};

void BoardSpace::adjustAnimationTimer(double time)
{
	animationTimer += time;
};

double BoardSpace::getAnimationTimer()
{
	return animationTimer;
};

void BoardSpace::setAnimationRect(int t, int l, int r, int b){
	unitAnimationRect.top		=	t;
	unitAnimationRect.left		=	l;
	unitAnimationRect.right		=	r;
	unitAnimationRect.bottom	=	b;
};

RECT BoardSpace::getAnimationRect(){
	return unitAnimationRect;
};

void BoardSpace::setWhoUnitBelongsTo(int player){
	unitBelongsTo = player;
};

int BoardSpace::getWhoUnitBelongsTo(){
	return unitBelongsTo;
};