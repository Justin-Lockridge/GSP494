#include "Buttons.h"

Buttons::Buttons()
{
	highlight = false;
	position.x = 0;
	position.y = 0;
	position.z = 0;
}

Buttons::~Buttons()
{
}

void Buttons::setHighlight(bool thisBool)
{
	highlight = thisBool;
}

void Buttons::setPosition(float x, float y)
{
	position.x = x;
	position.y = y;
}

void Buttons::setRect(RECT thisRect)
{
	rect.top = thisRect.top;
	rect.left = thisRect.left;
	rect.bottom = thisRect.bottom;
	rect.right = thisRect.right;

	width = rect.right - rect.left;
	height = rect.bottom -rect.top;
}

void Buttons::setColor(D3DCOLOR thisColor)
{
	color = thisColor;
}

D3DXVECTOR3 Buttons::getPosition()
{
	return position;
}

D3DCOLOR Buttons::getColor()
{
	return color;
}

bool Buttons::isHighlighted()
{
	return highlight;
}

bool Buttons::isOn(float x, float y, int scale)
{
	return (x < (position.x + (width / scale)) && x > (position.x -  (width / scale)) &&
		y < (position.y + (height / scale)) && y > (position.y -  (height / scale)));
}

