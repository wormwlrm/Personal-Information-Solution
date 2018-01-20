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
@brief	사용자 선택한 내용을 문자열로 저장하는 enum
*/
enum CHOICE { 
	CHOICE_NOTHING = 0, 
	CHOICE_START, 
	CHOICE_PAUSE, 
	CHOICE_STOP, 
	CHOICE_EXIT 
};

/*
 @brief	비동기 쓰레드를 이용한 실시간 감지 클래스
 */
class Detector
{
public:
	/*
	@brief	사용자가 선택한 내용을 가리키는 포인터 초기화
	*/
	Detector();

	/*
	@brief	소멸자
	*/
	virtual ~Detector();

	/*
	@brief	사용자로부터 입력을 받는 인터페이스를 제공하는 클래스
	*/
	int run();

	/*
	@brief	파일 변경 감지 시 확장자 정보 확인 후 큐에 보관
	*/
	static unsigned int __stdcall threadFun1(void *lpVoid);

	/*
	@brief	보관된 큐에서 파일 크기 정책에 맞는 파일인지 필터링 후 보고
	*/
	static unsigned int __stdcall threadFun2(void *lpVoid);

private:
	static int choice; // 사용자의 입력을 저장하는 변수
	void* lpVoid; // 쓰레드에 들어가는 파라미터
	static wstring extension; // 사용자의 확장자 입력을 저장하는 변수
	static queue<FILE_NOTIFY_INFORMATION*> myQueue; // 파일 정보 구조체를 저장하는 큐
	HANDLE hThread[THREAD_COUNT]; // 쓰레드 핸들을 저장하는 변수
};

