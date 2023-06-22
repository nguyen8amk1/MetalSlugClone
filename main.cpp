#include<Windows.h>
#include "SDL2BackEnd.cpp"
#include "SDL2SandBox.cpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	return SDL2Platform::Game().run();
	//return SDL2Testing::SandBox().run();
}
