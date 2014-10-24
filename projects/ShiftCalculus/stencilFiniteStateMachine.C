// Implementation of finte state machine (see comments in associated header file).

#include "rose.h"

#include "stencilFiniteStateMachine.h"

using namespace std;


// **************************************************
// Stencil Offset Finite State Machine Implementation
// **************************************************

StencilOffsetFSM::StencilOffsetFSM(int x, int y, int z)
   {
     offsetValues[0] = x;
     offsetValues[1] = y;
     offsetValues[2] = z;
   }

StencilOffsetFSM StencilOffsetFSM::operator*=(int n)
   {
     offsetValues[0] *= n;
     offsetValues[1] *= n;
     offsetValues[2] *= n;

     return *this;
   }

void StencilOffsetFSM::display(const string & label)
   {
     printf ("In StencilOffsetFSM::display(): label = %s \n",label.c_str());
     printf ("   --- offsetValues[0] = %d [1] = %d [2] = %d \n",offsetValues[0],offsetValues[1],offsetValues[2]);
   }







// *******************************************
// Stencil Finite State Machine Implementation
// *******************************************

StencilFSM::StencilFSM()
   {
  // Nothing to do here.
#if 0
     printf ("In StencilFSM default constructor \n");
#endif
   }

StencilFSM::StencilFSM(StencilOffsetFSM X, double stencilCoeficient)
   {
  // This is the simplest stencil (a single point).

#if 0
      printf ("In StencilFSM constructor: StencilFSM(StencilOffsetFSM x, double stencilCoeficient)tencilFSM(StencilOffsetFSM x, double stencilCoeficient) \n");
      printf ("   --- X.offsetValues[0] = %3d \n",X.offsetValues[0]);
      printf ("   --- X.offsetValues[1] = %3d \n",X.offsetValues[1]);
      printf ("   --- X.offsetValues[2] = %3d \n",X.offsetValues[2]);
      printf ("   --- stencilCoeficient = %3.4f \n",stencilCoeficient);
#endif

  // vector<pair<Offset,double> > stencilPointList;
     stencilPointList.push_back(pair<StencilOffsetFSM,double>(X,stencilCoeficient));
   }


StencilFSM StencilFSM::operator+(const StencilFSM & X)
   {
  // Union operator
  // The semantics of this operator match that of the Stencil class used to specify the stencil.

#if 0
      printf ("In StencilFSM::operator+(): X.stencilPointList.size() = %zu \n",X.stencilPointList.size());
#endif

     for (size_t i = 0; i < X.stencilPointList.size(); i++)
        {
#if 0
          printf ("In StencilFSM::operator+(): In Loop: stencilPointList.size() = %zu \n",stencilPointList.size());
          printf ("In StencilFSM::operator+(): In Loop: X.stencilPointList.size() = %zu \n",X.stencilPointList.size());
#endif
          stencilPointList.push_back(X.stencilPointList[i]);
#if 0
          printf ("In StencilFSM::operator+(): In Loop: stencilPointList.size() = %zu \n",stencilPointList.size());
#endif
        }

#if 0
      printf ("Leaving StencilFSM::operator+(): stencilPointList.size() = %zu \n",stencilPointList.size());
#endif

     return *this;
   }

StencilFSM StencilFSM::operator*(const StencilFSM & X)
   {
  // Convolution operator

     printf ("stencil convolution not defined yet! \n");
     ROSE_ASSERT(false);

     return *this;
   }

StencilFSM StencilFSM::operator=(const StencilFSM & X)
   {
  // Assignment operator (operator=).
#if 0
     printf ("In StencilFSM operator= \n");
#endif

     stencilPointList = X.stencilPointList;

#if 0
     printf ("Leaving StencilFSM operator= \n");
#endif

     return *this;
   }

StencilFSM::StencilFSM (const StencilFSM & X)
   {
  // Copy constructor (defined using the assignment operator).
#if 0
     printf ("In StencilFSM copy constructor \n");
#endif
  // this->operator=(X);
     stencilPointList = X.stencilPointList;
   }


void StencilFSM::display(const string & label)
   {
     printf ("In StencilFSM::display(): label = %s \n",label.c_str());
     printf ("   --- stencilDimension = %d \n",stencilDimension());
     printf ("   --- stencilWidth     = %d \n",stencilWidth());
     printf ("   --- stencilPointList: \n");
     for (size_t i = 0; i < stencilPointList.size(); i++)
        {
          StencilOffsetFSM stencilOffsetFSM  = stencilPointList[i].first;
          double           stencilCoeficient = stencilPointList[i].second;

          printf ("      --- stencilOffsetFSM.offsetValues[0] = %3d [1] = %3d [2] = %3d stencilCoeficient = %4.2f \n",stencilOffsetFSM.offsetValues[0],stencilOffsetFSM.offsetValues[1],stencilOffsetFSM.offsetValues[2],stencilCoeficient);
        }

   }


// Access functions: Reports the width of the stencil (edge to edge).
int StencilFSM::stencilWidth()
   {
     int lowerBound = 0;
     int upperBound = 0;
     for (size_t i = 0; i < stencilPointList.size(); i++)
        {
          StencilOffsetFSM stencilOffsetFSM  = stencilPointList[i].first;

          for (size_t j = 0; j < 3; j++)
             {
               lowerBound = stencilOffsetFSM.offsetValues[j] < lowerBound ? stencilOffsetFSM.offsetValues[j] : lowerBound;
               upperBound = stencilOffsetFSM.offsetValues[j] > upperBound ? stencilOffsetFSM.offsetValues[j] : upperBound;
             }
        }

     int stencilWidthValue = (upperBound - lowerBound) + 1;

     return stencilWidthValue;
   }


// Access functions: Reports the dimensionality of the stencil.
int StencilFSM::stencilDimension()
   {
     int stencilDimensionValue = 0;

  // Count the number of dimensions that have a non-zero stencil width.
     for (int j = 0; j < 3; j++)
        {
          int lowerBound = 0;
          int upperBound = 0;

          for (size_t i = 0; i < stencilPointList.size(); i++)
             {
               StencilOffsetFSM stencilOffsetFSM  = stencilPointList[i].first;

               lowerBound = stencilOffsetFSM.offsetValues[j] < lowerBound ? stencilOffsetFSM.offsetValues[j] : lowerBound;
               upperBound = stencilOffsetFSM.offsetValues[j] > upperBound ? stencilOffsetFSM.offsetValues[j] : upperBound;
             }

          int stencilWidthValue = (upperBound - lowerBound);

       // printf ("dimension j = %d stencilWidthValue = %d \n",j,stencilWidthValue);

          if (stencilWidthValue != 0)
             {
               stencilDimensionValue++;
             }
        }

     return stencilDimensionValue;
   }




