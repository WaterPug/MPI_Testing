#include "SerialImplementation.h"
#include "MPIImplementation.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
   // std::cout << "Serial solve -- " << serialSolve();

   std::cout << "MPI Serial solve -- " << MPI::MPIParallelSolve(argc, argv);

   return -1;
}
