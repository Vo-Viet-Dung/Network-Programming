#include<stdio.h>
#include<WinSock2.h>
#include<Windows.h>
#include "mytelnets.h"

CRITICAL_SECTION cs;

DWORD WINAPI ClientThread(LPVOID param) {
	SOCKET c = (SOCKET)param;
	char* welcome = (char*)"Gui user/password theo dinh dang [user password]\n";
	send(c, welcome, strlen(welcome), 0);
	
	char buffer[1024]; // cap phat mang char
	memset(buffer, 0, sizeof(buffer));
	recv(c, buffer, sizeof(buffer), 0);

	// Trim

	while(buffer[strlen(buffer) - 1] == '\r'
		||buffer[strlen(buffer) - 1] == '\n'){
		buffer[strlen(buffer) - 1] = 0;
	}

	char user[1024];
	char password[1024];
	memset(user, 0, sizeof(user));
	memset(password, 0, sizeof(password));

	sscanf(buffer, "%s%s", user, password);

	// matching
	FILE* f = fopen("d:\\temp\\Telnet.txt", "rt"); // read text
	int matched = 0;

	while (!feof(f)) {
		char line[1024];
		memset(line, 0, sizeof(line));
		fgets(line, sizeof(line), f);
		char _u[1024];
		char _p[1024];

		memset(_u, 0, sizeof(_u));
		memset(_p, 0, sizeof(_p));

		sscanf(line, "%s%s", _u, _p);
		if (strcmp(user, _u) == 0 && strcmp(password, _p) == 0) {
			matched = 1;
			break;
		}
	}

	fclose(f);


	if (matched == 0) {
		char* failed = (char*)"Failed to login\n";
		send(c, failed, strlen(failed), 0);
		closesocket(c);
	}
	else {
		char* succeeded = (char*)"Please send command to execute\n";
		send(c, succeeded, strlen(succeeded), 0);

		memset(buffer, 0, sizeof(buffer));
		recv(c, buffer, sizeof(buffer), 0);

		// tien xu ly
		while (buffer[strlen(buffer) - 1] == '\r'
			|| buffer[strlen(buffer) - 1] == '\n') {
			buffer[strlen(buffer) - 1] = 0;
		}

		sprintf(buffer + strlen(buffer), "%s", "> d:\\temp\\result.txt"); // in vao cuoi xau
		EnterCriticalSection(&cs);
		system(buffer);

		FILE* f = fopen("d:\\temp\\result.txt", "rb"); // read file binary se ko co eof

		fseek(f, 0, SEEK_END); // dua con tro ve cuoi file
		int flen = ftell(f);  // lay vi tri con tro
		fseek(f, 0, SEEK_SET); // dua con tro tu vi tri dau

		// cap phat dong
		char* fdata = (char*)calloc(flen, 1); // cap phat dong
		fread(fdata, 1, flen, f);				// doc va luu vao fdata

		fclose(f);
		LeaveCriticalSection(&cs);

		send(c, fdata, flen, 0); // ko dung  strlen voi file binary do co nhieu byte 0
		free(fdata);
		closesocket(c);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Thieu tham so.\n");
		return 0;
	}
	else {
		short port = atoi(argv[1]);

		//system("dir d:\\temp >> d:\\temp\\Telnet.txt");

		
		InitializeCriticalSection(&cs);
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);
		SOCKADDR_IN saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(port);
		saddr.sin_addr.S_un.S_addr = INADDR_ANY;

		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		bind(s, (sockaddr*)&saddr, sizeof(saddr));

		listen(s, 10);

		while (1) {
			SOCKADDR_IN caddr;
			int clen = sizeof(caddr);
			SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
			CreateThread(NULL, 
				0, 
				ClientThread, 
				(LPVOID)c, 
				0, 
				NULL);
		}
		DeleteCriticalSection(&cs);
	}
}