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
	const int MAX = 1024;
	int totalReceived = 0;
	char tmp[MAX];
	int byteReceived = 0;
	do
	{
		byteReceived = recv(c, tmp, MAX, 0);
		if (byteReceived > 0)
		{
			*buffer = (char*)realloc(*buffer, totalReceived + byteReceived + 1);
			memset(*buffer + totalReceived, 0, byteReceived + 1);
			memcpy(*buffer + totalReceived, tmp, byteReceived);
			totalReceived += byteReceived;
		}
	} while (byteReceived == MAX);
	return totalReceived;
}

void main()
{
	//Khoi tao va khoi dong thu vien
	WSADATA DATA;
	WSAStartup(MAKEWORD(2, 2), &DATA);
	//Tao socket c danh cho client
	SOCKET c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//Tao cau truc socket server address de noi den server
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(7777);
	saddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//kết nối đến server bằng socket c qua cau truc dia chi saddr voi chieu dai cua cau truc dia chi = sizeof(saddr)
	int result = connect(c, (sockaddr*)&saddr, sizeof(saddr));
	
	//Nếu như kết nối được thi tien hanh xu ly nhan du lieu
	if (result != SOCKET_ERROR)
	{
		//Gui thong tin len server
		char* hello = (char*)"Hello Network Programming\n";
		SendData(c, hello, strlen(hello));
		char* buffer = NULL;
		RecvData(c, &buffer);
		printf("%s\n", buffer);
		free(buffer);
		buffer = NULL;
		closesocket(c);
	}
	WSACleanup();
}