#pragma warning(disable:4996)
#include "Main.h"
#include "../Project1/Application.h"

int main(void)
{
	WORD		wVersionRequested; // ������ ���� ����
	WSADATA		wsaData; // ������ ���� �ʱ�ȭ ���� ����ü
	SOCKADDR_IN servAddr, cliAddr; //���� �ּ� ����ü
	SOCKET		endPointSocket, clientSocket; // ���� ��ü
	int			err; // ���� �ڵ�
	int			bytesSent;
	char        buf[BUF_SIZE]; // ����
	string		clientIP; // Ŭ���̾�Ʈ IP
	Application	A; // ���� ���� ��� ���ø����̼�
	vector<Log>	Logs; // �α� ��� �� ����
	time_t		rawtime;
	while (true) {
		// ���� ���� �Է�(2.0)
		wVersionRequested = MAKEWORD(2, 0);
		// ������ ���� �ʱ�ȭ
		err = WSAStartup(wVersionRequested, &wsaData);

		// ���� ���� Ȯ�ο��� ���� �ڵ� �߻� ��
		if (err != 0)
		{
			cout << "Error : ���� �ʱ�ȭ �������� ����(" << WSAGetLastError() << ")�� �߻��Ͽ����ϴ�." << endl;
			WSACleanup();
			return 1;
		}

		servAddr.sin_family = AF_INET; // �ּ� ü��(IPv4 �ּ� ü�迡�� ����ϹǷ� �׻� AF_INET���� ����)
		servAddr.sin_port = htons(SERVER_PORT); // 16��Ʈ ��Ʈ ��ȣ
		servAddr.sin_addr.s_addr = inet_addr(IPAddress); // ���� IP ����

		// �������� listen�� ���� ���� ��ü �ʱ�ȭ
		// �Ķ���� : �ּ� ü��, ���� Ÿ��(���� ���⼺ ����) , ��������(TCP)
		endPointSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		// ������ ��ȿ���� ���� ���
		if (endPointSocket == INVALID_SOCKET)
		{
			cout << "Error : ���� ���� �������� ����(" << WSAGetLastError() << ")�� �߻��Ͽ����ϴ�." << endl;
			WSACleanup();
			return 1;
		}

		// ���ε�
		// �Ķ���� : ���� ���� ��ü, ���� ��ü�� �ο��� �ּ� ������ ���� ����ü �̸�, �̸� ����
		int x = ::bind(endPointSocket, reinterpret_cast<SOCKADDR *>(&servAddr), sizeof(servAddr));
		if (x == SOCKET_ERROR)
		{
			cout << "Error : ���ε� �������� ����( " << WSAGetLastError() << ")�� �߻��Ͽ����ϴ�." << endl;
			WSACleanup();
			return 1;
		}

		// ���� ���� ���� �Ϸ�, ����� ����ϰ� Ŭ���̾�Ʈ�� ������ ��ٸ�
		cout << "------------------------------------------------------" << endl;
		cout << "���� �ּ� : " << IPAddress << endl;
		cout << "���� ��Ʈ : " << SERVER_PORT << endl;
		cout << "���� ��ȣ : " << endPointSocket << endl;
		cout << "------------------------------------------------------" << endl;
		cout << "System : Ŭ���̾�Ʈ�� ������ ����ϴ� ���Դϴ�." << endl;

		// ���� ��⿭ ����
		// �Ķ���� : ���� ���� ��ü, ���� ��⿭�� ũ��
		listen(endPointSocket, 5);

		int xx = sizeof(cliAddr);
		// ���� ��û �������� Ŭ���̾�Ʈ ���� �ʱ�ȭ
		// �Ķ���� : ���� ���� ��ü, ���� ��ü�� �ο��� �ּ� ������ ���� ����ü �̸�, �̸� ����
		clientSocket = accept(endPointSocket, reinterpret_cast<SOCKADDR *>(&cliAddr), &xx);

		// ���� ���� �� ���� ���� Ȯ��
		// ����� �������κ��� �����͸� ���� ����
		// �Ķ���� : Ŭ���̾�Ʈ ���� ��ü, ���޹��� �����͸� ������ ����, ���� ����, �÷���
		int n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
		if (!strcmp(buf, "CONNECTION"))
		{
			bytesSent = send(clientSocket, "SUCCESS", strlen("SUCCESS"), 0);
			n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
			clientIP = buf;
			cout << "System : Ŭ���̾�Ʈ " << clientIP << "�κ��� ���� �õ��� �����Ǿ����ϴ�." << endl;
			bool find_kicked = false;
			for (size_t i = Logs.size(); i > 0; i--)
			{
				// ���� �� ������ ���
				if (Logs.at(i - 1).seconds > 0)
				{
					// �ܿ� ��� �ð��� ���� ���
					if(BAN_TIME - (time(0) - Logs.at(i - 1).seconds) > 0)
					{
						find_kicked = true;
						cout << "System : Ŭ���̾�Ʈ " << clientIP << "�� �� �����̹Ƿ� �����Ͽ����ϴ�." << endl;
						LONGLONG tem = BAN_TIME - (time(0) - Logs.at(i - 1).seconds);
						string temp = "System : Ŭ���̾�Ʈ " + clientIP + "�� �� ���·�, " + to_string(tem) + "�� �� ���� �����մϴ�.\n";
						bytesSent = send(clientSocket, temp.c_str(), strlen(temp.c_str()), 0);
						temp = "System : ��� �� �ٽ� ������ �ֽñ� �ٶ��ϴ�. ����� �Է��ϸ� ����˴ϴ�.\n";
						bytesSent = send(clientSocket, temp.c_str(), strlen(temp.c_str()), 0);
						cout << "------------------------------------------------------" << endl;
						closesocket(clientSocket);
						closesocket(endPointSocket);
						WSACleanup(); 
					}
					// �ܿ� ��� �ð��� ���� ��� �� ����� ����
					else
					{
						Logs.at(i - 1).seconds = 0;
					}
				}
				if (find_kicked)
					break;
			}
			if (find_kicked)
				continue;
			cout << "System : Ŭ���̾�Ʈ " << clientIP << "�� ����Ǿ����ϴ�." << endl;
			cout << "------------------------------------------------------" << endl;

			// �α� �߰�
			Log newLog;  time(&rawtime);
			newLog.IP = clientIP; newLog.activity = "Connection Success"; newLog.time = ctime(&rawtime); newLog.seconds = 0;
			cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
			Logs.push_back(newLog);
		}
		else
		{
			cout << "Error : Ŭ���̾�Ʈ�� ���ῡ �����߽��ϴ�." << endl;

			// �α� �߰�
			Log newLog; time(&rawtime);
			newLog.IP = clientIP; newLog.activity = "Connection Failure"; newLog.time = ctime(&rawtime);
			cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
			Logs.push_back(newLog);

			WSACleanup();
			return 1;
		}

		// ���ῡ �������� ��� ���� �ݺ�
		string str_line = "------------------------------------------------------\n";
		string str_menu = "1. �̸��� �˻�\n2. �ֹε�Ϲ�ȣ �˻�\n0. ���α׷� ����\n";
		string str_error = "�߸��� �Է��Դϴ�.\n";
		string str_dir = "�˻��� ���丮�� �Է����ּ���(�⺻ dir) : \n";
		string str_degree = "������ �������ּ���(1,2,3) : \n";
		string str_exit = "������ �����մϴ�.\n";

		bytesSent = send(clientSocket, str_line.c_str(), strlen(str_line.c_str()), 0);

		string str_input;
		string str_input2;
		int int_input;
		bool exit_program = false;
		while (!exit_program)
		{
			try
			{
				if (clientSocket == SOCKET_ERROR)
				{
					cout << "System : ������ ����Ǿ����ϴ�." << endl;
					break;
				}
				// �⺻ �޴� ���
				bytesSent = send(clientSocket, str_line.c_str(), strlen(str_line.c_str()), 0);
				bytesSent = send(clientSocket, str_menu.c_str(), strlen(str_menu.c_str()), 0);
				bytesSent = send(clientSocket, str_line.c_str(), strlen(str_line.c_str()), 0);

				n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
				str_input = buf;
				int_input = stoi(buf);

				Log newLog; time(&rawtime);
				switch (int_input)
				{
				case 1:
					// �̸��� �˻� - ���丮 ����
					bytesSent = send(clientSocket, str_line.c_str(), strlen(str_line.c_str()), 0);
					bytesSent = send(clientSocket, str_dir.c_str(), strlen(str_dir.c_str()), 0);
					n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
					str_input = buf;

					// C����̺� �Ǵ� D����̺긦 �˻��� �� ��
					if (str_input == "C:\\" || str_input == "D:\\" || str_input == "c:\\" || str_input == "d:\\")
					{
						newLog.seconds = time(0);
						newLog.IP = clientIP; newLog.activity = "Banned for searching directory (" + str_input + ")"; newLog.time = ctime(&rawtime);
						cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
						Logs.push_back(newLog);
						exit_program = true;
						break;
					}

					A.E.run(str_input);
					bytesSent = send(clientSocket, A.E.print().c_str(), strlen(A.E.print().c_str()), 0);

					// �α� �߰�				
					newLog.IP = clientIP; newLog.activity = "Search Eamil in directory (" + str_input + ")"; newLog.time = ctime(&rawtime);
					cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
					Logs.push_back(newLog);

					break;
				case 2:
					// �ֹε�Ϲ�ȣ �˻� - ���丮 ����
					bytesSent = send(clientSocket, str_line.c_str(), strlen(str_line.c_str()), 0);
					bytesSent = send(clientSocket, str_dir.c_str(), strlen(str_dir.c_str()), 0);
					n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
					str_input = buf;

					// C����̺� �Ǵ� D����̺긦 �˻��� �� ��
					if (str_input == "C:\\" || str_input == "D:\\" || str_input == "c:\\" || str_input == "d:\\")
					{
						newLog.seconds = time(0);
						newLog.IP = clientIP; newLog.activity = "Banned for search directory (" + str_input + ")"; newLog.time = ctime(&rawtime);
						cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
						Logs.push_back(newLog);
						exit_program = true;
						break;
					}

					// �ֹε�Ϲ�ȣ �˻� - ���丮 ���� - ���� ����
					bytesSent = send(clientSocket, str_line.c_str(), strlen(str_line.c_str()), 0);
					bytesSent = send(clientSocket, str_degree.c_str(), strlen(str_degree.c_str()), 0);
					n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
					str_input2 = buf;
					int_input = stoi(str_input2);
					A.R.run(str_input, int_input);
					bytesSent = send(clientSocket, A.R.print().c_str(), strlen(A.R.print().c_str()), 0);

					// �α� �߰�	
					newLog.IP = clientIP; newLog.activity = "Search Registration Number in directory(" + str_input + ") with degree (" + to_string(int_input) + ")"; newLog.time = ctime(&rawtime);
					cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
					Logs.push_back(newLog);
					break;

				case 0:
					exit_program = true;

					// �α� �߰�				
					newLog.IP = clientIP; newLog.activity = "Exit Program"; newLog.time = ctime(&rawtime);
					cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
					Logs.push_back(newLog);

					break;
				default:
					bytesSent = send(clientSocket, str_line.c_str(), strlen(str_line.c_str()), 0);
					bytesSent = send(clientSocket, str_error.c_str(), strlen(str_error.c_str()), 0);
					break;
				}
			}
			catch (...)
			{
				cerr << "Error" << endl;
				break;
			}
		}

		closesocket(clientSocket);
		closesocket(endPointSocket);
		WSACleanup();

		//return 0;

	}
}