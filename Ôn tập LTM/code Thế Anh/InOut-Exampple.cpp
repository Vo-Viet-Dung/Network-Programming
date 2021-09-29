#include<stdio.h>
#include<WinSock2.h>
WSABUF wsabuf;
char buffer[1024];
DWORD byteRecv = 0;
DWORD flag = 0;
OVERLAPPED ov;

void CALLBACK RecvCompleted(IN DWORD dwError, IN DWORD cbTransferred, IN LPWSAOVERLAPPED lpOverlapped, IN DWORD dwFlags)
{
	if (dwError == 0)
	{
		printf("Byte recieved: %d\n", cbTransferred);
		printf("Data: %s\n", wsabuf.buf);
	}
}

int main()
{
	WSADATA DATA;
	WSAStartup(MAKEWORD(2, 2), &DATA);

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8888);
	saddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	connect(s, (sockaddr*)&saddr, sizeof(saddr));
	wsabuf.buf = buffer;
	wsabuf.len = sizeof(buffer);
	WSARecv(s, &wsabuf,1, &byteRecv, &flag, &ov, RecvCompleted);
	while (true)
	{
		SleepEx(1000, TRUE);
	}
}