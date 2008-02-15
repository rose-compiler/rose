// P++ requires that either COMPILE_PPP or COMPILE_SERIAL_APP be defined
// where as A++ defines COMPILE_APP in config.h (P++ can't do that and
// still maintain simplicity using AutoMake)
// So if we don't see COMPILE_APP then we assume we are compiling for P++!
#if !defined(COMPILE_APP)
#define COMPILE_PPP
#endif

#include "A++.h"



#define MAIN_PROCESSOR_GROUP_NAME "MainProcessorGroup"

int    Communication_Manager::Number_Of_Processors            = 1;
#if !defined(USE_PADRE)
VPROC* Communication_Manager::VirtualProcessorSpace           = NULL;
#endif
int    Communication_Manager::My_Task_ID                      = -1;
int    Communication_Manager::My_Process_Number               = -1;
int    Communication_Manager::Task_ID_Array[MAX_PROCESSORS];
char*  Communication_Manager::MainProcessorGroupName          = MAIN_PROCESSOR_GROUP_NAME;

int    Communication_Manager::restrictOutput                  = FALSE;
int    Communication_Manager::ioProcessor                     = -1;


// This variable provides a means of communicating the Number of processors and a common 
// name to use in referencing the PVM group of processors to the lower level parallel library.
int   Global_PARTI_P_plus_plus_Interface_Number_Of_Processors         = 0;
char* Global_PARTI_P_plus_plus_Interface_Name_Of_Main_Processor_Group = MAIN_PROCESSOR_GROUP_NAME;

#if !defined(USE_PADRE)
// Since we are linking with the version of PARTI in the PADRE library
// we want to use the Global_PARTI_PADRE_Interface_PADRE_Comm_World variable.
// MPI_Comm Global_PARTI_P_plus_plus_Interface_PPP_Comm_World;
extern MPI_Comm Global_PARTI_PADRE_Interface_PADRE_Comm_World;
#endif

bool Communication_Manager::prefixParallelPrintf = TRUE;

Communication_Manager::~Communication_Manager()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of destructor for Communication_Manager! \n");
#endif

#if !defined(COMPILE_APP)
     Stop_Parallel_Machine();  
#endif
   }

void
Communication_Manager::setOutputProcessor ( int processor )
   {
  // This function allows the user to specify which processor will 
  // output the results of any printf executed on all processors.

     if (processor >= 0)
        {
          restrictOutput = TRUE;
          ioProcessor = processor;
        }
       else
        {
          restrictOutput = FALSE;
          ioProcessor = -1;
        }
   }

#if 0
MPI_Comm &
Communication_Manager::getMPI_Communicator()
   {
  // This is an access function for the MPI communicator used within P++
#if defined(USE_PADRE)
     return Global_PARTI_PADRE_Interface_PADRE_Comm_World;
#else
// Since we are linking with the version of PARTI in the PADRE library
// we want to use the Global_PARTI_PADRE_Interface_PADRE_Comm_World variable.
// MPI_Comm Global_PARTI_P_plus_plus_Interface_PPP_Comm_World;
     return Global_PARTI_PADRE_Interface_PADRE_Comm_World;
#endif
   }
#endif

Communication_Manager::Communication_Manager() 
   {
     printf ("ERROR: Inside of DEFAULT constructor for Communication_Manager! \n");
     APP_ABORT();
   }

#if 0
Communication_Manager::Communication_Manager( char* Application_Program_Name , int Input_Number_Of_Processors )
   {
  // this constructor can not be used anymore ...

     printf("Communication_Manager::Communication_Manager( char* Application_Program_Name , int Input_Number_Of_Processors)    is no longer working!\n");
     APP_ABORT();
   }
#endif

