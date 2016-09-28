// Allow repeated includes of optimization.h without error
#ifndef _APP_OPTIMIZATION_H
#define _APP_OPTIMIZATION_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#endif

#define START_DEFERRED_EVALUATION 0
#define STOP_DEFERRED_EVALUATION  1

class Set_Of_Tasks;
class Communication_Manager;

#define Deferred_Evaluation(TASK_SET)for(Optimization_Manager::APP_Global_DE_Var=Optimization_Manager::Deferred_Evaluation_ON(TASK_SET); \
                                         Optimization_Manager::APP_Global_DE_Var == START_DEFERRED_EVALUATION; \
                                         Optimization_Manager::APP_Global_DE_Var=Optimization_Manager::Deferred_Evaluation_OFF())

class Optimization_Manager
   {
     public:
       // Support variables for Deferred Evaluation!
          static int APP_Global_DE_Var;   // used for macro implementation!
          static int Previous_Deferred_Evaluation_State;

          static Set_Of_Tasks *Current_Deferred_Block_Pointer;

          static int Deferred_Evaluation_ON  ( const Set_Of_Tasks & Deferred_Block );
          static int Deferred_Evaluation_OFF ();

          static int Number_Of_Assignments_Optimized_Out;

          static Communication_Manager* Communication_Manager_Pointer;

          static bool Optimize_Scalar_Indexing;
          static bool ForceVSG_Update;

       // These should be moved to the Diagnostic_Manager (they are now!)
       // static int Number_Of_Messages_Sent;
       // static int Number_Of_Messages_Recieved;

       // This new version of the initialization function takes all the arguments in the union
       // of PVM and MPI requirements
       // static void Initialize_Virtual_Machine ( char* Application_Program_Name = "" , int Number_Of_Processors = 1 );
          static void Initialize_Virtual_Machine (
               char* Application_Program_Name , int & Number_Of_Processors , int & argc , char** & argv );
          static void Exit_Virtual_Machine ();
          static void setOptimizedScalarIndexing ( On_Off_Type On_Off = On );

       // We can force all message passing to be done using the VSG model.
       // This would force communication at every binary operation.
       // This is mostly a debugging option since it only slows the performance
       // for all but unaligned operations (most operations occure between aligned 
       // array objects).
          static void setForceVSG_Update ( On_Off_Type On_Off = Off );

       // part of optional mechanism to cleanup after the last array has been deleted
          static void freeMemoryInUse();

     public:
         ~Optimization_Manager ();
          Optimization_Manager ();
   };


class DiagnosticInfo
   {
  // An array of these objects are allocated together with the reference count 
  // array (handled by the same functions).  One object is allocated for each
  // array object allocated (but not for views).  The array is indexed using the
  // Array_ID (same as for the reference count array).
     public:

       // Maybe we should just store pointers to the array domain objects in each array
       // then we can answer any and all possible questions.  NO! NO! NO! (VSG support in P++ 
       // (use of the "Temp" array in VSG macro in conform_enforce.C_m4) is an example!)
       // The problem with storing array domains is that the array object could go out of scope
       // before the view of the array object goes out of scope and then we would have a valid
       // array object but with the diagnostic info pointing off into heap memory!
       // Better to have the diagnostics info store it's own data and force functions 
       // that modify such data in the array to update the diagnostic info.

          int typeCode;
          int arraySize [MAX_ARRAY_DIMENSION];

          DiagnosticInfo ();

       // compute the array size (number of elements)
          int size() const;

       // compute the size of each element
          int getTypeSize() const;

          void setTypeCode(int);
          int getTypeCode() const;

       // Get type string for each code value
          char* getTypeString() const;

       // compute the number of dimensions of the array object
          int numberOfDimensions() const;

          DiagnosticInfo ( const DiagnosticInfo & X );
          DiagnosticInfo & operator= ( const DiagnosticInfo & X );
	  void initialize();

#if defined(SERIAL_APP)
          DiagnosticInfo ( const SerialArray_Domain_Type & X );
          void initialize( const SerialArray_Domain_Type & X, int inputTypeCode );
#else
          DiagnosticInfo ( const Array_Domain_Type & X );
          void initialize( const Array_Domain_Type & X, int inputTypeCode );
#endif
   };


