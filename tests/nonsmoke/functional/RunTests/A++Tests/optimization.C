#if 0
// P++ requires that either COMPILE_PPP or COMPILE_SERIAL_APP be defined
// where as A++ defines COMPILE_APP in config.h (P++ can't do that and
// still maintain simplicity using AutoMake)
// So if we don't see COMPILE_APP then we assume we are compiling for P++!
#if defined(COMPILE_APP)
#error "In optimization.C COMPILE_APP is defined"
#else
#error "In optimization.C COMPILE_APP is NOT defined"
#endif

#if defined(COMPILE_SERIAL_APP)
#error "In optimization.C COMPILE_SERIAL_APP is defined"
#else
#error "In optimization.C COMPILE_SERIAL_APP is NOT defined"
#endif

#if !defined(COMPILE_APP)
#error "COMPILE_APP not defined in optimization.C"
// define COMPILE_SERIAL_APP
#endif
#endif

#include "A++.h"
#include <string.h>

// If we are compiling with P++ then se want to compile this 
// file as though it were part of the serial array compilation.
#if defined(COMPILE_PPP)
// error "COMPILE_APP not defined in optimization.C"
#define SERIAL_APP
#endif

// global variables to permit use of static class member by PARTI.
// The internal diagnostics includes variables which are references these.
int PARTI_numberOfMessagesSent               = 0;
int PARTI_numberOfMessagesRecieved           = 0;
int PARTI_messagePassingInterpretationReport = 0;

// Static variable declaration
int Optimization_Manager::Previous_Deferred_Evaluation_State               = FALSE;
Set_Of_Tasks *Optimization_Manager::Current_Deferred_Block_Pointer         = NULL;

int Optimization_Manager::Number_Of_Assignments_Optimized_Out              = 0;

bool Optimization_Manager::Optimize_Scalar_Indexing                     = FALSE;
bool Optimization_Manager::ForceVSG_Update                              = FALSE;


Communication_Manager* Optimization_Manager::Communication_Manager_Pointer = NULL;

bool Diagnostic_Manager::trackArrayData                                 = FALSE;
DiagnosticInfo** Diagnostic_Manager::diagnosticInfoArray                   = NULL;

// Make these references to the global variables (we need 
// the global variable so that C code can reference these)
int & Diagnostic_Manager::messagePassingInterpretationReport = PARTI_messagePassingInterpretationReport;
int & Diagnostic_Manager::numberOfMessagesSent               = PARTI_numberOfMessagesSent;
int & Diagnostic_Manager::numberOfMessagesRecieved           = PARTI_numberOfMessagesRecieved;

int Diagnostic_Manager::numberOfGhostBoundaryUpdates                             = 0;
int Diagnostic_Manager::numberOfScalarIndexingOperations                         = 0;
int Diagnostic_Manager::numberOfScalarIndexingOperationsRequiringGlobalBroadcast = 0;
int Diagnostic_Manager::numberOfRegularSectionTransfers                          = 0;

int Diagnostic_Manager::referenceCountingReport                                  = 0;

// Location of A++/P++ error return code (returned by exit to unix operating system)
int Diagnostic_Manager::APP_ErrorCode                                            = 0;

// ****************************************************************************
// ****************************************************************************
//               Optimization_Manager class member functions
// ****************************************************************************
// ****************************************************************************

// We have for simplicity put this function into the this file rather than building
// a separate file just for the Diagnostic_Manager functions.
int
Diagnostic_Manager::getNumberOfMessagesSent()
   {
  // Total number of messages sent
     return numberOfMessagesSent;
   }

void
Diagnostic_Manager::incrementNumberOfMessagesSent()
   {
  // Total number of messages sent
     numberOfMessagesSent++;
   }

int
Diagnostic_Manager::getNumberOfMessagesReceived()
   {
  // Total number of messages recieved
     return numberOfMessagesRecieved;
   }

void
Diagnostic_Manager::incrementNumberOfMessagesReceived()
   {
  // Total number of messages recieved
     numberOfMessagesRecieved++;
   }

int
Diagnostic_Manager::getNumberOfGhostBoundaryUpdates()
   {
  // This function counts the number of ghost boudary updates 
  // 1 count represents all the ghost boundaries updated once
  // for a single array object.
     return numberOfGhostBoundaryUpdates;
   }

void
Diagnostic_Manager::incrementNumberOfGhostBoundaryUpdates()
   {
  // This function counts the number of ghost boudary updates 
  // 1 count represents all the ghost boundaries updated once
  // for a single array object.
     numberOfGhostBoundaryUpdates++;
   }

int
Diagnostic_Manager::getNumberOfRegularSectionTransfers()
   {
  // This function counts the number of regular section transfers
  // (the more expensive but more general communication mechanism).
     return numberOfRegularSectionTransfers;
   }

void
Diagnostic_Manager::incrementNumberOfRegularSectionTransfers()
   {
  // This function counts the number of regular section transfers
  // (the more expensive but more general communication mechanism).
     numberOfRegularSectionTransfers++;
   }

int
Diagnostic_Manager::getNumberOfScalarIndexingOperationsRequiringGlobalBroadcast()
   {
  // Total number of global broadcasts associated with P++ scalar indexing
  // These are generally to be avoided since they are very slow
     return numberOfScalarIndexingOperationsRequiringGlobalBroadcast;
   }

void
Diagnostic_Manager::incrementNumberOfScalarIndexingOperationsRequiringGlobalBroadcast()
   {
  // Total number of global broadcasts associated with P++ scalar indexing
  // These are generally to be avoided since they are very slow
     numberOfScalarIndexingOperationsRequiringGlobalBroadcast++;
   }

int
Diagnostic_Manager::getNumberOfScalarIndexingOperations()
   {
  // Total number of scalar indexing operations (includes both the number that
  // require global broadcasts and those for which the communication is optimized
  // away (when the user can specify that it should be)
     return numberOfScalarIndexingOperations;
   }

void
Diagnostic_Manager::incrementNumberOfScalarIndexingOperations()
   {
  // Total number of scalar indexing operations (includes both the number that
  // require global broadcasts and those for which the communication is optimized
  // away (when the user can specify that it should be)
     numberOfScalarIndexingOperations++;
   }

int
Diagnostic_Manager::getMessagePassingInterpretationReport()
   {
     return messagePassingInterpretationReport;
   }

void
Diagnostic_Manager::setMessagePassingInterpretationReport( int x )
   {
  // Only set one of these since it is a reference to the other.
  // PARTI_messagePassingInterpretationReport = x;
     messagePassingInterpretationReport       = x;
   }

int
Diagnostic_Manager::getReferenceCountingReport()
   {
     return referenceCountingReport;
   }

void
Diagnostic_Manager::setReferenceCountingReport( int x )
   {
  // Only set one of these since it is a reference to the other.
  // PARTI_messagePassingInterpretationReport = x;
     referenceCountingReport = x;
   }

