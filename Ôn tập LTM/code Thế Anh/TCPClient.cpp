#include<stdio.h>
#include<conio.h>
#include<iostream>
#include<string>
#include<sstream>
#include<WinSock2.h>

using namespace std;

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


void main(int argc, char* argv[]) {
	if (argc < 3) {
		printf("Lack of parameters.\n");
	}
	else {
		WSADATA Data;
		int error = WSAStartup(MAKEWORD(2, 2), &Data);
		if (error == SOCKET_ERROR) {
			printf("WinSock Started up fail ...\n");
		}
		else {
			printf("WinSock Started up success ...\n");
			printf("Connect to Server? \nSyntax: TCPClient.exe <IP Address> <Port>:");

			char* ipaddr = argv[1];

			char* cPort = argv[2];
			short port = (short)atoi(cPort);

			printf("Connect to IP address: %s\n", ipaddr);
			printf("Connect to Port: %d\n", port);

			SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			SOCKADDR_IN saddr;
			saddr.sin_family = AF_INET;
			saddr.sin_port = htons(port);
			saddr.sin_addr.S_un.S_addr = inet_addr(ipaddr);

			int res = connect(s, (sockaddr*)&saddr, sizeof(saddr));
			if (res != SOCKET_ERROR) {
				while (1) {
					cout << "Your message : ";
					string message;
					getline(cin, message, '\n');
					int byteSent = sendData(s, (char*)message.c_str(), strlen(message.c_str()));
					printf("Sent: %d bytes\n", byteSent);
				}
			}
			else {
				printf("Socket error!");
			}
			closesocket(s);
			WSACleanup();
		}
	}

}