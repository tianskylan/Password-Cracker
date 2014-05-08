#include "dictionary.h"
#include <iostream>
#include <fstream>
#include "sha1.h"
#include <windows.h>
#include <sstream>
#include <algorithm>
#include <ppl.h>

using std::cout;
using std::endl;
using std::cin;
using std::getline;
dictionary::dictionary()
{
	myDict.rehash(100000);
}


dictionary::~dictionary()
{
	cout << "clearing the dictionary and deleting its entries." << endl;
	for (auto it = myDict.begin(); it != myDict.end(); it++)
	{
		delete it->second;
	}
	myDict.clear();

	cout << "clearing the unsolved list and deleting its entries." << endl;

	unsolved_list.clear();

	cout << "clearing the lookup map and deleting its entries." << endl;
	for (auto it = look_up_result.begin(); it != look_up_result.end(); it++)
	{
		delete it->second;
	}
	look_up_result.clear();


}

void dictionary::loadDefaultDict()
{
	loadCustomDict("d8.txt");
}

void dictionary::loadCustomDict(std::string filename)
{
	//Error check on loading the same dictionary
	if (filename == dict_name)
	{
		cout << "You can't reload the same dictionary" << endl;
		return;
	}

	cout << "Loading dictionary " << filename << endl;
	dict_name = filename;

	LARGE_INTEGER freq, before, after;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&before);

	std::ifstream fin(filename);
	if (fin.is_open())
	{
		while (!fin.eof())
		{
			pwd_entry *newEntry = new pwd_entry();

			//read in the original unencrypted word
			getline(fin, newEntry->original_word);

			//calculate the sha1 hash of it
			sha1::calc(newEntry->original_word.c_str(), newEntry->original_word.length(), newEntry->hash);

			//convert the hash to hex
			sha1::toHexString(newEntry->hash, newEntry->hex);

			//Store it in the map
			myDict[newEntry->hex] = newEntry;
		}

		QueryPerformanceCounter(&after);
		float fElapsed = static_cast<float>(after.QuadPart - before.QuadPart) /
			freq.QuadPart;

		cout << "Finished loading the dictionary in " << fElapsed << " seconds." << endl;
	}
	else
	{
		cout << "Open dictionary failed." << endl;
	}
}

void dictionary::loadDefaultPwdFile()
{
	loadCustomPwdFile("pass.txt");
}

void dictionary::loadCustomPwdFile(std::string filename)
{
	if (myDict.size() == 0)
	{
		cout << "Please load a dictionary file before you try to decrpyt!" << endl;
		return;
	}

	cout << "cracking password file " << filename << endl;

	LARGE_INTEGER freq, before, after;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&before);

	std::ifstream fin(filename);
	std::ofstream fout("pass_solved.txt");

	int entry_count = 0;

	if (fin.is_open())
	{
		while (!fin.eof())
		{
			decrypted_info *new_info = new decrypted_info();

			getline(fin, new_info->hex);
			new_info->entry_number = entry_count++;

			auto it = myDict.find(new_info->hex);

			if (it == myDict.end())
			{
				unsolved_list.push_back(new_info);
			}
			else
			{
				new_info->plain_text_solution = it->second->original_word;
				look_up_result[new_info->entry_number] = new_info;
			}
		}

		if (unsolved_list.size() > 0)
		{
			bruteForce();
		}

		for (auto it = look_up_result.begin(); it != look_up_result.end(); it++)
		{
			fout << it->second->hex << "," << it->second->plain_text_solution << endl;
		}

		QueryPerformanceCounter(&after);
		float fElapsed = static_cast<float>(after.QuadPart - before.QuadPart) /
			freq.QuadPart;

		cout << "Finished cracking the password file in " << fElapsed << "seconds." << endl;
	}
	else
	{
		cout << "Open password file failed." << endl;
	}
}

//Brute force each password in the unsolved list and put it into the map which
//store the resolved passwords
void dictionary::bruteForce()
{
	cout << "Brute Forcing..." << endl;
	concurrency::parallel_for_each(unsolved_list.begin(), unsolved_list.end(), [this](decrypted_info* it)
	{
		it->plain_text_solution = singleBruteForce(it->hex);
		look_up_result.insert(std::pair<int, decrypted_info*>(it->entry_number, it));
	});
}

//Brute force one hashed hex into plain text if possible. Otherwise return ????
std::string dictionary::singleBruteForce(std::string hex_input)
{
	// -1 means that digit does not exist yet
	// 0~25 => a-z
	// 26-35 => 0-9
	int pin[4] = { -1, -1, -1, 0 };

	while (pin[0] < 35)
	{
		//Convert the integer array to a plain text
		std::string converted_text = convert(pin);
		//Basic Hash the plain text to a hashed hex
		unsigned char hashed_output[20];
		sha1::calc(converted_text.c_str(), converted_text.length(), hashed_output);

		char hex_str[41];
		sha1::toHexString(hashed_output, hex_str);

		//Compare the input hex with the hashed hex

		//if they are equal which returns 0, return the plain text as the decrypted password
		if (strcmp(hex_str, hex_input.c_str()) == 0)
		{
			cout << "Brute force successful: " << converted_text << endl;
			return converted_text;
		}
		//if they are not equal, increment the digits and try again
		else
		{
			increment(pin);
		}
	}

	cout << "Brute force failed, returning ????" << endl;
	return "????";


}

// increment the digits to one
void dictionary::increment(int digits[])
{

	if (digits[3] < 35)
	{
		digits[3]++;
	}
	else if (digits[2] < 35)
	{
		digits[2]++;
		digits[3] -= 35;
	}
	else if (digits[1] < 35)
	{
		digits[1]++;
		digits[2] -= 35;
		digits[3] -= 35;
	}
	else if (digits[0] < 35)
	{
		digits[0]++;
		digits[1] -= 35;
		digits[2] -= 35;
		digits[3] -= 35;
	}
}

//Convert integer digits to a string
std::string dictionary::convert(int digits[])
{
	std::stringstream ss;
	for (int i = 0; i <= 3; i++)
	{
		// a-z
		if (digits[i] >= 0 && digits[i] <= 25)
		{
			ss << static_cast<char>(digits[i] + 97);
		}
		// 0-9
		else if (digits[i] >= 26 && digits[i] <= 35)
		{
			ss << static_cast<char>(digits[i] + 22);
		}
	}
	return ss.str();
}