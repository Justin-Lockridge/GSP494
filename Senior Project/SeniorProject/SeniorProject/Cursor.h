#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <vector>

class Cursor
{
private:
	D3DXVECTOR3	scale, trans, rot;

public:
	Cursor();
	~Cursor();

	D3DXVECTOR2		cursorPos;

	void init();
	void update(float x, float y);
	void render(ID3DXSprite*, IDirect3DTexture9*, D3DXIMAGE_INFO);
};