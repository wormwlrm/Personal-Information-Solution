#pragma warning(disable:4996)
#include "Main.h"

string getMyIPaddress();

int main(void)
{
	WORD		wVersionRequested; // ������ ���� ����
	WSADATA		wsaData; // ������ ���� �ʱ�ȭ ���� ����ü
	SOCKADDR_IN target; // ���� �ּ� ����ü
	SOCKET      clientSocket; // ���� ��ü
	int			err; // ���� �ڵ�
	int			bytesSent;
	char        buf[BUF_SIZE]; // ����
	string		CLIENT_IP = getMyIPaddress();

	wVersionRequested = MAKEWORD(2, 0);
	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0)
	{
		cout << "Error : ���� �ʱ�ȭ �������� ����(" << WSAGetLastError() << ")�� �߻��Ͽ����ϴ�." << endl;
		WSACleanup();
		return false;
	}

	target.sin_family = AF_INET; // �ּ� ü��(IPv4 �ּ� ü�迡�� ����ϹǷ� �׻� AF_INET���� ����)
	target.sin_port = htons(SERVER_PORT); // 16��Ʈ ��Ʈ ��ȣ
	target.sin_addr.s_addr = inet_addr(IPAddress); // ���� IP ����

	// ������ ������ Ŭ���̾�Ʈ ���� �ʱ�ȭ
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// ������ ��ȿ���� ���� ���
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "Error : ���� ���� �������� ����(" << WSAGetLastError() << ")�� �߻��Ͽ����ϴ�." << endl;
		WSACleanup();
		return 0;
	}

	// ������ �������� ������ �õ�
	if (connect(clientSocket, reinterpret_cast<SOCKADDR *>(&target), sizeof(target)) == SOCKET_ERROR)
	{
		cout << "Error : ������ �����Ǿ� ���� �ʽ��ϴ�(" << WSAGetLastError() << "). ������ �������ּ���." << endl;
		WSACleanup();
		return 0; //Couldn't connect
	}

	cout << "------------------------------------------------------" << endl;
	cout << "Ŭ���̾�Ʈ �ּ� : " << CLIENT_IP << endl;
	cout << "------------------------------------------------------" << endl;
	cout << "System : ������ ���� �õ� ���Դϴ�." << endl;

	// ������ �׽�Ʈ �޽����� Ŭ���̾�Ʈ IP ����
	bytesSent = send(clientSocket, "CONNECTION", strlen("CONNECTION"), 0);

	int n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
	if (!strcmp(buf, "SUCCESS"))
	{
		bytesSent = send(clientSocket, CLIENT_IP.c_str(), strlen(CLIENT_IP.c_str()), 0);
		cout << "System : ���� " << IPAddress << "�κ��� ���������� ������ �޾ҽ��ϴ�." << endl;
	}
	else
	{
		cout << "Error : ������ ���ῡ �����߽��ϴ�." << endl;
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
				cout << "System : ������ ����Ǿ����ϴ�." << endl;
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