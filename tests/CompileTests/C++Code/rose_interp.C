
int max(int a,int b)
{
  return (a < b)?b:a;
}

#include <A++.h>
// #include "simpleA++.h"

#define CLOCK_TICKS_PER_SEC 1000000

#if 0
#define SIZE 501
int iterationLimit = 100;
#else
#define SIZE 1001
int iterationLimit = 100;
#endif

void interpolate1D(floatArray & fineGrid,floatArray & coarseGrid)
{
// int coarseGrid_length_1 = (((coarseGrid.Array_Descriptor).Array_Domain).DomainType::getLength(0));
  int coarseGrid_length_1 = (((coarseGrid.Array_Descriptor).Array_Domain).getLength(0));
  int coarseGrid_stride_1 = ((((coarseGrid.Array_Descriptor).Array_Domain).Stride)[0]
      );
  int coarseGrid_size_1 = ((((coarseGrid.Array_Descriptor).Array_Domain).Size)[0]);
  float * coarseGrid_pointer = (coarseGrid.floatArray::getDataPointer());
  int fineGrid_length_1 = (((fineGrid.Array_Descriptor).Array_Domain).getLength(0));
  int fineGrid_stride_1 = ((((fineGrid.Array_Descriptor).Array_Domain).Stride)[0]);
  int fineGrid_size_1 = ((((fineGrid.Array_Descriptor).Array_Domain).Size)[0]);
  float * fineGrid_pointer = (fineGrid.floatArray::getDataPointer());
  int _var_1;
  int _var_0;
  int fineGridSize = (fineGrid_length_1);
  int coarseGridSize = (coarseGrid_length_1);

     printf ("fineGridSize = %d coarseGridSize = %d \n",fineGridSize,coarseGridSize);

  // Interior fine points
     class Range If(2,fineGridSize - 2,2);
     class Range Ic(1,coarseGridSize - 1,1);

     long timeStart = 0,timeEnd = 0;

     printf ("Calling A++ Statement ... \n");

     timeStart = clock();
     for (int i = 0; i < iterationLimit; i++)
        {
       // separate loops to be fused
          fineGrid(If)    = coarseGrid(Ic);
          fineGrid(If-1)  = (coarseGrid(Ic-1) + coarseGrid(Ic)) / 2.0;
        }
     timeEnd = clock();
     double AppTime = (double (timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);

     printf ("Time for A++ statements = %f \n",AppTime);

     printf ("Calling unfused loops ... \n");

     timeStart = clock();
        for (int i = 0; i < iterationLimit; i++)
        {
       // separate loops to be fused
          for (_var_0 = 0; _var_0 <= -1 + (((fineGridSize - 2) + -1 * 2) + 1) / 2; _var_0 += 1)
             {
               fineGrid_pointer[(_var_0 * 2 + 2)]            = coarseGrid_pointer[(_var_0 * 1 + 1)];
             }
          for (_var_0 = 0; _var_0 <= -1 + (((fineGridSize - 2) + -1 * 2) + 1) / 2; _var_0 += 1)
             {
               fineGrid_pointer[((2 + -1 * 1) + _var_0 * 2)] = (coarseGrid_pointer[(_var_0 * 1)] + 
                                                                coarseGrid_pointer[(_var_0 * 1 + 1)]) / 2.0;
             }
        }
     timeEnd = clock();
     double unfusedLoopTime = (double(timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);
     printf ("Time for unfused loops = %f \n",unfusedLoopTime);

     double speedupFromAppLoopTransformation = AppTime / unfusedLoopTime;
     printf ("speedupFromA++LoopTransformation = %2.3f \n",speedupFromAppLoopTransformation);

     printf ("Calling fused for loops ... \n");

     timeStart = clock();
        for (int i = 0; i < iterationLimit; i++)
        {
       // separate loops to be fused
          for (_var_0 = 0; _var_0 <= -1 + (((fineGridSize - 2) + -1 * 2) + 1) / 2; _var_0 += 1)
             {
               fineGrid_pointer[(_var_0 * 2 + 2)]            = coarseGrid_pointer[(_var_0 * 1 + 1)];
               fineGrid_pointer[((2 + -1 * 1) + _var_0 * 2)] = (coarseGrid_pointer[(_var_0 * 1)] + 
                                                                coarseGrid_pointer[(_var_0 * 1 + 1)]) / 2.0;
             }
        }
     timeEnd = clock();
     double fusedLoopTime = (double(timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);
     printf ("Time for fused loops = %f \n",fusedLoopTime);

     double speedupFromFusion = unfusedLoopTime / fusedLoopTime;
     printf ("speedupFromFusion = %2.3f \n",speedupFromFusion);
   }

#if 1

void interpolate2D(floatArray & fineGrid,floatArray & coarseGrid)
{
  int coarseGrid_length_2 = (((coarseGrid.Array_Descriptor).Array_Domain).getLength(1));
  int coarseGrid_length_1 = (((coarseGrid.Array_Descriptor).Array_Domain).getLength(0));
  int coarseGrid_stride_2 = ((((coarseGrid.Array_Descriptor).Array_Domain).Stride)[1]
      );
  int coarseGrid_stride_1 = ((((coarseGrid.Array_Descriptor).Array_Domain).Stride)[0]
      );
  int coarseGrid_size_2 = ((((coarseGrid.Array_Descriptor).Array_Domain).Size)[1]);
  int coarseGrid_size_1 = ((((coarseGrid.Array_Descriptor).Array_Domain).Size)[0]);
  float * coarseGrid_pointer = (coarseGrid.floatArray::getDataPointer());
  int fineGrid_length_2 = (((fineGrid.Array_Descriptor).Array_Domain).getLength(1));
  int fineGrid_length_1 = (((fineGrid.Array_Descriptor).Array_Domain).getLength(0));
  int fineGrid_stride_2 = ((((fineGrid.Array_Descriptor).Array_Domain).Stride)[1]);
  int fineGrid_stride_1 = ((((fineGrid.Array_Descriptor).Array_Domain).Stride)[0]);
  int fineGrid_size_2 = ((((fineGrid.Array_Descriptor).Array_Domain).Size)[1]);
  int fineGrid_size_1 = ((((fineGrid.Array_Descriptor).Array_Domain).Size)[0]);
  float * fineGrid_pointer = (fineGrid.floatArray::getDataPointer());
  int _var_7;
  int _var_6;
  int _var_5;
  int _var_4;
  int _var_3;
  int _var_2;
  int _var_1;
  int _var_0;
  int fineGridSizeX = (fineGrid_length_1);
  int fineGridSizeY = (fineGrid_length_2);
  int coarseGridSizeX = (coarseGrid_length_1);
  int coarseGridSizeY = (coarseGrid_length_2);
// Interior fine grid points
  
  class Range If(2,fineGridSizeX - 2,2);
  class Range Jf(2,fineGridSizeY - 2,2);
  class Range Ic(1,coarseGridSizeX,1);
  class Range Jc(1,coarseGridSizeY - 1,1);

     long timeStart = 0,timeEnd = 0;

     printf ("Calling A++ Statement ... \n");

     timeStart = clock();
        for (int i = 0; i < iterationLimit; i++)
        {
       // separate loops to be fused
          fineGrid(If,Jf)     =  coarseGrid(Ic,Jc);
          fineGrid(If-1,Jf)   = (coarseGrid(Ic-1,Jc) + coarseGrid(Ic,Jc)) / 2.0;
          fineGrid(If,Jf-1)   = (coarseGrid(Ic,Jc-1) + coarseGrid(Ic,Jc)) / 2.0;
          fineGrid(If-1,Jf-1) = (coarseGrid(Ic-1,Jc-1) + coarseGrid(Ic-1,Jc) + 
                                 coarseGrid(Ic,Jc-1) + coarseGrid(Ic,Jc)) / 4.0;
        }
     timeEnd = clock();
     double AppTime = (double (timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);

     printf ("Time for A++ statements = %f \n",AppTime);

     printf ("Calling unfused loops ... \n");

     timeStart = clock();
        for (int i = 0; i < iterationLimit; i++)
        {
       // separate loops to be fused
  for (_var_1 = 0; _var_1 <= -1 + (((fineGridSizeY - 2) + -1 * 2) + 1) / 2; _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= -1 + (((fineGridSizeX - 2) + -1 * 2) + 1) / 2; _var_0 += 1) {
      fineGrid_pointer[((_var_0 * 2 + 2) + ((_var_1 * 2 + 2) * fineGrid_stride_1) * 
          fineGrid_size_1)] = coarseGrid_pointer[((_var_0 * 1 + 1) + ((_var_1 * 1 + 1) *
           coarseGrid_stride_1) * coarseGrid_size_1)];
    }
  }
  for (_var_1 = 0; _var_1 <= -1 + (((fineGridSizeY - 2) + -1 * 2) + 1) / 2; _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= -1 + (((fineGridSizeX - 2) + -1 * 2) + 1) / 2; _var_0 += 1) {
      fineGrid_pointer[(((2 + -1 * 1) + _var_0 * 2) + ((_var_1 * 2 + 2) * fineGrid_stride_1
          ) * fineGrid_size_1)] = (coarseGrid_pointer[(_var_0 * 1 + ((_var_1 * 1 + 1) *
           coarseGrid_stride_1) * coarseGrid_size_1)] + coarseGrid_pointer[((_var_0 
          * 1 + 1) + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * coarseGrid_size_1)]) / 2.0;
    }
  }
  for (_var_1 = 0; _var_1 <= -1 + (((fineGridSizeY - 2) + -1 * 2) + 1) / 2; _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= -1 + (((fineGridSizeX - 2) + -1 * 2) + 1) / 2; _var_0 += 1) {
      fineGrid_pointer[((_var_0 * 2 + 2) + (((2 + -1 * 1) + _var_1 * 2) * fineGrid_stride_1
          ) * fineGrid_size_1)] = (coarseGrid_pointer[((_var_0 * 1 + 1) + ((_var_1 * 1)
           * coarseGrid_stride_1) * coarseGrid_size_1)] + coarseGrid_pointer[((
          _var_0 * 1 + 1) + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * coarseGrid_size_1
          )]) / 2.0;
    }
  }
  for (_var_1 = 0; _var_1 <= -1 + (((fineGridSizeY - 2) + -1 * 2) + 1) / 2; _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= -1 + (((fineGridSizeX - 2) + -1 * 2) + 1) / 2; _var_0 += 1) {
      fineGrid_pointer[(((2 + -1 * 1) + _var_0 * 2) + (((2 + -1 * 1) + _var_1 * 2) * 
          fineGrid_stride_1) * fineGrid_size_1)] = (((coarseGrid_pointer[(_var_0 * 1 
          + ((_var_1 * 1) * coarseGrid_stride_1) * coarseGrid_size_1)] + 
          coarseGrid_pointer[(_var_0 * 1 + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * 
          coarseGrid_size_1)]) + coarseGrid_pointer[((_var_0 * 1 + 1) + ((_var_1 * 1) *
           coarseGrid_stride_1) * coarseGrid_size_1)]) + coarseGrid_pointer[((_var_0
           * 1 + 1) + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * coarseGrid_size_1)]) / 4.0
          ;
    }
  }
        }
     timeEnd = clock();
     double unfusedLoopTime = (double(timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);
     printf ("Time for unfused loops = %f \n",unfusedLoopTime);

     double speedupFromAppLoopTransformation = AppTime / unfusedLoopTime;
     printf ("speedupFromA++LoopTransformation = %2.3f \n",speedupFromAppLoopTransformation);

     printf ("Calling fused for loops ... \n");

     timeStart = clock();
        for (int i = 0; i < iterationLimit; i++)
        {
       // separate loops to be fused
  for (_var_1 = 0; _var_1 <= -1 + (((fineGridSizeY - 2) + -1 * 2) + 1) / 2; _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= -1 + (((fineGridSizeX - 2) + -1 * 2) + 1) / 2; _var_0 += 1) {
      fineGrid_pointer[((_var_0 * 2 + 2) + ((_var_1 * 2 + 2) * fineGrid_stride_1) * 
          fineGrid_size_1)] = coarseGrid_pointer[((_var_0 * 1 + 1) + ((_var_1 * 1 + 1) *
           coarseGrid_stride_1) * coarseGrid_size_1)];
      fineGrid_pointer[(((2 + -1 * 1) + _var_0 * 2) + ((_var_1 * 2 + 2) * fineGrid_stride_1
          ) * fineGrid_size_1)] = (coarseGrid_pointer[(_var_0 * 1 + ((_var_1 * 1 + 1) *
           coarseGrid_stride_1) * coarseGrid_size_1)] + coarseGrid_pointer[((_var_0 
          * 1 + 1) + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * coarseGrid_size_1)]) / 2.0;
      fineGrid_pointer[((_var_0 * 2 + 2) + (((2 + -1 * 1) + _var_1 * 2) * fineGrid_stride_1
          ) * fineGrid_size_1)] = (coarseGrid_pointer[((_var_0 * 1 + 1) + ((_var_1 * 1)
           * coarseGrid_stride_1) * coarseGrid_size_1)] + coarseGrid_pointer[((
          _var_0 * 1 + 1) + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * coarseGrid_size_1
          )]) / 2.0;
      fineGrid_pointer[(((2 + -1 * 1) + _var_0 * 2) + (((2 + -1 * 1) + _var_1 * 2) * 
          fineGrid_stride_1) * fineGrid_size_1)] = (((coarseGrid_pointer[(_var_0 * 1 
          + ((_var_1 * 1) * coarseGrid_stride_1) * coarseGrid_size_1)] + 
          coarseGrid_pointer[(_var_0 * 1 + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * 
          coarseGrid_size_1)]) + coarseGrid_pointer[((_var_0 * 1 + 1) + ((_var_1 * 1) *
           coarseGrid_stride_1) * coarseGrid_size_1)]) + coarseGrid_pointer[((_var_0
           * 1 + 1) + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * coarseGrid_size_1)]) / 4.0
          ;
    }
  }
        }
     timeEnd = clock();
     double fusedLoopTime = (double(timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);
     printf ("Time for fused loops = %f \n",fusedLoopTime);

     double speedupFromFusion = unfusedLoopTime / fusedLoopTime;
     printf ("speedupFromFusion = %2.3f \n",speedupFromFusion);

  for (_var_1 = 0; _var_1 <= -1 + (((fineGridSizeY - 2) + -1 * 2) + 1) / 2; _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= -1 + (((fineGridSizeX - 2) + -1 * 2) + 1) / 2; _var_0 += 1) {
      fineGrid_pointer[((_var_0 * 2 + 2) + ((_var_1 * 2 + 2) * fineGrid_stride_1) * 
          fineGrid_size_1)] = coarseGrid_pointer[((_var_0 * 1 + 1) + ((_var_1 * 1 + 1) *
           coarseGrid_stride_1) * coarseGrid_size_1)];
      fineGrid_pointer[(((2 + -1 * 1) + _var_0 * 2) + ((_var_1 * 2 + 2) * fineGrid_stride_1
          ) * fineGrid_size_1)] = (coarseGrid_pointer[(_var_0 * 1 + ((_var_1 * 1 + 1) *
           coarseGrid_stride_1) * coarseGrid_size_1)] + coarseGrid_pointer[((_var_0 
          * 1 + 1) + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * coarseGrid_size_1)]) / 2.0;
      fineGrid_pointer[((_var_0 * 2 + 2) + (((2 + -1 * 1) + _var_1 * 2) * fineGrid_stride_1
          ) * fineGrid_size_1)] = (coarseGrid_pointer[((_var_0 * 1 + 1) + ((_var_1 * 1)
           * coarseGrid_stride_1) * coarseGrid_size_1)] + coarseGrid_pointer[((
          _var_0 * 1 + 1) + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * coarseGrid_size_1
          )]) / 2.0;
      fineGrid_pointer[(((2 + -1 * 1) + _var_0 * 2) + (((2 + -1 * 1) + _var_1 * 2) * 
          fineGrid_stride_1) * fineGrid_size_1)] = (((coarseGrid_pointer[(_var_0 * 1 
          + ((_var_1 * 1) * coarseGrid_stride_1) * coarseGrid_size_1)] + 
          coarseGrid_pointer[(_var_0 * 1 + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * 
          coarseGrid_size_1)]) + coarseGrid_pointer[((_var_0 * 1 + 1) + ((_var_1 * 1) *
           coarseGrid_stride_1) * coarseGrid_size_1)]) + coarseGrid_pointer[((_var_0
           * 1 + 1) + ((_var_1 * 1 + 1) * coarseGrid_stride_1) * coarseGrid_size_1)]) / 4.0
          ;
    }
  }
}
#endif

