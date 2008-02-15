// This class contains data and functionality specific to the message passing
// required for P++.  It is intended that there would be only a single instance
// of the class in a running P++ application.  The class is not used by the user
// except to access features specific to a parallel implementation.  This class is 
// mostly used internally in P++ to hold the number of processors and the 
// Virtual_Processor_Space (specific to use with Block Parti).

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif

// This is a problem section of code since it causes an error in
// mpi.h (lines 127 and 131) if mpi.h is included after A++.h.
// For now the work around is to use mpi.h BEFORE A++.h.
// This problem came up when combining the MLB work into
// and A++ application.  It would be handy if the MPI header
// would define the MPI macro -- I think.
// A better solution is to test if MPI is defined by testing for MPI_INT
//#ifndef MPI_INT
#if !defined(MPI)
#define MPI_Comm int
#define MPI_Group int
#endif

#if !defined(PPP)
// We need to define these in the case where we are not using P++
#define VPROC int
#define MAX_PROCESSORS 1
#endif


// The macro MAX_PROCESSORS is defined in the A++_headers.h file

class Communication_Manager
   {
  // This class provides a consistant interface for the use of 
  // P++ on different parallel environments
  
     public:
#if defined(MPI)
       // Bugfix (12/21/95) Jacob Ystrom (Sweden) sent us this fix.
          static MPI_Group MPI_Comm_Group, PPP_Comm_Group;
          static MPI_Comm PPP_Comm_World;
#endif

          static int Number_Of_Processors;

       // Used to control the prefix of all output using printf with the node number
          static bool prefixParallelPrintf;

#if !defined(USE_PADRE)
       // PARTI needs this: array of virtual processors (processor space) 
          static VPROC *VirtualProcessorSpace;
#endif
  
       // the PVM interface needs these
          static int My_Task_ID;                    // my task id
          static char* MainProcessorGroupName;
          static int Task_ID_Array[MAX_PROCESSORS]; // array of task id

       // This variable is used within the Sync member function to 
       // determine if the parallel machine has been initialized.
       // It is default initialized to be -1 and if it is -1 then
       // the parallel machine has NOT been initialized.  If it is
       // a valid processor number (greater than 0) then the parallel
       // machine has been initialized.
       // at a later point we should have a static member function that
       // determines if the parallel machine has been initialized.
          static int My_Process_Number;  // my process number

       // Processor from which to print output of parallelPrintf
          static int ioProcessor;
          static int restrictOutput;
          static void setOutputProcessor ( int processor );

         ~Communication_Manager ();
          Communication_Manager ();

       // this constructor doesn't work anymore
       // Communication_Manager ( char* Application_Program_Name, int Number_Of_Processors );


       // we need to hand over argc and argv in order to make MPI happy
          Communication_Manager ( char* Application_Program_Name, 
                                  int & Number_Of_Processors, int & argc, char** & argv);

          static int localProcessNumber();
          static void Initialize_Runtime_System();

          static bool IsParallelMachine_Initialized() { return (My_Process_Number != -1); }
  
          static void MessagePassingInterpretation ( On_Off_Type On_Off_Variable );

       // This is an access function for the MPI communicator used within P++
          static MPI_Comm & getMPI_Communicator();

       // Used to control the prefix of all output using printf with the node number
	 static void setPrefixParallelPrintf ( bool X )
            { prefixParallelPrintf = X; }
	 static bool getPrefixParallelPrintf ()
            { return prefixParallelPrintf; }

     private:
       // Don't define the functions here even if they are not used!
          Communication_Manager & operator= ( const Communication_Manager & X );
          Communication_Manager ( const Communication_Manager & X );

       // These are either PVM or MPI specific and are located in the 
       // PVM_specific.C or MPI_specific.C files
          static void Start_Parallel_Machine( char* Application_Program_Name , 
                                              int & Input_Number_Of_Processors, 
                                              int & argc, char** & argv);
          static void Stop_Parallel_Machine();

     public:
       // This is either PVM or MPI specific and is located in the 
       // PVM_specific.C or MPI_specific.C files
          static double Wall_Clock_Time();

          static int numberOfProcessors() 
             { return Number_Of_Processors; }

          static void fillProcessorArray ( double* processorArray, double value );
          static void fillProcessorArray ( float*  processorArray, float  value );
          static void fillProcessorArray ( int*    processorArray, int    value );

#if defined(APP)
       // A++ code should be able to call Sync -- though it should do nothing!
          static void Sync() {};
#else
       // Both PVM and MPI define this function differently so it is
       // implemented in P++/src/PVM_specific.C or P++/MPI_specific.C
          static void Sync();
#endif
   };












