#pragma warning(disable:4996)
#include "Main.h"

string getMyIPaddress();

int main(void)
{
	WORD		wVersionRequested; // 윈도우 소켓 버전
	WSADATA		wsaData; // 윈도우 소켓 초기화 정보 구조체
	SOCKADDR_IN target; // 소켓 주소 구조체
	SOCKET      clientSocket; // 소켓 객체
	int			err; // 에러 코드
	int			bytesSent;
	char        buf[BUF_SIZE]; // 버퍼
	string		CLIENT_IP = getMyIPaddress();

	wVersionRequested = MAKEWORD(2, 0);
	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0)
	{
		cout << "Error : 소켓 초기화 과정에서 에러(" << WSAGetLastError() << ")가 발생하였습니다." << endl;
		WSACleanup();
		return false;
	}

	target.sin_family = AF_INET; // 주소 체계(IPv4 주소 체계에서 사용하므로 항상 AF_INET으로 설정)
	target.sin_port = htons(SERVER_PORT); // 16비트 포트 번호
	target.sin_addr.s_addr = inet_addr(IPAddress); // 서버 IP 설정

	// 서버와 연결할 클라이언트 소켓 초기화
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 소켓이 유효하지 않은 경우
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "Error : 소켓 생성 과정에서 에러(" << WSAGetLastError() << ")가 발생하였습니다." << endl;
		WSACleanup();
		return 0;
	}

	// 지정된 소켓으로 연결을 시도
	if (connect(clientSocket, reinterpret_cast<SOCKADDR *>(&target), sizeof(target)) == SOCKET_ERROR)
	{
		cout << "Error : 서버가 생성되어 있지 않습니다(" << WSAGetLastError() << "). 서버를 생성해주세요." << endl;
		WSACleanup();
		return 0; //Couldn't connect
	}

	cout << "------------------------------------------------------" << endl;
	cout << "클라이언트 주소 : " << CLIENT_IP << endl;
	cout << "------------------------------------------------------" << endl;
	cout << "System : 서버와 연결 시도 중입니다." << endl;

	// 서버로 테스트 메시지와 클라이언트 IP 전송
	bytesSent = send(clientSocket, "CONNECTION", strlen("CONNECTION"), 0);

	int n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
	if (!strcmp(buf, "SUCCESS"))
	{
		bytesSent = send(clientSocket, CLIENT_IP.c_str(), strlen(CLIENT_IP.c_str()), 0);
		cout << "System : 서버 " << IPAddress << "로부터 성공적으로 응답을 받았습니다." << endl;
	}
	else
	{
		cout << "Error : 서버와 연결에 실패했습니다." << endl;
		WSACleanup();
		return 1;
	}
	
	string str_select;
	while (true)
	{
		try
		{
			n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0; cout << buf;
			if(n <= 0)
			{ 
				cout << "------------------------------------------------------" << endl;
				cout << "System : 연결이 종료되었습니다." << endl;
				break; 
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0; cout << buf;
			cin >> str_select;
			bytesSent = send(clientSocket, str_select.c_str(), strlen(str_select.c_str()), 0);
		}
		catch (...)
		{
			cout << "Error" << endl;
			break;
		}
	}

	closesocket(clientSocket);
	WSACleanup();

	return 0;
}

string getMyIPaddress()
{
	WORD w;
	WSADATA wsa;
	char name[255];
	string ip;
	PHOSTENT host;
	w = MAKEWORD(2, 0);

	if (WSAStartup(w, &wsa) == 0)
	{
		if (gethostname(name, sizeof(name)) == 0)
		{
			if ((host = gethostbyname(name)) != NULL)
			{
				ip = inet_ntoa(*(struct in_addr *)*host->h_addr_list);
			}
		}
	}
	return ip;
}