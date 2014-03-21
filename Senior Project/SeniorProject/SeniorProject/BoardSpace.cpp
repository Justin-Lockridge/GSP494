#include "BoardSpace.h"

BoardSpace::BoardSpace(){
	posX		=	0;
	posY		=	0;
	spaceNumber =	0;
	occupiedBy	=	0;
	highlighted	=	false;
};

void BoardSpace::setPosX(int pX){
	posX = pX;
};

int BoardSpace::getPosX(){
	return posX;
};

void BoardSpace::setPosY(int pY){
	posY = pY;
};

int BoardSpace::getPosY(){
	return posY;
};

void BoardSpace::setSpaceNumber(int space){
	spaceNumber = space;
};

int BoardSpace::getSpaceNumber(){
	return spaceNumber;
};

void BoardSpace::setOccupiedBy(int type){
	occupiedBy = type;
};

int BoardSpace::getOccupiedBy(){
	return occupiedBy;
};

void BoardSpace::setHighlight(bool b){
	highlighted = b;
};

bool BoardSpace::isHighlighted(){
	return highlighted;
};