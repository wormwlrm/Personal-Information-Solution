#pragma once
#pragma warning(disable:4251)

#ifdef _WINDLL
#define REGISTRATION_DLL __declspec(dllexport)
#else
#define REGISTRATION_DLL __declspec(dllimport)
#endif

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <locale>
#include <ctime>
#include <experimental/filesystem>
#include "Data.h"

template class __declspec(dllexport) std::vector<Data>;
namespace fs = std::experimental::filesystem;
using namespace std;

class REGISTRATION_DLL RegistrationDLL
{
public:
	RegistrationDLL();
	~RegistrationDLL();

public:
	void run(wstring dir, wstring extenstion, int degree, wstring keyword);
	wstring print();
	bool check_registration_validation(wstring input,  int degree, Data* D, wstring keyword);
	bool save();
	int make_int(wchar_t c);
	bool split_keyword(wstring keyword);

public:
	string registration_pattern;
	vector<Data> Result;
	int SearchCount;
	int DetectCount;
	int RegistrationCount;
//	SYSTEMTIME Timer;
	vector<wstring> Keywords;
};

