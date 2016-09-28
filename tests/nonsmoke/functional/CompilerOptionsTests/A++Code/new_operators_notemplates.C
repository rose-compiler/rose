#include "A++.h"

















int  doubleArray::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray* doubleArray::Current_Link                      = NULL;

int doubleArray::Memory_Block_Index                = 0;

const int doubleArray::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray::operator new: Calling malloc because Size(%d) != sizeof(doubleArray)(%d) \n",Size,sizeof(doubleArray));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray)(%d) \n",Size,sizeof(doubleArray));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray) );
#else
               Current_Link = (doubleArray*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray::operator delete: Size(%d)  sizeof(doubleArray)(%d) \n",sizeOfObject,sizeof(doubleArray));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray)(%d) \n",sizeOfObject,sizeof(doubleArray));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray *New_Link = (doubleArray*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_Steal_Data::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_Steal_Data* doubleArray_Function_Steal_Data::Current_Link                      = NULL;

int doubleArray_Function_Steal_Data::Memory_Block_Index                = 0;

const int doubleArray_Function_Steal_Data::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_Steal_Data::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_Steal_Data::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_Steal_Data::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_Steal_Data))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_Steal_Data
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_Steal_Data::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_Steal_Data)(%d) \n",Size,sizeof(doubleArray_Function_Steal_Data));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_Steal_Data::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_Steal_Data)(%d) \n",Size,sizeof(doubleArray_Function_Steal_Data));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_Steal_Data*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_Steal_Data) );
#else
               Current_Link = (doubleArray_Function_Steal_Data*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_Steal_Data) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_Steal_Data* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_Steal_Data::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_Steal_Data::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_Steal_Data::operator delete: Size(%d)  sizeof(doubleArray_Function_Steal_Data)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_Steal_Data));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_Steal_Data))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_Steal_Data
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_Steal_Data::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_Steal_Data)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_Steal_Data));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_Steal_Data *New_Link = (doubleArray_Function_Steal_Data*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_Steal_Data::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_Steal_Data)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_Steal_Data));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_Steal_Data::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_Steal_Data::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_0::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_0* doubleArray_Function_0::Current_Link                      = NULL;

int doubleArray_Function_0::Memory_Block_Index                = 0;

const int doubleArray_Function_0::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_0::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_0::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_0::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_0))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_0
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_0::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_0)(%d) \n",Size,sizeof(doubleArray_Function_0));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_0::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_0)(%d) \n",Size,sizeof(doubleArray_Function_0));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_0*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_0) );
#else
               Current_Link = (doubleArray_Function_0*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_0) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_0* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_0::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_0::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_0::operator delete: Size(%d)  sizeof(doubleArray_Function_0)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_0));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_0))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_0
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_0::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_0)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_0));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_0 *New_Link = (doubleArray_Function_0*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_0::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_0)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_0));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_0::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_0::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_1::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_1* doubleArray_Function_1::Current_Link                      = NULL;

int doubleArray_Function_1::Memory_Block_Index                = 0;

const int doubleArray_Function_1::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_1::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_1::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_1::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_1))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_1
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_1::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_1)(%d) \n",Size,sizeof(doubleArray_Function_1));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_1::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_1)(%d) \n",Size,sizeof(doubleArray_Function_1));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_1*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_1) );
#else
               Current_Link = (doubleArray_Function_1*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_1) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_1* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_1::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_1::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_1::operator delete: Size(%d)  sizeof(doubleArray_Function_1)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_1));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_1))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_1
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_1::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_1)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_1));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_1 *New_Link = (doubleArray_Function_1*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_1::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_1)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_1));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_1::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_1::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_2::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_2* doubleArray_Function_2::Current_Link                      = NULL;

int doubleArray_Function_2::Memory_Block_Index                = 0;

const int doubleArray_Function_2::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_2::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_2::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_2::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_2))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_2
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_2::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_2)(%d) \n",Size,sizeof(doubleArray_Function_2));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_2::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_2)(%d) \n",Size,sizeof(doubleArray_Function_2));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_2*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_2) );
#else
               Current_Link = (doubleArray_Function_2*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_2) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_2* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_2::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_2::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_2::operator delete: Size(%d)  sizeof(doubleArray_Function_2)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_2));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_2))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_2
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_2::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_2)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_2));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_2 *New_Link = (doubleArray_Function_2*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_2::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_2)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_2));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_2::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_2::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_3::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_3* doubleArray_Function_3::Current_Link                      = NULL;

int doubleArray_Function_3::Memory_Block_Index                = 0;

const int doubleArray_Function_3::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_3::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_3::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_3::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_3))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_3
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_3::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_3)(%d) \n",Size,sizeof(doubleArray_Function_3));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_3::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_3)(%d) \n",Size,sizeof(doubleArray_Function_3));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_3*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_3) );
#else
               Current_Link = (doubleArray_Function_3*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_3) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_3* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_3::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_3::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_3::operator delete: Size(%d)  sizeof(doubleArray_Function_3)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_3));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_3))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_3
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_3::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_3)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_3));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_3 *New_Link = (doubleArray_Function_3*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_3::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_3)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_3));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_3::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_3::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_4::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_4* doubleArray_Function_4::Current_Link                      = NULL;

int doubleArray_Function_4::Memory_Block_Index                = 0;

const int doubleArray_Function_4::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_4::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_4::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_4::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_4))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_4
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_4::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_4)(%d) \n",Size,sizeof(doubleArray_Function_4));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_4::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_4)(%d) \n",Size,sizeof(doubleArray_Function_4));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_4*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_4) );
#else
               Current_Link = (doubleArray_Function_4*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_4) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_4* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_4::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_4::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_4::operator delete: Size(%d)  sizeof(doubleArray_Function_4)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_4));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_4))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_4
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_4::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_4)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_4));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_4 *New_Link = (doubleArray_Function_4*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_4::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_4)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_4));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_4::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_4::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_5::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_5* doubleArray_Function_5::Current_Link                      = NULL;

int doubleArray_Function_5::Memory_Block_Index                = 0;

const int doubleArray_Function_5::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_5::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_5::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_5::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_5))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_5
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_5::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_5)(%d) \n",Size,sizeof(doubleArray_Function_5));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_5::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_5)(%d) \n",Size,sizeof(doubleArray_Function_5));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_5*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_5) );
#else
               Current_Link = (doubleArray_Function_5*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_5) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_5* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_5::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_5::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_5::operator delete: Size(%d)  sizeof(doubleArray_Function_5)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_5));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_5))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_5
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_5::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_5)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_5));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_5 *New_Link = (doubleArray_Function_5*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_5::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_5)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_5));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_5::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_5::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_6::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_6* doubleArray_Function_6::Current_Link                      = NULL;

int doubleArray_Function_6::Memory_Block_Index                = 0;

const int doubleArray_Function_6::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_6::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_6::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_6::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_6))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_6
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_6::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_6)(%d) \n",Size,sizeof(doubleArray_Function_6));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_6::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_6)(%d) \n",Size,sizeof(doubleArray_Function_6));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_6*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_6) );
#else
               Current_Link = (doubleArray_Function_6*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_6) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_6* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_6::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_6::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_6::operator delete: Size(%d)  sizeof(doubleArray_Function_6)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_6));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_6))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_6
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_6::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_6)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_6));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_6 *New_Link = (doubleArray_Function_6*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_6::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_6)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_6));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_6::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_6::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_7::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_7* doubleArray_Function_7::Current_Link                      = NULL;

