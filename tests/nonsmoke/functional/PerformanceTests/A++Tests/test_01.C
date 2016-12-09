#include "test_01.h"
#include "A++.h"


//\begin{>PPP_2d_Jacobi_1d_Part.tex}{\subsection{Public Member Functions}}
//\no function header:
//\end{PPP_2d_Jacobi_1d_Part.tex}

//\begin{>>PPP_2d_Jacobi_1d_Part.tex}{\subsubsection{Constructor}}
PPP_2d_Jacobi_1d_Part::
PPP_2d_Jacobi_1d_Part()
    :BenchmarkBase(),mUnknownsPerProc(1000)
//================================================================
// /Description:  Default constructor.  Does nothing yet.
//
// /Return Values: None.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  24 August 2000
//\end{PPP_2d_Jacobi_1d_Part.tex}
//================================================================
{
}


//\begin{>>PPP_2d_Jacobi_1d_Part.tex}{\subsubsection{Constructor}}
PPP_2d_Jacobi_1d_Part::
PPP_2d_Jacobi_1d_Part(int inWarmups, int inTimings, int inNumTests, int inUnknownsPerProc)
    :BenchmarkBase(inWarmups,inTimings,inNumTests),mUnknownsPerProc(inUnknownsPerProc)
//================================================================
// /Description:  Normal constructor, initializes data.
//
// /Return Values: None.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  24 August 2000
//\end{PPP_2d_Jacobi_1d_Part.tex}
//================================================================
{
}


//\begin{>>PPP_2d_Jacobi_1d_Part.tex}{\subsubsection{Copy Constructor}}
PPP_2d_Jacobi_1d_Part::
PPP_2d_Jacobi_1d_Part( const PPP_2d_Jacobi_1d_Part &inBB )
    :BenchmarkBase(inBB)
//================================================================
// /Description:  Normal constructor, initializes data.
//
// /Return Values: None.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  24 August 2000
//\end{PPP_2d_Jacobi_1d_Part.tex}
//================================================================
{
  mUnknownsPerProc = inBB.mUnknownsPerProc;
}


//\begin{>>PPP_2d_Jacobi_1d_Part.tex}{\subsubsection{Destructor}}
PPP_2d_Jacobi_1d_Part::
~PPP_2d_Jacobi_1d_Part( )
//================================================================
// /Description:  Normal destructor.
//
// /Return Values: None.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  24 August 2000
//\end{PPP_2d_Jacobi_1d_Part.tex}
//================================================================
{
}


//\begin{>>PPP_2d_Jacobi_1d_Part.tex}{\subsubsection{operator=}}
PPP_2d_Jacobi_1d_Part& PPP_2d_Jacobi_1d_Part::
operator=( const PPP_2d_Jacobi_1d_Part& inRhs)
//================================================================
// /Description:  Assignment operator.
//
// /inRhs (input):  The object to assign to this
//
// /Return Values: *this, after the assignment.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  24 August 2000
//\end{PPP_2d_Jacobi_1d_Part.tex}
//================================================================
{
  if( this != &inRhs )
  {
    /* call assignement operator for base class */
    BenchmarkBase::operator=( inRhs );
    
    mUnknownsPerProc = inRhs.mUnknownsPerProc;
  }
  return *this;
}



//\begin{>>PPP_2d_Jacobi_1d_Part.tex}{\subsubsection{setUnknownsPerProc}}
void PPP_2d_Jacobi_1d_Part::
setUnknownsPerProc( int inUnknownsPerProc )
//================================================================
// /Description:  Sets the length of each dimension of the arrays.
//
// /inUnknownsPerProc (input):  The specified dimension size.
//
// /Return Values: None.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  29 August 2000
//\end{PPP_2d_Jacobi_1d_Part.tex}
//================================================================
{
  mUnknownsPerProc = inUnknownsPerProc;
}


//\begin{>>PPP_2d_Jacobi_1d_Part.tex}{\subsubsection{getUnknownsPerProc}}
int PPP_2d_Jacobi_1d_Part::
getUnknownsPerProc( )const
//================================================================
// /Description:  Returns the length of the arrays in each dimension.
//
// /Return Values:  The length of the arrays in each dimension.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  29 August 2000
//\end{PPP_2d_Jacobi_1d_Part.tex}
//================================================================
{
  return mUnknownsPerProc;
}



//\begin{>>PPP_2d_Jacobi_1d_Part.tex}{\subsubsection{initialize}}
void PPP_2d_Jacobi_1d_Part::
initialize( int inArgc, char** inArgv  )
//================================================================
// /Description:  Sets up any initialization needed for this benchmark.
//
// /inArgc (input):  The reference to argc.
// /inArgv (input):  The reference to the argv pointer.
//
// /Return Values: None.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  28 August 2000
//\end{PPP_2d_Jacobi_1d_Part.tex}
//================================================================
{
  // Sync up the C++ and C i/o systems so that printf can work together with cout
  // sync printf's with cout's
  ios::sync_with_stdio();

  // turn on bounds checking
  Index::setBoundsCheck(On);
    
  // The correct number of processors is obtained in numberOfProcessors after the function call
  int numberOfProcessors = -1;
  Optimization_Manager::Initialize_Virtual_Machine ("", numberOfProcessors, inArgc, inArgv);

  // sync up all processors
  // MPI_Barrier(MPI_COMM_WORLD);

}



