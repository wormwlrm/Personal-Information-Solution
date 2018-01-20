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

HWND RADIO1, RADIO2; // Ž�� ���� ���ÿ� ���� ��ư
HWND EDIT1; // Ű���� ���� �� ����� �� �ִ� ����Ʈ �ڽ�
HWND EDIT2; // Ȯ���� ���� �� ����� �� �ִ� ����Ʈ �ڽ�
HWND EDIT3; // ���丮 ���� �� �ڵ����� ���õǸ� ������ �� �ִ� ����Ʈ �ڽ�
HWND BUTTON1; // ���丮 ���� â�� ������ ��ư
HWND LIST1; // �˻� ����� ��µǴ� ����Ʈ
HWND EDIT4, EDIT5, EDIT6; // �˻� ���� ��, ����� ���� ��, ����� �������� ���� ǥ���ϴ� ����Ʈ �ڽ�
HWND BUTTON2; // �ؽ�Ʈ ���Ϸ� �˻� ��� �����ϴ� ��ư
HWND LIST2; // �ǽð� �˻� ����� ��µǴ� ����Ʈ

HWND RADIO3, RADIO4; // �ǽð� ���ÿ� ������ư
HWND EDIT7; // Ű���� �Է� ����Ʈ �ڽ�
HWND EDIT8; // Ȯ���� �Է�
HWND BUTTON3, BUTTON4, BUTTON5, BUTTON6; // ����, �Ͻ�����, ����


RegistrationDLL R; // �ֹε�Ϲ�ȣ ���� Ŭ����
wchar_t DIRECTORY[MAX_PATH]; // ���丮 ���� ����
wchar_t FILENAMEEXTENSTION[BUFFER_SIZE]; // Ȯ���� ���� ����
wchar_t FILENAMEEXTENSTION2[BUFFER_SIZE]; // Ȯ���� ���� ����
wchar_t KEYWORD[BUFFER_SIZE]; // Ű���� ����
wchar_t FROMREALTIME[BUFFER_SIZE]; // �ǽð� �������� ������ ���ڿ� ����
UINT SEARCHEDFILECOUNT; // �˻��� ���� ����
UINT DETECTEDFILECOUNT; // ����� ���� ����
UINT DETECTEDREGISTRATIONCOUNT; // ����� �������� ��
HANDLE SAVER; // ���� ���� �ڵ�
SYSTEMTIME TIMER; // ���� �ð��� ����
HANDLE hThread[2]; // �ǽð� ���� ������ �ڵ�

queue<wstring> sendQueue; // ���ۿ� ť
queue<wstring> receiveQueue; // ���ſ� ť

