#include "Email.h"

Email::Email()
{
}

Email::~Email()
{
}

void Email::run()
{
	string file_location, file_name;
	// 재귀적으로 디렉토리 dir 안에 있는 모든 파일 검색
	for (auto& p : fs::recursive_directory_iterator("dir"))
	{
		// 확장자가 .txt인 파일을 발견할 경우
		if (fs::path(p).extension() == ".txt")
		{
			Data D;
			// 파일 위치 확인
			file_location = fs::current_path().string() + "\\" + fs::path(p).parent_path().string();
			D.set_file_location(file_location);

			// 파일 이름 확인
			file_name = fs::path(p).filename().string();
			D.set_file_name(file_name);

			// 파일 내부에서 이메일 확인
			string dummy;
			ifstream file_reader(fs::path(p).string());
			while (file_reader >> dummy)
			{
				check_mail_validation(dummy, &D);
			}
			Result.push_back(D);
		}
	}
	print();
//	save();
}

void Email::print()
{
	// 저장
	cout << "============================================output=========================================" << endl;
	cout << "                         [ 파일 위치 ]                       [ 파일명 ] [ 건수 ] [ 이메일 ]" << endl;
	// 출력
	for (size_t i = 0; i < Result.size(); i++)
	{
		cout.setf(ios::left);
		cout << setw(60) << Result.at(i).get_file_location() << setw(16) << Result.at(i).get_file_name() << setw(6) << Result.at(i).get_email_list().size();
		if (Result.at(i).get_email_list().size() != 0)
		{
			for (size_t j = 0; j < Result.at(i).get_email_list().size(); j++)
			{
				cout << Result.at(i).get_email_list().at(j) << " ";
			}
		}
		cout << endl;
	}
}

void Email::save()
{
	ofstream file_saver("result.txt");
	string dummy;
	for (size_t i = 0; i < Result.size(); i++)
	{
		if (Result.at(i).get_email_list().size() != 0)
		{
			file_saver << Result.at(i).get_file_location() << "\t" << Result.at(i).get_file_name() << "\t" << Result.at(i).get_email_list().size() << "\t";

			for (size_t j = 0; j < Result.at(i).get_email_list().size(); j++)
			{
				file_saver << Result.at(i).get_email_list().at(j) << "\t";
			}
		}
		file_saver << endl;
	}
	file_saver.close();
}


bool Email::check_mail_validation(string s, Data* D)
{
	int count = 0;
	string front, back;
	// 적어도 하나의 @와 .을 포함하고 있는 지, @가 .보다 앞서 나오는 지 체크
	if (((s.find("@") != std::string::npos) && (s.find(".") != std::string::npos)))
	{
		int at_index = s.find("@");
		int dot_index = s.find(".");

		// 잘못된 케이스 1 : @보다 .가 앞서 나온 경우
		if (at_index > dot_index)
		{
			return false;
		}

		// 잘못된 케이스 2 : @ 앞 문자열이 잘못된 경우
		for (int i = at_index - 1; i >= 0; i--)
		{
			front = s.at(i);
			if (!(("0" <= front && front <= "9") || ("a" <= front && front <= "z") || ("A" <= front && front <= "Z") || (front == "_") || (front == "-")))
			{
				return false;
			}
		}

		// 잘못된 케이스 3 : @에서 .까지의 문자열이 잘못된 경우
		for (int i = at_index + 1; i < dot_index; i++)
		{
			back = s.at(i);
			if (!(("0" <= back && back <= "9") || ("a" <= back && back <= "z") || ("A" <= back && back <= "Z")))
			{
				return false;
			}
		}

		// 잘못된 케이스 4 : .에서 마지막까지의 문자열이 잘못된 경우
		int count = 0;
		for (size_t i = dot_index + 1; i < s.length(); i++)
		{
			back = s.at(i);
			if (!(("0" <= back && back <= "9") || ("a" <= back && back <= "z") || ("A" <= back && back <= "Z")))
			{
				return false;
			}
			count++;
		}
		if (count < 2 || count > 4) // 2~4 글자 이외의 경우
		{
			return false;
		}
		D->add_email_list(s);
		return true;
	}
	return false;
}
