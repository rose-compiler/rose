// Allow repeated includes of hash.h without error
#ifndef _APP_HASH_H
#define _APP_HASH_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif

#if !defined(GNU) && !defined(__KCC)
#define bool int
// Added to avoid STL compile error in alloc.h ("false is not defined")
#define true  1
#define false 0
#endif

// We want this to be a prime number (a la hashing theory)!
#define NUMBER_OF_BUCKETS 13
#define HASH_TABLE_DEPTH  100





/* MACRO EXPANSION BEGINS HERE */
/* Now expand the class definition! */

class doubleSerialArray_Data_Hash_Table
   {
  /* We build a special sort of hash table that is two dimensional.
  // The first dimension acts the way a usual hash table works, but the
  // second dimension acts as a cache by letting the first several
  // (determined by the Hash_Table_Depth) buckets to exist in an 
  // array where the access is very fast.  Additional entries are 
  // available as in a standard hash table by having a linked list
  // at the end of the last bucket in the array of buckets of length
  // Hash_Table_Depth.  The idea behind this sort of hash table is that 
  // there are very few entries to be stored in the hash table so
  // it can almost always avoid the mallocing of the small bucket 
  // objects, and thus the method should be fast on the CRAY!
  */

     class double_Hashed_Array_Type
        {
          public:
             int Memory_Size;
             double* Raw_Memory;

            ~double_Hashed_Array_Type () 
                {
               // Memory_Size = 0;
               // delete []Raw_Memory;
                  delete Raw_Memory;
                };

             double_Hashed_Array_Type () : Memory_Size (0), Raw_Memory (NULL) 
                { };

             double_Hashed_Array_Type ( int Size ) : Memory_Size (Size)
                { 
               // We need access to the descriptor to allocate memory using the MDI functions!
               // Raw_Memory = MDI_double_Allocate ();
                  Raw_Memory = (double*) malloc ( Size * sizeof(double) );
                };

             double_Hashed_Array_Type ( double* Memory_To_Store , int Size ) : Memory_Size (Size),
                                                                    Raw_Memory (Memory_To_Store)
                { };

          /* Used to resize existing memory, if the correct size was not found! */
             double_Hashed_Array_Type & redim( int New_Size )
                {
                  Memory_Size = New_Size;
                  if (Raw_Memory == NULL)
                     {
                       Raw_Memory  = (double*) ( malloc ( New_Size * sizeof(double) ) );
                     }
                    else
                     {
                       Raw_Memory  = (double*) ( realloc ( (char *) Raw_Memory , New_Size * sizeof(double) ) );
                     }

                  return *this;
                };
        };

     public:
          static bool USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE;
          int Number_Of_Buckets;
          int Depth_Of_Table;

          static int Hash_Table_Hits;
          static int Hash_Table_Misses;

          double_Hashed_Array_Type *Search_Table [NUMBER_OF_BUCKETS][HASH_TABLE_DEPTH];

       /* This stores the largest values of the indexes used in the
       // HASH_TABLE_DEPTH dimension of the search table.  Thus the
       // searching of the search table is shortened from the maximum of
       // the worst case and the search table can contain NULL entries
       // which will then be skipped over instead of signaling the end
       // of the list of entries in the search table bucket list.  
       // This avoids the problem of having to  all the entries 
       // in a bucket list so that the list is a contigous set of non-NULL
       // pointers. It is a little bit complicated but allows for very 
       // fast access to the hash table data with mimimal calls to the 
       // heap to add new buckets to the list etc.  It is entended to be
       // very fast on the CRAY!
       */

          int Max_Index_In_Table [NUMBER_OF_BUCKETS];
          
       /* MEMBER FUNCTIONS */

         ~doubleSerialArray_Data_Hash_Table();
          doubleSerialArray_Data_Hash_Table();

          double* Get_Primative_Array ( int Size );

          void Put_Primative_Array_Into_Storage ( double* Memory_To_Store , int Size );
   };

