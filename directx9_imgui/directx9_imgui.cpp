#include "directx9_imgui.h"

#include <d3dx9.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "log/log.h"	
#include "min_hook/include/MinHook.h"
#include <cstdio>
#include "features/Esp.h"
#include "Overlay.h"

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

// Variáveis globais
ID3DXFont* pFont = NULL;
HWND hGameWindow = NULL;
HWND hExternalOverlay = NULL;
Overlay* overlay = NULL;
Esp* internalEsp = NULL;
Esp* externalEsp = NULL;
std::thread overlayThread;
std::thread messageLoopThread;

// Função para inicializar a fonte
void InitFont(LPDIRECT3DDEVICE9 pD3D9)
{
	D3DXFONT_DESC fontDesc = {
		24,                   // Altura
		0,                    // Largura
		400,                  // Peso
		0,                    // Nível MIP
		FALSE,                // Itálico
		DEFAULT_CHARSET,      // Charset
		OUT_DEFAULT_PRECIS,   // Precisão de saída
		ANTIALIASED_QUALITY,  // Qualidade
		DEFAULT_PITCH | FF_DONTCARE, // Pitch e família
		TEXT("Arial")         // Nome da fonte
	};

	D3DXCreateFontIndirect(pD3D9, &fontDesc, &pFont);
}

void DrawExternalEsp();

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	char windowTitle[256];
	GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
	if (hGameWindow != NULL)
	{
		return true;
	}
	if (strstr(windowTitle, "GTA: San Andreas") != NULL || strstr(windowTitle, "GTA:SA:MP") != NULL)
	{
		hGameWindow = hwnd;
		return FALSE;
	}
	return true;
}

HWND FindGameWindow()
{
	EnumWindows(EnumWindowsProc, 0);
	return hGameWindow;
}

void CheckDetachHotKey() {
	if (GetAsyncKeyState(VK_END) & 1) {
		FreeConsole();
		FreeLibraryAndExitThread(hModule, 0);
	}
}

void CheckEspHotKey() {
	if ((GetAsyncKeyState(VK_F2) & 1) && (internalEsp != NULL||externalEsp != NULL)) {
		std::string currentStatus = !internalEsp->enabled ? "enabled" : "disabled";
		Logger::log("ESP " + currentStatus);
		internalEsp->enabled = !internalEsp->enabled;
	}
}

void CheckMenuHotKey() {
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		ImGui::GetIO().MouseDrawCursor = !ImGui::GetIO().MouseDrawCursor;
	}
}

void InitConsole() {
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);
}

void InitExternalOverlay() {
	Logger::log("Creating Overlay");
	try
	{
		overlay = new Overlay();
		hExternalOverlay = *overlay->GetCurrentScreen();
		LPDIRECT3DDEVICE9 direct3d9Device = overlay->GetDevice();
		externalEsp = new Esp(direct3d9Device);
		externalEsp->enabled = true;
		Logger::log("Overlay created successfully");

		//overlayThread = std::thread(DrawExternalEsp);
		//overlayThread.detach();

	}
	catch (const std::exception&)
	{
		Logger::log("Error creating overlay");
	}
}

void DrawExternalEsp() {
	while (true) {
		LPDIRECT3DDEVICE9 pD3D9 = overlay->GetDevice();
		CheckEspHotKey();
		pD3D9->BeginScene();
		pD3D9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		externalEsp->drawAllEsp();
		pD3D9->EndScene();
		pD3D9->Present(NULL, NULL, NULL, NULL);
	}
}

