#pragma once

#define THREAD_COUNT 2
#define BUFFER_SIZE 4096
#define MAXIMUM_SIZE 10485760
#define SEARCH_DIRECTORY L"C:\\Users\\comtrue\\Desktop\\testcase"
#define SERVER_NAME         L"."
#define RECEIVE_PIPE_NAME	L"PcToRealTime"
#define SEND_PIPE_NAME      L"RealTimeToPc"
#define FULL_SEND_PIPE_NAME      L"\\\\" SERVER_NAME L"\\pipe\\" SEND_PIPE_NAME
#define FULL_RECEIVE_PIPE_NAME	L"\\\\" SERVER_NAME L"\\pipe\\" RECEIVE_PIPE_NAME

#include "Data.h"
#include "../../T111/RegistrationDLL/RegistrationDLL.h"
#include <queue>
#include <string>
#include <iostream>
#include <Windows.h>
#include <atlstr.h>
#include <process.h>
#include <strsafe.h>

using namespace std;

/*
@brief	����� ������ ������ ���ڿ��� �����ϴ� enum
*/
enum CHOICE { 
	CHOICE_NOTHING = 0, 
	CHOICE_START, 
	CHOICE_PAUSE, 
	CHOICE_STOP, 
	CHOICE_EXIT 
};

/*
 @brief	�񵿱� �����带 �̿��� �ǽð� ���� Ŭ����
 */
class Detector
{
public:
	/*
	@brief	����ڰ� ������ ������ ����Ű�� ������ �ʱ�ȭ
	*/
	Detector();

	/*
	@brief	�Ҹ���
	*/
	virtual ~Detector();

	/*
	@brief	����ڷκ��� �Է��� �޴� �������̽��� �����ϴ� Ŭ����
	*/
	int run();

	/*
	@brief	���� ���� ���� �� Ȯ���� ���� Ȯ�� �� ť�� ����
	*/
	static unsigned int __stdcall threadFun1(void *lpVoid);

	/*
	@brief	������ ť���� ���� ũ�� ��å�� �´� �������� ���͸� �� ����
	*/
	static unsigned int __stdcall threadFun2(void *lpVoid);

private:
	static int choice; // ������� �Է��� �����ϴ� ����
	void* lpVoid; // �����忡 ���� �Ķ����
	static wstring extension; // ������� Ȯ���� �Է��� �����ϴ� ����
	static queue<FILE_NOTIFY_INFORMATION*> myQueue; // ���� ���� ����ü�� �����ϴ� ť
	HANDLE hThread[THREAD_COUNT]; // ������ �ڵ��� �����ϴ� ����
};

