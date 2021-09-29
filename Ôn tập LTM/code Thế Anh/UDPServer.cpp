#include<stdio.h>
#include<WinSock2.h>
#include<conio.h>
int main() {
	const int MAX_CLIENT = 1024;
	int clientCount = 0;
	SOCKADDR_IN caddrs[MAX_CLIENT];
	WSADATA Data;
	WSAStartup(MAKEWORD(2, 2), &Data);
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	char* welcome = (char*)"Hello UDP Programming";
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8888);
	saddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	while (1) {
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);
		int byteRecv = recvfrom(s, buf, sizeof(buf), 0, (sockaddr*)&caddr, &clen);
		memcpy(&caddrs[clientCount], &caddr, sizeof(caddr));
		clientCount++;
		for (int i = 0; i < clientCount; i++) {
			SOCKET c = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			sendto(c, buf, strlen(buf), 0, (sockaddr*)&caddrs[i], sizeof(SOCKADDR_IN));
			closesocket(c);
		}
		printf("Da nhan: %s  %d bytes", buf, byteRecv);
	}
	getch();
}