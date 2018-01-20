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

	wstring get_file_location() { return file_location; };
	wstring get_file_name() { return file_name; };
	vector<wstring> get_email_list() { return email_list; };
	wstring get_search_type() { return search_type; };

	void set_file_location(wstring s) { file_location = s; };
	void set_file_name(wstring s) { file_name = s; };
	void add_email_list(wstring s) { email_list.push_back(s); };
	void set_search_type(wstring s) { search_type = s; };

private:
	wstring file_location;
	wstring file_name;
	vector<wstring> email_list;
	wstring search_type;
};

