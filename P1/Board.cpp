#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <Windows.h>
#include <vector>

#include "Board.h"

//color codes
#define BLACK 0
#define CYAN 3
#define RED 4
#define LIGHTGRAY 7
#define LIGHTBLUE 9
#define WHITE 15


//changes font color and background color
void setcolor(unsigned int color, unsigned int background_color)
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	if (background_color == BLACK)
		SetConsoleTextAttribute(hCon, color);
	else
		SetConsoleTextAttribute(hCon, color | BACKGROUND_BLUE | BACKGROUND_GREEN |
			BACKGROUND_RED);
}

//returns upper equivalent of string
std::string Upper(std::string str)
{
	transform(str.begin(), str.end(), str.begin(), toupper);
	return str;
}

//loads a board from file called boardName
Board::Board(std::string &boardName)
{
	std::ifstream inputStream;
	std::vector<std::vector<char>> tempBoardDef;
	std::string line;

	inputStream.open(boardName);
	while (!inputStream.is_open() || boardName.at(0) != 'b')
	{
		std::cin.clear();
		std::cin.ignore(9999, '\n');
		std::cout << "Invalid board file name, try again: ";
		std::cin >> boardName;
		inputStream.open(boardName);
	}
	std::cout << "Board opened successfully." << std::endl;

	//get rid of first two lines (not part of board)
	getline(inputStream, line);
	getline(inputStream, line);

	unsigned int lines;
	unsigned int cols;

	std::vector<char> currLine;

	//make first line (with column index letters)
	getline(inputStream, line);
	cols = (unsigned int)line.size() / 2;
	currLine.push_back(' ');
	for (unsigned int i = 0; i < cols; i++)
	{
		currLine.push_back('a' + i);
	}
	tempBoardDef.push_back(currLine);
	currLine.clear();

	//while no blank lines are encountered, line is part of the board so integrate it in board
	do
	{
		//push the line index letter
		currLine.push_back('A' + (int)tempBoardDef.size() - 1);
		//push each character of line one by one
		for (unsigned int i = 0; i < line.size(); i += 2)
		{
			currLine.push_back(line.at(i));
		}
		//push finished line into temporary board definition
		tempBoardDef.push_back(currLine);
		//resets currLine
		currLine.clear();
	} while ((getline(inputStream, line) && line.size() != 0));

	lines = (unsigned int)tempBoardDef.size() - 1;
	cols = (unsigned int)tempBoardDef.at(0).size() - 1;

	//set private board as the finished temporary definition
	this->board = tempBoardDef;
	//set private dimensions
	this->numLines = lines;
	this->numCols = cols;

	//add every word to the private words vector that contains all words present and starting positions
	while (getline(inputStream, line))
	{
		std::string startTile;
		std::string currWord;
		startTile = line.substr(0, 3);
		currWord = line.substr(4, line.size() - 4);
		this->words.push_back(make_pair(startTile, currWord));
	}
}

//creates a new board, given its dimensions
Board::Board(unsigned int l, unsigned int c)
{
	std::vector<std::vector<char>> tempBoardDef(l + 1, std::vector<char>(c + 1));
	this->numLines = l;
	this->numCols = c;
	//fill board with dots
	for (unsigned int i = 0; i <= this->numLines; i++)
	{
		fill(tempBoardDef.at(i).begin(), tempBoardDef.at(i).end(), '.');
	}
	//clean first position
	tempBoardDef.at(0).at(0) = ' ';
	//place letter coordinates for the lines
	for (unsigned int i = 0; i < this->numLines; i++)
	{
		tempBoardDef.at(i + 1).at(0) = 'A' + i;
	}
	//place letter coordinates for the columns
	for (unsigned int i = 0; i < this->numCols; i++)
	{
		tempBoardDef.at(0).at(i + 1) = 'a' + i;
	}
	this->board = tempBoardDef;
}

