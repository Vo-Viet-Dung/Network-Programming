
#include <iostream>
#include<WinSock2.h>
using namespace std;
int main()
{
    //Khoi tao bien DATA nham phuc vu khoi tao thu vien
    WSADATA DATA;
    //Khoi tao thu vien Winsock
    WSAStartup(MAKEWORD(2, 2), &DATA);

    //Khoi tao socket de truyen du lieu
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        //Neu khong khoi tao duoc
        cout << "Khong tao duoc Socket, ma loi: " << WSAGetLastError()<<endl;
    }
    else {
        //khoi tao doi tuong muon noi den
        //saddr: server address
        SOCKADDR_IN saddr;
        //Giao thuc ipv4
        saddr.sin_family = AF_INET;
        //Cong dich vu 8888
        saddr.sin_port = htons(8888);
        //noi den boi bat ky dia chi ip nao
        saddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//0.0.0.0

        //Ghep socket voi giao dien mang tuong ung
        bind(s, (sockaddr*)&saddr, sizeof(saddr));
        //Doi ket noi tren socket s
        listen(s, 10);

        //caddr:client address
        //danh cho client khi ket noi den server
        SOCKADDR_IN caddr;
        int clen = sizeof(caddr);
        //socket c dam nhan vai tro xu ly truyen du lieu
        SOCKET c = accept(s, (sockaddr*)&caddr, &clen );
        char* hello = (char*)"HELLO PROGRAMMING\n";
        //Gui chuoi Hello programming tu server cho client
        int byteSend = send(c,hello, strlen(hello), 0);
        //phan tich chuoi da gui tu phia server cho client
        cout << "So byte da gui: " << byteSend << endl;

        //tao buffer chuan bi nhan data tu phia client gui len
        char data[1024]; //Cap phat tinh 1024 bytes
        memset(data, 0, sizeof(data));
        //Nhan du lieu tu phia client gui len
        int byteReceived = recv(c, data, 1024, 0);
        //phan tich chuoi gui len
        cout << "So byte nhan duoc: " << byteReceived << endl;
        cout << data;
        //Xu ly dong ket noi
        free(data);
        closesocket(c);
        closesocket(s);
        WSACleanup();
    }
}


