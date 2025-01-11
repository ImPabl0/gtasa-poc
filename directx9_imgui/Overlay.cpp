#include "Overlay.h"

Overlay::~Overlay()
{

    if (pDevice) {
        pDevice->Release();
    }
    if (hOverlayWindow) {
        DestroyWindow(hOverlayWindow);
    }
}

HWND* Overlay::GetCurrentScreen()
{
	if (!hOverlayWindow) {
		CreateOverlayWindow();
	}
	return &hOverlayWindow;
}

LPDIRECT3DDEVICE9 Overlay::GetDevice()
{
	if (!pDevice) {
		CreateDevice();
	}
	return pDevice;
}
void Overlay::RefreshScreen()
{
    bool updated = UpdateWindow(hOverlayWindow);
    bool showed = ShowWindow(hOverlayWindow, SW_SHOW);
}
void Overlay::RunMessageLoop()
{
    if (!messageLoopThread.joinable()) {
        messageLoopThread = std::thread(&Overlay::MessageLoop, this);
    }

}
void Overlay::MessageLoop()
{
	MSG message;
	while (GetMessage(&message, hOverlayWindow, 0, 0))
	{
        Beep(100, 50);
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}
void Overlay::CreateOverlayWindow()
{
    if (hOverlayWindow) return;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = DefWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "Overlay";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "RegisterClassEx failed", "Error", MB_OK);
        return;
    }

    hOverlayWindow = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        "Overlay",
        "Overlay",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (!hOverlayWindow)
    {
        MessageBox(NULL, "CreateWindow failed", "Error", MB_OK);
        Logger::log("CreateWindow failed");
        return;
    }
    else {
        Logger::log("CreateWindow success");
    }

    // Set the window to be transparent
    SetLayeredWindowAttributes(hOverlayWindow, RGB(0, 0, 0), 0, LWA_COLORKEY);
    //SetLayeredWindowAttributes(hOverlayWindow, 0, 255, LWA_ALPHA);

    ShowWindow(hOverlayWindow, SW_SHOW);
    UpdateWindow(hOverlayWindow);
    Logger::log("Overlay window created and showing");
}


void Overlay::CreateDevice()
{
    if (!hOverlayWindow) {
        CreateOverlayWindow();
    }
    if (!pDevice) {
        LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);
        if (!d3d) {
            MessageBox(NULL, "Direct3DCreate9 failed", "Error", MB_OK);
            return;
        }

        D3DPRESENT_PARAMETERS d3dpp;
        ZeroMemory(&d3dpp, sizeof(d3dpp));
        d3dpp.Windowed = TRUE;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = hOverlayWindow;

        d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; // Use um formato de buffer de back apropriado

        HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hOverlayWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice);
        if (result != D3D_OK) {
            MessageBox(NULL, "CreateDevice failed", "Error", MB_OK);
			Logger::log("CreateDevice failed");
            return;
        }
    }
}
