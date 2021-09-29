
#include <iostream>
#include<WinSock2.h>
#define MAX_CLIENT 1024
SOCKET clients[MAX_CLIENT];
int count = 0; //so luong ket noi
DWORD WINAPI ClientThread(LPVOID param);
int main()
{

    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);


    SOCKADDR_IN saddr;
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
           
//========================================================================================================================================================
//Chuc nang dang nhap de xac nhan client ket noi
//=========================================================================================================================================================

            SOCKET c = (SOCKET)tmp; // do tham chieu den socket c nen can ep kieu

            //Gui chuoi ky tu chao mung huong dan client nhap du lieu
            char* welcome = (char*)"Gui user/pass theo dinh dang \n";
            send(c, welcome, strlen(welcome), 0);

            //Tao ra chuoi ky tu buffer dung de nhan thong tin dang nhap tu phia client gui len server
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer)); //*Chu y: cap phat dong khong duoc su dung sizeof

            //Nhan xau ky tu chua username va password tu phia client gui len
            recv(c, buffer, sizeof(buffer), 0);
            //xu ly khoang trang (TRIM)
            while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n') {
                buffer[strlen(buffer) - 1] = 0;
            }
            // cau truc [user pass] => dung ham sscanf de tach username va password
            char user[1024];
            char pass[1024];
            memset(user, 0, sizeof(user));
            memset(pass, 0, sizeof(pass));
            sscanf(buffer, "%s%s", user, pass);

            // so sanh voi DB
            //Tao ra co matched de danh dau trang thai dang
            int matched = 0;
            FILE* f = fopen("c:\\telnetuser.txt", "rt");
            while (!feof(f)) {
                //Lay tung dong trong file telnetuser.txt de so sanh thong tin username va password
                char line[1024];
                memset(line, 0, sizeof(line));
                fgets(line, sizeof(line), f);
                char _u[1024];
                char _p[1024];
                memset(_u, 0, sizeof(_u));
                memset(_p, 0, sizeof(_p));
                sscanf(line, "%s%s", _u, _p);
                if (strcmp(user, _u) == 0 && strcmp(pass, _p) == 0) {
                    matched = 1;
                    break;
                }
            }
            fclose(f);
            if (matched == 0) {
                char* failed = (char*)"login that bai \n";
                send(c, failed, strlen(failed), 0);
                closesocket(c);
            }
            else {
                //Tao va gui chuoi thong bao ket qua dang nhap thanh cong ve cho client
                char* succeded = (char*)"login thanh cong \n";
                send(c, succeded, strlen(succeded), 0);
                clients[count] = c;
                count += 1;

            }
        }
//==============================================================================================================================
//Dang nhap
//==============================================================================================================================
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

DWORD WINAPI ClientThread(LPVOID param) {
    //Ep kieu socket cho tham so truyen vao
    SOCKET c = (SOCKET)param; // do tham chieu den socket c nen can ep kieu

    //Gui chuoi ky tu chao mung huong dan client nhap du lieu
    char* welcome = (char*)"Gui user/pass theo dinh dang \n";
    send(c, welcome, strlen(welcome), 0);

    //Tao ra chuoi ky tu buffer dung de nhan thong tin dang nhap tu phia client gui len server
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer)); //*Chu y: cap phat dong khong duoc su dung sizeof

    //Nhan xau ky tu chua username va password tu phia client gui len
    recv(c, buffer, sizeof(buffer), 0);
    //xu ly khoang trang (TRIM)
    while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = 0;
    }
    // cau truc [user pass] => dung ham sscanf de tach username va password
    char user[1024];
    char pass[1024];
    memset(user, 0, sizeof(user));
    memset(pass, 0, sizeof(pass));
    sscanf(buffer, "%s%s", user, pass);

    // so sanh voi DB
    //Tao ra co matched de danh dau trang thai dang
    int matched = 0;
    FILE* f = fopen("c:\\telnetuser.txt", "rt");
    while (!feof(f)) {
        //Lay tung dong trong file telnetuser.txt de so sanh thong tin username va password
        char line[1024];
        memset(line, 0, sizeof(line));
        fgets(line, sizeof(line), f);
        char _u[1024];
        char _p[1024];
        memset(_u, 0, sizeof(_u));
        memset(_p, 0, sizeof(_p));
        sscanf(line, "%s%s", _u, _p);
        if (strcmp(user, _u) == 0 && strcmp(pass, _p) == 0) {
            matched = 1;
            break;
        }
    }
    fclose(f);
    if (matched == 0) {
        char* failed = (char*)"login that bai \n";
        send(c, failed, strlen(failed), 0);
        closesocket(c);
    }
    else {
        //Tao va gui chuoi thong bao ket qua dang nhap thanh cong ve cho client
        char* succeded = (char*)"login thanh cong \n";
        send(c, succeded, strlen(succeded), 0);
        clients[count] = c;
        count += 1;
        
    }
    return 0;
}