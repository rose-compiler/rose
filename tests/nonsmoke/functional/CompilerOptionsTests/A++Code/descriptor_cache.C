#include "A++.h"






#if 0
// This is inconsistant with the present design of A++/P++ since the
// descriptors are now a member of the array object and not pointed to by an array object.
// so we don't need this sort of descriptor caching now!

// d e f i n e d  in array.C
extern int APP_Global_Array_Base;

Array_Descriptor_Type** Array_Descriptor_Type::Descriptor_0D_Cache             = NULL;
Descriptor_1D_Cache_Type* Array_Descriptor_Type::Descriptor_1D_Cache             = NULL;
Descriptor_2D_Cache_Type* Array_Descriptor_Type::Descriptor_2D_Cache             = NULL;

// Will be supported later!
// Array_Descriptor_Type*********** Array_Descriptor_Type::Descriptor_3D_Cache    = NULL;
// Array_Descriptor_Type************** Array_Descriptor_Type::Descriptor_4D_Cache = NULL;

// typedef Array_Descriptor_Type*    Size_Pointers_Type        [SIZE_2D_HASH_TABLE_LENGTH]
//                                                               [SIZE_2D_HASH_TABLE_LENGTH];
// typedef Size_Pointers_Type*         Access_Bound_Pointers_Type[ACCESS_BOUND_2D_HASH_TABLE_LENGTH]
//                                                              [ACCESS_BOUND_2D_HASH_TABLE_LENGTH];
// typedef Access_Bound_Pointers_Type* Access_Base_Pointers_Type [ACCESS_BASE_2D_HASH_TABLE_LENGTH]
//                                                               [ACCESS_BASE_2D_HASH_TABLE_LENGTH];
// typedef Access_Base_Pointers_Type*  Descriptor_2D_Cache_Type  [DATA_BASE_HASH_TABLE_LENGTH];

#if 0
// Comment all the caching out!
void Array_Descriptor_Type::Put_Descriptor_Into_Cache ( Array_Descriptor_Type* & X )
   {
     int i = 0;
     int j = 0;
     Array_Descriptor_Type* Temp = NULL;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Inside of Array_Descriptor_Type::Put_Descriptor_Into_Cache(X) \n");
#endif

     if ( X->Descriptor_Dimension == 0 )
        {
#if 1
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("X->Descriptor_Dimension == 0 \n");
#endif

       // Put into default descriptor cache!
          if (X->Constant_Data_Base)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("X->Constant_Data_Base == TRUE \n");
#endif

            // Put at front of list LIFO (Last In First Out)
            // All NULL array descriptors are the same length so we can just
            // store this as a list directly (no other hash table dimensions 
            // are required).
               if (Descriptor_0D_Cache == NULL)
                  {
                 // Descriptor_0D_Cache = new (Array_Descriptor_Type**) [DATA_BASE_HASH_TABLE_LENGTH];
                    Descriptor_0D_Cache = (Array_Descriptor_Type**) malloc ( DATA_BASE_HASH_TABLE_LENGTH * sizeof (Array_Descriptor_Type**) );
                    for (i=0; i < DATA_BASE_HASH_TABLE_LENGTH; i++)
                         Descriptor_0D_Cache[i] = NULL;
                  }

               int Data_Base_Key = X->Data_Base[0] % DATA_BASE_HASH_TABLE_LENGTH;

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("Data_Base_Key = %d \n",Data_Base_Key);
#endif

               if (Descriptor_0D_Cache[Data_Base_Key] == NULL) 
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("Put initial link into list! \n");
#endif
                    Descriptor_0D_Cache[Data_Base_Key] = X;
                    X = NULL;
                  }
                 else
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("Put into front of existing linked list! \n");
#endif
                    Temp = Descriptor_0D_Cache[Data_Base_Key];
                    Descriptor_0D_Cache[Data_Base_Key] = X;
                    X->Next_Cache_Link = Temp;
                  }
             }
            else
             {
            // Don't store this descriptor in the cache!

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("X->Constant_Data_Base == FALSE \n");
#endif
               delete X;
            // X->ReferenceCountedDelete();
               X = NULL;
             }