void Board::removeWord(std::string startTile)
{
	{
		//resets whole board to dots
		for (unsigned int i = 1; i <= this->numLines; i++)
			for (unsigned int j = 1; j <= this->numCols; j++)
				this->board.at(i).at(j) = '.';

		//finds the word at specified startTile in words vector and removes it from the vector
		for (unsigned int i = 0; i < this->words.size(); i++)
		{
			if (this->words.at(i).first == startTile)
			{
				this->words.erase(this->words.begin() + i);
				break;
			}
		}

		//adds any words left in vector back into board
		for (unsigned int i = 0; i < this->words.size(); i++)
		{
			insertWord(this->words.at(i).first, this->words.at(i).second, true);
		}
		return;
	}
}

void Board::insertWord(std::string startTile, std::string inputWord, bool repeat)
{
	if (!(std::cin.eof()))
	{
		inputWord = Upper(inputWord);
		unsigned int line = startTile.at(0) - 'A' + 1,
			column = startTile.at(1) - 'a' + 1;
		char direction = startTile.at(2);
		//checks if word is already placed
		for (unsigned int i = 0; i < this->words.size(); i++)
		{
			if (this->words.at(i).second == inputWord && repeat == false)
			{
				std::cout << "Word is already in board." << std::endl;
				return;
			}
		}
		//see if it fits and put in if possible
		//horizontal
		if (direction == 'H')
		{
			//too big to fit
			if (inputWord.size() > (this->numCols - column + 1))
			{
				std::cout << "Input word is too long." << std::endl;
				return;
			}
			//check for conflicts with other words in space reserved for current word
			for (unsigned int i = 0; i < inputWord.size(); i++)
			{
				if (this->board.at(line).at(column + i) != '.' &&
					this->board.at(line).at(column + i) != '?' &&
					this->board.at(line).at(column + i) != inputWord.at(i))
				{
					std::cout << "Word conflicts with another word in board." << std::endl;
					return;
				}
			}
			//insert
			//black squares (#)
			if (column - 1 > 0)
				if (this->board.at(line).at(column - 1) == '.')
					this->board.at(line).at(column - 1) = '#';
			if (column + inputWord.size() <= this->numCols)
				if (this->board.at(line).at(column + inputWord.size()) == '.')
					this->board.at(line).at(column + inputWord.size()) = '#';
			//actual word
			if (!repeat)
				this->words.push_back(make_pair(startTile, inputWord));
			for (unsigned int i = 0; i < inputWord.size(); i++)
			{
				this->board.at(line).at(column + i) = inputWord.at(i);
			}
			return;
		}
		//vertical
		if (direction == 'V')
		{
			//too big to fit
			if (inputWord.size() >(this->numLines - line + 1))
			{
				std::cout << "Input word is too long" << std::endl;
				return;
			}
			//check for conflicts with other words
			for (unsigned int i = 0; i < inputWord.size(); i++)
			{
				//Check for space in front for word
				if (this->board.at(line + i).at(column) != '.' &&
					this->board.at(line + i).at(column) != '?' &&
					this->board.at(line + i).at(column) != inputWord.at(i))
				{
					std::cout << "Word conflicts with another word in board." << std::endl;
					return;
				}
			}
			//insert
			//black squares (#)
			if (column - 1 > 0)
				if (this->board.at(line - 1).at(column) == '.')
					this->board.at(line - 1).at(column) = '#';
			if (line + inputWord.size() <= this->numLines)
				if (this->board.at(line + inputWord.size()).at(column) == '.')
					this->board.at(line + inputWord.size()).at(column) = '#';
			//actual word
			if (!repeat)
				this->words.push_back(make_pair(startTile, inputWord));
			for (unsigned int i = 0; i < inputWord.size(); i++)
			{
				this->board.at(line + i).at(column) = inputWord.at(i);
			}
			return;
		}
	}
}

