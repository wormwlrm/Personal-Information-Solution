#include <windows.h>
#include <ShlObj.h>
#include <atlstr.h>
#include <queue>
#include "resource.h"
#include "../RegistrationDLL/RegistrationDLL.h"

#define BUFFER_SIZE 4096
#define SERVER_NAME         L"."
#define SEND_PIPE_NAME      L"PcToRealTime"
#define RECEIVE_PIPE_NAME	L"RealTimeToPc"
#define FULL_SEND_PIPE_NAME      L"\\\\" SERVER_NAME L"\\pipe\\" SEND_PIPE_NAME
#define FULL_RECEIVE_PIPE_NAME	L"\\\\" SERVER_NAME L"\\pipe\\" RECEIVE_PIPE_NAME

HWND RADIO1, RADIO2; // 탐색 정보 선택용 라디오 버튼
HWND EDIT1; // 키워드 선택 시 사용할 수 있는 에디트 박스
HWND EDIT2; // 확장자 선택 시 사용할 수 있는 에디트 박스
HWND EDIT3; // 디렉토리 선택 시 자동으로 선택되며 수정할 수 있는 에디트 박스
HWND BUTTON1; // 디렉토리 선택 창이 열리는 버튼
HWND LIST1; // 검사 결과가 출력되는 리스트
HWND EDIT4, EDIT5, EDIT6; // 검사 파일 수, 검출된 파일 수, 검출된 개인정보 수를 표시하는 에디트 박스
HWND BUTTON2; // 텍스트 파일로 검사 결과 저장하는 버튼
HWND LIST2; // 실시간 검사 결과가 출력되는 리스트

HWND RADIO3, RADIO4; // 실시간 감시용 라디오버튼
HWND EDIT7; // 키워드 입력 에디트 박스
HWND EDIT8; // 확장자 입력
HWND BUTTON3, BUTTON4, BUTTON5, BUTTON6; // 시작, 일시정지, 중지


RegistrationDLL R; // 주민등록번호 검출 클래스
wchar_t DIRECTORY[MAX_PATH]; // 디렉토리 저장 버퍼
wchar_t FILENAMEEXTENSTION[BUFFER_SIZE]; // 확장자 저장 버퍼
wchar_t FILENAMEEXTENSTION2[BUFFER_SIZE]; // 확장자 저장 버퍼
wchar_t KEYWORD[BUFFER_SIZE]; // 키워드 버퍼
wchar_t FROMREALTIME[BUFFER_SIZE]; // 실시간 감지에도 도착한 문자열 버퍼
UINT SEARCHEDFILECOUNT; // 검색한 파일 갯수
UINT DETECTEDFILECOUNT; // 검출된 파일 갯수
UINT DETECTEDREGISTRATIONCOUNT; // 검출된 개인정보 수
HANDLE SAVER; // 파일 저장 핸들
SYSTEMTIME TIMER; // 현재 시각을 저장
HANDLE hThread[2]; // 실시간 감시 쓰레드 핸들

queue<wstring> sendQueue; // 전송용 큐
queue<wstring> receiveQueue; // 수신용 큐

ITEMIDLIST  *pidlBrowse; // System에서 중복되지 않는 유일한 항목을 나타내기 위한 구조체(PIDL)
WCHAR       pszPathname[MAX_PATH]; // 선택한 폴더 경로를 저장
wstring backslash = L"\\";
wstring command = L"";

enum COMMAND {
	COMMAND_NOTHING = 0,
	COMMAND_START,
	COMMAND_PAUSE,
	COMMAND_STOP,
	COMMAND_EXIT
};

INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
void SelectFolder();
unsigned int __stdcall threadFun1(void* lpVoid);
unsigned int __stdcall threadFun2(void* lpVoid);


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), HWND_DESKTOP, DlgProc);
	return 0;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	RADIO1 = ::GetDlgItem(hDlg, IDC_RADIO1);
	RADIO2 = ::GetDlgItem(hDlg, IDC_RADIO2);
	RADIO3 = ::GetDlgItem(hDlg, IDC_RADIO3);
	RADIO4 = ::GetDlgItem(hDlg, IDC_RADIO4);
	EDIT1 = ::GetDlgItem(hDlg, IDC_EDIT1);
	EDIT2 = ::GetDlgItem(hDlg, IDC_EDIT2);
	EDIT3 = ::GetDlgItem(hDlg, IDC_EDIT3);
	EDIT4 = ::GetDlgItem(hDlg, IDC_EDIT4);
	EDIT5 = ::GetDlgItem(hDlg, IDC_EDIT5);
	EDIT6 = ::GetDlgItem(hDlg, IDC_EDIT6);
	EDIT7 = ::GetDlgItem(hDlg, IDC_EDIT7);
	EDIT8 = ::GetDlgItem(hDlg, IDC_EDIT8);
	BUTTON1 = ::GetDlgItem(hDlg, IDC_BUTTON1);
	BUTTON2 = ::GetDlgItem(hDlg, IDC_BUTTON2);
	BUTTON3 = ::GetDlgItem(hDlg, IDC_BUTTON3);
	BUTTON4 = ::GetDlgItem(hDlg, IDC_BUTTON4);
	BUTTON5 = ::GetDlgItem(hDlg, IDC_BUTTON5);
	BUTTON6 = ::GetDlgItem(hDlg, IDC_BUTTON6);
	LIST1 = ::GetDlgItem(hDlg, IDC_LIST1);
	LIST2 = ::GetDlgItem(hDlg, IDC_LIST2);

	switch (message)
	{
	case WM_INITDIALOG:
	{	
		CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
		CheckRadioButton(hDlg, IDC_RADIO3, IDC_RADIO4, IDC_RADIO3);
		SendMessageW(EDIT4, EM_SETREADONLY, TRUE, 0);
		SendMessageW(EDIT5, EM_SETREADONLY, TRUE, 0);
		SendMessageW(EDIT6, EM_SETREADONLY, TRUE, 0);
		SetDlgItemText(hDlg, IDC_EDIT3, L"C:\\");

		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			// PC스캔 탐색 정보 중 개인정보
		case IDC_RADIO1:
			SendMessageW(EDIT1, EM_SETREADONLY, TRUE, 0);
			SetDlgItemText(hDlg, IDC_EDIT1, NULL);
			return TRUE;

			// PC스캔 탐색 정보 중 키워드
		case IDC_RADIO2:
			SendMessageW(EDIT1, EM_SETREADONLY, FALSE, 0);
			return TRUE;

			// 실시간 감시 중 개인정보
		case IDC_RADIO3:
			SendMessageW(EDIT7, EM_SETREADONLY, TRUE, 0);
			SetDlgItemText(hDlg, IDC_EDIT7, NULL);
			return TRUE;

			// 실시간 감시 중 키워드
		case IDC_RADIO4:
			SendMessageW(EDIT7, EM_SETREADONLY, FALSE, 0);
			return TRUE;

			// PC스캔 탐색 정보 중 키워드 에디트박스
		case IDC_EDIT1:
			return TRUE;

			// PC스캔 확장자
		case IDC_EDIT2:
			return TRUE;

			// PC스캔 디렉토리 선택
		case IDC_EDIT3:
			return TRUE;

			// PC스캔 검사 파일 수
		case IDC_EDIT4:
			return TRUE;

			// PC스캔 검출된 파일 수
		case IDC_EDIT5:
			return TRUE;

			// PC스캔 검출된 개인정보 수
		case IDC_EDIT6:
			return TRUE;

			// 실시간 감시 중 키워드 에디트박스
		case IDC_EDIT7:
			return TRUE;

			// 실시간 감시 중 확장자 에디트박스
		case IDC_EDIT8:
			return TRUE;

			// PC스캔 폴더 선택 버튼
		case IDC_BUTTON1:
			SelectFolder();
			if (pidlBrowse != NULL)
			{
				SetDlgItemText(hDlg, IDC_EDIT3, pszPathname);
				return TRUE;
			}
			else
			{
				return TRUE;
			}

			// PC스캔 텍스트 파일 저장 버튼
		case IDC_BUTTON2:
		{	
		//	R.save();
			SelectFolder();
			// 파일 경로를 올바르게 선택한 경우
			if (pidlBrowse != NULL)
			{
				GetLocalTime(&TIMER);
				unsigned short mark = 0xFEFF;

				wstring Month = to_wstring(TIMER.wMonth);
				if (Month.size() == 1) Month = L"0" + Month;
				wstring Day = to_wstring(TIMER.wDay);
				if (Day.size() == 1) Day = L"0" + Day;
				wstring Hour = to_wstring(TIMER.wHour);
				if (Hour.size() == 1) Hour = L"0" + Hour;
				wstring Minute = to_wstring(TIMER.wMinute);
				if (Minute.size() == 1) Minute = L"0" + Minute;
				wstring Second = to_wstring(TIMER.wSecond);
				if (Second.size() == 1) Second = L"0" + Second;
				wstring CurrentTime = pszPathname + backslash + to_wstring(TIMER.wYear) + L"-" + Month + L"-" + Day + L"_" + Hour + L"-" + Minute + L"-" + Second + L"_result.txt";
				SAVER = CreateFile(CurrentTime.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				wstring bufferTitle = L"검사 파일 수(" + to_wstring(R.SearchCount) + L"), 검출된 파일 수(" + to_wstring(R.DetectCount) +
					L"), 검출된 개인정보 수(" + to_wstring(R.RegistrationCount) + L")\r\n";
				DWORD written;
				wstring buf;
				WriteFile(SAVER, &mark, sizeof(mark), &written, NULL);
				WriteFile(SAVER, bufferTitle.c_str(), bufferTitle.length() * sizeof(wchar_t), &written, NULL);
				for (size_t i = 0; i < R.Result.size(); i++)
				{
					buf = R.Result.at(i).get_file_location() + L"\t" + R.Result.at(i).get_file_name() + L"\t"
						+ R.Result.at(i).get_search_type() + L"\t" + to_wstring(R.Result.at(i).get_email_list().size()) + L"\r\n";
					WriteFile(SAVER, buf.c_str(), buf.length() * sizeof(wchar_t), &written, NULL);
				}
				CloseHandle(SAVER);

				MessageBox(hDlg, TEXT("파일이 저장되었습니다."), TEXT("저장 완료"), MB_OK);
				SetDlgItemText(hDlg, IDC_EDIT3, L"C:\\");
				return TRUE;
			}
			else
			{
				return TRUE;
			}
		}
			// 실시간 감시 시작
		case IDC_BUTTON3:
			command = to_wstring(COMMAND_START);
			sendQueue.push(command);
			GetDlgItemText(hDlg, IDC_EDIT8, FILENAMEEXTENSTION2, BUFFER_SIZE);
			command = wstring(FILENAMEEXTENSTION2);
			if (command == L"") {
				command = L"ALL";
			}
			sendQueue.push(command);

			hThread[0] = (HANDLE)_beginthreadex(NULL, 0, threadFun1, NULL, 0, NULL);
			hThread[1] = (HANDLE)_beginthreadex(NULL, 0, threadFun2, NULL, 0, NULL);

			return TRUE;

			// 실시간 감시 일시 정지
		case IDC_BUTTON4:
			if (hThread[0] == NULL || hThread[1] == NULL) {
				break;
			}
			command = to_wstring(COMMAND_PAUSE);
			sendQueue.push(command);
			return TRUE;

			// 실시간 감지 중지
		case IDC_BUTTON5:
			if (hThread[0] == NULL || hThread[1] == NULL) {
				break;
			}
			SendMessage(LIST2, LB_RESETCONTENT, 0, 0);
			command = to_wstring(COMMAND_STOP);
			sendQueue.push(command);
			return TRUE;

			// 실시간 감지 저장
		case IDC_BUTTON6:
			return TRUE;

			// PC스캔 검사 결과
		case IDC_LIST1:
			return TRUE;

			// 실시간 감시 검사 결과
		case IDC_LIST2:
			return TRUE;

			// PC스캔 탐색 시작
		case IDOK:
		{
			SEARCHEDFILECOUNT = 0; // 검색한 파일 갯수
			DETECTEDFILECOUNT = 0; // 검출된 파일 갯수
			DETECTEDREGISTRATIONCOUNT = 0; // 검출된 개인정보 수
			SendMessage(LIST1, LB_RESETCONTENT, 0, 0);

			GetDlgItemText(hDlg, IDC_EDIT3, DIRECTORY, BUFFER_SIZE);
			GetDlgItemText(hDlg, IDC_EDIT2, FILENAMEEXTENSTION, BUFFER_SIZE);

			if (IsDlgButtonChecked(hDlg, IDC_RADIO2))
			{
				GetDlgItemText(hDlg, IDC_EDIT1, KEYWORD, BUFFER_SIZE);
				R.run(DIRECTORY, FILENAMEEXTENSTION, 4, KEYWORD);
			}
			else
			{
				R.run(DIRECTORY, FILENAMEEXTENSTION, 1, KEYWORD);
			}


			SEARCHEDFILECOUNT = R.SearchCount;
			DETECTEDFILECOUNT = R.DetectCount;

			for (size_t i = 0; i < R.Result.size(); i++)
			{
				SendMessage(LIST1, LB_ADDSTRING, 0,
					(LPARAM)((LPCSTR)(R.Result.at(i).get_file_location() + L"(" + to_wstring(R.Result.at(i).get_email_list().size()) +  L")").c_str()));
				/*wstring a = R.Result.at(i).get_file_name();
				LPCTSTR lpsz = (LPCTSTR)a.c_str();
				SendMessage(LIST1, LB_ADDSTRING, 0, (LPARAM)lpsz);*/
			}
			DETECTEDREGISTRATIONCOUNT = R.RegistrationCount;

			SetDlgItemInt(hDlg, IDC_EDIT4, SEARCHEDFILECOUNT, TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT5, DETECTEDFILECOUNT, TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT6, DETECTEDREGISTRATIONCOUNT, TRUE);

			return TRUE;
		}

		// 닫기
		case IDCANCEL:
		{
			if (MessageBox(hDlg, TEXT("종료하시겠습니까?"), TEXT("종료"), MB_ICONQUESTION | MB_YESNO) == IDYES) {
				command = to_wstring(COMMAND_EXIT);
				sendQueue.push(command);
				Sleep(250);
				PostQuitMessage(0); 				
			}
			return TRUE;
		}
		break;
		}
	case WM_CLOSE:
		return FALSE;
	}
	return FALSE;
}

