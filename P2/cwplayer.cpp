#include <iostream>
#include <string>
#include <fstream>

#include "Dict.h"
#include "Board.h"
#include "Player.h"

//shows the initial and additional clues for each word along with its starting tile
void showHints(Board &board, Dict &dictionary, std::vector<std::vector<std::string>> &hintList)
{
	static std::vector<std::pair<std::string, std::string>> words = board.wordSort();
	if (hintList.empty())
	{
		std::vector<std::string> currentWord;
		for (unsigned int i = 0; i < words.size(); i++)
		{
			currentWord.push_back(words.at(i).first);
			currentWord.push_back(dictionary.obtainShuffledSynonyms(words.at(i).second).at(0));
			hintList.push_back(currentWord);
			currentWord.clear();
		}
	}
	int i = 0;
	if (hintList.at(0).at(0).at(2) == 'H')
	{
		std::cout << std::endl << "Horizontal:" << std::endl;
		do {
			std::cout << hintList.at(i).at(0).substr(0, 2) << " - ";
			
			for (unsigned int j = 1; j < (hintList.at(i).size() - 1); j++)
				std::cout << hintList.at(i).at(j) << ", ";
			std::cout << hintList.at(i).at(hintList.at(i).size() - 1) << std::endl;
			i++;
			if (i == hintList.size())
				return;
		} while (hintList.at(i).at(0).at(2) == 'H');
	}
	if (hintList.at(i).at(0).at(2) == 'V')
	{
		std::cout << std::endl << "Vertical:" << std::endl;
		do {
			std::cout << hintList.at(i).at(0).substr(0, 2) << " - ";

			for (unsigned int j = 1; j < (hintList.at(i).size() - 1); j++)
				std::cout << hintList.at(i).at(j) << ", ";
			std::cout << hintList.at(i).at(hintList.at(i).size() - 1) << std::endl;
			i++;
		} while (i < hintList.size());
	}
}

//acquires an additional clue for a chosen word if possible
void getAdditionalHint(Board &board, Dict &dictionary, std::string &startTile, std::vector<std::vector<std::string>> &hintList, Player &player)
{
	std::string actualWord = board.wordFind(startTile);
	std::vector<std::string> synonymList = dictionary.obtainShuffledSynonyms(actualWord);
	int hintPos = -1, i = 0;
	while (hintPos == -1)
	{
		if (hintList.at(i).at(0) == startTile)
			hintPos = i;
		i++;
	}
	for (unsigned int j = 0; j < synonymList.size(); j++)
	{
		if (std::find(hintList.at(hintPos).begin(), hintList.at(hintPos).end(), synonymList.at(j)) == hintList.at(hintPos).end())
		{
			hintList.at(hintPos).push_back(synonymList.at(j));
			player.incrementClueNumber();
			return;
		}
	}
	std::cout << std::endl << "There are no more clues available for this word." << std::endl;
}

void receiveWords(Dict dictionary, Board &board, Board originalBoard, Player &player)
{
	std::string startTile;
	std::string inputWord;
	static std::vector<std::vector<std::string>> hintList;
	board.showBoard();
	showHints(originalBoard, dictionary, hintList);
	std::cout << std::endl;
	std::cout << "Position (\"LCD\" / CTRL-Z = stop)  ? ";

	while (!(std::cin >> startTile))
	{
		std::cin.clear();
		std::cin.ignore(9999, '\n');
		std::cout << "Invalid input. Try again: ";
	}

	if (startTile.size() != 3)
	{
		std::cout << "Invalid input" << std::endl;
		return;
	}

	if (startTile.at(0) < 'A' ||
		startTile.at(0) > 'A' + board.getDimensions().first ||
		startTile.at(1) < 'a' ||
		startTile.at(1) > 'a' + board.getDimensions().second)
	{
		std::cout << "Position out of range" << std::endl;
		return;
	}
	if (startTile.at(2) != 'H' && startTile.at(2) != 'V')
	{
		std::cout << "Invalid direction" << std::endl;
		return;
	}

	std::cout << "word ( - = remove / ?  = help ) ..  ? ";
	while (!(std::cin >> inputWord))
	{
		std::cin.clear();
		std::cin.ignore(9999, '\n');
		std::cout << "Invalid input. Try again: ";
	}

	if (inputWord == "-")
	{
		board.removeGuess(originalBoard, startTile);
		return;
	}

	if (inputWord == "?")
	{
		getAdditionalHint(originalBoard, dictionary, startTile, hintList, player);
		return;
	}

	else if (dictionary.isValid(inputWord))
	{
		board.insertGuess(startTile, inputWord, false, originalBoard);
		return;
	}

	else std::cout << "Word not in dictionary." << std::endl;
}

void menu()
{
	char goBack = 'Y';
	do
	{
		std::ifstream inputStream;
		std::string dictName;
		std::string boardName;
		std::string playerName;

		std::cout << "CROSSWORDS PUZZLE PLAYER" << std::endl;
		std::cout << "==============================================" << std::endl << std::endl;
		std::cout << "INSTRUCTIONS:" << std::endl;
		std::cout << "Position ( LCD )" << std::endl;
		std::cout << " LCD stands for Line Column and Direction" << std::endl;
		std::cout << std::endl << "--------------------------------------------------" << std::endl << std::endl;

		std::cout << "Board file name ? ";
		while (!(std::cin >> boardName))
		{
			std::cin.clear();
			std::cin.ignore(9999, '\n');
			std::cout << "Invalid input, try again: ";
		}

		//open board file and create board object
		Board board(boardName);
		//create counterpart "hidden board" that doesn't reveal the words
		Board hiddenBoard = board;
		hiddenBoard.hideWords();

		inputStream.open(boardName);
		getline(inputStream, dictName);
		Dict dictionary(dictName);

		std::cout << "Player name ? ";
		while (!(std::cin >> playerName))
		{
			std::cin.clear();
			std::cin.ignore(9999, '\n');
			std::cout << "Invalid input, try again: ";
		}
		std::cin.clear();
		std::cin.ignore(9999, '\n');

		Player player(playerName);

		while (!hiddenBoard.hasSameContents(board))
		{
			do
			{
				receiveWords(dictionary, hiddenBoard, board, player);
			} while (!hiddenBoard.isFinished());

			if (hiddenBoard.isFinished() && !hiddenBoard.hasSameContents(board))
			{
				std::cout << "The following words are wrong:" << std::endl;
				hiddenBoard.showWrongWords(board);
				std::cout << std::endl;
			}
		}
		std::cout << "Congratulations! You won!" << std::endl;
		std::cout << "Saving performance...";
		player.savePlayerPerformance(boardName);
		std::cout << "DONE!" << std::endl;
		std::cout << "Back to menu ( Y / N ) ?";
		while (!(std::cin >> goBack) && goBack != 'Y' && goBack != 'N')
		{
			std::cin.clear();
			std::cin.ignore(9999, '\n');
			std::cout << "Invalid input, try again: ";
		}
	} while (goBack == 'Y');
}

int main()
{
	menu();
}