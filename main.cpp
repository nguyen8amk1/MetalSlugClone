#include<Windows.h>
#include "SDL2BackEnd.cpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	return SDL2Platform::Game().run();
}
