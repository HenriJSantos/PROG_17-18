#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>

#include "Dict.h"

std::string upper(std::string str)
{
	transform(str.begin(), str.end(), str.begin(), toupper);
	return str;
}

std::vector<std::string> extractSynonyms(std::string line)
{
	std::vector<std::string> synonyms;
	std::string word = "";
	for (unsigned int i = 0; i < line.size(); i++)
	{
		if (line.at(i) != ' ')
		{
			if (line.at(i) != ',')
				word += line.at(i);
			else
			{
				synonyms.push_back(word);
				word = "";
			}
		}
	}
	synonyms.push_back(word);
	return synonyms;
}

std::vector<std::string> Dict::returnWords()
{
	std::map<std::string, std::vector<std::string>> inMap = this->dictionary;
	std::vector<std::string> words;
	for (auto const &element : this->dictionary)
		words.push_back(element.first);
	return words;
}

Dict::Dict(std::string &inputFileName)
{
	std::ifstream inputStream;
	std::string line;
	std::string word;
	std::vector<std::string> synonyms;

	inputStream.open(inputFileName);
	while (!inputStream.is_open())
	{
		std::cin.clear();
		std::cin.ignore(9999, '\n');
		std::cout << "Input dictionary file not found, try again: ";
		std::cin >> inputFileName;
		inputStream.open(inputFileName);
	}
	std::cout << "Dictionary opened successfully." << std::endl;
	this->dictOrigin = inputFileName;

	while (getline(inputStream, line))
	{
		line = upper(line);
		word = line.substr(0, line.find(':'));
		synonyms = extractSynonyms(line.substr(line.find(':') + 1, line.size()));
		this->dictionary.insert(std::pair<std::string, std::vector<std::string>>(word, synonyms));
	}
}

bool Dict::isValid(std::string word)
{
	if (this->dictionary.find(upper(word)) != this->dictionary.end())
		return true;
	else
		return false;
}

std::string Dict::originFile()
{
	return this->dictOrigin;
}

std::vector<std::string> Dict::obtainShuffledSynonyms(std::string word)
{
	std::vector<std::string> synonyms = this->dictionary.at(word);
	unsigned seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();

	shuffle(synonyms.begin(), synonyms.end(), std::default_random_engine(seed));
	return synonyms;
}