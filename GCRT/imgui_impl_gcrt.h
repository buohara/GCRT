#pragma once

#include "imgui.h"
#include "GCRT.h"
#include <Windows.h>
#include <chrono>

using namespace std::chrono;

void ImGuiGCRTInit(HWND hwnd, uint32_t w, uint32_t h);
void ImGuiGCRTNewFrame();
bool ImGuiGCRTCreateDeviceObjects();
void ImGuiGCRTSetMouse(double x, double y, bool lDown, bool rDown, bool mDown);