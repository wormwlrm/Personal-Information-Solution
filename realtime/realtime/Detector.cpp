#include "Detector.h"

// static 멤버 변수를 사용하기 위해 클래스 외부에서 초기 선언
int Detector::choice = CHOICE_NOTHING;
wstring Detector::extension;
queue<FILE_NOTIFY_INFORMATION*> Detector::myQueue;
wchar_t buffer[BUFFER_SIZE];
wchar_t bufferExtension[BUFFER_SIZE];
queue<wstring> receiveQueue;
RegistrationDLL R; // 주민번호 탐색 DLL

Detector::Detector()
{
	setlocale(LC_ALL, "korean");
}

Detector::~Detector()
{
}


int Detector::run()
{
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	DWORD dwWritten = ERROR_SUCCESS;
	
	while(true){
		hPipe = CreateFile(FULL_RECEIVE_PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hPipe != INVALID_HANDLE_VALUE) {
			break;
		}
		Sleep(250);
	}
	wcout << L"-----------------------" << endl;
	wcout << L"실시간 감시 프로그램" << endl;
	wcout << L"-----------------------" << endl;
	while(true){
		ReadFile(hPipe, buffer, BUFFER_SIZE, &dwWritten, NULL);
		receiveQueue.push(buffer);
		if (receiveQueue.size() != 0)
		{
			choice = stoi(buffer);
			wcout << L"현재 ";
			switch (choice) {
			case CHOICE_NOTHING:
			{
				wcout << L"선택되지 않음" << endl;
				break;
			}
			case CHOICE_START:
			{
				wcout << L"실시간 감시 중" << endl;
				receiveQueue.pop();
				memset(bufferExtension, 0, BUFFER_SIZE);
				ReadFile(hPipe, bufferExtension, BUFFER_SIZE, &dwWritten, NULL);
				receiveQueue.push(bufferExtension);
				extension = receiveQueue.front();
				wcout << L"확장자 : " << receiveQueue.front() << endl;
				if (hThread[0] != NULL || hThread[1] != NULL) {
					break;
				}
				// 새로 시작하는 경우
				hThread[0] = (HANDLE)_beginthreadex(NULL, 0, threadFun1, NULL, 0, NULL);
				hThread[1] = (HANDLE)_beginthreadex(NULL, 0, threadFun2, NULL, 0, NULL);
				break;
			}
			case CHOICE_STOP:
			{
				wcout << L"감시 중지됨" << endl;
				memset(bufferExtension, 0, BUFFER_SIZE);
				TerminateThread(hThread[0], 0);
				TerminateThread(hThread[1], 0);
				hThread[0] = NULL;
				hThread[1] = NULL;
				break;
			}
			case CHOICE_PAUSE:
			{
				wcout << L"일시정지" << endl;
				break;
			}
			case CHOICE_EXIT: 
			{
				wcout << L"프로그램을 종료합니다." << endl;
				exit(0);
				break;
			}
			default:
			{
				break;
			}
			}
			receiveQueue.pop();
		}
		Sleep(250);
	}
	CloseHandle(hPipe);
	return 0;
}

unsigned int __stdcall Detector::threadFun1(void* lpVoid)
{
	// 검색할 디렉토리 이름에 접근하는 핸들러를 생성
	HANDLE hDir = CreateFileW(SEARCH_DIRECTORY,
		GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);

	// ReadDirectoryChangesW의 결과값을 저장하고 있을 임시 버퍼
	CONST DWORD buffer = BUFFER_SIZE;

	// 버퍼 포인터
	BYTE* pBuffer = (PBYTE)malloc(buffer);

	// 재귀적으로 탐색할 지 여부를 저장
	BOOL watchSubtree = TRUE;

	// ReadDirectoryChangesW에 사용될 필터링 값
	DWORD notifyFilter = FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE;
	
	DWORD bytesReturned;
	
	// 파일 경로를 저장하는 변수
	WCHAR fileName[MAX_PATH] = { 0 };
	memset(fileName, 0, MAX_PATH);

	int count = 0;
	
	while (true){
		
		Sleep(250); 

		FILE_NOTIFY_INFORMATION* pfni;

		// 핸들러와 그 하위 디렉토리 중 파일 변경을 탐지
		BOOL isChanged = ReadDirectoryChangesW(hDir, pBuffer, buffer,
			watchSubtree, notifyFilter, &bytesReturned, 0, 0);
		if (choice == CHOICE_EXIT) {
			break;
		}
		
		if (isChanged == false){
			DWORD dwLastError = GetLastError();
			CloseHandle(hDir);
			wcout << L"error : " << dwLastError << endl;
			break;
		}
		// 사용자 선택이 실시간 탐색에서 바뀌었다면 파일 내용 변경을 감지하지 않음	
		if (choice != CHOICE_START){
			continue;
		}

		pfni = (FILE_NOTIFY_INFORMATION*)pBuffer;
		CString fileExtension;

		do {
			Sleep(250);
			StringCbCopyNW(fileName, sizeof(fileName), pfni->FileName, pfni->FileNameLength);
			fileExtension = PathFindExtension(fileName);

			// 디렉토리가 아닌 파일이고
			if (fileExtension != L"") {
				// 모두 허용이면
				if (extension == L"ALL") {
					myQueue.push(pfni);
					count++;
				}
				// 사용자가 확장자를 입력했으면
				else if((extension.find(fileExtension) != wstring::npos)){
					myQueue.push(pfni);
					count++;
				}
			}
			
			// 포인터를 다음 값으로 옮김
			pfni = (FILE_NOTIFY_INFORMATION*)((PBYTE)pfni + pfni->NextEntryOffset);
		} while (pfni->NextEntryOffset > 0);

		// 파일 이동류
		if (count > 2)
		{
			StringCbCopyNW(fileName, sizeof(fileName), pfni->FileName, pfni->FileNameLength);
			fileExtension = PathFindExtension(fileName);

			// 디렉토리가 아닌 파일이고
			if (fileExtension != L"") {
				// 모두 허용이면
				if (extension == L"ALL") {
					myQueue.push(pfni);
					count++;
				}
				// 사용자가 확장자를 입력했으면
				else if ((extension.find(fileExtension) != wstring::npos)) {
					myQueue.push(pfni);
					count++;
				}
			}
			count = 0;
		}
	}
	return 0;
}

