#include "Exo2.h"
#include "Exo3.h"
#include "Exo4.h"

#include <iostream>
#include <string>

#include <chrono>

int main(int argc, char* argv[])
{
   auto start = std::chrono::steady_clock::now();

   //std::cout << "Exo2 : Serial solve -- " << Exercice2::solve();
   //std::cout << "Exo 3: MPI Serial solve -- " << Exercice3::solve(argc, argv);
   //std::cout << "Exo 4: MPI 2 proc solve -- " << Exercice4::solve(argc, argv);

   auto end = std::chrono::steady_clock::now();
   std::chrono::duration<double> elapsedTime = end - start;
   std::cout << "\nElapsed time: " << elapsedTime.count() << "s\n";

   return -1;
}
