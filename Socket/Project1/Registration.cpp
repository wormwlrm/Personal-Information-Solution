#include "Registration.h"



Registration::Registration()
{
//	registration_pattern = "^(?:[0-9]{2}(?:0[1-9]|1[0-2])(?:0[1-9]|[1,2][0-9]|3[0,1]))-[1-4][0-9]{6}$";
}


Registration::~Registration()
{
}

void Registration::run()
{
	string input;
	int degree;
	cout << "주민등록번호를 입력하세요. : "; cin >> input;
	cout << "강도를 입력하세요. : "; cin >> degree;
	if (check_registration_validation(input, degree))
	{
		cout << "올바른 주민등록번호입니다." << endl;
	}
	else
	{
		cout << "올바르지 않은 주민등록번호입니다." << endl;
	}
}

bool Registration::check_registration_validation(string input, int degree)
{
	int sum;
	int rest;
	switch(degree)
	{
		// 숫자는 모두 붙어있고, 구분자는 '-'만 포함하는 경우
	case 1:
		if (input.at(6) != '-')
			return false;
		sum = make_int(input.at(0)) * 2 + make_int(input.at(1)) * 3 + make_int(input.at(2)) * 4 + make_int(input.at(3)) * 5 + make_int(input.at(4)) * 6 + make_int(input.at(5)) * 7 + 
			make_int(input.at(7)) * 8 + make_int(input.at(8)) * 9 + make_int(input.at(9)) * 2 + make_int(input.at(10)) * 3 + make_int(input.at(11)) * 4 + make_int(input.at(12)) * 5;
		rest = sum % 11;
		if (11 - rest == make_int(input.at(13)))
			return true;
		else
			return false;
		break;

		// 숫자는 모두 붙어있고, 구분자는 모든 특수문자를 포함하는 경우
	case 2:
		if ((int)input.at(6) < 33 || (47 < (int)input.at(6) && (int)input.at(6) < 58) || (64 < (int)input.at(6) && (int)input.at(6) < 91) || (96 < (int)input.at(6) && (int)input.at(6) < 123 ))
			return false;
		sum = make_int(input.at(0)) * 2 + make_int(input.at(1)) * 3 + make_int(input.at(2)) * 4 + make_int(input.at(3)) * 5 + make_int(input.at(4)) * 6 + make_int(input.at(5)) * 7 +
			make_int(input.at(7)) * 8 + make_int(input.at(8)) * 9 + make_int(input.at(9)) * 2 + make_int(input.at(10)) * 3 + make_int(input.at(11)) * 4 + make_int(input.at(12)) * 5;
		rest = sum % 11;
		if (11 - rest == make_int(input.at(13)))
			return true;
		else
			return false;
		break;

		// 모든 문자 사이에는 특수문자를 포함하며 구분자 또한 모든 특수문자를 포함하는 경우
	case 3:
		for (int i = 1; i < 26;)
		{
			if ((int)input.at(i) < 33 || (47 < (int)input.at(i) && (int)input.at(i) < 58) || (64 < (int)input.at(i) && (int)input.at(i) < 91) || (96 < (int)input.at(i) && (int)input.at(i) < 123))
				return false;
			i += 2;
		}
		if ((int)input.at(12) < 33 || (47 < (int)input.at(12) && (int)input.at(12) < 58) || (64 < (int)input.at(12) && (int)input.at(12) < 91) || (96 < (int)input.at(12) && (int)input.at(12) < 123))
			return false;
		sum = make_int(input.at(0)) * 2 + make_int(input.at(2)) * 3 + make_int(input.at(4)) * 4 + make_int(input.at(6)) * 5 + make_int(input.at(8)) * 6 + make_int(input.at(10)) * 7 +
			make_int(input.at(14)) * 8 + make_int(input.at(16)) * 9 + make_int(input.at(18)) * 2 + make_int(input.at(20)) * 3 + make_int(input.at(22)) * 4 + make_int(input.at(24)) * 5;
		rest = sum % 11;
		if (11 - rest == make_int(input.at(26)))
			return true;
		else
			return false;
		break;
	default:
		cout << "잘못된 입력입니다." << endl;
		break;
	}
	return false;
}

int Registration::make_int(char c)
{
	int i = int(c) - '0';
	return i;
}
