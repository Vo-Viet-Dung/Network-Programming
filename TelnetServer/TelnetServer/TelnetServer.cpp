

//them tham so dong lenh tai property
#include <iostream>
#include<WinSock2.h>

using namespace std;
DWORD WINAPI ClientThread(LPVOID param);
CRITICAL_SECTION cs;
void main(int argc, char** argv)
{
    if (argc < 2)
        return;
    else {
        short port = atoi(argv[1]);
        //chay lenh dir dau va ket qua dua vao file telnet.txt
        //system("dir c:\\temp > c:\\Users\admin\OneDrive\Documents\telnet.txt");

        //Khoi tao thu vien va cau truc dia chi cho server
        WSADATA DATA;
        WSAStartup(MAKEWORD(2, 2), &DATA);
        //hoi tao cau truc dia chi va socket le tan lam nhiem vu chap nhan ket noi
        SOCKADDR_IN saddr;
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        saddr.sin_family = AF_INET;
        saddr.sin_addr.S_un.S_addr = INADDR_ANY;
        saddr.sin_port = htons(port);
        bind(s, (sockaddr*)&saddr, sizeof(saddr));
        listen(s, 10);
        //doi ket noi
        while (1) {

            //Tao cau truc dia chi va socket xu ly du lieu 
            SOCKADDR_IN caddr;
            // Khong dung sizeof voi nhung bien con tro
            int clen = sizeof(caddr);

            SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
            // Giao tiep voi socket c
            CreateThread(NULL, 0, ClientThread, (LPVOID)c, 0, NULL);

        }
    }

}
// Loi ma khong giai thich duoc thi 99% la do vung nho
DWORD WINAPI ClientThread(LPVOID param) {
    //Ep kieu socket cho tham so truyen vao
    SOCKET c = (SOCKET)param; // do tham chieu den socket c nen can ep kieu

    //Gui chuoi ky tu chao mung huong dan client nhap du lieu
    char* welcome = (char*)"Gui user/pass theo dinh dang ";
    send(c, welcome, strlen(welcome), 0);

    //Tao ra chuoi ky tu buffer dung de nhan thong tin dang nhap tu phia client gui len server
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer)); //*Chu y: cap phat dong khong duoc su dung sizeof

    //Nhan xau ky tu chua username va password tu phia client gui len
    recv(c, buffer, sizeof(buffer), 0);
    //xu ly khoang trang (TRIM)
    while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer)-1] = 0;
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

        //Nhan chuoi ky tu tu client gui len server
        memset(buffer, 0, sizeof(buffer));
        recv(c, buffer, sizeof(buffer), 0);
        // chuyen con tro den vi tri sau ki tu cuoi cung cua xau
        while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer)-1] = 0;
        }

        //Ghep chuoi lenh tu phia client vao chuoi lenh ghi du lieu vao file text
        sprintf(buffer + strlen(buffer), "%s", "> E:\\telnet.txt");
        //Chiem quyen su dung file
       // EnterCriticalSection(&cs);
        //Thuc hien lenh bao gom "cu phap cua client" + "ghi du lieu vao file telnet.txt"
        system(buffer);

        //Mo file telnet.txt va lay phan ket qua cua lenh vua thuc hien sau do gui lai cho client
        FILE* f = fopen("E:\\telnet.txt", "rb");
        //dua con tro ve cuoi file
        fseek(f, 0, SEEK_END);
        // lay vi tri con tro => lay so byte chinh xac can gui chinh
        int flen = ftell(f);
        // Dua con tro ve dau file
        fseek(f, 0, SEEK_SET);
        char* fdata = (char*)calloc(flen, 1);
        fread(fdata, 1, flen, f);
        fclose(f);
        //Tra lai quyen su dung file
        //LeaveCriticalSection(&cs);
        // khong gui so byte toi da nua do neu la file anh se co rat nhieu byte 0, nen can gui chinh xac
        send(c, fdata, flen, 0);
        free(fdata);
        closesocket(c);
    }
    return 0;
}