#if 1
void redBlackRelax1D(floatArray & s,floatArray & rhs)
{
  int rhs_length_1 = (((rhs.Array_Descriptor).Array_Domain).getLength(0))
      ;
  int rhs_stride_1 = ((((rhs.Array_Descriptor).Array_Domain).Stride)[0]);
  int rhs_size_1 = ((((rhs.Array_Descriptor).Array_Domain).Size)[0]);
  float * rhs_pointer = (rhs.floatArray::getDataPointer());
  int s_length_1 = (((s.Array_Descriptor).Array_Domain).getLength(0));
  int s_stride_1 = ((((s.Array_Descriptor).Array_Domain).Stride)[0]);
  int s_size_1 = ((((s.Array_Descriptor).Array_Domain).Size)[0]);
  float * s_pointer = (s.floatArray::getDataPointer());
  int _var_1;
  int _var_0;
  int gridSize = (s_length_1);
  
  class Index Black_Odd(1,(gridSize - 1) / 2,2);  
  class Index Black_Even(2,(gridSize - 2) / 2,2);
  class Index Red_Odd(1,(gridSize - 1) / 2,2);
  class Index Red_Even(2,(gridSize - 2) / 2,2);
  class Index Odd_Rows(1,(gridSize - 1) / 2,2);  
  class Index Even_Rows(2,(gridSize - 2) / 2,2);



     long timeStart = 0,timeEnd = 0;

     double hSqr = 1.0 / (double )(gridSize * gridSize);
     printf ("Calling A++ Statement ... \n");

     timeStart = clock();
     for (int i = 0; i < iterationLimit; i++)
        {
       // separate loops to be fused
          s(Black_Even) = ( hSqr * rhs(Black_Even) + s(Black_Even+1) + s(Black_Even-1) ) / 2.0;
          s(Red_Odd)    = ( hSqr * rhs(Red_Odd)    + s(Red_Odd+1)    + s(Red_Odd-1)    ) / 2.0;
        }
     timeEnd = clock();
     double AppTime = (double (timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);

     printf ("Time for A++ statements = %f \n",AppTime);

     printf ("Calling unfused loops ... \n");

     timeStart = clock();
     for (int i = 0; i < iterationLimit; i++)
        {
       // separate loops to be fused
          for (_var_0 = 0; _var_0 <= max((-1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2),(-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2)); _var_0 += 1)
             {
               if (_var_0 <= -1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2)
                  {
                    s_pointer[(_var_0 * 2 + 2)] = ((hSqr * rhs_pointer[(_var_0 * 2 + 2)] + s_pointer[(
                              (2 + 1) + _var_0 * 2)]) + s_pointer[((2 + -1 * 1) + _var_0 * 2)]) / 2.0;
                  }
             }
          for (_var_0 = 0; _var_0 <= max((-1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2),(-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2)); _var_0 += 1)
             {
               if (_var_0 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2)
                  {
                    s_pointer[(_var_0 * 2 + 1)] = ((hSqr * rhs_pointer[(_var_0 * 2 + 1)] + s_pointer[(
                               _var_0 * 2 + 2 * 1)]) + s_pointer[(_var_0 * 2)]) / 2.0;
                  }
             }
        }
     timeEnd = clock();
     double unfusedLoopTime = (double(timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);
     printf ("Time for unfused loops = %f \n",unfusedLoopTime);

     double speedupFromAppLoopTransformation = AppTime / unfusedLoopTime;
     printf ("speedupFromA++LoopTransformation = %2.3f \n",speedupFromAppLoopTransformation);

     printf ("Calling fused for loops ... \n");

     timeStart = clock();
        for (int i = 0; i < iterationLimit; i++)
        {
       // separate loops to be fused
#if 1
// Removed conditionals in loops to simulate loop splitting
          for (_var_0 = 0; _var_0 <= max((-1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2),(-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2)); _var_0 += 1)
             {
               s_pointer[(_var_0 * 2 + 2)] = ((hSqr * rhs_pointer[(_var_0 * 2 + 2)] + s_pointer[(
                         (2 + 1) + _var_0 * 2)]) + s_pointer[((2 + -1 * 1) + _var_0 * 2)]) / 2.0;
               s_pointer[(_var_0 * 2 + 1)] = ((hSqr * rhs_pointer[(_var_0 * 2 + 1)] + s_pointer[(
                          _var_0 * 2 + 2 * 1)]) + s_pointer[(_var_0 * 2)]) / 2.0;
             }
#else
          for (_var_0 = 0; _var_0 <= max((-1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2),(-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2)); _var_0 += 1)
             {
               if (_var_0 <= -1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2)
                  {
                    s_pointer[(_var_0 * 2 + 2)] = ((hSqr * rhs_pointer[(_var_0 * 2 + 2)] + s_pointer[(
                              (2 + 1) + _var_0 * 2)]) + s_pointer[((2 + -1 * 1) + _var_0 * 2)]) / 2.0;
                  }
               if (_var_0 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2)
                  {
                    s_pointer[(_var_0 * 2 + 1)] = ((hSqr * rhs_pointer[(_var_0 * 2 + 1)] + s_pointer[(
                               _var_0 * 2 + 2 * 1)]) + s_pointer[(_var_0 * 2)]) / 2.0;
                  }
             }
#endif
        }
     timeEnd = clock();
     double fusedLoopTime = (double(timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);
     printf ("Time for fused loops = %f \n",fusedLoopTime);

     double speedupFromFusion = unfusedLoopTime / fusedLoopTime;
     printf ("speedupFromFusion = %2.3f \n",speedupFromFusion);
}
#endif

#if 1
void redBlackRelax2D(floatArray & s,floatArray & rhs)
{
  int rhs_length_2 = (((rhs.Array_Descriptor).Array_Domain).getLength(1))
      ;
  int rhs_length_1 = (((rhs.Array_Descriptor).Array_Domain).getLength(0))
      ;
  int rhs_stride_2 = ((((rhs.Array_Descriptor).Array_Domain).Stride)[1]);
  int rhs_stride_1 = ((((rhs.Array_Descriptor).Array_Domain).Stride)[0]);
  int rhs_size_2 = ((((rhs.Array_Descriptor).Array_Domain).Size)[1]);
  int rhs_size_1 = ((((rhs.Array_Descriptor).Array_Domain).Size)[0]);
  float * rhs_pointer = (rhs.floatArray::getDataPointer());
  int s_length_2 = (((s.Array_Descriptor).Array_Domain).getLength(1));
  int s_length_1 = (((s.Array_Descriptor).Array_Domain).getLength(0));
  int s_stride_2 = ((((s.Array_Descriptor).Array_Domain).Stride)[1]);
  int s_stride_1 = ((((s.Array_Descriptor).Array_Domain).Stride)[0]);
  int s_size_2 = ((((s.Array_Descriptor).Array_Domain).Size)[1]);
  int s_size_1 = ((((s.Array_Descriptor).Array_Domain).Size)[0]);
  float * s_pointer = (s.floatArray::getDataPointer());
  int _var_7;
  int _var_6;
  int _var_5;
  int _var_4;
  int _var_3;
  int _var_2;
  int _var_1;
  int _var_0;
  int gridSize = (s_length_1);

     printf ("gridSize = %d \n",gridSize);

     class Index Black_Odd(1,(gridSize - 1) / 2,2);
     class Index Black_Even(2,(gridSize - 2) / 2,2);
     class Index Red_Odd(1,(gridSize - 1) / 2,2);
     class Index Red_Even(2,(gridSize - 2) / 2,2);
     class Index Odd_Rows(1,(gridSize - 1) / 2,2);
     class Index Even_Rows(2,(gridSize - 2) / 2,2);

     double hSqr = 1.0 / (double )(gridSize * gridSize);

     long timeStart = 0,timeEnd = 0;

     printf ("Calling A++ Statement ... \n");

     timeStart = clock();
     for (int i = 0; i < iterationLimit; i++)
        {
          s(Black_Odd,Odd_Rows)   = ( hSqr * rhs(Black_Odd,Odd_Rows) + 
                                      s(Black_Odd+1,Odd_Rows) + s(Black_Odd-1,Odd_Rows) + 
                                      s(Black_Odd,Odd_Rows+1) + s(Black_Odd,Odd_Rows-1) ) / 4.0;
          s(Black_Even,Even_Rows) = ( hSqr * rhs(Black_Even,Even_Rows) + 
                                      s(Black_Even+1,Even_Rows) + s(Black_Even-1,Even_Rows) + 
                                      s(Black_Even,Even_Rows+1) + s(Black_Even,Even_Rows-1) ) / 4.0;
          s(Red_Even,Odd_Rows)    = ( hSqr * rhs(Red_Even,Odd_Rows) + 
                                      s(Red_Even+1,Odd_Rows) + s(Red_Even-1,Odd_Rows) + 
                                      s(Red_Even,Odd_Rows+1) + s(Red_Even,Odd_Rows-1) ) / 4.0;
          s(Red_Odd,Even_Rows)    = ( hSqr * rhs(Red_Odd,Even_Rows) + 
                                      s(Red_Odd+1,Even_Rows) + s(Red_Odd-1,Even_Rows) + 
                                      s(Red_Odd,Even_Rows+1) + s(Red_Odd,Even_Rows-1) ) / 4.0;
        }
     timeEnd = clock();
     double AppTime = (double (timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);

     printf ("Time for A++ statements = %f \n",AppTime);

#if 1
     printf ("Calling unfused loops ... \n");

     timeStart = clock();
        for (int i = 0; i < iterationLimit; i++)
        {
       // separate loops to be fused
  for (_var_1 = 0; _var_1 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize -
       2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize
         - 2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_0 += 1) {
      if (_var_0 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
        if (_var_1 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
          s_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 1) * s_stride_1) * s_size_1)] = (
              (((hSqr * rhs_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 1) * rhs_stride_1
              ) * rhs_size_1)] + s_pointer[((_var_0 * 2 + 2 * 1) + ((_var_1 * 2 + 1) * 
              s_stride_1) * s_size_1)]) + s_pointer[(_var_0 * 2 + ((_var_1 * 2 + 1) * 
              s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 2
               * 1) * s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2 + 1) + ((
              _var_1 * 2) * s_stride_1) * s_size_1)]) / 4.0;
        }
      }
    }
  }
  for (_var_1 = 0; _var_1 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize -
       2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize
         - 2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_0 += 1) {
      if (_var_0 <= (((gridSize - 2) / 2 + -1 * 2) + 1) / 2 + 1) {
        if (_var_1 <= -1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2) {
          if (_var_0 <= -1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2) {
            s_pointer[((_var_0 * 2 + 2) + ((_var_1 * 2 + 2) * s_stride_1) * s_size_1)] =
                 ((((hSqr * rhs_pointer[((_var_0 * 2 + 2) + ((_var_1 * 2 + 2) * 
                rhs_stride_1) * rhs_size_1)] + s_pointer[(((2 + 1) + _var_0 * 2) + ((
                _var_1 * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[(((2 + -1 * 1) + 
                _var_0 * 2) + ((_var_1 * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer
                [((_var_0 * 2 + 2) + (((2 + 1) + _var_1 * 2) * s_stride_1) * s_size_1)]) 
                + s_pointer[((_var_0 * 2 + 2) + (((2 + -1 * 1) + _var_1 * 2) * s_stride_1) 
                * s_size_1)]) / 4.0;
          }
        }
    }
  }
}
  for (_var_1 = 0; _var_1 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize -
       2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize
         - 2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_0 += 1) {
        if (_var_1 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
          if (_var_0 >= 2) {
            s_pointer[(((-2 + _var_0) * 2 + 2) + ((_var_1 * 2 + 1) * s_stride_1) * 
                s_size_1)] = ((((hSqr * rhs_pointer[(((-2 + _var_0) * 2 + 2) + ((_var_1 
                * 2 + 1) * rhs_stride_1) * rhs_size_1)] + s_pointer[(((2 + 1) + (-2 + 
                _var_0) * 2) + ((_var_1 * 2 + 1) * s_stride_1) * s_size_1)]) + 
                s_pointer[(((2 + -1 * 1) + (-2 + _var_0) * 2) + ((_var_1 * 2 + 1) * 
                s_stride_1) * s_size_1)]) + s_pointer[(((-2 + _var_0) * 2 + 2) + ((
                _var_1 * 2 + 2 * 1) * s_stride_1) * s_size_1)]) + s_pointer[(((-2 + 
                _var_0) * 2 + 2) + ((_var_1 * 2) * s_stride_1) * s_size_1)]) / 4.0;
          }
        }
      }
    }
  for (_var_1 = 0; _var_1 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize -
       2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize
         - 2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_0 += 1) {
      if (_var_0 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
        if (_var_1 >= 2 && _var_1 <= (((gridSize - 2) / 2 + -1 * 2) + 1) / 2 + 1) {
          s_pointer[((_var_0 * 2 + 1) + (((-2 + _var_1) * 2 + 2) * s_stride_1) * s_size_1)
              ] = ((((hSqr * rhs_pointer[((_var_0 * 2 + 1) + (((-2 + _var_1) * 2 + 2) * 
              rhs_stride_1) * rhs_size_1)] + s_pointer[((_var_0 * 2 + 2 * 1) + (((-2 + 
              _var_1) * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[(_var_0 * 2 + ((
              (-2 + _var_1) * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2
               + 1) + (((2 + 1) + (-2 + _var_1) * 2) * s_stride_1) * s_size_1)]) + 
              s_pointer[((_var_0 * 2 + 1) + (((2 + -1 * 1) + (-2 + _var_1) * 2) * s_stride_1)
               * s_size_1)]) / 4.0;
        }
      }
    }
  }
        }
     timeEnd = clock();
#endif
     double unfusedLoopTime = (double(timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);
     printf ("Time for unfused loops = %f \n",unfusedLoopTime);

     double speedupFromAppLoopTransformation = AppTime / unfusedLoopTime;
     printf ("speedupFromA++LoopTransformation = %2.3f \n",speedupFromAppLoopTransformation);

#if 1
     printf ("Calling fused for loops ... \n");

     timeStart = clock();
        for (int i = 0; i < iterationLimit; i++)
        {
       // separate loops to be fused
#if 1
// Removed conditionals in loops to simulate loop splitting
  for (_var_1 = 0; _var_1 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize -
       2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize
         - 2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_0 += 1) {
          s_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 1) * s_stride_1) * s_size_1)] = (
              (((hSqr * rhs_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 1) * rhs_stride_1
              ) * rhs_size_1)] + s_pointer[((_var_0 * 2 + 2 * 1) + ((_var_1 * 2 + 1) * 
              s_stride_1) * s_size_1)]) + s_pointer[(_var_0 * 2 + ((_var_1 * 2 + 1) * 
              s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 2
               * 1) * s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2 + 1) + ((
              _var_1 * 2) * s_stride_1) * s_size_1)]) / 4.0;
            s_pointer[((_var_0 * 2 + 2) + ((_var_1 * 2 + 2) * s_stride_1) * s_size_1)] =
                 ((((hSqr * rhs_pointer[((_var_0 * 2 + 2) + ((_var_1 * 2 + 2) * 
                rhs_stride_1) * rhs_size_1)] + s_pointer[(((2 + 1) + _var_0 * 2) + ((
                _var_1 * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[(((2 + -1 * 1) + 
                _var_0 * 2) + ((_var_1 * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer
                [((_var_0 * 2 + 2) + (((2 + 1) + _var_1 * 2) * s_stride_1) * s_size_1)]) 
                + s_pointer[((_var_0 * 2 + 2) + (((2 + -1 * 1) + _var_1 * 2) * s_stride_1) 
                * s_size_1)]) / 4.0;
            s_pointer[(((-2 + _var_0) * 2 + 2) + ((_var_1 * 2 + 1) * s_stride_1) * 
                s_size_1)] = ((((hSqr * rhs_pointer[(((-2 + _var_0) * 2 + 2) + ((_var_1 
                * 2 + 1) * rhs_stride_1) * rhs_size_1)] + s_pointer[(((2 + 1) + (-2 + 
                _var_0) * 2) + ((_var_1 * 2 + 1) * s_stride_1) * s_size_1)]) + 
                s_pointer[(((2 + -1 * 1) + (-2 + _var_0) * 2) + ((_var_1 * 2 + 1) * 
                s_stride_1) * s_size_1)]) + s_pointer[(((-2 + _var_0) * 2 + 2) + ((
                _var_1 * 2 + 2 * 1) * s_stride_1) * s_size_1)]) + s_pointer[(((-2 + 
                _var_0) * 2 + 2) + ((_var_1 * 2) * s_stride_1) * s_size_1)]) / 4.0;
          s_pointer[((_var_0 * 2 + 1) + (((-2 + _var_1) * 2 + 2) * s_stride_1) * s_size_1)
              ] = ((((hSqr * rhs_pointer[((_var_0 * 2 + 1) + (((-2 + _var_1) * 2 + 2) * 
              rhs_stride_1) * rhs_size_1)] + s_pointer[((_var_0 * 2 + 2 * 1) + (((-2 + 
              _var_1) * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[(_var_0 * 2 + ((
              (-2 + _var_1) * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2
               + 1) + (((2 + 1) + (-2 + _var_1) * 2) * s_stride_1) * s_size_1)]) + 
              s_pointer[((_var_0 * 2 + 1) + (((2 + -1 * 1) + (-2 + _var_1) * 2) * s_stride_1)
               * s_size_1)]) / 4.0;
    }
  }
#else
  for (_var_1 = 0; _var_1 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize -
       2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize
         - 2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_0 += 1) {
      if (_var_0 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
        if (_var_1 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
          s_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 1) * s_stride_1) * s_size_1)] = (
              (((hSqr * rhs_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 1) * rhs_stride_1
              ) * rhs_size_1)] + s_pointer[((_var_0 * 2 + 2 * 1) + ((_var_1 * 2 + 1) * 
              s_stride_1) * s_size_1)]) + s_pointer[(_var_0 * 2 + ((_var_1 * 2 + 1) * 
              s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 2
               * 1) * s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2 + 1) + ((
              _var_1 * 2) * s_stride_1) * s_size_1)]) / 4.0;
        }
      }
      if (_var_0 <= (((gridSize - 2) / 2 + -1 * 2) + 1) / 2 + 1) {
        if (_var_1 <= -1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2) {
          if (_var_0 <= -1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2) {
            s_pointer[((_var_0 * 2 + 2) + ((_var_1 * 2 + 2) * s_stride_1) * s_size_1)] =
                 ((((hSqr * rhs_pointer[((_var_0 * 2 + 2) + ((_var_1 * 2 + 2) * 
                rhs_stride_1) * rhs_size_1)] + s_pointer[(((2 + 1) + _var_0 * 2) + ((
                _var_1 * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[(((2 + -1 * 1) + 
                _var_0 * 2) + ((_var_1 * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer
                [((_var_0 * 2 + 2) + (((2 + 1) + _var_1 * 2) * s_stride_1) * s_size_1)]) 
                + s_pointer[((_var_0 * 2 + 2) + (((2 + -1 * 1) + _var_1 * 2) * s_stride_1) 
                * s_size_1)]) / 4.0;
          }
        }
        if (_var_1 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
          if (_var_0 >= 2) {
            s_pointer[(((-2 + _var_0) * 2 + 2) + ((_var_1 * 2 + 1) * s_stride_1) * 
                s_size_1)] = ((((hSqr * rhs_pointer[(((-2 + _var_0) * 2 + 2) + ((_var_1 
                * 2 + 1) * rhs_stride_1) * rhs_size_1)] + s_pointer[(((2 + 1) + (-2 + 
                _var_0) * 2) + ((_var_1 * 2 + 1) * s_stride_1) * s_size_1)]) + 
                s_pointer[(((2 + -1 * 1) + (-2 + _var_0) * 2) + ((_var_1 * 2 + 1) * 
                s_stride_1) * s_size_1)]) + s_pointer[(((-2 + _var_0) * 2 + 2) + ((
                _var_1 * 2 + 2 * 1) * s_stride_1) * s_size_1)]) + s_pointer[(((-2 + 
                _var_0) * 2 + 2) + ((_var_1 * 2) * s_stride_1) * s_size_1)]) / 4.0;
          }
        }
      }
      if (_var_0 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
        if (_var_1 >= 2 && _var_1 <= (((gridSize - 2) / 2 + -1 * 2) + 1) / 2 + 1) {
          s_pointer[((_var_0 * 2 + 1) + (((-2 + _var_1) * 2 + 2) * s_stride_1) * s_size_1)
              ] = ((((hSqr * rhs_pointer[((_var_0 * 2 + 1) + (((-2 + _var_1) * 2 + 2) * 
              rhs_stride_1) * rhs_size_1)] + s_pointer[((_var_0 * 2 + 2 * 1) + (((-2 + 
              _var_1) * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[(_var_0 * 2 + ((
              (-2 + _var_1) * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2
               + 1) + (((2 + 1) + (-2 + _var_1) * 2) * s_stride_1) * s_size_1)]) + 
              s_pointer[((_var_0 * 2 + 1) + (((2 + -1 * 1) + (-2 + _var_1) * 2) * s_stride_1)
               * s_size_1)]) / 4.0;
        }
      }
    }
  }
#endif
        }
     timeEnd = clock();
#endif

     double fusedLoopTime = (double(timeEnd - timeStart)) / double(CLOCK_TICKS_PER_SEC);
     printf ("Time for fused loops = %f \n",fusedLoopTime);

     double speedupFromFusion = unfusedLoopTime / fusedLoopTime;
     printf ("speedupFromFusion = %2.3f \n",speedupFromFusion);


#if 0
  for (_var_1 = 0; _var_1 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize -
       2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_1 += 1) {
    for (_var_0 = 0; _var_0 <= max((-1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2),((((gridSize
         - 2) / 2 + -1 * 2) + 1) / 2 + 1)); _var_0 += 1) {
      if (_var_0 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
        if (_var_1 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
          s_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 1) * s_stride_1) * s_size_1)] = (
              (((hSqr * rhs_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 1) * rhs_stride_1
              ) * rhs_size_1)] + s_pointer[((_var_0 * 2 + 2 * 1) + ((_var_1 * 2 + 1) * 
              s_stride_1) * s_size_1)]) + s_pointer[(_var_0 * 2 + ((_var_1 * 2 + 1) * 
              s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2 + 1) + ((_var_1 * 2 + 2
               * 1) * s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2 + 1) + ((
              _var_1 * 2) * s_stride_1) * s_size_1)]) / 4.0;
        }
      }
      if (_var_0 <= (((gridSize - 2) / 2 + -1 * 2) + 1) / 2 + 1) {
        if (_var_1 <= -1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2) {
          if (_var_0 <= -1 + (((gridSize - 2) / 2 + -1 * 2) + 1) / 2) {
            s_pointer[((_var_0 * 2 + 2) + ((_var_1 * 2 + 2) * s_stride_1) * s_size_1)] =
                 ((((hSqr * rhs_pointer[((_var_0 * 2 + 2) + ((_var_1 * 2 + 2) * 
                rhs_stride_1) * rhs_size_1)] + s_pointer[(((2 + 1) + _var_0 * 2) + ((
                _var_1 * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[(((2 + -1 * 1) + 
                _var_0 * 2) + ((_var_1 * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer
                [((_var_0 * 2 + 2) + (((2 + 1) + _var_1 * 2) * s_stride_1) * s_size_1)]) 
                + s_pointer[((_var_0 * 2 + 2) + (((2 + -1 * 1) + _var_1 * 2) * s_stride_1) 
                * s_size_1)]) / 4.0;
          }
        }
        if (_var_1 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
          if (_var_0 >= 2) {
            s_pointer[(((-2 + _var_0) * 2 + 2) + ((_var_1 * 2 + 1) * s_stride_1) * 
                s_size_1)] = ((((hSqr * rhs_pointer[(((-2 + _var_0) * 2 + 2) + ((_var_1 
                * 2 + 1) * rhs_stride_1) * rhs_size_1)] + s_pointer[(((2 + 1) + (-2 + 
                _var_0) * 2) + ((_var_1 * 2 + 1) * s_stride_1) * s_size_1)]) + 
                s_pointer[(((2 + -1 * 1) + (-2 + _var_0) * 2) + ((_var_1 * 2 + 1) * 
                s_stride_1) * s_size_1)]) + s_pointer[(((-2 + _var_0) * 2 + 2) + ((
                _var_1 * 2 + 2 * 1) * s_stride_1) * s_size_1)]) + s_pointer[(((-2 + 
                _var_0) * 2 + 2) + ((_var_1 * 2) * s_stride_1) * s_size_1)]) / 4.0;
          }
        }
      }
      if (_var_0 <= -1 + (((gridSize - 1) / 2 + -1 * 1) + 1) / 2) {
        if (_var_1 >= 2 && _var_1 <= (((gridSize - 2) / 2 + -1 * 2) + 1) / 2 + 1) {
          s_pointer[((_var_0 * 2 + 1) + (((-2 + _var_1) * 2 + 2) * s_stride_1) * s_size_1)
              ] = ((((hSqr * rhs_pointer[((_var_0 * 2 + 1) + (((-2 + _var_1) * 2 + 2) * 
              rhs_stride_1) * rhs_size_1)] + s_pointer[((_var_0 * 2 + 2 * 1) + (((-2 + 
              _var_1) * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[(_var_0 * 2 + ((
              (-2 + _var_1) * 2 + 2) * s_stride_1) * s_size_1)]) + s_pointer[((_var_0 * 2
               + 1) + (((2 + 1) + (-2 + _var_1) * 2) * s_stride_1) * s_size_1)]) + 
              s_pointer[((_var_0 * 2 + 1) + (((2 + -1 * 1) + (-2 + _var_1) * 2) * s_stride_1)
               * s_size_1)]) / 4.0;
        }
      }
    }
  }
#endif
}

#endif

int main()
{
#if 0
  int rhs_length_1;
  int rhs_stride_1;
  int rhs_size_1;
  float * rhs_pointer;
  int solution_length_1;
  int solution_stride_1;
  int solution_size_1;
  float * solution_pointer;
  int coarseGrid_length_1;
  int coarseGrid_stride_1;
  int coarseGrid_size_1;
  float * coarseGrid_pointer;
  int fineGrid_length_1;
  int fineGrid_stride_1;
  int fineGrid_size_1;
  float * fineGrid_pointer;
  int _var_11;
  int _var_10;
  int _var_9;
  int _var_8;
  int _var_7;
  int _var_6;
  int _var_5;
  int _var_4;
  int _var_3;
  int _var_2;
  int _var_1;
  int _var_0;
#endif

     int fineGridSize2D = SIZE;
     assert(fineGridSize2D % 2 == 1);
     int coarseGridSize2D = fineGridSize2D / 2 + 1;
  
     int fineGridSize1D = SIZE * SIZE;
     assert(fineGridSize1D % 2 == 1);
     int coarseGridSize1D = fineGridSize1D / 2 + 1;
  
     printf ("2D fineGridSize = %d coarseGridSize = %d \n",fineGridSize2D,coarseGridSize2D);   
     printf ("1D fineGridSize = %d coarseGridSize = %d \n",fineGridSize1D,coarseGridSize1D);

     class floatArray fineGrid1D(fineGridSize1D);
     class floatArray coarseGrid1D(coarseGridSize1D);

     printf ("Inialized data ... \n");

  // Initialization of grids
     fineGrid1D   = 0.0;
     coarseGrid1D = 1.0;

     printf ("Calling interpolate1D ... \n");

     interpolate1D(fineGrid1D,coarseGrid1D);

     class floatArray fineGrid2D(fineGridSize2D,fineGridSize2D);
     class floatArray coarseGrid2D(coarseGridSize2D,coarseGridSize2D);

     printf ("Inialized data ... \n");

  // Initialization of grids
     fineGrid2D   = 0.0;
     coarseGrid2D = 1.0;

     printf ("Calling interpolate2D ... \n");

     interpolate2D(fineGrid2D,coarseGrid2D);

#if 1
     class floatArray solution1D(fineGridSize1D);
     class floatArray rhs1D(fineGridSize1D);
     rhs1D      = 0.0;
     solution1D = 1.0;

     printf ("Calling redBlackRelax1D ... \n");

     redBlackRelax1D(solution1D,rhs1D);
#endif

#if 1
     class floatArray solution2D(fineGridSize2D,fineGridSize2D);
     class floatArray rhs2D(fineGridSize2D,fineGridSize2D);
     rhs2D      = 0.0;
     solution2D = 1.0;

     printf ("Calling redBlackRelax2D ... \n");

     redBlackRelax2D(solution2D,rhs2D);
#endif

  return 0;
}

