int doubleArray_Function_7::Memory_Block_Index                = 0;

const int doubleArray_Function_7::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_7::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_7::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_7::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_7))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_7
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_7::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_7)(%d) \n",Size,sizeof(doubleArray_Function_7));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_7::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_7)(%d) \n",Size,sizeof(doubleArray_Function_7));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_7*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_7) );
#else
               Current_Link = (doubleArray_Function_7*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_7) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_7* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_7::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_7::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_7::operator delete: Size(%d)  sizeof(doubleArray_Function_7)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_7));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_7))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_7
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_7::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_7)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_7));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_7 *New_Link = (doubleArray_Function_7*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_7::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_7)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_7));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_7::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_7::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


#ifndef INTARRAY
int  doubleArray_Function_8::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_8* doubleArray_Function_8::Current_Link                      = NULL;

int doubleArray_Function_8::Memory_Block_Index                = 0;

const int doubleArray_Function_8::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_8::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_8::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_8::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_8))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_8
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_8::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_8)(%d) \n",Size,sizeof(doubleArray_Function_8));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_8::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_8)(%d) \n",Size,sizeof(doubleArray_Function_8));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_8*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_8) );
#else
               Current_Link = (doubleArray_Function_8*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_8) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_8* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_8::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_8::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_8::operator delete: Size(%d)  sizeof(doubleArray_Function_8)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_8));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_8))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_8
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_8::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_8)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_8));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_8 *New_Link = (doubleArray_Function_8*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_8::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_8)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_8));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_8::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_8::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }

#endif

int  doubleArray_Function_9::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_9* doubleArray_Function_9::Current_Link                      = NULL;

int doubleArray_Function_9::Memory_Block_Index                = 0;

const int doubleArray_Function_9::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_9::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_9::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_9::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_9))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_9
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_9::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_9)(%d) \n",Size,sizeof(doubleArray_Function_9));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_9::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_9)(%d) \n",Size,sizeof(doubleArray_Function_9));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_9*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_9) );
#else
               Current_Link = (doubleArray_Function_9*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_9) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_9* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_9::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_9::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_9::operator delete: Size(%d)  sizeof(doubleArray_Function_9)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_9));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_9))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_9
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_9::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_9)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_9));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_9 *New_Link = (doubleArray_Function_9*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_9::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_9)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_9));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_9::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_9::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_11::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_11* doubleArray_Function_11::Current_Link                      = NULL;

int doubleArray_Function_11::Memory_Block_Index                = 0;

const int doubleArray_Function_11::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_11::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_11::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_11::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_11))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_11
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_11::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_11)(%d) \n",Size,sizeof(doubleArray_Function_11));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_11::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_11)(%d) \n",Size,sizeof(doubleArray_Function_11));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_11*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_11) );
#else
               Current_Link = (doubleArray_Function_11*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_11) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_11* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_11::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_11::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_11::operator delete: Size(%d)  sizeof(doubleArray_Function_11)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_11));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_11))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_11
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_11::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_11)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_11));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_11 *New_Link = (doubleArray_Function_11*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_11::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_11)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_11));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_11::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_11::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_12::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_12* doubleArray_Function_12::Current_Link                      = NULL;

int doubleArray_Function_12::Memory_Block_Index                = 0;

const int doubleArray_Function_12::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_12::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_12::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_12::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_12))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_12
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_12::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_12)(%d) \n",Size,sizeof(doubleArray_Function_12));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_12::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_12)(%d) \n",Size,sizeof(doubleArray_Function_12));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_12*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_12) );
#else
               Current_Link = (doubleArray_Function_12*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_12) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_12* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_12::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_12::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_12::operator delete: Size(%d)  sizeof(doubleArray_Function_12)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_12));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_12))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_12
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_12::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_12)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_12));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_12 *New_Link = (doubleArray_Function_12*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_12::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_12)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_12));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_12::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_12::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_14::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_14* doubleArray_Function_14::Current_Link                      = NULL;

int doubleArray_Function_14::Memory_Block_Index                = 0;

const int doubleArray_Function_14::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_14::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_14::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_14::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_14))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_14
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_14::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_14)(%d) \n",Size,sizeof(doubleArray_Function_14));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_14::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_14)(%d) \n",Size,sizeof(doubleArray_Function_14));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_14*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_14) );
#else
               Current_Link = (doubleArray_Function_14*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_14) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_14* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_14::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_14::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_14::operator delete: Size(%d)  sizeof(doubleArray_Function_14)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_14));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_14))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_14
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_14::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_14)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_14));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_14 *New_Link = (doubleArray_Function_14*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_14::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_14)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_14));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_14::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_14::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  doubleArray_Function_15::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_15* doubleArray_Function_15::Current_Link                      = NULL;

int doubleArray_Function_15::Memory_Block_Index                = 0;

const int doubleArray_Function_15::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_15::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_15::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_15::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_15))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_15
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_15::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_15)(%d) \n",Size,sizeof(doubleArray_Function_15));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_15::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_15)(%d) \n",Size,sizeof(doubleArray_Function_15));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_15*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_15) );
#else
               Current_Link = (doubleArray_Function_15*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_15) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_15* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_15::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_15::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_15::operator delete: Size(%d)  sizeof(doubleArray_Function_15)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_15));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_15))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_15
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_15::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_15)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_15));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_15 *New_Link = (doubleArray_Function_15*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_15::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_15)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_15));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_15::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_15::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
int  doubleArray_Function_16::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Function_16* doubleArray_Function_16::Current_Link                      = NULL;

int doubleArray_Function_16::Memory_Block_Index                = 0;

const int doubleArray_Function_16::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Function_16::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Function_16::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Function_16::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Function_16))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_16
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Function_16::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Function_16)(%d) \n",Size,sizeof(doubleArray_Function_16));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Function_16::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_16)(%d) \n",Size,sizeof(doubleArray_Function_16));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Function_16*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_16) );
#else
               Current_Link = (doubleArray_Function_16*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Function_16) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Function_16* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Function_16::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Function_16::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Function_16::operator delete: Size(%d)  sizeof(doubleArray_Function_16)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_16));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Function_16))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Function_16
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_16::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Function_16)(%d) \n",sizeOfObject,sizeof(doubleArray_Function_16));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Function_16 *New_Link = (doubleArray_Function_16*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Function_16::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Function_16)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Function_16));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Function_16::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Function_16::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }

#endif

int  doubleArray_Aggregate_Operator::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

doubleArray_Aggregate_Operator* doubleArray_Aggregate_Operator::Current_Link                      = NULL;

int doubleArray_Aggregate_Operator::Memory_Block_Index                = 0;

