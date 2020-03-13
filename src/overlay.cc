// https://github.com/ocornut/imgui/blob/master/examples/example_win32_directx9/main.cpp

#include "globals.h"
#include "overlay.h"
#include "console.h"
#include "features/base.h"
#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <dwmapi.h>
#include <ctime>
#include <string>

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

MARGINS gMargin = {0, 0, 600, 600};

typedef struct _ScreenInfo {
    int width;
    int height;
} ScreenInfo;

ScreenInfo GetScreenInfo() 
{
    ScreenInfo sci = { 0 };
    sci.width = GetSystemMetrics(SM_CXSCREEN);
    sci.height = GetSystemMetrics(SM_CYSCREEN);
    return sci;
}

void Overlay::Loop(void* blank) 
{      
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("b"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("a"), WS_EX_TOPMOST | WS_POPUP, 0, 0, GetScreenInfo().width, GetScreenInfo().height, NULL, NULL, wc.hInstance, NULL);

    SetWindowLong(hwnd, GWL_EXSTYLE,(int)GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED |WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(hwnd, RGB(0,0,0), 0, ULW_COLORKEY);
    //SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

    gMargin = {0, 0, GetScreenInfo().width, GetScreenInfo().height};

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Arial.ttf", 18.0f);
    //IM_ASSERT(font != NULL);

    g_Vars->width = GetScreenInfo().width;
    g_Vars->height = GetScreenInfo().height;

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }
        ::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        Render::Start();
        Helper::RenderStatic();
        Helper::RenderNotifications();
        Helper::RenderFeatures();
        Render::End();

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*255.0f), (int)(clear_color.y*255.0f), (int)(clear_color.z*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_PAINT:
        DwmExtendFrameIntoClientArea(hWnd, &gMargin);  
        break;
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

std::string GetTime() 
{
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
	std::string str(buffer);

	return str;
}

void Render::Start() 
{	
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(GetScreenInfo().width, GetScreenInfo().height), ImGuiCond_Always);
	ImGui::Begin(" ",
		(bool*)true,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);

    ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
}

void Render::End() 
{
	ImGui::End();
}

void Render::Text(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width,
	const ImVec4* cpu_fine_clip_rect) 
{
    ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), pos, color, text_begin, text_end,
		wrap_width, cpu_fine_clip_rect);
}

void Render::RectFilled(int x0, int y0, int x1, int y1, ImColor color, float rounding, int rounding_corners_flags) 
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
}

void Render::Line(ImVec2 a, ImVec2 b, ImColor color, float thickness) 
{
	ImGui::GetWindowDrawList()->AddLine(a, b, color, thickness);
}

void Render::EasyText(ImVec2 pos, ImColor color, const char* text) 
{
    Render::Text(pos, color, text, text + strlen(text), 500, 0);
}

void Render::DrawBox(ImColor color, int x, int y, int w, int h) 
{
	float thicc = 2.0f;

	// ------
	// |    |
	Line(ImVec2(x, y), ImVec2(x + w, y), color, thicc);

	// |
	// |
	// |
	Line(ImVec2(x, y), ImVec2(x, y + h), color, thicc);

	//      |
	//      |
	//      |
	Line(ImVec2(x + w, y), ImVec2(x + w, y + h), color, thicc);

	// |    |
	// ------
	Line(ImVec2(x, y + h), ImVec2(x + w, y + h), color, thicc);
}

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
void Render::Progress(int x, int y, int w, int h, int phealth)
{
	int healthValue = max(0, min(phealth, 100));
	float HealthPerc = healthValue / 100.f;

	ImColor barColor = ImColor(
		min(510 * (100 - healthValue) / 100, 255),
		min(510 * healthValue / 100, 255),
		25,
		255
	);

	RectFilled(x, y, x + w, y + (int)(((float)h / 100.0f) * (float)phealth), barColor, 0.0f, 0);
}

void Helper::RenderStatic() 
{   
    std::string toptext = "amlegit.com\n";
	toptext += GetTime();
	const char* text = toptext.c_str();
    Render::EasyText(ImVec2(10, 10), ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), text);

    for (int i = 0; i < g_VecTextData.size(); i++) 
    {
        Console::TextData* tx = g_VecTextData.at(i);
        if (tx == nullptr) break;
        Render::EasyText(ImVec2(10, 75 + (i * 20)), ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), tx->text);
    }
}

void Helper::RenderNotifications()
{
    ImVec2 pos = ImVec2(10, 45);
    if (g_Vars->activated) 
    {
        Render::EasyText(ImVec2(pos.x, pos.y), ImColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), "Connected and activated");
    } else 
    {
        Render::EasyText(ImVec2(pos.x, pos.y), ImColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), "Not connected to server");
    }
}

void Helper::RenderFeatures()
{
    FeatureBase::Loop();
}