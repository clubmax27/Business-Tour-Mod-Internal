#include <iostream>
#include <sstream>
#include <Windows.h>

#pragma comment(lib, "User32.lib")

using namespace std;

// Outputs a MessageBox with any value you give this function
template<typename Type> void DebugMessageBox(Type message);

template<typename Type> void DebugMessageBox(Type message)
{
	stringstream os;
	os << message;
	string intString = os.str();
	MessageBoxA(NULL, (LPCSTR)intString.c_str(), "Debug", 0);
}