const int doubleArray_Aggregate_Operator::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *doubleArray_Aggregate_Operator::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *doubleArray_Aggregate_Operator::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call doubleArray_Aggregate_Operator::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(doubleArray_Aggregate_Operator))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Aggregate_Operator
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In doubleArray_Aggregate_Operator::operator new: Calling malloc because Size(%d) != sizeof(doubleArray_Aggregate_Operator)(%d) \n",Size,sizeof(doubleArray_Aggregate_Operator));

          return malloc(Size);
        }
       else
        {
       // printf ("In doubleArray_Aggregate_Operator::operator new: Using the pool mechanism Size(%d) == sizeof(doubleArray_Aggregate_Operator)(%d) \n",Size,sizeof(doubleArray_Aggregate_Operator));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (doubleArray_Aggregate_Operator*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Aggregate_Operator) );
#else
               Current_Link = (doubleArray_Aggregate_Operator*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(doubleArray_Aggregate_Operator) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     doubleArray_Aggregate_Operator* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from doubleArray_Aggregate_Operator::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void doubleArray_Aggregate_Operator::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In doubleArray_Aggregate_Operator::operator delete: Size(%d)  sizeof(doubleArray_Aggregate_Operator)(%d) \n",sizeOfObject,sizeof(doubleArray_Aggregate_Operator));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(doubleArray_Aggregate_Operator))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from doubleArray_Aggregate_Operator
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Aggregate_Operator::operator delete: Calling global delete (free) because Size(%d) != sizeof(doubleArray_Aggregate_Operator)(%d) \n",sizeOfObject,sizeof(doubleArray_Aggregate_Operator));
             }
#endif

          free(Pointer);
        }
       else
        {
          doubleArray_Aggregate_Operator *New_Link = (doubleArray_Aggregate_Operator*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In doubleArray_Aggregate_Operator::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(doubleArray_Aggregate_Operator)(%d) \n",Pointer,sizeOfObject,sizeof(doubleArray_Aggregate_Operator));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In doubleArray_Aggregate_Operator::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving doubleArray_Aggregate_Operator::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray* floatArray::Current_Link                      = NULL;

int floatArray::Memory_Block_Index                = 0;

const int floatArray::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray::operator new: Calling malloc because Size(%d) != sizeof(floatArray)(%d) \n",Size,sizeof(floatArray));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray)(%d) \n",Size,sizeof(floatArray));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray) );
#else
               Current_Link = (floatArray*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray::operator delete: Size(%d)  sizeof(floatArray)(%d) \n",sizeOfObject,sizeof(floatArray));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray)(%d) \n",sizeOfObject,sizeof(floatArray));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray *New_Link = (floatArray*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_Steal_Data::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_Steal_Data* floatArray_Function_Steal_Data::Current_Link                      = NULL;

int floatArray_Function_Steal_Data::Memory_Block_Index                = 0;

const int floatArray_Function_Steal_Data::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_Steal_Data::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_Steal_Data::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_Steal_Data::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_Steal_Data))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_Steal_Data
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_Steal_Data::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_Steal_Data)(%d) \n",Size,sizeof(floatArray_Function_Steal_Data));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_Steal_Data::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_Steal_Data)(%d) \n",Size,sizeof(floatArray_Function_Steal_Data));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_Steal_Data*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_Steal_Data) );
#else
               Current_Link = (floatArray_Function_Steal_Data*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_Steal_Data) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_Steal_Data* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_Steal_Data::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_Steal_Data::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_Steal_Data::operator delete: Size(%d)  sizeof(floatArray_Function_Steal_Data)(%d) \n",sizeOfObject,sizeof(floatArray_Function_Steal_Data));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_Steal_Data))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_Steal_Data
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_Steal_Data::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_Steal_Data)(%d) \n",sizeOfObject,sizeof(floatArray_Function_Steal_Data));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_Steal_Data *New_Link = (floatArray_Function_Steal_Data*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_Steal_Data::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_Steal_Data)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_Steal_Data));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_Steal_Data::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_Steal_Data::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_0::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_0* floatArray_Function_0::Current_Link                      = NULL;

int floatArray_Function_0::Memory_Block_Index                = 0;

const int floatArray_Function_0::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_0::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_0::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_0::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_0))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_0
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_0::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_0)(%d) \n",Size,sizeof(floatArray_Function_0));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_0::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_0)(%d) \n",Size,sizeof(floatArray_Function_0));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_0*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_0) );
#else
               Current_Link = (floatArray_Function_0*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_0) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_0* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_0::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_0::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_0::operator delete: Size(%d)  sizeof(floatArray_Function_0)(%d) \n",sizeOfObject,sizeof(floatArray_Function_0));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_0))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_0
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_0::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_0)(%d) \n",sizeOfObject,sizeof(floatArray_Function_0));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_0 *New_Link = (floatArray_Function_0*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_0::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_0)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_0));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_0::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_0::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_1::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_1* floatArray_Function_1::Current_Link                      = NULL;

int floatArray_Function_1::Memory_Block_Index                = 0;

const int floatArray_Function_1::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_1::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_1::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_1::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_1))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_1
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_1::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_1)(%d) \n",Size,sizeof(floatArray_Function_1));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_1::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_1)(%d) \n",Size,sizeof(floatArray_Function_1));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_1*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_1) );
#else
               Current_Link = (floatArray_Function_1*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_1) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_1* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_1::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_1::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_1::operator delete: Size(%d)  sizeof(floatArray_Function_1)(%d) \n",sizeOfObject,sizeof(floatArray_Function_1));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_1))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_1
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_1::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_1)(%d) \n",sizeOfObject,sizeof(floatArray_Function_1));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_1 *New_Link = (floatArray_Function_1*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_1::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_1)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_1));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_1::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_1::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_2::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_2* floatArray_Function_2::Current_Link                      = NULL;

int floatArray_Function_2::Memory_Block_Index                = 0;

const int floatArray_Function_2::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_2::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_2::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_2::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_2))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_2
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_2::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_2)(%d) \n",Size,sizeof(floatArray_Function_2));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_2::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_2)(%d) \n",Size,sizeof(floatArray_Function_2));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_2*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_2) );
#else
               Current_Link = (floatArray_Function_2*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_2) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_2* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_2::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_2::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_2::operator delete: Size(%d)  sizeof(floatArray_Function_2)(%d) \n",sizeOfObject,sizeof(floatArray_Function_2));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_2))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_2
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_2::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_2)(%d) \n",sizeOfObject,sizeof(floatArray_Function_2));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_2 *New_Link = (floatArray_Function_2*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_2::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_2)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_2));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_2::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_2::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_3::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_3* floatArray_Function_3::Current_Link                      = NULL;

int floatArray_Function_3::Memory_Block_Index                = 0;

const int floatArray_Function_3::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_3::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_3::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_3::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_3))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_3
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_3::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_3)(%d) \n",Size,sizeof(floatArray_Function_3));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_3::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_3)(%d) \n",Size,sizeof(floatArray_Function_3));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_3*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_3) );
#else
               Current_Link = (floatArray_Function_3*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_3) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_3* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_3::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_3::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_3::operator delete: Size(%d)  sizeof(floatArray_Function_3)(%d) \n",sizeOfObject,sizeof(floatArray_Function_3));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_3))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_3
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_3::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_3)(%d) \n",sizeOfObject,sizeof(floatArray_Function_3));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_3 *New_Link = (floatArray_Function_3*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_3::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_3)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_3));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_3::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_3::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_4::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_4* floatArray_Function_4::Current_Link                      = NULL;

int floatArray_Function_4::Memory_Block_Index                = 0;

const int floatArray_Function_4::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_4::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_4::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_4::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_4))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_4
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_4::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_4)(%d) \n",Size,sizeof(floatArray_Function_4));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_4::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_4)(%d) \n",Size,sizeof(floatArray_Function_4));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_4*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_4) );
#else
               Current_Link = (floatArray_Function_4*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_4) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_4* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_4::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_4::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_4::operator delete: Size(%d)  sizeof(floatArray_Function_4)(%d) \n",sizeOfObject,sizeof(floatArray_Function_4));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_4))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_4
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_4::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_4)(%d) \n",sizeOfObject,sizeof(floatArray_Function_4));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_4 *New_Link = (floatArray_Function_4*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_4::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_4)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_4));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_4::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_4::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_5::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_5* floatArray_Function_5::Current_Link                      = NULL;

