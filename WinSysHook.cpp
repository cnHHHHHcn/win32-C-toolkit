#include "pch.h"

#include "WinSysHook.h"

WinSysHook* WinSysHook::pHook = nullptr;

WinSysHook::WinSysHook() {
    pHook = this;
}

WinSysHook::~WinSysHook() {
    if (HookHandle) UnInstallHook();
    if (pHook == this) pHook = nullptr;
}

// 设置钩子类型，并设置钩子
bool WinSysHook::IntsallHook(WinSysHook::HookType hookID, DWORD ThreadId) {
    HookHandle = SetWindowsHookEx(hookID, GetHookProc(hookID), GetModuleHandle(DLLName), ThreadId);
    bool Flag = (HookHandle != NULL) ? (ID = hookID, true) : false;
    return Flag;
}

// 卸载钩子
bool WinSysHook::UnInstallHook() {
    bool Flag = UnhookWindowsHookEx(HookHandle) != 0;
    HookHandle = NULL;
    ID = HOOK_NULL;
    return Flag;
}

// 发送 Windows Message ,与 InstallHook 连用
void WinSysHook::DispatchMSG() {
    MSG msg;
    if (ID == WinSysHook::HOOK_KEYBOARD_LL || ID == WinSysHook::HOOK_MOUSE_LL) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }else {
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

// 获取所对应钩子类型的函数地址
HOOKPROC WinSysHook::GetHookProc(WinSysHook::HookType ID) {
    switch (ID) {
    case WH_KEYBOARD: return &WinSysHook::KeyboardProc;
    case WH_KEYBOARD_LL: return &WinSysHook::KeyboardLLProc;
    case WH_MOUSE: return &WinSysHook::MouseProc;
    case WH_MOUSE_LL: return &WinSysHook::MouseLLProc;
    case WH_CALLWNDPROC: return &WinSysHook::CallWndProc;
    case WH_GETMESSAGE: return &WinSysHook::GetMsgProc;
    case WH_CBT: return &WinSysHook::CBTProc;
    case WH_SYSMSGFILTER: return &WinSysHook::SysMsgFilterProc;
    case WH_DEBUG: return &WinSysHook::DebugProc;
    case WH_SHELL: return &WinSysHook::ShellProc;
    case WH_FOREGROUNDIDLE: return &WinSysHook::ForegroundIdle;
    }
    return nullptr;
}

// 静态回调函数实现
LRESULT CALLBACK WinSysHook::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT* keyInfo = (KBDLLHOOKSTRUCT*)lParam;
    if (pHook && nCode >= 0) {
        return pHook->OnKeyboard(nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WinSysHook::KeyboardLLProc(int nCode, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT* keyInfo = (KBDLLHOOKSTRUCT*)lParam;
    if (pHook && nCode >= 0) {
        return pHook->OnKeyboardLL(nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WinSysHook::MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    MSLLHOOKSTRUCT* mouseInfo = (MSLLHOOKSTRUCT*)lParam;
    if (pHook && nCode >= 0) {
        return pHook->OnMouse(nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WinSysHook::MouseLLProc(int nCode, WPARAM wParam, LPARAM lParam) {
    MSLLHOOKSTRUCT* mouseInfo = (MSLLHOOKSTRUCT*)lParam;
    if (pHook && nCode >= 0) {
        return pHook->OnMouseLL(nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WinSysHook::CallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
    CWPSTRUCT* cwp = (CWPSTRUCT*)lParam;
    if (pHook && nCode >= 0) {
        return pHook->OnCallWndProc(nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WinSysHook::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam) {
    MSG* msg = (MSG*)lParam;
    if (pHook && nCode >= 0) {
        return pHook->OnGetMessage(nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WinSysHook::CBTProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // HCBT_*** 类消息 参数 nCode
    if (pHook && nCode >= 0) {
        return pHook->OnCBT(nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WinSysHook::SysMsgFilterProc(int nCode, WPARAM wParam, LPARAM lParam) {
    MSG* msg = (MSG*)lParam;
    if (pHook && nCode >= 0) {
        return pHook->OnSysMsgFilter(nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::DebugProc(int nCode, WPARAM wParam, LPARAM lParam) {
    DEBUGHOOKINFO* debugInfo = (DEBUGHOOKINFO*)lParam;
    if (pHook && nCode >= 0) {
        return pHook->OnDebug(nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::ShellProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // HSHELL_*** 类消息 参数 nCode
    if (pHook && nCode >= 0) {
        return pHook->OnShell(nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// 前台空闲钩子 参数 nCode
LRESULT WinSysHook::ForegroundIdle(int nCode, WPARAM wParam, LPARAM lParam) {
    if (pHook && nCode >= 0) {
        return pHook->OnForegroundIdle(nCode, wParam, lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


// 默认的虚函数实现（空实现，子类需要重写）
LRESULT WinSysHook::OnKeyboard(int nCode, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::OnKeyboardLL(int nCode, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT* keyInfo = (KBDLLHOOKSTRUCT*)lParam;
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::OnMouse(int nCode, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::OnMouseLL(int nCode, WPARAM wParam, LPARAM lParam) {
    MSLLHOOKSTRUCT* mouseInfo = (MSLLHOOKSTRUCT*)lParam;
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::OnCallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
    CWPSTRUCT* cwp = (CWPSTRUCT*)lParam;
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::OnGetMessage(int nCode, WPARAM wParam, LPARAM lParam) {
    MSG* msg = (MSG*)lParam;
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::OnCBT(int nCode, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::OnSysMsgFilter(int nCode, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::OnDebug(int nCode, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::OnShell(int nCode, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT WinSysHook::OnForegroundIdle(int nCode, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}