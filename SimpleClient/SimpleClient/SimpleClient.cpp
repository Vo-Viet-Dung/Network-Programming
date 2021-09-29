#include<iostream>
#include<stdio.h>
#include<string.h>
#include<WinSock2.h>

using namespace std;
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
int main() {
	WSADATA DATA;
	WSAStartup(MAKEWORD(2,2), &DATA);
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        cout << "Khong tao duoc Socket, ma loi: " << WSAGetLastError() << endl;
    }
    else {
		char* ip = new char[10];
		long long port;
		cin >> ip;
		cin >> port;
        SOCKADDR_IN saddr;
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);

        saddr.sin_addr.S_un.S_addr = inet_addr(ip);
        int result = connect(s, (sockaddr*)&saddr, sizeof(saddr));
        if (result != SOCKET_ERROR) {

            //char* hello = (char*)"Hello\n";
			char c[255];
			//memset(c, 0, sizeof(c));
			cin.ignore();
			cin.getline(c, 255);
			fflush(stdin);
			char* hello = (char*)c;
			SendData(s, hello, strlen(hello));
			char* buffer = NULL;
			RecvData(s, &buffer);
			cout << buffer << endl;
			free(buffer);
			buffer = NULL;
			closesocket(s);
        }
    }
	WSACleanup();

}