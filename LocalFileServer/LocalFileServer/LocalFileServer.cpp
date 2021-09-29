#include <iostream>
#include <WinSock2.h>
using namespace std;
void ConCat(char **phtml, char *str)
{

	int oldlen = ((*phtml) == NULL ? 0 : strlen((*phtml)));
	int tmplen = strlen(str);
	*phtml = (char *)realloc((*phtml), oldlen + tmplen + 1);
	memset((*phtml) + oldlen, 0, tmplen + 1);
	sprintf((*phtml) + oldlen, "%s", str);
}
//tra ve mot chuoi html cho trinh duyet
char *ScanFolder(const char *folder)
{
	char *html = NULL;
	ConCat(&html, (char *)"<html>");
	//FindFirstFile & FindNextFile
	WIN32_FIND_DATAA FindData;
	char findpath[1024];
	memset(findpath, 0, sizeof(findpath));
	if (folder[strlen(folder) - 1] == '/')
		sprintf(findpath, "C:%s*.*", folder);
	else
		sprintf(findpath, "C:%s/*.*", folder);
	HANDLE hFind = FindFirstFileA(findpath, &FindData); //ket qua la xau ki tu 1 byte
	if (hFind != INVALID_HANDLE_VALUE)
	{
		ConCat(&html, (char *)"<a href=\"");
		ConCat(&html, (char *)folder);
		if (folder[strlen(folder) - 1] != '/')
		{
			ConCat(&html, (char *)"/");
		}
		ConCat(&html, FindData.cFileName);
		ConCat(&html, (char *)"\">");
		ConCat(&html, FindData.cFileName);
		ConCat(&html, (char *)"</a><br>");
		while (FindNextFileA(hFind, &FindData))
		{
			ConCat(&html, (char *)"<a href=\"");
			ConCat(&html, (char *)folder);
			if (folder[strlen(folder) - 1] != '/')
			{
				ConCat(&html, (char *)"/");
			}
			ConCat(&html, FindData.cFileName);
			ConCat(&html, (char *)"\">");
			ConCat(&html, FindData.cFileName);
			ConCat(&html, (char *)"</a><br>");
		}
	}
	ConCat(&html, (char *)"</html>");

	return html;
}
int IsFolder(char *path)
{
	WIN32_FIND_DATAA FindData;
	char findpath[1024];
	memset(findpath, 0, sizeof(findpath));
	sprintf(findpath, "C:%s", path);
	if (findpath[strlen(findpath) - 1] == '/')
	{
		findpath[strlen(findpath) - 1] = 0; // Bo dau gach o cuoi duong dan
	}

	HANDLE hFind = FindFirstFileA(findpath, &FindData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// Neu and voi co FILE_DIRECTORY ma bang 1 thi la thu muc
			return 1;
		}
		else
			return 0;
		//CloseHandle(hFind);
	}
	else
		return -1;
}
void ProcessSpace(char *str)
{
	char *space = NULL;
	do
	{
		space = strstr(str, "%20");
		if (space != NULL)
		{
			space[0] = 32;
			strcpy(space + 1, space + 3);
		}
	} while (space != NULL);
}
DWORD WINAPI ClientThread(LPVOID param)
{
	SOCKET c = (SOCKET)param;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	recv(c, buffer, sizeof(buffer), 0);
	char action[1024];
	char path[1024];
	memset(action, 0, sizeof(action));
	memset(path, 0, sizeof(path));
	sscanf(buffer, "%s%s", action, path);
	ProcessSpace(path);
	if (strcmp(action, "GET") == 0)
	{
		if (strcmp(path, "/") == 0)
		{
			char *html = ScanFolder("/");
			char *response = (char *)calloc(strlen(html) + 1024, 1);
			sprintf(response, "HTTP/1.1 200 OK\r\nServer: MYLOCAL\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n%s", strlen(html), html);
			//char* response = (char*)"HTTP/1.1 200 OK\r\nServer: MYLOCAL\r\nContent-Type: text/html\r\nContent-Length:25\r\n\r\n<html><b>HELLO</b></html>";
			send(c, response, strlen(response), 0);
			closesocket(c);
			free(response);
			response = NULL;
			free(html);
			html = NULL;
		}
		else
		{
			if (IsFolder(path) == 1)
			{
				char *html = ScanFolder(path);
				char *response = (char *)calloc(strlen(html) + 1024, 1);
				sprintf(response, "HTTP/1.1 200 OK\r\nServer: MYLOCAL\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n%s", strlen(html), html);
				//char* response = (char*)"HTTP/1.1 200 OK\r\nServer: MYLOCAL\r\nContent-Type: text/html\r\nContent-Length:25\r\n\r\n<html><b>HELLO</b></html>";
				send(c, response, strlen(response), 0);
				closesocket(c);
				free(response);
				response = NULL;
				free(html);
				html = NULL;
			}
			else if (IsFolder(path) == 0)
			{
			}
			else
			{
				//return 404 error
			}
		}
		/*char* response = (char*)"HTTP/1.1 200 OK\r\nServer: MYLOCAL\r\nContent-Type: text/html\r\nContent-Length:25\r\n\r\n<html><b>HELLO</b></html>";
		send(c, response, strlen(response), 0);
		closesocket(c);*/
	}
	if (strcmp(action, "POST") == 0)
	{
	}
	return 0;
}
int main()
{
	ScanFolder("C:");
	WSADATA DATA;
	WSAStartup(MAKEWORD(2, 2), &DATA);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_addr.S_un.S_addr = 0;
	saddr.sin_port = htons(8888);
	saddr.sin_family = AF_INET;
	bind(s, (sockaddr *)&saddr, sizeof(saddr));
	listen(s, 10);
	while (1)
	{
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);
		SOCKET c = accept(s, (sockaddr *)&caddr, &clen);
		CreateThread(NULL, 0, ClientThread, (LPVOID)c, 0, NULL);
	}
	return 0;
}