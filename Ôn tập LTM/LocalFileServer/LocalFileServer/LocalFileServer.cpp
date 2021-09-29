#include<stdio.h>
#include<WinSock2.h>

void ProcessSpace(char* str) {
	char* space = NULL;
	do {
		space = strstr(str, "%20");
		if (space != NULL) {
			space[0] = 32; // dau cach
			strcpy(space + 1, space + 3);
		}
	} while (space != NULL);
}

void Concat(char** phtml, char* str) {
	int oldlen = ((*phtml) == NULL) ? 0 : strlen((*phtml));
	int tmplen = strlen(str);
	*phtml = (char*)realloc((*phtml), oldlen + tmplen + 1);
	memset((*phtml) + oldlen, 0, tmplen + 1);
	sprintf((*phtml) + oldlen, "%s", str);
}

// can them hyperlink de xem thao tac tren giao dien web
char* ScanFolder(char* folder) { // "/"

	// cap phat dong html boi vi co nhieu folder co rat nhieu file. Nen khong the cap phat tinh duoc
	char* html = NULL;
	Concat(&html, (char*)"<html>");
	char findpath[1024];  // "C:\*.*"
	memset(findpath, 0, sizeof(findpath));

	if (folder[strlen(folder) - 1] == '/')
		sprintf(findpath, "C:%s*.*", folder); // noi sau theo dinh dang minh muon
	else
		sprintf(findpath, "C:%s/*.*", folder);
	// thieu mot thuoc tinh
	Concat(&html, (char*)"<form method=\"post\" action=\"/action_page.php\"><input type=\"file\" id=\"myFile\" name=\"filename\"><input type=\"submit\"></form>");

	WIN32_FIND_DATAA FindData; // Phien ban ASCII
	HANDLE hFind = FindFirstFileA(findpath, &FindData); // tim file va luu ra bien ra la FindData. Chu A o cuoi la phien ban ASCII
	// HANDLE thuc chat la con tro toi mot vung nho trong windows.
	if (hFind != INVALID_HANDLE_VALUE) { // giong kieu con tro khac null
		// Dinh dang hyperlink <a href="path">FileName</a>

		Concat(&html, (char*)"<a href=\"");
		Concat(&html, folder);
		if (folder[strlen(folder) - 1] != '/') Concat(&html, (char*)"/");
		Concat(&html, FindData.cFileName);
		Concat(&html, (char*)"\">");
		Concat(&html, FindData.cFileName);
		Concat(&html, (char*)"</a><br>");

		while (FindNextFileA(hFind, &FindData)) { // phien ban ASCII, truyen handle cua FindFirst, tra du lieu vao FindData. Ham nay tra ve false neu khong con gi de tim nua
			Concat(&html, (char*)"<a href=\"");
			Concat(&html, folder);
			if (folder[strlen(folder) - 1] != '/') Concat(&html, (char*)"/");
			Concat(&html, FindData.cFileName);
			Concat(&html, (char*)"\">");
			Concat(&html, FindData.cFileName);
			Concat(&html, (char*)"</a><br>");
		}

		//CloseHandle(hFind); // dong Handle
	}

	Concat(&html, (char*)"</html>");

	return html;
}

int IsFolder(char* path)
{
	char findpath[1024];  // "C:\*.*"
	memset(findpath, 0, sizeof(findpath));
	sprintf(findpath, "C:%s", path); // noi sau theo dinh dang minh muon
	if (findpath[strlen(findpath) - 1] == '/')
		findpath[strlen(findpath) - 1] = 0; // bo dau / cuoi cung
	WIN32_FIND_DATAA FindData; // Phien ban ASCII
	HANDLE hFind = FindFirstFileA(findpath, &FindData);
	if (hFind != INVALID_HANDLE_VALUE) {
		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			// day la co thuoc tinh cua tep. Neu la folder thi folder flag se bat
			return 1;
		}
		else return 0;
		//CloseHandle(hFind);
	}
	else return -1;

}
int FindBoundary(char* content, int len, char* boundary)
{
	int start = strlen(boundary + 1);
	for (int i = start; i < len; i++) {
		if (content[i] == boundary[0]) {
			int found = 1;
			for (int j = 0; j < strlen(boundary); j++) {
				if (content[i + j] != boundary[j]) {
					found = 0;
					break;
				}
			}

			if (found == 1) {
				return i;
			}
		}
	}
}

