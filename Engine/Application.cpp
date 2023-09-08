#include "Application.h"
#include "GameTime.h"
#include <string>

Application* Application::Get()
{
	static Application app(800, 600);
	return &app;
}

bool Application::Init(HINSTANCE hInstance)
{
	if (!InitWindow(hInstance))
	{
		return false;
	}

	if (!InitDirectX())
	{
		return false;
	}

	return true;
}

bool Application::InitWindow(HINSTANCE hInstance)
{
	if (!RegisterWindow(hInstance))
	{
		return false;
	}
	if (!CreateMyWindow(hInstance))
	{
		return false;
	}
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return true;
}

bool Application::InitDirectX()
{
	//创建工厂和设备
	DXCall(CreateDXGIFactory2(0, IID_PPV_ARGS(&pDXGIFactory)));
	DXCall(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&pDevice)));
	NAME_D3D12_OBJECT(pDevice, L"Main Device");

	//创建命令队列，命令列表，命令分配器
	CreateCommandObject(D3D12_COMMAND_LIST_TYPE_DIRECT);

	return true;
}

bool Application::Run()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); //翻译消息
			DispatchMessage(&msg);  //转发给消息处理函数
		}
		else
		{
			if (gameTime)
			{
				gameTime->Tick();
				CalculateFrameState();
			}
		}
	}

	return (int)msg.wParam;
}

void Application::CreateCommandObject(D3D12_COMMAND_LIST_TYPE CommandListType)
{
	//创建命令队列，需要一个描述结构来定义要什么样的命令队列
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.NodeMask = 0;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Type = CommandListType;  //命令列表的类型
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	DXCall(pDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&pCommandQueue)));
	NAME_D3D12_OBJECT(pCommandQueue, CommandListType == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"Direct Command Queue" : CommandListType == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command Queue" : L"Command Queue");

	for (uint32_t i = 0; i < Engine::frame_buffer_count; i++)
	{
		//创建命令分配器
		CommandFrame& frame{ command_frame[i] };
		DXCall(pDevice->CreateCommandAllocator(CommandListType, IID_PPV_ARGS(&frame.pCommandAllocator)));
		NAME_D3D12_OBJECT_Indexed(frame.pCommandAllocator, i, CommandListType == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"Direct Command Allocator" : CommandListType == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command Allocator" : L"Command Allocator");
	}

	//创建命令列表
	DXCall(pDevice->CreateCommandList(
		0, //掩码值为0，单GPU
		CommandListType, //命令列表类型
		command_frame[0].pCommandAllocator,	//命令分配器指针
		nullptr,						//渲染流水线状态对象PSO(pipeline state object),这里不绘制为nullptr
		IID_PPV_ARGS(&pCommandList)
	));
	NAME_D3D12_OBJECT(pCommandList, CommandListType == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"Direct Command List" : CommandListType == D3D12_COMMAND_LIST_TYPE_COMPUTE ? L"Compute Command List" : L"Command List");

	//首先需要重置命令列表，因为在第一次引用命令列表时需要对它进行重置，而重置前必须先关闭
	DXCall(pCommandList->Close());
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Application::Get()->WndProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//创建时
	case WM_CREATE:
	{
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Application::Application(int width, int height)
{
	m_width = width;
	m_height = height;

	gameTime = new GameTime();
}

bool Application::RegisterWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MyWindowClass";
	wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	this->hInstance = hInstance;

	ATOM WindowId = RegisterClassEx(&wcex);
	if (!WindowId)
	{
		MessageBox(NULL, L"注册窗口失败!", L"Error", MB_OK | MB_ICONSTOP);
		return false;
	}
	return true;
}

bool Application::CreateMyWindow(HINSTANCE hInstance)
{
	RECT R = { 0, 0, m_width, m_height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"MyWindowClass", L"Engine", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		MessageBox(NULL, L"创建窗口失败!", L"Error", MB_OK | MB_ICONSTOP);
		return false;
	}
	m_hWnd = hWnd;
	return true;
}

void Application::CalculateFrameState()
{
	mFrameCount++;

	if (gameTime && gameTime->GetTotalTime() - timeElapsed >= 1.0f)
	{
		float fps = float(mFrameCount); //每秒多少帧
		float mspf = 1000.f / fps; //每帧多少毫秒

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);
		std::wstring windowTitle = L"D3D12    fsp:" + fpsStr + L"    mspf:" + mspfStr;
		SetWindowText(m_hWnd, windowTitle.c_str());
		mFrameCount = 0;
		timeElapsed += 1.0f;
	}
}
