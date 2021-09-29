#include<stdio.h>
#include<Windows.h>
#include<conio.h>
#include<time.h>

/*
	Nếu không tạo thêm thread thì sẽ có vấn đề đặt ra khi tạo server nhiều người dùng
	Đó là chương trình sẽ luôn có 2 điểm treo là listen và recv
	Khi đang lắng nghe kết nối thì sẽ không thể nhận dữ liệu và khi đang nhận dữ liệu thì không thể kết nối
	Tuy nhiên vấn đề có thể giải quyết bằng cách dùng UDP thay vì TCP, bởi vì UDP chỉ có 1 điểm treo là recvfrom mà không cần listen
*/

#define MAX_CLIENT 1024
SOCKET c[MAX_CLIENT];
int clientCount = 0;

// Luồng người dùng
DWORD WINAPI MyThread(LPVOID param) {
	// Ép lại về kiếu SOCKET
	SOCKET tmp = (SOCKET)param;
	printf("%d\n", GetCurrentThreadId());
	
	// Tạo chuỗi nhận dữ liệu từ người dùng
	char buffer[1024];
	// Nhận dữ liệu liên tục từ phía người dùng đó qua socket c
	while (1) {
		// Nhận dữ liệu từ phía người dùng
		memset(buffer, 0, sizeof(buffer));
		recv(tmp, buffer, sizeof(buffer), 0);

		// Gửi dữ liệu tới những người dùng khác
		for (int i = 0; i < clientCount; i++) {
			if (c[i] != tmp) {
				send(c[i], buffer, strlen(buffer), 0);
			}
		}
	}
	return 0;
}

int main()
{
	// start WinSock
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);

	// Tạo một giao diện mạng phía server
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8888);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;

	// Tạo socket s làm lễ tân và kết nối vào giao diện trên
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(s, (sockaddr*)&saddr, sizeof(saddr));

	// socket s lắng nghe ở giao diện đó và có hàng đợi là 30
	listen(s, 30);

	// lặp lại quá trình kết nối tới người dùng
	while (1) {
		
		// Tạo 1 giao diện để kết nối từ phía người dùng
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);

		// Nhận kết nối khi có người dùng truy cập và địa chỉ mà lễ tân đang đứng đợi, sau đó chuyển kết nối vào socket c để truyền nhận dữ liệu
		c[clientCount] = accept(s, (sockaddr*)&caddr, &clen);

		// Tạo mới 1 thread với đoạn chương trình MyThread và tham số truyền vào là socket đã được kết nối tới người dùng dưới dạng con trổ LPVOID
		CreateThread(NULL, 0, MyThread, (LPVOID)c[clientCount], 0, NULL);
		clientCount++;
	}
}