int floatArray_Function_5::Memory_Block_Index                = 0;

const int floatArray_Function_5::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_5::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_5::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_5::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_5))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_5
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_5::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_5)(%d) \n",Size,sizeof(floatArray_Function_5));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_5::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_5)(%d) \n",Size,sizeof(floatArray_Function_5));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_5*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_5) );
#else
               Current_Link = (floatArray_Function_5*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_5) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_5* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_5::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_5::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_5::operator delete: Size(%d)  sizeof(floatArray_Function_5)(%d) \n",sizeOfObject,sizeof(floatArray_Function_5));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_5))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_5
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_5::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_5)(%d) \n",sizeOfObject,sizeof(floatArray_Function_5));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_5 *New_Link = (floatArray_Function_5*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_5::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_5)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_5));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_5::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_5::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_6::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_6* floatArray_Function_6::Current_Link                      = NULL;

int floatArray_Function_6::Memory_Block_Index                = 0;

const int floatArray_Function_6::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_6::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_6::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_6::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_6))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_6
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_6::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_6)(%d) \n",Size,sizeof(floatArray_Function_6));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_6::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_6)(%d) \n",Size,sizeof(floatArray_Function_6));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_6*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_6) );
#else
               Current_Link = (floatArray_Function_6*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_6) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_6* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_6::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_6::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_6::operator delete: Size(%d)  sizeof(floatArray_Function_6)(%d) \n",sizeOfObject,sizeof(floatArray_Function_6));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_6))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_6
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_6::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_6)(%d) \n",sizeOfObject,sizeof(floatArray_Function_6));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_6 *New_Link = (floatArray_Function_6*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_6::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_6)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_6));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_6::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_6::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_7::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_7* floatArray_Function_7::Current_Link                      = NULL;

int floatArray_Function_7::Memory_Block_Index                = 0;

const int floatArray_Function_7::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_7::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_7::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_7::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_7))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_7
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_7::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_7)(%d) \n",Size,sizeof(floatArray_Function_7));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_7::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_7)(%d) \n",Size,sizeof(floatArray_Function_7));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_7*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_7) );
#else
               Current_Link = (floatArray_Function_7*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_7) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_7* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_7::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_7::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_7::operator delete: Size(%d)  sizeof(floatArray_Function_7)(%d) \n",sizeOfObject,sizeof(floatArray_Function_7));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_7))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_7
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_7::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_7)(%d) \n",sizeOfObject,sizeof(floatArray_Function_7));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_7 *New_Link = (floatArray_Function_7*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_7::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_7)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_7));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_7::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_7::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


#ifndef INTARRAY
int  floatArray_Function_8::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_8* floatArray_Function_8::Current_Link                      = NULL;

int floatArray_Function_8::Memory_Block_Index                = 0;

const int floatArray_Function_8::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_8::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_8::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_8::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_8))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_8
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_8::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_8)(%d) \n",Size,sizeof(floatArray_Function_8));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_8::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_8)(%d) \n",Size,sizeof(floatArray_Function_8));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_8*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_8) );
#else
               Current_Link = (floatArray_Function_8*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_8) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_8* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_8::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_8::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_8::operator delete: Size(%d)  sizeof(floatArray_Function_8)(%d) \n",sizeOfObject,sizeof(floatArray_Function_8));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_8))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_8
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_8::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_8)(%d) \n",sizeOfObject,sizeof(floatArray_Function_8));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_8 *New_Link = (floatArray_Function_8*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_8::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_8)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_8));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_8::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_8::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }

#endif

int  floatArray_Function_9::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_9* floatArray_Function_9::Current_Link                      = NULL;

int floatArray_Function_9::Memory_Block_Index                = 0;

const int floatArray_Function_9::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_9::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_9::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_9::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_9))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_9
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_9::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_9)(%d) \n",Size,sizeof(floatArray_Function_9));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_9::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_9)(%d) \n",Size,sizeof(floatArray_Function_9));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_9*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_9) );
#else
               Current_Link = (floatArray_Function_9*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_9) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_9* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_9::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_9::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_9::operator delete: Size(%d)  sizeof(floatArray_Function_9)(%d) \n",sizeOfObject,sizeof(floatArray_Function_9));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_9))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_9
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_9::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_9)(%d) \n",sizeOfObject,sizeof(floatArray_Function_9));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_9 *New_Link = (floatArray_Function_9*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_9::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_9)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_9));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_9::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_9::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_11::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_11* floatArray_Function_11::Current_Link                      = NULL;

int floatArray_Function_11::Memory_Block_Index                = 0;

const int floatArray_Function_11::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_11::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_11::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_11::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_11))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_11
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_11::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_11)(%d) \n",Size,sizeof(floatArray_Function_11));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_11::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_11)(%d) \n",Size,sizeof(floatArray_Function_11));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_11*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_11) );
#else
               Current_Link = (floatArray_Function_11*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_11) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_11* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_11::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_11::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_11::operator delete: Size(%d)  sizeof(floatArray_Function_11)(%d) \n",sizeOfObject,sizeof(floatArray_Function_11));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_11))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_11
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_11::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_11)(%d) \n",sizeOfObject,sizeof(floatArray_Function_11));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_11 *New_Link = (floatArray_Function_11*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_11::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_11)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_11));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_11::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_11::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_12::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_12* floatArray_Function_12::Current_Link                      = NULL;

int floatArray_Function_12::Memory_Block_Index                = 0;

const int floatArray_Function_12::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_12::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_12::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_12::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_12))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_12
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_12::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_12)(%d) \n",Size,sizeof(floatArray_Function_12));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_12::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_12)(%d) \n",Size,sizeof(floatArray_Function_12));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_12*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_12) );
#else
               Current_Link = (floatArray_Function_12*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_12) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_12* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_12::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_12::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_12::operator delete: Size(%d)  sizeof(floatArray_Function_12)(%d) \n",sizeOfObject,sizeof(floatArray_Function_12));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_12))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_12
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_12::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_12)(%d) \n",sizeOfObject,sizeof(floatArray_Function_12));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_12 *New_Link = (floatArray_Function_12*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_12::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_12)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_12));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_12::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_12::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_14::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_14* floatArray_Function_14::Current_Link                      = NULL;

int floatArray_Function_14::Memory_Block_Index                = 0;

const int floatArray_Function_14::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_14::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_14::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_14::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_14))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_14
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_14::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_14)(%d) \n",Size,sizeof(floatArray_Function_14));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_14::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_14)(%d) \n",Size,sizeof(floatArray_Function_14));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_14*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_14) );
#else
               Current_Link = (floatArray_Function_14*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_14) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_14* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_14::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_14::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_14::operator delete: Size(%d)  sizeof(floatArray_Function_14)(%d) \n",sizeOfObject,sizeof(floatArray_Function_14));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_14))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_14
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_14::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_14)(%d) \n",sizeOfObject,sizeof(floatArray_Function_14));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_14 *New_Link = (floatArray_Function_14*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_14::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_14)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_14));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_14::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_14::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  floatArray_Function_15::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_15* floatArray_Function_15::Current_Link                      = NULL;