DWORD WINAPI DirectXInit(__in  LPVOID lpParameter)
{
	InitConsole();
	InitExternalOverlay();
	Logger::log("DirectXInit called");
	if (FindGameWindow()) {
		Logger::log("Game window found!!");
	}
	else {
		Logger::log("Game window not found");
		return 0;
	}
	while (GetModuleHandle("d3d9.dll") == 0)
	{
		Sleep(100);
	}

	LPDIRECT3D9 d3d = NULL;
	LPDIRECT3DDEVICE9 d3ddev = NULL;

	HWND tmpWnd = CreateWindowA("BUTTON", "Temp Window", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, hModule, NULL);
	if (tmpWnd == NULL)
	{
		return 0;
	}

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL)
	{
		DestroyWindow(tmpWnd);
		return 0;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = tmpWnd;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tmpWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if (result != D3D_OK)
	{
		Logger::log("CreateDevice failed");
		d3d->Release();
		DestroyWindow(tmpWnd);
		return 0;
	}

#if defined _M_X64
	DWORD64* dVtable = (DWORD64*)d3ddev;
	dVtable = (DWORD64*)dVtable[0];
#elif defined _M_IX86
	DWORD* dVtable = (DWORD*)d3ddev;
	dVtable = (DWORD*)dVtable[0];
#endif

	EndScene_orig = (EndScene)dVtable[42];
	DrawIndexedPrimitive_orig = (DrawIndexedPrimitive)dVtable[82];
	Reset_orig = (Reset)dVtable[16];

	if (MH_Initialize() != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)dVtable[42], &EndScene_hook, reinterpret_cast<void**>(&EndScene_orig)) != MH_OK) { Logger::log("Failed to hook endscene"); return 1; }
	if (MH_EnableHook((DWORD_PTR*)dVtable[42]) != MH_OK) {
		Logger::log("Failed to enable endscene hook");
		return 1;
	}
	if (MH_CreateHook((DWORD_PTR*)dVtable[82], &DrawIndexedPrimitive_hook, reinterpret_cast<void**>(&DrawIndexedPrimitive_orig)) != MH_OK) {
		Logger::log("Failed to hook drawindexedprimitive");
		return 1;
	}
	if (MH_EnableHook((DWORD_PTR*)dVtable[82]) != MH_OK) {
		Logger::log("Failed to enable drawindexedprimitive hook");
		return 1;
	}
	if (MH_CreateHook((DWORD_PTR*)dVtable[16], &Reset_hook, reinterpret_cast<void**>(&Reset_orig)) != MH_OK) {
		Logger::log("Failed to hook reset");
		return 1;
	}
	if (MH_EnableHook((DWORD_PTR*)dVtable[16]) != MH_OK) {
		Logger::log("Failed to enable reset hook");
		return 1;
	}

	Logger::log("Hooks created successfully!");
	//overlay->MessageLoop();
#if defined _M_X64
	oWndProc = (WNDPROC)SetWindowLongPtr(hGameWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);
#elif defined _M_IX86
	oWndProc = (WNDPROC)SetWindowLongPtr(hGameWindow, GWL_WNDPROC, (LONG_PTR)WndProc);
#endif

	d3ddev->Release();
	d3d->Release();
	DestroyWindow(tmpWnd);

	return 1;
}

HRESULT APIENTRY DrawIndexedPrimitive_hook(LPDIRECT3DDEVICE9 pD3D9, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	return DrawIndexedPrimitive_orig(pD3D9, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

bool menu;

// Função para desenhar texto
void DrawText(LPDIRECT3DDEVICE9 pD3D9, const char* text, int x, int y, D3DCOLOR color)
{
	if (pFont)
	{
		RECT rect;
		SetRect(&rect, x, y, x + 200, y + 50); // Define a área de desenho do texto
		pFont->DrawText(NULL, text, -1, &rect, DT_NOCLIP, color);
	}
}

HRESULT APIENTRY EndScene_hook(LPDIRECT3DDEVICE9 pD3D9) {
	static bool init = true;
	if (init) {
		if (externalEsp) {
			internalEsp = new Esp(pD3D9);
			externalEsp->SetGameDevice(pD3D9);
			Logger::log("pGameDevice setted");
		}
		init = false;
	}
	CheckEspHotKey();
	internalEsp->drawAllEsp();
	return EndScene_orig(pD3D9);
}

HRESULT APIENTRY Reset_hook(LPDIRECT3DDEVICE9 pD3D9, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	HRESULT ResetReturn = Reset_orig(pD3D9, pPresentationParameters);
	return ResetReturn;
}

void c_imgui_halt(void)
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}