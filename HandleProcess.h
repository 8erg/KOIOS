#ifndef HANDLE_PROCESS_H
#define HANDLE_PROCESS_H

#include "CustomLog.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <sddl.h>
#include <wchar.h>
#include <stdlib.h>
#include <AclAPI.h>

#pragma comment(lib,"advapi32.lib")

void EnumerateProcesses();
DWORD GetProcIdByName(const wchar_t* procName);
BOOL DisplayPrivs(HANDLE hProcess);
BOOL isProcessElevated(HANDLE hProcess);
LUID GetLuidValue(const wchar_t* privName);
BOOL HasPrivilege(wchar_t* privName, HANDLE hProcess);
BOOL SetPrivilege(wchar_t* privName, HANDLE hProcess, BOOL enable);
HANDLE FindProcessWithPrivilege(wchar_t* privName);

#endif