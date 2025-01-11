#pragma once
#include "Esp.h"
#include <string>
#include "../log/log.h"
#include "../mem/mem.h"


Esp::Esp(LPDIRECT3DDEVICE9 pDevice)
{
	this->pDevice = pDevice;

	// Inicialize a fonte
	D3DXFONT_DESC fontDesc = {
		24,                   // Altura
		0,                    // Largura
		900,                  // Peso
		0,                    // Nível MIP
		FALSE,                // Itálico
		DEFAULT_CHARSET,      // Charset
		OUT_DEFAULT_PRECIS,   // Precisão de saída
		ANTIALIASED_QUALITY,  // Qualidade
		DEFAULT_PITCH | FF_DONTCARE, // Pitch e família
		TEXT("Arial")         // Nome da fonte
	};
	initPedPoolInfo();
	D3DXCreateLine(pDevice, &pLine);
	D3DXCreateFontIndirect(pDevice, &fontDesc, &pFont);
}

void Esp::initPedPoolInfo() {

	uintptr_t moduleBase = (uintptr_t)GetModuleHandle(NULL);
	pPedPoolInfo = reinterpret_cast<struct CPedPoolInfo*>(*(struct CPedPoolInfo**)(moduleBase + 0x774490));
}

void Esp::drawSkeleton(CPed* ped, D3DCOLOR color)
{
	drawLineSkeleton(ped, BONE_HEAD, BONE_NECK, color);
	drawLineSkeleton(ped, BONE_NECK, BONE_SPINE1, color);
	drawLineSkeleton(ped, BONE_SPINE1, BONE_PELVIS, color);
	drawLineSkeleton(ped, BONE_SPINE1, BONE_RIGHTUPPERTORSO, color);
	drawLineSkeleton(ped, BONE_SPINE1, BONE_LEFTUPPERTORSO, color);
	drawLineSkeleton(ped, BONE_RIGHTUPPERTORSO, BONE_RIGHTSHOULDER, color);
	drawLineSkeleton(ped, BONE_LEFTUPPERTORSO, BONE_LEFTSHOULDER, color);
	drawLineSkeleton(ped, BONE_RIGHTSHOULDER, BONE_RIGHTELBOW, color);
	drawLineSkeleton(ped, BONE_LEFTSHOULDER, BONE_LEFTELBOW, color);
	drawLineSkeleton(ped, BONE_RIGHTELBOW, BONE_RIGHTWRIST, color);
	drawLineSkeleton(ped, BONE_LEFTELBOW, BONE_LEFTWRIST, color);
	drawLineSkeleton(ped, BONE_RIGHTWRIST, BONE_RIGHTHAND, color);
	drawLineSkeleton(ped, BONE_LEFTWRIST, BONE_LEFTHAND, color);
	drawLineSkeleton(ped, BONE_RIGHTHAND, BONE_RIGHTTHUMB, color);
	drawLineSkeleton(ped, BONE_LEFTHAND, BONE_LEFTTHUMB, color);
	drawLineSkeleton(ped, BONE_PELVIS, BONE_LEFTHIP, color);
	drawLineSkeleton(ped, BONE_PELVIS, BONE_RIGHTHIP, color);
	drawLineSkeleton(ped, BONE_LEFTHIP, BONE_LEFTKNEE, color);
	drawLineSkeleton(ped, BONE_RIGHTHIP, BONE_RIGHTKNEE, color);
	drawLineSkeleton(ped, BONE_LEFTKNEE, BONE_LEFTANKLE, color);
	drawLineSkeleton(ped, BONE_RIGHTKNEE, BONE_RIGHTANKLE, color);
	drawLineSkeleton(ped, BONE_LEFTANKLE, BONE_LEFTFOOT, color);
	drawLineSkeleton(ped, BONE_RIGHTANKLE, BONE_RIGHTFOOT, color);
}

