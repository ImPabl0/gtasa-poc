#pragma once
#include <Windows.h>
#include <d3d9.h>
#include "log/log.h"
#include <thread>
class Overlay
{
public:
	~Overlay();
	HWND* GetCurrentScreen();
	LPDIRECT3DDEVICE9 GetDevice();
	void RefreshScreen();
	void RunMessageLoop();
	void MessageLoop();
	
private:
	HWND hOverlayWindow;
	LPDIRECT3DDEVICE9 pDevice;
	WNDCLASSEX wc;
	std::thread messageLoopThread;


	void CreateOverlayWindow();
	void CreateDevice();
};

