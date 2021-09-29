#include <stdio.h>
#include <WinSock2.h>

int SendData(SOCKET c, char* data, int len)
{
	int byteSent = 0;
	while (byteSent < len)
	{
		byteSent += send(c, data + byteSent, len - byteSent, 0);
	}
	return byteSent;
}

int RecvData(SOCKET c, char** buffer)
{
	//Do dai toi da cua xau ky tu nhan duoc
	const int MAX = 1024;
	//So ky tu da nhan duoc tu phia client gui len
	int totalReceived = 0;
	//Xau buffer tam dung de nhan va chuyen tiep xu ly 
	char tmp[MAX];
	//Byte da nhan duoc
	int byteReceived = 0;
	do
	{	
		//Nhan ky tu duoc truyen vao trong bo dem
		//Ky tu duoc truyen vao bo dem temp
		//Kich thuoc la 1024 phan tu
		//Co quy dinh cach thuc nhan = 0 => khong co quy dinh gi
		//Socket nhan la socket c
		byteReceived = recv(c, tmp, MAX, 0);
		//Neu nhu van con nhan duoc ky tu vao bo dem thi xu ly chuyen tiep du lieu
		if (byteReceived > 0)	
		{	
			//Tao ra bo dem ky tu buffer co kich thuoc = byteReceived + totalReceived + 1 de tranh tran bo dem
			*buffer = (char*)realloc(*buffer, totalReceived + byteReceived + 1);
			//Thuc hien don bo dem ung voi phan thong diep chuan bi chen vao 
			memset(*buffer + totalReceived, 0, byteReceived + 1);
			//Copy phan ky tu trong bo dem temp vao phan buffer la mang chua xau ky tu cuoi cung
			memcpy(*buffer + totalReceived, tmp, byteReceived);
			//So byte nhan duoc = so byte da nhan + so byte vua nhan them
			totalReceived += byteReceived;
		}
	} while (byteReceived == MAX);
	//Tra ve so byte ma server nhan duoc
	return totalReceived;
}

void main()
{

	WSADATA DATA;
	WSAStartup(MAKEWORD(2, 2), &DATA);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		printf("Khong tao duoc socket, ma loi: %d\n", WSAGetLastError());
		return;
	}
	else
	{

		SOCKADDR_IN saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(8888);
		saddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		bind(s, (sockaddr*)&saddr, sizeof(saddr));
		listen(s, 10);
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);
		SOCKET c = accept(s, (sockaddr*)&caddr, &clen);

		int errorCode = WSAGetLastError();
		//Server sau khi chap nhan ket noi se gui xau chao mung cho client
		char* hello = (char*)"Hello Network Programming\n";
		int byteSent = SendData(c, hello, strlen(hello));
		printf("So byte da gui: %d\n", byteSent);
		//Tao ra con tro data de nhan du lieu tu client
		//Du lieu nhan duoc se la xau ky tu data
		char* data = NULL;
		int totalReceived = RecvData(c, &data);
		printf("So byte nhan duoc: %d\n", totalReceived);
		printf("Du lieu nhan duoc: %s\n", data);
		free(data);
		data = NULL;
		closesocket(c);
		closesocket(s);
		WSACleanup();
	}
}