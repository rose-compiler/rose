#include <stdlib.h>
#include <vector>
#include <string>

using namespace std;

#include "./core.h"
#include "./multicore.h"

template <typename T>
void relax2D_now ( MulticoreArray<T> & array, MulticoreArray<T> &old_array )
 {
   const int arraySizeX = array.get_arraySize(0);
   const int arraySizeY = array.get_arraySize(1);

#pragma stencil-manycore(array, old_array) D(i,0,arraySizeX) D(j,0,arraySizeY) 
#pragma omp for
        for (int j = 1; j < arraySizeY-1; j++)
           {
             for (int i = 1; i < arraySizeX-1; i++)
                {
                  old_array(i,j) = (array(i-1,j) + array(i+1,j) + array(i,j-1) + array(i,j+1))/4.0;
                }
           }
 }
