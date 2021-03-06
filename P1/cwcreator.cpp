#include <iostream>
#include <string>
#include <fstream>

#include "Dict.h"
#include "Board.h"

int main();

inline bool fileNameCheck(const std::string& name) {
	//returns true if the file already exists
	std::ifstream f(name.c_str());
	return f.good();
}

std::string getFileName()
{
	std::string fileName = "b001.txt";
	while (fileNameCheck(fileName))
	{
		if (fileName.at(3) < '9')
			fileName.at(3)++;
		else
		{
			fileName.at(3) = '0';
			if (fileName.at(2) < '9')
				fileName.at(2)++;
			else
			{
				fileName.at(2) = '0';
				if (fileName.at(1) < '9')
					fileName.at(1)++;
				else
				{
					std::cout << "No file names are available, file cannot be saved. (HOW and WHY did you make 1000 boards?)" << std::endl;
				}
			}
		}
	}
	return fileName;
}

void receiveWords(Dict dictionary, Board &board)
{
	std::string startTile;
	std::string inputWord;
	board.showBoard();
	std::cout << "Position (\"LCD\" / CTRL-Z = stop)  ? ";

	while (!(std::cin >> startTile) && !(std::cin.eof()))
	{
		if (std::cin.eof())
			break;
		std::cin.clear();
		std::cin.ignore(9999, '\n');
		std::cout << "Invalid input. Try again: ";
	}

	if (std::cin.eof())
		return;

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
		board.removeWord(startTile);
		return;
	}

	if (inputWord == "?")
	{
		board.showHelp(startTile, dictionary.returnWords());
		return;
	}

	if (inputWord == "+")
	{
		board.addNewlyGeneratedWord(startTile, dictionary.returnWords());
		return;
	}

	else if (dictionary.isValid(inputWord))
	{
		board.insertWord(startTile, inputWord, false);
		return;
	}

	else std::cout << "Word not in dictionary." << std::endl;
}

void buildBoard(Board board, Dict dictionary, std::string saveFile)
{
	do
	{
		std::cin.clear();
		while (!(std::cin.eof()) && !board.isFinished())
		{
			receiveWords(dictionary, board);
		}

		if (board.checkForInvalidWords(dictionary.returnWords()))
		{
			std::cout << "Can't finish board: invalid words have been created." << std::endl;
		}
	} while (board.checkForInvalidWords(dictionary.returnWords()));

	if (board.isFinished())
	{
		std::cout << "Board is finished." << std::endl;
	}

	std::cout << "Press ENTER to continue...";
	std::cin.clear();
	std::cin.ignore(9999, '\n');

	std::cout << std::endl << "What would you like to do?" << std::endl;
	std::cout << "1. Save board and finish later;" << std::endl;
	std::cout << "2. Finish board now." << std::endl;
	std::cout << "0. Discard board;" << std::endl << std::endl;

	std::cout << "Option? ";

	int boardOption;
	while (!(std::cin >> boardOption) || boardOption < 0 || boardOption > 2)
	{
		std::cin.clear();
		std::cin.ignore(9999, '\n');
		std::cout << "Invalid input, try again: ";
	}

	switch (boardOption)
	{
	case 0: if (saveFile != "")
		remove(saveFile.c_str());
		std::cout << "Board discarded." << std::endl;
		break;
	case 1: if (saveFile == "")
		board.saveBoard(false, getFileName(), dictionary.originFile());
			else
				board.saveBoard(false, saveFile, dictionary.originFile());
		break;
	case 2: if (saveFile == "")
		board.saveBoard(true, getFileName(), dictionary.originFile());
			else
				board.saveBoard(true, saveFile, dictionary.originFile());
		break;
	}

}

void resumePuzzle()
{
	std::ifstream inputStream;
	std::string dictName;
	std::string boardName;

	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "RESUME PUZZLE" << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;

	std::cout << "Board file name ? ";
	while (!(std::cin >> boardName))
	{
		std::cin.clear();
		std::cin.ignore(9999, '\n');
		std::cout << "Invalid input, try again: ";
	}
	Board board(boardName);

	inputStream.open(boardName);
	getline(inputStream, dictName);
	Dict dictionary(dictName);

	std::cin.clear();
	std::cin.ignore(9999, '\n');

	buildBoard(board, dictionary, boardName);
}

void createPuzzle()
{
	std::ifstream inputStream;
	std::string dictName;
	int boardLines, boardCols;

	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "CREATE PUZZLE" << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;

	std::cout << "Dictionary file name ? ";
	std::cin >> dictName;
	Dict dictionary(dictName);

	std::cout << "Board size (lines columns) ? ";
	while (!(std::cin >> boardLines) || !(std::cin >> boardCols) || boardLines > 26 || boardCols > 26)
	{
		std::cin.clear();
		std::cin.ignore(9999, '\n');
		std::cout << "Invalid input. Try again: ";
	}

	std::cin.clear();
	std::cin.ignore(9999, '\n');

	Board board(boardLines, boardCols);
	buildBoard(board, dictionary, "");
}

void menu()
{
	int menuOption;

	do
	{
		std::cout << "CROSSWORDS PUZZLE CREATOR" << std::endl;
		std::cout << "==============================================" << std::endl << std::endl;
		std::cout << "INSTRUCTIONS:" << std::endl;
		std::cout << "Position ( LCD / CTRL-Z = stop )" << std::endl;
		std::cout << " LCD stands for Line Column and Direction" << std::endl;
		std::cout << std::endl << "--------------------------------------------------" << std::endl;

		std::cout << "OPTIONS:" << std::endl;
		std::cout << "1 - Create puzzle" << std::endl;
		std::cout << "2 - Resume puzzle" << std::endl;
		std::cout << "0 - Exit" << std::endl << std::endl;

		std::cout << "Option? ";

		while (!(std::cin >> menuOption) || menuOption < 0 || menuOption > 2)
		{
			std::cin.clear();
			std::cin.ignore(9999, '\n');
			std::cout << "Invalid input, try again: ";
		}

		std::cin.clear();
		std::cin.ignore(9999, '\n');

		std::cout << std::endl;
		switch (menuOption)
		{
		case 1: createPuzzle();
			break;
		case 2: resumePuzzle();
			break;
		case 0: std::cout << "Shutting down..." << std::endl;
			exit(0);
		}
	} while (menuOption != 0);
}

int main()
{
	menu();
}