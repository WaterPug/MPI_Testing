#ifndef GAME_OF_LIFE_EXO_3
#define GAME_OF_LIFE_EXO_3

#include "mpi.h"
#include "Exo2.h"
#include "Utilities.h"

namespace Exercice3
{
	// Exercice 3 : MPI serial solve
	std::string solve(int argc, char* argv[])
	{
		// Initialize MPI
		MPI_CHECK(MPI_Init(&argc, &argv), "failed to init");

		// Get process rank
		int procID = 0;
		MPI_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &procID), "failed to get comm rank");

		// Get total number of processes specificed at start of run
		int nproc = 0;
		MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &nproc), "failed to get comm size");

		std::string result = "Proc " + std::to_string(procID) + " -- solve : " + Exercice2::solve();

		MPI_Finalize();

		return result;
	}
}

#endif