void Board::insertGuess(std::string startTile, std::string inputWord, bool repeat, Board originalBoard)
{
	if (!(std::cin.eof()))
	{
		inputWord = Upper(inputWord);
		unsigned int line = startTile.at(0) - 'A' + 1,
			column = startTile.at(1) - 'a' + 1;
		char direction = startTile.at(2);
		//checks if word is already placed
		for (unsigned int i = 0; i < this->words.size(); i++)
		{
			if (this->words.at(i).second == inputWord && repeat == false)
			{
				std::cout << "Word is already in board." << std::endl;
				return;
			}
		}
		//see if it fits and put in if possible
		//horizontal
		if (direction == 'H')
		{
			//remove if overwriting a word
			for (unsigned int i = 0; i < this->words.size(); i++)
			{
				if (startTile == this->words.at(i).first)
					this->removeGuess(originalBoard, startTile);
			}

			//check if word fits
			int i = 0;
			int spaces = 0;
			while (column + i <= this->numCols && this->board.at(line).at(column + i) != '#')
			{
				spaces++;
				i++;
			}

			if (inputWord.size() != spaces)
			{
				std::cout << "Input word has wrong size." << std::endl;
				return;
			}

			//check for conflicts with other words in space reserved for current word
			for (unsigned int i = 0; i < inputWord.size(); i++)
			{
				if (this->board.at(line).at(column + i) != '.' &&
					this->board.at(line).at(column + i) != '?' &&
					this->board.at(line).at(column + i) != inputWord.at(i))
				{
					std::cout << "Word conflicts with another word in board." << std::endl;
					return;
				}
			}
			//insert
			//black squares (#)
			if (column - 1 > 0)
				if (this->board.at(line).at(column - 1) == '.')
					this->board.at(line).at(column - 1) = '#';
			if (column + inputWord.size() <= this->numCols)
				if (this->board.at(line).at(column + inputWord.size()) == '.')
					this->board.at(line).at(column + inputWord.size()) = '#';
			//actual word
			if (!repeat)
				this->words.push_back(make_pair(startTile, inputWord));
			for (unsigned int i = 0; i < inputWord.size(); i++)
			{
				this->board.at(line).at(column + i) = inputWord.at(i);
			}
			return;
		}
		//vertical
		if (direction == 'V')
		{
			//remove if overwriting a word
			for (unsigned int i = 0; i < this->words.size(); i++)
			{
				if (startTile == this->words.at(i).first)
					this->removeGuess(originalBoard, startTile);
			}

			//check if word fits
			int i = 0;
			int spaces = 0;
			while (line + i <= this->numLines && this->board.at(line + i).at(column) != '#')
			{
				spaces++;
				i++;
			}

			if (inputWord.size() != spaces)
			{
				std::cout << "Input word has wrong size." << std::endl;
				return;
			}

			//check for conflicts with other words
			for (unsigned int i = 0; i < inputWord.size(); i++)
			{
				//Check for space in front for word
				if (this->board.at(line + i).at(column) != '.' &&
					this->board.at(line + i).at(column) != '?' &&
					this->board.at(line + i).at(column) != inputWord.at(i))
				{
					std::cout << "Word conflicts with another word in board." << std::endl;
					return;
				}
			}
			//insert word
			if (!repeat)
				this->words.push_back(make_pair(startTile, inputWord));
			for (unsigned int i = 0; i < inputWord.size(); i++)
			{
				this->board.at(line + i).at(column) = inputWord.at(i);
			}
			return;
		}
	}
}

void Board::addNewlyGeneratedWord(std::string startTile, std::vector<std::string> validWords)
{
	unsigned int line = startTile.at(0) - 'A' + 1;
	unsigned int column = startTile.at(1) - 'a' + 1;
	std::string word = "";
	if (startTile.at(2) == 'H')
		for (unsigned int i = 0; i < this->numCols - column; i++)
		{
			if (this->board.at(line).at(column + i) == '#' || this->board.at(line).at(column + i) == '.')
				break;
			else
				word += this->board.at(line).at(column + i);
		}
	else if (startTile.at(2) == 'V')
		for (unsigned int i = 0; i < this->numLines - line; i++)
		{
			if (this->board.at(line + i).at(column) == '#' || this->board.at(line + i).at(column) == '.')
				break;
			else
				word += this->board.at(line + i).at(column);
		}
	if (std::find(validWords.begin(), validWords.end(), word) != validWords.end())
	{
		std::cout << "Word \"" << word << "\"successfully added to word list." << std::endl;
		this->words.push_back(std::make_pair(startTile, word));
	}
	else
		std::cout << "Not a valid word." << std::endl;
}

