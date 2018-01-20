#pragma once
#include <iostream>
#include <string>
//#include <regex>

using namespace std;

class Registration
{
public:
	Registration();
	~Registration();

public:
	void run();
	bool check_registration_validation(string input, int degree);
	int make_int(char c);

private:
	string registration_pattern;
};

