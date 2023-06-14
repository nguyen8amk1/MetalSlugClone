#ifndef UNICODE
#define UNICODE
#endif 

#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <cassert>
#include <time.h>

#ifdef DIRECTX
// Prototypes
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void initD3D(HWND* hwnd);
void cleanUpD3D();
void d3dGameProcessing(HWND hwnd);
LPDIRECT3DTEXTURE9 loadTexture(char* filename, D3DCOLOR transColor);

struct WindowContext {
    const wchar_t* name;
    int x, y, width, height;
};

struct Sprite {
    int x, y;
    int width, height;
    int moveX, moveY;
    int curFrame, lastFrame;
    int animDelay, animCount;
};

// GLOBAL VARIABLES
LPDIRECT3D9 g_d3d = NULL;             // DirectX 9 interface
LPDIRECT3DDEVICE9 g_d3dDevice = NULL;

const int g_FULLSCREEN_WIDTH = 1920;
const int g_FULLSCREEN_HEIGHT = 1080;

const int g_WINDOWED_WIDTH = 800;
const int g_WINDOWED_HEIGHT = 600;

#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) * 0x8000) ? 1: 0)
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0: 1)


WindowContext g_winCtx = {};

LPDIRECT3DTEXTURE9 meo[8];
Sprite tempMeo;
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPD3DXSPRITE spriteHandler;

// timing var  
long start;

#define WINDOWED_MODE
int g_bytesPerPixel = 4;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Metal Slug Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);


    // Create the window.
    // windowed mode 
#ifdef WINDOWED_MODE 
    g_winCtx.x = CW_USEDEFAULT;
    g_winCtx.y = CW_USEDEFAULT;
    g_winCtx.width = g_WINDOWED_WIDTH;
    g_winCtx.height = g_WINDOWED_HEIGHT;
    g_winCtx.name = L"Metal Slug";
#else
    g_winCtx.x = 0;
    g_winCtx.y = 0;
    g_winCtx.width = g_FULLSCREEN_WIDTH;
    g_winCtx.height = g_FULLSCREEN_HEIGHT;
    g_winCtx.name = L"Metal Slug";
#endif 

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        g_winCtx.name,    // Window text

        // WS_OVERLAPPEDWINDOW,            // Window style
#ifdef WINDOWED_MODE 
        WS_OVERLAPPEDWINDOW,            // Window style
#else 
        WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
#endif
        // Size and position
        g_winCtx.x,                            // X-position
        g_winCtx.y,                            // Y-position
        g_winCtx.width,                            // Width
        g_winCtx.height,                            // Height

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
    UpdateWindow(hwnd);

    // Setup d3d9
    initD3D(&hwnd);

    // Run the message loop.
    MSG msg = { };
    bool done = false;


    // Create a surface 
    /*
#define SURFACE_WIDTH  200
#define SURFACE_HEIGHT 200

    int bitmapWidth = SURFACE_WIDTH;
    int bitmapHeight = SURFACE_HEIGHT;
    
    RECT rect; 
	rect.left = 0; 
	rect.right = SURFACE_WIDTH;
	rect.top = 0;
	rect.bottom = SURFACE_HEIGHT;
    LPDIRECT3DSURFACE9 surface = NULL;
    g_d3dDevice->CreateOffscreenPlainSurface(SURFACE_WIDTH, SURFACE_WIDTH, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &surface, NULL);
    */

    /*
    // @StartTest: Loading image from file 
    int result = D3DXLoadSurfaceFromFileA(surface, NULL, NULL, "assets/imgs/sprites_cat_running.png", NULL, D3DX_DEFAULT, 0, NULL);
    assert(result == D3D_OK);
    // @EndTest
    */

    // @StartTest: Animation  

    // TODO:

    // @EndTest 


    // TODO: Allocate a block of memory for the bitmap  
    /*
    int bitmapSize = SURFACE_HEIGHT * SURFACE_WIDTH * g_bytesPerPixel;
    unsigned char *p_bitmap = (unsigned char *)malloc(bitmapSize);
    assert(p_bitmap != NULL);
	memset(p_bitmap, 150, bitmapSize);
    */

    /*
    // @StartTest
    // TODO: fill a little small blue circle inside our little gray square   
    int littleSquareWidth = 50;
    int littleSquareHeight = 50;
    int littleSquareX = 10;
    int littleSquareY= 10;

    for (int y = littleSquareY; y < littleSquareY + littleSquareHeight; y++) {
        for (int x = littleSquareX; x < littleSquareX + littleSquareWidth; x++) {
            int index = (y * bitmapWidth + x) * g_bytesPerPixel;
            p_bitmap[index + 0] = 255;      // Blue component
            p_bitmap[index + 1] = 0;        // Green component
            p_bitmap[index + 2] = 0;        // Red component
            p_bitmap[index + 3] = 255;      // Unused padding component
        }
    }
    // @EndTest
    */

    // @StartTest: 
    tempMeo.x = 0;
    tempMeo.y = 0;
    tempMeo.width = 120;
    tempMeo.height = 90;
    tempMeo.curFrame = 0;
    tempMeo.lastFrame = 7;
    tempMeo.animDelay = 1;
    tempMeo.animCount = 0;
    tempMeo.moveX = 8;
    tempMeo.moveY = 0;
    // @EndTest


    char s[100];
    while (!done)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                done = true;

            TranslateMessage(&msg);
            DispatchMessage(&msg);

            long delta = GetTickCount() - start;

            if (delta >= 30) {
                start = GetTickCount();
				d3dGameProcessing(hwnd);

                //wsprintfA(s, "delta: %ld\n", delta);
                //OutputDebugStringA(s);
            }
        }
    }

    //free(p_bitmap);
    cleanUpD3D();

    return 0;
}

