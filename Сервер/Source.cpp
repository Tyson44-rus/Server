#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <time.h>
#include <iomanip>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[])
{
	setlocale(0, "");
	WSADATA wsadata;
	WSAStartup(2, &wsadata);
	char* buffer;
	PADDRINFOA addr;
	if (!getaddrinfo("127.0.0.1", "12000", NULL, &addr))
	{
		SOCKET Xiaomi = socket(AF_INET, SOCK_STREAM, 0);
		if (!bind(Xiaomi, addr->ai_addr, addr->ai_addrlen))
		{
			listen(Xiaomi, 1);
			if (argc == 2)
			{
				HANDLE a = CreateFileA(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
				if (a != INVALID_HANDLE_VALUE)
				{
					LARGE_INTEGER size;
					if (GetFileSizeEx(a, &size) && (size.QuadPart < 1024 * 1024) && (size.QuadPart > 0))
					{
						HANDLE b = CreateFileMappingA(a, NULL, PAGE_READONLY, size.HighPart, size.LowPart, NULL);
						size_t total = size.QuadPart;
						buffer = (char*)MapViewOfFile(b, FILE_MAP_READ, 0, 0, total);
						size_t sent = 0;
						cout << "Ожидание клиента\n";
						SOCKET client = accept(Xiaomi, NULL, NULL);
						while (sent < total)
						{
							int temp_check = send(client, buffer + sent, total - sent, 0);
							if (temp_check > 0)
								sent += temp_check;
							else if (temp_check == 0)
							{
								printf("%s", "Соединение прервано\n");
								break;
							}
							else if (temp_check < 0)
							{
								printf("%s", "Ошибка соединения\n");
								break;
							}
							printf("%s%d%s%d%s", "Передано ", sent, " байтов из ", total, "\n");
						}
						shutdown(client, SD_BOTH);
						closesocket(client);
						CloseHandle(b);
					}
				}
				CloseHandle(a);
			}
			cout << "bind успешно\n";
		}
		else
			cout << "Error...bind: " << WSAGetLastError() << endl;
		closesocket(Xiaomi);
		freeaddrinfo(addr);
		WSACleanup();
		cout << "Getaddrinfo успешно\n";
	}
}