#ifndef GAME_OF_LIFE_SERIAL_IMPLEMENTAION
#define GAME_OF_LIFE_SERIAL_IMPLEMENTAION

#include <array>
#include <cstddef>
#include <random>
#include <string>
#include <memory>

namespace Serial
{
   constexpr int width = 500;
   constexpr int height = 500;
   constexpr int widthWithGhost = width + 2;
   constexpr int heightWithGhost = height + 2;
   constexpr int numberOfLoops = 1000;

   enum cellState
   {
      dead = 0,
      alive = 1
   };

   int isAlive(cellState cell)
   {
      return cell == alive;
   }

   std::string serialSolve()
   {
      std::random_device rd;
      std::default_random_engine e1(rd());
      std::uniform_int_distribution<int> uniform_dist(0, 1);

      std::unique_ptr<std::array<std::array<cellState, widthWithGhost>, heightWithGhost>> dataArrayOld;
      std::unique_ptr<std::array<std::array<cellState, widthWithGhost>, heightWithGhost>> dataArrayNew;

      dataArrayOld = std::make_unique<std::array<std::array<cellState, widthWithGhost>, heightWithGhost>>();
      dataArrayNew = std::make_unique<std::array<std::array<cellState, widthWithGhost>, heightWithGhost>>();

      for (auto& row : *dataArrayOld.get())
      {
         for (auto& element : row)
         {
            element = static_cast<cellState>(uniform_dist(e1));
         }
      }

      for (int loop = 0; loop < numberOfLoops; ++loop)
      {
         // Do corners 
         (*dataArrayOld)[0][0] = (*dataArrayOld)[height][width];
         (*dataArrayOld)[0][width + 1] = (*dataArrayOld)[height][1];
         (*dataArrayOld)[height + 1][width + 1] = (*dataArrayOld)[1][1];
         (*dataArrayOld)[height + 1][0] = (*dataArrayOld)[1][width];

         for (int i = 1; i < height; ++i)
         {
            (*dataArrayOld)[i][0] = (*dataArrayOld)[i][width];
            (*dataArrayOld)[i][width + 1] = (*dataArrayOld)[i][1];
         }

         for (int j = 1; j < width; ++j)
         {
            (*dataArrayOld)[0][j] = (*dataArrayOld)[height][j];
            (*dataArrayOld)[height + 1][j] = (*dataArrayOld)[1][j];
         }

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
      for (auto& row : *dataArrayNew)
      {
         for (auto& element : row)
         {
            sum += isAlive(element);
         }
      }

      return "Sum = " + std::to_string(sum);
   }
}
#endif