#endif
        }
       else
        {
          if ( X->Descriptor_Dimension == 1 )
             {
#if 1
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > -1)
               printf ("X->Descriptor_Dimension == 1 \n");
#endif

            // 1D descriptor cache!
            // Store descriptor in chain of lists (Data_Base, Access_Base, Length)
               if ( X->Constant_Data_Base && X->Constant_Unit_Stride && X->Stride[0] == 1)
                  {
                    int Data_Base_Key    = X->Data_Base[0] % DATA_BASE_HASH_TABLE_LENGTH;
                    int Access_Base_Key  = X->Base[0]      % ACCESS_BASE_HASH_TABLE_LENGTH;
                    int Access_Bound_Key = X->Bound[0]     % ACCESS_BOUND_HASH_TABLE_LENGTH;
                    int Size_Key         = X->Size[0]      % SIZE_HASH_TABLE_LENGTH;

                    if (Descriptor_1D_Cache == NULL)
                       {
                      // Descriptor_1D_Cache = new (Array_Descriptor_Type*****) [DATA_BASE_HASH_TABLE_LENGTH];
                         Descriptor_1D_Cache = (Descriptor_1D_Cache_Type*) malloc ( sizeof (Descriptor_1D_Cache_Type) );
                         for (i=0; i < DATA_BASE_HASH_TABLE_LENGTH; i++)
                              (*Descriptor_1D_Cache)[i] = NULL;
                       }
     
                    if ( (*Descriptor_1D_Cache)[Data_Base_Key] == NULL) 
                       {
                      // Descriptor_1D_Cache[Data_Base_Key] = new (Array_Descriptor_Type****) [ACCESS_BASE_HASH_TABLE_LENGTH];
                         (*Descriptor_1D_Cache)[Data_Base_Key] = (Access_Base_Pointers_1D_Type*) malloc ( sizeof (Access_Base_Pointers_1D_Type) );
                      // Initialize pointers in list
                         for (i=0; i < ACCESS_BASE_HASH_TABLE_LENGTH; i++)
                              (*(*Descriptor_1D_Cache)[Data_Base_Key])[i] = NULL;
                       }
     
                    if ( (*(*Descriptor_1D_Cache)[Data_Base_Key])[Access_Base_Key] == NULL) 
                       {
                      // Descriptor_1D_Cache[Data_Base_Key][Access_Base_Key] = new (Array_Descriptor_Type***) [ACCESS_BOUND_HASH_TABLE_LENGTH];
                         (*(*Descriptor_1D_Cache)[Data_Base_Key])[Access_Base_Key] = (Access_Bound_Pointers_1D_Type*) malloc ( sizeof(Access_Bound_Pointers_1D_Type) );

                         for (i=0; i < ACCESS_BOUND_HASH_TABLE_LENGTH; i++)
                              (*(*(*Descriptor_1D_Cache)[Data_Base_Key])[Access_Base_Key])[i] = NULL;
                       }

                    if ( (*(*(*Descriptor_1D_Cache)[Data_Base_Key])[Access_Base_Key])[Access_Bound_Key] == NULL) 
                       {
                      // Descriptor_1D_Cache[Data_Base_Key][Access_Base_Key][Access_Bound_Key] = new (Array_Descriptor_Type**) [SIZE_HASH_TABLE_LENGTH];
                         (*(*(*Descriptor_1D_Cache)[Data_Base_Key])[Access_Base_Key])[Access_Bound_Key] = (Size_Pointers_1D_Type*) malloc ( sizeof(Size_Pointers_1D_Type) );
                         for (i=0; i < SIZE_HASH_TABLE_LENGTH; i++)
                              (*(*(*(*Descriptor_1D_Cache)[Data_Base_Key])[Access_Base_Key])[Access_Bound_Key])[i] = NULL;
                       }

                    if ( (*(*(*(*Descriptor_1D_Cache)[Data_Base_Key])[Access_Base_Key])[Access_Bound_Key])[Size_Key] == NULL) 
                       {
                         (*(*(*(*Descriptor_1D_Cache)[Data_Base_Key])[Access_Base_Key])[Access_Bound_Key])[Size_Key] = X;
                         X = NULL;
                       }
                      else
                       {
                         Temp = (*(*(*(*Descriptor_1D_Cache)[Data_Base_Key])[Access_Base_Key])[Access_Bound_Key])[Size_Key];
                         (*(*(*(*Descriptor_1D_Cache)[Data_Base_Key])[Access_Base_Key])[Access_Bound_Key])[Size_Key] = X;
                         X->Next_Cache_Link = Temp;
                         X = NULL;
                       }
                  }
                 else
                  {
                 // Don't store this descriptor in the cache!
                    delete X;
                 // X->ReferenceCountedDelete();
                    X = NULL;
                  }
#endif
             }
            else
             {
               if ( X->Descriptor_Dimension == 2 )
                  {
#if 1
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("X->Descriptor_Dimension == 2 \n");
#endif

                 // 2D descriptor cache!
                 // Store descriptor in chain of lists (Data_Base, Access_Base, Length)
                    if (X->Constant_Data_Base && X->Constant_Unit_Stride && X->Stride[0] == 1)
                       {
                         int Data_Base_Key      = X->Data_Base[0] % DATA_BASE_2D_HASH_TABLE_LENGTH;
                         int Access_Base_I_Key  = X->Base[0]      % ACCESS_BASE_2D_HASH_TABLE_LENGTH;
                         int Access_Base_J_Key  = X->Base[1]      % ACCESS_BASE_2D_HASH_TABLE_LENGTH;
                         int Access_Bound_I_Key = X->Bound[0]     % ACCESS_BOUND_2D_HASH_TABLE_LENGTH;
                         int Access_Bound_J_Key = X->Bound[1]     % ACCESS_BOUND_2D_HASH_TABLE_LENGTH;
                         int Size_I_Key         = X->Size[0]      % SIZE_2D_HASH_TABLE_LENGTH;
                         int Size_J_Key         = X->Size[1]      % SIZE_2D_HASH_TABLE_LENGTH;
     

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                       {
                         printf ("Data_Base_Key = %d \n",Data_Base_Key);
                         printf ("Access_Base_I_Key = %d \n",Access_Base_I_Key);
                         printf ("Access_Base_J_Key = %d \n",Access_Base_J_Key);
                         printf ("Access_Bound_I_Key = %d \n",Access_Bound_I_Key);
                         printf ("Access_Bound_J_Key = %d \n",Access_Bound_J_Key);
                         printf ("Size_I_Key = %d \n",Size_I_Key);
                         printf ("Size_J_Key = %d \n",Size_J_Key);
                       }
#endif

                         if (Descriptor_2D_Cache == NULL)
                            {
                           // Descriptor_2D_Cache = new (Array_Descriptor_Type*) [DATA_BASE_HASH_TABLE_LENGTH];
                              Descriptor_2D_Cache = (Descriptor_2D_Cache_Type*) malloc ( sizeof (Descriptor_2D_Cache_Type) );
                              for (i=0; i < DATA_BASE_HASH_TABLE_LENGTH; i++)
                                   (*Descriptor_2D_Cache)[i] = NULL;
                            }

     
                         if ( (*Descriptor_2D_Cache)[Data_Base_Key] == NULL) 
                            {
                           // printf ("Stage 1 \n");
                           // Descriptor_2D_Cache[Data_Base_Key] = 
                           //      new (Array_Descriptor_Type*) [ACCESS_BASE_2D_HASH_TABLE_LENGTH][ACCESS_BASE_2D_HASH_TABLE_LENGTH];
                           // Descriptor_2D_Cache[Data_Base_Key] = new Access_Base_Pointers_Type;
                              (*Descriptor_2D_Cache)[Data_Base_Key] = (Access_Base_Pointers_2D_Type*) malloc ( sizeof (Access_Base_Pointers_2D_Type) );
                           // Initialize pointers in list
                              for (i=0; i < ACCESS_BASE_2D_HASH_TABLE_LENGTH; i++)
                                   for (j=0; j < ACCESS_BASE_2D_HASH_TABLE_LENGTH; j++)
                                        (*(*Descriptor_2D_Cache)[Data_Base_Key])[i][j] = NULL;
                            }

                         if ( (*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key] == NULL)
                            {
                           // printf ("Stage 2 \n");
                           // Descriptor_2D_Cache[Data_Base_Key][Access_Base_I_Key][Access_Base_J_Key] = 
                           //      new (Array_Descriptor_Type*) [ACCESS_BOUND_2D_HASH_TABLE_LENGTH][ACCESS_BOUND_2D_HASH_TABLE_LENGTH];
                           // Descriptor_2D_Cache[Data_Base_Key][Access_Base_I_Key][Access_Base_J_Key] = new Access_Bound_Pointers_Type;
                              (*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key] = (Access_Bound_Pointers_2D_Type*) malloc ( sizeof (Access_Bound_Pointers_2D_Type) );

                              for (i=0; i < ACCESS_BOUND_2D_HASH_TABLE_LENGTH; i++)
                                   for (j=0; j < ACCESS_BOUND_2D_HASH_TABLE_LENGTH; j++)
                                        (*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])[i][j] = NULL;
                            }

                         if ( (*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])[Access_Bound_I_Key][Access_Bound_J_Key] == NULL)
                            {
                           // printf ("Stage 3 \n");
                           // Descriptor_2D_Cache[Data_Base_Key][Access_Base_I_Key][Access_Base_J_Key][Access_Bound_I_Key][Access_Bound_J_Key] = new (Array_Descriptor_Type*) [SIZE_2D_HASH_TABLE_LENGTH][SIZE_2D_HASH_TABLE_LENGTH];
                           // Descriptor_2D_Cache[Data_Base_Key][Access_Base_I_Key][Access_Base_J_Key][Access_Bound_I_Key][Access_Bound_J_Key] = new Size_Pointers_Type;
                              (*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])[Access_Bound_I_Key][Access_Bound_J_Key] = (Size_Pointers_2D_Type*) malloc ( sizeof (Size_Pointers_2D_Type) );
                              for (i=0; i < SIZE_2D_HASH_TABLE_LENGTH; i++)
                                   for (j=0; j < SIZE_2D_HASH_TABLE_LENGTH; j++)
                                        (*(*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])[Access_Bound_I_Key][Access_Bound_J_Key])[i][j] = NULL;
                            } 

                         if ( (*(*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])[Access_Bound_I_Key][Access_Bound_J_Key])[Size_I_Key][Size_J_Key] == NULL)
                            {
                           // printf ("Stage 4 \n");
                              (*(*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])[Access_Bound_I_Key][Access_Bound_J_Key])[Size_I_Key][Size_J_Key] = X;
                              X = NULL;
                            }
                           else
                            {
                           // printf ("Stage 5 \n");
                              Temp = (*(*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])[Access_Bound_I_Key][Access_Bound_J_Key])[Size_I_Key][Size_J_Key];
                              (*(*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])[Access_Bound_I_Key][Access_Bound_J_Key])[Size_I_Key][Size_J_Key] = X;
                              X->Next_Cache_Link = Temp;
                              X = NULL;
                            }
                       }
                      else
                       {
                      // printf ("Stage 6 \n");
                      // Don't store this descriptor in the cache!
                         delete X;
                      // X->ReferenceCountedDelete();
                         X = NULL;
                       }
