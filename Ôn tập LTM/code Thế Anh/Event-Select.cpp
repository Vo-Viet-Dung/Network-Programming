#include<stdio.h>
#include<WinSock2.h>
#define MAX_CLIENT 1024
WSAEVENT g_events[MAX_CLIENT];
int g_count = 0;
SOCKET g_sockets[MAX_CLIENT];

int main()
{
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	saddr.sin_port = htons(8888);
	saddr.sin_family = AF_INET;

	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);

	g_sockets[g_count] = s;

	g_events[g_count] = WSACreateEvent();

	WSAEventSelect(g_sockets[g_count], g_events[g_count], FD_ACCEPT);

	g_count += 1;
	while (0 == 0)
	{
		int index = WSAWaitForMultipleEvents(g_count, g_events, FALSE, INFINITE, FALSE); // treo o diem nay voi thoi gian doi la vo han
		index = index - WSA_WAIT_EVENT_0; // smallest index of open event
		for (int i = index; i < g_count; i++) {
			WSANETWORKEVENTS networkEvent;
			WSAEnumNetworkEvents(g_sockets[i], g_events[i], &networkEvent);
			WSAResetEvent(g_events[i]);

			if (networkEvent.lNetworkEvents & FD_ACCEPT) { // co su kien accept tren scoket i
				if (networkEvent.iErrorCode[FD_ACCEPT_BIT] == 0) // no error
				{
					SOCKADDR_IN caddr;
					int clen = sizeof(caddr);
					SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
					HANDLE e = WSACreateEvent();
					g_sockets[g_count] = c;
					g_events[g_count] = e;

					// send instruction
					char* welcome = (char*)"Send command to execute: \n";
					send(c, welcome, strlen(welcome), 0);

					WSAEventSelect(g_sockets[g_count], g_events[g_count], FD_READ | FD_CLOSE);
					g_count++;
				}
			}

			if (networkEvent.lNetworkEvents & FD_READ) {
				if (networkEvent.iErrorCode[FD_READ_BIT] == 0) {
					//recv command
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					recv(g_sockets[i], buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);

					// trim command
					while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n') {
						buffer[strlen(buffer) - 1] = 0;
					}

					FILE* f = fopen("d:\\result\\result.txt", "wt");
					fclose(f);

					sprintf(buffer + strlen(buffer), "%s", "> d:\\result\\result.txt"); // viet ket qua vao file result.txt
					system(buffer);

					// doc file va tra ket qua ve cho nguoi dung
					f = fopen("d:\\result\\result.txt", "rt");
					char result[1000000];
					memset(result, 0, sizeof(result));

					int rlen = 0;

					while (!feof(f)) {
						char line[1024];
						memset(line, 0, sizeof(line));
						fgets(line, sizeof(line), f);

						sprintf(result + rlen, "%s\n", line);
						rlen += strlen(line);
					}

					printf("%s\n", result);

					send(g_sockets[i], result, strlen(result), 0);


					/*
					
					for (int j = 1; j < g_count; j++) {
						if (j != i) {
							send(g_sockets[j], buffer, strlen(buffer), 0);
						}
					}

					*/

					fclose(f);

				}
			}

			if (networkEvent.lNetworkEvents & FD_CLOSE) {
					printf("A client has disconnected.\n");
				
			}
		}


	}
}