// tra loi nguoi dung
DWORD WINAPI ClientThread(LPVOID param) {
	SOCKET c = (SOCKET)param;

	// http://127.0.0.1:8888/... -> trinh duyet se vao do va la du lieu va sau khi nhan xong thi se chuyen sang trang thai recv
	// server se nhan du lieu tu trinh duyet va xu ly de gui tra ve de lieu cho trinh duyet

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	recv(c, buffer, sizeof(buffer), 0);

	char action[1024];
	char path[1024];
	memset(action, 0, sizeof(action));
	memset(path, 0, sizeof(path));

	sscanf(buffer, "%s%s", action, path);

	ProcessSpace(path); // thay %20 thanh dau cach

	if (strcmp(action, "GET") == 0) {

		//xem xet 
		if (strcmp(path, "/") == 0) {
			char* html = ScanFolder((char*)"/");
			char* response = (char*)calloc(strlen(html) + 1024, 1);
			sprintf(response, "HTTP/1.1 200 OK\r\nServer: MyLocal\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", strlen(html), html);
			//Gui chuoi html cua thu muc goc
			send(c, response, strlen(response), 0);
			closesocket(c);
			free(response); // cap phat dong
			response = NULL;
			free(html);
			html = NULL;
		} // can phai kiem tra xem duong dan toi file hay la thu muc
		else {
			if (IsFolder(path) == 1) {
				char* html = ScanFolder(path);
				char* response = (char*)calloc(strlen(html) + 1024, 1);
				sprintf(response, "HTTP/1.1 200 OK\r\nServer: MyLocal\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", strlen(html), html);
				//Gui chuoi html cua thu muc con
				send(c, response, strlen(response), 0);
				closesocket(c);
				free(response); // cap phat dong
				response = NULL;
				free(html);
				html = NULL;
			}
			else if (IsFolder(path) == 0) {
				char type[1024];
				memset(type, 0, sizeof(type));
				if (strlen(path) > 4) {
					// dat dinh dang cho du lieu tra ve de hien thi tren trinh duyet

					if (strcmp(path + strlen(path) - 4, ".mp4") == 0) {
						strcpy(type, "video/mp4");
					}
					else if (strcmp(path + strlen(path) - 4, ".mp3") == 0) {
						strcpy(type, "audio/mp3");
					}
					else if (strcmp(path + strlen(path) - 4, ".jpg") == 0) {
						strcpy(type, "image/jpg");
					}
					else if (strcmp(path + strlen(path) - 4, ".txt") == 0) {
						strcpy(type, "text/html");
					}
					else
						strcpy(type, "application/octet-stream");
				}
				else {
					strcpy(type, "application/octet-stream");
				}
				char fullpath[1024];  // "C:\*.*"
				memset(fullpath, 0, sizeof(fullpath));
				sprintf(fullpath, "C:%s", path); // noi sau theo dinh dang minh muon
				FILE* f = fopen(fullpath, "rb"); // doc file binary

				fseek(f, 0, SEEK_END);
				int flen = ftell(f);
				fseek(f, 0, SEEK_SET);

				char* data = (char*)calloc(flen, 1);
				if (data != NULL) {
					fread(data, 1, flen, f);
					char response[1024];
					memset(response, 0, 1024);
					sprintf(response, "HTTP/1.1 200 OK\r\nServer: MyLocal\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", type, flen);
					send(c, response, strlen(response), 0);
					int sent = 0;
					printf("Start sending file\n");
					while (sent < flen)
					{
						sent += send(c, data + sent, flen - sent, 0);

					}

					printf("Send : %d\n", sent);
					printf("Sending completed\n");
					free(data);
					closesocket(c);
				}


				fclose(f);
			}
			else {
				// 404 not found
				char response[1024];
				memset(response, 0, 1024);
				sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n<html>404 Not Found</html>");
				send(c, response, strlen(response), 0);
			}
		}
	}

	else if (strcmp(action, "POST") == 0) {
		//upload file
		char html[1024];
		if (strstr(buffer, "Content-Length")) {


			char tmp[1024];
			memset(tmp, 0, 1024);
			int len = 0;
			sscanf(strstr(buffer, "Content-Length:"), "%s%d", tmp, &len);
			char* content = (char*)calloc(len, 1);
			int r = 0;
			printf("prepare to received: %d bytes\r\n", len);
			while (r < len) {
				r += recv(c, content + r, len - r, 0);
				printf("da nhan so bytes: %d \r\n", r);
			}
			char* fnamestart = strstr(content, "filename=\"");
			char* fnamestop = strstr(fnamestart, "\"") - 1;
			char fname[1024];
			memset(fname, 0, sizeof(fname));
			strcpy(fname, "D:\\upload\\");
			memcpy(fname, fnamestart + strlen(fname), fnamestop - fnamestart + 1);

			//r = recv(c, tmp, sizeof(tmp), 0);
			////printf("Recieved: %d\n", r);
			sscanf(content, "%s", tmp);
			int start = strstr(content, "\r\n\r\n") + 4 - content;
			int end = FindBoundary(content, len, tmp) - 3;
			FILE* f = fopen(fname, "wb");
			fwrite(content + start, 1, end - start + 1, f);
			fclose(f);
			memset(html, 0, sizeof(html));
			strcpy(html, "<html>File uploaded</html>");
			char* response = (char*)calloc(strlen(html) + 1024, 1);
			sprintf(response,
				"HTTP/1.1 200 OK\r\nServer: MyLocal\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", strlen(html), html);
			send(c, response, strlen(response), 0);
			closesocket(c);

		}
	}

	else {

	}

	return 0;
}



int main()
{
	WSADATA DATA;
	WSAStartup(MAKEWORD(2, 2), &DATA);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_addr.S_un.S_addr = 0;
	saddr.sin_port = htons(8888);
	saddr.sin_family = AF_INET;

	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);
	while (1) {
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);
		SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
		CreateThread(NULL, 0, ClientThread, (LPVOID)c, 0, NULL);
	}
}