#pragma once

#ifdef DLLEXPORT
#define EMAIL_DLL __declspec(dllexport)
#else
#define EMAIL_DLL __declspec(dllimport)
#endif

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <experimental/filesystem>
#include "Data.h"

namespace fs = std::experimental::filesystem;
using namespace std;

class EMAIL_DLL EmailDLL
{
public:
	EmailDLL();
	~EmailDLL();

public:
	void run(string dir);

	string print();
	void save();
	bool check_mail_validation(string s, Data* D);

private:
	vector<Data> Result;
};
