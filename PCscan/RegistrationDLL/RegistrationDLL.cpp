// RegistrationDLL.cpp: DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "RegistrationDLL.h"

RegistrationDLL::RegistrationDLL()
{
	//	registration_pattern = "^(?:[0-9]{2}(?:0[1-9]|1[0-2])(?:0[1-9]|[1,2][0-9]|3[0,1]))-[1-4][0-9]{6}$";
}


RegistrationDLL::~RegistrationDLL()
{
}

void RegistrationDLL::run(wstring dir, wstring extenstion, int degree, wstring keyword)
{
	Result.clear();
	Keywords.clear();
	bool AllExtention = false;
	if (extenstion.size() == 0)
	{
		AllExtention = true;
	}
	if (keyword.size() != 0)
	{
		split_keyword(keyword);
	}
	SearchCount = 0;
	DetectCount = 0;
	RegistrationCount = 0;
	wstring file_location, file_name;
	std::locale::global(std::locale("Korean"));
	
	// 재귀적으로 디렉토리 dir 안에 있는 모든 파일 검색
	for (auto& p : fs::recursive_directory_iterator(dir))
	{
		SearchCount++;
		// 확장자가 .txt인 파일을 발견할 경우
		if (extenstion.find(fs::path(p).extension()) != string::npos || AllExtention)
		{			
			Data D;
			// 파일 위치 확인
			file_location = /*fs::current_path().wstring() + L"\\" +*/ fs::path(p).parent_path().wstring() + L"\\" + fs::path(p).filename().wstring();
			D.set_file_location(file_location);

			// 파일 이름 확인
			file_name = fs::path(p).filename().wstring();
			D.set_file_name(file_name);

			// 파일 내부에서 이메일 확인
			wstring dummy;
			bool detected = false;
			wifstream file_reader(fs::path(p).wstring(), std::ios::binary);
			while (file_reader >> dummy)
			{
				if (check_registration_validation(dummy, degree, &D, keyword))
					detected = true;
			}
			if (detected)
			{
				DetectCount++;
			}
			Result.push_back(D);
		}
	}
}

// 주민 검사
bool RegistrationDLL::check_registration_validation(wstring input, int degree, Data* D, wstring keyword)
{
	int sum;
	int rest;
	switch (degree)
	{
		// 숫자는 모두 붙어있고, 구분자는 '-'만 포함하는 경우
	case 1:
		D->set_search_type(L"개인정보");
		if (input.length() < 14)
			return false;
		if (input.at(6) != '-')
			return false;
		sum = make_int(input.at(0)) * 2 + make_int(input.at(1)) * 3 + make_int(input.at(2)) * 4 + make_int(input.at(3)) * 5 + make_int(input.at(4)) * 6 + make_int(input.at(5)) * 7 +
			make_int(input.at(7)) * 8 + make_int(input.at(8)) * 9 + make_int(input.at(9)) * 2 + make_int(input.at(10)) * 3 + make_int(input.at(11)) * 4 + make_int(input.at(12)) * 5;
		rest = sum % 11;
		if ((11 - rest) % 10 == make_int(input.at(13)))
		{
			D->add_email_list(input);			
			RegistrationCount++;
			return true;
		}
		else
			return false;
		break;

		// 숫자는 모두 붙어있고, 구분자는 모든 특수문자를 포함하는 경우
	case 2:
		D->set_search_type(L"개인정보");
		if (input.length() < 14)
			return false;
		if ((int)input.at(6) < 33 || (47 < (int)input.at(6) && (int)input.at(6) < 58) || (64 < (int)input.at(6) && (int)input.at(6) < 91) || (96 < (int)input.at(6) && (int)input.at(6) < 123))
			return false;
		sum = make_int(input.at(0)) * 2 + make_int(input.at(1)) * 3 + make_int(input.at(2)) * 4 + make_int(input.at(3)) * 5 + make_int(input.at(4)) * 6 + make_int(input.at(5)) * 7 +
			make_int(input.at(7)) * 8 + make_int(input.at(8)) * 9 + make_int(input.at(9)) * 2 + make_int(input.at(10)) * 3 + make_int(input.at(11)) * 4 + make_int(input.at(12)) * 5;
		rest = sum % 11;
		if ((11 - rest) % 10 == make_int(input.at(13)))
		{
			D->add_email_list(input);			
			RegistrationCount++;
			return true;
		}
		else
			return false;
		break;

		// 모든 문자 사이에는 특수문자를 포함하며 구분자 또한 모든 특수문자를 포함하는 경우
	case 3:
		D->set_search_type(L"개인정보");
		if (input.length() < 14)
			return false;
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
		if ((11 - rest) % 10 == make_int(input.at(26)))
		{
			D->add_email_list(input);			
			RegistrationCount++;
			return true;
		}
		else
			return false;
		break;

		// 키워드
	case 4:
	{	
		D->set_search_type(L"키워드");
		bool found = false;
		for (size_t i = 0; i < Keywords.size(); i++)
		{
			if (input.find(Keywords.at(i)) != string::npos)
			{
				D->add_email_list(input);
				RegistrationCount++;
				found = true;
			}
		}
		return found;
		break;
	}
	default:
		return false;
		break;
	}
	return false;
}