#endif
                  }
                 else
                  {
                    if ( X->Descriptor_Dimension == 3 )
                       {
                      // 3D descriptor cache!
                      // Don't store this descriptor in the cache!
                         delete X;
                      // X->ReferenceCountedDelete();
                         X = NULL;
                       }
                      else
                       {
                         if ( X->Descriptor_Dimension == 4 )
                            {
                           // 4D descriptor cache!
                           // Don't store this descriptor in the cache!
                              delete X;
                           // X->ReferenceCountedDelete();
                              X = NULL;
                            }
                           else
                            {
                              printf ("ERROR: Descriptor_Dimension out of range (X->Descriptor_Dimension = %d) \n",X->Descriptor_Dimension);
                              APP_ABORT();
                            }
                       }
                  }
             }
        }
   }

Array_Descriptor_Type *Array_Descriptor_Type::Get_Descriptor_From_Cache ()
   {
     Array_Descriptor_Type *Return_Descriptor = NULL;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of Array_Descriptor_Type::Get_Descriptor_From_Cache() \n");
#endif

  // 0D descriptor cache!
     if (Descriptor_0D_Cache != NULL)
        {
          int Data_Base_Key = APP_Global_Array_Base % DATA_BASE_HASH_TABLE_LENGTH;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Data_Base_Key = %d \n",Data_Base_Key);
#endif

          if (Descriptor_0D_Cache[Data_Base_Key] != NULL)
             {
               Return_Descriptor = Descriptor_0D_Cache[Data_Base_Key];

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("Check to see if this descriptor has the correct base! \n");
#endif

            // Since only constant stride and constant data base info goes into the cache
            // we can skip checking for these constant values when we get descriptors from the cache!
               if (Return_Descriptor->Data_Base[0] != APP_Global_Array_Base) 
                  {
                    bool Found     = FALSE;
                    Return_Descriptor = Return_Descriptor->Next_Cache_Link;
                    while (Return_Descriptor != NULL && !Found)
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 3)
                              printf ("Not found -- Looking through linked list! \n");
#endif
                         if (Return_Descriptor->Data_Base[0] == APP_Global_Array_Base)
                            {
                              Found = TRUE;
                            }
                           else
                            {
                              Return_Descriptor = Return_Descriptor->Next_Cache_Link;
                            }
                       }
                  }
               
             }
        }
       else
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Descriptor_0D_Cache == NULL \n");
#endif
        }
 
     if (Return_Descriptor == NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Building a new descriptor (calling constructor!) \n");
#endif
          Return_Descriptor = new Array_Descriptor_Type ();
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Return_Descriptor = %p \n",Return_Descriptor);
#endif
 
     return Return_Descriptor;
   }
 
