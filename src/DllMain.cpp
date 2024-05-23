#include <windows.h>
#include <winnt.h>
#include <stdio.h>
#include <iostream>

static void openConsole()
{
    AllocConsole();
    FILE* fs{};
    freopen_s(&fs, "CONIN$", "r", stdin);
    freopen_s(&fs, "CONOUT$", "w", stdout);
    freopen_s(&fs, "CONOUT$", "w", stderr);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        openConsole();
        std::cout << "Hello From DllMain()" << std::endl;
    }

    return TRUE;
}
