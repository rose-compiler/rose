#include "test_01.h"
#include <iostream.h>

static int theMaxNumberOfBMarks(100);

int main(int argc, char* argv[])
{
  int i,j;
  double **theTimes;
  char *outFileName = new char[100];
  ofstream outFile;

  //
  // declare an array of abstract base class (BenchmarkBase)'s
  //
  BenchmarkBase **theBenchmark = new BenchmarkBase*[theMaxNumberOfBMarks];

  //===========================================================================
  // assign the first benchmark to a P++ 2d Jacobi iteration, partitioned 1d.
  // with 3 warmup iters, 50 timing iters, 1 distinct type of tests,
  // and unknowns per proc 1000.
  //===========================================================================
  theBenchmark[0] = new PPP_2d_Jacobi_1d_Part(3,10,1,100);

  // initialize
  (*theBenchmark[0]).setName(0,"time");
  (*theBenchmark[0]).initialize( argc, argv );

  int myProcNum = Communication_Manager::My_Process_Number;
  int theNumProcs = Communication_Manager::Number_Of_Processors;
 
  sprintf(outFileName,"PPP_2d_Jacobi_1d_Part.data");
  

//   if(myProcNum == 0 )
//   {  
//     strcpy(outFileName,(*theBenchmark[0]).getName(0));

//     outFile.open( strcat(outFileName,".data"),ios::app );
//     outFile<<"#";
//     outFile<<setw(12)<<"num procs"<<setw(13)<<"Unknwns/Proc";
//     outFile<<"  "<<setw(13)<<"min Time"<<"  "<<setw(13)<<"max Time"<<"  ";
//     outFile<<setw(13)<<"avg Time"<<"  "<<setw(13)<<"dev Time";
//     outFile<<endl;
//     outFile.close();
//   }
  
   outFile.open( outFileName ,ios::app );
   (*theBenchmark[0]).writeLabels( outFile );
// outFile.close();

// for(int theUnknownsPerProc=10;theUnknownsPerProc<=100000;theUnknownsPerProc*=10)
  for(int theUnknownsPerProc=1000;theUnknownsPerProc<=100000;theUnknownsPerProc*=10)
  {
    printf ("Running test with theUnknownsPerProc = %d \n",theUnknownsPerProc);

// #if !defined(_AIX)
//     dynamic_cast< PPP_2d_Jacobi_1d_Part* >(theBenchmark[0])->setUnknownsPerProc( theUnknownsPerProc );
// #else
    ((PPP_2d_Jacobi_1d_Part*)(theBenchmark[0]))->setUnknownsPerProc( theUnknownsPerProc );
// #endif

    (*theBenchmark[0]).runBenchmark();
//     theTimes = (*theBenchmark[0]).getTimings();
      
    if(myProcNum==0)
    {
      cout<<"Jacobi iteration completed for unknowns per proc= "<<theUnknownsPerProc<<endl;
    }

    (*theBenchmark[0]).writeToFile( outFile );

//     strcpy(outFileName,(*theBenchmark[0]).getName());
    
//     outFile.open( strcat(outFileName,".data"),ios::app );
//     (*theBenchmark[0]).writeToFile( outFile );
//     outFile.close();
  }//end of loop over array length


  outFile.close();

  //===========================================================================
  // finalize the P++ benchmark (implicitly calls MPI_Finalize() );
  //===========================================================================
  (*theBenchmark[0]).finalize();


  //===========================================================================
  // delete the individual b-marks in the pointer list
  //===========================================================================
  delete theBenchmark[0];
  
  //
  // delete the array of benchmark pointers.
  //
  delete[] theBenchmark;
  delete[] outFileName;
  
  return 0;
}
