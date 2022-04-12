#include <iostream>
#include <vector>
#include <fstream>
#include <optional>
#include <tuple>
#include <string>
#include <algorithm>
using namespace std;

struct  Person
{
	string m_name;
	string m_surname;
	optional<string>m_patronymic;
	friend ostream& operator << (ostream& out, Person& p);
	friend bool operator < (Person& p1, Person& p2);
	friend bool operator == (Person& p1, Person& p2);
};
ostream& operator << (ostream& out, Person& p)
{
	if (p.m_patronymic.has_value())
	{
		out << p.m_surname << " " << p.m_name << " " << p.m_patronymic.value();
	}
	else
	{
		out << p.m_surname << " " << p.m_name;
	}
	return out;
}

bool operator < (Person& p1, Person& p2)
{
	return tie(p1.m_name, p1.m_surname, p1.m_patronymic) < tie(p2.m_name, p2.m_surname, p2.m_patronymic);
}

bool operator == (const Person& p1, const Person& p2)
{
	return tie(p1.m_name, p1.m_surname, p1.m_patronymic) == tie(p2.m_name, p2.m_surname, p2.m_patronymic);
}

struct PhoneNumber
{
	int m_codeCountry;
	int m_codeCity;
	string m_number;
	optional<int>m_extensionNumber;
	friend ostream& operator << (ostream& out, PhoneNumber& pn);
	friend bool operator < (PhoneNumber& pn1, PhoneNumber& pn2);
	friend bool operator == (PhoneNumber& pn1, PhoneNumber& pn2);
};

ostream& operator << (ostream& out, PhoneNumber& pn) 
{
	if (pn.m_extensionNumber.has_value())
	{
		out << "+" << pn.m_codeCountry << "(" << pn.m_codeCity << ")" << pn.m_number <<" "<< pn.m_extensionNumber.value();
	}
	else
	{
		out << "+" << pn.m_codeCountry << "(" << pn.m_codeCity << ")" << pn.m_number;
	}
	return out;
}

bool operator < (const PhoneNumber& pn1,const PhoneNumber& pn2)
{
	return tie(pn1.m_codeCountry, pn1.m_codeCity, pn1.m_number, pn1.m_extensionNumber) < tie(pn2.m_codeCountry, pn2.m_codeCity, pn2.m_number, pn2.m_extensionNumber);
}

bool operator == (const PhoneNumber& pn1, const PhoneNumber& pn2)
{
	return tie(pn1.m_codeCountry, pn1.m_codeCity, pn1.m_number, pn1.m_extensionNumber) == tie(pn2.m_codeCountry, pn2.m_codeCity, pn2.m_number, pn2.m_extensionNumber);
}

class PhoneBook
{
private:
	vector<pair<Person, PhoneNumber>>book;

	static bool compare_surname(const pair<Person, PhoneNumber>& p1, const pair<Person, PhoneNumber>& p2)
	{
		return p1.first.m_surname < p2.first.m_surname;
	}

	static bool compare_phone(const pair<Person, PhoneNumber>& p1, const pair<Person, PhoneNumber>& p2)
	{
		return p1.second < p2.second;
		//return tie(p1.second.m_codeCountry, p1.second.m_codeCity, p1.second.m_number, p1.second.m_extensionNumber) < tie(p2.second.m_codeCountry, p2.second.m_codeCity, p2.second.m_number, p2.second.m_extensionNumber);
	}

public:
	PhoneBook(ifstream &file)
	{
		while (!file.eof())
		{
			string l[5];
			int arr[3];
			Person p;
			PhoneNumber pn;
			file >> l[0];
			file >> l[1];
			file >> l[2];
			if (l[2] == "-")
			{
				p = { l[1],l[0] };
				file >> arr[0];
				file >> arr[1];
				file >> l[2];
				file >> l[3];
				if (l[3] == "-")
				{
					pn = { arr[0], arr[1],l[2]};
				}
				else
				{
					pn = { arr[0], arr[1],l[2], stoi(l[3])};
				}
			}
			else
			{
				p = { l[1],l[0],l[2] };
				file >> arr[0];
				file >> arr[1];
				file >> l[3];
				file >> l[4];
				if (l[4] != "-")
				{
					pn = { arr[0],arr[1],l[3],stoi(l[4]) };
				}
				else
				{
					pn = { arr[0],arr[1],l[3]};
				}
			}
			book.push_back(make_pair(p, pn));
			
		}
		
	}

