#ifndef LSASS_DUMPER_H
#define LSASS_DUMPER_H

#include "Custom_Log.h"
#include <Windows.h>
#include <DbgHelp.h>

#pragma comment(lib,"DbgHelp.lib")

void Dump(DWORD procId);

#endif