int floatArray_Function_15::Memory_Block_Index                = 0;

const int floatArray_Function_15::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_15::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_15::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_15::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_15))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_15
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_15::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_15)(%d) \n",Size,sizeof(floatArray_Function_15));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_15::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_15)(%d) \n",Size,sizeof(floatArray_Function_15));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_15*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_15) );
#else
               Current_Link = (floatArray_Function_15*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_15) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_15* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_15::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_15::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_15::operator delete: Size(%d)  sizeof(floatArray_Function_15)(%d) \n",sizeOfObject,sizeof(floatArray_Function_15));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_15))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_15
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_15::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_15)(%d) \n",sizeOfObject,sizeof(floatArray_Function_15));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_15 *New_Link = (floatArray_Function_15*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_15::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_15)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_15));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_15::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_15::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
int  floatArray_Function_16::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Function_16* floatArray_Function_16::Current_Link                      = NULL;

int floatArray_Function_16::Memory_Block_Index                = 0;

const int floatArray_Function_16::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Function_16::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Function_16::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Function_16::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Function_16))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_16
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Function_16::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Function_16)(%d) \n",Size,sizeof(floatArray_Function_16));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Function_16::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Function_16)(%d) \n",Size,sizeof(floatArray_Function_16));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Function_16*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_16) );
#else
               Current_Link = (floatArray_Function_16*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Function_16) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Function_16* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Function_16::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Function_16::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Function_16::operator delete: Size(%d)  sizeof(floatArray_Function_16)(%d) \n",sizeOfObject,sizeof(floatArray_Function_16));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Function_16))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Function_16
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_16::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Function_16)(%d) \n",sizeOfObject,sizeof(floatArray_Function_16));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Function_16 *New_Link = (floatArray_Function_16*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Function_16::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Function_16)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Function_16));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Function_16::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Function_16::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }

#endif

int  floatArray_Aggregate_Operator::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

floatArray_Aggregate_Operator* floatArray_Aggregate_Operator::Current_Link                      = NULL;

int floatArray_Aggregate_Operator::Memory_Block_Index                = 0;

const int floatArray_Aggregate_Operator::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *floatArray_Aggregate_Operator::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *floatArray_Aggregate_Operator::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call floatArray_Aggregate_Operator::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(floatArray_Aggregate_Operator))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Aggregate_Operator
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In floatArray_Aggregate_Operator::operator new: Calling malloc because Size(%d) != sizeof(floatArray_Aggregate_Operator)(%d) \n",Size,sizeof(floatArray_Aggregate_Operator));

          return malloc(Size);
        }
       else
        {
       // printf ("In floatArray_Aggregate_Operator::operator new: Using the pool mechanism Size(%d) == sizeof(floatArray_Aggregate_Operator)(%d) \n",Size,sizeof(floatArray_Aggregate_Operator));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (floatArray_Aggregate_Operator*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Aggregate_Operator) );
#else
               Current_Link = (floatArray_Aggregate_Operator*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(floatArray_Aggregate_Operator) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     floatArray_Aggregate_Operator* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from floatArray_Aggregate_Operator::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void floatArray_Aggregate_Operator::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In floatArray_Aggregate_Operator::operator delete: Size(%d)  sizeof(floatArray_Aggregate_Operator)(%d) \n",sizeOfObject,sizeof(floatArray_Aggregate_Operator));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(floatArray_Aggregate_Operator))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from floatArray_Aggregate_Operator
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Aggregate_Operator::operator delete: Calling global delete (free) because Size(%d) != sizeof(floatArray_Aggregate_Operator)(%d) \n",sizeOfObject,sizeof(floatArray_Aggregate_Operator));
             }
#endif

          free(Pointer);
        }
       else
        {
          floatArray_Aggregate_Operator *New_Link = (floatArray_Aggregate_Operator*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In floatArray_Aggregate_Operator::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(floatArray_Aggregate_Operator)(%d) \n",Pointer,sizeOfObject,sizeof(floatArray_Aggregate_Operator));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In floatArray_Aggregate_Operator::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving floatArray_Aggregate_Operator::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }



#define INTARRAY
int  intArray::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray* intArray::Current_Link                      = NULL;

int intArray::Memory_Block_Index                = 0;

const int intArray::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray::operator new: Calling malloc because Size(%d) != sizeof(intArray)(%d) \n",Size,sizeof(intArray));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray::operator new: Using the pool mechanism Size(%d) == sizeof(intArray)(%d) \n",Size,sizeof(intArray));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray) );
#else
               Current_Link = (intArray*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray::operator delete: Size(%d)  sizeof(intArray)(%d) \n",sizeOfObject,sizeof(intArray));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray)(%d) \n",sizeOfObject,sizeof(intArray));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray *New_Link = (intArray*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray)(%d) \n",Pointer,sizeOfObject,sizeof(intArray));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_Steal_Data::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_Steal_Data* intArray_Function_Steal_Data::Current_Link                      = NULL;

int intArray_Function_Steal_Data::Memory_Block_Index                = 0;

const int intArray_Function_Steal_Data::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_Steal_Data::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_Steal_Data::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_Steal_Data::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_Steal_Data))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_Steal_Data
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_Steal_Data::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_Steal_Data)(%d) \n",Size,sizeof(intArray_Function_Steal_Data));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_Steal_Data::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_Steal_Data)(%d) \n",Size,sizeof(intArray_Function_Steal_Data));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_Steal_Data*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_Steal_Data) );
#else
               Current_Link = (intArray_Function_Steal_Data*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_Steal_Data) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_Steal_Data* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_Steal_Data::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_Steal_Data::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_Steal_Data::operator delete: Size(%d)  sizeof(intArray_Function_Steal_Data)(%d) \n",sizeOfObject,sizeof(intArray_Function_Steal_Data));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_Steal_Data))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_Steal_Data
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_Steal_Data::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_Steal_Data)(%d) \n",sizeOfObject,sizeof(intArray_Function_Steal_Data));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_Steal_Data *New_Link = (intArray_Function_Steal_Data*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_Steal_Data::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_Steal_Data)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_Steal_Data));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_Steal_Data::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_Steal_Data::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_0::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_0* intArray_Function_0::Current_Link                      = NULL;

int intArray_Function_0::Memory_Block_Index                = 0;

const int intArray_Function_0::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_0::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_0::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_0::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_0))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_0
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_0::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_0)(%d) \n",Size,sizeof(intArray_Function_0));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_0::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_0)(%d) \n",Size,sizeof(intArray_Function_0));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_0*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_0) );
#else
               Current_Link = (intArray_Function_0*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_0) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_0* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_0::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_0::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_0::operator delete: Size(%d)  sizeof(intArray_Function_0)(%d) \n",sizeOfObject,sizeof(intArray_Function_0));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_0))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_0
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_0::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_0)(%d) \n",sizeOfObject,sizeof(intArray_Function_0));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_0 *New_Link = (intArray_Function_0*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_0::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_0)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_0));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_0::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_0::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_1::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_1* intArray_Function_1::Current_Link                      = NULL;

int intArray_Function_1::Memory_Block_Index                = 0;