Array_Descriptor_Type *Array_Descriptor_Type::Get_Descriptor_From_Cache ( const Internal_Index & I )
   {
     Array_Descriptor_Type *Return_Descriptor = NULL;

  // 1D descriptor cache!
  // Store descriptor in chain of lists (Data_Base, Access_Base, Length)
     if (Descriptor_Dimension == 1 && Constant_Data_Base && Constant_Unit_Stride)
        {
          int Data_Base_Key    = Data_Base[0] % DATA_BASE_HASH_TABLE_LENGTH;
          int Access_Base_Key  = I.Base       % ACCESS_BASE_HASH_TABLE_LENGTH;
          int Access_Bound_Key = I.Bound      % ACCESS_BOUND_HASH_TABLE_LENGTH;
          int Size_Key         = Size[0]      % SIZE_HASH_TABLE_LENGTH;

          if ( (Descriptor_1D_Cache != NULL) && 
               (Descriptor_1D_Cache[Data_Base_Key] != NULL) &&
               (Descriptor_1D_Cache[Data_Base_Key][Access_Base_Key] != NULL) &&
               (Descriptor_1D_Cache[Data_Base_Key][Access_Base_Key][Access_Bound_Key] != NULL) &&
               (Descriptor_1D_Cache[Data_Base_Key][Access_Base_Key][Access_Bound_Key][Size_Key] != NULL) )
             {
               Return_Descriptor = (*(*(*(*Descriptor_1D_Cache)[Data_Base_Key])[Access_Base_Key])[Access_Bound_Key])[Size_Key];

            // Since only constant stride and constant data base info goes into the cache
            // we can skip checking for these constant values when we get descriptors from the cache!
               if ( (Return_Descriptor->Bound[0]     != I.Bound)      || (Return_Descriptor->Size[0] != Size[0]) ||
                    (Return_Descriptor->Data_Base[0] != Data_Base[0]) || (Return_Descriptor->Base[0] != I.Base) )
                  {
                    bool Found     = FALSE;
                    Return_Descriptor = Return_Descriptor->Next_Cache_Link;
                    while (Return_Descriptor != NULL && !Found)
                       {
                         if ( (Return_Descriptor->Bound[0] == I.Bound) && 
                              (Return_Descriptor->Size[0] == Size[0]) &&
                              (Return_Descriptor->Data_Base[0] == Data_Base[0]) && 
                              (Return_Descriptor->Base[0] == I.Base) )
                            {
                              Found = TRUE;
                            }
                           else
                            {
                              Return_Descriptor = Return_Descriptor->Next_Cache_Link;
                            }
                       }
                  }
             }
        }

     printf ("Line commented out so constructor could be inlined! \n");
     APP_ABORT();
  // if (Return_Descriptor == NULL)
  //      Return_Descriptor = new Array_Descriptor_Type ( *this , I );

     return Return_Descriptor;
   }

