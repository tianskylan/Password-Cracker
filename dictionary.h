#pragma once

#include <string>
#include <unordered_map>
#include <map>

struct pwd_entry
{
	std::string original_word;
	unsigned char hash[20];
	char hex[41];
};

struct decrypted_info
{
	int entry_number;
	std::string hex;
	std::string plain_text_solution;
};


class dictionary
{
private:
	// Key:Hex String - Value:dictionary entry
	std::unordered_map<std::string, pwd_entry*> myDict;

	std::map <int, decrypted_info*> look_up_result;
	std::list<decrypted_info*> unsolved_list;

	std::string dict_name;

public:
	dictionary();
	~dictionary();
	// load dictionary
	void loadDefaultDict();
	void loadCustomDict(std::string);

	// load and decrypt password file
	void loadDefaultPwdFile();
	void loadCustomPwdFile(std::string);

	// solve the password by brute force
	void bruteForce();
	std::string singleBruteForce(std::string);
	void increment(int[]);
	std::string convert(int[]);

};

