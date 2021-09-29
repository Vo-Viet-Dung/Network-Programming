#include<WinSock2.h>
#include <iostream>
using namespace std;
int main()
{
    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);
    SOCKADDR_IN saddr;
    saddr.sin_addr.S_un.S_addr = 0;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);


}


