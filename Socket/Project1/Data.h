#pragma once
#include <vector>
#include <string>
using namespace std;

// µ•¿Ã≈Õ ∞¥√º
class Data
{
public:
	Data() {};
	~Data() {};

	string get_file_location() { return file_location; };
	string get_file_name() { return file_name; };
	vector<string> get_email_list() { return email_list; };	

	void set_file_location(string s) { file_location = s; };
	void set_file_name(string s) { file_name = s; };
	void add_email_list(string s) { email_list.push_back(s); }

private:
	string file_location;
	string file_name;
	vector<string> email_list;
};

