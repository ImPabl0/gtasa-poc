#pragma once
#include <d3dx9.h>
#include "../Classes.h"
#include <vector>
class Esp
{
public:
	Esp(LPDIRECT3DDEVICE9 pDevice);
	~Esp();
	bool enabled = false;
	bool shouldDrawSkeleton = true;
	bool souldDrawBox = true;
	void drawEsp(float x, float y, float z, float w, float h);
	void drawAllEsp();
	void drawText(float x, float y, float z, const char* text, D3DCOLOR color);
	void drawText(float x, float y, const char* text, D3DCOLOR color);
	inline void SetGameDevice(LPDIRECT3DDEVICE9 pGameDevice) { this->pGameDevice = pGameDevice; }
private:
	LPDIRECT3DDEVICE9 pDevice;
	LPDIRECT3DDEVICE9 pGameDevice;
	bool WorldToScreen(D3DXVECTOR3* pos, D3DXVECTOR3* out);
	bool WorldToScreen(Vector3* pos, Vector3* out);
	bool drawEspOnPed(CPed* ped);
	void initPedPoolInfo();
	void drawSkeleton(CPed* ped, D3DCOLOR color);
	void drawBox2D(CPed* ped);
	void drawLine(Vector2* from, Vector2* to, float thickness, D3DCOLOR color);
	void drawLineSkeleton(CPed* ped, ePedBones boneFrom, ePedBones boneTo, D3DCOLOR color);
	float getDistance(D3DXVECTOR3* pos1, D3DXVECTOR3* pos2);
	float getDistance(Vector3* pos1, Vector3* pos2);
	bool getViewMatrix(Matrix* matrix);
	ID3DXFont* pFont;
	LPD3DXLINE pLine;
	std::vector<CPed*> GetCPed();
	CPedPoolInfo* pPedPoolInfo;
	inline bool IsValidCPed(CPed* ped) noexcept {
		return (ped && ped->pViewMatrix && ped->pViewMatrix->w1 != 0.0f&& ped->Health>0.0f);
	}
};

