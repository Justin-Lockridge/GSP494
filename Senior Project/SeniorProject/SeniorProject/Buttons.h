////////////////////////////////////////////////////
// Buttons class
// Used for creating buttons on screen for everything -
//			Units buttons, position on map highllighting,
//			Choosing abilities...etc..
//////////////////////////////////////////////////////

#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")


class Buttons
{
private:
	D3DCOLOR color;
	D3DXVECTOR3 position;

	RECT rect;

	bool highlight;

public:
	int width, height;
	
	Buttons();
	~Buttons();

	void setHighlight(bool);
	void setPosition(float, float);
	void setRect(RECT);
	void setColor(D3DCOLOR);

	D3DXVECTOR3 getPosition();
	RECT getRect(){return rect;}

	bool isHighlighted();
	bool isOn(float, float, int);
};