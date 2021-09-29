#include<stdio.h>

#define FD_SETSIZE 1024 // kich thuoc cua tap fd_set

#include<WinSock2.h>
#include<Windows.h>


int main() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKADDR_IN saddr;
	saddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	saddr.sin_port = htons(8888);
	saddr.sin_family = AF_INET;;
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);
	fd_set fdread;
	while (1) {
		FD_ZERO(&fdread);
		FD_SET(s, &fdread); // chi them duoc den gioi han FD_SETSIZE
		select(0, &fdread, NULL, NULL, NULL);
		if (FD_ISSET(s, &fdread)) {
			SOCKADDR_IN caddr;
			int clen = sizeof(caddr);
			SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
			printf("Client: %d\n", c);
		}
	}
}