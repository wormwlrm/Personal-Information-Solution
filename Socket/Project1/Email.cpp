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
	// ��������� ���丮 dir �ȿ� �ִ� ��� ���� �˻�
	for (auto& p : fs::recursive_directory_iterator("dir"))
	{
		// Ȯ���ڰ� .txt�� ������ �߰��� ���
		if (fs::path(p).extension() == ".txt")
		{
			Data D;
			// ���� ��ġ Ȯ��
			file_location = fs::current_path().string() + "\\" + fs::path(p).parent_path().string();
			D.set_file_location(file_location);

			// ���� �̸� Ȯ��
			file_name = fs::path(p).filename().string();
			D.set_file_name(file_name);

			// ���� ���ο��� �̸��� Ȯ��
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
	// ����
	cout << "============================================output=========================================" << endl;
	cout << "                         [ ���� ��ġ ]                       [ ���ϸ� ] [ �Ǽ� ] [ �̸��� ]" << endl;
	// ���
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
	// ��� �ϳ��� @�� .�� �����ϰ� �ִ� ��, @�� .���� �ռ� ������ �� üũ
	if (((s.find("@") != std::string::npos) && (s.find(".") != std::string::npos)))
	{
		int at_index = s.find("@");
		int dot_index = s.find(".");

		// �߸��� ���̽� 1 : @���� .�� �ռ� ���� ���
		if (at_index > dot_index)
		{
			return false;
		}

		// �߸��� ���̽� 2 : @ �� ���ڿ��� �߸��� ���
		for (int i = at_index - 1; i >= 0; i--)
		{
			front = s.at(i);
			if (!(("0" <= front && front <= "9") || ("a" <= front && front <= "z") || ("A" <= front && front <= "Z") || (front == "_") || (front == "-")))
			{
				return false;
			}
		}

		// �߸��� ���̽� 3 : @���� .������ ���ڿ��� �߸��� ���
		for (int i = at_index + 1; i < dot_index; i++)
		{
			back = s.at(i);
			if (!(("0" <= back && back <= "9") || ("a" <= back && back <= "z") || ("A" <= back && back <= "Z")))
			{
				return false;
			}
		}

		// �߸��� ���̽� 4 : .���� ������������ ���ڿ��� �߸��� ���
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
		if (count < 2 || count > 4) // 2~4 ���� �̿��� ���
		{
			return false;
		}
		D->add_email_list(s);
		return true;
	}
	return false;
}
