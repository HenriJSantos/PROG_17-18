#include <time.h>
#include <string>

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
public:
	//creates an object of class Player and starts the timer
	Player(std::string playerName);
	//increments the number of additional clues used by one
	void incrementClueNumber();
	//calculates the time consumed by the player to do the puzzle and saves the player's statistics to a file
	void savePlayerPerformance(std::string boardName);
private:
	std::string name;
	unsigned long startTime;
	unsigned int numClues = 0;
};
#endif