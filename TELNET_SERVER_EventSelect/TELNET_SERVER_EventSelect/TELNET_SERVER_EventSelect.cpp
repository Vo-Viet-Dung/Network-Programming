#include <WinSock2.h>
#include <iostream>

#define MAX_CLIENT 1024
DWORD WINAPI ClientThread(LPVOID param);
WSAEVENT g_events[MAX_CLIENT];
SOCKET g_sockets[MAX_CLIENT];
int g_count = 0;
int matched[MAX_CLIENT];
//CRITICAL_SECTION cs;

void main()
{
	//Khoi tao thu vien va cau truc dia chi cho server
	WSADATA DATA;
	WSAStartup(MAKEWORD(2, 2), &DATA);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	saddr.sin_port = htons(8888);
	bind(s, (sockaddr *)&saddr, sizeof(saddr));
	listen(s, 10);

	//Tao ra socket va su kien tuong ung cho socket le tan
	g_sockets[g_count] = s;
	g_events[g_count] = WSACreateEvent();
	//Lang nghe su kien Accept tren socket le tan s
	WSAEventSelect(g_sockets[g_count], g_events[g_count], FD_ACCEPT);

	g_count += 1;
	while (0 == 0)
	{
		//Theo doi su kien treen tap g_event (gom g_count doi tuong)
		//Doi vo han (INFINITE)
		int index = WSAWaitForMultipleEvents(g_count, g_events, FALSE, INFINITE, FALSE);
		//Khoa nho nhat duoc mo trong tap g_events
		index = index - WSA_WAIT_EVENT_0; //Smallest index of the opened event
		//Xet tap cac su kien tu su kien co khoa nho nhat duoc mo
		for (int i = index; i < g_count; i++)
		{
			//Tham do su kien voi tung socket tuong ung
			WSANETWORKEVENTS networkEvent;
			WSAEnumNetworkEvents(g_sockets[i], g_events[i], &networkEvent);
			//So sanh su kien tai socket voi co FD_ACCEPT de biet co ket noi toi server hay ko
			if (networkEvent.lNetworkEvents & FD_ACCEPT) //Accept - g_sockets[i]
			{
				//Neu nhu ma ket noi thanh cong thi tien hanh giao tiep client-server
				if (networkEvent.iErrorCode[FD_ACCEPT_BIT] == 0) //No error
				{

					SOCKADDR_IN caddr;
					int clen = sizeof(caddr);
					SOCKET c = accept(s, (sockaddr *)&caddr, &clen);
					//Them doi tuong duoc ket noi den o giua phien
					HANDLE e = WSACreateEvent();
					g_sockets[g_count] = c;
					g_events[g_count] = e;
					WSAEventSelect(g_sockets[g_count], g_events[g_count], FD_READ | FD_CLOSE);
					g_count += 1;
					char *welcome = (char *)"Gui user/pass theo dinh dang ";
					send(c, welcome, strlen(welcome), 0);
					/*CreateThread(NULL,
								 0,
								 ClientThread,
								 (LPVOID)c,
								 0, NULL);*/
					//DeleteCriticalSection(&cs);
				}
			}
			//So sanh su kien tai socket voi co FD_READ de biet rang co hoat dong gui du lieu tu phia client
			if (networkEvent.lNetworkEvents & FD_READ)
			{
				if (networkEvent.iErrorCode[FD_READ_BIT] == 0)
				{
					//Neu lang nghe thanh cong
					SOCKET c = (SOCKET)g_sockets[i]; // do tham chieu den socket c nen can ep kieu

					//Gui chuoi ky tu chao mung huong dan client nhap du lieu

					//Tao ra chuoi ky tu buffer dung de nhan thong tin dang nhap tu phia client gui len server
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer)); //*Chu y: cap phat dong khong duoc su dung sizeof

					//Nhan xau ky tu chua username va password tu phia client gui len
					recv(c, buffer, sizeof(buffer), 0);
					//xu ly khoang trang (TRIM)
					while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n')
					{
						buffer[strlen(buffer) - 1] = 0;
					}
					// cau truc [user pass] => dung ham sscanf de tach username va password
					if (matched[i] == 0) {
						char user[1024];
						char pass[1024];
						memset(user, 0, sizeof(user));
						memset(pass, 0, sizeof(pass));
						sscanf(buffer, "%s%s", user, pass);

						// so sanh voi DB
						//Tao ra co matched de danh dau trang thai dang

						FILE* f = fopen("c:\\telnetuser.txt", "rt");
						while (!feof(f))
						{
							//Lay tung dong trong file telnetuser.txt de so sanh thong tin username va password
							char line[1024];
							memset(line, 0, sizeof(line));
							fgets(line, sizeof(line), f);
							char _u[1024];
							char _p[1024];
							memset(_u, 0, sizeof(_u));
							memset(_p, 0, sizeof(_p));
							sscanf(line, "%s%s", _u, _p);
							if (strcmp(user, _u) == 0 && strcmp(pass, _p) == 0)
							{
								matched[i] = 1;
								break;
							}
						}
						fclose(f);
					}
					if (matched[i] == 0)
					{
						char *failed = (char *)"login that bai \n";
						send(c, failed, strlen(failed), 0);
						closesocket(c);
					}
					else if(matched[i] == 1)
					{
						//Tao va gui chuoi thong bao ket qua dang nhap thanh cong ve cho client
						char *succeded = (char *)"login thanh cong \n";
						send(c, succeded, strlen(succeded), 0);
						matched[i] = 2;
						continue;
					}
					else if (matched[i] == 2)
					{
						while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n')
						{
							buffer[strlen(buffer) - 1] = 0;
						}

						//Ghep chuoi lenh tu phia client vao chuoi lenh ghi du lieu vao file text
						sprintf(buffer + strlen(buffer), "%s", "> E:\\telnet.txt");
						//Chiem quyen su dung file
						// EnterCriticalSection(&cs);
						//Thuc hien lenh bao gom "cu phap cua client" + "ghi du lieu vao file telnet.txt"
						system(buffer);

						//Mo file telnet.txt va lay phan ket qua cua lenh vua thuc hien sau do gui lai cho client
						FILE *f = fopen("E:\\telnet.txt", "rb");
						//dua con tro ve cuoi file
						fseek(f, 0, SEEK_END);
						// lay vi tri con tro => lay so byte chinh xac can gui chinh
						int flen = ftell(f);
						// Dua con tro ve dau file
						fseek(f, 0, SEEK_SET);
						char *fdata = (char *)calloc(flen, 1);
						fread(fdata, 1, flen, f);
						fclose(f);
						//Tra lai quyen su dung file
						//LeaveCriticalSection(&cs);
						// khong gui so byte toi da nua do neu la file anh se co rat nhieu byte 0, nen can gui chinh xac
						send(c, fdata, flen, 0);
						free(fdata);
						closesocket(c);
					}
				}
			}
			//So sanh su kien tai socket voi co FD_CLOSE de biet duoc client co ngat ket noi hay khong
			if (networkEvent.lNetworkEvents & FD_CLOSE)
			{
				printf("A client has disconnected\n");
			}
		}
	}
}

