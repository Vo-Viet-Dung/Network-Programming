#include <iostream>
#include<WinSock2.h>
using namespace std;
int main()
{
    
    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    char* welcome = (char*)"Hello";
    SOCKADDR_IN toAddr;
    toAddr.sin_family = AF_INET;
    toAddr.sin_port = htons(8888);
    toAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    int send = sendto(s, welcome, strlen(welcome), 0, (sockaddr*)&toAddr, sizeof(toAddr));
    cout << send;
    return 0;
}

