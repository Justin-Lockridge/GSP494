#pragma once

class BoardSpace{
	int posX, posY, spaceNumber, occupiedBy;
	bool highlighted;
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
};