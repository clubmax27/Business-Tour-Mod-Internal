#include "Functions.h"
#include "Memory.h"

using namespace std;

// Variables needed
Memory m("Business Tour"); // Name of the window (used by ReadMemory)
char MoneyBytePattern[] = "\x00\x00\x00\x00\xFF\xFF\xFF\xFF\x1C\xC8\x06\x00\x9C\x4C\x18\x00\x80\x84\x1E\x00\x01\x00\x00\x00\x1C\xC8\x06\x00\x1C\xC8\x06\x00\x00\x00\x00\x00\x01\x00\x00\x00\x1C\xC8\x06\x00\x1D\xC8\x06\x00"; // Byte Pattern for the money
char MoneyByteMask[] = "xxxx????xxxx????????xxxxxxxx?xxx?xxxxxxxxxxxxxxx"; // Byte Mask for the money


// Starting the hack
void InitiatePatch()
{

	DWORD address = m.AoBscan((DWORD)&MoneyBytePattern, MoneyByteMask, 900); // We start to look at page 900, it's where the money value usually is

	if (address == (DWORD)0) // If we didn't find the address
	{
		MessageBoxA(NULL, (LPCSTR)"Game not detected, exiting hack ...", "Game not detected", 0);
		return;
	}
	MessageBoxA(NULL, (LPCSTR)"All hacks loaded, ready to start", "Game detected", 0);
	
	// Used to be sure that the hack actually found the right address
	//DebugMessageBox(m.ReadMemory<int>(address + 0x10));
	
	// As long as the hack isn't closed
	while (true)
	{
		Sleep(100); // Sleep 0.1 second to not overwhelm the processor
		if (GetAsyncKeyState(0x31) < 0) // If "1" key is pressed
		{
			m.WriteMemory(address + 0x10, 20000000); // 0x10 is the main money offset
			m.WriteMemory(address + 0x0C, 20440348); // 0x0C is the encrypted money offset
		}
	}
	
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	// Set global DLL module handle

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			InitiatePatch(); // Start the hack
			break;
	}

	// Successful DLL_PROCESS_ATTACH.
	return TRUE;
}