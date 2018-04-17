// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma implementation "data_hash.h"
#endif

#include "A++.h"





bool doubleArray_Data_Hash_Table::USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE = FALSE;
int doubleArray_Data_Hash_Table::Hash_Table_Hits   = 0;
int doubleArray_Data_Hash_Table::Hash_Table_Misses = 0;

doubleArray_Data_Hash_Table::~doubleArray_Data_Hash_Table()
   {
   }

doubleArray_Data_Hash_Table::doubleArray_Data_Hash_Table()
   {
     if (APP_DEBUG > 0)
          printf ("Initialize the doubleArray_Data_hash table! \n");

     Number_Of_Buckets = NUMBER_OF_BUCKETS;
     Depth_Of_Table    = HASH_TABLE_DEPTH;

  // Hash_Table_Hits   = 0;
  // Hash_Table_Misses = 0;

  // Initialize the Search table (hash_table)!
     int i;
     for (i=0; i < Number_Of_Buckets; i++)
          for (int j=0; j < Depth_Of_Table; j++)
               Search_Table [i][j] = NULL;

  // This stores the largest  i n d e x  position that is currently
  // in use in the search table for each Bucket in the hash table!
     for (i=0; i < Number_Of_Buckets; i++)
          Max_Index_In_Table [i] = 0;

     if (APP_DEBUG > 0)
          printf ("Leaving Hash_Table constructor! \n");
   }


double* doubleArray_Data_Hash_Table::Get_Primative_Array ( int Size )
   {
     int Key                        = Size;
     int Hash_Table_Key             = Key % Number_Of_Buckets;
     int Index                      = -1;
     double_Hashed_Array_Type *Temp_Bucket = NULL;

     do {
          Index++;
          Temp_Bucket = Search_Table [Hash_Table_Key][Index];
        }
     while ( (Index < Max_Index_In_Table [Hash_Table_Key]) && 
             ( (Temp_Bucket == NULL) || ( (Temp_Bucket != NULL) && (Temp_Bucket->Memory_Size != Key) ) ) );

     if ( (Index == Depth_Of_Table-1) && ( (Temp_Bucket != NULL ) && ( Temp_Bucket->Memory_Size != Key ) ) )
        {
          printf ("ERROR:Get_Temporary_Vector - Not yet implemented, 2nd level of hash table! \n");
          APP_ABORT();
        }

     if (Temp_Bucket == NULL)
        {
       // If we can't find what what we need then we should build it!
          Temp_Bucket = new double_Hashed_Array_Type ( Size ); // Set to the desired length!
          Hash_Table_Misses++;
        }
       else
        {
          Search_Table [Hash_Table_Key][Index] = NULL;

       // Now we have to  the positions farther down the table to
       // a position one back.  So that the next search will not prematurly
       // find a NULL pointer before getting to the end of the list!
          Hash_Table_Hits++;
        }

  // Error checking
     if (Temp_Bucket->Memory_Size != Key)
        {
       // printf ("NOTE: Get_Primative_Array: Temp_Bucket->Memory_Size() != Key (input size) (Redimensioning) \n");

          Temp_Bucket->redim(Key);       // Set to the desired length!

       // Record this as a MISS!
          Hash_Table_Misses++;
          Hash_Table_Hits--;
        }

     return Temp_Bucket->Raw_Memory;
   }


void doubleArray_Data_Hash_Table::Put_Primative_Array_Into_Storage ( double* Memory_To_Store, int Size )
   {
     int Key = Size;
     int Hash_Table_Key = Key % Number_Of_Buckets;
     int Index = -1;
     double_Hashed_Array_Type *Temp_Bucket = NULL;
     double_Hashed_Array_Type *New_Bucket  = new double_Hashed_Array_Type ( Memory_To_Store , Size );

     do {
          Index++;
          Temp_Bucket = Search_Table [Hash_Table_Key][Index];
        }
     while ( (Index < Depth_Of_Table-1) && (Temp_Bucket != NULL ) );

     if ( (Index == Depth_Of_Table-1) && (Temp_Bucket != NULL ) )
        {
          printf ("ERROR:Put_Primative_Memory_Into_Storage - Not yet implemented, 2nd level of hash table! \n");
          APP_ABORT();
        }

     Max_Index_In_Table [Hash_Table_Key] = (Max_Index_In_Table [Hash_Table_Key] < Index) ? Index : Max_Index_In_Table [Hash_Table_Key];

     if ( Temp_Bucket == NULL )
        {
          Search_Table [Hash_Table_Key][Index] = New_Bucket;
        }
   }

