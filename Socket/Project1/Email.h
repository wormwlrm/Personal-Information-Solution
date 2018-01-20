#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <experimental/filesystem>
#include "Data.h"

namespace fs = std::experimental::filesystem;
using namespace std;

class Email
{
public:
	Email();
	~Email();

public:
	void run();

	void print();
	void save();
	bool check_mail_validation(string s, Data* D);

private:
	vector<Data> Result;
};