//
// 폴더선택 다이얼로그를 구현하기 위해 사용되는 사용자정의 Callback 함수
//
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg) {
		// 폴더선택 다이얼로그의 초기화가 끝난 경우
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	return 0;
}

//
// 폴더선택 다이얼로그
//
void SelectFolder()
{
	BROWSEINFO  BrInfo;
	//BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;

	memset(&BrInfo, 0x00, sizeof(BrInfo));
	BrInfo.pszDisplayName = pszPathname;
	BrInfo.lpszTitle = _T("탐색할 폴더를 선택하세요.");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	// 이벤트에 대한 사용자정의 함수
	BrInfo.lpfn = BrowseCallbackProc;

	// 사용자정의 함수에 넘겨질 인자로 사용자가 설정하고자 하는 경로를 설정한다.
	// 예를들어 초기폴더경로를 C드라이브로 설정하는 경우
	CString strInitPath = _T("C:\\");
	BrInfo.lParam = (LPARAM)strInitPath.GetBuffer();

	pidlBrowse = ::SHBrowseForFolder(&BrInfo);
	if (pidlBrowse != NULL)
	{
		// 선택된 폴더 경로얻음
		SHGetPathFromIDList(pidlBrowse, pszPathname);
		// 경로(pszPathname)를 이용하여 이후작업 추가
		// http://mooyou.tistory.com/16
	}
}