int i = 0;
void d3dGameProcessing(HWND hwnd) {
    // checking key input
    if (KEY_UP(VK_ESCAPE)) {
        PostMessage(hwnd, WM_DESTROY, 0, 0);
    }

    /*
    if (KEY_DOWN(VK_RETURN)) {
        OutputDebugStringA("Enter\n");
    }
    */


    if (g_d3dDevice == NULL) {
        OutputDebugStringA("D3D Device is NULL");
        return;
    }

    g_d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 0, 255), 1.0f, 0);

    // This is the rendering part 
    if (g_d3dDevice->BeginScene()) {

        /*
        // @StartTest
        D3DLOCKED_RECT lockedRect;
        surface->LockRect(&lockedRect, nullptr, 0);

        // Copy data into the locked surface
        unsigned char* pDest = static_cast<unsigned char*>(lockedRect.pBits);
        const unsigned char* pSrc = p_bitmap;

        for (int y = 0; y < bitmapHeight; ++y)
        {
            memcpy(pDest, pSrc, bitmapWidth * g_bytesPerPixel);
            pDest += lockedRect.Pitch;
            pSrc += bitmapWidth * g_bytesPerPixel;
        }

        surface->UnlockRect();

        g_d3dDevice->StretchRect(surface, NULL, backbuffer, &rect, D3DTEXF_NONE);
        // @EndTest
        */

        //g_d3dDevice->StretchRect(surface, NULL, backbuffer, &rect, D3DTEXF_NONE);
        tempMeo.x += tempMeo.moveX;
        tempMeo.y += tempMeo.moveY;

        char s[100];
		wsprintfA(s, "tempMeo.x: %d, win width: %d\n", tempMeo.x, g_winCtx.width - tempMeo.width);
		OutputDebugStringA(s);
        if (tempMeo.x >= (g_winCtx.width - tempMeo.width)) {
            tempMeo.x = 0;
        }

        if (tempMeo.y >= (g_winCtx.height - tempMeo.height)) {
            tempMeo.y = 0;
        }


        // @StartTest: animation 
        if (++tempMeo.animCount > tempMeo.animDelay) {
            tempMeo.animCount = 0;
            if (++tempMeo.curFrame > tempMeo.lastFrame) {
                tempMeo.curFrame = 0;
            }
        }
        // @EndTest 
        /*
        RECT rect = {};
        rect.top = 0;
        rect.left = 0;
        rect.bottom = 120;
        rect.right = 90;
        */

        spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);

        // All sprite drawing should be done here 
        D3DXVECTOR3 position = {(FLOAT)tempMeo.x, (FLOAT)tempMeo.y, 0};
		spriteHandler->Draw(meo[tempMeo.curFrame], NULL, NULL, &position, D3DCOLOR_XRGB(255, 255, 255));

        spriteHandler->End();


        // stop rendering
        g_d3dDevice->EndScene();
        i = ++i % 8;
    }

    // show the frame 
    g_d3dDevice->Present(NULL, NULL, NULL, NULL);
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
    size of mode 
    d3dpp.BackBufferWidth = WINDOWED_WIDTH;
    d3dpp.BackBufferWidth = WINDOWED_HEIGHT;
    d3dpp.hDeviceWindow = *hwnd;
    */

#else 
    d3dpp.Windowed = false; // @Bug: i don't know why but this works instead of windowed = false 
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = g_winCtx.width;
    d3dpp.BackBufferWidth = g_winCtx.height;
    d3dpp.hDeviceWindow = *hwnd;
#endif 

    g_d3d = Direct3DCreate9(D3D_SDK_VERSION);
    g_d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        *hwnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &g_d3dDevice);
    // Get back buffer
    g_d3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer); 

    // @StartTest: animation init 
    start = GetTickCount();

    srand(time(NULL));
    int result = D3DXCreateSprite(g_d3dDevice, &spriteHandler);
    assert(result == D3D_OK);

	char s[100];
    // loading sprites 
    for (int i = 0; i < 8; i++) {
        // TODO: get the individual sprites from a single image
        // but for now i'm just gonna cut it with my hand using PTS 
        wsprintfA(s, "assets/imgs/testCat/tile00%d.png", i);
        meo[i] = loadTexture(s, D3DCOLOR_XRGB(255, 0, 255));
    }

    // @EndTest
}

LPDIRECT3DTEXTURE9 loadTexture(char* filename, D3DCOLOR transColor) {
    LPDIRECT3DTEXTURE9 texture = NULL; 
    D3DXIMAGE_INFO info;

    HRESULT result = D3DXGetImageInfoFromFileA(filename, &info);
    assert(result == D3D_OK);

    result = D3DXCreateTextureFromFileExA(
        g_d3dDevice, 
        filename, 
        info.Width, 
        info.Height, 
        1, 
        D3DPOOL_DEFAULT, 
        D3DFMT_UNKNOWN, 
        D3DPOOL_DEFAULT, 
        D3DX_DEFAULT, 
        D3DX_DEFAULT, 
        transColor, &info, NULL, &texture);

    assert(result == D3D_OK);

    return texture;
}

void cleanUpD3D() {
    if (g_d3dDevice != NULL) {
        g_d3dDevice->Release();
    }

    if (g_d3d != NULL) {
        g_d3d->Release();
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
#endif 