void wildcardMatch(std::vector<std::string> dictList, std::string currWord, std::vector<std::string> &validWords)
{
	for (unsigned int word = 0; word < dictList.size(); word++)
	{
		bool valid = true;
		if (dictList.at(word).size() > currWord.size())
			continue;
		for (unsigned int letter = 0; letter < dictList.at(word).size(); letter++)
		{
			if (!(currWord.at(letter) == '?' || dictList.at(word).at(letter) == currWord.at(letter)))
				valid = false;
			if (letter == dictList.at(word).size() - 1 && dictList.at(word).size() < currWord.size())
				if (currWord.at(dictList.at(word).size()) != '?')
					valid = false;
		}
		if (valid)
			validWords.push_back(dictList.at(word));
	}
}

void Board::showHelp(std::string startTile, std::vector<std::string> words)
{
	std::string currWord = "";
	std::vector<std::string> validWords;
	unsigned int line = startTile.at(0) - 'A' + 1;
	unsigned int column = startTile.at(1) - 'a' + 1;
	char direction = startTile.at(2);
	if (direction == 'H')
	{
		for (unsigned int currCol = column; currCol <= this->numCols; currCol++)
		{
			if (this->board.at(line).at(currCol) == '#')
				break;
			else if (this->board.at(line).at(currCol) == '.')
				currWord += '?';
			else
				currWord += this->board.at(line).at(currCol);
		}

		wildcardMatch(words, currWord, validWords);
	}

	if (direction == 'V')
	{
		for (unsigned int currLine = line; currLine < this->numLines; currLine++)
		{
			if (this->board.at(currLine).at(column) == '#')
				break;
			else if (this->board.at(currLine).at(column) == '.')
				currWord += '?';
			else
				currWord += this->board.at(currLine).at(column);
		}

		wildcardMatch(words, currWord, validWords);
	}

	unsigned int suggestions = 5;
	if (validWords.size() == 0)
		std::cout << "No suggestions found." << std::endl;
	else if (validWords.size() <= suggestions)
	{
		setcolor(RED, BLACK);
		std::cout << "Suggestions:" << std::endl;
		setcolor(LIGHTGRAY, BLACK);
		for (unsigned int i = 0; i < validWords.size(); i++)
		{
			std::cout << validWords.at(i) << std::endl;
		}
	}
	else
	{
		srand((int)time(NULL));
		setcolor(RED, BLACK);
		std::cout << "Suggestions:" << std::endl;
		setcolor(LIGHTGRAY, BLACK);
		for (unsigned int i = 0; i < suggestions; i++)
		{
			int sug = rand() % validWords.size();
			std::cout << validWords.at(sug) << std::endl;
			validWords.erase(std::find(validWords.begin(), validWords.end(), validWords.at(sug)));
		}
	}
}

