#include<stdio.h>
#include<WinSock2.h>
#define N 2

SOCKET* clients = NULL;
int count = 0;

DWORD WINAPI ClientThread(LPVOID param) {
	//Luong tham do va xu ly hoat dong tu client thu startIndex
	int startIndex = (int)param;
	//In ra luong vua duoc tao
	printf("Thread %d, start index %d\n", GetCurrentThreadId(), startIndex);
	//Tao ra tap tham do cho luong tuong ung
	fd_set fdread;
	while (1) {
		//Don tap tham do
		FD_ZERO(&fdread);
		//Tien hanh them client moi vao tap tham do cua luong tuong ung
		for (int i = startIndex; i < startIndex + N && i < count; i++) {
			FD_SET(clients[i], &fdread);
		}
		//tao cau truc time limit de gioi han thoi gian treo trong ham select
		timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		//ham select se tham do su kien tren tap fdread tai moi mot socket thi dung lai tham do 1s
		select(0, &fdread, NULL, NULL, &tv);
		//Nghe su kien tren tap tham do cua Thread va tien hanh chuyen tiep goi tin sang cac client khác
		for (int i = startIndex; i < startIndex + N && i < count; i++) {
			if (FD_ISSET(clients[i], &fdread)) {
				char buffer[1024];
				memset(buffer, 0, sizeof(buffer));
				recv(clients[i], buffer, sizeof(buffer), 0);
				//*Chu y: j<count vi se can chuyen tiep ra cac client khac nam ngoai thread dang xet 
				for (int j = 0; j < count; j++) {
					if (j != i) {
						send(clients[j], buffer, strlen(buffer), 0);
					}
				}
			}
		}
	}

	return 0;
}

int main(int argc, char* argv[]) {
	//Khoi tao thu vien, socket va cau truc dia chi cho socket le tan
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKADDR_IN saddr;
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	saddr.sin_port = htons(8888);
	saddr.sin_family = AF_INET;
	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);

	//Tao fd_set de tham do N client dau tien
	fd_set fdread; // toi da 64
	while (1) {
		FD_ZERO(&fdread); // xoa het cac phan tu di (dat so phan tu bang 0)
		//Them socket le tan vao tap tham do fdread
		FD_SET(s, &fdread);

		//Neu co < N client ket noi den thi them luon vao tap tham do cua luong main
		for (int i = 0; i < count; i++) {
			//neu nhu co nho hon N client ket noi den thi them vao tap fdset de tham do tren luong main
			if (i < N) {
				FD_SET(clients[i], &fdread);
			}

		}
		//Tien hanh tham do tap fdread bao gom N client dau tien ket noi den
		select(0, &fdread, NULL, NULL, NULL); // diem treo

		//Neu co client ket noi den server thi su kien se duoc phat hien boi ham select
		if (FD_ISSET(s, &fdread)) {
			//Chap nhan ket noi tu phia client
			SOCKADDR_IN caddr;
			int clen = sizeof(caddr);
			SOCKET tmp = accept(s, (sockaddr*)&caddr, &clen);
			//======================================================================================================================
			/// <summary>
			/// Neu can chuc nang dang nhap thi them module dang nhap vao day
			/// </summary>
			/// <param name="argc"></param>
			/// <param name="argv"></param>
			/// <returns></returns>
			//============================================================================================================================

			//Tao ra socket client de duy tri ket noi va them vao mang clients[]
			clients = (SOCKET*)realloc(clients, (count + 1) * sizeof(SOCKET));
			clients[count] = tmp;
			count++;
			//neu nhu co >= N client ket noi den thi bat dau xu ly tao thread moi voi chi so bat dau tu count
			if (count >= N && (count % N) == 0) {
				CreateThread(
					NULL,
					0,
					ClientThread,
					(LPVOID)count,
					0,
					NULL
				);
			}
		}
		//Xu ly su kien lang nghe va chuyen tiep goi tin (vao de thi nen toi uu dieu kien cua vong for) 
		for (int i = 0; i < count; i++) {
			//Chi xu ly cho N client dau tien, con lai thi se chuyen sang luong con de xu ly
			if (i < N) {
				if (FD_ISSET(clients[i], &fdread)) {
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					recv(clients[i], buffer, sizeof(buffer), 0);
					for (int j = 0; j < count; j++) {
						if (j != i) {
							send(clients[j], buffer, strlen(buffer), 0);
						}
					}
				}
			}
		}
	}
}

