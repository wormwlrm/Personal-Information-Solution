#pragma once

#ifdef DLLEXPORT
#define REGISTRATION_DLL __declspec(dllexport)
#else
#define REGISTRATION_DLL __declspec(dllimport)
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

class REGISTRATION_DLL RegistrationDLL
{
public:
	RegistrationDLL();
	~RegistrationDLL();

public:
	void run(string dir, int degree);
	string print();
	bool check_registration_validation(string input, int degree, Data* D);
	int make_int(char c);

private:
	string registration_pattern;
	vector<Data> Result;
};

