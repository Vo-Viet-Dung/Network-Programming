#include <iostream>
#include <WinSock2.h>
#define MAX_CLIENT 1024
using namespace std;
int main()
{
    /*const int MAX_CLIENT = 1024;*/

    //Tao ra mang cau truc dia chi client cho 1024 may noi vao
    SOCKADDR_IN caddrs[MAX_CLIENT];
    //Bien dem so client da ket noi
    int clientCount = 0;

    //Khoi tao va khoi dong thu vien Winsock
    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);

    //Tao socket theo giao thuc UDP cho server
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    //Chuoi ky tu chao cua server
    char *welcome = (char *)"Hello";

    //Tao cau truc thong tin dia chi cho server chap nhan ket noi tu moi dia chi
    SOCKADDR_IN sAddr;
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(8888);
    sAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    //Ghep socket vao cau truc dia chi sAddr
    bind(s, (sockaddr *)&sAddr, sizeof(sAddr));

    //Treo de thuc hien xu ly ket noi
    while (1)
    {
        //Xau ky tu ma server se nhan duoc
        char buffer[1024];
        //Don dep bo nho cua buffer
        memset(buffer, 0, sizeof(buffer));

        //Tao ra cau truc dia cho cho client de ket noi len server
        SOCKADDR_IN caddr;
        //Bien clen chua do dai cua cau truc caddr
        int clen = sizeof(caddr);
        //Xu ly nhan du lieu
        //Nhan du lieu tu phia client qua cau truc dia chi caddr
        //Ky tu nhan duoc la xau buffer co kich thuoc = sizeof(buffer)
        //Socket nhan la socket s
        int received = recvfrom(s, buffer, sizeof(buffer), 0, (sockaddr *)&caddr, &clen);
        // luu lai danh sach client ket noi den
        // Copy cau truc dia chi client dang nhan duoc vao mang dia chi client
        memcpy(&caddrs[clientCount], &caddr, sizeof(caddr));
        //tang bien dem client len
        clientCount++;
        cout << buffer << endl;
        cout << inet_ntoa(caddr.sin_addr) << endl;
        cout << ntohs(caddr.sin_port) << endl;
        //lay dan trong danh sach ra de ket noi va xu ly dan
        for (int i = 0; i < clientCount; i++)
        {
            //Xu ly gui du lieu voi tat ca nhung client da chap nhan ket noi
            //Tao ra mot socket xu ly gui du lieu tu server ve client
            SOCKET c = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            //Gui tu du lieu qua socket c den cau truc dia chi ung voi client dang xet voi do dai cua cau truc tuong ung
            sendto(c, buffer, strlen(buffer), 0, (sockaddr *)&caddrs[i], sizeof(SOCKADDR_IN));
            //dong socket
            closesocket(c);
        }
        //int send = sendto(s, welcome, strlen(welcome), 0, (sockaddr*)&toAddr, sizeof(toAddr));
        //in ra so byte da nhan duoc
        cout << "da nhan:" << received;
    }
    return 0;
}
