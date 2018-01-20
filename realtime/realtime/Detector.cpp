#include "Detector.h"

// static ��� ������ ����ϱ� ���� Ŭ���� �ܺο��� �ʱ� ����
int Detector::choice = CHOICE_NOTHING;
wstring Detector::extension;
queue<FILE_NOTIFY_INFORMATION*> Detector::myQueue;
wchar_t buffer[BUFFER_SIZE];
wchar_t bufferExtension[BUFFER_SIZE];
queue<wstring> receiveQueue;
RegistrationDLL R; // �ֹι�ȣ Ž�� DLL

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
	wcout << L"�ǽð� ���� ���α׷�" << endl;
	wcout << L"-----------------------" << endl;
	while(true){
		ReadFile(hPipe, buffer, BUFFER_SIZE, &dwWritten, NULL);
		receiveQueue.push(buffer);
		if (receiveQueue.size() != 0)
		{
			choice = stoi(buffer);
			wcout << L"���� ";
			switch (choice) {
			case CHOICE_NOTHING:
			{
				wcout << L"���õ��� ����" << endl;
				break;
			}
			case CHOICE_START:
			{
				wcout << L"�ǽð� ���� ��" << endl;
				receiveQueue.pop();
				memset(bufferExtension, 0, BUFFER_SIZE);
				ReadFile(hPipe, bufferExtension, BUFFER_SIZE, &dwWritten, NULL);
				receiveQueue.push(bufferExtension);
				extension = receiveQueue.front();
				wcout << L"Ȯ���� : " << receiveQueue.front() << endl;
				if (hThread[0] != NULL || hThread[1] != NULL) {
					break;
				}
				// ���� �����ϴ� ���
				hThread[0] = (HANDLE)_beginthreadex(NULL, 0, threadFun1, NULL, 0, NULL);
				hThread[1] = (HANDLE)_beginthreadex(NULL, 0, threadFun2, NULL, 0, NULL);
				break;
			}
			case CHOICE_STOP:
			{
				wcout << L"���� ������" << endl;
				memset(bufferExtension, 0, BUFFER_SIZE);
				TerminateThread(hThread[0], 0);
				TerminateThread(hThread[1], 0);
				hThread[0] = NULL;
				hThread[1] = NULL;
				break;
			}
			case CHOICE_PAUSE:
			{
				wcout << L"�Ͻ�����" << endl;
				break;
			}
			case CHOICE_EXIT: 
			{
				wcout << L"���α׷��� �����մϴ�." << endl;
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
	// �˻��� ���丮 �̸��� �����ϴ� �ڵ鷯�� ����
	HANDLE hDir = CreateFileW(SEARCH_DIRECTORY,
		GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);

	// ReadDirectoryChangesW�� ������� �����ϰ� ���� �ӽ� ����
	CONST DWORD buffer = BUFFER_SIZE;

	// ���� ������
	BYTE* pBuffer = (PBYTE)malloc(buffer);

	// ��������� Ž���� �� ���θ� ����
	BOOL watchSubtree = TRUE;

	// ReadDirectoryChangesW�� ���� ���͸� ��
	DWORD notifyFilter = FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE;
	
	DWORD bytesReturned;
	
	// ���� ��θ� �����ϴ� ����
	WCHAR fileName[MAX_PATH] = { 0 };
	memset(fileName, 0, MAX_PATH);

	int count = 0;
	
	while (true){
		
		Sleep(250); 

		FILE_NOTIFY_INFORMATION* pfni;

		// �ڵ鷯�� �� ���� ���丮 �� ���� ������ Ž��
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
		// ����� ������ �ǽð� Ž������ �ٲ���ٸ� ���� ���� ������ �������� ����	
		if (choice != CHOICE_START){
			continue;
		}

		pfni = (FILE_NOTIFY_INFORMATION*)pBuffer;
		CString fileExtension;

		do {
			Sleep(250);
			StringCbCopyNW(fileName, sizeof(fileName), pfni->FileName, pfni->FileNameLength);
			fileExtension = PathFindExtension(fileName);

			// ���丮�� �ƴ� �����̰�
			if (fileExtension != L"") {
				// ��� ����̸�
				if (extension == L"ALL") {
					myQueue.push(pfni);
					count++;
				}
				// ����ڰ� Ȯ���ڸ� �Է�������
				else if((extension.find(fileExtension) != wstring::npos)){
					myQueue.push(pfni);
					count++;
				}
			}
			
			// �����͸� ���� ������ �ű�
			pfni = (FILE_NOTIFY_INFORMATION*)((PBYTE)pfni + pfni->NextEntryOffset);
		} while (pfni->NextEntryOffset > 0);

		// ���� �̵���
		if (count > 2)
		{
			StringCbCopyNW(fileName, sizeof(fileName), pfni->FileName, pfni->FileNameLength);
			fileExtension = PathFindExtension(fileName);

			// ���丮�� �ƴ� �����̰�
			if (fileExtension != L"") {
				// ��� ����̸�
				if (extension == L"ALL") {
					myQueue.push(pfni);
					count++;
				}
				// ����ڰ� Ȯ���ڸ� �Է�������
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
	// ���� ������ �����ϴ� ����ü
	WIN32_FIND_DATA wfd;
	double fileSize;

	UINT SEARCHEDFILECOUNT = 0; // �˻��� ���� ����
	UINT DETECTEDFILECOUNT = 0; // ����� ���� ����
	UINT DETECTEDREGISTRATIONCOUNT = 0; // ����� �������� ��

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

		// ������ 1�κ��� Queue�� ���� ���� ���
		for (size_t i = 0; i < myQueue.size(); i++){
			fileSize = 0;
			WCHAR fileName[MAX_PATH] = { 0 };
			StringCbCopyNW(fileName, sizeof(fileName), myQueue.front()->FileName, myQueue.front()->FileNameLength);
			wstring temp(L"\\");
			wstring absoluteRoot = SEARCH_DIRECTORY + temp + fileName;
			DETECTEDREGISTRATIONCOUNT = 0; // ����� �������� ��

			// FindFirstFile�� �Ķ���ͷ� ���� ��ġ�� �Է¹��� �ڵ鷯 ����
			HANDLE hFile = FindFirstFile(absoluteRoot.c_str(), &wfd);
			if (hFile == INVALID_HANDLE_VALUE){

				CloseHandle(hFile);
			}
			else {
				SEARCHEDFILECOUNT++;
				fileSize = (wfd.nFileSizeHigh * ((double)MAXDWORD + 1)) + wfd.nFileSizeLow;
				// ���� ũ�Ⱑ MAXIMUM_SIZE �̻��̸� �˶� ����� ����
				if (fileSize > MAXIMUM_SIZE) {
					myQueue.pop();
					continue;
				}
				// ���� ũ�Ⱑ MAMIMUM_SIZE �����̸� �������� Ž��
				else {
					Data D;
					D.set_file_location(absoluteRoot);
					D.set_file_name(fileName);
					D.set_search_type(L"��������");
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

			// CreateNamedPipe�� ȣ��� ��, ConnectedNamedPipe�� ȣ��Ǳ� ������ Ŭ���̾�Ʈ�� ������ �� ���
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