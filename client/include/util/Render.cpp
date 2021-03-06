/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Render.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

bool Render::Init(
    DeviceInitHandlerType&& deviceInitHandler,
    BeforeResetHandlerType&& beforeResetHandler,
    BeginSceneHandlerType&& beginSceneHandler,
    RenderHandlerType&& renderHandler,
    EndSceneHandlerType&& endSceneHandler,
    AfterResetHandlerType&& afterResetHandler,
    DeviceFreeHandlerType&& deviceFreeHandler
) noexcept
{
    if (Render::initStatus) return false;

    Logger::LogToFile("[dbg:render:init] : module initializing...");

    if (!(Render::hookDirect3DCreate9 = MakeJumpHook(GtaDirect3DCreate9, Render::HookFuncDirect3DCreate9)))
    {
        Logger::LogToFile("[err:render:init] : failed to create 'Direct3DCreate9' function hook");
        return false;
    }

    Render::deviceInitHandler = std::move(deviceInitHandler);
    Render::beforeResetHandler = std::move(beforeResetHandler);
    Render::beginSceneHandler = std::move(beginSceneHandler);
    Render::renderHandler = std::move(renderHandler);
    Render::endSceneHandler = std::move(endSceneHandler);
    Render::afterResetHandler = std::move(afterResetHandler);
    Render::deviceFreeHandler = std::move(deviceFreeHandler);

    Render::deviceMutex.lock();
    Render::pDirectInterface = nullptr;
    Render::pDeviceInterface = nullptr;
    Render::deviceParameters = {};
    Render::deviceMutex.unlock();

    Logger::LogToFile("[dbg:render:init] : module initialized");

    Render::initStatus = true;

    return true;
}

void Render::Free() noexcept
{
    if (!Render::initStatus) return;

    Logger::LogToFile("[dbg:render:free] : module releasing...");

    Render::hookDirect3DCreate9.reset();

    Render::deviceInitHandler = nullptr;
    Render::beforeResetHandler = nullptr;
    Render::beginSceneHandler = nullptr;
    Render::renderHandler = nullptr;
    Render::endSceneHandler = nullptr;
    Render::afterResetHandler = nullptr;
    Render::deviceFreeHandler = nullptr;

    Render::deviceMutex.lock();
    Render::pDirectInterface = nullptr;
    Render::pDeviceInterface = nullptr;
    Render::deviceMutex.unlock();

    Logger::LogToFile("[dbg:render:free] : module released");

    Render::initStatus = false;
}

bool Render::GetWindowHandle(HWND& windowHandle) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (!Render::pDeviceInterface) return false;

    windowHandle = Render::deviceParameters.hDeviceWindow;

    return true;
}

bool Render::GetScreenSize(float& screenWidth, float& screenHeight) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (!Render::pDeviceInterface) return false;

    screenWidth = Render::deviceParameters.BackBufferWidth;
    screenHeight = Render::deviceParameters.BackBufferHeight;

    return true;
}

bool Render::ConvertBaseXValueToScreenXValue(const float baseValue, float& screenValue) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (!Render::pDeviceInterface) return false;

    const float screenWidth = Render::deviceParameters.BackBufferWidth;

    screenValue = (screenWidth / Render::BaseWidth) * baseValue;

    return true;
}

bool Render::ConvertBaseYValueToScreenYValue(const float baseValue, float& screenValue) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (!Render::pDeviceInterface) return false;

    const float screenHeight = Render::deviceParameters.BackBufferHeight;

    screenValue = (screenHeight / Render::BaseHeight) * baseValue;

    return true;
}

bool Render::ConvertScreenXValueToBaseXValue(const float screenValue, float& baseValue) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (!Render::pDeviceInterface) return false;

    const float screenWidth = Render::deviceParameters.BackBufferWidth;

    baseValue = (Render::BaseWidth / screenWidth) * screenValue;

    return true;
}

bool Render::ConvertScreenYValueToBaseYValue(const float screenValue, float& baseValue) noexcept
{
    const std::scoped_lock lock { Render::deviceMutex };

    if (!Render::pDeviceInterface) return false;

    const float screenHeight = Render::deviceParameters.BackBufferHeight;

    baseValue = (Render::BaseHeight / screenHeight) * screenValue;

    return true;
}

IDirect3D9* CALLBACK Render::HookFuncDirect3DCreate9(UINT SDKVersion) noexcept
{
    Render::hookDirect3DCreate9->Disable();
    auto pOrigDirect = GtaDirect3DCreate9(SDKVersion);
    Render::hookDirect3DCreate9->Enable();

    if (pOrigDirect)
    {
        Logger::LogToFile("[dbg:render:hookdirect3dcreate9] : intercepted instance (ptr:%p) of IDirect3D9", pOrigDirect);

        if (const auto pHookDirect = new IDirect3D9Hook(pOrigDirect))
        {
            Logger::LogToFile(
                "[dbg:render:hookdirect3dcreate9] : pointer successfully replaced from orig (ptr:%p) to hook (ptr:%p)",
                pOrigDirect, pHookDirect
            );

            pOrigDirect = pHookDirect;
        }
    }

    return pOrigDirect;
}

bool Render::initStatus { false };

std::mutex Render::deviceMutex;

IDirect3D9* Render::pDirectInterface { nullptr };
IDirect3DDevice9* Render::pDeviceInterface { nullptr };
D3DPRESENT_PARAMETERS Render::deviceParameters {};

Render::DeviceInitHandlerType Render::deviceInitHandler { nullptr };
Render::BeforeResetHandlerType Render::beforeResetHandler { nullptr };
Render::BeginSceneHandlerType Render::beginSceneHandler { nullptr };
Render::RenderHandlerType Render::renderHandler { nullptr };
Render::EndSceneHandlerType Render::endSceneHandler { nullptr };
Render::AfterResetHandlerType Render::afterResetHandler { nullptr };
Render::DeviceFreeHandlerType Render::deviceFreeHandler { nullptr };

Memory::JumpHookPtr Render::hookDirect3DCreate9 { nullptr };
