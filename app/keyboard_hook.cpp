#include <windows.h>
#include <atomic>

static HINSTANCE g_hinst = nullptr;
static HHOOK g_hook = nullptr;
static std::atomic<int> g_number_keypress{0};
static std::atomic<bool> g_ctrl_pressed{false};

LRESULT CALLBACK wireKeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
    g_number_keypress.fetch_add(1,std::memory_order_relaxed);
    if  (code == HC_ACTION)      
    {
        KBDLLHOOKSTRUCT* kbdStruct = ((KBDLLHOOKSTRUCT *)lParam);
        if(kbdStruct!=nullptr && wParam == WM_KEYDOWN && kbdStruct->vkCode == VK_LCONTROL)
        {
            g_ctrl_pressed.store(true, std::memory_order_relaxed);
        }
        else if(kbdStruct!=nullptr && wParam == WM_KEYUP && kbdStruct->vkCode == VK_LCONTROL)
        {
            g_ctrl_pressed.store(false, std::memory_order_relaxed);
        }
    }
    return CallNextHookEx(g_hook, code, wParam, lParam);
}

extern "C" __declspec(dllexport) HHOOK install()
{
    // use WH_KEYBOARD_LL isntead of WH_KEYBOARD to get keyboard events even while we are in background
    g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, wireKeyboardProc, g_hinst, NULL);
    return g_hook;
}

extern "C" __declspec(dllexport) bool uninstall()
{
    return UnhookWindowsHookEx(g_hook);
}

extern "C" __declspec(dllexport) int keypress_count()
{
    return g_number_keypress.load(std::memory_order_relaxed);
}

extern "C" __declspec(dllexport) bool control_pressed()
{
    return g_ctrl_pressed.load(std::memory_order_relaxed);
}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(
    __in HINSTANCE hinstDLL,
    __in DWORD fdwReason,
    __in LPVOID lpvReserved)
{
    g_hinst = hinstDLL;
    return TRUE;
}