void Esp::drawBox2D(CPed* ped)
{
	CPed* currentPed = pPedPoolInfo->FirstCPed;
	Vector3 pedPosition = { ped->pViewMatrix->w1 ,ped->pViewMatrix->w2,ped->pViewMatrix->w3 };
	Vector3 currentPedPosition = currentPed->getCurrentPosition();
	Vector3 headPos, leftShoulderPos, rightShoulderPos, rightFeetPos, leftFeetPos;
	Vector3 head2DPos, leftShoulder2DPos, rightShoulder2DPos, ped2DPos, rightFeet2DPos, leftFeet2DPos;
	ped->GetBonePosition(headPos, BONE_HEAD, false);
	ped->GetBonePosition(leftShoulderPos, BONE_LEFTSHOULDER, false);
	ped->GetBonePosition(rightShoulderPos, BONE_RIGHTSHOULDER, false);
	ped->GetBonePosition(rightFeetPos, BONE_RIGHTFOOT, false);
	ped->GetBonePosition(leftFeetPos, BONE_LEFTFOOT, false);

	if (!(WorldToScreen(&headPos, &head2DPos) && WorldToScreen(&leftFeetPos, &leftFeet2DPos) && WorldToScreen(&rightFeetPos, &rightFeet2DPos) && WorldToScreen(&leftShoulderPos, &leftShoulder2DPos) && WorldToScreen(&rightShoulderPos, &rightShoulder2DPos) && WorldToScreen(&pedPosition, &ped2DPos))) {
		return;
	}


	float height = fabs(head2DPos.y - fmaxf(rightFeet2DPos.y, leftFeet2DPos.y));
	float width = getDistance(&leftShoulderPos, &rightShoulderPos);

	Vector2 lines[] = {
		{head2DPos.x - (height * 0.2),head2DPos.y - (height * 0.12)},
		{head2DPos.x + (height * 0.2),head2DPos.y - (height * 0.12)},
		{ped2DPos.x - (height * 0.2),ped2DPos.y + (height * 0.6)},
		{ped2DPos.x + (height * 0.2),ped2DPos.y + (height * 0.6)},
		{head2DPos.x - (height * 0.2),head2DPos.y - (height * 0.12)},
		{ped2DPos.x - (height * 0.2),ped2DPos.y + (height * 0.6)},
		{head2DPos.x + (height * 0.2),head2DPos.y - (height * 0.12)},
		{ped2DPos.x + (height * 0.2),ped2DPos.y + (height * 0.6)},

	};
	for (int i = 0; i < 8; i += 2)
	{
		drawLine(&lines[i], &lines[i + 1], 1.2f, D3DCOLOR_XRGB(255, 255, 255));
	}
}

void Esp::drawLine(Vector2* from, Vector2* to, float thickness, D3DCOLOR color)
{

	D3DXVECTOR2 line[] = { {from->x,from->y}, {to->x,to->y} };
	pLine->SetWidth(thickness);
	pLine->Begin();
	pLine->Draw(line, 2, color);
	pLine->End();
}

void Esp::drawLineSkeleton(CPed* ped, ePedBones boneFrom, ePedBones boneTo, D3DCOLOR color)
{
	Vector3 pos1, pos2;
	ped->GetBonePosition(pos1, boneFrom, true);
	ped->GetBonePosition(pos2, boneTo, true);
	D3DXVECTOR3 in1 = { pos1.x,pos1.y,pos1.z };
	D3DXVECTOR3 in2 = { pos2.x,pos2.y,pos2.z };
	D3DXVECTOR3 out1, out2;

	if (WorldToScreen(&in1, &out1) && WorldToScreen(&in2, &out2)) {
		Vector2 from = { out1.x ,out1.y };
		Vector2 to = { out2.x,out2.y };
		drawLine(&from, &to, 1.5f, color);
	}
}

float Esp::getDistance(D3DXVECTOR3* pos1, D3DXVECTOR3* pos2)
{
	return sqrt(pow(pos1->x - pos2->x, 2) + pow(pos1->y - pos2->y, 2) + pow(pos1->z - pos2->z, 2));
}

float Esp::getDistance(Vector3* pos1, Vector3* pos2)
{
	return sqrt(pow(pos1->x - pos2->x, 2) + pow(pos1->y - pos2->y, 2) + pow(pos1->z - pos2->z, 2));
}

bool Esp::getViewMatrix(Matrix* matrix)
{
	/*if (!pDevice || !(&matrix)) {
		return false;
	}
	D3DMATRIX d3dMatrix;
	HRESULT result = pDevice->GetTransform(D3DTS_VIEW, &d3dMatrix);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			matrix->m[i][j] = d3dMatrix.m[i][j];
		}
	}
	if (FAILED(result)) {
		return false;
	}*/

	Camera* camera = (Camera*)0xB6F028;
	if (!camera) {
		return false;
	}
	matrix = &camera->vMatrix;

	return true; // Sucesso
}

Esp::~Esp()
{
	if (pFont)
	{
		pFont->Release();
		pFont = NULL;
	}
}

void Esp::drawEsp(float x, float y, float z, float w, float h)
{
	if (!enabled)return;
	D3DXVECTOR3 pos = { x, y, z };
	D3DXVECTOR3 out;
	if (WorldToScreen(&pos, &out))
	{
		float width = w / out.z;
		float height = h / out.z;
		float x = out.x - width / 2;
		float y = out.y - height / 2;
		D3DRECT rect = { x, y, x + width, y + height };
		pDevice->Clear(1, &rect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 0, 0);
	}
	else {
		Logger::log("Coordinate is not visible");
	}
}

