// EventSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<WinSock2.h>
#define MAX_CLIENT 1024
WSAEVENT g_events[MAX_CLIENT];
int g_count = 0;
SOCKET g_socket[MAX_CLIENT];


int main()
{
    WSADATA DATA;
    WSAStartup(MAKEWORD(2, 2), &DATA);
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr;
    saddr.sin_addr.S_un.S_addr = INADDR_ANY;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    bind(s, (sockaddr*)&saddr, sizeof(saddr));
    listen(s, 10);
    g_socket[g_count] = s;
    g_events[g_count++] = WSACreateEvent();
    WSAEventSelect(g_socket[g_count], g_events[g_count], FD_ACCEPT);
    g_count += 1;
    while (0 == 0) {
        int index = WSAWaitForMultipleEvents(g_count, g_events, FALSE, INFINITE, FALSE);
        index = index - WSA_WAIT_EVENT_0;
        for (int i = index; i < g_count; i++) {
            WSANETWORKEVENTS networkEvent;
            WSAEnumNetworkEvents(g_socket[i], g_events[i], &networkEvent);
            if (networkEvent.lNetworkEvents & FD_ACCEPT) {
                if (networkEvent.iErrorCode[FD_ACCEPT_BIT] == 0) {
                    SOCKADDR_IN caddr;
                    int clen = sizeof(caddr);
                    SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
                    HANDLE e = WSACreateEvent();
                    g_socket[g_count] = c;
                    g_events[g_count] = e;
                    WSAEventSelect(g_socket[g_count], g_events[g_count], FD_READ | FD_CLOSE);
                    g_count += 1;
                }
                if (networkEvent.lNetworkEvents & FD_READ) {
                    if (networkEvent.iErrorCode[FD_READ_BIT] == 0) {
                        char buffer[1024];
                        memset(buffer, 0, sizeof(buffer));
                        recv(g_socket[i], buffer, sizeof(buffer), 0);
                        printf("%s\n", buffer);
                        for (int j = 0; j < g_count; j++) {
                            if (j != i) {
                                send(g_socket[j], buffer, strlen(buffer),0);
                            }
                        }
                    }
                }
                if (networkEvent.lNetworkEvents & FD_CLOSE) {
                    if (networkEvent.iErrorCode[FD_CLOSE_BIT] == 0) {
                        
                        printf("Dong ket noi");
                    }
                }
            }
        }
    }
}

