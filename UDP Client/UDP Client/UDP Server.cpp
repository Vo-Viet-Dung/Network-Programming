#include <iostream>
#include<WinSock2.h>
using namespace std;
int main()
{

    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    char* welcome = (char*)"Hello";
    SOCKADDR_IN sAddr;
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(8888);
    sAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(s, (sockaddr*)&sAddr, sizeof(sAddr));
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    SOCKADDR_IN caddr;
    int clen = sizeof(caddr);
    int received = recvfrom(s, buffer, sizeof(buffer),0, (sockaddr*)&caddr, &clen);
    //int send = sendto(s, welcome, strlen(welcome), 0, (sockaddr*)&toAddr, sizeof(toAddr));
    cout <<"da nhan:" << received;
    return 0;
}
