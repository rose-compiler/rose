#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<vector>
#include<string>
using namespace std;
#include "multicore.h" 

template <typename T>
void
high_level_relax ( MulticoreArray<T> & array, MulticoreArray<T> & old_array, int dist)
   {
     const int arraySizeX = array.get_arraySize(0);
     const int arraySizeY = array.get_arraySize(1);
     const int arraySizeZ = array.get_arraySize(2);

#pragma omp data_distribution(blocked:array[0:arraySizeX][0:arraySizeY][0:arraySizeZ],old_array[0:arraySizeX][0:arraySizeY][0:arraySizeZ]) halo(array[0:arraySizeX][0:arraySizeY][0:arraySizeZ],old_array[0:arraySizeX][0:arraySizeY][0:arraySizeZ]:<dist,dist>[periodic],<dist,dist>[periodic],<dist,dist>[periodic])
//#pragma omp parallel for
     for (int k = dist; k < arraySizeZ-dist; k++)
        {
          for (int j = dist; j < arraySizeY-dist; j++)
             {
               for (int i = dist; i < arraySizeX-dist; i++)
                  {
                 // 3D simple stencil
                 T tmp(0.0);
                 for(int d=1; d<=dist; d++)
                    tmp += (array(i-d,j,k) + array(i+d,j,k) + array(i,j-d,k) + array(i,j+d,k) + array(i,j,k-d) + array(i,j,k+d) );
                 old_array(i,j,k) = tmp / (6.0 * dist);
                  }
             }
        }
   }

int main(int argc, char *argv[])
   {
     int arraySize[3]              = {0,0,0};
     int numberOfCoresArraySize[3] = {0,0,0};
     int haloSize[3]               = {0,0,0};

     int interArraySubsectionPadding =  0; // Page size to avoid cache conflicts with memory allocted for use by each core

     haloSize[0] = 1;
     haloSize[1] = 1;
     haloSize[2] = 1;

     arraySize[0] = 64;
     arraySize[1] = 64;
     arraySize[2] = 64;

//     int numberOfCores =  4; // Number of cores to use (each axis)
     numberOfCoresArraySize[0] = 4;
     numberOfCoresArraySize[1] = 4;
     numberOfCoresArraySize[2] = 4;
  // Array constructor call
     MulticoreArray<double>::haloType type = MulticoreArray<double>::attached;
     printf ("Allocate array1 \n");
     MulticoreArray<double> array1    (arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],haloSize[0],haloSize[1],haloSize[2],type,interArraySubsectionPadding,false);
     printf ("Allocate array1 \n");
     MulticoreArray<double> old_array1(arraySize[0],arraySize[1],arraySize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],haloSize[0],haloSize[1],haloSize[2],type,interArraySubsectionPadding,false);
     high_level_relax<double>(array1, old_array1, 1);
     return 0;
   }

