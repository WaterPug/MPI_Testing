#ifndef GAME_OF_LIFE_UTILITIES
#define GAME_OF_LIFE_UTILITIES

#include "mpi.h"
#include <stdexcept>
#include <string>

enum cellState
{
	dead = 0,
	alive = 1
};

int isAlive(cellState cell)
{
	return cell == alive;
}

void MPI_CHECK(int statusCode, const std::string& failureMessage)
{
	if (statusCode != MPI_SUCCESS)
	{
		throw std::runtime_error("Failed MPI call : " + failureMessage);
	}
}

#endif