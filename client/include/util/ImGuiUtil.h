/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <Windows.h>
#include <d3d9.h>

class ImGuiUtil {
public:

    static bool Init(IDirect3DDevice9* pDevice,
                     D3DPRESENT_PARAMETERS* pParameters) noexcept;
    static bool IsInited() noexcept;
    static void Free() noexcept;

    static bool RenderBegin() noexcept;
    static bool IsRendering() noexcept;
    static void RenderEnd() noexcept;

    static LRESULT OnWndMessage(HWND hWnd, UINT uMsg,
                                WPARAM wParam, LPARAM lParam) noexcept;

private:

    static bool initStatus;
    static bool win32loadStatus;
    static bool dx9loadStatus;
    static bool renderStatus;

};
