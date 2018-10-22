#include <iostream>
#include <string>
#include <vector>

#ifndef BOARD_H
#define BOARD_H

class Board
{
public:
	//constructor with board width and length (new board)
	Board(unsigned int l, unsigned int c);
	//constructor from existing board file
	Board(std::string &boardName);
	//insert inputWord at startTile (repeat is used in the remove word function)
	void insertWord(std::string startTile, std::string inputWord, bool repeat);
	//same as inputWord but for the crossword player
	void insertGuess(std::string startTile, std::string inputWord, bool repeat, Board originalBoard);
	//remove word that begins at startTile;
	void removeWord(std::string startTile);
	//adds a word that has been generated from the adjacency of other words
	void addNewlyGeneratedWord(std::string startTile, std::vector<std::string> validWords);
	//shows list of words from words vector that can fit starting from startTile
	void showHelp(std::string startTile, std::vector<std::string> words);
	//checks if any invalid words have been formed during the creation of the board
	bool checkForInvalidWords(std::vector<std::string> validWords);
	//displays board contents
	void showBoard();
	//checks if every tile in board is either a letter or a black space
	bool isFinished();
	//saves board (switches '.' with '#' if "finish" = true) at file called fileName;
	void saveBoard(bool finish, std::string fileName, std::string dictName);
	//returns board dimensions
	std::pair<int, int> getDimensions();
	//hides words in board
	void hideWords();
	//compares two boards to see if they're identical
	bool hasSameContents(Board board);
	//removes a guess from the hidden word board
	void removeGuess(Board originalBoard, std::string startTile);
	//sorts the words vector and returns it
	std::vector<std::pair<std::string, std::string>> wordSort();
	//returns the word which starts at the indicated tile
	std::string wordFind(std::string startTile);
	//displays the words that are not the same as original board
	void showWrongWords(Board originalBoard);
private:
	unsigned int numLines, numCols;
	std::vector<std::vector<char>> board;
	std::vector<std::pair<std::string, std::string>> words;
};

#endif