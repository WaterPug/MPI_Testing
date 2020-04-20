#ifndef GAME_OF_LIFE_EXO_5
#define GAME_OF_LIFE_EXO_5

#include "mpi.h"
#include "Constants.h"
#include "Utilities.h"

#include <iostream>
#include <string>

#include <array>
#include <cstddef>
#include <random>
#include <memory>

namespace Exercice5
{
	// Exercice 5 : Custom data types -- 1/2 proc only
	// Simply make MPI_Datatype
	std::string solve(int argc, char* argv[])
	{
		MPI_Status status;
		// Initialize MPI
		MPI_CHECK(MPI_Init(&argc, &argv), "failed to init");

		// Get process rank
		int procID;
		MPI_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &procID), "failed to get comm rank");

		// Get total number of processes specificed at start of run
		int nproc;
		MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &nproc), "failed to get comm size");

		if (nproc > 2)
		{
			throw std::runtime_error("invalid number of processes");
		}

		std::random_device rd;
		std::default_random_engine e1(rd());
		std::uniform_int_distribution<int> uniform_dist(0, 1);

		std::unique_ptr<std::array<std::array<cellState, widthWithGhost>, heightWithGhost>> dataArrayOld;
		std::unique_ptr<std::array<std::array<cellState, widthWithGhost>, heightWithGhost>> dataArrayNew;

		dataArrayOld = std::make_unique<std::array<std::array<cellState, widthWithGhost>, heightWithGhost>>();
		dataArrayNew = std::make_unique<std::array<std::array<cellState, widthWithGhost>, heightWithGhost>>();

		// Need to know start Height and total Height to process
		// To do so : calc previous and next height with ceil
		int heightPerSlice = height / nproc;
		int startHeight = (heightPerSlice * procID) + 1;
		int endHeight = startHeight + heightPerSlice + 1;
		if (procID == (nproc - 1))
		{
			endHeight = height;
		}

		int previousProcID = procID - 1;
		if (previousProcID < 0)
		{
			previousProcID = nproc - 1;
		}
		int nextProcID = procID + 1;
		if (nextProcID == nproc)
		{
			nextProcID = 0;
		}

		// Only update your part of the array... Alternative = only host does this
		for (int i = startHeight; i < endHeight; ++i)
		{
			for (int j = 1; j < width; ++j)
			{
				(*dataArrayOld)[i][j] = static_cast<cellState>(uniform_dist(e1));
			}
		}

		MPI_Datatype rowData;
		MPI_Type_vector(width, 1, 1, MPI_C_BOOL, &rowData);
		MPI_Type_commit(&rowData);

		for (int loop = 0; loop < numberOfLoops; ++loop)
		{
			// 1.1 Local -- Update left and right ghost cells
			for (int i = startHeight; i < endHeight; ++i)
			{
				(*dataArrayOld)[i][0] = (*dataArrayOld)[i][width];
				(*dataArrayOld)[i][width + 1] = (*dataArrayOld)[i][1];
			}

			// 1.2 Remote -- Corners (only ones that matter are 0 and last)
			// 1.3 Remote -- Rows on top and bottom
			if (nproc == 1)
			{
				// 1.2
				(*dataArrayOld)[0][0] = (*dataArrayOld)[height][width];
				(*dataArrayOld)[0][width + 1] = (*dataArrayOld)[height][1];
				(*dataArrayOld)[height + 1][width + 1] = (*dataArrayOld)[1][1];
				(*dataArrayOld)[height + 1][0] = (*dataArrayOld)[1][width];

				// 1.3
				for (int j = 1; j < width; ++j)
				{
					(*dataArrayOld)[0][j] = (*dataArrayOld)[height][j];
					(*dataArrayOld)[height + 1][j] = (*dataArrayOld)[1][j];
				}
			}
			else
			{
				if (procID == 0)
				{
					// 1.2
					MPI_Send(&(*dataArrayOld)[1][1], 1, MPI_C_BOOL, previousProcID, 0, MPI_COMM_WORLD);
					MPI_Send(&(*dataArrayOld)[1][width], 1, MPI_C_BOOL, previousProcID, 1, MPI_COMM_WORLD);

					MPI_Recv(&(*dataArrayOld)[0][0], 1, MPI_C_BOOL, previousProcID, 2, MPI_COMM_WORLD, &status);
					MPI_Recv(&(*dataArrayOld)[0][width + 1], 1, MPI_C_BOOL, previousProcID, 3, MPI_COMM_WORLD, &status);

					// 1.3
					MPI_Send(&((*dataArrayOld)[1][1]), 1, rowData, previousProcID, 4, MPI_COMM_WORLD);
					MPI_Send(&((*dataArrayOld)[endHeight][1]), 1, rowData, previousProcID, 5, MPI_COMM_WORLD);

					MPI_Recv(&((*dataArrayOld)[0][1]), 1, rowData, previousProcID, 6, MPI_COMM_WORLD, &status);
					MPI_Recv(&((*dataArrayOld)[endHeight + 1][1]), 1, rowData, previousProcID, 7, MPI_COMM_WORLD, &status);
				}
				else if (procID == (nproc - 1))
				{
					// 1.2
					MPI_Recv(&(*dataArrayOld)[height + 1][width + 1], 1, MPI_C_BOOL, nextProcID, 0, MPI_COMM_WORLD, &status);
					MPI_Recv(&(*dataArrayOld)[height + 1][0], 1, MPI_C_BOOL, nextProcID, 1, MPI_COMM_WORLD, &status);

					MPI_Send(&(*dataArrayOld)[height][width], 1, MPI_C_BOOL, nextProcID, 2, MPI_COMM_WORLD);
					MPI_Send(&(*dataArrayOld)[height][1], 1, MPI_C_BOOL, nextProcID, 3, MPI_COMM_WORLD);

					// 1.3
					MPI_Recv(&(*dataArrayOld)[startHeight][1], 1, rowData, previousProcID, 4, MPI_COMM_WORLD, &status);
					MPI_Recv(&(*dataArrayOld)[endHeight][1], 1, rowData, previousProcID, 5, MPI_COMM_WORLD, &status);

					MPI_Send(&(*dataArrayOld)[startHeight - 1][1], 1, rowData, previousProcID, 6, MPI_COMM_WORLD);
					MPI_Send(&(*dataArrayOld)[endHeight + 1][1], 1, rowData, previousProcID, 7, MPI_COMM_WORLD);
				}
			}

			// 1.4 Local solve
			auto neighbourAliveCellsCount = [&](int column, int row)->int
			{
				return isAlive((*dataArrayOld)[column + 1][row]) +
					isAlive((*dataArrayOld)[column - 1][row]) +
					isAlive((*dataArrayOld)[column][row - 1]) +
					isAlive((*dataArrayOld)[column][row + 1]);
			};

			for (int i = 1; i < height; ++i)
			{
				for (int j = 1; j < width; ++j)
				{
					switch (neighbourAliveCellsCount(i, j))
					{
					case 2:  (*dataArrayNew)[i][j] = (*dataArrayOld)[i][j]; break;
					case 3:  (*dataArrayNew)[i][j] = alive; break;
					default: (*dataArrayNew)[i][j] = dead; break;
					}
				}
			}

			*dataArrayOld = *dataArrayNew;
		}

		int sum = 0;
		for (int i = startHeight; i < endHeight; ++i)
		{
			for (int j = 1; j < width; ++j)
			{
				sum += isAlive((*dataArrayOld)[i][j]);
			}
		}
		std::cout << "Proc #" << std::to_string(procID) << " local sum : " << std::to_string(sum) << "\n";

		int nextSum = 0;
		int totalSum = 0;
		std::string returnMessage = "";
		if (nproc == 1)
		{
			returnMessage = "Parallel job with only 1 proc -- Total Sum = " + std::to_string(sum);
		}
		else
		{
			if (procID == 0)
			{
				MPI_Recv(&nextSum, 1, MPI_INT, nextProcID, 8, MPI_COMM_WORLD, &status);
				totalSum = sum + nextSum;
				returnMessage = "Proc 0 -- Total Sum = " + std::to_string(totalSum);
			}
			else
			{
				MPI_Send(&sum, 1, MPI_INT, previousProcID, 8, MPI_COMM_WORLD);
				returnMessage = "Worker proc";
			}
		}

		MPI_Finalize();
		return returnMessage;
	}
}

#endif