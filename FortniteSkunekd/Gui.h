#pragma once
#include "./Imgui/imgui.h"
#include "./Imgui/imgui_impl_dx9.h"
#include "./Imgui/imgui_impl_win32.h"

#include <d3d9.h>

#pragma comment(lib, "D3d9.lib")
// 


class Gui {
private:
	Gui() = default;
public:
	// idrk man
	static LPDIRECT3D9              g_pD3D;
	static LPDIRECT3DDEVICE9        g_pd3dDevice;
	static UINT                     g_ResizeWidth, g_ResizeHeight;
	static D3DPRESENT_PARAMETERS    g_d3dpp;

	static bool CreateDeviceD3D(HWND hWnd);
	static void CleanupDeviceD3D();
	static void ResetDevice();
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void InitImGui();
	static DWORD WINAPI ImGuiThread(LPVOID);
};