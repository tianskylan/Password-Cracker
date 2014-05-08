// password.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <sstream>
#include <string>

#include "stdafx.h"
#include "sha1.h"
#include "dictionary.h"

using std::cout;
using std::cin;
using std::endl;

//Test function to be called at the end of basic Hashing
//Not used in the final version
void bruteForceTest(std::string hex_input)
{
	cout << "Testing string: " << hex_input << endl;
	dictionary dict;
	cout << "Single Brute Force result: " << dict.singleBruteForce(hex_input) << endl;;
}

void basicHashing()
{
	std::string sample;

	cout << "In [Basic Hashing]..." << endl;
	cout << "Please enter a sample password: ";
	std::getline(cin, sample);

	unsigned char hashed_output[20];


	sha1::calc(sample.c_str(), sample.length(), hashed_output);

	char hex_str[41];
	sha1::toHexString(hashed_output, hex_str);

	cout << "Hashed: " << hex_str << endl;

}


void decrypt(dictionary &dict)
{
	std::string part3_option;
	do
	{
		cout << endl;
		cout << "In [Decrypt]..." << endl;
		cout << "1. Load default password file" << endl;
		cout << "2. Load your custom password file" << endl;
		cout << "3. Return to previous level" << endl;

		std::getline(cin, part3_option);

		if (part3_option[1] != '\0')
		{
			cout << "Wrong option. Try again." << endl << endl;
		}

		else if (part3_option[0] == '1')
		{
			dict.loadDefaultPwdFile();
		}

		else if (part3_option[0] == '2')
		{
			cout << "Please enter the filename of your password file" << endl;
			std::string input;
			std::getline(cin, input);

			dict.loadCustomPwdFile(input);
		}

		else if (part3_option[0] == '3')
		{
			cout << "Returning to previous menu..." << endl;
		}

	} while (part3_option[0] != '3');

}

void part2Options(dictionary &dict)
{
	std::string part2_option;

	do
	{
		cout << endl;
		cout << "In [Loading Dictionary]..." << endl;
		cout << "1. Load default dictionary" << endl;
		cout << "2. Load your custom dictionary" << endl;
		cout << "3. Return to previous level" << endl;

		std::getline(cin, part2_option);
		
		if (part2_option[1] != '\0')
		{
			cout << "Wrong option. Try again." << endl << endl;
		}

		else if (part2_option[0] == '1')
		{
			dict.loadDefaultDict();
		}

		else if (part2_option[0] == '2')
		{
			cout << "Please enter the filename of your dictionary" << endl;
			std::string input;
			std::getline(cin, input);

			dict.loadCustomDict(input);
		}

		else if (part2_option[0] == '3')
		{
			cout << "Returning to previous menu..." << endl;
		}

	} while (part2_option[0] != '3');
}

void printMenu(dictionary &dict)
{
	std::string option;

	do
	{
		cout << endl;
		cout << "In [Main Menu]..." << endl;
		cout << "1. Basic Hashing" << endl;
		cout << "2. Load Dictionary" << endl;
		cout << "3. Decrypt" << endl;
		cout << "4. Exit" << endl;

		std::getline(cin, option);

		if (option[1] != '\0')
		{
			cout << "Wrong option. Try again." << endl << endl;
		}

		else if (option[0] == '1')
		{
			basicHashing();
		}

		else if (option[0] == '2')
		{
			part2Options(dict);
		}

		else if (option[0] == '3')
		{
			decrypt(dict);
		}

	} while (option[0] != '4');

}



int _tmain(int argc, _TCHAR* argv[])
{
	dictionary dict;
	printMenu(dict);
	return 0;
}