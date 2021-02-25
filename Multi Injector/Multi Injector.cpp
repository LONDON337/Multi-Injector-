// Multi Injector.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//by london1337

#include <iostream>
#include <Windows.h>
using namespace std;

int Inject(DWORD, char*);
int GetDLL(char*);
int GetPID(int*);
int getProc(HANDLE, DWORD);

int GetDLL(char* dll)
{
	std::cout << "add DLL\n";
	cin >> dll;
	return 1;
}

int GetPID(int*PID)
{
	cout << "add PID\n";
	cin >> *PID;
	return 1;
}

int getProc(HANDLE* handleToProc, DWORD pid) {
	*handleToProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	DWORD dwLastError = GetLastError();
	if (*handleToProc == NULL)
	{
		std::cout << "open process\n";
		return -1;
	}
	else { std::cout << "process Opened.\n"; };
}

int Inject(DWORD PID, char* dll)
{
	HANDLE handleToProc;
	LPVOID LoadLibAddr;
	LPVOID baseAddr;
	HANDLE remThread;


	//получить длину длл
	int dllLength = strlen(dll) + 1;

	//обработка процесса
	if(getProc(&handleToProc, PID) <0 )
		return -1;

	//загрузка kernel32
	LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (!LoadLibAddr)
		return -1;

	baseAddr = VirtualAllocEx(handleToProc, NULL, dllLength, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!baseAddr)
		return -1;
	//записываем путь к длл
	if (!WriteProcessMemory(handleToProc, baseAddr, dll, dllLength, NULL))
		return -1;
	//создаем удаленный поток 
	remThread = CreateRemoteThread(handleToProc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, baseAddr, 0, NULL);
	if (!remThread)
		return -1;

	WaitForSingleObject(remThread, INFINITE);

	VirtualFreeEx(handleToProc, baseAddr, dllLength, MEM_RELEASE);

	//закрываем обрабодчик
	if (CloseHandle(remThread) == 0)
	{
		std::cout << "Faild to close handle";
		return -1;
	}
	if (CloseHandle(handleToProc) == 0)
	{
		std::cout << "Failed to close handle to process\n";
	}
}


int main()
{
	SetConsoleTitle("Multi Injector");

	int PID = -1;
	char* dll = new char[255];

	GetDLL(dll);
	GetPID(&PID);
	Inject(PID, dll);
	system("pause");
	return 0;
}

