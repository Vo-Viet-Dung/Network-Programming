#include<stdio.h>
#include<WinSock2.h>

int main()
{
	SOCKET c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_addr.S_un.S_addr = inet_addr("10.0.0.1");
	saddr.sin_port = htons(8888);
	saddr.sin_family = AF_INET;
	int r = connect(c, (sockaddr*)&saddr, sizeof(saddr));
	printf("XONG.\n");


}