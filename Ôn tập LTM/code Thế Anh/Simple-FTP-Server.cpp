#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>

#define MAX_CLIENT 1024
WSAEVENT g_events[MAX_CLIENT];
SOCKET g_sockets[MAX_CLIENT];
char g_wd[MAX_CLIENT][1000];
int g_count = 0;
unsigned char g_ip1[MAX_CLIENT];
unsigned char g_ip2[MAX_CLIENT];
unsigned char g_ip3[MAX_CLIENT];
unsigned char g_ip4[MAX_CLIENT];
unsigned char g_p1[MAX_CLIENT];
unsigned char g_p2[MAX_CLIENT];

SOCKET c1 = NULL; // socket accept
int acceptIndex = 0;
char* Month(int month)
{
	char* result = (char*)calloc(10, 1);
	switch (month)
	{
	case 1:
		strcpy(result, "Jan"); break;
	case 2:
		strcpy(result, "Feb"); break;
	case 3:
		strcpy(result, "Mar"); break;
	case 4:
		strcpy(result, "Apr"); break;
	case 5:
		strcpy(result, "May"); break;
	case 6:
		strcpy(result, "Jun"); break;
	case 7:
		strcpy(result, "Jul"); break;
	case 8:
		strcpy(result, "Aug"); break;
	case 9:
		strcpy(result, "Sep"); break;
	case 10:
		strcpy(result, "Oct"); break;
	case 11:
		strcpy(result, "Nov"); break;
	case 12:
		strcpy(result, "Dec"); break;
	}
	return result;
}

char* ScanFolderLIST(const char* folder) //"/"
{
	char* html = NULL;
	WIN32_FIND_DATAA FindData;
	char findpath[1024]; //"C:\*.*"
	char oneline[1024];
	memset(findpath, 0, sizeof(findpath));
	if (folder[strlen(folder) - 1] == '/')
		sprintf(findpath, "C:%s*.*", folder);
	else
		sprintf(findpath, "C:%s/*.*", folder);

	HANDLE hFind = FindFirstFileA(findpath, &FindData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		SYSTEMTIME st;
		FileTimeToSystemTime(&(FindData.ftLastWriteTime), &st);
		memset(oneline, 0, sizeof(oneline));
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			sprintf(oneline, "drwx------ 1 ftp ftp              0 %s %02d   %04d %s\r\n", Month(st.wMonth), st.wDay, st.wYear, FindData.cFileName);
		}
		else
		{
			sprintf(oneline, "drwx------ 1 ftp ftp              %d %s %02d   %04d %s\r\n", (__int64)(FindData.nFileSizeHigh << 32 | FindData.nFileSizeLow), Month(st.wMonth), st.wDay, st.wYear, FindData.cFileName);
		int oldlen = html == NULL ? 0 : strlen(html);
		html = (char*)realloc(html, oldlen + strlen(oneline) + 1);
		strcpy(html + oldlen, oneline);

		while (FindNextFileA(hFind, &FindData))
		{
			SYSTEMTIME st;
			FileTimeToSystemTime(&(FindData.ftLastWriteTime), &st);
			memset(oneline, 0, sizeof(oneline));
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sprintf(oneline, "drwx------ 1 ftp ftp              0 %s %02d   %04d %s\r\n", Month(st.wMonth), st.wDay, st.wYear, FindData.cFileName);
			}
			else
			{
				sprintf(oneline, "drwx------ 1 ftp ftp              %d %s %02d   %04d %s\r\n", (__int64)(FindData.nFileSizeHigh << 32 | FindData.nFileSizeLow), Month(st.wMonth), st.wDay, st.wYear, FindData.cFileName);
			}
			int oldlen = html == NULL ? 0 : strlen(html);
			html = (char*)realloc(html, oldlen + strlen(oneline) + 1);
			strcpy(html + oldlen, oneline);
		}
	}
	return html;
}

