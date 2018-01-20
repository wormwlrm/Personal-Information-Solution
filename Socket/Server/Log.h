#pragma once
#include <iostream>
#include <ctime>
#include <string>
using namespace std;

class Log 
{
public:
	Log() { seconds = 0; };

public:
	string IP;
	string time;
	string activity;
	time_t seconds;
};