bool Board::checkForInvalidWords(std::vector<std::string> validWords)
{
	//horizonally
	for (unsigned int currLine = 1; currLine <= this->numLines; currLine++)
	{
		//reset current word
		std::string currWord = "";
		for (unsigned int currCol = 1; currCol <= this->numCols; currCol++)
		{
			//if a black space is encountered, check if the current word is valid
			if (this->board.at(currLine).at(currCol) == '#')
			{
				//if the currWord's size is 0 or 1, then it's not a word
				if (currWord.size() > 1)
				{
					bool valid = false;
					for (unsigned int word = 0; word < validWords.size(); word++)
					{
						if (validWords.at(word) == currWord)
							valid = true;
					}
					if (!valid)
						return true;
				}
				currWord = "";
			}

			//same treatment as black spaces, but for dots
			else if (this->board.at(currLine).at(currCol) == '.')
			{
				if (currWord.size() > 1)
				{
					bool valid = false;
					for (unsigned int word = 0; word < validWords.size(); word++)
					{
						if (validWords.at(word) == currWord)
							valid = true;
					}
					if (!valid)
						return true;
				}
				currWord = "";
			}

			//if it reaches the end of the line, check the current word aswell
			else if (currCol == this->numCols)
			{
				currWord += board.at(currLine).at(currCol);
				if (currWord.size() > 1)
				{
					bool valid = false;
					for (unsigned int word = 0; word < validWords.size(); word++)
					{
						if (validWords.at(word) == currWord)
							valid = true;
					}
					if (!valid)
						return true;
				}
				currWord = "";
			}

			//If none of the cases previously tested are done, then it's a letter so it's added to the current word
			else
				currWord += this->board.at(currLine).at(currCol);
		}
	}

	//same treatment, but for vertical words
	for (unsigned int currCol = 1; currCol <= this->numCols; currCol++)
	{
		std::string currWord = "";
		for (unsigned int currLine = 1; currLine <= this->numLines; currLine++)
		{
			if (this->board.at(currLine).at(currCol) == '#')
			{
				if (currWord.size() > 1)
				{
					bool valid = false;
					for (unsigned int word = 0; word < validWords.size(); word++)
					{
						if (validWords.at(word) == currWord)
							valid = true;
					}
					if (!valid)
						return true;
				}
				currWord = "";
			}

			else if (this->board.at(currLine).at(currCol) == '.')
			{
				if (currWord.size() > 1)
				{
					bool valid = false;
					for (unsigned int word = 0; word < validWords.size(); word++)
					{
						if (validWords.at(word) == currWord)
							valid = true;
					}
					if (!valid)
						return true;
				}
				currWord = "";
			}

			else if (currLine == this->numLines)
			{
				currWord += board.at(currLine).at(currCol);
				if (currWord.size() > 1)
				{
					bool valid = false;
					for (unsigned int word = 0; word < validWords.size(); word++)
					{
						if (validWords.at(word) == currWord)
							valid = true;
					}
					if (!valid)
						return true;
				}
				currWord = "";
			}

			else
				currWord += this->board.at(currLine).at(currCol);
		}
	}

	//if after going through all lines and columns no invalid words are found, there are no invalid words in board
	return false;
}

void Board::showBoard()
{
	for (unsigned int i = 0; i < this->numLines + 1; i++)
	{
		for (unsigned int j = 0; j < this->numCols + 1; j++)
		{
			if (i == 0 || j == 0)
				setcolor(CYAN, BLACK);
			else if (board.at(i).at(j) == '#')
				setcolor(RED, WHITE);
			else if (board.at(i).at(j) == '?')
				setcolor(LIGHTBLUE, WHITE);
			else
				setcolor(BLACK, WHITE);
			std::cout << std::setw(3) << this->board.at(i).at(j);
		}
		std::cout << std::endl;
	}
	setcolor(LIGHTGRAY, BLACK);
}

bool Board::isFinished()
{
	for (unsigned int i = 1; i < this->numLines; i++)
		for (unsigned int j = 1; j < this->numCols; j++)
			if (this->board.at(i).at(j) == '.' || this->board.at(i).at(j) == '?')
				return false;
	return true;
}

void Board::saveBoard(bool finish, std::string fileName, std::string dictName)
{
	std::ofstream outputStream;
	outputStream.open(fileName);

	outputStream << dictName << std::endl << std::endl;

	for (unsigned int line = 1; line <= this->numLines; line++)
	{
		for (unsigned int column = 1; column <= this->numCols; column++)
			if (finish && this->board.at(line).at(column) == '.')
				outputStream << '#' << " ";
			else
				outputStream << this->board.at(line).at(column) << " ";
		outputStream << std::endl;
	}

	outputStream << std::endl;

	for (unsigned int i = 0; i < this->words.size(); i++)
		outputStream << this->words.at(i).first << " " << this->words.at(i).second << std::endl;

	std::cout << "Board saved in file \"" << fileName << "\"." << std::endl;
}

