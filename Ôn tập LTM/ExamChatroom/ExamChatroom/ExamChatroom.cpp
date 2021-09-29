#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
//using namespace std;
#define MAX_CLIENT 100

SOCKET clients[MAX_CLIENT];
int mark = 0;

DWORD WINAPI ClientThread(LPVOID param);
char RoomId[MAX_CLIENT][1024];
int count = 0; //so luong ket noi
int main()
{

    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);

    SOCKADDR_IN saddr;
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    saddr.sin_addr.S_un.S_addr = 0;
    saddr.sin_port = htons(5555);
    saddr.sin_family = AF_INET;
    bind(s, (sockaddr *)&saddr, sizeof(saddr));
    listen(s, 10);
    fd_set fdread;
    while (1)
    {
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
        if (FD_ISSET(s, &fdread))
        {
            SOCKADDR_IN caddr;
            int clen = sizeof(caddr);
            SOCKET tmp = accept(s, (sockaddr*)&caddr, &clen);

            

            SOCKET c = (SOCKET)tmp; // do tham chieu den socket c nen can ep kieu
            char* welcome = (char*)"Nhap so phong theo dinh dang ROOM: <Sophong>\n";
            send(c, welcome, strlen(welcome), 0);

            //while (mark == 0)
            //{
                //Tao ra chuoi ky tu buffer dung de nhan thong tin dang nhap tu phia client gui len server
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer)); //*Chu y: cap phat dong khong duoc su dung sizeof

            //    //Nhan xau ky tu chua username va password tu phia client gui len
                recv(c, buffer, sizeof(buffer), 0);
                //    //xu ly khoang trang (TRIM)
                while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n')
                {
                    buffer[strlen(buffer) - 1] = 0;
                }

                char command[1024];
                char roomId[1024];
                memset(command, 0, sizeof(command));
                memset(roomId, 0, sizeof(roomId));
                sscanf(buffer, "%s%s", command, roomId);
                int room = atoi(roomId);
                if (strcmp("ROOM:", command) == 0 && room > 0 && room < 100)
                {
                    mark = 1;
                    char* succeded = (char*)"login thanh cong \n";
                    send(c, succeded, strlen(succeded), 0);
                    memset(RoomId[count], 0, sizeof(roomId[count]));
                    strcat(RoomId[count], roomId);
                    //sscanf(roomId[count], roomId);
                    clients[count] = c;
                    count += 1;
                }
                else if (strcmp("MSG:", command) == 0) {
                    char* warning = (char*)"Please use ROOM command to join first.\n";
                    send(c, warning, strlen(warning), 0);
                }
                else
                {
                    //Tao va gui chuoi thong bao ket qua dang nhap thanh cong ve cho client
                    char* failed = (char*)"Invalid room number, please try again. \n";
                    send(c, failed, strlen(failed), 0);
                    //Khong dong socket khi gui sai cu phap
                    closesocket(c);
                }
            //}
        }

        //duyet lang nghe tat ca cac client
        char idData[1024];
        for (int i = 0; i < count; i++)
        {
            //Neu client[i] muon gui tin thi nhan tin nhan vao chuoi buffer
            if (FD_ISSET(clients[i], &fdread))
            {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                memset(idData, 0, sizeof(idData));
                strcat(idData, RoomId[i]);
                recv(clients[i], buffer, sizeof(buffer), 0);
                char msgcommand[1024];
                char msgcontent[1024];
                memset(msgcommand, 0, sizeof(msgcommand));
                memset(msgcontent, 0, sizeof(msgcontent));
                sscanf(buffer, "%s%s", msgcommand, msgcontent);

                if (strcmp(buffer, "QUIT\n") == 0)
                {
                    closesocket(clients[i]);
                    for (int k = i; i < count - 1; i++)
                    {
                        clients[i] = clients[i + 1];
                    }
                    count -= 1;
                }
                else if (strcmp(msgcommand, "MSG") != 0 && strcmp(buffer, "QUIT\n") != 0 ){
                    char* warning = (char*)"Invalid command !\n";
                    send(clients[i], warning, strlen(warning), 0);
                    continue;
                }
                else {
                    strcat(idData, ": ");
                    strcat(idData, buffer);
                    for (int j = 0; j < count; j++)
                    {
                        //Sau khi nhan tin nhan thi duyet toan bo mang client, neu khong phai client gui tin thi tien hanh gui thong diep ma server nhan duoc
                        if (j != i)
                        {

                            send(clients[j], idData, strlen(idData), 0);
                        }
                    }
                }

                
            }
        }
    }
}

