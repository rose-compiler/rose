#ifndef __PPP_2D_JACOBI_1D_PART__
#define __PPP_2D_JACOBI_1D_PART__

#include "A++.h"
#include "BenchmarkBase.h"

class PPP_2d_Jacobi_1d_Part:public BenchmarkBase
{
 public:
  PPP_2d_Jacobi_1d_Part();
  PPP_2d_Jacobi_1d_Part(int inWarmups, int inTimings, int inNumTests=1, int inUnknownsPerProc=1000);
  PPP_2d_Jacobi_1d_Part( const PPP_2d_Jacobi_1d_Part &inBB);
  virtual ~PPP_2d_Jacobi_1d_Part();

  PPP_2d_Jacobi_1d_Part& operator=( const PPP_2d_Jacobi_1d_Part &inRhs );

  void setUnknownsPerProc( int inUnknownsPerProc );
  int getUnknownsPerProc( ) const;
  
  virtual void initialize(int inArgc, char** inArgv);
  virtual void finalize();
  
  virtual void runBenchmark();

  virtual void writeToFile(ostream &outFile);
  virtual void writeLabels(ostream &outFile);
  
 private:
  int mUnknownsPerProc;
};

#endif
