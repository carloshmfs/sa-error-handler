#include <windows.h>
#include <winnt.h>
#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <winnt.h>
#include <chrono>
#include <dbghelp.h>
#include <spdlog/spdlog.h>

PVOID handler;

LONG CALLBACK VectoredHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
    const auto Section = [](const char* name) {
        spdlog::info("*********{}**********", name);
    };

    spdlog::info("Exception Code: {:#010x}", pExceptionInfo->ExceptionRecord->ExceptionCode);
    spdlog::info("Exception Flags: {:#010x}", pExceptionInfo->ExceptionRecord->ExceptionFlags);
    spdlog::info("Exception Address: {:#010x}", (uintptr_t)pExceptionInfo->ExceptionRecord->ExceptionAddress);

    Section("PARAMETERS");
    {
        spdlog::info("Parameters[{}]:", pExceptionInfo->ExceptionRecord->NumberParameters);
        for (DWORD i = 0; i < pExceptionInfo->ExceptionRecord->NumberParameters; i++) {
            spdlog::info("{:>8}: {:#010x}", i, pExceptionInfo->ExceptionRecord->ExceptionInformation[i]);
        }
    }

    CONTEXT& context = *pExceptionInfo->ContextRecord;

    Section("REGISTERS");
    {
        const auto DumpRegister = [](auto name, auto value) {
            spdlog::info("\t{}: {:#010x}", name, value);
        };
        DumpRegister("EAX", context.Eax);
        DumpRegister("EBX", context.Ebx);
        DumpRegister("ECX", context.Ecx);
        DumpRegister("EDX", context.Edx);
        DumpRegister("ESI", context.Esi);
        DumpRegister("EDI", context.Edi);
        DumpRegister("EBP", context.Ebp);
        DumpRegister("ESP", context.Esp);
        DumpRegister("EIP", context.Eip);
        DumpRegister("EFLAGS", context.EFlags);
    }

    Section("CALL STACK");
    {
        HANDLE hProcess = GetCurrentProcess();
        HANDLE hThread = GetCurrentThread();
    
        // Initialize symbol handler
        SymInitialize(hProcess, NULL, TRUE);

        STACKFRAME stackFrame = {};
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrStack.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrPC.Offset = context.Eip;
        stackFrame.AddrStack.Offset = context.Esp;
        stackFrame.AddrFrame.Offset = context.Ebp;

        DWORD prevFrameOffset = 0;
        while (StackWalk(
            IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stackFrame, &context, NULL,
            SymFunctionTableAccess, SymGetModuleBase, NULL))
        {
            DWORD pcOffset = stackFrame.AddrPC.Offset;
            DWORD moduleBase = SymGetModuleBase(hProcess, pcOffset);
            if (moduleBase == NULL) {
                break;
            }

            DWORD displacement = 0;
            IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
            BOOL hasLineInfo = SymGetLineFromAddr(hProcess, pcOffset, &displacement, &lineInfo);

            IMAGEHLP_MODULE moduleInfo{ sizeof(IMAGEHLP_MODULE) };
            BOOL hasModuleInfo = SymGetModuleInfo(hProcess, moduleBase, &moduleInfo);
        
            char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = { 0 };
            PSYMBOL_INFO sym = (PSYMBOL_INFO)symbolBuffer;
            sym->SizeOfStruct = sizeof(SYMBOL_INFO);
            sym->MaxNameLen = MAX_SYM_NAME;
            BOOL hasSym = SymFromAddr(hProcess, pcOffset, NULL, sym);

            spdlog::info(
                "\t{:#010x}: {}!{}:{}",
                pcOffset,
                hasModuleInfo ? moduleInfo.ModuleName : "<unknown>",
                hasSym ? sym->Name : "<unknown>",
                hasLineInfo ? lineInfo.LineNumber : 0
            );
        }

        // Cleanup symbol handler
        SymCleanup(hProcess);
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

static void openConsole()
{
    AllocConsole();
    FILE* fs {};
    freopen_s(&fs, "CONIN$", "r", stdin);
    freopen_s(&fs, "CONOUT$", "w", stdout);
    freopen_s(&fs, "CONOUT$", "w", stderr);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        openConsole();

        using namespace std::chrono_literals;


        spdlog::set_pattern("%^[%l][%H:%M:%S.%e][%s:%#]: %v%$");
        spdlog::enable_backtrace(128);
        spdlog::set_level(spdlog::level::debug);


        spdlog::flush_every(100ms);
        handler = AddVectoredExceptionHandler(1, VectoredHandler);
    }

    if (fdwReason == DLL_PROCESS_DETACH) {
        RemoveVectoredExceptionHandler(handler);
    }

    return TRUE;
}
