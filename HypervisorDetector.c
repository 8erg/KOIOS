#include "HypervisorDetector.h"

/*
* This will probably get flagged
* To be more stealthy maybe use the token impersonation to access the registry as another user, so we can hide the fact
*  that it's our process that had access to it
*/
void GetBIOSVendorName()
{
	HKEY hKey;
	DWORD valueType;
	DWORD dataSize = 0;
	LPBYTE data = NULL;
	const char* keyName = "BIOSVendor";

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"HARDWARE\\DESCRIPTION\\System\\BIOS",
		0, KEY_READ,
		&hKey) != ERROR_SUCCESS) warn("Failed to open registry, error : %lu", GetLastError());


	if (RegQueryValueExA(hKey,
		keyName,
		NULL,
		&valueType,
		NULL,
		&dataSize) != ERROR_SUCCESS) warn("Failed to read registry key, error : %lu", GetLastError());

	data = (LPBYTE)malloc(dataSize);

	if (data == NULL) warn("Memory allocation failed.");

	if (RegQueryValueExA(hKey, keyName, NULL, &valueType, data, &dataSize) != ERROR_SUCCESS) {
		warn("Failed to retrieve registry value, error : %lu\n", GetLastError());
		free(data);
	}

	info("%s value is %s\n", keyName, (char*)data);

	RegCloseKey(hKey);
	free(data);
}

void DetectHyperV() {
	if (GetFileAttributes(L"C:\\Windows\\System32\\Drivers\\vmmouse.sys") != INVALID_FILE_ATTRIBUTES ||
		GetFileAttributes(L"C:\\Windows\\System32\\Drivers\\vmhgfs.sys") != INVALID_FILE_ATTRIBUTES ||
		GetFileAttributes(L"C:\\Windows\\System32\\Drivers\\vmmemctl.sys") != INVALID_FILE_ATTRIBUTES ||
		GetFileAttributes(L"C:\\Windows\\System32\\Drivers\\vmrawdsk.sys") != INVALID_FILE_ATTRIBUTES ||
		GetFileAttributes(L"C:\\Windows\\System32\\Drivers\\vboxmouse.sys") != INVALID_FILE_ATTRIBUTES)
		info("The process is probably running on a vm based on the drivers present on it...");

	else info("No VM drivers found, the process is probably running on the local machine...");
}
