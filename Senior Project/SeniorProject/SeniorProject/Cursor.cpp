#include "Cursor.h"

Cursor::Cursor()
{
}

Cursor::~Cursor()
{}

void Cursor::init()
{
	cursorPos.x = 500;
	cursorPos.y = 300;

	scale = D3DXVECTOR3(0.1f, 0.1f, 0.0f);
	trans = D3DXVECTOR3(cursorPos.x, cursorPos.y, 0.0f);
	rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void Cursor::update(float x, float y)
{
	cursorPos.x += x;
	cursorPos.y += y;

	if(cursorPos.x < 0) cursorPos.x = 0;
	if(cursorPos.y < 0) cursorPos.y = 0;
	if(cursorPos.x > 800) cursorPos.x = 800;
	if(cursorPos.y > 600) cursorPos.y = 600;

	trans.x = cursorPos.x;
	trans.y = cursorPos.y;
}

void Cursor::render(ID3DXSprite *a_sprite, IDirect3DTexture9* a_tex , D3DXIMAGE_INFO a_info)
{
	D3DXMATRIX	transMat, scaleMat, rotMat, worldMat;

	D3DXMatrixIdentity(&transMat);
	D3DXMatrixIdentity(&scaleMat);
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixIdentity(&worldMat);

	D3DXMatrixScaling(&scaleMat, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&transMat, trans.x, trans.y, trans.z);
	D3DXMatrixRotationYawPitchRoll(&rotMat, D3DXToRadian(rot.y), D3DXToRadian(rot.x), D3DXToRadian(rot.z));
	D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
	D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);

	a_sprite->SetTransform(&worldMat);
	a_sprite->Draw(a_tex, 0, &D3DXVECTOR3(a_info.Width * 0.5f, a_info.Height * 0.5f, 0.0f), 0, D3DCOLOR_ARGB(255, 255, 255, 255));
}
