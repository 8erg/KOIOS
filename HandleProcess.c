#include "HandleProcess.h"


void EnumerateProcesses()
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;

		procEntry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				printf("%-10lu %-15ls\n", procEntry.th32ProcessID, procEntry.szExeFile);
			} while (Process32Next(hSnap, &procEntry));
		}
	}

	CloseHandle(hSnap);
}

DWORD GetProcIdByName(const wchar_t* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;

		procEntry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (wcscmp(procEntry.szExeFile, procName) == 0)
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}

	CloseHandle(hSnap);

	return procId;
}

BOOL DisplayPrivs(HANDLE hProcess)
{
	HANDLE hToken = NULL;

	if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
	{
		warn("Failed to get access token...");

		return FALSE;
	}

	DWORD dwSize = 0;

	if (!GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &dwSize)
		&& GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		warn("Failed to query the buffer size..");
		CloseHandle(hToken);

		return FALSE;
	}

	PTOKEN_PRIVILEGES pTokenPrivileges = (PTOKEN_PRIVILEGES)malloc(dwSize);

	memset(pTokenPrivileges, 0, dwSize);

	if (!GetTokenInformation(hToken, TokenPrivileges, pTokenPrivileges, dwSize, &dwSize))
	{
		warn("Failed to retrieve privileges...");
		free(pTokenPrivileges);
		CloseHandle(hToken);

		return FALSE;
	}

	for (DWORD i = 0; i < pTokenPrivileges->PrivilegeCount; i++)
	{
		DWORD attributes = pTokenPrivileges->Privileges[i].Attributes;
		LUID luid = pTokenPrivileges->Privileges[i].Luid;

		if (attributes & SE_PRIVILEGE_ENABLED)
		{
			printf("[i] Enabled - ");
		}
		else {
			printf("[i] Disabled - ");
		}

		dwSize = 0;

		if (!LookupPrivilegeName(NULL, &luid, NULL, &dwSize)
			&& GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			warn("Failed to query buffer size for privilege name...");
			continue;
		}

		DWORD privilegeNameBuffer = dwSize + sizeof(wchar_t);
		wchar_t* pPriName = (wchar_t*)malloc(privilegeNameBuffer * sizeof(wchar_t));

		if (!LookupPrivilegeName(NULL, &luid, pPriName, &dwSize))
		{
			warn("Failed to convert LUID to string...");
			free(pPriName);
			continue;
		}

		printf("%ls - ", pPriName);

		dwSize = 0;
		DWORD langId = LANG_USER_DEFAULT;

		if (!LookupPrivilegeDisplayName(NULL, pPriName, NULL, &dwSize, &langId)
			&& GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			warn("Failed to query buffer size for privilege display name");
			continue;
		}

		DWORD dispNameBuffer = dwSize + sizeof(wchar_t);
		wchar_t* pDisplayName = (wchar_t*)malloc(dispNameBuffer * sizeof(wchar_t));

		if (!LookupPrivilegeDisplayName(NULL, pPriName, pDisplayName, &dwSize, &langId))
		{
			warn("Failed to convert to display name...");
			free(pPriName);
			free(pDisplayName);
			continue;
		}

		wprintf(L"%ls\n", pDisplayName);

		free(pPriName);
		free(pDisplayName);
	}

	free(pTokenPrivileges);
	CloseHandle(hToken);

	printf("\n");

	return TRUE;
}

BOOL isProcessElevated(HANDLE hProcess)
{
	HANDLE hToken;
	BOOL isElevated = FALSE;

	if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
	{
		TOKEN_ELEVATION elevation;
		DWORD tokenCheck = sizeof(TOKEN_ELEVATION);

		if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &tokenCheck))
		{
			isElevated = elevation.TokenIsElevated;
		}
		CloseHandle(hToken);
	}

	return isElevated;
}

LUID GetLuidValue(const wchar_t* privName)
{
	LUID luid;

	if (!LookupPrivilegeValue(NULL, privName, &luid))warn("Could not retrieve luid from privilege constant name...");

	return luid;
}

BOOL HasPrivilege(wchar_t* privName, HANDLE hProcess)
{
	HANDLE hToken;
	BOOL hasPrivilege = FALSE;
	LUID privLuid = GetLuidValue(privName);


	if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
	{
		warn("Failed to open the process token...");

		return FALSE;
	}

	PRIVILEGE_SET privilegeSet;
	privilegeSet.PrivilegeCount = 1;
	privilegeSet.Privilege[0].Luid = privLuid;
	privilegeSet.Control = PRIVILEGE_SET_ALL_NECESSARY;
	privilegeSet.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!PrivilegeCheck(hToken, &privilegeSet, &hasPrivilege)) warn("Failed to check the privilege...");

	return hasPrivilege;
}

BOOL SetPrivilege(wchar_t* privName, HANDLE hProcess, BOOL enable)
{
	HANDLE hToken;
	BOOL status = FALSE;
	LUID luid = GetLuidValue(privName);
	TOKEN_PRIVILEGES priv = { 0,0,0,0 };

	if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))goto EXIT;

	priv.PrivilegeCount = 1;
	priv.Privileges[0].Luid = luid;
	priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &priv, 0, 0, 0)) goto EXIT;

	CloseHandle(hToken);

	return TRUE;


EXIT:
	if (hToken)CloseHandle(hToken);
	return FALSE;
}

//After finding it, got to see if i can use it to impersonate another process or i have to do another step before
HANDLE FindProcessWithPrivilege(wchar_t* privName)
{
	HANDLE hProcess = NULL;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;

		procEntry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, procEntry.th32ProcessID);

				if (hProcess)
				{
					if (HasPrivilege(SE_IMPERSONATE_NAME, hProcess))
					{
						succ("The process %ls with the id : %lu, has the %ls", procEntry.szExeFile,procEntry.th32ProcessID, SE_IMPERSONATE_NAME);
						CloseHandle(hProcess);

						break;
					}

					CloseHandle(hProcess);
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}

	CloseHandle(hSnap);

	return hProcess;
}
