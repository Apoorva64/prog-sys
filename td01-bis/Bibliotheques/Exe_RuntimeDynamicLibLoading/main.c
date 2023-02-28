#include <stdio.h>
#include <Windows.h>
typedef void (*printStopProto)(char*);

int main()
{
	printStopProto PrintStop;
	void* dll_handler = LoadLibrary(L"DynamicLib.dll");
	if (!dll_handler)
		printf("Failed to load library");
	else
	{
		PrintStop = (printStopProto)GetProcAddress(dll_handler, "PrintStop");
		PrintStop("pewpewpew");
		FreeLibrary(dll_handler);
	}
}
