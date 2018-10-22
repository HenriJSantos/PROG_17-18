#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "Player.h"

Player::Player(std::string playerName) 
{
	this->name = playerName;
	this->startTime = clock();
	this->numClues = 0;
}

void Player::incrementClueNumber()
{
	this->numClues++;
}

void Player::savePlayerPerformance(std::string boardName)
{
	//add the "_p" to the name
	boardName = boardName.substr(0, boardName.size() - 4);
	boardName += "_p.txt";

	std::ofstream outputStream;
	outputStream.open(boardName, std::fstream::app);

	double elapsedTime = ((double)clock() - this->startTime) / CLOCKS_PER_SEC;

	outputStream << "PLAYER NAME : " << std::setw(10) << this->name << " | TIME : " << std::setw(2) << elapsedTime << "s | HINTS : " << this->numClues << std::endl;
}