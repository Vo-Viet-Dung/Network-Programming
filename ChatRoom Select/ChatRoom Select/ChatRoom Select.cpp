// ChatRoom Select.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<WinSock2.h>
#define MAX_CLIENT 1024
SOCKET clients[MAX_CLIENT];
int count = 0; //so luong ket noi
int main()
{

    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);


    SOCKADDR_IN saddr;
    SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    saddr.sin_addr.S_un.S_addr = INADDR_ANY;
    saddr.sin_port = htons(8888);
    saddr.sin_family = AF_INET;
    bind(s, (sockaddr*)&saddr, sizeof(saddr));
    listen(s, 10);
    fd_set fdread;
    while (1) {
        FD_ZERO(&fdread); // xoa het cac socket di
        //Bo socket le tan vao tap tham do
        FD_SET(s, &fdread);
        /// <summary>
        ///Tao tap set tham do chua cac socket client 
        /// </summary>
        /// <returns></returns>
        for (int i = 0; i < count; i++)
        {
            FD_SET(clients[i], &fdread);
        }

        select(0, &fdread, NULL, NULL, NULL);

        //Xac dinh co ket noi den socket le tan (socket s) va tien hanh them vao tap socket client
        if (FD_ISSET(s, &fdread)) {
            SOCKADDR_IN caddr;
            int clen = sizeof(caddr);
            SOCKET tmp = accept(s, (sockaddr*)&caddr, &clen);





            
            clients[count] = tmp;
            count += 1;

        }
            //duyet lang nghe tat ca cac client
            for (int i = 0; i < count; i++) {
                //Neu client[i] muon gui tin thi nhan tin nhan vao chuoi buffer
                if (FD_ISSET(clients[i], &fdread)) {
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    recv(clients[i], buffer, sizeof(buffer), 0);
                    for (int j = 0; j < count; j++) {
                        //Sau khi nhan tin nhan thi duyet toan bo mang client, neu khong phai client gui tin thi tien hanh gui thong diep ma server nhan duoc
                        if (j != i) {
                            send(clients[j], buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