Communication_Manager::Communication_Manager( 
     char* Application_Program_Name , 
     int & Input_Number_Of_Processors, 
     int & argc, char** & argv )
   {
  // this is the constructor

  // This is a test to verify that the constructor was called for a global variable used internally
  // in A++/P++.
#if 0
     if (APP_Unit_Range.getMode() == 0)
        {
          printf ("ERROR: Static initialization of global variable APP_Unit_Range failed \n");
          printf ("       This error occures often as a result of purelink -- please relink application \n");
          abort();
        }
     APP_ASSERT (APP_Unit_Range.getMode() != 0);
#endif

#if !defined(COMPILE_APP)
     Start_Parallel_Machine ( Application_Program_Name, Input_Number_Of_Processors, argc, argv );
     Initialize_Runtime_System();
     APP_ASSERT (IsParallelMachine_Initialized() == TRUE);
#endif
   }




int
Communication_Manager::localProcessNumber()
   {
#if 0
  // Since localProcessNumber() is called in the parallelPrintf function 
  // we can't call printf here (else we end up with endless recursion)
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Communication_Manager::localProcessNumber() \n");
#endif
#endif
  
#if defined(PPP)
  // Use scope resolution operator to make sure we execute the global function
  // return ::PARTI_myproc();
     return My_Process_Number;
#else
     return 0;
#endif
   }



#if 0
void
Communication_Manager::sync ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 6)
          printf ("Inside of Communication_Manager::sync() \n");
#endif
  
#if defined(PPP)
  // Finish writing out all the I/O before the sync (this simplifies debugging)
     fflush(stdout);
     fflush(stderr);
  
  // Use scope resolution operator to make sure we execute the global function
  // we don't want any explicit calls to pvm in the P++ code (only PARTI)

     Sync();
#endif
   }
#endif

void
Communication_Manager::Initialize_Runtime_System ()
   {
  // here we initialize PARTI
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf("Inside Communication_Manager::Initialize_Runtime_System ()\n");
#endif 

#if !defined(COMPILE_APP)
#if !defined(USE_PADRE)
     FFlag = 1;                    // so PARTI treats all arrays as FORTRAN ordered arrays
#else
     PADRE::setFortranOrderingFlag(1);
#endif
#if defined(PVM)
     ::myproc(My_Process_Number);  // initialize the PARTI library (we call the global myprog)
#endif

  // Bugfix (6/11/95) fixed bug in translation from old version of comm_man.C to
  // new version of comm_man.C (with ability to switch communication libraries PVM and MPI)
     Global_PARTI_P_plus_plus_Interface_Number_Of_Processors = Number_Of_Processors;

     APP_ASSERT(Global_PARTI_P_plus_plus_Interface_Number_Of_Processors == Number_Of_Processors);
     APP_ASSERT(Global_PARTI_P_plus_plus_Interface_Number_Of_Processors > 0);
     APP_ASSERT(Global_PARTI_P_plus_plus_Interface_Name_Of_Main_Processor_Group != NULL);
#endif
   }

#if defined(COMPILE_APP)
// We need to have this here for when A++ requires it
#include <sys/time.h>
double
Communication_Manager::Wall_Clock_Time()
   {
  // this one only works for unix workstation networks
  // and then only on the SUN using the Sun compilers (I think).
  // make sure that sys/time.h is included somewhere
#if defined(SUN4) && !defined(GNU)
     unsigned long ustime;
     struct timeval tp;
     struct timezone tzp;

     gettimeofday(&tp,&tzp);
     ustime = (unsigned long) tp.tv_sec;
     ustime = (ustime * 1000000) + (unsigned long) tp.tv_usec;

     return ((double) ustime) * 1e-6 ;
#else
  // Due to a silly error in the Cray T3D C compiler we can't
  // specify 1e-6 or even 0.0!
     return ((double) clock());
#endif
   }
#endif

#if 1
// Leave unimplemented!

Communication_Manager &
Communication_Manager::operator= ( const Communication_Manager & X )
   {
     printf ("Error: Inside of operator= for Communication_Manager! \n");
     APP_ABORT();

     return *this;
   }

Communication_Manager::Communication_Manager ( const Communication_Manager & X )
   {
     printf ("Error: Inside of copy constructor for Communication_Manager! \n");
     APP_ABORT();
   }
#endif