Array_Descriptor_Type *Array_Descriptor_Type::Get_Descriptor_From_Cache ( const Internal_Index & I , const Internal_Index & J )
   {
     Array_Descriptor_Type *Return_Descriptor = NULL;

  // 1D descriptor cache!
  // Store descriptor in chain of lists (Data_Base, Access_Base, Length)
     if (Descriptor_Dimension == 2 && Constant_Data_Base && Constant_Unit_Stride)
        {
          int Data_Base_Key      = Data_Base[0] % DATA_BASE_2D_HASH_TABLE_LENGTH;
          int Access_Base_I_Key  = I.Base       % ACCESS_BASE_2D_HASH_TABLE_LENGTH;
          int Access_Base_J_Key  = J.Base       % ACCESS_BASE_2D_HASH_TABLE_LENGTH;
          int Access_Bound_I_Key = I.Bound      % ACCESS_BOUND_2D_HASH_TABLE_LENGTH;
          int Access_Bound_J_Key = J.Bound      % ACCESS_BOUND_2D_HASH_TABLE_LENGTH;
          int Size_I_Key         = Size[0]      % SIZE_2D_HASH_TABLE_LENGTH;
          int Size_J_Key         = Size[1]      % SIZE_2D_HASH_TABLE_LENGTH;

          if ( (Descriptor_2D_Cache != NULL) && 
               ( (*Descriptor_2D_Cache)[Data_Base_Key] != NULL) &&
               ( (*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key ][Access_Base_J_Key ] != NULL) &&
               ( (*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])
                                                  [Access_Bound_I_Key][Access_Bound_J_Key] != NULL) &&
               ( (*(*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key ][Access_Base_J_Key ])
                                                  [Access_Bound_I_Key][Access_Bound_J_Key])
                                                  [Size_I_Key        ][Size_J_Key        ] != NULL) )
             {
               Return_Descriptor = (*(*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])
                                                    [Access_Bound_I_Key][Access_Bound_J_Key])[Size_I_Key][Size_J_Key];

            // Since only constant stride and constant data base info goes into the cache
            // we can skip checking for these constant values when we get descriptors from the cache!
               if ( (Return_Descriptor->Bound[0]     != I.Bound)      || (Return_Descriptor->Size[0] != Size[0]) ||
                    (Return_Descriptor->Data_Base[0] != Data_Base[0]) || (Return_Descriptor->Base[0] != I.Base) )
                  {
                    bool Found     = FALSE;
                    Return_Descriptor = Return_Descriptor->Next_Cache_Link;
                    while (Return_Descriptor != NULL && !Found)
                       {
                         if ( (Return_Descriptor->Bound[0] == I.Bound) && 
                              (Return_Descriptor->Size[0] == Size[0]) &&
                              (Return_Descriptor->Data_Base[0] == Data_Base[0]) && 
                              (Return_Descriptor->Base[0] == I.Base) )
                            {
                              Found = TRUE;
                            }
                           else
                            {
                              Return_Descriptor = Return_Descriptor->Next_Cache_Link;
                            }
                       }
                  }
             }
        }

     printf ("Line commented out so constructor could be inlined! \n");
     APP_ABORT();
  // if (Return_Descriptor == NULL)
  //      Return_Descriptor = new Array_Descriptor_Type ( *this , I );

     return Return_Descriptor;
   }

