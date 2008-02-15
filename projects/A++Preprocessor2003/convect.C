#include "A++.h"

#ifdef USE_TAU
#include <Profile/Profiler.h>
#endif

/*
  main program starts here
*/

int 
main(int argc, char** argv)
{
  ios::sync_with_stdio();

#ifdef USE_TAU
  TAU_PROFILE_INIT(argc,argv);
  TAU_PROFILE("main","int (int argc, char** argv)",TAU_DEFAULT);
  TAU_PROFILE_TIMER(loop_timer,"Computation Loop","", TAU_USER);
  TAU_PROFILE_TIMER(bc_timer,"Boundary Condition","", TAU_USER);
  TAU_PROFILE_TIMER(update_timer,"Ghost Boundary Update","", TAU_USER);
  TAU_PROFILE_TIMER(copy_timer,"Array Copy","", TAU_USER);
#endif

  int Number_of_Processors=0;
  
  Optimization_Manager::setForceVSG_Update(Off);
  Optimization_Manager::Initialize_Virtual_Machine("",Number_of_Processors,argc,argv);
  Partitioning_Type::SpecifyDefaultInternalGhostBoundaryWidths(1);
  Optimization_Manager::setForceVSG_Update(Off);
  
  Index::setBoundsCheck(off);

  int myid = Communication_Manager::My_Process_Number;
  int numProcs = Communication_Manager::Number_Of_Processors;
  
  const int Xsize=1003*numProcs, Ysize=1003;

  const Range ix(-1,Xsize-2), iy(-1,Ysize-2), all;
  const Range ix1(0,Xsize-3), iy1(0,Ysize-3);
  
  Partitioning_Type thePartition;
  thePartition.SpecifyDecompositionAxes(1);

  doubleArray A(ix,iy), old_A(ix,iy), x(ix,iy), y(ix,iy), temp(ix,iy);
  const double dx=1./(Xsize-3), dy=1./(Ysize-3), dt=0.1/(Xsize+Ysize);
  double theTime=0.0,maxError;
  
  A.partition(thePartition);
  old_A.partition(thePartition);
  temp.partition(thePartition);
  x.partition(thePartition);
  y.partition(thePartition);
  
  intSerialArray theProcessorSet = (A.getPartition()).getProcessorSet();  
  
  doubleSerialArray xlocal = x.getLocalArray();
  doubleSerialArray ylocal = y.getLocalArray();  
  
  cout<<"size of xlocal: "<<xlocal.getSize()<<endl;
  
  Optimization_Manager::setOptimizedScalarIndexing(On);
  for( int i=xlocal.getBase(0);i<=xlocal.getBound(0);i++)
    xlocal(i,all) = dx*i;

  for( int j=ylocal.getBase(1);j<=ylocal.getBound(1);j++)
    ylocal(all,j) = dy*j;
  Optimization_Manager::setOptimizedScalarIndexing(Off);

  x.updateGhostBoundaries();
  y.updateGhostBoundaries();  

  A = (1.0 + theTime)*(2.0 + x + y);

  doubleSerialArray oldALocal = old_A.getLocalArray();
  doubleSerialArray ALocal = A.getLocalArray();
  doubleSerialArray tempLocal = temp.getLocalArray();

  int iLower = (myid>0)?ALocal.getBase(0)+1:ALocal.getBase(0)+1;
  int iUpper = (myid<numProcs-1)?ALocal.getBound(0)-1:ALocal.getBound(0)-1;
  int jLower = ALocal.getBase(1)+1;
  int jUpper = ALocal.getBound(1)-1;
  
  Range ILocInterior(iLower,iUpper);
  Range JLocInterior(jLower,jUpper);
  
  iLower = (myid>0)?ALocal.getBase(0)+1:ALocal.getBase(0);
  iUpper = (myid<numProcs-1)?ALocal.getBound(0)-1:ALocal.getBound(0);
  jLower = ALocal.getBase(1);
  jUpper = ALocal.getBound(1);
  
  Range ILoc(iLower,iUpper);
  Range JLoc(jLower,jUpper);

  if (myid == 0)  
    cout << "-----Starting computation" << endl; 

  double t = MPI_Wtime();

#ifdef USE_TAU
  TAU_PROFILE_START(copy_timer);
#endif

  oldALocal = ALocal;

#ifdef USE_TAU
  TAU_PROFILE_STOP(copy_timer);
  TAU_PROFILE_START(update_timer);
#endif
  
  old_A.updateGhostBoundaries();

#ifdef USE_TAU
  TAU_PROFILE_STOP(update_timer);
  TAU_PROFILE_START(loop_timer);
#endif

  tempLocal(ILocInterior,JLocInterior) = ALocal(ILocInterior,JLocInterior) -
    dt*( ( ALocal(ILocInterior+1,JLocInterior) - ALocal(ILocInterior-1,JLocInterior) ) / (2.0*dx) + 
         ( ALocal(ILocInterior,JLocInterior+1) - ALocal(ILocInterior,JLocInterior-1) ) / (2.0*dy) -
         (4.0 + 2.0*theTime + xlocal(ILocInterior,JLocInterior) + ylocal(ILocInterior,JLocInterior)) );
  
#ifdef USE_TAU
  TAU_PROFILE_STOP(loop_timer);
  TAU_PROFILE_START(copy_timer);
#endif

  //
  // copy temp into A
  //
  ALocal(ILocInterior,JLocInterior) = tempLocal(ILocInterior,JLocInterior);
  
#ifdef USE_TAU
  TAU_PROFILE_STOP(copy_timer);
  TAU_PROFILE_START(bc_timer);
#endif

  A(all,Ysize-2) = (1.0+(theTime+dt))*(2.0+x(all,Ysize-2)+y(all,Ysize-2));
  A(all,-1) = (1.0+(theTime+dt))*(2.0+x(all,-1)+y(all,-1));
  A(-1,iy1) = (1.0+(theTime+dt))*(2.0+x(-1,iy1)+y(-1,iy1));
  A(Xsize-2,iy1) = (1.0+(theTime+dt))*(2.0+x(Xsize-2,iy1)+y(Xsize-2,iy1));

#ifdef USE_TAU
  TAU_PROFILE_STOP(bc_timer);
  TAU_PROFILE_START(update_timer);
#endif

  A.updateGhostBoundaries();

#ifdef USE_TAU
  TAU_PROFILE_STOP(update_timer);
#endif

  theTime += dt;
  
  for (int k = 0; k<100; k++)
  {
      
#ifdef USE_TAU
    TAU_PROFILE_START(loop_timer);
#endif

    tempLocal(ILocInterior,JLocInterior) = oldALocal(ILocInterior,JLocInterior) - 
      2.*dt*( ( ALocal(ILocInterior+1,JLocInterior) - ALocal(ILocInterior-1,JLocInterior) ) / (2.0*dx) + 
              ( ALocal(ILocInterior,JLocInterior+1) - ALocal(ILocInterior,JLocInterior-1) ) / (2.0*dy) -
              (4.0 + 2.0*theTime + xlocal(ILocInterior,JLocInterior) + ylocal(ILocInterior,JLocInterior))  );


#ifdef USE_TAU
    TAU_PROFILE_STOP(loop_timer);
    TAU_PROFILE_START(copy_timer);
#endif

    oldALocal(ILoc,JLoc) = ALocal(ILoc,JLoc);
    ALocal(ILoc,JLoc) = tempLocal(ILoc,JLoc);

#ifdef USE_TAU
    TAU_PROFILE_STOP(copy_timer);
    TAU_PROFILE_START(bc_timer);
#endif

    A(all,Ysize-2) = (1.0+(theTime+dt))*(2.0+x(all,Ysize-2)+y(all,Ysize-2));
    A(all,-1) = (1.0+(theTime+dt))*(2.0+x(all,-1)+y(all,-1));
    A(-1,iy1) = (1.0+(theTime+dt))*(2.0+x(-1,iy1)+y(-1,iy1));
    A(Xsize-2,iy1) = (1.0+(theTime+dt))*(2.0+x(Xsize-2,iy1)+y(Xsize-2,iy1));
  
#ifdef USE_TAU
    TAU_PROFILE_STOP(bc_timer);
    TAU_PROFILE_START(update_timer);
#endif

    A.updateGhostBoundaries();

#ifdef USE_TAU
  TAU_PROFILE_STOP(update_timer);
#endif

    theTime += dt;
  }
  t = MPI_Wtime() - t;
  double maxTime;

  MPI_Reduce(&t, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  if (myid == 0)  
  {
    cout << "   Total Time= " << maxTime<< " s" << endl; 
  }
  
  maxError = max(fabs(A(ix,iy)-(1.0+theTime)*(2.0+x(ix,iy)+y(ix,iy))));
  if (myid == 0)  
  {
    cout << "max error at t="<<theTime<<"  is: "<<maxError<<endl;
    cout << "number of messages sent= "<<Diagnostic_Manager::getNumberOfMessagesSent()<<endl;
    cout << "number of messages received= "<<Diagnostic_Manager::getNumberOfMessagesReceived()<<endl;

    ofstream OutFile;
    OutFile.open("/p/gb1/bmiller/ConvectPpp/ConvectPppNew.ConstSizePerProc.out",ios::app);
    OutFile << numProcs << "  " << maxTime << "  " << maxError << endl;
    OutFile.close();
  }

#ifdef USE_TAU
  TAU_PROFILE_EXIT("Finishing Profiling.");
#endif

  Optimization_Manager::Exit_Virtual_Machine();

  return 0;
}

