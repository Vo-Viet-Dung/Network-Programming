#include<WinSock2.h>
#include <windows.h>
#include <iostream>
#include<string>
#include<sstream>
#pragma comment(lib, "user32.lib")
using namespace std;
int main(int argc, char* argv[])
{
	if (argc <3) {
		return 0;
	}
	else {
		//khoi tao thu vien
		WSADATA DATA;
		int error = WSAStartup(MAKEWORD(2, 2), &DATA);
		if (error == SOCKET_ERROR) {
			cout << "loi khoi dong";

		}
		else {
			//neu khoi tao thanh cong thi lay ra IP va Port
			char* ipaddr = argv[1];
			char* cPort = argv[2];

			//chuyen port tu ma ascii ve interger
			short port = (short)atoi(cPort);
			//Khoi tao cau truc dia chi cho server
			SOCKADDR_IN saddr;
			saddr.sin_family = AF_INET;
			saddr.sin_port = htons(port);
			saddr.sin_addr.S_un.S_addr = inet_addr(ipaddr);
			//tao ra 1 xau buffer de chua chuoi ky tu can gui
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			SYSTEM_INFO siSysInfo;
			GetSystemInfo(&siSysInfo);

			//lay ra thong tin may tinh va chuyen tu dang so ve dang chuoi
			char nbOfProcessor[20];
			memset(nbOfProcessor, 0, sizeof(nbOfProcessor));
			itoa(siSysInfo.dwNumberOfProcessors, nbOfProcessor, 10);
			
			char OemId[20];
			memset(OemId, 0, sizeof(OemId));
			itoa(siSysInfo.dwOemId, OemId, 10);

			//Ghep cac xau can gui vao xau buffer
			char ProcessorType[20];
			memset(ProcessorType, 0, sizeof(ProcessorType));
			itoa(siSysInfo.dwProcessorType, ProcessorType, 10);
			strcat(buffer, "Hardware information: \n");
			
			strcat(buffer, "  OEM ID:");
			strcat(buffer, OemId);
			strcat(buffer, "\n");

			strcat(buffer, "  Number of processors:");
			strcat(buffer, nbOfProcessor);
			strcat(buffer, "\n");

			strcat(buffer, " Processor type:");
			strcat(buffer, ProcessorType);
			strcat(buffer, "\n");

			//Tao ra socket tiep nhan cua server
			SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			//Ket noi den server qua ham connet
			//Ket noi den cau truc dia chi saddr qua socket s 
			int res = connect(s, (sockaddr*)&saddr, sizeof(saddr));
			if (res != SOCKET_ERROR) {
				//Neu ket noi thanh cong thi gui xau infor
				send(s, buffer, strlen(buffer),0);

			}
			else {
				printf("Socket error!");
			}

		}
	}
}