const int intArray_Function_1::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_1::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_1::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_1::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_1))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_1
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_1::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_1)(%d) \n",Size,sizeof(intArray_Function_1));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_1::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_1)(%d) \n",Size,sizeof(intArray_Function_1));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_1*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_1) );
#else
               Current_Link = (intArray_Function_1*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_1) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_1* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_1::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_1::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_1::operator delete: Size(%d)  sizeof(intArray_Function_1)(%d) \n",sizeOfObject,sizeof(intArray_Function_1));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_1))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_1
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_1::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_1)(%d) \n",sizeOfObject,sizeof(intArray_Function_1));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_1 *New_Link = (intArray_Function_1*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_1::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_1)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_1));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_1::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_1::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_2::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_2* intArray_Function_2::Current_Link                      = NULL;

int intArray_Function_2::Memory_Block_Index                = 0;

const int intArray_Function_2::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_2::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_2::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_2::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_2))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_2
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_2::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_2)(%d) \n",Size,sizeof(intArray_Function_2));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_2::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_2)(%d) \n",Size,sizeof(intArray_Function_2));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_2*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_2) );
#else
               Current_Link = (intArray_Function_2*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_2) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_2* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_2::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_2::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_2::operator delete: Size(%d)  sizeof(intArray_Function_2)(%d) \n",sizeOfObject,sizeof(intArray_Function_2));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_2))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_2
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_2::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_2)(%d) \n",sizeOfObject,sizeof(intArray_Function_2));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_2 *New_Link = (intArray_Function_2*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_2::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_2)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_2));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_2::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_2::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_3::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_3* intArray_Function_3::Current_Link                      = NULL;

int intArray_Function_3::Memory_Block_Index                = 0;

const int intArray_Function_3::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_3::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_3::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_3::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_3))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_3
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_3::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_3)(%d) \n",Size,sizeof(intArray_Function_3));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_3::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_3)(%d) \n",Size,sizeof(intArray_Function_3));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_3*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_3) );
#else
               Current_Link = (intArray_Function_3*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_3) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_3* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_3::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_3::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_3::operator delete: Size(%d)  sizeof(intArray_Function_3)(%d) \n",sizeOfObject,sizeof(intArray_Function_3));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_3))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_3
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_3::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_3)(%d) \n",sizeOfObject,sizeof(intArray_Function_3));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_3 *New_Link = (intArray_Function_3*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_3::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_3)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_3));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_3::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_3::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_4::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_4* intArray_Function_4::Current_Link                      = NULL;

int intArray_Function_4::Memory_Block_Index                = 0;

const int intArray_Function_4::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_4::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_4::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_4::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_4))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_4
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_4::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_4)(%d) \n",Size,sizeof(intArray_Function_4));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_4::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_4)(%d) \n",Size,sizeof(intArray_Function_4));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_4*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_4) );
#else
               Current_Link = (intArray_Function_4*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_4) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_4* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_4::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_4::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_4::operator delete: Size(%d)  sizeof(intArray_Function_4)(%d) \n",sizeOfObject,sizeof(intArray_Function_4));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_4))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_4
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_4::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_4)(%d) \n",sizeOfObject,sizeof(intArray_Function_4));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_4 *New_Link = (intArray_Function_4*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_4::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_4)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_4));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_4::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_4::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_5::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_5* intArray_Function_5::Current_Link                      = NULL;

int intArray_Function_5::Memory_Block_Index                = 0;

const int intArray_Function_5::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_5::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_5::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_5::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_5))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_5
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_5::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_5)(%d) \n",Size,sizeof(intArray_Function_5));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_5::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_5)(%d) \n",Size,sizeof(intArray_Function_5));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_5*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_5) );
#else
               Current_Link = (intArray_Function_5*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_5) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_5* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_5::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_5::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_5::operator delete: Size(%d)  sizeof(intArray_Function_5)(%d) \n",sizeOfObject,sizeof(intArray_Function_5));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_5))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_5
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_5::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_5)(%d) \n",sizeOfObject,sizeof(intArray_Function_5));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_5 *New_Link = (intArray_Function_5*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_5::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_5)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_5));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_5::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_5::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_6::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_6* intArray_Function_6::Current_Link                      = NULL;

int intArray_Function_6::Memory_Block_Index                = 0;

const int intArray_Function_6::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_6::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_6::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_6::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_6))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_6
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_6::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_6)(%d) \n",Size,sizeof(intArray_Function_6));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_6::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_6)(%d) \n",Size,sizeof(intArray_Function_6));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_6*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_6) );
#else
               Current_Link = (intArray_Function_6*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_6) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_6* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_6::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_6::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_6::operator delete: Size(%d)  sizeof(intArray_Function_6)(%d) \n",sizeOfObject,sizeof(intArray_Function_6));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_6))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_6
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_6::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_6)(%d) \n",sizeOfObject,sizeof(intArray_Function_6));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_6 *New_Link = (intArray_Function_6*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_6::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_6)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_6));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_6::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_6::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_7::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_7* intArray_Function_7::Current_Link                      = NULL;

int intArray_Function_7::Memory_Block_Index                = 0;

const int intArray_Function_7::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_7::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_7::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_7::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_7))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_7
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_7::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_7)(%d) \n",Size,sizeof(intArray_Function_7));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_7::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_7)(%d) \n",Size,sizeof(intArray_Function_7));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_7*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_7) );
#else
               Current_Link = (intArray_Function_7*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_7) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_7* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_7::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_7::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_7::operator delete: Size(%d)  sizeof(intArray_Function_7)(%d) \n",sizeOfObject,sizeof(intArray_Function_7));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_7))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_7
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_7::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_7)(%d) \n",sizeOfObject,sizeof(intArray_Function_7));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_7 *New_Link = (intArray_Function_7*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_7::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_7)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_7));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_7::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_7::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


#ifndef INTARRAY
int  intArray_Function_8::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_8* intArray_Function_8::Current_Link                      = NULL;

int intArray_Function_8::Memory_Block_Index                = 0;

const int intArray_Function_8::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_8::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_8::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_8::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_8))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_8
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_8::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_8)(%d) \n",Size,sizeof(intArray_Function_8));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_8::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_8)(%d) \n",Size,sizeof(intArray_Function_8));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_8*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_8) );
#else
               Current_Link = (intArray_Function_8*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_8) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_8* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_8::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_8::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_8::operator delete: Size(%d)  sizeof(intArray_Function_8)(%d) \n",sizeOfObject,sizeof(intArray_Function_8));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_8))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_8
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_8::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_8)(%d) \n",sizeOfObject,sizeof(intArray_Function_8));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_8 *New_Link = (intArray_Function_8*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_8::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_8)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_8));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_8::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_8::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }

#endif

int  intArray_Function_9::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_9* intArray_Function_9::Current_Link                      = NULL;

int intArray_Function_9::Memory_Block_Index                = 0;

const int intArray_Function_9::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_9::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_9::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_9::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_9))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_9
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_9::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_9)(%d) \n",Size,sizeof(intArray_Function_9));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_9::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_9)(%d) \n",Size,sizeof(intArray_Function_9));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_9*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_9) );
#else
               Current_Link = (intArray_Function_9*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_9) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_9* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_9::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_9::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_9::operator delete: Size(%d)  sizeof(intArray_Function_9)(%d) \n",sizeOfObject,sizeof(intArray_Function_9));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_9))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_9
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_9::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_9)(%d) \n",sizeOfObject,sizeof(intArray_Function_9));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_9 *New_Link = (intArray_Function_9*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_9::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_9)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_9));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_9::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_9::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_11::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_11* intArray_Function_11::Current_Link                      = NULL;