void Esp::drawAllEsp()
{
	std::vector<CPed*> peds = GetCPed();
	int totalDrawed = 0;
	
	for (auto ped : peds)
	{
		
		drawBox2D(ped);
		if (drawEspOnPed(ped)) {
			totalDrawed++;
		}
	}
	if (pPedPoolInfo&&IsValidCPed(pPedPoolInfo->FirstCPed)) {
		drawSkeleton(pPedPoolInfo->FirstCPed, D3DCOLOR_XRGB(255, 255, 255));
		drawBox2D(pPedPoolInfo->FirstCPed);
	}

	std::string text = "Total de peds desenhados: " + std::to_string(totalDrawed)+"/"+std::to_string(peds.size());
	drawText(1000, 900, text.c_str(), D3DCOLOR_XRGB(255, 0, 0));
}

void Esp::drawText(float x, float y, float z, const char* text, D3DCOLOR color)
{
	if (!enabled)return;
	D3DXVECTOR3 pos = { x, y, z };
	D3DXVECTOR3 out = { x, y, z };
	if (WorldToScreen(&pos, &out)) {
		RECT rect;
		rect.left = static_cast<LONG>(out.x);
		rect.top = static_cast<LONG>(out.y);
		rect.right = rect.left + 200; // Largura do retângulo de texto
		rect.bottom = rect.top + 50;  // Altura do retângulo de texto

		pFont->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP, color);
	}


}

void Esp::drawText(float x, float y, const char* text, D3DCOLOR color)
{
	if (!enabled)return;
	D3DXVECTOR3 pos = { x, y, 1 };

	RECT rect;
	rect.left = static_cast<LONG>(x);
	rect.top = static_cast<LONG>(y);
	rect.right = rect.left + 200; // Largura do retângulo de texto
	rect.bottom = rect.top + 50;  // Altura do retângulo de texto

	pFont->DrawTextA(NULL, text, -1, &rect, DT_NOCLIP, color);
}



	/** C++-ifyed function 0x71DA00, formerly called by CHudSA::CalcScreenCoors **/

bool Esp::WorldToScreen(D3DXVECTOR3* pos, D3DXVECTOR3* out)
{
	
   // Get the static view matrix as D3DXMATRIX
	D3DXMATRIX m((float*)(0xB6FA2C));

	// Get the static virtual screen (x,y)-sizes
	DWORD* dwLenX = (DWORD*)(0xC17044);
	DWORD* dwLenY = (DWORD*)(0xC17048);

	// Do a transformation
	out->x = pos->x * m._31 + pos->y * m._21 + pos->z * m._11 + m._41;
	out->y = pos->x * m._32 + pos->y * m._22 + pos->z * m._12 + m._42;
	out->z = pos->x * m._33 + pos->y * m._23 + pos->z * m._13 + m._43;

	// Get the correct screen coordinates
	float fRecip = 1.0f / out->z;
	out->x *= fRecip * (*dwLenX);
	out->y *= fRecip * (*dwLenY);
	return true;
}

bool Esp::WorldToScreen(Vector3* pos, Vector3* out)
{
	D3DXVECTOR3 posIn = { pos->x, pos->y, pos->z };
	D3DXVECTOR3 posOut;

	bool result = WorldToScreen(&posIn, &posOut);
	out->x = posOut.x;
	out->y = posOut.y;
	out->z = posOut.z;
	return result;
}

bool Esp::drawEspOnPed(CPed* ped)
{
	if (!enabled)return false;
	if (shouldDrawSkeleton) drawSkeleton(ped, D3DCOLOR_XRGB(255, 255, 255));
	Vector3 pos;
	try
	{
		ped->GetBonePosition(pos, BONE_HEAD, true);
		D3DXVECTOR3 in = { pos.x,pos.y,pos.z };
		D3DXVECTOR3 screenPos;
		if (WorldToScreen(&in, &screenPos)) {
			Vector2 from = { 0,0 };
			Vector2 to = { screenPos.x, screenPos.y };
			drawLine(&from, &to, 1.0f, D3DCOLOR_XRGB(255, 255, 255));
			return true;
		}
		return false;
	}
	catch (const std::exception&)
	{
		Logger::log("Error drawing PED");
		return false;
	}
	return false;
}

std::vector<CPed*> Esp::GetCPed()
{
	std::vector<CPed*> peds;
	if (pPedPoolInfo == nullptr) {
		initPedPoolInfo();
		return peds;
	}

	if (pPedPoolInfo->FirstCPed == NULL || pPedPoolInfo->CurrentCPedNum <= 0)return std::vector<CPed*>();
	for (uint8_t i = 2; i < 140; i++) {
		auto ped = ptr_add(pPedPoolInfo->FirstCPed, 0x7c4 * i);
		auto PedInUse = (*pPedPoolInfo->CPedsInUse)[i];
		if (!(PedInUse > 0x00 && 0x80 > PedInUse))
			continue;
		if (IsValidCPed(ped))
			peds.push_back(ped);
	}

	return peds;
}