unsigned int __stdcall threadFun1(void* lpVoid)
{
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	BOOL bCon = FALSE;
	DWORD dwWritten;
	hPipe = CreateNamedPipe(FULL_SEND_PIPE_NAME, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, PIPE_UNLIMITED_INSTANCES, BUFFER_SIZE, 0, 0, NULL);
	bCon = ConnectNamedPipe(hPipe, NULL);
	unsigned short mark = 0xFEFF;
	WriteFile(SAVER, &mark, sizeof(mark), &dwWritten, NULL);

	// CreateNamedPipe가 호출된 후, ConnectedNamedPipe가 호출되기 직전에 클라이언트가 접속을 한 경우
	if ((bCon == false) && (GetLastError() == ERROR_PIPE_CONNECTED)){
		bCon = TRUE;
	}
	while (bCon == TRUE)
	{
		Sleep(250);
		while (sendQueue.size() != 0)
		{
			WriteFile(hPipe, sendQueue.front().c_str(), sendQueue.front().size() * 2, &dwWritten, NULL);
			FlushFileBuffers(hPipe);
			Sleep(250);
			sendQueue.pop();
		}		
	}
	return 0;
}

unsigned int __stdcall threadFun2(void* lpVoid)
{
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	DWORD dwWritten = ERROR_SUCCESS;

	while (true) {
		hPipe = CreateFile(FULL_RECEIVE_PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hPipe != INVALID_HANDLE_VALUE) {
			break;
		}
		Sleep(250);
	}
	while (true) {
		Sleep(250);
		memset(FROMREALTIME, 0, BUFFER_SIZE);
		ReadFile(hPipe, FROMREALTIME, BUFFER_SIZE, &dwWritten, NULL);
		SendMessage(LIST2, LB_ADDSTRING, 0,
				(LPARAM)((LPCSTR)(FROMREALTIME)));
	}
	return 0;
}