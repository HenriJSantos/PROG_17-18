#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

#ifndef DICT_H
#define DICT_H

class Dict
{
public:
	//constructor from file inputFileName
	Dict(std::string &inputFileName);
	//returns a vector of all words present in dictionary
	std::vector<std::string> returnWords();
	//returns wether a given word is valid or not
	bool isValid(std::string word);
	//returns name of original file from which dictionary was created
	std::string originFile();
	//returns a vector of synonyms in a random order for a given word
	std::vector<std::string> obtainShuffledSynonyms(std::string word);

private:
	//map of words and respective synonyms
	std::map<std::string, std::vector<std::string>> dictionary;
	//original file name
	std::string dictOrigin;
};

#endif