std::pair<int, int> Board::getDimensions()
{
	return std::make_pair(this->numLines, this->numCols);
}

void Board::hideWords()
{
	for (unsigned int i = 1; i <= this->numLines; i++)
		for (unsigned int j = 1; j <= this->numCols; j++)
			if (this->board.at(i).at(j) != '#')
				this->board.at(i).at(j) = '?';
	this->words.clear();
}

bool Board::hasSameContents(Board board)
{
	for (unsigned int i = 1; i <= this->numLines; i++)
		for (unsigned int j = 1; j <= this->numCols; j++)
			if (this->board.at(i).at(j) != board.board.at(i).at(j))
				return false;

	return true;
}

void Board::removeGuess(Board originalBoard, std::string startTile)
{
	//resets board to original board but with words concealed
	for (unsigned int i = 1; i <= this->numLines; i++)
		for (unsigned int j = 1; j <= this->numCols; j++)
			if (originalBoard.board.at(i).at(j) == '#')
				this->board.at(i).at(j) = '#';
			else
				this->board.at(i).at(j) = '?';

	//finds the word at specified startTile in words vector and removes it from the vector
	for (unsigned int i = 0; i < this->words.size(); i++)
	{
		if (this->words.at(i).first == startTile)
		{
			this->words.erase(this->words.begin() + i);
			break;
		}
	}

	//adds any words left in vector back into board
	for (unsigned int i = 0; i < this->words.size(); i++)
	{
		insertWord(this->words.at(i).first, this->words.at(i).second, true);
	}
	return;
}

bool sortingCondition(std::pair<std::string, std::string> pairOne, std::pair<std::string, std::string> pairTwo)
{
	if (pairOne.first.at(2) < pairTwo.first.at(2))
		return true;
	else if (pairOne.first.at(2) > pairTwo.first.at(2))
		return false;
	else
		if (pairOne.first.at(2) == 'H')
		{
			if (pairOne.first.at(0) < pairTwo.first.at(0))
				return true;
			else if (pairOne.first.at(0) > pairTwo.first.at(0))
				return false;
			else if (pairOne.first.at(1) < pairTwo.first.at(1))
				return true;
			else
				return false;
		}
		else if (pairOne.first.at(2) == 'V')
		{
			if (pairOne.first.at(1) < pairTwo.first.at(1))
				return true;
			else if (pairOne.first.at(1) > pairTwo.first.at(1))
				return false;
			else if (pairOne.first.at(0) < pairTwo.first.at(0))
				return true;
			else
				return false;
		}
	return false;
}

std::vector<std::pair<std::string, std::string>> Board::wordSort()
{
	std::sort(this->words.begin(), this->words.end(), sortingCondition);
	return this->words;
}

std::string Board::wordFind(std::string startTile)
{
	std::string word = "nullword";
	int i = 0;
	while (word == "nullword")
	{
		if (this->words.at(i).first == startTile)
			word = words.at(i).second;
		i++;
	}
	return word;
}

void Board::showWrongWords(Board originalBoard)
{
	std::vector<std::string> boardStartTiles;
	std::vector<std::string> boardWords;
	std::vector<std::string> originalBoardStartTiles;
	std::vector<std::string> originalBoardWords;

	for (unsigned int i = 0; i < this->words.size(); i++)
	{
		boardStartTiles.push_back(this->words.at(i).first);
		boardWords.push_back(this->words.at(i).second);
	}

	for (unsigned int i = 0; i < originalBoard.words.size(); i++)
	{
		originalBoardStartTiles.push_back(originalBoard.words.at(i).first);
		originalBoardWords.push_back(originalBoard.words.at(i).second);
	}

	for (unsigned int i = 0; i < boardWords.size(); i++)
	{
		if (std::find(originalBoardWords.begin(), originalBoardWords.end(), boardWords.at(i)) == originalBoardWords.end())
			std::cout << boardStartTiles.at(i) << " - " << boardWords.at(i) << std::endl;
	}
}