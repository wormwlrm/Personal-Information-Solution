#pragma warning(disable:4996)
#include "Main.h"
#include "../Project1/Application.h"

int main(void)
{
	WORD		wVersionRequested; // 윈도우 소켓 버전
	WSADATA		wsaData; // 윈도우 소켓 초기화 정보 구조체
	SOCKADDR_IN servAddr, cliAddr; //소켓 주소 구조체
	SOCKET		endPointSocket, clientSocket; // 소켓 객체
	int			err; // 에러 코드
	int			bytesSent;
	char        buf[BUF_SIZE]; // 버퍼
	string		clientIP; // 클라이언트 IP
	Application	A; // 연산 수행 담당 어플리케이션
	vector<Log>	Logs; // 로그 출력 및 저장
	time_t		rawtime;
	while (true) {
		// 소켓 버전 입력(2.0)
		wVersionRequested = MAKEWORD(2, 0);
		// 윈도우 소켓 초기화
		err = WSAStartup(wVersionRequested, &wsaData);

		// 소켓 버전 확인에서 에러 코드 발생 시
		if (err != 0)
		{
			cout << "Error : 소켓 초기화 과정에서 에러(" << WSAGetLastError() << ")가 발생하였습니다." << endl;
			WSACleanup();
			return 1;
		}

		servAddr.sin_family = AF_INET; // 주소 체계(IPv4 주소 체계에서 사용하므로 항상 AF_INET으로 설정)
		servAddr.sin_port = htons(SERVER_PORT); // 16비트 포트 번호
		servAddr.sin_addr.s_addr = inet_addr(IPAddress); // 서버 IP 설정

		// 서버에서 listen할 서버 소켓 객체 초기화
		// 파라미터 : 주소 체계, 소켓 타입(연결 지향성 소켓) , 프로토콜(TCP)
		endPointSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		// 소켓이 유효하지 않은 경우
		if (endPointSocket == INVALID_SOCKET)
		{
			cout << "Error : 소켓 생성 과정에서 에러(" << WSAGetLastError() << ")가 발생하였습니다." << endl;
			WSACleanup();
			return 1;
		}

		// 바인드
		// 파라미터 : 서버 소켓 객체, 소켓 객체에 부여할 주소 정보를 담은 구조체 이름, 이름 길이
		int x = ::bind(endPointSocket, reinterpret_cast<SOCKADDR *>(&servAddr), sizeof(servAddr));
		if (x == SOCKET_ERROR)
		{
			cout << "Error : 바인딩 과정에서 에러( " << WSAGetLastError() << ")가 발생하였습니다." << endl;
			WSACleanup();
			return 1;
		}

		// 소켓 서버 생성 완료, 결과를 출력하고 클라이언트의 접속을 기다림
		cout << "------------------------------------------------------" << endl;
		cout << "서버 주소 : " << IPAddress << endl;
		cout << "서버 포트 : " << SERVER_PORT << endl;
		cout << "소켓 번호 : " << endPointSocket << endl;
		cout << "------------------------------------------------------" << endl;
		cout << "System : 클라이언트의 접속을 대기하는 중입니다." << endl;

		// 연결 대기열 생성
		// 파라미터 : 서버 소켓 객체, 연결 대기열의 크기
		listen(endPointSocket, 5);

		int xx = sizeof(cliAddr);
		// 연결 요청 가져오고 클라이언트 소켓 초기화
		// 파라미터 : 서버 소켓 객체, 소켓 객체에 부여할 주소 정보를 담은 구조체 이름, 이름 길이
		clientSocket = accept(endPointSocket, reinterpret_cast<SOCKADDR *>(&cliAddr), &xx);

		// 최초 연결 시 연결 여부 확인
		// 연결된 소켓으로부터 데이터를 전달 받음
		// 파라미터 : 클라이언트 소켓 객체, 전달받은 데이터를 저장할 버퍼, 버퍼 길이, 플래그
		int n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
		if (!strcmp(buf, "CONNECTION"))
		{
			bytesSent = send(clientSocket, "SUCCESS", strlen("SUCCESS"), 0);
			n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
			clientIP = buf;
			cout << "System : 클라이언트 " << clientIP << "로부터 접속 시도가 감지되었습니다." << endl;
			bool find_kicked = false;
			for (size_t i = Logs.size(); i > 0; i--)
			{
				// 현재 밴 상태인 경우
				if (Logs.at(i - 1).seconds > 0)
				{
					// 잔여 대기 시간이 남은 경우
					if(BAN_TIME - (time(0) - Logs.at(i - 1).seconds) > 0)
					{
						find_kicked = true;
						cout << "System : 클라이언트 " << clientIP << "는 밴 상태이므로 차단하였습니다." << endl;
						LONGLONG tem = BAN_TIME - (time(0) - Logs.at(i - 1).seconds);
						string temp = "System : 클라이언트 " + clientIP + "는 밴 상태로, " + to_string(tem) + "초 후 접근 가능합니다.\n";
						bytesSent = send(clientSocket, temp.c_str(), strlen(temp.c_str()), 0);
						temp = "System : 잠시 후 다시 접속해 주시기 바랍니다. 명령을 입력하면 종료됩니다.\n";
						bytesSent = send(clientSocket, temp.c_str(), strlen(temp.c_str()), 0);
						cout << "------------------------------------------------------" << endl;
						closesocket(clientSocket);
						closesocket(endPointSocket);
						WSACleanup(); 
					}
					// 잔여 대기 시간이 없는 경우 밴 기록을 지움
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
			cout << "System : 클라이언트 " << clientIP << "와 연결되었습니다." << endl;
			cout << "------------------------------------------------------" << endl;

			// 로그 추가
			Log newLog;  time(&rawtime);
			newLog.IP = clientIP; newLog.activity = "Connection Success"; newLog.time = ctime(&rawtime); newLog.seconds = 0;
			cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
			Logs.push_back(newLog);
		}
		else
		{
			cout << "Error : 클라이언트와 연결에 실패했습니다." << endl;

			// 로그 추가
			Log newLog; time(&rawtime);
			newLog.IP = clientIP; newLog.activity = "Connection Failure"; newLog.time = ctime(&rawtime);
			cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
			Logs.push_back(newLog);

			WSACleanup();
			return 1;
		}

		// 연결에 성공했을 경우 무한 반복
		string str_line = "------------------------------------------------------\n";
		string str_menu = "1. 이메일 검색\n2. 주민등록번호 검색\n0. 프로그램 종료\n";
		string str_error = "잘못된 입력입니다.\n";
		string str_dir = "검색할 디렉토리를 입력해주세요(기본 dir) : \n";
		string str_degree = "강도를 선택해주세요(1,2,3) : \n";
		string str_exit = "연결을 종료합니다.\n";

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
					cout << "System : 연결이 종료되었습니다." << endl;
					break;
				}
				// 기본 메뉴 출력
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
					// 이메일 검색 - 디렉토리 선택
					bytesSent = send(clientSocket, str_line.c_str(), strlen(str_line.c_str()), 0);
					bytesSent = send(clientSocket, str_dir.c_str(), strlen(str_dir.c_str()), 0);
					n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
					str_input = buf;

					// C드라이브 또는 D드라이브를 검색할 때 밴
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

					// 로그 추가				
					newLog.IP = clientIP; newLog.activity = "Search Eamil in directory (" + str_input + ")"; newLog.time = ctime(&rawtime);
					cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
					Logs.push_back(newLog);

					break;
				case 2:
					// 주민등록번호 검색 - 디렉토리 선택
					bytesSent = send(clientSocket, str_line.c_str(), strlen(str_line.c_str()), 0);
					bytesSent = send(clientSocket, str_dir.c_str(), strlen(str_dir.c_str()), 0);
					n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
					str_input = buf;

					// C드라이브 또는 D드라이브를 검색할 때 밴
					if (str_input == "C:\\" || str_input == "D:\\" || str_input == "c:\\" || str_input == "d:\\")
					{
						newLog.seconds = time(0);
						newLog.IP = clientIP; newLog.activity = "Banned for search directory (" + str_input + ")"; newLog.time = ctime(&rawtime);
						cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
						Logs.push_back(newLog);
						exit_program = true;
						break;
					}

					// 주민등록번호 검색 - 디렉토리 선택 - 강도 선택
					bytesSent = send(clientSocket, str_line.c_str(), strlen(str_line.c_str()), 0);
					bytesSent = send(clientSocket, str_degree.c_str(), strlen(str_degree.c_str()), 0);
					n = recv(clientSocket, buf, BUF_SIZE, 0); buf[n] = 0;
					str_input2 = buf;
					int_input = stoi(str_input2);
					A.R.run(str_input, int_input);
					bytesSent = send(clientSocket, A.R.print().c_str(), strlen(A.R.print().c_str()), 0);

					// 로그 추가	
					newLog.IP = clientIP; newLog.activity = "Search Registration Number in directory(" + str_input + ") with degree (" + to_string(int_input) + ")"; newLog.time = ctime(&rawtime);
					cout << clientIP << "\t" << newLog.activity << "\t" << newLog.time;
					Logs.push_back(newLog);
					break;

				case 0:
					exit_program = true;

					// 로그 추가				
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