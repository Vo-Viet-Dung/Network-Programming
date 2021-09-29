#include <iostream>
#include<WinSock2.h>

using namespace std;
DWORD WINAPI ClientThread(LPVOID param);
CRITICAL_SECTION cs;

void main()
{
    
        //chay lenh dir dau va ket qua dua vao file telnet.txt
        //system("dir c:\\temp > c:\\Users\admin\OneDrive\Documents\telnet.txt");
        int count = 0;
        //Khoi tao thu vien va cau truc dia chi cho server
        WSADATA DATA;
        WSAStartup(MAKEWORD(2, 2), &DATA);
        //hoi tao cau truc dia chi va socket le tan lam nhiem vu chap nhan ket noi
        SOCKADDR_IN saddr;
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        saddr.sin_family = AF_INET;
        saddr.sin_addr.S_un.S_addr = INADDR_ANY;
        saddr.sin_port = htons(5555);
        bind(s, (sockaddr*)&saddr, sizeof(saddr));
        listen(s, 32);
        //doi ket noi
        while (1) {

            //Tao cau truc dia chi va socket xu ly du lieu
   
            SOCKADDR_IN caddr;
            // Khong dung sizeof voi nhung bien con tro
            int clen = sizeof(caddr);
            SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
            // Giao tiep voi socket c
            cout << inet_ntoa(caddr.sin_addr);
            CreateThread(NULL, 0, ClientThread, (LPVOID)c, 0, NULL);
         
        }
}
int SendData(SOCKET c, char* data, int len)
{
    int byteSent = 0;
    while (byteSent < len)
    {
        byteSent += send(c, data + byteSent, len - byteSent, 0);
    }
    return byteSent;
}

DWORD WINAPI ClientThread(LPVOID param) {
    //Ep kieu socket cho tham so truyen vao
    SOCKET c = (SOCKET)param; // do tham chieu den socket c nen can ep kieu
    //SOCKADDR_IN caddr = param.caddr;
    //printf("%s", caddr.sin_addr);
    //Gui chuoi ky tu chao mung huong dan client nhap du lieu
    char* welcome = (char*)"Nhap lenh theo cu phap GET FILENAME PORT\n";
    send(c, welcome, strlen(welcome), 0);

    //Tao ra chuoi ky tu buffer dung de nhan thong tin dang nhap tu phia client gui len server
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer)); //*Chu y: cap phat dong khong duoc su dung sizeof
    while (1) {
        //Nhan xau ky tu chua username va password tu phia client gui len
        recv(c, buffer, sizeof(buffer), 0);
        //xu ly khoang trang (TRIM)
        while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = 0;
        }
        char command[1024];
        char fileName[1024];
        char port[128];
        memset(command, 0, sizeof(command));
        memset(fileName, 0, sizeof(fileName));
        memset(port, 0, sizeof(port));
        sscanf(buffer, "%s%s%s", command, fileName, port);
        int portNumber = atoi(port);
        if (strcmp(command, "GET") == 0 && strcmp(fileName, "") != 0 && strcmp(command, "") != 0)
        {
            char* succeded = (char*)"login thanh cong \n";
            memset(buffer, 0, sizeof(buffer));
            send(c, succeded, strlen(succeded), 0);
            FILE* f = fopen("E:\\telnet.txt", "rt");
            //char* fdata;
            //int flen = 0;
            //while (!feof(f)) {
            //    //Lay tung dong trong file telnetuser.txt de so sanh thong tin username va password
            //    char line[1024];
            //    memset(line, 0, sizeof(line));
            //    fgets(line, sizeof(line), f);
            //    flen += sizeof(line);
            //    strcat(fdata, line);
            //}
            //dua con tro ve cuoi file
            fseek(f, 0, SEEK_END);
            // lay vi tri con tro => lay so byte chinh xac can gui chinh
            int flen = ftell(f);
            // Dua con tro ve dau file
            fseek(f, 0, SEEK_SET);
            char* fdata = (char*)calloc(flen, 1);
            fread(fdata, 1, flen, f);
            fclose(f);
            SendData(c, fdata, flen);
            free(fdata);
        }
        else if (strncmp(buffer, "QUIT", 4) == 0)
        {
            closesocket(c);
            break;
        }
        else
        {
            memset(buffer, 0, sizeof(buffer));
            sprintf_s(buffer, sizeof(buffer), "INVALID COMMAND\r\n");
            send(c, buffer, strlen(buffer), 0);
        }
    }
    return 0;
}