Array_Descriptor_Type *Array_Descriptor_Type::Get_Descriptor_From_Cache ( int i, int j, int k, int l )
   {
     Array_Descriptor_Type *Return_Descriptor = NULL;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Inside of Array_Descriptor_Type::Get_Descriptor_From_Cache(%d,%d,%d,%d) \n",i,j,k,l);
#endif

  // Compute the dimension we are interested in!
     int Descriptor_Dimension = 4;
     if (l <= 1)
        {
          Descriptor_Dimension--;
          if (k <= 1)
             {
               Descriptor_Dimension--;
               if (j <= 1)
                  {
                    Descriptor_Dimension--;
                    if (i == 0)
                       {
                         Descriptor_Dimension--;
                       }
                  }
             }
        }

     bool Constant_Data_Base   = TRUE;
     bool Constant_Unit_Stride = TRUE;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Descriptor_Dimension = %d \n",Descriptor_Dimension);
#endif
     
  // 2D descriptor cache!
  // Store descriptor in chain of lists (Data_Base, Access_Base, Length)
     if (Descriptor_Dimension == 2 && Constant_Data_Base && Constant_Unit_Stride)
        {
          int Data_Base_Key      = APP_Global_Array_Base % DATA_BASE_2D_HASH_TABLE_LENGTH;
          int Access_Base_I_Key  = APP_Global_Array_Base % ACCESS_BASE_2D_HASH_TABLE_LENGTH;
          int Access_Base_J_Key  = APP_Global_Array_Base % ACCESS_BASE_2D_HASH_TABLE_LENGTH;
          int Access_Bound_I_Key = APP_Global_Array_Base+(i-1) % ACCESS_BOUND_2D_HASH_TABLE_LENGTH;
          int Access_Bound_J_Key = APP_Global_Array_Base+(j-1) % ACCESS_BOUND_2D_HASH_TABLE_LENGTH;
          int Size_I_Key         = i     % SIZE_2D_HASH_TABLE_LENGTH;
          int Size_J_Key         = (i*j) % SIZE_2D_HASH_TABLE_LENGTH;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             {
               printf ("Data_Base_Key = %d \n",Data_Base_Key);
               printf ("Access_Base_I_Key = %d \n",Access_Base_I_Key);
               printf ("Access_Base_J_Key = %d \n",Access_Base_J_Key);
               printf ("Access_Bound_I_Key = %d \n",Access_Bound_I_Key);
               printf ("Access_Bound_J_Key = %d \n",Access_Bound_J_Key);
               printf ("Size_I_Key = %d \n",Size_I_Key);
               printf ("Size_J_Key = %d \n",Size_J_Key);
             }
#endif

          if ( (Descriptor_2D_Cache != NULL) && 
               ( (*Descriptor_2D_Cache)[Data_Base_Key] != NULL) &&
               ( (*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key ][Access_Base_J_Key ] != NULL) &&
               ( (*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])
                                                  [Access_Bound_I_Key][Access_Bound_J_Key] != NULL) &&
               ( (*(*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key ][Access_Base_J_Key ])
                                                  [Access_Bound_I_Key][Access_Bound_J_Key])
                                                  [Size_I_Key        ][Size_J_Key        ] != NULL) )
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("A descriptor has been found inthe cache! \n");
#endif
               Return_Descriptor = (*(*(*(*Descriptor_2D_Cache)[Data_Base_Key])[Access_Base_I_Key][Access_Base_J_Key])
                                                    [Access_Bound_I_Key][Access_Bound_J_Key])[Size_I_Key][Size_J_Key];

            // Since only constant stride and constant data base info goes into the cache
            // we can skip checking for these constant values when we get descriptors from the cache!

            // Return_Descriptor->display("Return_Descriptor");

               if ( (Return_Descriptor->Bound[0]     != APP_Global_Array_Base+(i-1)) || 
                    (Return_Descriptor->Size[0]      != i) ||
                    (Return_Descriptor->Bound[1]     != APP_Global_Array_Base+(j-1)) || 
                    (Return_Descriptor->Size[1]      != (i*j) ) ||
                    (Return_Descriptor->Data_Base[0] != APP_Global_Array_Base) || 
                    (Return_Descriptor->Base[0]      != APP_Global_Array_Base) )
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("First descriptor found was not correct (search linked list)! \n");
#endif

                    bool Found     = FALSE;
                    Return_Descriptor = Return_Descriptor->Next_Cache_Link;
                    while (Return_Descriptor != NULL && !Found)
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 1)
                              printf ("Searching list! \n");
