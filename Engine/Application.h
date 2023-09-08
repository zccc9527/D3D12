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
	ID3D12CommandAllocator* pCommandAllocator { nullptr };
	uint64_t fenceValue { 0 };
	void Wait(HANDLE fenceEvent, ID3D12Fence* fence)
	{
		assert(fence && fenceEvent);
		if (fence->GetCompletedValue() < fenceValue)
		{
			DXCall(fence->SetEventOnCompletion(fenceValue, fenceEvent));
			WaitForSingleObject(fenceEvent, INFINITE);
		}
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
	void Render();

	void CreateCommandObject(D3D12_COMMAND_LIST_TYPE CommandListType);
	void BeginFrame();
	void EndFrame();


	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); //消息处理函数

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

	class GameTime* gameTime = nullptr; //计时类
	int mFrameCount = 0; //总帧数
	double timeElapsed = 0.f; //流逝的时间


	//D3D12
	IDXGIFactory7* pDXGIFactory;
	IDXGIAdapter4* pAdapter{ nullptr };
	ID3D12Device* pDevice;

	uint32_t frame_index{ 0 };
	ID3D12CommandQueue* pCommandQueue{ nullptr };
	CommandFrame command_frame[Engine::frame_buffer_count]{ nullptr };
	ID3D12GraphicsCommandList6* pCommandList{ nullptr };

	ID3D12Fence* pFence{ nullptr };
	uint64_t fenceValue{ 0 };

	HANDLE fenceEvent { nullptr };
};