	void SortByName()
	{
		sort(book.begin(), book.end(), compare_surname);
	}

	void SortByPhone()
	{
		sort(book.begin(), book.end(), compare_phone);
	}

	tuple<string, PhoneNumber> GetPhoneNumber(string surname)
	{
		int8_t cnt = 0;
		for_each(book.begin(), book.end(), [surname, &cnt](const auto& pers) 
		{
				if (pers.first.m_surname == surname)
				{
					cnt++;
				}
		});

		for (const auto& [pers, phone] : book)
		{
			if (pers.m_surname == surname && cnt==1)
			{
				return tie("", phone);
			}
		}
		PhoneNumber errorNumber = { 0,0,"0",0 };
		if (cnt == 0)
		{
			return tie("not found", errorNumber);
		}
		else if(cnt > 1)
		{
			return tie("found more than 1", errorNumber);
		}
	}

	void ChangePhoneNumber(const Person &p,const PhoneNumber& pn)
	{
		for (auto& [pers, phone] : book)
		{
			if (pers == p)
			{
				phone.m_codeCountry = pn.m_codeCountry;
				phone.m_codeCity = pn.m_codeCity;
				phone.m_number = pn.m_number;
				phone.m_extensionNumber.value() = pn.m_extensionNumber.value();
			}
		}
	}

	friend ostream& operator << (ostream& out, PhoneBook& pB);
};

ostream& operator << (ostream& out, PhoneBook& pB)
{
	for (auto& [person, phoneNumber] : pB.book)
	{
		/*if (person.m_patronymic.has_value())
		{
			out << person.m_surname << " " << person.m_name << " " << person.m_patronymic.value() << "\t";
		}
		else
		{
			out << person.m_surname << " " << person.m_name << "\t";
		}*/
		out << person << "\t";
		/*if (phoneNumber.m_extensionNumber.has_value())
		{
			out << phoneNumber.m_codeCountry << " " << phoneNumber.m_codeCity << " " << phoneNumber.m_number << " " << phoneNumber.m_extensionNumber.value() << endl;
		}
		else
		{
			out << phoneNumber.m_codeCountry << " " << phoneNumber.m_codeCity << " " << phoneNumber.m_number << endl;
		}*/
		out << phoneNumber << endl;
	}
	return out;
}


int main()
{
	ifstream file("PhoneBook.txt");
	PhoneBook book(file);
	cout << book;
	cout << "------SortByPhone-------" << endl;
	book.SortByPhone();
	cout << book;
	cout << "------SortByName--------" << endl;
	book.SortByName();
	cout << book;
	cout << "-----GetPhoneNumber-----" << endl;
		auto print_phone_number = [&book](const string& surname) {
		cout << surname << "\t";
		auto answer = book.GetPhoneNumber(surname);
		if (get<0>(answer).empty())
			cout << get<1>(answer);
		else
			cout << get<0>(answer);
		cout << endl;
	};
	print_phone_number("Ivanov");
	print_phone_number("Petrov");
	cout << "----ChangePhoneNumber----" << endl;
	book.ChangePhoneNumber(Person{"Kotov", "Vasilii", "Eliseevich" },
		PhoneNumber{ 7, 123, "15344458", nullopt });
	book.ChangePhoneNumber(Person{ "Mironova", "Margarita", "Vladimirovna" },
		PhoneNumber{ 16, 465, "9155448", 13 });
	cout << book;
}