int
Diagnostic_Manager::getMaxNumberOfArrays()
   {
  // In the current implementation this returns only the maximum number of
  // arrays that have been built.  Once we fix the reference counting to
  // use zero properly it will be rturn a acurate value for the number of
  // array objects that are currently in use. In P++ this will only apply
  // to the processor where this function is called.
     return Array_Domain_Type::getMaxNumberOfArrays();
   }

int
Diagnostic_Manager::getNumberOfArraysInUse()
   {
  // In the current implementation this returns only the maximum number of
  // arrays that have been built.  Once we fix the reference counting to
  // use zero properly it will be rturn a acurate value for the number of
  // array objects that are currently in use. In P++ this will only apply
  // to the processor where this function is called.
#if defined(SERIAL_APP)
     return SerialArray_Domain_Type::getNumberOfArraysInUse();
#else
     return Array_Domain_Type::getNumberOfArraysInUse();
#endif
   }

void
Diagnostic_Manager::setSmartReleaseOfInternalMemory ( On_Off_Type On_Off )
   {
  // This sets the optional use of a mechanism within A++/P++ to release
  // internal memory used by A++/P++ based on the destructor call for the 
  // last existing array object.  We can't make this the default (or let the
  // default be TRUE or On) because this might force A++/P++ to prematurely
  // release its internal memory which is required for any subsequent A++/P++
  // operation.  This is an alternative to the Memory_Manager_Type object
  // which can presently be used with A++/P++ but not Overture applications.
     if ( (On_Off == On) || (On_Off == ON) || (On_Off == on) )
#if defined(SERIAL_APP)
          SerialArray_Domain_Type::smartReleaseOfInternalMemory = TRUE;
       else
          SerialArray_Domain_Type::smartReleaseOfInternalMemory = FALSE;
#else
          Array_Domain_Type::smartReleaseOfInternalMemory = TRUE;
       else
          Array_Domain_Type::smartReleaseOfInternalMemory = FALSE;
#endif
   }

void
Diagnostic_Manager::setExitFromGlobalMemoryRelease ( bool trueFalse )
   {
  // This sets the optional use of a mechanism within A++/P++ to release
  // internal memory used by A++/P++ based on the destructor call for the 
  // last existing array object.  We can't make this the default (or let the
  // default be TRUE or On) because this might force A++/P++ to prematurely
  // release its internal memory which is required for any subsequent A++/P++
  // operation.  This is an alternative to the Memory_Manager_Type object
  // which can presently be used with A++/P++ but not Overture applications.
     if ( trueFalse == TRUE )
#if defined(SERIAL_APP)
          SerialArray_Domain_Type::exitFromGlobalMemoryRelease = TRUE;
       else
          SerialArray_Domain_Type::exitFromGlobalMemoryRelease = FALSE;
#else
          Array_Domain_Type::exitFromGlobalMemoryRelease = TRUE;
       else
          Array_Domain_Type::exitFromGlobalMemoryRelease = FALSE;
#endif
   }

bool
Diagnostic_Manager::getSmartReleaseOfInternalMemory()
   {
#if defined(SERIAL_APP)
     return SerialArray_Domain_Type::smartReleaseOfInternalMemory;
#else
     return Array_Domain_Type::smartReleaseOfInternalMemory;
#endif
   }


bool
Diagnostic_Manager::getExitFromGlobalMemoryRelease()
   {
#if defined(SERIAL_APP)
     return SerialArray_Domain_Type::exitFromGlobalMemoryRelease;
#else
     return Array_Domain_Type::exitFromGlobalMemoryRelease;
#endif
   }

void
Diagnostic_Manager::setTrackArrayData( bool trueFalse )
   {
  // Access functions for turning on and off the internal diagnostics
     trackArrayData = trueFalse;
   }

bool
Diagnostic_Manager::getTrackArrayData()
   {
  // Access functions for turning on and off the internal diagnostics
     return trackArrayData;
   }


int
Diagnostic_Manager::getTotalArrayMemoryInUse()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Diagnostic_Manager::getTotalMemoryInUse() \n");
#endif

     int total = 0;
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
#if defined(SERIAL_APP)
          int length = SerialArray_Domain_Type::Array_Reference_Count_Array_Length;
#else
          int length = Array_Domain_Type::Array_Reference_Count_Array_Length;
#endif

          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray != NULL);
          for (int i=0; i < length; i++)
             {
            // if (Diagnostic_Manager::diagnosticInfoArray[i] != NULL)
               if ( (Diagnostic_Manager::diagnosticInfoArray[i] != NULL) &&
                    (Diagnostic_Manager::diagnosticInfoArray[i]->getTypeCode() != APP_UNDEFINED_ELEMENT_TYPE) )
                  {
                    APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[i] != NULL);
#if 0
                    printf ("found an array object! array id = %d \n",i);
#endif
                    int subtotal = Diagnostic_Manager::diagnosticInfoArray[i]->size();
                    int sizeOfElement = Diagnostic_Manager::diagnosticInfoArray[i]->getTypeSize();
                    total += subtotal * sizeOfElement;
                  }
                 else
                  {
#if 0
                    printf ("No corresponding array for array id = %d \n",i);
#endif
                  }
             }
        }

#if defined(SERIAL_APP)
  // In the parallel environment at this point we only
  // have the total memory in use on this processor.

     int numberOfNodes = Communication_Manager::numberOfProcessors();

  // temp storage (required for Communication_Manager::fillProcessorArray)
     int* processorArray = new int [numberOfNodes];
     APP_ASSERT (processorArray != NULL);

  // initialize the newly allocated space
     int j = 0;
     for (j = 0; j < numberOfNodes; j++)
        {
          processorArray[j] = 0;
        }

  // fill in the local totals each processor (into the processorArray)
     Communication_Manager::fillProcessorArray ( processorArray, total );

     total = 0;
     for (j = 0; j < numberOfNodes; j++)
        {
          total += processorArray[j];
        }

  // now delete the allocated data (from above)
     delete processorArray;
     processorArray = NULL;
#endif

     return total;
   }


// ***********************************************************************
// This function computes the total loads per processor for all the
// processors on the machine. The load is considered to be the number
// of grid points per processor for all serial and parallel array objects.
// ***********************************************************************

#if defined(SERIAL_APP)
intSerialArray
#else
intArray
#endif
Diagnostic_Manager::getProcessorLoads()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Diagnostic_Manager::getProcessorLoads() \n");
#endif

     int total = 0;
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
#if defined(SERIAL_APP)
          int length = SerialArray_Domain_Type::Array_Reference_Count_Array_Length;
#else
          int length = Array_Domain_Type::Array_Reference_Count_Array_Length;
