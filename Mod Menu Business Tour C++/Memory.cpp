#include "Memory.h"
#include "Functions.h"

Memory::Memory(LPCSTR WindowNameInit) // Constructor of the class
{
	WindowName = WindowNameInit; // Set WindowName to the window name given
}

void Memory::WriteMemory(uintptr_t addressToWrite, int value) // Write value to memory
{
	DWORD oldProtect, newProtect;
	VirtualProtectEx(GetCurrentProcess(), reinterpret_cast<void*>(addressToWrite), sizeof(value), PAGE_EXECUTE_READWRITE, &oldProtect); // Set page to read/write/execute
	WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(addressToWrite), &value, sizeof(value), nullptr); // Write value to address
	VirtualProtectEx(GetCurrentProcess(), reinterpret_cast<void*>(addressToWrite), sizeof(value), oldProtect, &newProtect); // Set page to old protection
}

DWORD Memory::AoBscan(DWORD patternAddress, char * mask, int StartPage)
{
	SYSTEM_INFO sysInfo; // Holds System Information
	GetSystemInfo(&sysInfo);
	DWORD procMin = (DWORD)sysInfo.lpMinimumApplicationAddress;  // Minimum memory address of process
	DWORD procMax = (DWORD)sysInfo.lpMaximumApplicationAddress;  // Maximum memory address of process

	MEMORY_BASIC_INFORMATION mBI, mBINext;

	DWORD firstOldProtect = NULL;
	DWORD secondOldProtect = NULL;

	DWORD patternSize = (DWORD)strlen(mask);

	for (int i = 0; i < StartPage; i++) //Skip a given number of page
	{
		VirtualQueryEx(GetCurrentProcess(), (LPVOID)procMin, &mBI, sizeof(MEMORY_BASIC_INFORMATION)); // Get memory page details
		procMin += mBI.RegionSize; //Skip page
	}

	while (procMin < procMax) // While still scanning memory
	{  
		VirtualQueryEx(GetCurrentProcess(), (LPVOID)procMin, &mBI, sizeof(MEMORY_BASIC_INFORMATION)); // Get memory page details

		if ((mBI.State == MEM_COMMIT) && (mBI.Type == MEM_PRIVATE)) // Check state of current page
		{  
			VirtualProtect((LPVOID)procMin, mBI.RegionSize, PAGE_EXECUTE_READWRITE, &firstOldProtect); // Set page to read/write/execute

			for (auto n = (DWORD)mBI.BaseAddress; n < (DWORD)mBI.BaseAddress + mBI.RegionSize; n += 0x01) // For each byte in this page
			{  
				if (n + patternSize > procMax) // If our pattern will extend past the maximum memory address, break
				{  
					break;
				}

				if (*(char*)n == (*(char*)patternAddress))  // If first byte of pattern matches current byte
				{
					if (n + patternSize < (UINT)mBI.BaseAddress + mBI.RegionSize) // If entire length of pattern is within this page
					{  
						if (ComparePattern((DWORD)n, patternAddress, mask)) // If full pattern matches
						{
							return (DWORD)n; // Right address found
						}
					}
					else // If it isn't within the same page
					{  
						VirtualQueryEx(GetCurrentProcess(), (LPVOID)(procMin + mBI.RegionSize), &mBINext, sizeof(MEMORY_BASIC_INFORMATION));  // Same memory page stuff with next page

						if ((mBINext.State == MEM_COMMIT) && (mBINext.Type == MEM_PRIVATE)) // Check state of current page
						{
							VirtualProtect((LPVOID)(procMin + mBI.RegionSize), mBINext.RegionSize, PAGE_EXECUTE_READWRITE, &secondOldProtect); // Set page to read/write/execute

							if (ComparePattern((DWORD)n, patternAddress, mask)) // If full pattern matches
							{
								return (DWORD)n; // Right address found
							}
						}
					}
				}
			}


			VirtualProtect((LPVOID)procMin, mBI.RegionSize, firstOldProtect, NULL);  // Reset memory page state of first page

			if (secondOldProtect)  // If we scanned into the second page
			{ 
				VirtualProtect((LPVOID)procMin, mBINext.RegionSize, secondOldProtect, NULL);  // Reset memory page state of second page
				secondOldProtect = NULL;
			}
		}
		procMin += mBI.RegionSize;  // Start scanning next page
	}

	return NULL;  // Return NULL if no matches
}

bool Memory::ComparePattern(DWORD address, DWORD patternAddress, char * mask)
{
	int patternLen = strlen(mask); // Pattern length

	for (auto i = 1; i < patternLen; i++) 
	{
		if (mask[i] != *"?" && *(char*)(address + i) != *(char*)(patternAddress + i)) // If byte is not flagged as ? and the bytes don't match
		{  
			return false; // Not the right address
		}
	}
	if (address != patternAddress) // Make sure we aren't returning a match for the pattern defined within your own hack
	{  
		return true; 
	}
	else // Else, we scanned our own byte pattern, so it's not the right one
	{
		return false;
	}
}