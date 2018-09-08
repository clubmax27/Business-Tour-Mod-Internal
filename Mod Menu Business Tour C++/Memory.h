#pragma once
#include <iostream>
#include <Windows.h>

class Memory
{
public:

	Memory(LPCSTR WindowNameInit);
	template<typename Type> Type ReadMemory(DWORD address);
	void WriteMemory(uintptr_t addressToWrite, int value);
	DWORD AoBscan(DWORD patternAddress, char * mask, int StartPage = 0);

private:

	bool ComparePattern(DWORD address, DWORD patternAddress, char * mask);
	LPCSTR WindowName;
};

template<typename Type> Type Memory::ReadMemory(DWORD address)
{
	Type DataType; 
	DWORD pid;
	HWND window = FindWindow(NULL, WindowName); // Get the window handle

	if (!window) // If window not found
	{
		MessageBoxA(NULL, (LPCSTR)"Can't find Window", "Error", 0);
	}

	GetWindowThreadProcessId(window, &pid); // Get PID of window
	HANDLE pHandle = OpenProcess(PROCESS_VM_READ, 0, pid);  // Get process handle

	if (!pHandle) // If process not found
	{
		MessageBoxA(NULL, (LPCSTR)"Can't find Process", "Error", 0);
	}

	ReadProcessMemory(pHandle, (void*)address, &DataType, sizeof(DataType), NULL); // Read memory

	return DataType; // Return result
}