class floatSerialArray_Data_Hash_Table
   {
  /* We build a special sort of hash table that is two dimensional.
  // The first dimension acts the way a usual hash table works, but the
  // second dimension acts as a cache by letting the first several
  // (determined by the Hash_Table_Depth) buckets to exist in an 
  // array where the access is very fast.  Additional entries are 
  // available as in a standard hash table by having a linked list
  // at the end of the last bucket in the array of buckets of length
  // Hash_Table_Depth.  The idea behind this sort of hash table is that 
  // there are very few entries to be stored in the hash table so
  // it can almost always avoid the mallocing of the small bucket 
  // objects, and thus the method should be fast on the CRAY!
  */

     class float_Hashed_Array_Type
        {
          public:
             int Memory_Size;
             float* Raw_Memory;

            ~float_Hashed_Array_Type () 
                {
               // Memory_Size = 0;
               // delete []Raw_Memory;
                  delete Raw_Memory;
                };

             float_Hashed_Array_Type () : Memory_Size (0), Raw_Memory (NULL) 
                { };

             float_Hashed_Array_Type ( int Size ) : Memory_Size (Size)
                { 
               // We need access to the descriptor to allocate memory using the MDI functions!
               // Raw_Memory = MDI_float_Allocate ();
                  Raw_Memory = (float*) malloc ( Size * sizeof(float) );
                };

             float_Hashed_Array_Type ( float* Memory_To_Store , int Size ) : Memory_Size (Size),
                                                                    Raw_Memory (Memory_To_Store)
                { };

          /* Used to resize existing memory, if the correct size was not found! */
             float_Hashed_Array_Type & redim( int New_Size )
                {
                  Memory_Size = New_Size;
                  if (Raw_Memory == NULL)
                     {
                       Raw_Memory  = (float*) ( malloc ( New_Size * sizeof(float) ) );
                     }
                    else
                     {
                       Raw_Memory  = (float*) ( realloc ( (char *) Raw_Memory , New_Size * sizeof(float) ) );
                     }

                  return *this;
                };
        };

     public:
          static bool USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE;
          int Number_Of_Buckets;
          int Depth_Of_Table;

          static int Hash_Table_Hits;
          static int Hash_Table_Misses;

          float_Hashed_Array_Type *Search_Table [NUMBER_OF_BUCKETS][HASH_TABLE_DEPTH];

       /* This stores the largest values of the indexes used in the
       // HASH_TABLE_DEPTH dimension of the search table.  Thus the
       // searching of the search table is shortened from the maximum of
       // the worst case and the search table can contain NULL entries
       // which will then be skipped over instead of signaling the end
       // of the list of entries in the search table bucket list.  
       // This avoids the problem of having to  all the entries 
       // in a bucket list so that the list is a contigous set of non-NULL
       // pointers. It is a little bit complicated but allows for very 
       // fast access to the hash table data with mimimal calls to the 
       // heap to add new buckets to the list etc.  It is entended to be
       // very fast on the CRAY!
       */

          int Max_Index_In_Table [NUMBER_OF_BUCKETS];
          
       /* MEMBER FUNCTIONS */

         ~floatSerialArray_Data_Hash_Table();
          floatSerialArray_Data_Hash_Table();

          float* Get_Primative_Array ( int Size );

          void Put_Primative_Array_Into_Storage ( float* Memory_To_Store , int Size );
   };

class intSerialArray_Data_Hash_Table
   {
  /* We build a special sort of hash table that is two dimensional.
  // The first dimension acts the way a usual hash table works, but the
  // second dimension acts as a cache by letting the first several
  // (determined by the Hash_Table_Depth) buckets to exist in an 
  // array where the access is very fast.  Additional entries are 
  // available as in a standard hash table by having a linked list
  // at the end of the last bucket in the array of buckets of length
  // Hash_Table_Depth.  The idea behind this sort of hash table is that 
  // there are very few entries to be stored in the hash table so
  // it can almost always avoid the mallocing of the small bucket 
  // objects, and thus the method should be fast on the CRAY!
  */

     class int_Hashed_Array_Type
        {
          public:
             int Memory_Size;
             int* Raw_Memory;

            ~int_Hashed_Array_Type () 
                {
               // Memory_Size = 0;
               // delete []Raw_Memory;
                  delete Raw_Memory;
                };

             int_Hashed_Array_Type () : Memory_Size (0), Raw_Memory (NULL) 
                { };

             int_Hashed_Array_Type ( int Size ) : Memory_Size (Size)
                { 
               // We need access to the descriptor to allocate memory using the MDI functions!
               // Raw_Memory = MDI_int_Allocate ();
                  Raw_Memory = (int*) malloc ( Size * sizeof(int) );
                };

             int_Hashed_Array_Type ( int* Memory_To_Store , int Size ) : Memory_Size (Size),
                                                                    Raw_Memory (Memory_To_Store)
                { };

          /* Used to resize existing memory, if the correct size was not found! */
             int_Hashed_Array_Type & redim( int New_Size )
                {
                  Memory_Size = New_Size;
                  if (Raw_Memory == NULL)
                     {
                       Raw_Memory  = (int*) ( malloc ( New_Size * sizeof(int) ) );
                     }
                    else
                     {
                       Raw_Memory  = (int*) ( realloc ( (char *) Raw_Memory , New_Size * sizeof(int) ) );
                     }

                  return *this;
                };
        };

     public:
          static bool USE_HASH_TABLE_FOR_ARRAY_DATA_STORAGE;
          int Number_Of_Buckets;
          int Depth_Of_Table;

          static int Hash_Table_Hits;
          static int Hash_Table_Misses;

          int_Hashed_Array_Type *Search_Table [NUMBER_OF_BUCKETS][HASH_TABLE_DEPTH];

       /* This stores the largest values of the indexes used in the
       // HASH_TABLE_DEPTH dimension of the search table.  Thus the
       // searching of the search table is shortened from the maximum of
       // the worst case and the search table can contain NULL entries
       // which will then be skipped over instead of signaling the end
       // of the list of entries in the search table bucket list.  
       // This avoids the problem of having to  all the entries 
       // in a bucket list so that the list is a contigous set of non-NULL
       // pointers. It is a little bit complicated but allows for very 
       // fast access to the hash table data with mimimal calls to the 
       // heap to add new buckets to the list etc.  It is entended to be
       // very fast on the CRAY!
       */

          int Max_Index_In_Table [NUMBER_OF_BUCKETS];
          
       /* MEMBER FUNCTIONS */

         ~intSerialArray_Data_Hash_Table();
          intSerialArray_Data_Hash_Table();

          int* Get_Primative_Array ( int Size );

          void Put_Primative_Array_Into_Storage ( int* Memory_To_Store , int Size );
   };


#endif  /* !defined(_APP_HASH_H) */





