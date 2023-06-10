#ifndef UNICODE
#define UNICODE
#endif 

#include <d3d9.h>
#include <windows.h>

//#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "winmm.lib")

#define internal static; 

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void initD3D(HWND* hwnd);
void cleanUpD3D();

struct WindowContext {
    const wchar_t* name;
    int x, y, width, height;
};

// GLOBAL VARIABLES
LPDIRECT3D9 d3d = NULL;             // DirectX 9 interface
LPDIRECT3DDEVICE9 d3dDevice = NULL;

const int FULLSCREEN_WIDTH = 1920;
const int FULLSCREEN_HEIGHT = 1080;

const int WINDOWED_WIDTH = 800;
const int WINDOWED_HEIGHT = 600;

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1: 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1: 0)


#define WINDOWED_MODE

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Metal Slug Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    WindowContext winCtx = {};

    // Create the window.
    // windowed mode 
#ifdef WINDOWED_MODE 
    winCtx.x = CW_USEDEFAULT;
    winCtx.y = CW_USEDEFAULT;
    winCtx.width = WINDOWED_WIDTH;
    winCtx.height = WINDOWED_HEIGHT;
    winCtx.name = L"Metal Slug";
#else
    winCtx.x = 0;
    winCtx.y = 0;
    winCtx.width = FULLSCREEN_WIDTH;
    winCtx.height = FULLSCREEN_HEIGHT;
    winCtx.name = L"Metal Slug";
#endif 

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        winCtx.name,    // Window text

        // WS_OVERLAPPEDWINDOW,            // Window style
#ifdef WINDOWED_MODE 
        WS_OVERLAPPEDWINDOW,            // Window style
#else 
        WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
#endif
        // Size and position
        winCtx.x,                            // X-position
        winCtx.y,                            // Y-position
        winCtx.width,                            // Width
        winCtx.height,                            // Height

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);

    // Setup d3d9
    initD3D(&hwnd);

    // Run the message loop.
    MSG msg = { };
    bool done = false;
    while (!done)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                done = true;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (d3dDevice == NULL) {
            OutputDebugStringA("D3D Device is NULL");
            return 1;
        }

        d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 0, 255), 1.0f, 0);

        if (d3dDevice->BeginScene()) {
            // TODO: Draw Frame 

            d3dDevice->EndScene();
        }

        // show the frame 
        d3dDevice->Present(NULL, NULL, NULL, NULL);
    }

    cleanUpD3D();

    return 0;
}

void initD3D(HWND* hwnd) {
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    // init some settings for d3d   

#ifdef WINDOWED_MODE 
    d3dpp.Windowed = true;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    /*
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = WINDOWED_WIDTH;
    d3dpp.BackBufferWidth = WINDOWED_HEIGHT;
    d3dpp.hDeviceWindow = *hwnd;
    */

#else 
    d3dpp.Windowed = true; // i don't know why but this works instead of windowed = false 
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = FULLSCREEN_WIDTH;
    d3dpp.BackBufferWidth = FULLSCREEN_HEIGHT;
    d3dpp.hDeviceWindow = *hwnd;
#endif 

    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        *hwnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &d3dDevice);
}

void cleanUpD3D() {
    if (d3dDevice != NULL) {
        d3dDevice->Release();
    }

    if (d3d != NULL) {
        d3d->Release();
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        return 0;
    }
    break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}