#include<stdio.h>
#include<WinSock2.h>

int sendData(SOCKET s, char* data, int len) {
	int byteSend = 0;
	while (byteSend < len) {
		byteSend += send(s, data + byteSend, len - byteSend, 0);
	}
	return byteSend;
}

int recvData(SOCKET c, char** buf) {
	
	const int MAX = 1024;
	int totalRecv = 0;
	char tmp[MAX];
	int byteRecv = 0;
	do {
		byteRecv = recv(c, tmp, MAX, 0);
		if (byteRecv > 0) {
			*buf = (char*)realloc(*buf, totalRecv + byteRecv + 1);
			memset(*buf + totalRecv, 0, byteRecv + 1);
			memcpy(*buf + totalRecv, tmp, byteRecv);
			totalRecv += byteRecv;
		}
	} while (byteRecv == MAX);
	return totalRecv;
}

void main()
{
	WSADATA DATA;
	WSAStartup(MAKEWORD(2, 2), &DATA);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) {
		printf("Khong tao duoc socket, ma loi: %d", WSAGetLastError());
		return;
	}
	else {
		// socket le tan
		SOCKADDR_IN saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(8888);
		saddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

		//ket noi socket voi interface
		// ep kieu SOCKADDR_IN ve sockaddr_in, ve co ban ca 2 deu giong nhau
		bind(s, (sockaddr*)&saddr, sizeof(saddr));
		listen(s, 10); // active socket voi do dai hang doi la 10
		
		// dia chi client ket noi den
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);
		SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
		char* hello = (char*) "Hello Programming \n";
		int byteSend = sendData(c, hello, strlen(hello));
		printf("So byte da gui: %d\n", byteSend);

		char* data = NULL; // cap phat tinh 1024 byte
		//memset(data, 0, sizeof(data)); // khoi tao chuoi rong
		int byteRecv = recvData(c, &data);
		printf("So byte da nhan: %d\n", byteRecv);
		printf("Du lieu gui ve: %s\n", data);
		free(data);
		closesocket(s);
		closesocket(c);
		WSACleanup();
	}
}