#endif

       // (12/29/2000) Cannot make assertion unless array objects have been allocated
          if (length > 0)
             {
               APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray != NULL);
             }

          for (int i=0; i < length; i++)
             {
            // if (Diagnostic_Manager::diagnosticInfoArray[i] != NULL)
               if ( (Diagnostic_Manager::diagnosticInfoArray[i] != NULL) &&
                    (Diagnostic_Manager::diagnosticInfoArray[i]->getTypeCode() != APP_UNDEFINED_ELEMENT_TYPE) )
                  {
                    APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray[i] != NULL);
#if 0
                    printf ("found an array object! array id = %d \n",i);
#endif
                    int subtotal = Diagnostic_Manager::diagnosticInfoArray[i]->size();
#if 0
                    printf ("subtotal on current processor = %d \n",subtotal);
#endif
                    total += subtotal;
                  }
                 else
                  {
#if 0
                    printf ("No corresponding array for array id = %d \n",i);
#endif
                  }
             }
        }

#if 0
     printf ("total on current processor = %d \n",total);
#endif

#if defined(SERIAL_APP)
  // In the parallel environment at this point we only
  // have the total memory in use on this processor.

     int numberOfNodes = Communication_Manager::numberOfProcessors();

  // temp storage (required for Communication_Manager::fillProcessorArray)
     intSerialArray result (numberOfNodes);
     result = 0;

  // fill in the local totals each processor (into the processorArray)
     Communication_Manager::fillProcessorArray ( result.getDataPointer(), total );
#else
  // A++ case
     intArray result(1);
     result = total;
#endif

     return result;
   }

void
Diagnostic_Manager::displayProcessorLoads()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Diagnostic_Manager::displayProcessorLoads() \n");
#endif

     int numberOfNodes = Communication_Manager::numberOfProcessors();

     const intSerialArray & processorLoads = getProcessorLoads();

     int j = 0;

  // Turn OFF the mechanism that prints the node number before each output string!
     if (Communication_Manager::localProcessNumber() == 0)
        {
          Communication_Manager::setPrefixParallelPrintf(FALSE);
          printf ("***************************************************************** \n");
          printf ("     Display of information about loads for each processor \n");
          printf ("***************************************************************** \n");

          printf ("Distribution of data: \n");
          for (j = 0; j < numberOfNodes; j++)
             {
            // Later handle threads separately
            // printf ("Node %3d ",localBase(i),localBound(i));
               printf ("Node: %3d: ",j);

               printf ("%3d ",processorLoads(j));

               printf ("\n");
             }

          printf ("\n");
          printf ("\n");

       // Turn ON the mechanism that prints the node number before each output string!
          Communication_Manager::setPrefixParallelPrintf(TRUE);
        }
   }


int
Diagnostic_Manager::getnumberOfArraysConstantDimensionInUse( int dimension, int inputTypeCode )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Diagnostic_Manager::getTotalMemoryInUse() \n");
#endif

     int total = 0;
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
#if defined(SERIAL_APP)
          int length = SerialArray_Domain_Type::Array_Reference_Count_Array_Length;
#else
          int length = Array_Domain_Type::Array_Reference_Count_Array_Length;
#endif

          APP_ASSERT (Diagnostic_Manager::diagnosticInfoArray != NULL);
          for (int i=0; i < length; i++)
             {
	       if (Diagnostic_Manager::diagnosticInfoArray[i] != NULL)
                  {
                 // printf ("found an array object! array id = %d \n",i);
                    if (Diagnostic_Manager::diagnosticInfoArray[i]->getTypeCode() == inputTypeCode)
                       {
                         if (Diagnostic_Manager::diagnosticInfoArray[i]->numberOfDimensions() == dimension)
                            {
                              total++;
                            }
                       }
                  }
             }
        }

     return total;
   }

void
Diagnostic_Manager::getSizeOfClasses()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Diagnostic_Manager::getSizeOfClasses() \n");
#endif

     int distributionOverhead = 0;
     int total = 0;

     int sizeOfArrayDomain       = sizeof(Array_Domain_Type);
     total += sizeOfArrayDomain;

     printf ("Sizes of objects used within A++/P++ (in bytes): \n");
     printf ("     sizeOfArrayDomain              = %4d \n",sizeOfArrayDomain);

#if defined(PPP)
     int sizeOfSerialArrayDomain = sizeof(SerialArray_Domain_Type);
     printf ("     sizeOfSerialArrayDomain        = %4d \n",sizeOfSerialArrayDomain);

     total += sizeOfSerialArrayDomain;

     int sizeOfPartitioningType = sizeof(Partitioning_Type);
     printf ("     sizeOfPartitioningType         = %4d \n",sizeOfPartitioningType);
     total += sizeOfPartitioningType;

     int sizeOfInternalPartitioningType = sizeof(Internal_Partitioning_Type);
     printf ("     sizeOfInternalPartitioningType = %4d \n",sizeOfInternalPartitioningType);
     total += sizeOfInternalPartitioningType;