int intArray_Function_11::Memory_Block_Index                = 0;

const int intArray_Function_11::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_11::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_11::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_11::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_11))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_11
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_11::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_11)(%d) \n",Size,sizeof(intArray_Function_11));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_11::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_11)(%d) \n",Size,sizeof(intArray_Function_11));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_11*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_11) );
#else
               Current_Link = (intArray_Function_11*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_11) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_11* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_11::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_11::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_11::operator delete: Size(%d)  sizeof(intArray_Function_11)(%d) \n",sizeOfObject,sizeof(intArray_Function_11));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_11))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_11
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_11::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_11)(%d) \n",sizeOfObject,sizeof(intArray_Function_11));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_11 *New_Link = (intArray_Function_11*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_11::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_11)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_11));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_11::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_11::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_12::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_12* intArray_Function_12::Current_Link                      = NULL;

int intArray_Function_12::Memory_Block_Index                = 0;

const int intArray_Function_12::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_12::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_12::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_12::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_12))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_12
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_12::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_12)(%d) \n",Size,sizeof(intArray_Function_12));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_12::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_12)(%d) \n",Size,sizeof(intArray_Function_12));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_12*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_12) );
#else
               Current_Link = (intArray_Function_12*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_12) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_12* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_12::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_12::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_12::operator delete: Size(%d)  sizeof(intArray_Function_12)(%d) \n",sizeOfObject,sizeof(intArray_Function_12));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_12))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_12
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_12::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_12)(%d) \n",sizeOfObject,sizeof(intArray_Function_12));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_12 *New_Link = (intArray_Function_12*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_12::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_12)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_12));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_12::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_12::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_14::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_14* intArray_Function_14::Current_Link                      = NULL;

int intArray_Function_14::Memory_Block_Index                = 0;

const int intArray_Function_14::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_14::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_14::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_14::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_14))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_14
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_14::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_14)(%d) \n",Size,sizeof(intArray_Function_14));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_14::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_14)(%d) \n",Size,sizeof(intArray_Function_14));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_14*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_14) );
#else
               Current_Link = (intArray_Function_14*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_14) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_14* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_14::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_14::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_14::operator delete: Size(%d)  sizeof(intArray_Function_14)(%d) \n",sizeOfObject,sizeof(intArray_Function_14));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_14))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_14
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_14::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_14)(%d) \n",sizeOfObject,sizeof(intArray_Function_14));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_14 *New_Link = (intArray_Function_14*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_14::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_14)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_14));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_14::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_14::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


int  intArray_Function_15::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_15* intArray_Function_15::Current_Link                      = NULL;

int intArray_Function_15::Memory_Block_Index                = 0;

const int intArray_Function_15::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_15::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_15::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_15::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_15))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_15
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_15::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_15)(%d) \n",Size,sizeof(intArray_Function_15));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_15::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_15)(%d) \n",Size,sizeof(intArray_Function_15));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_15*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_15) );
#else
               Current_Link = (intArray_Function_15*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_15) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_15* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_15::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_15::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_15::operator delete: Size(%d)  sizeof(intArray_Function_15)(%d) \n",sizeOfObject,sizeof(intArray_Function_15));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_15))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_15
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_15::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_15)(%d) \n",sizeOfObject,sizeof(intArray_Function_15));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_15 *New_Link = (intArray_Function_15*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_15::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_15)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_15));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_15::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_15::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
int  intArray_Function_16::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Function_16* intArray_Function_16::Current_Link                      = NULL;

int intArray_Function_16::Memory_Block_Index                = 0;

const int intArray_Function_16::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Function_16::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Function_16::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Function_16::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Function_16))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_16
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Function_16::operator new: Calling malloc because Size(%d) != sizeof(intArray_Function_16)(%d) \n",Size,sizeof(intArray_Function_16));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Function_16::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Function_16)(%d) \n",Size,sizeof(intArray_Function_16));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Function_16*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_16) );
#else
               Current_Link = (intArray_Function_16*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Function_16) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Function_16* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Function_16::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Function_16::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Function_16::operator delete: Size(%d)  sizeof(intArray_Function_16)(%d) \n",sizeOfObject,sizeof(intArray_Function_16));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Function_16))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Function_16
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_16::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Function_16)(%d) \n",sizeOfObject,sizeof(intArray_Function_16));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Function_16 *New_Link = (intArray_Function_16*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Function_16::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Function_16)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Function_16));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Function_16::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Function_16::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }

#endif

int  intArray_Aggregate_Operator::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

intArray_Aggregate_Operator* intArray_Aggregate_Operator::Current_Link                      = NULL;

int intArray_Aggregate_Operator::Memory_Block_Index                = 0;

const int intArray_Aggregate_Operator::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *intArray_Aggregate_Operator::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *intArray_Aggregate_Operator::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call intArray_Aggregate_Operator::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(intArray_Aggregate_Operator))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Aggregate_Operator
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In intArray_Aggregate_Operator::operator new: Calling malloc because Size(%d) != sizeof(intArray_Aggregate_Operator)(%d) \n",Size,sizeof(intArray_Aggregate_Operator));

          return malloc(Size);
        }
       else
        {
       // printf ("In intArray_Aggregate_Operator::operator new: Using the pool mechanism Size(%d) == sizeof(intArray_Aggregate_Operator)(%d) \n",Size,sizeof(intArray_Aggregate_Operator));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (intArray_Aggregate_Operator*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Aggregate_Operator) );
#else
               Current_Link = (intArray_Aggregate_Operator*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(intArray_Aggregate_Operator) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     intArray_Aggregate_Operator* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from intArray_Aggregate_Operator::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void intArray_Aggregate_Operator::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In intArray_Aggregate_Operator::operator delete: Size(%d)  sizeof(intArray_Aggregate_Operator)(%d) \n",sizeOfObject,sizeof(intArray_Aggregate_Operator));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(intArray_Aggregate_Operator))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from intArray_Aggregate_Operator
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Aggregate_Operator::operator delete: Calling global delete (free) because Size(%d) != sizeof(intArray_Aggregate_Operator)(%d) \n",sizeOfObject,sizeof(intArray_Aggregate_Operator));
             }
#endif

          free(Pointer);
        }
       else
        {
          intArray_Aggregate_Operator *New_Link = (intArray_Aggregate_Operator*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In intArray_Aggregate_Operator::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(intArray_Aggregate_Operator)(%d) \n",Pointer,sizeOfObject,sizeof(intArray_Aggregate_Operator));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In intArray_Aggregate_Operator::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving intArray_Aggregate_Operator::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }


#undef INTARRAY

/* New _ Operator _ For _ Array _ Descriptor () */

int Array_Statement_Type::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

Array_Statement_Type* Array_Statement_Type::Current_Link                      = NULL;

int Array_Statement_Type::Memory_Block_Index                = 0;

const int Array_Statement_Type::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *Array_Statement_Type::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *Array_Statement_Type::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call Array_Statement_Type::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(Array_Statement_Type))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from Array_Statement_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In Array_Statement_Type::operator new: Calling malloc because Size(%d) != sizeof(Array_Statement_Type)(%d) \n",Size,sizeof(Array_Statement_Type));

          return malloc(Size);
        }
       else
        {
       // printf ("In Array_Statement_Type::operator new: Using the pool mechanism Size(%d) == sizeof(Array_Statement_Type)(%d) \n",Size,sizeof(Array_Statement_Type));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (Array_Statement_Type*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(Array_Statement_Type) );
#else
               Current_Link = (Array_Statement_Type*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(Array_Statement_Type) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     Array_Statement_Type* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from Array_Statement_Type::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void Array_Statement_Type::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In Array_Statement_Type::operator delete: Size(%d)  sizeof(Array_Statement_Type)(%d) \n",sizeOfObject,sizeof(Array_Statement_Type));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(Array_Statement_Type))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from Array_Statement_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In Array_Statement_Type::operator delete: Calling global delete (free) because Size(%d) != sizeof(Array_Statement_Type)(%d) \n",sizeOfObject,sizeof(Array_Statement_Type));
             }