ITEMIDLIST  *pidlBrowse; // System���� �ߺ����� �ʴ� ������ �׸��� ��Ÿ���� ���� ����ü(PIDL)
WCHAR       pszPathname[MAX_PATH]; // ������ ���� ��θ� ����
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
			// PC��ĵ Ž�� ���� �� ��������
		case IDC_RADIO1:
			SendMessageW(EDIT1, EM_SETREADONLY, TRUE, 0);
			SetDlgItemText(hDlg, IDC_EDIT1, NULL);
			return TRUE;

			// PC��ĵ Ž�� ���� �� Ű����
		case IDC_RADIO2:
			SendMessageW(EDIT1, EM_SETREADONLY, FALSE, 0);
			return TRUE;

			// �ǽð� ���� �� ��������
		case IDC_RADIO3:
			SendMessageW(EDIT7, EM_SETREADONLY, TRUE, 0);
			SetDlgItemText(hDlg, IDC_EDIT7, NULL);
			return TRUE;

			// �ǽð� ���� �� Ű����
		case IDC_RADIO4:
			SendMessageW(EDIT7, EM_SETREADONLY, FALSE, 0);
			return TRUE;

			// PC��ĵ Ž�� ���� �� Ű���� ����Ʈ�ڽ�
		case IDC_EDIT1:
			return TRUE;

			// PC��ĵ Ȯ����
		case IDC_EDIT2:
			return TRUE;

			// PC��ĵ ���丮 ����
		case IDC_EDIT3:
			return TRUE;

			// PC��ĵ �˻� ���� ��
		case IDC_EDIT4:
			return TRUE;

			// PC��ĵ ����� ���� ��
		case IDC_EDIT5:
			return TRUE;

			// PC��ĵ ����� �������� ��
		case IDC_EDIT6:
			return TRUE;

			// �ǽð� ���� �� Ű���� ����Ʈ�ڽ�
		case IDC_EDIT7:
			return TRUE;

			// �ǽð� ���� �� Ȯ���� ����Ʈ�ڽ�
		case IDC_EDIT8:
			return TRUE;

			// PC��ĵ ���� ���� ��ư
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

			// PC��ĵ �ؽ�Ʈ ���� ���� ��ư
		case IDC_BUTTON2:
		{	
		//	R.save();
			SelectFolder();
			// ���� ��θ� �ùٸ��� ������ ���
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
				wstring bufferTitle = L"�˻� ���� ��(" + to_wstring(R.SearchCount) + L"), ����� ���� ��(" + to_wstring(R.DetectCount) +
					L"), ����� �������� ��(" + to_wstring(R.RegistrationCount) + L")\r\n";
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

				MessageBox(hDlg, TEXT("������ ����Ǿ����ϴ�."), TEXT("���� �Ϸ�"), MB_OK);
				SetDlgItemText(hDlg, IDC_EDIT3, L"C:\\");
				return TRUE;
			}
			else
			{
				return TRUE;
			}
		}
			// �ǽð� ���� ����
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

			// �ǽð� ���� �Ͻ� ����
		case IDC_BUTTON4:
			if (hThread[0] == NULL || hThread[1] == NULL) {
				break;
			}
			command = to_wstring(COMMAND_PAUSE);
			sendQueue.push(command);
			return TRUE;

			// �ǽð� ���� ����
		case IDC_BUTTON5:
			if (hThread[0] == NULL || hThread[1] == NULL) {
				break;
			}
			SendMessage(LIST2, LB_RESETCONTENT, 0, 0);
			command = to_wstring(COMMAND_STOP);
			sendQueue.push(command);
			return TRUE;

			// �ǽð� ���� ����
		case IDC_BUTTON6:
			return TRUE;

			// PC��ĵ �˻� ���
		case IDC_LIST1:
			return TRUE;

			// �ǽð� ���� �˻� ���
		case IDC_LIST2:
			return TRUE;

			// PC��ĵ Ž�� ����
		case IDOK:
		{
			SEARCHEDFILECOUNT = 0; // �˻��� ���� ����
			DETECTEDFILECOUNT = 0; // ����� ���� ����
			DETECTEDREGISTRATIONCOUNT = 0; // ����� �������� ��
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

		// �ݱ�
		case IDCANCEL:
		{
			if (MessageBox(hDlg, TEXT("�����Ͻðڽ��ϱ�?"), TEXT("����"), MB_ICONQUESTION | MB_YESNO) == IDYES) {
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
// �������� ���̾�α׸� �����ϱ� ���� ���Ǵ� ��������� Callback �Լ�
//
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg) {
		// �������� ���̾�α��� �ʱ�ȭ�� ���� ���
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	return 0;
}

//
// �������� ���̾�α�
//
void SelectFolder()
{
	BROWSEINFO  BrInfo;
	//BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;

	memset(&BrInfo, 0x00, sizeof(BrInfo));
	BrInfo.pszDisplayName = pszPathname;
	BrInfo.lpszTitle = _T("Ž���� ������ �����ϼ���.");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	// �̺�Ʈ�� ���� ��������� �Լ�
	BrInfo.lpfn = BrowseCallbackProc;

	// ��������� �Լ��� �Ѱ��� ���ڷ� ����ڰ� �����ϰ��� �ϴ� ��θ� �����Ѵ�.
	// ������� �ʱ�������θ� C����̺�� �����ϴ� ���
	CString strInitPath = _T("C:\\");
	BrInfo.lParam = (LPARAM)strInitPath.GetBuffer();

	pidlBrowse = ::SHBrowseForFolder(&BrInfo);
	if (pidlBrowse != NULL)
	{
		// ���õ� ���� ��ξ���
		SHGetPathFromIDList(pidlBrowse, pszPathname);
		// ���(pszPathname)�� �̿��Ͽ� �����۾� �߰�
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

	// CreateNamedPipe�� ȣ��� ��, ConnectedNamedPipe�� ȣ��Ǳ� ������ Ŭ���̾�Ʈ�� ������ �� ���
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