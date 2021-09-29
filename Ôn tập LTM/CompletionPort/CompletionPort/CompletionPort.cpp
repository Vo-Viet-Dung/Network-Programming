// CompletionPort.cpp : Defines the entry point for the console application.
//
#include<stdio.h>
#include <stdafx.h>
#include <WinSock2.h>

#define DATA_BUFSIZE 1024
#define RECV_POSTED 1000
#define SEND_POSTED 1001

typedef struct _PER_HANDLE_DATA
{
	SOCKET			Socket;
	SOCKADDR_STORAGE  ClientAddr;
} PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

typedef struct
{
	OVERLAPPED	Overlapped;
	WSABUF		Buffer;
	int			OperationType;
} PER_IO_DATA, * LPPER_IO_DATA;

HANDLE CompletionPort;
WSADATA wsd;
SYSTEM_INFO SystemInfo;
SOCKADDR_IN InternetAddr;
SOCKET Listen;
int i;

SOCKET Clients[1024];
int nClient = 0;
PER_IO_DATA* per_io_data[1024];

DWORD WINAPI ServerWorkerThread(LPVOID CompletionPortID)
{
	HANDLE CompletionPort = (HANDLE)CompletionPortID;
	DWORD BytesTransferred;
	LPOVERLAPPED Overlapped;
	LPPER_HANDLE_DATA PerHandleData;
	LPPER_IO_DATA PerIoData;
	DWORD SendBytes, RecvBytes;
	DWORD Flags;

	while (TRUE)
	{
		// Wait for I/O to complete on any socket
		// associated with the completion port

		BOOL ret = GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (LPDWORD)&PerHandleData, (LPOVERLAPPED*)&PerIoData, INFINITE);

		if (BytesTransferred == 0 && (PerIoData->OperationType == RECV_POSTED || PerIoData->OperationType == SEND_POSTED))
		{
			closesocket(PerHandleData->Socket);
			GlobalFree(PerHandleData);
			GlobalFree(PerIoData);
			continue;
		}

		if (PerIoData->OperationType == RECV_POSTED)
		{
			printf("RECV: %d: %s\n", PerHandleData->Socket, PerIoData->Buffer.buf);
			for (int i = 0; i < nClient; i++)
			{
				if (Clients[i] != PerHandleData->Socket)
				{
					DWORD Bytes, Flags;
					memcpy(per_io_data[i]->Buffer.buf, PerIoData->Buffer.buf, BytesTransferred);
					per_io_data[i]->Buffer.len = BytesTransferred;
					ZeroMemory(&(per_io_data[i]->Overlapped), sizeof(OVERLAPPED));
					Flags = 0;
					per_io_data[i]->OperationType = SEND_POSTED;
					WSASend(Clients[i], &(per_io_data[i]->Buffer), 1, &Bytes, Flags, &(per_io_data[i]->Overlapped), NULL);

				}
			}
			memset(PerIoData->Buffer.buf, 0, DATA_BUFSIZE);
		}

		if (PerIoData->OperationType == SEND_POSTED)
		{
			printf("SEND: %d Bytes\n", BytesTransferred);
			memset(PerIoData->Buffer.buf, 0, DATA_BUFSIZE);
		}

		Flags = 0;

		ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));

		PerIoData->OperationType = RECV_POSTED;

		WSARecv(PerHandleData->Socket, &(PerIoData->Buffer), 1, &RecvBytes, &Flags, &(PerIoData->Overlapped), NULL);
	}

	return 0;
}

int main()
{
	WSADATA DATA;
	WSAStartup(MAKEWORD(2, 2), &DATA);
	// Step 1: Create an I/O completion port
	CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	// Step 2: Determine how many processors are on the system
	GetSystemInfo(&SystemInfo);
	// Step 3: Create pooling threads
	for (i = 0; i < SystemInfo.dwNumberOfProcessors; i++)
	{
		HANDLE ThreadHandle;
		ThreadHandle = CreateThread(NULL, 0, ServerWorkerThread, CompletionPort, 0, NULL);
		CloseHandle(ThreadHandle);
	}
	//Step 4: Create Async Socket
	Listen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(8888);
	bind(Listen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));
	listen(Listen, 5);
	while (0 == 0)
	{
		PER_HANDLE_DATA* PerHandleData = NULL;
		SOCKADDR_IN saRemote;
		SOCKET Accept;
		int RemoteLen = sizeof(saRemote);
		// Step 5: Accept connections and assign to the completion port		
		Accept = WSAAccept(Listen, (SOCKADDR*)&saRemote, &RemoteLen, NULL, NULL);
		// Step 6: Create per-handle data information structure to  associate with the socket
		PerHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
		printf("Socket number %d connected\n", Accept);
		PerHandleData->Socket = Accept;
		memcpy(&PerHandleData->ClientAddr, &saRemote, RemoteLen);
		// Step 7: Associate the accepted socket with the completion port
		CreateIoCompletionPort((HANDLE)Accept, CompletionPort, (DWORD)PerHandleData, 0);
		// Step 8: IO Operation
		Clients[nClient] = Accept;
		per_io_data[nClient] = (PER_IO_DATA*)calloc(1, sizeof(PER_IO_DATA));
		DWORD Bytes, Flags;
		per_io_data[nClient]->Buffer.buf = (char*)calloc(DATA_BUFSIZE, 1);
		per_io_data[nClient]->OperationType = SEND_POSTED;
		strcpy(per_io_data[nClient]->Buffer.buf, "Welcome to CompletionPORT\n");
		per_io_data[nClient]->Buffer.len = strlen(per_io_data[nClient]->Buffer.buf);
		ZeroMemory(&(per_io_data[nClient]->Overlapped), sizeof(OVERLAPPED));
		Flags = 0;
		WSASend(PerHandleData->Socket, &(per_io_data[nClient]->Buffer), 1, &Bytes, Flags, &(per_io_data[nClient]->Overlapped), NULL);
		nClient++;
	}
	return 0;
}