unsigned int __stdcall Detector::threadFun2(void* lpVoid)
{
	// 파일 정보를 저장하는 구조체
	WIN32_FIND_DATA wfd;
	double fileSize;

	UINT SEARCHEDFILECOUNT = 0; // 검색한 파일 갯수
	UINT DETECTEDFILECOUNT = 0; // 검출된 파일 갯수
	UINT DETECTEDREGISTRATIONCOUNT = 0; // 검출된 개인정보 수

	HANDLE hPipe = INVALID_HANDLE_VALUE;
	BOOL bCon = FALSE;
	DWORD dwWritten;
	hPipe = CreateNamedPipe(FULL_SEND_PIPE_NAME, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, PIPE_UNLIMITED_INSTANCES, BUFFER_SIZE, 0, 0, NULL);
	bCon = ConnectNamedPipe(hPipe, NULL);

	while (true){
		do
		{
			Sleep(250);
			if (choice == CHOICE_EXIT) {
				break;
			}
		} while (choice != CHOICE_START);

		// 쓰레드 1로부터 Queue에 값을 받은 경우
		for (size_t i = 0; i < myQueue.size(); i++){
			fileSize = 0;
			WCHAR fileName[MAX_PATH] = { 0 };
			StringCbCopyNW(fileName, sizeof(fileName), myQueue.front()->FileName, myQueue.front()->FileNameLength);
			wstring temp(L"\\");
			wstring absoluteRoot = SEARCH_DIRECTORY + temp + fileName;
			DETECTEDREGISTRATIONCOUNT = 0; // 검출된 개인정보 수

			// FindFirstFile의 파라미터로 파일 위치를 입력받은 핸들러 선언
			HANDLE hFile = FindFirstFile(absoluteRoot.c_str(), &wfd);
			if (hFile == INVALID_HANDLE_VALUE){

				CloseHandle(hFile);
			}
			else {
				SEARCHEDFILECOUNT++;
				fileSize = (wfd.nFileSizeHigh * ((double)MAXDWORD + 1)) + wfd.nFileSizeLow;
				// 파일 크기가 MAXIMUM_SIZE 이상이면 알람 띄우지 않음
				if (fileSize > MAXIMUM_SIZE) {
					myQueue.pop();
					continue;
				}
				// 파일 크기가 MAMIMUM_SIZE 이하이면 개인정보 탐색
				else {
					Data D;
					D.set_file_location(absoluteRoot);
					D.set_file_name(fileName);
					D.set_search_type(L"개인정보");
					wstring dummy;
					bool detected = false;
					wifstream file_reader(absoluteRoot, std::ios::binary);

					while (file_reader >> dummy){
						if (R.check_registration_validation(dummy, 1, &D, L"")) {
							detected = true;
							DETECTEDREGISTRATIONCOUNT++;
						}
					}
					if (detected) {
						DETECTEDFILECOUNT++;
					}
					R.Result.push_back(D);
				}
				

			}
			switch (myQueue.front()->Action){
				case FILE_ACTION_ADDED:
				{
					temp = L"FILE_ACTION_ADDED : \t";
					break;
				}
				case FILE_ACTION_REMOVED:
				{
					temp = L"FILE_ACTION_REMOVED : \t";
					break;
				}
				case FILE_ACTION_MODIFIED:
				{	
					temp = L"FILE_ACTION_MODIFIED : \t";
					break;
				}
				case FILE_ACTION_RENAMED_OLD_NAME:
				{
					temp = L"FILE_ACTION_RENAMED_OLD_NAME : \t";
					break;
				}
				case FILE_ACTION_RENAMED_NEW_NAME:
				{
					temp = L"FILE_ACTION_RENAMED_NEW_NAME : \t";
					break;
				}
				default:
				{
					temp = L"ERROR : \t";
					break;
				}
			}
			wcout << temp << absoluteRoot << "(" << DETECTEDREGISTRATIONCOUNT << ")" << endl;

			// CreateNamedPipe가 호출된 후, ConnectedNamedPipe가 호출되기 직전에 클라이언트가 접속을 한 경우
			if ((bCon == false) && (GetLastError() == ERROR_PIPE_CONNECTED)) {
				bCon = TRUE;
			}
			if (bCon == TRUE) {
				wstring sendingMessage = absoluteRoot + L"(" + to_wstring(DETECTEDREGISTRATIONCOUNT) + L")\n";
				WriteFile(hPipe, sendingMessage.c_str(), sendingMessage.size() * 2, &dwWritten, NULL);
				FlushFileBuffers(hPipe);
				Sleep(250);
			}
			myQueue.pop();
			Sleep(250);
		}		
	}
	DisconnectNamedPipe(hPipe);
	return 0;
}