#endif

          free(Pointer);
        }
       else
        {
          Array_Statement_Type *New_Link = (Array_Statement_Type*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In Array_Statement_Type::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(Array_Statement_Type)(%d) \n",Pointer,sizeOfObject,sizeof(Array_Statement_Type));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In Array_Statement_Type::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving Array_Statement_Type::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }



int Internal_Partitioning_Type::CLASS_ALLOCATION_POOL_SIZE = DEFAULT_CLASS_ALLOCATION_POOL_SIZE;
/* Static variable */

Internal_Partitioning_Type* Internal_Partitioning_Type::Current_Link                      = NULL;

int Internal_Partitioning_Type::Memory_Block_Index                = 0;

const int Internal_Partitioning_Type::Max_Number_Of_Memory_Blocks = MAX_NUMBER_OF_MEMORY_BLOCKS;

unsigned char *Internal_Partitioning_Type::Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];

#define USE_CPP_NEW_DELETE_OPERATORS FALSE

#ifndef INLINE_FUNCTIONS

void *Internal_Partitioning_Type::operator new ( size_t Size )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Call Internal_Partitioning_Type::operator new! Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     return malloc(Size);
#else
  // Because of the way the size of the memory blocks doubles in size
  // for each proceeding memory block 100 is a good limit for the size of
  // the memory block list!

  // These were taken out to allow the new operator to be inlined!
  // const int Max_Number_Of_Memory_Blocks = 1000;
  // static unsigned char *Memory_Block_List [Max_Number_Of_Memory_Blocks];
  // static int Memory_Block_Index = 0;

     if (Size != sizeof(Internal_Partitioning_Type))
        {
       // Bugfix (5/22/95) this case must be supported and was commented out by mistake
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from Internal_Partitioning_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

       // printf ("In Internal_Partitioning_Type::operator new: Calling malloc because Size(%d) != sizeof(Internal_Partitioning_Type)(%d) \n",Size,sizeof(Internal_Partitioning_Type));

          return malloc(Size);
        }
       else
        {
       // printf ("In Internal_Partitioning_Type::operator new: Using the pool mechanism Size(%d) == sizeof(Internal_Partitioning_Type)(%d) \n",Size,sizeof(Internal_Partitioning_Type));

          if (Current_Link == NULL)
             {
            // CLASS_ALLOCATION_POOL_SIZE *= 2;
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Call malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

            // Use new operator instead of malloc to avoid Purify FMM warning
#if 1
               Current_Link = (Internal_Partitioning_Type*) APP_MALLOC ( CLASS_ALLOCATION_POOL_SIZE * sizeof(Internal_Partitioning_Type) );
#else
               Current_Link = (Internal_Partitioning_Type*) new char [ CLASS_ALLOCATION_POOL_SIZE * sizeof(Internal_Partitioning_Type) ];
#endif

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Called malloc for Array Memory_Block_Index = %d \n",Memory_Block_Index);
#endif

#if EXTRA_ERROR_CHECKING
               if (Current_Link == NULL) 
                  { 
                    printf ("ERROR: malloc == NULL in Array::operator new! \n"); 
                    APP_ABORT();
                  } 

            // Initialize the Memory_Block_List to NULL
            // This is used to delete the Memory pool blocks to free memory in use
            // and thus prevent memory-in-use errors from Purify
               if (Memory_Block_Index == 0)
                  {
                    for (int i=0; i < Max_Number_Of_Memory_Blocks-1; i++)
                       Memory_Block_List [i] = NULL;
                  }
#endif

               Memory_Block_List [Memory_Block_Index++] = (unsigned char *) Current_Link;

#if EXTRA_ERROR_CHECKING
            // Bounds checking!
               if (Memory_Block_Index >= Max_Number_Of_Memory_Blocks)
                  {
                    printf ("ERROR: Memory_Block_Index (%d) >= Max_Number_Of_Memory_Blocks (%d) \n",Memory_Block_Index,Max_Number_Of_Memory_Blocks);
                    APP_ABORT();
                  }
#endif

            // Initialize the free list of pointers!
               for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
                  {
                    Current_Link [i].freepointer = &(Current_Link[i+1]);
                  }

            // Set the pointer of the last one to NULL!
               Current_Link [CLASS_ALLOCATION_POOL_SIZE-1].freepointer = NULL;
             }
        }

  // Save the start of the list and remove the first link and return that
  // first link as the new object!

     Internal_Partitioning_Type* Forward_Link = Current_Link;
     Current_Link = Current_Link->freepointer;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Returning from Internal_Partitioning_Type::operator new! (with address of %p) \n",Forward_Link);
#endif

     return Forward_Link;

  // case of USE_CPP_NEW_DELETE_OPERATORS
#endif
   }
#endif

void Internal_Partitioning_Type::operator delete ( void *Pointer, size_t sizeOfObject )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("In Internal_Partitioning_Type::operator delete: Size(%d)  sizeof(Internal_Partitioning_Type)(%d) \n",sizeOfObject,sizeof(Internal_Partitioning_Type));
        }
#endif

#if USE_CPP_NEW_DELETE_OPERATORS
     free (Pointer);
#else
     if (sizeOfObject != sizeof(Internal_Partitioning_Type))
        {
       // Overture's Grid Function class derives from A++/P++ array objects
       // and so must be able to return a valid pointer to memory when using 
       // even the A++ or P++ new operator.

       // If this is an object derived from Internal_Partitioning_Type
       // then we can't do anything with memory pools from here!
       // It would have to be done within the context of the derived objects
       // operator new!  So we just return the following!

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In Internal_Partitioning_Type::operator delete: Calling global delete (free) because Size(%d) != sizeof(Internal_Partitioning_Type)(%d) \n",sizeOfObject,sizeof(Internal_Partitioning_Type));
             }
#endif

          free(Pointer);
        }
       else
        {
          Internal_Partitioning_Type *New_Link = (Internal_Partitioning_Type*) Pointer;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("In Internal_Partitioning_Type::operator delete (%p): Using the pool mechanism Size(%d) == sizeof(Internal_Partitioning_Type)(%d) \n",Pointer,sizeOfObject,sizeof(Internal_Partitioning_Type));
             }
#endif
          if (New_Link != NULL)
             {
            // Put deleted object (New_Link) at front of linked list (Current_Link)!
               New_Link->freepointer = Current_Link;
               Current_Link = New_Link;
             }
#if EXTRA_ERROR_CHECKING
            else
             {
               printf ("ERROR: In Internal_Partitioning_Type::operator delete - attempt made to delete a NULL pointer! \n");
               APP_ABORT();
             }
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving Internal_Partitioning_Type::operator delete! \n");
#endif

  // case of USE_CPP_NEW_DELETE_OPERATORS FALSE
#endif
   }























