
#include "keyboard_listener.hpp"
#include <cstdio>
#include <strsafe.h>

static void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}

keyboard_listener::keyboard_listener() {
	library_ = LoadLibrary(TEXT("keyboard_hook.dll"));
	if (library_ == NULL)
	{
        ErrorExit(TEXT("LoadLibrary"));
	}

	install_ = (InstallFunc)GetProcAddress(library_, "install");
    if (install_ == NULL)
	{
        ErrorExit(TEXT("GetProcAddress(\"install\")"));
	}
	uninstall_ = (BoolFunc)GetProcAddress(library_, "uninstall");
    if (uninstall_ == NULL)
	{
        ErrorExit(TEXT("GetProcAddress(\"uninstall\")"));
	}
	keypress_count_ = (IntFunc)GetProcAddress(library_, "keypress_count");
    if (keypress_count_ == NULL)
	{
        ErrorExit(TEXT("GetProcAddress(\"keypress_count\")"));
	}
    ctrl_pressed_ = (BoolFunc)GetProcAddress(library_, "control_pressed");
    if (ctrl_pressed_ == NULL)
	{
        ErrorExit(TEXT("GetProcAddress(\"control_pressed\")"));
	}
}
 
keyboard_listener::~keyboard_listener()
{
    stop();
    if(FreeLibrary(library_)==0) {
        ErrorExit(TEXT("FreeLibrary"));
    }
}


int keyboard_listener::event_count() const
{
    return keypress_count_();
}

void keyboard_listener::listen()
{
    if(install_()==nullptr) {
        ErrorExit(TEXT("install"));
    }
    is_listening_ = true;
}

void keyboard_listener::stop()
{
    if(is_listening_) {
        if(!uninstall_()){
            ErrorExit(TEXT("uninstall"));
        }
    }
    is_listening_ = false;
}

bool keyboard_listener::ctrl_pressed() const
{
    return ctrl_pressed_();
}