bool floatArray_Data_Hash_Table::USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE = FALSE;
int floatArray_Data_Hash_Table::Hash_Table_Hits   = 0;
int floatArray_Data_Hash_Table::Hash_Table_Misses = 0;

floatArray_Data_Hash_Table::~floatArray_Data_Hash_Table()
   {
   }

floatArray_Data_Hash_Table::floatArray_Data_Hash_Table()
   {
     if (APP_DEBUG > 0)
          printf ("Initialize the floatArray_Data_hash table! \n");

     Number_Of_Buckets = NUMBER_OF_BUCKETS;
     Depth_Of_Table    = HASH_TABLE_DEPTH;

  // Hash_Table_Hits   = 0;
  // Hash_Table_Misses = 0;

  // Initialize the Search table (hash_table)!
     int i;
     for (i=0; i < Number_Of_Buckets; i++)
          for (int j=0; j < Depth_Of_Table; j++)
               Search_Table [i][j] = NULL;

  // This stores the largest  i n d e x  position that is currently
  // in use in the search table for each Bucket in the hash table!
     for (i=0; i < Number_Of_Buckets; i++)
          Max_Index_In_Table [i] = 0;

     if (APP_DEBUG > 0)
          printf ("Leaving Hash_Table constructor! \n");
   }


float* floatArray_Data_Hash_Table::Get_Primative_Array ( int Size )
   {
     int Key                        = Size;
     int Hash_Table_Key             = Key % Number_Of_Buckets;
     int Index                      = -1;
     float_Hashed_Array_Type *Temp_Bucket = NULL;

     do {
          Index++;
          Temp_Bucket = Search_Table [Hash_Table_Key][Index];
        }
     while ( (Index < Max_Index_In_Table [Hash_Table_Key]) && 
             ( (Temp_Bucket == NULL) || ( (Temp_Bucket != NULL) && (Temp_Bucket->Memory_Size != Key) ) ) );

     if ( (Index == Depth_Of_Table-1) && ( (Temp_Bucket != NULL ) && ( Temp_Bucket->Memory_Size != Key ) ) )
        {
          printf ("ERROR:Get_Temporary_Vector - Not yet implemented, 2nd level of hash table! \n");
          APP_ABORT();
        }

     if (Temp_Bucket == NULL)
        {
       // If we can't find what what we need then we should build it!
          Temp_Bucket = new float_Hashed_Array_Type ( Size ); // Set to the desired length!
          Hash_Table_Misses++;
        }
       else
        {
          Search_Table [Hash_Table_Key][Index] = NULL;

       // Now we have to  the positions farther down the table to
       // a position one back.  So that the next search will not prematurly
       // find a NULL pointer before getting to the end of the list!
          Hash_Table_Hits++;
        }

  // Error checking
     if (Temp_Bucket->Memory_Size != Key)
        {
       // printf ("NOTE: Get_Primative_Array: Temp_Bucket->Memory_Size() != Key (input size) (Redimensioning) \n");

          Temp_Bucket->redim(Key);       // Set to the desired length!

       // Record this as a MISS!
          Hash_Table_Misses++;
          Hash_Table_Hits--;
        }

     return Temp_Bucket->Raw_Memory;
   }


void floatArray_Data_Hash_Table::Put_Primative_Array_Into_Storage ( float* Memory_To_Store, int Size )
   {
     int Key = Size;
     int Hash_Table_Key = Key % Number_Of_Buckets;
     int Index = -1;
     float_Hashed_Array_Type *Temp_Bucket = NULL;
     float_Hashed_Array_Type *New_Bucket  = new float_Hashed_Array_Type ( Memory_To_Store , Size );

     do {
          Index++;
          Temp_Bucket = Search_Table [Hash_Table_Key][Index];
        }
     while ( (Index < Depth_Of_Table-1) && (Temp_Bucket != NULL ) );

     if ( (Index == Depth_Of_Table-1) && (Temp_Bucket != NULL ) )
        {
          printf ("ERROR:Put_Primative_Memory_Into_Storage - Not yet implemented, 2nd level of hash table! \n");
          APP_ABORT();
        }

     Max_Index_In_Table [Hash_Table_Key] = (Max_Index_In_Table [Hash_Table_Key] < Index) ? Index : Max_Index_In_Table [Hash_Table_Key];

     if ( Temp_Bucket == NULL )
        {
          Search_Table [Hash_Table_Key][Index] = New_Bucket;
        }
   }

