
#include <iostream>
#include <conio.h>
#include<Windows.h>
using namespace std;
DWORD WINAPI MyThread(LPVOID lp) {
	cout << GetCurrentThreadId() << endl;
	cout << "Hello\n";
	return 0;
}
void main()
{
	//sample 1

	cout << GetCurrentThreadId() <<endl;
	const int MAXT = 10;
	HANDLE hThread[MAXT];
	for (int i = 0; i < MAXT;i++) {
		hThread[i] = CreateThread(NULL, 0, MyThread, NULL, 0, NULL);
	}
   cout << "out"<<endl;
   WaitForMultipleObjects(MAXT, hThread, TRUE, INFINITE);
	   for (int i = 0;i < MAXT;i++) {
		   CloseHandle(hThread[i]);
	}
	   

   //sample 2 tham so ( contro HANDLE, Thoi gian doi)
   //WaitForSingleObject(Handle, INFINITE);
   //CloseHandle(Handle);
}

