#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <map>
int main()
{

	WSADATA wsData;
	//Ham khoi tao su kien => Neu khong co doan khoi tao nay thi chuong trinh khong chay duoc
	int error = WSAStartup(MAKEWORD(2, 2), &wsData);
	//Hứng lỗi xảy ra khi khởi tạo chương trình
	if (error == SOCKET_ERROR)
	{
		//Xu ly neu khong khoi tao duoc
		//Thuong nen co xu ly trong vong while de chuong trinh co the khoi tao duoc
		printf("Khong khoi tao duoc");
	}
	else
	{
		//Neu khoi tao thu vien thanh cong thi bat dau di khoi tao socket
		printf("OK");
		//AF_INET la ho giao thuc (ung voi ipv4) con AF_INET6 ung voi IPV6
		//type SOCK_STREAM ung voi giao thuc TCP-IP (SOCk-DGRAM ung voi UDP)
		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s == INVALID_SOCKET)
		{
			//Neu khong khoi tao duoc Socket thi xu ly su kien de co the tai khoi tao socket
			printf("Ko khoi tao duoc");
		}
		else
		{
			printf("GIa tri Socket vua tao: %d\n", s);
			//struct sockaddr_in saddr1;
			//hoac SOCKADDR_IN do có typedef
			//sockaddr_in: socket address input

			SOCKADDR_IN saddr;
			saddr.sin_family = AF_INET;
			// saddr.sin_port = 80 => Sai do thu tu byte cua may tinh va server nguoc nhau bigendian/little endian
			// => saddr.sin_port = 20480; //Co the su dung ham ho tro
			//chuyen thu tu byte tu littleendian ve bigendian
			//Nguoc lai la ntohs chuyen tu bigendian ve littleendian => dung khi muon ket noi tu server den mot may client
			saddr.sin_port = htons(80);
			//inet_addr => chuyen dia chi IP tu string ve ulong 32bit
			//muon chuyen nguoc lai dung ham inet_ntoa
			saddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

			PADDRINFOA result = NULL;
			getaddrinfo("vnexpress.net", "http", NULL, &result);
			PADDRINFOA root = result; // luu lai goc cua danh sach lien ket de con giai phong sau khi su dung
			while (result != NULL)
			{
				printf("Dia chi tim duoc: \n");
				SOCKADDR_IN addr;
				if (result->ai_family == AF_INET)
				{
					memcpy(&addr, result->ai_addr, result->ai_addrlen);
					printf("%s", inet_ntoa(addr.sin_addr));
					saddr.sin_addr.S_un.S_addr = addr.sin_addr.S_un.S_addr; // Gan 2 so long
					break;
				}
				result = result->ai_next;
			}
			freeaddrinfo(root); // Giai phong bo nho sau khi su dung
		}
		WSACleanup();
	}
	return 0;
}