#endif
                         if ( (Return_Descriptor->Bound[0]     != APP_Global_Array_Base+(i-1)) || 
                              (Return_Descriptor->Size[0]      != i) ||
                              (Return_Descriptor->Bound[1]     != APP_Global_Array_Base+(j-1)) || 
                              (Return_Descriptor->Size[1]      != (i*j) ) ||
                              (Return_Descriptor->Data_Base[0] != APP_Global_Array_Base) || 
                              (Return_Descriptor->Base[0]      != APP_Global_Array_Base) )
                            {
                              Found = TRUE;
                            }
                           else
                            {
                              Return_Descriptor = Return_Descriptor->Next_Cache_Link;
                            }
                       }
                  }
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Nothing found in Descriptor_2D_Cache! \n");
#endif
             }
        }

     if (Return_Descriptor == NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("NOT FOUND in CACHE -- Call new Array_Descriptor_Type (i,j,k,l) \n");
#endif
          Return_Descriptor = new Array_Descriptor_Type (i,j,k,l);
        }

     return Return_Descriptor;
   }

Array_Descriptor_Type *Array_Descriptor_Type::Get_Descriptor_From_Cache ( 
          const Array_Descriptor_Type & X )
   {
     Array_Descriptor_Type *Return_Descriptor = NULL;

     printf ("SORRY NO IMPLEMENTED, Descriptor caching not implemented yet! \n");
     APP_ABORT();

     if (Return_Descriptor == NULL)
          Return_Descriptor = new Array_Descriptor_Type (X);

     return Return_Descriptor;
   }
#endif

// turn off all of this descriptor caching code (see note at top of file)
#endif







// ***************************************************************************************
//     Array_Descriptor constructors put here to allow inlining in Indexing operators!
// ***************************************************************************************


