#include <windows.h>
#include <iostream>
#include "Application.h"

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	AllocConsole();
	SetConsoleTitle(L"Output");
	FILE* tempFile = nullptr;
	freopen_s(&tempFile, "conin$", "r+t", stdin);
	freopen_s(&tempFile, "conout$", "w+t", stdout);

	Application* app = Application::Get();
	if (!app->Init(hInstance))
	{
		MessageBox(app->m_hWnd, L"´°¿Ú³õÊ¼»¯Ê§°Ü!", L"Error", MB_OK | MB_ICONSTOP);
		return 0;
	}
	app->Run();

	return 1;
}