int RegistrationDLL::make_int(wchar_t c)
{
	// char 숫자 변수를 int로 변환함
	int i = int(c) - '0';
	return i;
}

bool RegistrationDLL::split_keyword(wstring keyword)
{
	wchar_t * buffer;
	wchar_t * part = wcstok_s(&keyword[0], L"/", &buffer);
	while (part)
	{
		Keywords.push_back(part);
		part = wcstok_s(NULL, L"/", &buffer);
	}
	return false;
}



wstring RegistrationDLL::print()
{
	return NULL;
	/*wstring s;
	s = "============================================output=========================================\n                      [ 파일 위치 ]                  [ 파일명 ] [ 건수 ] [ 주민등록번호 ]\n";
	for (size_t i = 0; i < Result.size(); i++)
	{
		s.append(Result.at(i).get_file_location() + "\t");
		s.append(Result.at(i).get_file_name() + "\t");
		s.append(to_string(Result.at(i).get_email_list().size()) + "\t");
		if (Result.at(i).get_email_list().size() != 0)
		{
			for (size_t j = 0; j < Result.at(i).get_email_list().size(); j++)
			{
				s.append(Result.at(i).get_email_list().at(j) + " ");
			}
		}
		s.append("\n");
	}
	return s;
	/*

	cout << "============================================output=========================================" << endl;
	cout << "                         [ 파일 위치 ]                       [ 파일명 ] [ 건수 ] [ 주민등록번호 ]" << endl;
	for (size_t i = 0; i < Result.size(); i++)
	{
	cout.setf(ios::left);
	cout << setw(60) << Result.at(i).get_file_location() << setw(16) << Result.at(i).get_file_name() << setw(6) << Result.at(i).get_email_list().size();
	if (Result.at(i).get_email_list().size() != 0)
	{
	for (size_t j = 0; j < Result.at(i).get_email_list().size(); j++)
	{
	cout << Result.at(i).get_email_list().at(j) << " / ";
	}
	}
	cout << endl;
	}*/
}

bool RegistrationDLL::save()
{
	/*GetLocalTime(&Timer);
	wstring Month = to_wstring(Timer.wMonth); if (Month.size() == 1) Month = L"0" + Month;
	wstring Day = to_wstring(Timer.wDay); if (Day.size() == 1) Day = L"0" + Day;
	wstring Hour = to_wstring(Timer.wHour); if (Hour.size() == 1) Hour = L"0" + Hour;
	wstring Minute = to_wstring(Timer.wMinute); if (Minute.size() == 1) Minute = L"0" + Minute;
	wstring Second = to_wstring(Timer.wSecond); if (Second.size() == 1) Second = L"0" + Second;
	wstring CurrentTime = to_wstring(Timer.wYear) + L"-" + Month + L"-" + Day + L"_" + Hour + L"-" + Minute + L"-" + Second + L"_result.txt";
	wofstream file_saver(CurrentTime);
	
	file_saver << L"검사 파일 수(" << SearchCount << L"), 검출된 파일 수(" << DetectCount << L"), 검출된 개인정보 수(" << RegistrationCount << L")\n";
	for (size_t i = 0 ; i < Result.size(); i++)
	{
		file_saver << Result.at(i).get_file_location() << L"\t" << Result.at(i).get_file_name() << L"\t" << Result.at(i).get_search_type() << L"\t" << Result.at(i).get_email_list().size() << "\n";
	}*/
	return true;
}