bool intArray_Data_Hash_Table::USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE = FALSE;
int intArray_Data_Hash_Table::Hash_Table_Hits   = 0;
int intArray_Data_Hash_Table::Hash_Table_Misses = 0;

intArray_Data_Hash_Table::~intArray_Data_Hash_Table()
   {
   }

intArray_Data_Hash_Table::intArray_Data_Hash_Table()
   {
     if (APP_DEBUG > 0)
          printf ("Initialize the intArray_Data_hash table! \n");

     Number_Of_Buckets = NUMBER_OF_BUCKETS;
     Depth_Of_Table    = HASH_TABLE_DEPTH;

  // Hash_Table_Hits   = 0;
  // Hash_Table_Misses = 0;

  // Initialize the Search table (hash_table)!
     int i;
     for (i=0; i < Number_Of_Buckets; i++)
          for (int j=0; j < Depth_Of_Table; j++)
               Search_Table [i][j] = NULL;

  // This stores the largest  i n d e x  position that is currently
  // in use in the search table for each Bucket in the hash table!
     for (i=0; i < Number_Of_Buckets; i++)
          Max_Index_In_Table [i] = 0;

     if (APP_DEBUG > 0)
          printf ("Leaving Hash_Table constructor! \n");
   }


int* intArray_Data_Hash_Table::Get_Primative_Array ( int Size )
   {
     int Key                        = Size;
     int Hash_Table_Key             = Key % Number_Of_Buckets;
     int Index                      = -1;
     int_Hashed_Array_Type *Temp_Bucket = NULL;

     do {
          Index++;
          Temp_Bucket = Search_Table [Hash_Table_Key][Index];
        }
     while ( (Index < Max_Index_In_Table [Hash_Table_Key]) && 
             ( (Temp_Bucket == NULL) || ( (Temp_Bucket != NULL) && (Temp_Bucket->Memory_Size != Key) ) ) );

     if ( (Index == Depth_Of_Table-1) && ( (Temp_Bucket != NULL ) && ( Temp_Bucket->Memory_Size != Key ) ) )
        {
          printf ("ERROR:Get_Temporary_Vector - Not yet implemented, 2nd level of hash table! \n");
          APP_ABORT();
        }

     if (Temp_Bucket == NULL)
        {
       // If we can't find what what we need then we should build it!
          Temp_Bucket = new int_Hashed_Array_Type ( Size ); // Set to the desired length!
          Hash_Table_Misses++;
        }
       else
        {
          Search_Table [Hash_Table_Key][Index] = NULL;

       // Now we have to  the positions farther down the table to
       // a position one back.  So that the next search will not prematurly
       // find a NULL pointer before getting to the end of the list!
          Hash_Table_Hits++;
        }

  // Error checking
     if (Temp_Bucket->Memory_Size != Key)
        {
       // printf ("NOTE: Get_Primative_Array: Temp_Bucket->Memory_Size() != Key (input size) (Redimensioning) \n");

          Temp_Bucket->redim(Key);       // Set to the desired length!

       // Record this as a MISS!
          Hash_Table_Misses++;
          Hash_Table_Hits--;
        }

     return Temp_Bucket->Raw_Memory;
   }


void intArray_Data_Hash_Table::Put_Primative_Array_Into_Storage ( int* Memory_To_Store, int Size )
   {
     int Key = Size;
     int Hash_Table_Key = Key % Number_Of_Buckets;
     int Index = -1;
     int_Hashed_Array_Type *Temp_Bucket = NULL;
     int_Hashed_Array_Type *New_Bucket  = new int_Hashed_Array_Type ( Memory_To_Store , Size );

     do {
          Index++;
          Temp_Bucket = Search_Table [Hash_Table_Key][Index];
        }
     while ( (Index < Depth_Of_Table-1) && (Temp_Bucket != NULL ) );

     if ( (Index == Depth_Of_Table-1) && (Temp_Bucket != NULL ) )
        {
          printf ("ERROR:Put_Primative_Memory_Into_Storage - Not yet implemented, 2nd level of hash table! \n");
          APP_ABORT();
        }

     Max_Index_In_Table [Hash_Table_Key] = (Max_Index_In_Table [Hash_Table_Key] < Index) ? Index : Max_Index_In_Table [Hash_Table_Key];

     if ( Temp_Bucket == NULL )
        {
          Search_Table [Hash_Table_Key][Index] = New_Bucket;
        }
   }