class Diagnostic_Manager
   {
  // This is where we place future diagnostic features of A++/P++
  // Substantial work is planned for this class it will also interact with 
  // the TAU Instrumentation tools to provide runtime feedback.

  // Note that the array domain objects can optionally include a Diagnostic_Manager
  // allowing a more detailed object by object report of the internal behavior.
  // The reporting is restricted to objects in use when the report is built.
  // Note that unless the Array_ID stack mechanism is disabled no unique identifier for
  // each array object is possible. Such a mechanism is easy to build (but has not been done).
  // Note that we have not added non-static data yet either!

     public:

       // A++/P++ error code returned by the exit function to the operating system
          static int APP_ErrorCode;

       // Total number of messages sent and recieved (for this local processor)
       // These must be references so that they can reference global variables in the
       // C language implementation of the lower level libraryes (PARTI for example).
          static int & numberOfMessagesSent;
          static int getNumberOfMessagesSent();
          static void incrementNumberOfMessagesSent();

          static int & numberOfMessagesRecieved;
          static int getNumberOfMessagesReceived();
          static void incrementNumberOfMessagesReceived();

       // Total number of ghost boundary updates (for this local processor)
       // these can include multiple messages (depending on the dimensionality of the array objects)
          static int numberOfGhostBoundaryUpdates;
          static int getNumberOfGhostBoundaryUpdates();
          static void incrementNumberOfGhostBoundaryUpdates();

       // Total number of regular section transfers (the most general communication mechanism)
       // (for this local processor) these can include multiple messages (depending on the 
       // dimensionality and the complexity of the distribution of the array objects)
          static int numberOfRegularSectionTransfers;
          static int getNumberOfRegularSectionTransfers();
          static void incrementNumberOfRegularSectionTransfers();

       // Total number of scalar indexing operatiosn that requied communication
          static int numberOfScalarIndexingOperationsRequiringGlobalBroadcast;
          static int getNumberOfScalarIndexingOperationsRequiringGlobalBroadcast();
          static void incrementNumberOfScalarIndexingOperationsRequiringGlobalBroadcast();

       // Total number of scalar indexing operatiosn that requied communication
          static int numberOfScalarIndexingOperations;
          static int getNumberOfScalarIndexingOperations();
          static void incrementNumberOfScalarIndexingOperations();

       // I think this variable should be here even though several 
       // similar variables are located in the Array_Domain objects.
       // This variable activates the internal tracking of memory 
       // allocation/deallocation so that reporting on total memory
       // useage can be done.
          static bool trackArrayData;

       // Diagnostic information is maintained by the functions that manipulate the 
       // reference count arrays.  Nearly any sort of information could be stored.
       // The length of this array is the same as that of the reference count 
       // array in the Array_Domain_Type
          static DiagnosticInfo** diagnosticInfoArray;

       // Helpful in understanding the message passing generated within P++
          static int & messagePassingInterpretationReport;
          static int  getMessagePassingInterpretationReport();
          static void setMessagePassingInterpretationReport( int x );

       // Helpful in understanding the reference counting used within P++
          static int referenceCountingReport;
          static int  getReferenceCountingReport();
          static void setReferenceCountingReport( int x );

         ~Diagnostic_Manager ();
          Diagnostic_Manager ();

       // These function are implemented in the optimization.C source file
          static int getMaxNumberOfArrays();
          static int getNumberOfArraysInUse();

#if defined(SERIAL_APP)
          static intSerialArray getProcessorLoads();
#else
          static intArray getProcessorLoads();
#endif

          static void displayProcessorLoads();

       // print out the sizes of the different classes used in A++/P++
          static void getSizeOfClasses();

          static int getTotalArrayMemoryInUse();
          static int getMemoryOverhead();
          static int getTotalMemoryInUse();
          static int getnumberOfArraysConstantDimensionInUse( int dimension, int inputTypeCode );

       // Access to functionality supported by purify
          static void displayPurify (const char* Label = "");
          static int getPurifyUnsupressedMemoryLeaks();

          static void report();

       // Control for memory cleanup for exit for A++/P++ applications
          static void setSmartReleaseOfInternalMemory( On_Off_Type On_Off = On );
          static bool getSmartReleaseOfInternalMemory();

          static void setExitFromGlobalMemoryRelease( bool trueFalse = TRUE );
          static bool getExitFromGlobalMemoryRelease();

       // Access functions for turning on and off the internal diagnostics
          static void setTrackArrayData( bool trueFalse = TRUE );
          static bool getTrackArrayData();

       // part of optional mechanism to cleanup after the last array has been deleted
          static void freeMemoryInUse();

       // Force input array object to be used in all possible ways to test it's consistancy
       // this is an alternative to the consistency check which only inspects the internal data
       // for correctness.  This function writes over any data stored in the array.
          static void test ( intArray    & X );
          static void test ( floatArray  & X );
          static void test ( doubleArray & X );

#if 1
#if defined(SERIAL_APP)
     // This only makes since in P++
        static intSerialArray buildCommunicationMap ();
        static intSerialArray buildPurifyMap ();
#else
     // Preserve the interface in A++
        static intArray buildCommunicationMap ();
        static intArray buildPurifyMap ();
#endif
#endif

     // Build a report showing the communication (numbers of different kinds of messages)
     // static intSerialArray buildCommunicationMap ();
        static void displayCommunication (const char* Label = "");
        static void resetCommunicationCounters ();
   };

  /* !defined(_APP_OPTIMIZATION_H) */
#endif

