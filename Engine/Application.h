#pragma once
#include <windows.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <D3Dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include "d3d12Util.h"

struct CommandFrame
{
	ID3D12CommandAllocator* pCommandAllocator = { nullptr };

	void Wait()
	{

	}
};

class Application
{
public:
	static Application* Get();

	bool Init(HINSTANCE hInstance);
	bool InitWindow(HINSTANCE hInstance);
	bool InitDirectX();
	bool Run();

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //��Ϣ������

	HWND m_hWnd;
	HINSTANCE hInstance;
	int m_width;
	int m_height;
private:
	Application(int width, int height);
	~Application() {}
	Application(const Application& other) {};
	Application& operator =(const Application& other) {};

	bool RegisterWindow(HINSTANCE hInstance);
	bool CreateMyWindow(HINSTANCE hInstance);

	void CalculateFrameState();

	class GameTime* gameTime = nullptr; //��ʱ��
	int mFrameCount = 0; //��֡��
	double timeElapsed = 0.f; //���ŵ�ʱ��


	//D3D12
	IDXGIFactory7* pDXGIFactory;
	IDXGIAdapter4* pAdapter = { nullptr };
	ID3D12Device* pDevice;

	uint32_t frame_index = { 0 };
	ID3D12CommandQueue* pCommandQueue = { nullptr };
	CommandFrame command_frame[Engine::frame_buffer_count]{ nullptr };
	ID3D12GraphicsCommandList6* pCommandList = { nullptr };
};