char* ScanFolder(const char* folder) //"/"
{
	char* html = NULL;
	WIN32_FIND_DATAA FindData;
	char findpath[1024]; //"C:\*.*"
	char oneline[1024];
	memset(findpath, 0, sizeof(findpath));
	if (folder[strlen(folder) - 1] == '/')
		sprintf(findpath, "C:%s*.*", folder);
	else
		sprintf(findpath, "C:%s/*.*", folder);

	HANDLE hFind = FindFirstFileA(findpath, &FindData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		SYSTEMTIME st;
		FileTimeToSystemTime(&(FindData.ftLastWriteTime), &st);
		memset(oneline, 0, sizeof(oneline));
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			sprintf(oneline, "type=dir;modify=%4d%02d%02d%02d%02d%02d; %s\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, FindData.cFileName);
		}
		else
		{
			sprintf(oneline, "type=file;modify=%4d%02d%02d%02d%02d%02d; %s\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, FindData.cFileName);
		}
		int oldlen = html == NULL ? 0 : strlen(html);
		html = (char*)realloc(html, oldlen + strlen(oneline) + 1);
		strcpy(html + oldlen, oneline);

		while (FindNextFileA(hFind, &FindData))
		{
			SYSTEMTIME st;
			FileTimeToSystemTime(&(FindData.ftLastWriteTime), &st);
			memset(oneline, 0, sizeof(oneline));
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sprintf(oneline, "type=dir;modify=%4d%02d%02d%02d%02d%02d; %s\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, FindData.cFileName);
			}
			else
			{
				sprintf(oneline, "type=file;modify=%4d%02d%02d%02d%02d%02d; %s\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, FindData.cFileName);
			}
			int oldlen = html == NULL ? 0 : strlen(html);
			html = (char*)realloc(html, oldlen + strlen(oneline) + 1);
			strcpy(html + oldlen, oneline);
		}
	}
	return html;
}

DWORD WINAPI AcceptThread(LPVOID param)
{
	// accept
	SOCKET s = (SOCKET)param;
	SOCKADDR_IN caddr;
	int clen = sizeof(caddr);
	c1 = accept(s, (sockaddr*)&caddr, &clen);
	printf("Accepted\n");

	// gui ma 150
	char response[1024];
	memset(response, 0, sizeof(response));
	sprintf_s(response, sizeof(response), "150 OK\r\n");
	send(g_sockets[acceptIndex], response, strlen(response), 0);

	return 0;
}

void main()
{
	WSADATA DATA;
	WSAStartup(MAKEWORD(2, 2), &DATA);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	saddr.sin_port = htons(8888);
	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);
	g_sockets[g_count] = s;
	g_events[g_count] = WSACreateEvent();
	WSAEventSelect(g_sockets[g_count], g_events[g_count], FD_ACCEPT);
	g_count += 1;
	while (0 == 0)
	{
		int index = WSAWaitForMultipleEvents(g_count, g_events, FALSE, INFINITE, FALSE);
		index = index - WSA_WAIT_EVENT_0; //Smallest index of the opened event
		for (int i = index; i < g_count; i++)
		{
			WSANETWORKEVENTS networkEvent;
			WSAEnumNetworkEvents(g_sockets[i], g_events[i], &networkEvent);
			WSAResetEvent(g_events[i]);
			if (networkEvent.lNetworkEvents & FD_ACCEPT) //Accept - g_sockets[i]
			{
				if (networkEvent.iErrorCode[FD_ACCEPT_BIT] == 0) //No error
				{
					SOCKADDR_IN caddr;
					int clen = sizeof(caddr);
					SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
					HANDLE e = WSACreateEvent();
					g_sockets[g_count] = c;
					g_events[g_count] = e;
					WSAEventSelect(g_sockets[g_count], g_events[g_count], FD_READ | FD_CLOSE);
					//Gui phan hoi service ready cho Client
					char* response = (char*)"This is a sample FTP server\r\n220 OK\r\n";
					send(g_sockets[g_count], response, strlen(response), 0);
					g_count += 1;
				}
			}
			if (networkEvent.lNetworkEvents & FD_READ)
			{
				if (networkEvent.iErrorCode[FD_READ_BIT] == 0)
				{
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					recv(g_sockets[i], buffer, sizeof(buffer), 0);
					for (int j = 0; j < strlen(buffer); j++)
					{
						buffer[j] = toupper(buffer[j]);
					}
					printf("%s\n", buffer);
					if (strncmp(buffer, "USER", 4) == 0)
					{
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "331 Please input password\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "PASS", 4) == 0)
					{
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "230 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
						memset(g_wd[i], 0, sizeof(g_wd[i]));
						strcpy_s(g_wd[i], sizeof(g_wd[i]), "/");
					}
					else if (strncmp(buffer, "SYST", 4) == 0)
					{
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "215 UNIX Emulated by VINHLT\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}

					// code start here

					else if (strncmp(buffer, "OPTS", 4) == 0)
					{
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "202 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "NOOP", 4) == 0)
					{
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "200 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "PASV", 4) == 0)
					{
						
						// tao socket cho o cong 9999
						SOCKET s1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						SOCKADDR_IN taddr;
						taddr.sin_family = AF_INET;
						taddr.sin_port = htons(9999);
						taddr.sin_addr.S_un.S_addr = INADDR_ANY;
						bind(s1, (sockaddr*)&taddr, sizeof(taddr));
						listen(s1, 10);
						
						// tao thread nhan du lieu
						acceptIndex = i;
						CreateThread(NULL, 0, AcceptThread, (LPVOID)s1, 0, NULL);

						// phan hoi dong nhan du lieu
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "227 Entering Passive Mode (127,0,0,1,39,15)\r\n"); // nhan du lieu o cong 9999
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "LIST", 4) == 0)
					{
						// lay danh sach
						char* ftpdata = ScanFolder(g_wd[i]);

						// gui du lieu tren socket accept 
						send(c1, ftpdata, strlen(ftpdata), 0);
						free(ftpdata);
						ftpdata = NULL;

						// gui xong dong socket
						closesocket(c1);
						c1 = NULL;

						// phan hoi 226 cho client
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "226 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}


					// code end here

					else if (strncmp(buffer, "FEAT", 4) == 0)
					{
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "Features: \n MDTM\n REST STREAM\n SIZE\n MLST type*;size*;modify*;\n MLSD\n UTF8\n CLNT\n MFMT\n EPSV\n EPRT\n211 End\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "CLNT", 4) == 0)
					{
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "200 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "PWD", 3) == 0)
					{
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "257 \"%s\" is current working directory\r\n", g_wd[i]);
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "TYPE", 4) == 0)
					{
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "200 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "PORT", 4) == 0)
					{
						for (int i = 0; i < strlen(buffer); i++)
						{
							if (buffer[i] == ',')
								buffer[i] = ' ';
						}
						char port[8];
						sscanf(buffer, "%s%d%d%d%d%d%d", port, &g_ip1[i], &g_ip2[i], &g_ip3[i], &g_ip4[i], &g_p1[i], &g_p2[i]);

						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "200 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "MLSD", 4) == 0)
					{
						SOCKET ds = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						SOCKADDR_IN addr;
						addr.sin_addr.S_un.S_un_b.s_b1 = g_ip1[i];
						addr.sin_addr.S_un.S_un_b.s_b2 = g_ip2[i];
						addr.sin_addr.S_un.S_un_b.s_b3 = g_ip3[i];
						addr.sin_addr.S_un.S_un_b.s_b4 = g_ip4[i];
						addr.sin_port = htons(g_p1[i] * 256 + g_p2[i]);
						addr.sin_family = AF_INET;
						if (connect(ds, (sockaddr*)&addr, sizeof(addr)) == 0)
						{
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "150 OK\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);

							char* ftpdata = ScanFolder(g_wd[i]);
							send(ds, ftpdata, strlen(ftpdata), 0);
							free(ftpdata);
							ftpdata = NULL;
							closesocket(ds);

							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "226 Successfully transferred\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
						}
						else
						{
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "425 Can't open data connection\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
						}
					}
					else if (strncmp(buffer, "CDUP", 4) == 0)
					{
						for (int j = strlen(g_wd[i]) - 1; j >= 0; j--)
						{
							if (g_wd[i][j] == '/')
							{
								g_wd[i][j] = 0;
								break;
							}
						}
						if (strlen(g_wd[i]) == 0) 
							strcpy(g_wd[i], "/");

						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "250 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "CWD", 3) == 0)
					{
						char cwd[4];
						char wd[1024];
						while (buffer[strlen(buffer) - 1] == '\r' ||
							buffer[strlen(buffer) - 1] == '\n')
							buffer[strlen(buffer) - 1] = 0;
						memset(wd, 0, sizeof(wd));
						strcpy(wd, buffer + 4);
						
						if (strcmp(g_wd[i],"/") == 0)
							sprintf(g_wd[i] + strlen(g_wd[i]), "%s", wd);
						else
							sprintf(g_wd[i] + strlen(g_wd[i]), "/%s", wd);

						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "250 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "RETR", 4) == 0)
					{
						char* fname = buffer + 5;
						while (fname[strlen(fname) - 1] == '\n' || fname[strlen(fname) - 1] == '\r')
							fname[strlen(fname) - 1] = 0;
						char fullpath[1024];
						memset(fullpath, 0, sizeof(fullpath));
						sprintf(fullpath, "C:%s/%s", g_wd[i], fname);

						SOCKET ds = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						SOCKADDR_IN addr;
						addr.sin_addr.S_un.S_un_b.s_b1 = g_ip1[i];
						addr.sin_addr.S_un.S_un_b.s_b2 = g_ip2[i];
						addr.sin_addr.S_un.S_un_b.s_b3 = g_ip3[i];
						addr.sin_addr.S_un.S_un_b.s_b4 = g_ip4[i];
						addr.sin_port = htons(g_p1[i] * 256 + g_p2[i]);
						addr.sin_family = AF_INET;
						if (connect(ds, (sockaddr*)&addr, sizeof(addr)) == 0)
						{
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "150 OK\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
							FILE* f = fopen(fullpath, "rb");
							char fdata[1024];
							while (!feof(f))
							{
								int r = fread(fdata, 1, sizeof(fdata), f);
								if (r > 0)
								{
									send(ds, fdata, r, 0);
								}
							}
							fclose(f);
							closesocket(ds);
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "226 OK\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
						}
					}
					else if (strncmp(buffer, "DELE", 4) == 0)
					{
						char* fname = buffer + 5;
						while (fname[strlen(fname) - 1] == '\n' || fname[strlen(fname) - 1] == '\r')
							fname[strlen(fname) - 1] = 0;
						char fullpath[1024];
						memset(fullpath, 0, sizeof(fullpath));
						sprintf(fullpath, "C:%s/%s", g_wd[i], fname);
						remove(fullpath);
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "200 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "SIZE", 4) == 0)
					{
						char* fname = buffer + 5;
						while (fname[strlen(fname) - 1] == '\n' || fname[strlen(fname) - 1] == '\r')
							fname[strlen(fname) - 1] = 0;
						char fullpath[1024];
						memset(fullpath, 0, sizeof(fullpath));
						sprintf(fullpath, "C:%s/%s", g_wd[i], fname);
						FILE* f = fopen(fullpath, "rb");
						fseek(f, 0, SEEK_END);
						long fsize = ftell(f);
						fclose(f);
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "213 %d\r\n", fsize);
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "STOR", 4) == 0)
					{
						char* fname = buffer + 5;
						while (fname[strlen(fname) - 1] == '\n' || fname[strlen(fname) - 1] == '\r')
							fname[strlen(fname) - 1] = 0;
						char fullpath[1024];
						memset(fullpath, 0, sizeof(fullpath));
						sprintf(fullpath, "C:%s/%s", g_wd[i], fname);

						SOCKET ds = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						SOCKADDR_IN addr;
						addr.sin_addr.S_un.S_un_b.s_b1 = g_ip1[i];
						addr.sin_addr.S_un.S_un_b.s_b2 = g_ip2[i];
						addr.sin_addr.S_un.S_un_b.s_b3 = g_ip3[i];
						addr.sin_addr.S_un.S_un_b.s_b4 = g_ip4[i];
						addr.sin_port = htons(g_p1[i] * 256 + g_p2[i]);
						addr.sin_family = AF_INET;
						if (connect(ds, (sockaddr*)&addr, sizeof(addr)) == 0)
						{
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "150 OK\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
							FILE* f = fopen(fullpath, "wb");
							char fdata[1024];
							while (0 == 0)
							{
								int r = recv(ds, fdata, sizeof(fdata), 0);
								if (r > 0)
								{
									fwrite(fdata, 1, r, f);
								}
								else
								{
									break;
								}
							}
							fclose(f);
							closesocket(ds);
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "226 OK\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
						}
					}
					else
					{
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "202 Command not implemented\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
				}
			}
			if (networkEvent.lNetworkEvents & FD_CLOSE)
			{
				printf("A client has disconnected\n");
			}
		}
	}
}