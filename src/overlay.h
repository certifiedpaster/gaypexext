#ifndef OVERLAY_H
#define OVERLAY_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

namespace Overlay 
{
    void Loop(void* blank);
}

namespace Render 
{
    void Start();
    void End();

    void Text(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width,
	const ImVec4* cpu_fine_clip_rect);
    void EasyText(ImVec2 pos, ImColor color, const char* text); 
    void RectFilled(int x0, int y0, int x1, int y1, ImColor color, float rounding, int rounding_corners_flags);
}

namespace Helper 
{
    void RenderStatic();
    void RenderNotifications();
    void RenderFeatures();
}

#endif