//\begin{>>PPP_2d_Jacobi_1d_Part.tex}{\subsubsection{finalize}}
void PPP_2d_Jacobi_1d_Part::
finalize(  )
//================================================================
// /Description:  Finalizes anything needed for this benchmark.
//
// /Return Values: None.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  24 August 2000
//\end{PPP_2d_Jacobi_1d_Part.tex}
//================================================================
{
  if( Communication_Manager::My_Process_Number == 0 )
    printf ("Program Terminated Normally! \n");

  // exit the parallel environment
  Optimization_Manager::Exit_Virtual_Machine();

}




//\begin{>>PPP_2d_Jacobi_1d_Part.tex}{\subsubsection{runBenchmark}}
void PPP_2d_Jacobi_1d_Part::
runBenchmark(  )
//================================================================
// /Description:  Executes the benchmarking code the specified
//  number of times and stores the times in the array.
//
// /Return Values: None.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  24 August 2000
//\end{PPP_2d_Jacobi_1d_Part.tex}
//================================================================
{
  double theTime,theStartTime,theEndTime;
  
  int theNumProcs = 1;
  int thisProcNum = 0;

// MPI_Comm_size(MPI_COMM_WORLD, &theNumProcs);
// MPI_Comm_rank(MPI_COMM_WORLD, &thisProcNum);
 
  Partitioning_Type Partitioning(Range(0,theNumProcs-1));
  Partitioning.SpecifyInternalGhostBoundaryWidths(1,1);
  Partitioning.SpecifyDecompositionAxes(1);

  int theArraySideLength = sqrt( mUnknownsPerProc * theNumProcs );

  printf ("theArraySideLength = %d \n",theArraySideLength);
  
  doubleArray U_old(theArraySideLength,theArraySideLength,Partitioning);
  U_old = 0.0;

  Index I (1,theArraySideLength-2);
  Index J (1,theArraySideLength-2);
  int i;

// printf ("Warming up ... \n");
  for(i=0;i< mNumberOfWarmupIterations;i++)
  {
    U_old(I,J) =
      (U_old(I+1,J+1) + U_old(I+1,J) + U_old(I+1,J-1) + U_old(I,J+1) +
       U_old(I,J-1) + U_old(I-1,J+1) + U_old(I-1,J) + U_old(I-1,J-1)) / 8.0;
  }
  
  // Now time the problem
// printf ("Running timing loop ... \n");
  for (i=0; i < mNumberOfTimingIterations; i++)
  {
//  printf ("Running timing loop iteration %d \n",i);
    theStartTime = Communication_Manager::Wall_Clock_Time();
//    theStartTime = clock();
    U_old(I,J) =
      (U_old(I+1,J+1) + U_old(I+1,J) + U_old(I+1,J-1) + U_old(I,J+1) +
       U_old(I,J-1) + U_old(I-1,J+1) + U_old(I-1,J) + U_old(I-1,J-1)) / 8.0;
    theEndTime = Communication_Manager::Wall_Clock_Time();
//    theEndTime = clock();
    theTime = theEndTime - theStartTime;
    
    mTimes[0][i] = theTime;
    printf("time= %f\n",theTime);
    
  }//end of loop over timed iterations
}


//\begin{>>PPP_2d_Jacobi_1d_Part.tex}{\subsubsection{writeToFile}}
void PPP_2d_Jacobi_1d_Part::
writeToFile(ostream &outFile )
//================================================================
// /Description:  Writes the Timing data to a file named from
//   the benchmark name.
//
// /outFile (input):  The stream to write data to.
//
// /Return Values: None.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  29 August 2000
//\end{PPP_2d_Jacobi_1d_Part.tex}
//================================================================
{
  int theNumProcs = 1;
  int myProcNum   = 0;

// MPI_Comm_rank(MPI_COMM_WORLD, &myProcNum);
// MPI_Comm_size(MPI_COMM_WORLD, &theNumProcs);
  
  computeStatistics();

  if(myProcNum == 0 )
  {
// Parallel specific data commented out  
// outFile<<setw(13)<<theNumProcs<<setw(13)<<mUnknownsPerProc; 
   outFile<<setw(13)<<mUnknownsPerProc;
   BenchmarkBase::writeToFile( outFile );
  }
}



//\begin{>>StepanovBench.tex}{\subsubsection{writeLabels}}
void PPP_2d_Jacobi_1d_Part::
writeLabels(ostream &outFile )
//================================================================
// /Description:  Writes the test names to a file named from
//   the benchmark name.
//
// /outFile (input):  The stream to write data to.
//
// /Return Values: None.
//
// /Errors:
//   None.
//
// /Author:  BJM
// /Date:  05 February 2001
//\end{StepanovBench.tex}
//================================================================
{

outFile<<"#"<<"Unknowns/Proc";
  
  BenchmarkBase::writeLabels( outFile );
}//end of writeLabels