#if defined(USE_PADRE)
     int sizeOfPADRE_Descriptor     = sizeof(PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
     int sizeOfPADRE_Representation = sizeof(PADRE_Representation<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
     int sizeOfPADRE_Distribution   = sizeof(PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
#if 0
     // This should be implemented in PADRE.  We are not PARTI-aware here.
     int sizeOfPARTI_Descriptor     = sizeof(PARTI_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
     int sizeOfPARTI_Representation = sizeof(PARTI_Representation<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
     int sizeOfPARTI_Distribution   = sizeof(PARTI_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
#endif
     distributionOverhead += sizeOfPADRE_Descriptor + 
                  sizeOfPADRE_Representation + sizeOfPADRE_Distribution
#if 0
     // This should be implemented in PADRE.  We are not PARTI-aware here.
       + sizeOfPARTI_Descriptor + sizeOfPARTI_Representation + sizeOfPARTI_Distribution
#endif
       ;

     printf ("     sizeOfPADRE_Descriptor         = %4d \n",sizeOfPADRE_Descriptor);
     printf ("     sizeOfPADRE_Representation     = %4d \n",sizeOfPADRE_Representation);
     printf ("     sizeOfPADRE_Distribution       = %4d \n",sizeOfPADRE_Distribution);
#if 0
     // This should be implemented in PADRE.  We are not PARTI-aware here.
     printf ("     sizeOfPARTI_Descriptor         = %4d \n",sizeOfPARTI_Descriptor);
     printf ("     sizeOfPARTI_Representation     = %4d \n",sizeOfPARTI_Representation);
     printf ("     sizeOfPARTI_Distribution       = %4d \n",sizeOfPARTI_Distribution);
#endif
#else
  // need to estimate this better!
     distributionOverhead = 0;
#endif

     total += distributionOverhead;
#endif

     printf ("     total overhead per array       = %4d \n",total);
   }

int
Diagnostic_Manager::getMemoryOverhead()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Diagnostic_Manager::getTotalMemoryInUse() \n");
#endif

     int total = 0;
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
          int numberOfArrayDomains = getNumberOfArraysInUse() / 2;
          total += numberOfArrayDomains * sizeof(Array_Domain_Type);
#if defined(PPP)
          total += numberOfArrayDomains * sizeof(SerialArray_Domain_Type);
#if defined(USE_PADRE)
          total += numberOfArrayDomains * sizeof(PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
          total += numberOfArrayDomains * sizeof(PADRE_Representation<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
          total += numberOfArrayDomains * sizeof(PADRE_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
#if 0
	  // This should be implemented in PADRE.  We are not PARTI-aware here.
          total += numberOfArrayDomains * sizeof(PARTI_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
          total += numberOfArrayDomains * sizeof(PARTI_Representation<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
          total += numberOfArrayDomains * sizeof(PARTI_Distribution<BaseArray,Array_Domain_Type,SerialArray_Domain_Type>);
#endif
#else
       // need to estimate this better!
          total += 0;
#endif
#endif
        }

     return total;
   }

int
Diagnostic_Manager::getPurifyUnsupressedMemoryLeaks()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Diagnostic_Manager::getPurifyUnsupressedMemoryLeaks() \n");
#endif

     int total = 0;
#if defined(USE_PURIFY)
     if (purify_is_running() == TRUE)
        {
          printf ("A++/P++ is configured to use PURIFY and (purify_is_running() == TRUE) \n");
          total = purify_new_leaks();
        }
       else
        {
          printf ("A++/P++ is configured to use PURIFY but (purify_is_running() == FALSE) \n");
        }
#else
     printf ("A++/P++ is not configured to use purify! \n");
#endif

     return total;
   }

void
Diagnostic_Manager::displayPurify (const char* Label)
   {
  // This function displays the results of purify functions called directly using the purify API

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Inside of Diagnostic_Manager::displayPurify (%s) \n",Label);
#endif

#if defined(PPP)
     int numberOfNodes = Communication_Manager::numberOfProcessors();

  // local storage to accumulate data to prior to display
     intSerialArray dataTable = buildPurifyMap();

     int i = 0;
     int j = 0;

  // Turn OFF the mechanism that prints the node number before each output string!
     if (Communication_Manager::localProcessNumber() == 0)
        {
          Communication_Manager::setPrefixParallelPrintf(FALSE);
          printf ("***************************************************************** \n");
          printf ("Display of information about the global communication statistics  \n");
          printf ("***************************************************************** \n");

          printf ("\n");
          printf ("         ");
          printf ("   leaked  inuse");
          printf ("\n");
          for (j = 0; j < numberOfNodes; j++)
             {
            // Later handle threads separately
            // printf ("Node %3d ",localBase(i),localBound(i));
               printf ("Node: %3d",j);
               printf ("%7d  %7d",dataTable(j,0),dataTable(j,1));
               printf ("\n");
             }

          printf ("\n");
          printf ("\n");
          printf ("\n");

       // Turn ON the mechanism that prints the node number before each output string!
          Communication_Manager::setPrefixParallelPrintf(TRUE);
        }
#endif
   }

#if defined(PPP)
intSerialArray
#else
intArray
#endif
Diagnostic_Manager::buildPurifyMap ()
   {
  // This function builds a 3D array object representing the communication done on
  // an array object on each processor.  It requires communication
  // which is handled by the Communication_Manager::fillProcessorArray() function.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Diagnostic_Manager::buildProcessorMap() \n");
#endif

     int numberOfNodes = Communication_Manager::numberOfProcessors();

  // local storage to accumulate data to prior to display
  // Keep track of:
  //      number of messages sent
  //      number of messages recieved
  //      number of ghost boundary updates
  //      number of regular section transfers
     int numberOfDataPoints = 2;

#if defined(PPP)
     intSerialArray dataTable (numberOfNodes,numberOfDataPoints);
#else
     intArray dataTable (numberOfNodes,numberOfDataPoints);
#endif

     int i = 0;
     int j = 0;
     for (j = 0; j < numberOfNodes; j++)
        {
       // initialize the table
          for (i = 0; i < numberOfDataPoints; i++)
             {
               dataTable(j,i) = 0;
             }
        }

#if defined(PPP)
     Communication_Manager::Sync();

  // temp storage (required for Communication_Manager::fillProcessorArray)
     int* processorArray = new int [numberOfNodes];
     APP_ASSERT (processorArray != NULL);
 
  // initialize the newly allocated space
     for (j = 0; j < numberOfNodes; j++)
        {
          processorArray[j] = 0;
        }

     int memoryLeaked = -1;
     int memoryInUse  = -1;

#if defined(USE_PURIFY)
     if (purify_is_running() == TRUE)
        {
          printf ("A++/P++ is configured to use PURIFY and (purify_is_running() == TRUE) \n");
          memoryLeaked = purify_new_leaks();
          memoryInUse  = purify_new_inuse();
        }
       else
        {
          printf ("A++/P++ is configured to use PURIFY but (purify_is_running() == FALSE) \n");
        }
#else
     printf ("A++/P++ is not configured to use purify! \n");
#endif

  // fill in the local base and bound for each dimension (into the dataTable)
     Communication_Manager::fillProcessorArray ( processorArray, memoryLeaked );

     for (j = 0; j < numberOfNodes; j++)
        {
          dataTable(j,0) = processorArray[j];
        }

     Communication_Manager::fillProcessorArray ( processorArray, memoryInUse );

     for (j = 0; j < numberOfNodes; j++)
        {
          dataTable(j,1) = processorArray[j];
        }

  // now delete the allocated data (from above)
     delete processorArray;
     processorArray = NULL;
#endif

     return dataTable;
   }

int
Diagnostic_Manager::getTotalMemoryInUse()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Diagnostic_Manager::getTotalMemoryInUse() \n");
#endif

     int total = 0;
     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
          int arrayDataMemory = getTotalArrayMemoryInUse();
          int memoryOverhead  = getMemoryOverhead();

       // printf ("arrayDataMemory = %d  memoryOverhead = %d \n",arrayDataMemory,memoryOverhead);

          total += arrayDataMemory;
          total += memoryOverhead;
        }

     return total;
   }

void
Diagnostic_Manager::report()
   {
  // Communication_Manager::setPrefixParallelPrintf(FALSE);
  // Communication_Manager::setOutputProcessor(0);

     int i;
     int j;

  // Turn OFF the mechanism that prints the node number before each output string!
     if (Communication_Manager::localProcessNumber() == 0)
        {
          Communication_Manager::setPrefixParallelPrintf(FALSE);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Diagnostic_Manager::report() \n");
#endif

          printf ("\n\n");
          printf ("##########################################################################################################\n");
          printf ("########################################   DIAGNOSTICS REPORT   ##########################################\n");
          printf ("##########################################################################################################\n");

          printf ("A++/P++ version = %s \n",APP_version());

       // print out the sizes of objects used in A++/P++
          printf ("\n");
          getSizeOfClasses();

          Communication_Manager::setPrefixParallelPrintf(TRUE);
	}

     if (Diagnostic_Manager::getTrackArrayData() == TRUE)
        {
          int numberOfArraysInUse      = getNumberOfArraysInUse();
          int arrayMemoryInUse         = getTotalArrayMemoryInUse();
          int arrayMemoryOverheadInUse = getMemoryOverhead();
          int totalMemoryInUse = getTotalMemoryInUse();

       // Turn OFF the mechanism that prints the node number before each output string!
          if (Communication_Manager::localProcessNumber() == 0)
             {
               Communication_Manager::setPrefixParallelPrintf(FALSE);
               printf ("\n");
               printf ("Total number of array objects in use: %d \n",numberOfArraysInUse);

               printf ("\n");
               int totalNumberOfArrays [MAX_ARRAY_DIMENSION+1];
               for (j=0; j <= MAX_ARRAY_DIMENSION; j++)
                  {
                    totalNumberOfArrays [j] = 0;
                  }

               int numberOfArrays [MAX_ARRAY_DIMENSION+1][4];
               for (i=0; i <= MAX_ARRAY_DIMENSION; i++)
                  {
                    numberOfArrays[i][0] = getnumberOfArraysConstantDimensionInUse(i,APP_UNDEFINED_ELEMENT_TYPE);
                    numberOfArrays[i][1] = getnumberOfArraysConstantDimensionInUse(i,APP_INT_ELEMENT_TYPE);
                    numberOfArrays[i][2] = getnumberOfArraysConstantDimensionInUse(i,APP_FLOAT_ELEMENT_TYPE);
                    numberOfArrays[i][3] = getnumberOfArraysConstantDimensionInUse(i,APP_DOUBLE_ELEMENT_TYPE);

                 // There are 4 different types of array domains (or to be
                 // more exact: 1 array domain used for 4 different purposes).
                    for (j=0; j < 4; j++)
                       {
                         totalNumberOfArrays [i] += numberOfArrays[i][j];
                       }
                  }

               printf ("Number of arrays in use by dimension: \n");

               for (i=0; i <= MAX_ARRAY_DIMENSION; i++)
                  {
                    printf ("     Dimension %-2d: total = %-4d undefined entries = %-4d intArrays = %-4d floatArrays = %-4d doubleArrays = %-3d \n",
                         i,totalNumberOfArrays[i],numberOfArrays[i][0],numberOfArrays[i][1],
                         numberOfArrays[i][2],numberOfArrays[i][3]);
                  }

               printf ("\n");
               printf ("Memory In Use: \n");
               printf ("     total A++/P++ memory = %d  memory used by array data = %d  memory used by array overhead = %d \n",
                    totalMemoryInUse,arrayMemoryInUse,arrayMemoryOverheadInUse);

               printf ("\n");

               Communication_Manager::setPrefixParallelPrintf(TRUE);
	     }

       // getProcessorLoads().display("Processor Loads");
          displayProcessorLoads();
        }
       else
        {
          printf ("\n");
          printf ("Note: Diagnostic_Manager::setTrackArrayData(TRUE) was not called at startup so no tracking of array objects was done! \n");
          printf ("\n");
        }

     printf ("Parallel Messages: \n");
     displayCommunication ("Parallel Messages");

  // This uses purify to report the memory leaks
  // printf ("Memory Leaked (bytes) = %d \n",getPurifyUnsupressedMemoryLeaks());

     displayPurify ("Purify Messages");
     
     printf ("##########################################################################################################\n");
     printf ("##########################################################################################################\n");
     printf ("\n\n");
   }

void
Diagnostic_Manager::freeMemoryInUse()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Diagnostic_Manager::freeMemoryInUse() \n");
#endif

#if 1
  // APP_ASSERT (diagnosticInfoArray != NULL);
     if (diagnosticInfoArray != NULL)
          free (diagnosticInfoArray);

     diagnosticInfoArray = NULL;
#endif
   }

void
Diagnostic_Manager::test ( intArray    & X )
   {
  // Use member function of array class to simplify the implementation
     X.testArray();
   }

void
Diagnostic_Manager::test ( floatArray  & X )
   {
  // Use member function of array class to simplify the implementation
     X.testArray();
   }

void
Diagnostic_Manager::test ( doubleArray & X )
   {
  // Use member function of array class to simplify the implementation
     X.testArray();
   }


#if defined(PPP)
intSerialArray
#else
intArray
#endif
Diagnostic_Manager::buildCommunicationMap ()
   {
  // This function builds a 3D array object representing the communication done on
  // an array object on each processor.  It requires communication
  // which is handled by the Communication_Manager::fillProcessorArray() function.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Diagnostic_Manager::buildProcessorMap() \n");
#endif

     int numberOfNodes = Communication_Manager::numberOfProcessors();

  // local storage to accumulate data to prior to display
  // Keep track of:
  //      number of messages sent
  //      number of messages recieved
  //      number of ghost boundary updates
  //      number of regular section transfers
     int numberOfDataPoints = 4;
  
#if defined(PPP)
     intSerialArray dataTable (numberOfNodes,numberOfDataPoints);
#else
     intArray dataTable (numberOfNodes,numberOfDataPoints);
#endif

     int i = 0;
     int j = 0;
     for (j = 0; j < numberOfNodes; j++)
        {
       // initialize the table
          for (i = 0; i < numberOfDataPoints; i++)
             {
               dataTable(j,i) = 0;
             }
        }

#if defined(PPP)
     Communication_Manager::Sync();

  // temp storage (required for Communication_Manager::fillProcessorArray)
     int* processorArray = new int [numberOfNodes];
     APP_ASSERT (processorArray != NULL);

  // initialize the newly allocated space
     for (j = 0; j < numberOfNodes; j++)
        {
          processorArray[j] = 0;
        }

  // fill in the local base and bound for each dimension (into the dataTable)
     Communication_Manager::fillProcessorArray ( processorArray, Diagnostic_Manager::getNumberOfMessagesSent() );

     for (j = 0; j < numberOfNodes; j++)
        {
          dataTable(j,0) = processorArray[j];
        }
  
     Communication_Manager::fillProcessorArray ( processorArray, Diagnostic_Manager::getNumberOfMessagesReceived() );
     for (j = 0; j < numberOfNodes; j++)
        {
          dataTable(j,1) = processorArray[j];
        }

     Communication_Manager::fillProcessorArray ( processorArray, Diagnostic_Manager::getNumberOfGhostBoundaryUpdates() );
     for (j = 0; j < numberOfNodes; j++)
        {
          dataTable(j,2) = processorArray[j];
        }

     Communication_Manager::fillProcessorArray ( processorArray, Diagnostic_Manager::getNumberOfRegularSectionTransfers() );
     for (j = 0; j < numberOfNodes; j++)
        {
          dataTable(j,3) = processorArray[j];
        }

  // now delete the allocated data (from above)
     delete processorArray;
     processorArray = NULL;
#endif

     return dataTable;
   }

void
Diagnostic_Manager::displayCommunication (const char* Label)
   {
  // This function displays the communication associated all array objects
  // A similar function could be built which would display the communication 
  // for a single array object.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Inside of Diagnostic_Manager::displayCommunication (%s) \n",Label);
#endif

#if defined(PPP)
     int numberOfNodes = Communication_Manager::numberOfProcessors();

  // local storage to accumulate data to prior to display
     intSerialArray dataTable = buildCommunicationMap();

     int i = 0;
     int j = 0;

  // Turn OFF the mechanism that prints the node number before each output string!
     if (Communication_Manager::localProcessNumber() == 0)
        {
          Communication_Manager::setPrefixParallelPrintf(FALSE);
          printf ("***************************************************************** \n");
          printf ("Display of information about the global communication statistics  \n");
          printf ("***************************************************************** \n");

          printf ("\n");
          printf ("         ");
          printf ("   send  recieve  ghost-boundary-update regular-section-transfer");
          printf ("\n");
          for (j = 0; j < numberOfNodes; j++)
             {
            // Later handle threads separately
            // printf ("Node %3d ",localBase(i),localBound(i));
               printf ("Node: %3d",j);
               printf ("%7d  %7d                %7d                  %7d",
                    dataTable(j,0),dataTable(j,1),dataTable(j,2),dataTable(j,3));
               printf ("\n");
             }

          printf ("\n");
          printf ("\n");
          printf ("\n");

       // Turn ON the mechanism that prints the node number before each output string!
          Communication_Manager::setPrefixParallelPrintf(TRUE);
        }
#endif
   }

void
Diagnostic_Manager::resetCommunicationCounters()
   {
  // Raw counters for calls to MPI (csend/isend) and (crecv/irecv)
     numberOfMessagesSent = 0;
     numberOfMessagesRecieved = 0;

  // Total number of ghost boundary updates (for this local processor)
  // these can include multiple messages (depending on the dimensionality of the array objects)
     numberOfGhostBoundaryUpdates = 0;

  // Total number of regular section transfers (the most general communication mechanism)
  // (for this local processor) these can include multiple messages (depending on the 
  // dimensionality and the complexity of the distribution of the array objects)
     numberOfRegularSectionTransfers = 0;
   }


Optimization_Manager::Optimization_Manager ()
   {
     printf ("Called Optimization_Manager constructor! \n");
   }

Optimization_Manager::~Optimization_Manager ()
   {
     printf ("Called Optimization_Manager destructor! \n");
     printf ("Number_Of_Assignments_Optimized_Out = %d \n",Number_Of_Assignments_Optimized_Out); 
   }

void
Optimization_Manager::Exit_Virtual_Machine ()
   {
     printf ("Exiting P++ Virtual Machine! \n");

#if defined(SERIAL_APP)
  // Have to call MPI_Abort before calling MPI_Finalize (contained in Exit_Virtual_Machine())
  // MPI_Abort(MPI_COMM_WORLD,Diagnostic_Manager::APP_ErrorCode);

     delete Communication_Manager_Pointer;
     Communication_Manager_Pointer = NULL;
#endif
   }

void
Optimization_Manager::setOptimizedScalarIndexing ( On_Off_Type On_Off )
   {
     // This function does nothing since currently index
     // checking is turned on by a preprocessor statement!

     if ( (On_Off == ON) || (On_Off == On) || (On_Off == on) )
        {
          Optimize_Scalar_Indexing = TRUE;
        }
       else
        {
          Optimize_Scalar_Indexing = FALSE;
        }
   }

void
Optimization_Manager::setForceVSG_Update ( On_Off_Type On_Off )
   {
     // This function does nothing since currently index
     // checking is turned on by a preprocessor statement!

     if ( (On_Off == ON) || (On_Off == On) || (On_Off == on) )
        {
          ForceVSG_Update = TRUE;
        }
       else
        {
          ForceVSG_Update = FALSE;
        }
   }

void
Optimization_Manager::Initialize_Virtual_Machine ( 
     char* Application_Program_Name , 
     int & Number_Of_Processors,
     int & argc, char** & argv )
   {
  // This function is used as the interface to allow the user to specify
  // the number of processors used in the execution of P++ array statements.
  // A greater degree of specification (number of processors and number of 
  // processes (tasks)) may be allowed at a later date.

#if 1
#if defined(AUTO_INITIALIZE_APPLICATION_PATH_NAME)
     const int MAXIMUM_PATH_LENGTH = 500;
     char pathName[MAXIMUM_PATH_LENGTH];
     if (strcmp(Application_Program_Name,"") == 0)   // meaning equal to ""
        {
          printf ("No application program name specified (searching internally for correct name ...) \n");
#if defined(PVM)
          getcwd(pathName,MAXIMUM_PATH_LENGTH);
       // cout << "pathName=" << pathName << ", program name = " << *argv[0] << endl;
       // String programName = String(pathName)+String("/")+String(*argv[0]);
          char* tempString = strcat(pathName,"/");
          Application_Program_Name = strcat(tempString,*argv[0]);
#elif defined(MPI)
          pathName[0] = 0;
          Application_Program_Name = strcat(pathName,argv[0]);
#endif
          printf ("Application_Program_Name = %s \n",Application_Program_Name);
        }
       else
        {
          printf ("Application_Program_Name set to something (Application_Program_Name = %s) \n",Application_Program_Name);
        }
#endif
#endif

#if 1
  // While we debug the Overlap update mechanism we want to use the VSG update 
  // mechanism because it is more robust (even though it is slower -- requires more message passing)
     Optimization_Manager::setForceVSG_Update(On);
#endif

  // We want to test the APP_Unit_Range object to see if it was propoerly initialized
  // We can only test it if it appears in the user code.  I don't know why this is this way!
  // APP_Unit_Range.Test_Consistency("Test static initialization of APP_Unit_Range");

#if defined(SERIAL_APP)
     if (Communication_Manager_Pointer == NULL)
          Communication_Manager_Pointer = new Communication_Manager (Application_Program_Name,Number_Of_Processors,argc,argv);
     APP_ASSERT (Communication_Manager_Pointer != NULL);

#if defined(USE_PTHREADS) && defined(USE_PURIFY)
     static char processorNameString[128];
     int threadID = 0;

  // Build a string to use in purify messages
     sprintf (processorNameString,"P++ Processor %d Thread %d ",Communication_Manager::My_Process_Number,threadID);

  // Name the thread so that purify will specify the name of 
  // the processor and thread when using issuing messages
     pure_name_thread(processorNameString);

  // Put a message into the logfile (or in the viewer) to report the processor number and thread ID
     purify_printf ("A++/P++ using Purify: %s \n",processorNameString);
#endif

     printf ("\n");
     printf ("***************************************************** \n");
  // printf ("P++ Virtual Machine Initialized (Application_Program_Name = %s  Number_Of_Processors = %d) \n",
  //           Application_Program_Name,Number_Of_Processors);
     printf ("P++ Virtual Machine Initialized: \n");
     printf ("     Process Number                 = %d \n", Communication_Manager::My_Process_Number);

     printf ("     UNIX process ID                = %d \n",getpid());

     char namehost[128] = "gethostname() system function unavailable";

  // Call MPI function to get host name
     int hostNameLength = 0;
     MPI_Get_processor_name(namehost,&hostNameLength);
     namehost[hostNameLength] = NULL;

     printf ("     Process located on machine     = %s \n",namehost);
     printf ("     Number_Of_Processors           = %d \n", Communication_Manager::Number_Of_Processors);
#if defined(AUTO_INITIALIZE_APPLICATION_PATH_NAME)
     // ... memory leak, store this so it can be deleted ...
     char* temp_path = getcwd(NULL, 500);
     printf ("     Local directory (for file I/O) = %s  \n", temp_path);
     //printf ("     Local directory (for file I/O) = %s  \n", getcwd(NULL, 500));
     free (temp_path);
     temp_path = NULL;
     // delete temp_path;
#endif
     printf ("     Application_Program_Name       = %s \n", Application_Program_Name);

  // While we redebug the overlap update we want to make the VSG update the default.
     printf ("     Default communication model    = %s \n",
          (ForceVSG_Update) ? "VSG everywhere" : "Overlap where possible otherwise VSG");
     printf ("***************************************************** \n");
     printf ("\n");

  // These can't be initialized until after the number of processors is known
  // Internal_Partitioning_Type::DefaultStarting_Processor = 0;
  // Internal_Partitioning_Type::DefaultEnding_Processor   = Communication_Manager::Number_Of_Processors - 1;
     Internal_Partitioning_Type::setDefaultStartingAndEndingProcessorsForDistribution 
          (0,Communication_Manager::Number_Of_Processors - 1);

#else
  // Use these to avoid compiler warnings
     Number_Of_Processors     = 1;
     printf ("A++ Virtual Machine Initialized (Application_Program_Name = %s  Number_Of_Processors = %d) \n",
               Application_Program_Name,Number_Of_Processors);
#endif
   }

// *****************************************************************************
// *****************************************************************************
// ****************  MEMORY CLEANUP MEMBER FUNCTION DEFINITION  ****************
// *****************************************************************************
// *****************************************************************************

void
Optimization_Manager::freeMemoryInUse()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        printf ("Inside of Optimization_Manager::freeMemoryInUse() \n");
#endif

  // This function is useful in conjuction with the Purify (from Pure Software Inc.)
  // it frees memory allocated for use internally in A++ <type>$2Array objects.
  // This memory is used internally and is reported as "in use" by Purify
  // if it is not freed up using this function.  This function works with
  // similar functions for each A++ object to free up all of the A++ memory in
  // use internally.

#if defined(SERIAL_APP)

  // Comment out to test problem with mpi mpich error at termination
#if 0
     APP_ASSERT (Communication_Manager_Pointer != NULL);
     delete Communication_Manager_Pointer;

     Communication_Manager_Pointer = NULL;
#endif
#endif
   }



// The following function returns a bool so that it can be used in an enclosing conditional!
int
Optimization_Manager::Deferred_Evaluation_ON ( const Set_Of_Tasks & Deferred_Block )
   {
  // The value True is returned on the first invocation and false forever after
  // (unless a reinitialized Set_Of_Tasks object is used).

     if (APP_DEBUG > 3)
          printf ("Inside of Optimization_Manager::Deferred_Evaluation_ON! \n");

     int Return_Value = START_DEFERRED_EVALUATION;

  // printf ("Previous_Deferred_Evaluation_State = %d \n",Previous_Deferred_Evaluation_State);

  // if (Previous_Deferred_Evaluation_State == TRUE)
  //    {
  //      printf ("ERROR: in Optimization_Manager::Deferred_Evaluation_ON -- Must turn Deferred Evaluation OFF before reusing! \n");
  //      APP_ABORT();
  //    }

  // Previous_Deferred_Evaluation_State = Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION;

     if (Deferred_Block.Setup ())
        {
       // Since the block is setup we can skip around the defer evaluation block
       // by returning FALSE!

          if (APP_DEBUG > 3)
               printf ("Since the block is already setup we can skip around the deferred evaluation block by returning STOP_DEFERRED_EVALUATION! \n");

          Current_Deferred_Block_Pointer = &((Set_Of_Tasks &) Deferred_Block);
          Return_Value = STOP_DEFERRED_EVALUATION;
          Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION = FALSE;
       // Previous_Deferred_Evaluation_State = TRUE;
        }
       else
        {
       // This block must be evaluated (though the execution is deferred) to setup
       // the block (so we return TRUE).

          if (APP_DEBUG > 3)
               printf ("This block must be evaluated (though the execution is deferred) to setup the block (so we return TRUE) \n");

          Current_Deferred_Block_Pointer = &((Set_Of_Tasks &) Deferred_Block);
          Return_Value = START_DEFERRED_EVALUATION;
          Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION = TRUE;
       // Previous_Deferred_Evaluation_State = FALSE;
        }

     Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION = TRUE;

     return Return_Value;
   }

int Optimization_Manager::Deferred_Evaluation_OFF ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of Optimization_Manager::Deferred_Evaluation_OFF! \n");

  // error checking
  // if (Previous_Deferred_Evaluation_State == FALSE)
  //    {
  //      printf ("ERROR: in Optimization_Manager::Deferred_Evaluation_OFF -- Must turn Deferred Evaluation ON before using! \n");
  //      APP_ABORT();
  //    }

  // error checking
     if (Current_Deferred_Block_Pointer == NULL)
        {
          printf ("ERROR: Current_Deferred_Block_Pointer == NULL in Optimization_Manager::Deferred_Evaluation_OFF! \n");
          APP_ABORT();
        }

     if ( !Current_Deferred_Block_Pointer->Setup() )
        {
       // We need to turn this off before the execution of 
       // Optimize_Binary_Operators_To_Aggregate_Operators_In_Statement_List
       // otherwise the Expression_Tree_Node_Type default constructor!
          Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION = FALSE;

       // printf ("Current_Block NOT setup so we set it up! \n");
          long Start_Clock_0 = clock();
          *Current_Deferred_Block_Pointer = Array_Statement_Type::Find_Independent_Sets_Of_Statements () ;
          long End_Clock_0 = clock();
          printf ("Time for Array_Statement_Type::Find_Independent_Sets_Of_Statements () %ld \n", (End_Clock_0-Start_Clock_0) / 1000 );

       // (*Current_Deferred_Block_Pointer).Display();

       // long Start_Clock_1 = clock();
       // I suspect an error here!
       // Array_Statement_Type::Optimize_Assignment_In_Statement_List ();
       // long End_Clock_1 = clock();
       // printf ("Time for Array_Statement_Type::Optimize_Assignment_In_Statement_List () %ld \n", (End_Clock_1-Start_Clock_1) / 1000 );
 
          long Start_Clock_2 = clock();
          printf ("Aggregate operators not included in this release of A++ (call to form aggregate operators is commented out in optimization.C) \n");
       // Array_Statement_Type::Optimize_Binary_Operators_To_Aggregate_Operators_In_Statement_List ();
          long End_Clock_2 = clock();
          printf ("Time for Array_Statement_Type::Optimize_Binary_Operators_To_Aggregate_Operators_In_Statement_List () %ld \n", (End_Clock_2-Start_Clock_2) / 1000 );
        }
       else
        {
          if (APP_DEBUG > 0)
               printf ("We can skip reseting up the block since it was done previously! \n");
        }

  // Previous_Deferred_Evaluation_State = FALSE;

  // Set_Of_Tasks *Temp_Set_Pointer = Current_Deferred_Block_Pointer;
  // Reset pointer to NULL to get ready for next block!
     Current_Deferred_Block_Pointer     = NULL;

     Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION = FALSE;

  // Set these to null to allow other seperate lists of Expression_Tree_Node_Type object
     Expression_Tree_Node_Type::Head_Of_List = NULL;
     Expression_Tree_Node_Type::Tail_Of_List = NULL;

  // Set these to null to allow other seperate lists of Array_Statement_Type object
     Array_Statement_Type::Head_Of_List = NULL;
     Array_Statement_Type::Tail_Of_List = NULL;

  // return *Temp_Set_Pointer;
     return STOP_DEFERRED_EVALUATION;
   }


DiagnosticInfo::DiagnosticInfo ()
   {
  // arrayDomain = NULL;
     initialize();
   }

// Use this function to set or reset an existing DiagnosticInfo object
void
#if defined(SERIAL_APP)
DiagnosticInfo::initialize( const SerialArray_Domain_Type & X, int inputTypeCode )
#else
DiagnosticInfo::initialize( const Array_Domain_Type & X, int inputTypeCode )
#endif
   {
     typeCode = inputTypeCode;

  // Check for ZERO values to avoid division by ZERO
     bool XisNullArray = FALSE;
     int temp;
     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
          if (X.Size[temp] == 0)
	     XisNullArray = TRUE;

     if (XisNullArray)
        {
          for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
               arraySize[temp] = 0;
        }
       else
        {
          arraySize[0] = X.Size[0];
          for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
               arraySize[temp] = X.Size[temp] / X.Size[temp-1];
        }
   }



#if defined(SERIAL_APP)
DiagnosticInfo::DiagnosticInfo ( const SerialArray_Domain_Type & X )
#else
DiagnosticInfo::DiagnosticInfo ( const Array_Domain_Type & X )
#endif
   {
     initialize(X,APP_UNDEFINED_ELEMENT_TYPE);
   }

DiagnosticInfo::DiagnosticInfo ( const DiagnosticInfo & X )
   {
     operator=(X);
   }

DiagnosticInfo &
DiagnosticInfo::operator= ( const DiagnosticInfo & X )
   {
  // arrayDomain = X.arrayDomain;
     if (&X);  // use X to avoid compiler warning

     printf ("ERROR: DiagnosticInfo::operator=() not implemented! \n");
     return *this;
   }

void
DiagnosticInfo::initialize()
   {
  // arrayDomain = NULL;
  // typeCode = APP_UNDEFINED_ELEMENT_TYPE;
     typeCode = -1;  // APP_UNDEFINED_ELEMENT_TYPE;
     for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
          arraySize[i] = 0;
   }

int
DiagnosticInfo::size() const
   {
     int returnValue = 0;
     int temp;

  // if this is not a null array then start out with 1 instead of 0
     for (temp = 1; temp < MAX_ARRAY_DIMENSION; temp++)
          if (arraySize[temp] > 0)
               returnValue = 1;

     for (temp = 0; temp < MAX_ARRAY_DIMENSION; temp++)
          returnValue *= arraySize[temp];

     return returnValue;
   }

char*
DiagnosticInfo::getTypeString() const
   {
  // This function outputs a string based upon the value of the typeCode
     char* returnString = NULL;

     switch (typeCode)
        {
          case APP_INT_ELEMENT_TYPE:       returnString = "int";       break;
          case APP_FLOAT_ELEMENT_TYPE:     returnString = "float";     break;
          case APP_DOUBLE_ELEMENT_TYPE:    returnString = "double";    break;
          case APP_UNDEFINED_ELEMENT_TYPE: returnString = "undefined"; break;
          default:
               printf ("Error: typeCode = %-3d not defined! \n",typeCode);
               APP_ABORT();
        }

     return returnString;
   }

int
DiagnosticInfo::getTypeSize() const
   {
  // This function outputs the size of the element type based upon the value of the typeCode
     int returnSize = 0;

     switch (typeCode)
        {
          case APP_INT_ELEMENT_TYPE:       returnSize = sizeof(int);    break;
          case APP_FLOAT_ELEMENT_TYPE:     returnSize = sizeof(float);  break;
          case APP_DOUBLE_ELEMENT_TYPE:    returnSize = sizeof(double); break;
          case APP_UNDEFINED_ELEMENT_TYPE: returnSize = 0;              break;
          default:
               printf ("Error: typeCode = %-3d not defined! \n",typeCode);
               APP_ABORT();
        }

     return returnSize;
   }

void
DiagnosticInfo::setTypeCode(int inputTypeCode)
   {
     typeCode = inputTypeCode;
   }

int
DiagnosticInfo::getTypeCode() const
   {
     return typeCode;
   }

int
DiagnosticInfo::numberOfDimensions() const
   {
     int arrayDimension = MAX_ARRAY_DIMENSION;
     bool Done = FALSE;
     int j = MAX_ARRAY_DIMENSION-1;
     while ( (!Done) && (j >= 0) )
        {
          if (arraySize[j] <= ((j > 0) ? 1 : 0))
               arrayDimension--;
            else
               Done = TRUE;
          j--;
        }

     APP_ASSERT (arrayDimension >= 0);
     APP_ASSERT (arrayDimension <= MAX_ARRAY_DIMENSION);

     return arrayDimension;
   }




