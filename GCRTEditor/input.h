#pragma once

#include "gcrt.h"

extern bool gResized;
extern uint32_t gWindowWidth;
extern uint32_t gWindowHeight;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);