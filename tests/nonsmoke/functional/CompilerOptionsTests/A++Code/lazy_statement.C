// P++ requires that either COMPILE_PPP or COMPILE_SERIAL_APP be defined
// where as A++ defines COMPILE_APP in config.h (P++ can't do that and
// still maintain simplicity using AutoMake)
// So if we don't see COMPILE_APP then we assume we are compiling for P++!
#if !defined(COMPILE_APP)
#define COMPILE_PPP
#endif

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma implementation "lazy_statement.h"
#endif

#include "A++.h"

extern int APP_Global_Array_ID;

Array_Statement_Type *Array_Statement_Type::Head_Of_List = NULL;
Array_Statement_Type *Array_Statement_Type::Tail_Of_List = NULL;

// Initialize the Array_Statement_Type::Statement_ID_Counter to start at 1!
int Array_Statement_Type::Statement_ID_Counter = 1;

// Setup hash table for use in dependence analysis!
Array_Statement_Type *Array_Statement_Type::Hash_Table_Of_Statement_Pointers [MAX_HASH_TABLE_SIZE];

// ****************************************************************************
// ****************************************************************************
//         Array_Statement_Type::Set_Union_Type  class member functions
// ****************************************************************************
// ****************************************************************************

#if 1
Array_Statement_Type::Set_Union_Type::~Set_Union_Type ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of destructor for Array_Statement_Type::Set_Union_Type \n");
#endif
   }

Array_Statement_Type::Set_Union_Type::Set_Union_Type ()
   {
     printf ("ERROR: don't use this constructor for Array_Statement_Type::Set_Union_Type \n");
     APP_ABORT();
   }

// This function will not inline using the AT&T compiler because it is too complex!
Array_Statement_Type::Set_Union_Type::Set_Union_Type ( int Max_Number_Of_Statements )
   {
     Count  = new int [Max_Number_Of_Statements];
     Name   = new int [Max_Number_Of_Statements];
     Father = new int [Max_Number_Of_Statements];
     Root   = new int [Max_Number_Of_Statements];
     Temp   = new int [Max_Number_Of_Statements];

  // Initialize arrays used for Union Find algorithm!
     for (int Index = 0; Index < Max_Number_Of_Statements; Index++)
        {
          Count  [Index] = 1;
          Name   [Index] = Index;
          Father [Index] = 0;
          Root   [Index] = Index;
          Temp   [Index] = 0;
        }
   }
#endif

// This function will not inline using the AT&T compiler because it contains a loop!
int Array_Statement_Type::Set_Union_Type::Fast_Find ( int Statement_ID_Of_Dependent_Statement )
   {
  // printf ("Fast_Find: Statement_ID_Of_Dependent_Statement = %d \n",Statement_ID_Of_Dependent_Statement);

     if ( Statement_ID_Of_Dependent_Statement <= 0 )
        {
          printf ("ERROR in Array_Statement_Type::Set_Union_Type::Fast_Find -- Statement_ID_Of_Dependent_Statement <= 0 \n");
          APP_ABORT();
        }

     int Index = -1;
     while (Father [Statement_ID_Of_Dependent_Statement] != 0)
       {
         Index++;
         Temp [Index] = Statement_ID_Of_Dependent_Statement;
         Statement_ID_Of_Dependent_Statement = Father [Statement_ID_Of_Dependent_Statement];
       }
   
  // Now do path compression
     while (Index > 0)
       {
         Father [ Temp [Index] ] = Statement_ID_Of_Dependent_Statement;
         Index--;
       }

     return Name [Statement_ID_Of_Dependent_Statement];
   }

// This function will not inline using the AT&T compiler because it is too large!
void Array_Statement_Type::Set_Union_Type::Fast_Union ( 
     int Statement_ID_Of_Current_Statement , int Statement_ID_Of_Dependent_Statement )
   {
  // printf ("Fast_Union: Statement_ID_Of_Current_Statement   = %d \n",Statement_ID_Of_Current_Statement);
  // printf ("Fast_Union: Statement_ID_Of_Dependent_Statement = %d \n",Statement_ID_Of_Dependent_Statement);

     int Small_Tree, Large_Tree;

     int Original_Statement_ID_Of_Dependent_Statement = Statement_ID_Of_Dependent_Statement;

     if ( Statement_ID_Of_Current_Statement != Statement_ID_Of_Dependent_Statement )
        {
          if (Count [ Root [Statement_ID_Of_Dependent_Statement] ] <= Count [ Root [Statement_ID_Of_Current_Statement] ])
            {
              int swap = Statement_ID_Of_Current_Statement;
              Statement_ID_Of_Current_Statement   = Statement_ID_Of_Dependent_Statement;
              Statement_ID_Of_Dependent_Statement = swap;
            }

          Large_Tree  = Root [Statement_ID_Of_Dependent_Statement];
          Small_Tree  = Root [Statement_ID_Of_Current_Statement];
          Father [Small_Tree] = Large_Tree;
          Count  [Large_Tree] += Count [Small_Tree];
          Name   [Large_Tree] = Original_Statement_ID_Of_Dependent_Statement;
          Root   [Original_Statement_ID_Of_Dependent_Statement]  = Large_Tree;
        }
   }

// ****************************************************************************
// ****************************************************************************
//               Array_Statement_Type class member functions
// ****************************************************************************
// ****************************************************************************

// Support function for operator new since without it operator new
// would contain a loop and would not be able to be inlined!
void Array_Statement_Type::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Array_Statement_Type::~Array_Statement_Type () 
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Calling Array_Statement_Type destructor! \n"); 
#endif
   }

Array_Statement_Type::Array_Statement_Type () 
   { 
     printf ("ERROR: Calling (default) Array_Statement_Type constructor! \n"); 
     APP_ABORT();
   }

Array_Statement_Type::Array_Statement_Type ( int iDef_Statement_Number ) 
   { 
  // Used to build Initial Definition Node (idef node) for setup of the hash table!
  // printf ("Calling Array_Statement_Type (int = %d) constructor! \n",iDef_Statement_Number); 

  // idef statement number!
  // Statement_ID = INITIAL_DEFINITION_NODE_STATEMENT_NUMBER;
     Statement_ID = -iDef_Statement_Number;
  // Region_ID    = Statement_ID;
     Equals_Operation = NULL;

     Prev_List_Element = NULL;
     Tail_Of_List      = NULL;
     Next_List_Element = NULL;
     Next_List_Element_In_Region = NULL;
   }

Array_Statement_Type::Array_Statement_Type ( Expression_Tree_Node_Type* Operation ) 
   { 
  // Initialize the Satement number for later dependence analysis
     Statement_ID = Statement_ID_Counter++;

  // Initially each region is seperate with the ID of the statement number.
  // Then we do the set union operation to build the minimal collections of sets (regions)!
  // Region_ID    = Statement_ID;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Calling Array_Statement_Type constructor (Statement_ID = %d)! \n",Statement_ID); 
#endif

  // pointer assignment!
     Equals_Operation = Operation;

  // Used for linked lists of statements of independent tasks!
     Next_List_Element_In_Region = NULL;

  // if ( Equals_Operation->Operation_Code != doubleArray::Assignment )
  //    {
  //      printf ("WARNING: Equals_Operation->Operator_Code != doubleArray::Assignment! \n");
  //   // APP_ABORT();
  //    }
 
  // This would execute the list in the FIFO order (Head --> Tail)
     if (Head_Of_List == NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Head_Of_List == NULL! \n");
#endif
          Head_Of_List = this;
        }
       else
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Head_Of_List != NULL! \n");
#endif
          if (Tail_Of_List != NULL)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("Tail_Of_List != NULL! \n");
#endif
               Tail_Of_List->Next_List_Element = this;
             }
            else
             {
               printf ("ERROR: (Tail_Of_List == NULL) \n");
               APP_ABORT();
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Setting Prev_List_Element = Tail_Of_List! \n");
#endif

     Prev_List_Element = Tail_Of_List;
     Tail_Of_List      = this;
     Next_List_Element = NULL;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving constructor! \n");
#endif
   } 

void Array_Statement_Type::Execute_Statement_List () 
   { 
  // This is not really what we want since it executes 
  // the WHOLE list not just a single statement!

     Expression_Tree_Node_Type::Execute_List ();
   } 

void Array_Statement_Type::Display () 
   { 
  // This would execute the list in the FIFO order (Head --> Tail)
     Array_Statement_Type* s = this;
     Expression_Tree_Node_Type* Terminate = (s->Prev_List_Element == NULL) ? NULL : 
                                             s->Prev_List_Element->Equals_Operation;

     printf ("Statement #%d: ",s->Statement_ID);
     for (Expression_Tree_Node_Type* p = s->Equals_Operation; 
          p != Terminate; p=p->Prev_List_Element)
        { 
          if (p == NULL)
             {
               printf ("ERROR: p == NULL! \n");
               APP_ABORT();
             }

          p->Display_Expression(); 
        }
     printf ("\n");
   } 

void Array_Statement_Type::Optimize_Assignment () 
   { 
     if (APP_DEBUG > 3)
          printf ("Inside of Array_Statement_Type::Optimize_Assignment! \n");

  // This would execute the list in the FIFO order (Head --> Tail)

     if (APP_DEBUG > 3)
        {
          printf ("Before Processing -- Display Statement #%d: ",Statement_ID);
          Display();
        }

  // Skip optimizations on non assignment operations (like the replace member function)
     if ( Equals_Operation->Operation_Code == doubleArray::Assignment)
        {
          if (APP_DEBUG > 3)
               printf ("Statement uses an assignment operation! \n");

       // Check if Rhs of assignment operation is a temporary!
          if (Equals_Operation->Is_Rhs_A_Temporary())
             {
               if (APP_DEBUG > 3)
                    printf ("This Statement can have the assignment optimized out! \n");

            // Get the Operand_Storage object holding the temporary so we can modify it
            // to point to the Lhs array (so it will be used as the temporary)! 
            // then loop through all the Expressions and modify the references to
            // the temporary object to us the Lhs (as a view I suppose).
            // We can only follow one branch of the expression tree to eliminate
            // the temporaries (we follow the temporary usage) and we choose
            // the brance arbitrarily in the case of more than one temporary used
            // as an operand!

            // Find the temporary to eliminate (if there are two then this function just chooses one)!
               int Temporary_Operand_ID = Equals_Operation->Get_Rhs_Temporary_Operand_ID();

               if (APP_DEBUG > 3)
                    printf ("Temporary_Operand_ID = %d \n",Temporary_Operand_ID);
               
               Equals_Operation->Replace_Temporary_Result_With_Lhs_Array ( Equals_Operation->Get_Result_Array() , Temporary_Operand_ID );
               Equals_Operation->Skip_MDI_Function();
               Optimization_Manager::Number_Of_Assignments_Optimized_Out++;
             }
            else
             {
               if (APP_DEBUG > 3)
                    printf ("Statement does not have a temporary on the Rhs to optimize out! \n");
             }
        }
       else
        {
          if (APP_DEBUG > 3)
               printf ("Statement NOT terminated using assignment (used replace, +=, -=, etc.  or something else)! \n");
        }

     if (APP_DEBUG > 3)
        {
          printf ("After Processing -- Display Statement #%d: ",Statement_ID);
          Display();
        }

     if (APP_DEBUG > 3)
          printf ("Leaving Array_Statement_Type::Optimize_Assignment! \n");
   } 


void Array_Statement_Type::Display_Statement_List ()
   {
  // This would execute the list in the FIFO order (Head --> Tail)
     printf ("\n");
     printf ("\n");
     printf ("Print out the arrays IDs used in each statement! \n");
     printf ("\n");
     Array_Statement_Type *s = NULL;
     for (s = Head_Of_List; s != NULL; s=s->Next_List_Element)
        {
          s->Display();
        }
     printf ("\n");
     printf ("\n");
   }

inline Array_Statement_Type *Array_Statement_Type::Put_Array_ID_Into_Dependence_Hash_Table_And_Test_For_Collision (
     int Lhs_Operand_ID , int Statement_ID )
   {
  // We have to pass the Statement_ID in because we must build the coresponding idef node for that
  // statement.  This is more efficent that building a huge array of idef nodes!

     Array_Statement_Type *Return_Statement_Pointer = NULL;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if ( (Lhs_Operand_ID < 0) || (Lhs_Operand_ID >= MAX_HASH_TABLE_SIZE) )
        {
          printf ("ERROR: (Lhs_Operand_ID < 0) || (Lhs_Operand_ID >= MAX_HASH_TABLE_SIZE) in Array_Statement_Type::Test_Array_ID_For_Collision_In_Dependence_Hash_Table! \n");
          APP_ABORT();
        }
#endif

     if (Hash_Table_Of_Statement_Pointers [Lhs_Operand_ID] == NULL)
        {
       // printf ("Building an Array_Statement_Type to be the idef node! \n");
          Hash_Table_Of_Statement_Pointers [Lhs_Operand_ID] = new Array_Statement_Type ( Statement_ID );
        }

     Return_Statement_Pointer = Hash_Table_Of_Statement_Pointers [Lhs_Operand_ID];

#if COMPILE_DEBUG_STATEMENTS
     if (Hash_Table_Of_Statement_Pointers [Lhs_Operand_ID]->Statement_ID == INITIAL_DEFINITION_NODE_STATEMENT_NUMBER)
        {
          if (APP_DEBUG > 3)
               printf ("Hash_Table_Of_Statement_Pointers [%d]->Statement_ID = INITIAL_DEFINITION_NODE_STATEMENT_NUMBER = %d \n",
                    Lhs_Operand_ID,INITIAL_DEFINITION_NODE_STATEMENT_NUMBER);
        }
       else
        {
          if (APP_DEBUG > 3)
               printf ("Hash_Table_Of_Statement_Pointers [%d]->Statement_ID = %d \n",
                    Lhs_Operand_ID , Hash_Table_Of_Statement_Pointers [Lhs_Operand_ID]->Statement_ID);
        }
#endif

  // Put entry into hash table (by entering the current statement(this))!
     Hash_Table_Of_Statement_Pointers [Lhs_Operand_ID] = this;
        
     return Return_Statement_Pointer;
   }

inline Array_Statement_Type *Array_Statement_Type::Test_Array_ID_For_Collision_In_Dependence_Hash_Table ( 
     int Rhs_Operand_ID , int Statement_ID )
   {
  // We have to pass the Statement_ID in because we must build the coresponding idef node for that
  // statement.  This is more efficent that building a huge array of idef nodes!

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if ( (Rhs_Operand_ID < 0) || (Rhs_Operand_ID >= MAX_HASH_TABLE_SIZE) )
        {
          printf ("ERROR: (Rhs_Operand_ID < 0) || (Rhs_Operand_ID >= MAX_HASH_TABLE_SIZE) in Array_Statement_Type::Test_Array_ID_For_Collision_In_Dependence_Hash_Table! \n");
          APP_ABORT();
        }
#endif

     if (Hash_Table_Of_Statement_Pointers [Rhs_Operand_ID] == NULL)
        {
       // printf ("Building an Array_Statement_Type to be the idef node! \n");
          Hash_Table_Of_Statement_Pointers [Rhs_Operand_ID] = new Array_Statement_Type ( Statement_ID );
        }

     return Hash_Table_Of_Statement_Pointers [Rhs_Operand_ID];
   }

void Array_Statement_Type::Optimize_Assignment_In_Statement_List () 
   { 
     if (APP_DEBUG > 3)
          printf ("Inside of Array_Statement_Type::Optimize_Assignment () \n");

  // This would execute the list in the FIFO order (Head --> Tail)
     Array_Statement_Type* s = NULL;
     for (s = Head_Of_List; s != NULL; s=s->Next_List_Element)
        { 
          bool Optimize_Assigment_In_This_Statement = TRUE;
          if (APP_DEBUG > 3)
               printf ("Going to optimize assignment for operation with Operation_Code = %d \n",s->Equals_Operation->Operation_Code); 

          if (APP_DEBUG > 3)
             {
               printf ("Look at statement #%d: ",s->Statement_ID);
               s->Display();
             }

       // We have to process the Lhs after the Rhs to avoid self reference in the depencence result!

          int Lhs_Operand_ID   = s->Equals_Operation->Get_Lhs_Array_ID ();

          if (APP_DEBUG > 3)
               printf ("Lhs_Operand_ID = %d \n",Lhs_Operand_ID);

          Expression_Tree_Node_Type* Terminate = (s->Prev_List_Element == NULL) ? NULL : 
                                                  s->Prev_List_Element->Equals_Operation;

          for (Expression_Tree_Node_Type* p = s->Equals_Operation; 
               p != Terminate; p=p->Prev_List_Element)
             { 
               if (p == NULL)
                  {
                    printf ("ERROR: p == NULL! \n");
                    APP_ABORT();
                  }

            // The first element of a statement is given a -1 Array_ID since it is
            // the Lhs for the statement and we want to avoid counting it twice!
               int Rhs_Operand_ID [MAX_NUMBER_OF_RHS_ARRAYS];
               p->Get_Rhs_Array_ID ( Rhs_Operand_ID ); 

               for (int i=0; i < MAX_NUMBER_OF_RHS_ARRAYS; i++)
                  {
                    if (APP_DEBUG > 3)
                         printf ("Now look at the Rhs objects (look for anti-dependence of Rhs on Lhs within the statement!) \n");

                 // Only look at valid Rhs (also acounts for the fact that each execution object can have
                 // an unspecified number of arguments (less than MAX_NUMBER_OF_RHS_ARRAYS)).
                    if (Rhs_Operand_ID [i] >= 0)
                       {
                         if (APP_DEBUG > 3)
                              printf ("Rhs_Operand_ID [%d] = %d \n",i,Rhs_Operand_ID [i]);

                         if (Rhs_Operand_ID [i] == Lhs_Operand_ID)
                            {
                              if (APP_DEBUG > 3)
                                   printf ("Rhs_Operand_ID [i] == Lhs_Operand_ID = %d -- Can't optimize out the assignment! \n",Lhs_Operand_ID);
                              Optimize_Assigment_In_This_Statement = FALSE;
                            }
                       }
                      else
                       {
                         if (APP_DEBUG > 7)
                              printf ("There is no Rhs operand in this position Rhs_Operand_ID [%d] = %d \n",i,Rhs_Operand_ID [i]);
                       }
                  }
             }

          if (Optimize_Assigment_In_This_Statement)
             {
               if (APP_DEBUG > 3)
                  {
                    printf ("*** Assignment might be optimized out! -- Pass to Optimize_Assignment! \n");
                    printf ("############################################ \n");
                  }

               s->Optimize_Assignment();

               if (APP_DEBUG > 3)
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
             }
            else
             {
               if (APP_DEBUG > 3)
                    printf ("*** Anti-Dependence found - can't optimize out assignement! \n");
             }
        }
   }

void Array_Statement_Type::Build_Dependence_Info () 
   { 
     Array_Statement_Type *Dependence_Statement = NULL;

     if (APP_DEBUG > 3)
          printf ("Inside of Array_Statement_Type::Build_Dependence_Info () \n");

  // Setup idef nodes for dependence analysis!
  // Initialize the hash table used for the dependence analysis
  // Instead of initializing all of them we can just initilize the highest
  // grid ID of them!
     int Max_Number_Of_Table_Entries = APP_Global_Array_ID+1;

  // error checking
     if ( Max_Number_Of_Table_Entries >= MAX_HASH_TABLE_SIZE )
        {
          printf ("ERROR: in Array_Statement_Type::Build_Dependence_Info -- Max_Number_Of_Table_Entries >= MAX_HASH_TABLE_SIZE! \n");
          APP_ABORT();
        }

  // Initialize the Hash Table to NULL!
     for (int Index=0; Index < Max_Number_Of_Table_Entries; Index++)
          Hash_Table_Of_Statement_Pointers [Index] = NULL;

  // This would execute the list in the FIFO order (Head --> Tail)
     Array_Statement_Type* s = NULL;
     for (s = Head_Of_List; s != NULL; s=s->Next_List_Element)
        { 
          if (APP_DEBUG > 3)
               printf ("Going to find dependence for operation with Operation_Code = %d \n",s->Equals_Operation->Operation_Code); 
          Expression_Tree_Node_Type* Terminate = (s->Prev_List_Element == NULL) ? 
                                                  NULL : 
                                                  s->Prev_List_Element->Equals_Operation;

          for (Expression_Tree_Node_Type* p = s->Equals_Operation; 
               p != Terminate; p=p->Prev_List_Element)
             { 
               if (p == NULL)
                  {
                    printf ("ERROR: p == NULL! \n");
                    APP_ABORT();
                  }

            // The first element of a statement is given a -1 Array_ID since it is
            // the Lhs for the statement and we want to avoid counting it twice!
               int Rhs_Operand_ID [MAX_NUMBER_OF_RHS_ARRAYS];

               p->Get_Rhs_Array_ID ( Rhs_Operand_ID ); 

               for (int i=0; i < MAX_NUMBER_OF_RHS_ARRAYS; i++)
                  {
                    if (APP_DEBUG > 3)
                         printf ("Now look at the Rhs objects (look for flow dependence!) \n");

                 // Only look at valid Rhs (also acounts for the fact that each execution object can have
                 // an unspecified number of arguments (less than MAX_NUMBER_OF_RHS_ARRAYS)).
                    if (Rhs_Operand_ID [i] >= 0)
                       {
                         if (APP_DEBUG > 3)
                              printf ("Rhs_Operand_ID [%d] = %d \n",i,Rhs_Operand_ID [i]);
                         Dependence_Statement = s->Test_Array_ID_For_Collision_In_Dependence_Hash_Table ( 
                                                     Rhs_Operand_ID [i] , s->Statement_ID );

                         if (Dependence_Statement == NULL)
                            {
                              printf ("Dependence NOT FOUND \n");
                              printf ("ERROR: Should have found a dependence on at least an idef node! \n");
                              APP_ABORT();
                            }
                           else
                            {
                              if (APP_DEBUG > 3)
                                   printf ("Dependence FOUND on Statement %d %s \n",Dependence_Statement->Statement_ID,
                                        (Dependence_Statement->Statement_ID < 0) ? "(idef node)" : "" );
                            }

                      // Setup pointer to statement on which this statement depends!
                         p->Flow_Dependence_Predesessor [i] = Dependence_Statement;
                       }
                      else
                       {
                         if (APP_DEBUG > 3)
                              printf ("There is not Rhs operand in this position %d \n",Rhs_Operand_ID [i]);
                         p->Flow_Dependence_Predesessor [i] = NULL;
                       }
                  }
             }

          if (APP_DEBUG > 3)
               printf ("Test 4 \n");

       // We have to process the Lhs after the Rhs to avoid self reference in the depencence result!

          int Lhs_Operand_ID   = s->Equals_Operation->Get_Lhs_Array_ID ();
          Dependence_Statement = s->Put_Array_ID_Into_Dependence_Hash_Table_And_Test_For_Collision ( 
                                      Lhs_Operand_ID , s->Statement_ID );
          s->Equals_Operation->Output_Dependence_Predesessor = Dependence_Statement;
        }

#if 0
  // Free up memory from allocation of idef node!
  // delete Initial_Definition_Statement;
     for (Index=0; Index < MAX_HASH_TABLE_SIZE; Index++)
        {
          if (Hash_Table_Of_Statement_Pointers [Index]->Statement_ID < 0) // then it is an idef node
               delete Hash_Table_Of_Statement_Pointers [Index];
        }
#endif
   }

void Array_Statement_Type::Display_Dependence_Info () 
   { 
     printf ("Inside of Array_Statement_Type::Display_Dependence_Info () \n");

  // This would execute the list in the FIFO order (Head --> Tail)
     Array_Statement_Type* s = NULL;
     for (s = Head_Of_List; s != NULL; s=s->Next_List_Element)
        { 
       // printf ("Going to find dependence for operation with Operation_Code = %d \n",s->Equals_Operation->Operation_Code); 
          Expression_Tree_Node_Type* Terminate = (s->Prev_List_Element == NULL) ? 
                                                  NULL : 
                                                  s->Prev_List_Element->Equals_Operation;

          for (Expression_Tree_Node_Type* p = s->Equals_Operation; 
               p != Terminate; p=p->Prev_List_Element)
             { 
               if (p == NULL)
                  {
                    printf ("ERROR: p == NULL! \n");
                    APP_ABORT();
                  }

               if (p->Output_Dependence_Predesessor != NULL)
                  {
                    printf ("Statement #%d:  Output Dependence ( %d , %d ) \n",
                         s->Statement_ID,s->Statement_ID,
                         p->Output_Dependence_Predesessor->Statement_ID);
                  }
                 else
                  {
                 // printf ("Statement #%d:  No more Output dependences on statement %d \n",s->Statement_ID,s->Statement_ID);
                  }

               for (int i=0; i < MAX_NUMBER_OF_RHS_ARRAYS; i++)
                  {
                 // printf ("Now look at the Rhs objects (look for flow dependence!) \n");
                    if (p->Flow_Dependence_Predesessor [i] != NULL)
                       {
                         printf ("Statement #%d:  Flow Dependence   ( %d , %d ) \n",
                              s->Statement_ID,s->Statement_ID,
                              p->Flow_Dependence_Predesessor [i]->Statement_ID);
                       }
                      else
                       {
                      // printf ("Statement #%d:  No more Flow dependences on statement %d \n",s->Statement_ID,s->Statement_ID);
                       }
                  }
             }
          printf ("\n");
        }
   } 

Set_Of_Tasks Array_Statement_Type::Find_Independent_Sets_Of_Statements () 
   { 
     const int NUMBER_OF_STATEMENTS = Statement_ID_Counter+1;

  // APP_DEBUG = 5;

     if (APP_DEBUG > 3)
          printf ("Inside of Array_Statement_Type::Find_Independent_Sets_Of_Statements() \n");

     if (Head_Of_List == NULL)
        {
          printf ("ERROR: in Array_Statement_Type::Find_Independent_Sets_Of_Statements -- Head_Of_List == NULL! \n");
          APP_ABORT();
        }

  // long Start_Clock_0 = clock();
  // I suspect an error here!
  // Optimize_Assignment_In_Statement_List ();
  // long End_Clock_0 = clock();
  // printf ("Time for Array_Statement_Type::Optimize_Assignment_In_Statement_List () %ld \n", (End_Clock_0-Start_Clock_0) / 1000 );

  // long Start_Clock_1 = clock();
  // Optimize_Binary_Operators_To_Aggregate_Operators_In_Statement_List ();
  // long End_Clock_1 = clock();
  // printf ("Time for Array_Statement_Type::Optimize_Binary_Operators_To_Aggregate_Operators_In_Statement_List () %ld \n", (End_Clock_1-Start_Clock_1) / 1000 );

     if (APP_DEBUG > 3)
          printf ("NOW DO THE DEPENDENCE ANALYSIS! \n");

     long Start_Clock_2 = clock();
     Build_Dependence_Info ();
     long End_Clock_2 = clock();
     printf ("Time for Array_Statement_Type::Build_Dependence_Info () %ld \n", (End_Clock_2-Start_Clock_2) / 1000 );

  // This is a nearly order n operation (using this algorithm) for set union! 

     Set_Union_Type Set ( NUMBER_OF_STATEMENTS );

   // Look at each array statement!
     Array_Statement_Type* s = NULL;
     for (s = Head_Of_List; s != NULL; s=s->Next_List_Element)
        { 
          Expression_Tree_Node_Type* Terminate = (s->Prev_List_Element == NULL) ? NULL : 
                                                  s->Prev_List_Element->Equals_Operation;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("New Statement: Statement_ID = %d  \n",s->Statement_ID);
#endif

        // Look at each array experession (operation) in the current statement!
          for (Expression_Tree_Node_Type* p = s->Equals_Operation; 
               p != Terminate; p=p->Prev_List_Element)
             { 
            // error checking
               if (p == NULL)
                  {
                    printf ("ERROR: p == NULL! \n");
                    APP_ABORT();
                  }

            // Is there an output dependence (there is if p->Output_Dependence_Predesessor != NULL)
               if (p->Output_Dependence_Predesessor != NULL)
                  {
                 // int Target_Region_ID = 0;
                 // int New_Region_ID    = 0;

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("p->Output_Dependence_Predesessor->Statement_ID = %d \n",p->Output_Dependence_Predesessor->Statement_ID);
#endif

                    if (p->Output_Dependence_Predesessor->Statement_ID > 0)
                       {
                      // Then the dependence is on a previous statement!
                      // Call fast find based on who I depend upon!
                         Set.Fast_Union ( s->Statement_ID , Set.Fast_Find ( p->Output_Dependence_Predesessor->Statement_ID ) );
                       }
                      else
                       {
                      // The dependence is on an idef node! So initialize idef.Region_ID with the current Statement_ID!
                         Set.Fast_Union ( s->Statement_ID , Set.Fast_Find ( -(p->Output_Dependence_Predesessor->Statement_ID) ) );
                       }
                  }

            // Now look for flow dependences (checking all the Rhs array ids for each expression)!
               for (int i=0; i < MAX_NUMBER_OF_RHS_ARRAYS; i++)
                  {
                 // Is there a flow dependence (there is if p->Flow_Dependence_Predesessor[i] != NULL)
                    if (p->Flow_Dependence_Predesessor [i] != NULL)
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 3)
                              printf ("p->Flow_Dependence_Predesessor [%d]->Statement_ID = %d \n",i,p->Flow_Dependence_Predesessor [i]->Statement_ID);
#endif
                      // Check for idef node
                         if (p->Flow_Dependence_Predesessor [i]->Statement_ID > 0)
                            {
                           // Then the dependence is on a previous statement!
                           // Call fast find based on who I depend upon!
                           // We have to call Fast_Find on our current statement because the last
                           // iteration might have reset our Region_ID
                              Set.Fast_Union ( Set.Fast_Find ( s->Statement_ID ) , Set.Fast_Find ( p->Flow_Dependence_Predesessor[i]->Statement_ID ) );
                            }
                           else
                            {
                           // The dependence is on an idef node! So initialize idef.Region_ID with the current Statement_ID!
                           // We have to call Fast_Find on our current statement because the last
                           // iteration might have reset our Region_ID
                              Set.Fast_Union ( Set.Fast_Find ( s->Statement_ID ) , Set.Fast_Find ( -(p->Flow_Dependence_Predesessor[i]->Statement_ID) ) );
                            }
                       }
                  }
             }
        }

  // END OF SET UNION ALGORITHM!
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("END OF SET UNION ALGORITHM! \n");
#endif

  // Now collect the independent elements into seperate sets!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf("Now collect the independent elements into seperate sets! \n");
#endif

  // This allows the aray sizes to be dynamic!
     Array_Statement_Type **Task_Head_Array = new Array_Statement_Type* [NUMBER_OF_STATEMENTS];
     Array_Statement_Type **Task_Tail_Array = new Array_Statement_Type* [NUMBER_OF_STATEMENTS];

     for (int i=0; i < NUMBER_OF_STATEMENTS; i++)
        {
          Task_Head_Array [i] = NULL;
          Task_Tail_Array [i] = NULL;
        }

#if 1
     for (s = Head_Of_List; s != NULL; s=s->Next_List_Element)
        {
          int Region_ID = Set.Fast_Find(s->Statement_ID);

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Looking at Statement %d of region %d \n",s->Statement_ID,Region_ID);
#endif

          if (Task_Tail_Array [Region_ID] == NULL)
             {
            // Now record the head of the list!

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("Building a new list for a new independent task! \n");
#endif

               Task_Head_Array [Region_ID] = s;
               Task_Tail_Array [Region_ID] = s;
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("Adding statement to existing task list! \n");
#endif
               if (Task_Tail_Array [Region_ID]->Next_List_Element_In_Region == NULL)
                  {
                    Task_Tail_Array [Region_ID]->Next_List_Element_In_Region = s;
                    Task_Tail_Array [Region_ID] = Task_Tail_Array [Region_ID]->Next_List_Element_In_Region;
                 // printf ("New element added at end of task list [%d] \n",Region_ID);
                  }
                 else
                  {
                 // Task_Tail_Array [Region_ID] = Task_Tail_Array [Region_ID]->Next_List_Element_In_Region;
                    printf ("ERROR: Should have been NULL! \n");
                    APP_ABORT();
                  }
             }
        }
#endif

  // Build set of isolated tasks

     Set_Of_Tasks Task_Set;

     for (int Region_Index=0; Region_Index < NUMBER_OF_STATEMENTS; Region_Index++)
        {
          if (Task_Head_Array [Region_Index] != NULL)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Statement List for Region_Index %d \n",Region_Index);
#endif

               Task_Set.Add_Task ( new Task_Type ( Task_Head_Array [Region_Index] ) );
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving Array_Statement_Type::Find_Independent_Sets_Of_Statements () \n");
#endif

  // free memory used to hold task lists before they were put into the Task_Set!
     delete Task_Head_Array;
     delete Task_Tail_Array;

     return Task_Set;
   } 


#if COMPILE_AGGREGATE_OPERATOR_OPTIMIZATIONS

// ****************************************************************************************
// ****************************************************************************************
// **************  Optimizations Related to Aggreagate Operators **************************
// ****************************************************************************************
// ****************************************************************************************

// assumptions:  0 - 3 are codes for +,-,* and /
// -1 is not a legal op_code
// op codes currently don't use numbers over 2000

#define TREE_SIZE 45
#define EXTRA_SIZE 2
#define FIX_OPS 4

#if 0
void Array_Statement_Type::Test_Case ()
   {
  // This function is used for testing the recognition of aggregate operators

     int op_len  = 8;
     int agg_len =  0;
     struct agg_struct agg_result[10];

     struct op_struct test_ops[15] = 
         {
          {37, 42, 16, 0},
          {0, 112, 111, 110},
          {0, 111, 3, 4},
          {0, 110, 1, 2},  /* (A + B) + (C + D) */
          {3, 200, 202, 203},
          {0, 102, 101, 4},
          {0, 101, 100, 3},
          {0, 100, 2, 3}   /* A + (B + C) + D */
        };

     for (int i = 0; i < op_len; i++)
        {
          printf("\n initial_op: %d ",test_ops[i].op_code);
        }

     Find_Aggregate_Operators (test_ops, op_len, agg_result, agg_len);

     for (i = 0; i < agg_len; i++)
        {
          printf("\n agg_op: %d start: %d end: %d",agg_result[i].agg_code,
 	        agg_result[i].start_code,agg_result[i].end_code);
        }
   }
#endif

// assumptions:  0 - 3 are codes for +,-,* and /  
// -1 is not a legal op_code  
// op codes currently don't use numbers over 2000 

void xMDI_d_2Plus ( int Number_Of_Operators, doubleArray & Result, doubleArray* Op[3] ) 
   {
     bool Optimizable = FALSE;

     if (Optimizable)
        {
       // MDI_d_AGG_2Plus (&Result,Op);
        }
       else
        {
       // In the worst case we use the binary operations
          Result = *Op[0] + *Op[1] + *Op[2];
        }
   }

void MDI_d_2Plus ( int Number_Of_Operators, doubleArray & Result, doubleArray* Op[3] ) 
   {
     double* Result_Pointer = Result.getDataPointer();
     double* Op1_Pointer    = Op[0]->getDataPointer();
     double* Op2_Pointer    = Op[1]->getDataPointer();
     double* Op3_Pointer    = Op[2]->getDataPointer();

     int Number_Of_Elements = Result.elementCount();

     for (int i=0; i < Number_Of_Elements; i++)
        {
          Result_Pointer[i] = Op1_Pointer[i] + Op2_Pointer[i] + Op3_Pointer[i];
        }
   }

void MDI_d_3Plus ( int Number_Of_Operators, doubleArray & Result, doubleArray* Operand_List [4] )
   {
     double* Result_Ptr = Result.getDataPointer();
     double* Op0_Ptr    = Operand_List[0]->getDataPointer();
     double* Op1_Ptr    = Operand_List[1]->getDataPointer();
     double* Op2_Ptr    = Operand_List[2]->getDataPointer();
     double* Op3_Ptr    = Operand_List[3]->getDataPointer();

     int Number_Of_Elements = Result.elementCount();

     for (int i=0; i < Number_Of_Elements; i++)
        {
          Result_Ptr[i] = Op0_Ptr[i] + Op1_Ptr[i] + Op2_Ptr[i] + Op3_Ptr[i];
        }
   }

void MDI_d_nPlus ( int Number_Of_Operators, doubleArray & Result, doubleArray* Operand_List [33] )
   {
     int i = 0;
     double* Op[33];
     register double Temp;

     double* Result_Ptr = Result.getDataPointer();

     printf ("Inside of MDI_d_nPlus! Number_Of_Operators = %d \n",Number_Of_Operators);

     for (int n=0; n <= Number_Of_Operators; n++)
          Op[n] = Operand_List[n]->getDataPointer();

     int Number_Of_Elements = Result.elementCount();

  // What sorts of optimizations are possible here?
     for (i=0; i < Number_Of_Elements; i++)
        {
          Temp = 0;
          for (int n=0; n <= Number_Of_Operators; n++)
               Temp += Op[n][i];
          Result_Ptr[i] = Temp;
        }
   }

void MDI_d_4Plus ( int Number_Of_Operators, doubleArray & Result, doubleArray* Operand_List [5] )
   {
     int i = 0;
     double* Result_Ptr = Result.getDataPointer();
     double* Op0_Ptr    = Operand_List[0]->getDataPointer();
     double* Op1_Ptr    = Operand_List[1]->getDataPointer();
     double* Op2_Ptr    = Operand_List[2]->getDataPointer();
     double* Op3_Ptr    = Operand_List[3]->getDataPointer();
     double* Op4_Ptr    = Operand_List[4]->getDataPointer();

     int Number_Of_Elements = Result.elementCount();

     for (i=0; i < Number_Of_Elements; i++)
        {
          Result_Ptr[i] = Op0_Ptr[i] + Op1_Ptr[i] + Op2_Ptr[i] + Op3_Ptr[i]
                        + Op4_Ptr[i];
        }
   }

void MDI_d_5Plus ( int Number_Of_Operators , doubleArray & Result, doubleArray* Operand_List[6] )
   {
     int i = 0;
     double* Result_Ptr = Result.getDataPointer();
     double* Op0_Ptr    = Operand_List[0]->getDataPointer();
     double* Op1_Ptr    = Operand_List[1]->getDataPointer();
     double* Op2_Ptr    = Operand_List[2]->getDataPointer();
     double* Op3_Ptr    = Operand_List[3]->getDataPointer();
     double* Op4_Ptr    = Operand_List[4]->getDataPointer();
     double* Op5_Ptr    = Operand_List[5]->getDataPointer();
 
     int Number_Of_Elements = Result.elementCount();
 
     for (i=0; i < Number_Of_Elements; i++)
        {
          Result_Ptr[i] = Op0_Ptr[i] + Op1_Ptr[i] + Op2_Ptr[i] + Op3_Ptr[i]
                        + Op4_Ptr[i] + Op5_Ptr[i];
        }
   }
 
void MDI_d_10Plus ( int Number_Of_Operators , doubleArray & Result, doubleArray* Operand_List[11] )
   {
     int i = 0;
     double* Result_Ptr = Result.getDataPointer();
     double* Op0_Ptr    = Operand_List[0]->getDataPointer();
     double* Op1_Ptr    = Operand_List[1]->getDataPointer();
     double* Op2_Ptr    = Operand_List[2]->getDataPointer();
     double* Op3_Ptr    = Operand_List[3]->getDataPointer();
     double* Op4_Ptr    = Operand_List[4]->getDataPointer();
     double* Op5_Ptr    = Operand_List[5]->getDataPointer();
     double* Op6_Ptr    = Operand_List[6]->getDataPointer();
     double* Op7_Ptr    = Operand_List[7]->getDataPointer();
     double* Op8_Ptr    = Operand_List[8]->getDataPointer();
     double* Op9_Ptr    = Operand_List[9]->getDataPointer();
     double* Op10_Ptr   = Operand_List[10]->getDataPointer();
  
     int Number_Of_Elements = Result.elementCount(); 
  
     for (i=0; i < Number_Of_Elements; i++) 
        { 
          Result_Ptr[i] = Op0_Ptr [i] +
               Op1_Ptr [i] + Op2_Ptr [i] + Op3_Ptr [i] + Op4_Ptr [i]
             + Op5_Ptr [i] + Op6_Ptr [i] + Op7_Ptr [i] + Op8_Ptr [i]
             + Op9_Ptr [i] + Op10_Ptr[i];
        } 
   } 

void MDI_d_20Plus ( int Number_Of_Operators , doubleArray & Result, doubleArray* Operand_List[21] )
   {
     int i = 0;
     double* Result_Ptr = Result.getDataPointer();
     double* Op0_Ptr    = Operand_List[0]->getDataPointer();
     double* Op1_Ptr    = Operand_List[1]->getDataPointer();
     double* Op2_Ptr    = Operand_List[2]->getDataPointer();
     double* Op3_Ptr    = Operand_List[3]->getDataPointer();
     double* Op4_Ptr    = Operand_List[4]->getDataPointer();
     double* Op5_Ptr    = Operand_List[5]->getDataPointer();
     double* Op6_Ptr    = Operand_List[6]->getDataPointer();
     double* Op7_Ptr    = Operand_List[7]->getDataPointer();
     double* Op8_Ptr    = Operand_List[8]->getDataPointer();
     double* Op9_Ptr    = Operand_List[9]->getDataPointer();
     double* Op10_Ptr   = Operand_List[10]->getDataPointer();
     double* Op11_Ptr   = Operand_List[11]->getDataPointer();
     double* Op12_Ptr   = Operand_List[12]->getDataPointer();
     double* Op13_Ptr   = Operand_List[13]->getDataPointer();
     double* Op14_Ptr   = Operand_List[14]->getDataPointer();
     double* Op15_Ptr   = Operand_List[15]->getDataPointer();
     double* Op16_Ptr   = Operand_List[16]->getDataPointer();
     double* Op17_Ptr   = Operand_List[17]->getDataPointer();
     double* Op18_Ptr   = Operand_List[18]->getDataPointer();
     double* Op19_Ptr   = Operand_List[19]->getDataPointer();
     double* Op20_Ptr   = Operand_List[20]->getDataPointer();
  
     int Number_Of_Elements = Result.elementCount(); 
  
     for (i=0; i < Number_Of_Elements; i++) 
        { 
          Result_Ptr[i] = Op0_Ptr [i] +
               Op1_Ptr [i] + Op2_Ptr [i] + Op3_Ptr [i] + Op4_Ptr [i]
             + Op5_Ptr [i] + Op6_Ptr [i] + Op7_Ptr [i] + Op8_Ptr [i]
             + Op9_Ptr [i] + Op10_Ptr[i] + Op11_Ptr[i] + Op12_Ptr[i]
             + Op13_Ptr[i] + Op14_Ptr[i] + Op15_Ptr[i] + Op16_Ptr[i]
             + Op17_Ptr[i] + Op18_Ptr[i] + Op19_Ptr[i] + Op20_Ptr[i];
        } 
   } 

void MDI_d_26Plus ( int Number_Of_Operators , doubleArray & Result, doubleArray* Operand_List[27] )
   {
     int i = 0;
     double* Result_Ptr = Result.getDataPointer();
     double* Op0_Ptr    = Operand_List[0]->getDataPointer();
     double* Op1_Ptr    = Operand_List[1]->getDataPointer();
     double* Op2_Ptr    = Operand_List[2]->getDataPointer();
     double* Op3_Ptr    = Operand_List[3]->getDataPointer();
     double* Op4_Ptr    = Operand_List[4]->getDataPointer();
     double* Op5_Ptr    = Operand_List[5]->getDataPointer();
     double* Op6_Ptr    = Operand_List[6]->getDataPointer();
     double* Op7_Ptr    = Operand_List[7]->getDataPointer();
     double* Op8_Ptr    = Operand_List[8]->getDataPointer();
     double* Op9_Ptr    = Operand_List[9]->getDataPointer();
     double* Op10_Ptr   = Operand_List[10]->getDataPointer();
     double* Op11_Ptr   = Operand_List[11]->getDataPointer();
     double* Op12_Ptr   = Operand_List[12]->getDataPointer();
     double* Op13_Ptr   = Operand_List[13]->getDataPointer();
     double* Op14_Ptr   = Operand_List[14]->getDataPointer();
     double* Op15_Ptr   = Operand_List[15]->getDataPointer();
     double* Op16_Ptr   = Operand_List[16]->getDataPointer();
     double* Op17_Ptr   = Operand_List[17]->getDataPointer();
     double* Op18_Ptr   = Operand_List[18]->getDataPointer();
     double* Op19_Ptr   = Operand_List[19]->getDataPointer();
     double* Op20_Ptr   = Operand_List[20]->getDataPointer();
     double* Op21_Ptr   = Operand_List[21]->getDataPointer();
     double* Op22_Ptr   = Operand_List[22]->getDataPointer();
     double* Op23_Ptr   = Operand_List[23]->getDataPointer();
     double* Op24_Ptr   = Operand_List[24]->getDataPointer();
     double* Op25_Ptr   = Operand_List[25]->getDataPointer();
     double* Op26_Ptr   = Operand_List[26]->getDataPointer();
 
     int Number_Of_Elements = Result.elementCount();
 
     for (i=0; i < Number_Of_Elements; i++)
        {
          Result_Ptr[i] = Op0_Ptr [i] +
               Op1_Ptr [i] + Op2_Ptr [i] + Op3_Ptr [i] + Op4_Ptr [i]
             + Op5_Ptr [i] + Op6_Ptr [i] + Op7_Ptr [i] + Op8_Ptr [i]
             + Op9_Ptr [i] + Op10_Ptr[i] + Op11_Ptr[i] + Op12_Ptr[i]
             + Op13_Ptr[i] + Op14_Ptr[i] + Op15_Ptr[i] + Op16_Ptr[i]
             + Op17_Ptr[i] + Op18_Ptr[i] + Op19_Ptr[i] + Op20_Ptr[i]
             + Op21_Ptr[i] + Op22_Ptr[i] + Op23_Ptr[i] + Op24_Ptr[i]
             + Op25_Ptr[i] + Op26_Ptr[i];
        }
   }


void MDI_d_32Plus ( int Number_Of_Operators , doubleArray & Result, doubleArray* Operand_List[32] )
   {
     int i = 0;
     double* Result_Ptr = Result.getDataPointer();
     double* Op0_Ptr    = Operand_List[0]->getDataPointer();
     double* Op1_Ptr    = Operand_List[1]->getDataPointer();
     double* Op2_Ptr    = Operand_List[2]->getDataPointer();
     double* Op3_Ptr    = Operand_List[3]->getDataPointer();
     double* Op4_Ptr    = Operand_List[4]->getDataPointer();
     double* Op5_Ptr    = Operand_List[5]->getDataPointer();
     double* Op6_Ptr    = Operand_List[6]->getDataPointer();
     double* Op7_Ptr    = Operand_List[7]->getDataPointer();
     double* Op8_Ptr    = Operand_List[8]->getDataPointer();
     double* Op9_Ptr    = Operand_List[9]->getDataPointer();
     double* Op10_Ptr   = Operand_List[10]->getDataPointer();
     double* Op11_Ptr   = Operand_List[11]->getDataPointer();
     double* Op12_Ptr   = Operand_List[12]->getDataPointer();
     double* Op13_Ptr   = Operand_List[13]->getDataPointer();
     double* Op14_Ptr   = Operand_List[14]->getDataPointer();
     double* Op15_Ptr   = Operand_List[15]->getDataPointer();
     double* Op16_Ptr   = Operand_List[16]->getDataPointer();
     double* Op17_Ptr   = Operand_List[17]->getDataPointer();
     double* Op18_Ptr   = Operand_List[18]->getDataPointer();
     double* Op19_Ptr   = Operand_List[19]->getDataPointer();
     double* Op20_Ptr   = Operand_List[20]->getDataPointer();
     double* Op21_Ptr   = Operand_List[21]->getDataPointer();
     double* Op22_Ptr   = Operand_List[22]->getDataPointer();
     double* Op23_Ptr   = Operand_List[23]->getDataPointer();
     double* Op24_Ptr   = Operand_List[24]->getDataPointer();
     double* Op25_Ptr   = Operand_List[25]->getDataPointer();
     double* Op26_Ptr   = Operand_List[26]->getDataPointer();
     double* Op27_Ptr   = Operand_List[27]->getDataPointer();
     double* Op28_Ptr   = Operand_List[28]->getDataPointer();
     double* Op29_Ptr   = Operand_List[29]->getDataPointer();
     double* Op30_Ptr   = Operand_List[30]->getDataPointer();
     double* Op31_Ptr   = Operand_List[31]->getDataPointer();
     double* Op32_Ptr   = Operand_List[32]->getDataPointer();
  
     int Number_Of_Elements = Result.elementCount(); 
  
     for (i=0; i < Number_Of_Elements; i++) 
        { 
          Result_Ptr[i] = Op0_Ptr [i] +
               Op1_Ptr [i] + Op2_Ptr [i] + Op3_Ptr [i] + Op4_Ptr [i]
             + Op5_Ptr [i] + Op6_Ptr [i] + Op7_Ptr [i] + Op8_Ptr [i]
             + Op9_Ptr [i] + Op10_Ptr[i] + Op11_Ptr[i] + Op12_Ptr[i]
             + Op13_Ptr[i] + Op14_Ptr[i] + Op15_Ptr[i] + Op16_Ptr[i]
             + Op17_Ptr[i] + Op18_Ptr[i] + Op19_Ptr[i] + Op20_Ptr[i]
             + Op21_Ptr[i] + Op22_Ptr[i] + Op23_Ptr[i] + Op24_Ptr[i]
             + Op25_Ptr[i] + Op26_Ptr[i] + Op27_Ptr[i] + Op28_Ptr[i]
             + Op29_Ptr[i] + Op30_Ptr[i] + Op31_Ptr[i] + Op32_Ptr[i];
        } 
   } 
 
void Array_Statement_Type::Find_Aggregate_Operators ( struct op_struct  *op_info  , int num_codes, 
                                                      struct agg_struct *agg_codes, int & agg_len )
   {
  // This function was developed by Rebecca Parsons to provide the mapping of 
  // the single operators into predefined aggregate operators.  The use of the
  // aggregate operators provides repreive from the poor regester utilization 
  // of the binary operator usage.

  // this function translates op codes into aggregate operators 
  // when there is a connection between the result of one       
  // operation feeds into an operand of the next and that this  
  // operator combination is represented in the table.          

     struct fstruct 
        {
          int op_code;
          int table_pos;
        };

     struct tnode 
        {
          int agg_operator;
          int fix[FIX_OPS];
          int rest_start;
          int rest_cnt;
          void *agg_function;
       // struct MDI_d_Prototype_Aggregate *agg_function;
        };


     static struct tnode table[TREE_SIZE] = 
 /* 0*/ { {  -1, { 1,  2,  3,  4}, -1, 0, NULL},  // root node pointers to single ops 
 /* 1*/   {   0, { 5, -1,  6, -1}, -1, 0, NULL},  // node for single plus 
 /* 2*/   {   1, {-1, -1, -1, -1}, -1, 0, NULL},  // node for single minus 
 /* 3*/   {   2, { 7, -1, -1, -1}, -1, 0, NULL},  // node for single times 
 /* 4*/   {   3, {-1, -1, -1, -1}, -1, 0, NULL},  // node for single division 
 /* 5*/   {2001, { 8, -1, -1, -1},  0, 1, (void*) MDI_d_2Plus },  // plus plus 
 /* 6*/   {2002, {-1, -1, -1, -1}, -1, 0, NULL},  // plus times 
 /* 7*/   {2003, {-1, -1, -1, -1}, -1, 0, NULL},  // times plus 
 /* 8*/   {2004, { 9, -1, -1, -1}, -1, 0, (void*) MDI_d_3Plus },  // plus plus plus
 /* 9*/   {2005, {10, -1, -1, -1}, -1, 0, (void*) MDI_d_4Plus },  // plus plus plus plus 
 /*10*/   {2006, {11, -1, -1, -1}, -1, 0, (void*) MDI_d_5Plus },  // plus plus plus plus plus 
 /*11*/   {2007, {12, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // # pluses = # code - 1
 /*12*/   {2008, {13, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*13*/   {2009, {14, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*14*/   {2010, {15, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*15*/ //{2011, {16, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*15*/   {2011, {16, -1, -1, -1}, -1, 0, (void*) MDI_d_10Plus },  // plus plus plus plus plus 
 /*16*/   {2012, {17, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*17*/   {2013, {18, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*18*/   {2014, {19, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*19*/   {2015, {20, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*20*/   {2016, {21, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*21*/   {2017, {22, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*22*/   {2018, {23, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*23*/   {2019, {24, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*24*/   {2020, {25, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*25*/ //{2021, {26, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*25*/   {2021, {26, -1, -1, -1}, -1, 0, (void*) MDI_d_20Plus },  // plus plus plus plus plus 
 /*26*/   {2022, {27, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*27*/   {2023, {28, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*28*/   {2024, {29, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*29*/   {2025, {30, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*30*/   {2026, {31, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus plus plus plus 
 /*31*/   {2027, {32, -1, -1, -1}, -1, 0, (void*) MDI_d_26Plus },  // plus plus plus plus plus 
 /*32*/   {2028, {33, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // this is the place for the 27+ code  
 /*33*/   {2029, {34, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // this is the place for the 28+ code  
 /*34*/   {2030, {35, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // this is the place for the 29+ code  
 /*35*/   {2031, {36, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // this is the place for the 30+ code  
 /*36*/   {2032, {37, -1, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // this is the place for the 31+ code  
 /*37*/   {2033, {-1, -1, -1, -1}, -1, 0, (void*) MDI_d_32Plus },  // this is the place for the 32+ code  
 /*38*/   {1999, {-1, 39, -1, -1}, -1, 0, (void*) MDI_d_nPlus },  // plus plus 15 
 /*39*/   {1998, {-1, -1, -1, -1}, -1, 0, NULL},  // plus plus 15 -
        };

  // for test, after plus plus
     static struct fstruct extras[EXTRA_SIZE] = { {15, 33}, {-1, -1} };
     int op_idx, table_idx, agg_idx;
     int extra_idx, temp;

     int i,found;
  
     agg_codes[0].start_code = num_codes - 1;

     op_idx = num_codes - 1;

     for ( agg_idx = table_idx = 0; op_idx >= 0;)
        {
          if (op_info[op_idx].op_code < FIX_OPS)  // one of the standards
             {
               if (table[table_idx].fix[op_info[op_idx].op_code] > 0)
                  {
                 // now check to see if the operands are ok, unless
                 // this is the first one to match
                    if (table_idx == 0)
                       {
                      // continue further
                         table_idx = table[table_idx].fix[op_info[op_idx].op_code];
                         op_idx--;
                       }
                      else
                       {
                      // this is a potential aggregate so check operands
                      // old code (bug fix to check result_id instead of op1_id and op2_id
                      // if ( (op_info[op_idx+1].result_id == op_info[op_idx].op1_id) ||
                      //      (op_info[op_idx+1].result_id == op_info[op_idx].op2_id) )
                         if (op_info[op_idx+1].result_id == op_info[op_idx].result_id)
                            {
                           // previous result is current operand so
                           // continuing looking
                              table_idx = table[table_idx].fix[op_info[op_idx].op_code];
                              op_idx--;
                            }
                           else
                            {
                           // match ends here since operands don't match
                           // since this isn't a first match, there must be
                           // an aggregate to record
                              agg_codes[agg_idx].end_code     = op_idx + 1;
                              agg_codes[agg_idx].agg_code     = table[table_idx].agg_operator;
                              agg_codes[agg_idx].agg_function = table[table_idx].agg_function;
                              agg_idx +=1;
                              agg_codes[agg_idx].start_code = op_idx;
                              table_idx = 0;
                            }
                       }
                  }
                 else
                 // match ends here since no function for this standard op
                  {
                    agg_codes[agg_idx].end_code     = op_idx + 1;
                    agg_codes[agg_idx].agg_code     = table[table_idx].agg_operator;
                    agg_codes[agg_idx].agg_function = table[table_idx].agg_function;
                    agg_idx +=1;
                    agg_codes[agg_idx].start_code = op_idx;
                    table_idx = 0;
                  }
             }
            else
             {
            // not one of the standard ops - check to see if it one
            // of the extras.
               extra_idx = table[table_idx].rest_start;
               temp = table[table_idx].rest_cnt;
               for (found = i = 0; i < temp; i++)
                  {
                    if (op_info[op_idx].op_code == extras[extra_idx].op_code)
                       {
                      // found the operator in the extra list
                         i = temp;
                         found = 1;
                       }
                  }

               if (found == 1)  // operator is in the table
                  {
                 // check to see if operands ok
                    if (table_idx == 0)   // first one so no operand check
                       {
                         table_idx = extras[extra_idx].table_pos;
                         op_idx--;
                       }
                      else
                       {
                      // this isn't the first in an aggregate so check result
                      // of the previous ones to see if the operands match
                         if ( (op_info[op_idx+1].result_id == op_info[op_idx].op1_id) ||
                              (op_info[op_idx+1].result_id == op_info[op_idx].op2_id) )
                           // match, so attempt to extend the aggregate
                            {
                              table_idx = extras[extra_idx].table_pos;
                              op_idx--;
                            }
                           else
                            {
                           // operand mis-match, so the aggregate ends
                              agg_codes[agg_idx].end_code = op_idx + 1;
                              agg_codes[agg_idx].agg_code = table[table_idx].agg_operator;
                              agg_codes[agg_idx].agg_function = table[table_idx].agg_function;
                              agg_idx +=1;
                              agg_codes[agg_idx].start_code = op_idx;
                              table_idx = 0;
                            }
                       }
                  }
                 else // match ends here since it wasn't in the extra table
                  {
                    if (table_idx > 0)
                       {
                      // record the in-process aggregate if there is one
                         agg_codes[agg_idx].end_code = op_idx + 1;
                         agg_codes[agg_idx].agg_code = table[table_idx].agg_operator;
                         agg_codes[agg_idx].agg_function = table[table_idx].agg_function;
                         agg_idx +=1;
                         agg_codes[agg_idx].start_code = op_idx;
                       }
                 // now record the not-in-table op_code

                    agg_codes[agg_idx].end_code = op_idx;
                    agg_codes[agg_idx].agg_code = op_info[op_idx].op_code;
                    agg_idx +=1;
                    agg_codes[agg_idx].start_code = op_idx - 1;
                    op_idx--;
                    table_idx = 0;
                  }
             }
        }
  // outside of loop now - last op_code pointer in table

     if (table[table_idx].agg_operator > -1)
        {
       // something still in process, so record it 
          agg_codes[agg_idx].end_code = op_idx + 1;
          agg_codes[agg_idx].agg_code = table[table_idx].agg_operator;
          agg_codes[agg_idx].agg_function = table[table_idx].agg_function;
          agg_len = agg_idx + 1;  // sets return length
        }
       else
        {
          agg_len = agg_idx;  // sets the return length
        }
   }

void Array_Statement_Type::Optimize_Binary_Operators_To_Aggregate_Operators_In_Statement_List ()
   {
  // This would execute the list in the FIFO order (Head --> Tail)
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
        {
          printf ("\n");
          printf ("\n");
          printf ("Inside of Array_Statement_Type::Optimize_Binary_Operators_To_Aggregate_Operators_In_Statement_List! \n");
          printf ("\n");
        }
#endif

     Array_Statement_Type* s = NULL;
     for (s = Head_Of_List; s != NULL; s=s->Next_List_Element)
        {
          s->Optimize_Binary_Operators_To_Aggregate_Operators();
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
        {
          printf ("\n");
          printf ("\n");
        }
#endif
   }

void Array_Statement_Type::Optimize_Binary_Operators_To_Aggregate_Operators () 
   { 
     const int MAX_NUMBER_OF_OPERATIONS = 100;
     struct op_struct  Simple_Operator_List    [MAX_NUMBER_OF_OPERATIONS];
     struct agg_struct Aggregate_Operator_List [MAX_NUMBER_OF_OPERATIONS];

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of Array_Statement_Type::Optimize_Binary_Operators_To_Aggregate_Operators! \n");
#endif

  // This would execute the list in the FIFO order (Head --> Tail)

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
        {
          printf ("Before Processing -- Display Statement #%d: ",Statement_ID);
          Display();
        }
#endif

  // Skip optimizations on non assignment operations (like the replace member function)
     if ( Equals_Operation->Operation_Code == doubleArray::Assignment)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Statement uses an assignment operation! \n");
#endif         
 
          Expression_Tree_Node_Type* Right_End = (Prev_List_Element == NULL) ? NULL :
                                                  Prev_List_Element->Equals_Operation;
       // Find the end of the statement (so that optimization 
       // occures in the original and correct order)!
       // int Number_Of_Simple_Operations = 0;
          int Number_Of_Simple_Operations = 1;
          Expression_Tree_Node_Type* p = Equals_Operation;
          while (p->Prev_List_Element != Right_End)
             {
            // Move pointer to previous array expression (elements of the statement)!
               p=p->Prev_List_Element;
               Number_Of_Simple_Operations++;
             }

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Number_Of_Simple_Operations = %d \n",Number_Of_Simple_Operations);
#endif         

       // error checking
          if (Number_Of_Simple_Operations >= MAX_NUMBER_OF_OPERATIONS)
             {
               printf ("ERROR: in Array_Statement_Type::\
                        Optimize_Binary_Operators_To_Aggregate_Operators -- \
                        Number_Of_Simple_Operations >= MAX_NUMBER_OF_OPERATIONS \n");
               APP_ABORT();
             }
               
       // error checking
          if (p == NULL)
             {
               printf ("ERROR: p == NULL in Array_Statement_Type::\
                        Optimize_Binary_Operators_To_Aggregate_Operators()! \n");
               APP_ABORT();
             }

       // printf ("Count number of simple operators in list! \n");
       // Reset p to the start of the list again
       // Skip the assingment operator!
       // p = Equals_Operation->Prev_List_Element;
          p = Equals_Operation;
       // for (int i=Number_Of_Simple_Operations-1; i >= 0; i--)
          for (int i=0; i < Number_Of_Simple_Operations; i++)
             {
               Simple_Operator_List[i].op_code   = p->Operation_Code;
               Simple_Operator_List[i].result_id = p->Get_Lhs_Array_ID();
               int Rhs_ID_List [4];
               p->Get_Rhs_Array_ID (Rhs_ID_List);
               Simple_Operator_List[i].op1_id    = Rhs_ID_List [1];
               Simple_Operator_List[i].op2_id    = Rhs_ID_List [2];
               p=p->Prev_List_Element;
             }

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
             {
               printf ("Print out operator lists! \n");
            // Print out simple operators
               for (i = 0; i < Number_Of_Simple_Operations; i++)
                  {
                    printf("Simple_Operator_List[%d].op_code = %d result_id = %d op1_id = %d op2_id = %d \n",
                         i,Simple_Operator_List[i].op_code,Simple_Operator_List[i].result_id,
                         Simple_Operator_List[i].op1_id,Simple_Operator_List[i].op2_id);
                  }
             }
#endif

       // Call build aggregate operator software (from Rebecca)!
          int Number_Of_Aggregate_Operations = 0;

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Call Find_Aggregate_Operators! \n");
#endif

          Find_Aggregate_Operators ( Simple_Operator_List, Number_Of_Simple_Operations, 
                                     Aggregate_Operator_List, Number_Of_Aggregate_Operations );
       // Test_Case ();

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
             {
               printf ("Print out operator lists! \n");
            // Print out aggregate operators
               for (i = 0; i < Number_Of_Aggregate_Operations; i++)
                  {
                    printf("Aggregate_Operator_List[%d].agg_code = %d start = %d end = %d Function Pointer = %p \n",i,
                         Aggregate_Operator_List[i].agg_code,
                         Aggregate_Operator_List[i].start_code,
                         Aggregate_Operator_List[i].end_code,
                         Aggregate_Operator_List[i].agg_function);
                  }
             }
#endif

       // printf ("Backup to start of execution list! \n");
          p = Equals_Operation;
          while (p->Prev_List_Element != Right_End)
             {
            // Move pointer to previous array expression (elements of the statement)!
            // printf ("In list p->display() p = %p \n",p);
            // p->display();
               p=p->Prev_List_Element;
             }

       // printf ("In list p->display() p = %p \n",p);
       // p->display();

          void** Array_Pointer_List = NULL; // List of pointers to operands!

          for (i=0; i < Number_Of_Aggregate_Operations; i++)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("Aggregate Operation #%d \n",i);
#endif

               int Number_Of_Operators = (Aggregate_Operator_List[i].start_code - Aggregate_Operator_List[i].end_code) + 1;

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("Number_Of_Operators = %d \n",Number_Of_Operators);
#endif

               if ( Number_Of_Operators > 1 )
                  {
                 // printf ("##### Do surgery on linked list to put in aggregate operator! \n");

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf("Aggregate_Operator_List[%d].agg_code = %d start = %d end = %d \n",i,
                              Aggregate_Operator_List[i].agg_code,
                              Aggregate_Operator_List[i].start_code,
                              Aggregate_Operator_List[i].end_code);
#endif

                    Expression_Tree_Node_Type* Right_Operator = p;
                    for (int j=0; j < Number_Of_Operators-1; j++)
                       {
                         p = p->Next_List_Element;
                       }
                    Expression_Tree_Node_Type* Left_Operator = p;

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                       {
                         printf ("Right_Operator->display() Right_Operator = %p \n",Right_Operator);
                         Right_Operator->display();

                         printf ("Left_Operator->display() Left_Operator = %p \n",Left_Operator);
                         Left_Operator->display();
                       }
#endif

                 // Now form the list of operands which the list of operators (some of which maybe
                 // aggregate operators) will use.  There is a fixed order to this list which is independent of
                 // the use of the aggregate operators.

                    Array_Pointer_List = NULL;

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("Allocate memory for Array_Pointer_List (Operand_List) \n");
#endif
                 // Maximum number of operand is number or binary operators + 1
                    Array_Pointer_List = (void**) malloc ( sizeof(void*) * (MAX_NUMBER_OF_OPERATIONS+1) );

                 // We can remove this loop later!
                    for (int temp=0; temp < MAX_NUMBER_OF_OPERATIONS+1; temp++)
                       {
                         Array_Pointer_List[temp] = NULL;
                       }

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("Build Operand List \n");
#endif

                 // Reset p to the start of the list again
                    Expression_Tree_Node_Type* Temp_p0 = Right_Operator;
                    int Counter = 0;
                    for (int k=0; k < Number_Of_Operators; k++)
                       {
                      // void *Operand_List [2] = { NULL , NULL };
                         void *Operand_List [2];
                         Operand_List [0] = NULL;
                         Operand_List [1] = NULL;
                         Temp_p0->Get_Rhs_Array (Operand_List);
 
                         Array_Pointer_List [Counter++] = Operand_List[0];
                         if (Operand_List[1] != NULL)
                              Array_Pointer_List [Counter++] = Operand_List[1];
 
                         Temp_p0 = Temp_p0->Next_List_Element;
                       }
                    Temp_p0 = NULL;

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                       {
                         printf ("Print out Operand List (Array_Pointer_List) \n");
                         for (int l=0; l < Number_Of_Operators+1; l++)
                              printf ("Array_Pointer_List[%d] = %p \n",l,Array_Pointer_List[l]);
                       }
#endif

                 // Now insert the aggregate operator and delete the sarting and ending
                 // links and any links inbetween! Lookup aggregate function to be used 
                 // and build the correct execution object.
                 // Then insert the new aggregate execution object into the list!

                 // Now edit the linked list of Expression_Tree_Node_Type (actually the 
                 // derived types) and build the aggregate operators.

                 // printf ("Initialize pointer to Result operand! \n");
                    void* Result_Pointer = Left_Operator->Get_Result_Array ();

                 // printf ("Call Starting_Operator->Build_Aggregate_Operator! (i=%d) \n",i);

                    if (Aggregate_Operator_List[i].agg_function == NULL)
                       {
                         printf ("ERROR: MDI Aggregate function not specified! \n");
                         APP_ABORT();
                       }

                    Expression_Tree_Node_Type* Aggregate_Operator = 
                         Equals_Operation->Build_Aggregate_Operator (
                              Number_Of_Operators,
                              Aggregate_Operator_List[i].agg_function,
                              Result_Pointer, Array_Pointer_List);

#if 1
   // Executing this builds an aggregate object and puts it into the list!
   // Not executing it should just leave the original list untouched!

                    if (Left_Operator->Next_List_Element != NULL)
                       {
                      // printf ("Left_Operator->Next_List_Element != NULL \n");
                         Left_Operator->Next_List_Element->Prev_List_Element = Aggregate_Operator;
                       }
                   // else
                   //    printf ("Left_Operator->Next_List_Element == NULL \n");

                    if (Right_Operator->Prev_List_Element != NULL)
                       {
                      // printf ("Right_Operator->Prev_List_Element != NULL \n");
                         Right_Operator->Prev_List_Element->Next_List_Element = Aggregate_Operator;
                       }
                   // else
                   //    printf ("Right_Operator->Prev_List_Element == NULL \n");

                    Aggregate_Operator->Next_List_Element = Left_Operator->Next_List_Element;
                    Aggregate_Operator->Prev_List_Element = Right_Operator->Prev_List_Element;

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("Delete the linked list of operators we are replacing with an aggregate operator! \n");
#endif

                    Expression_Tree_Node_Type* Temp_p1 = Right_Operator; 
                    Right_Operator = NULL;
                    while (Temp_p1 != Left_Operator)
                       {
                      // printf ("Deleting Right_Operator (and links between right and left operators)! \n");
                         Expression_Tree_Node_Type* Temp_p2 = Temp_p1->Next_List_Element;
                         if (Temp_p1 != NULL)
                            {
                           // delete Temp_p1;
                              Temp_p1 = NULL;
                              Temp_p1 = Temp_p2;
                            }
                       }
                    if (Temp_p1 == Left_Operator)
                       {
                      // printf ("Deleting Left_Operator! \n");

                      // The following causes an error when uncommented (I don't know why)
                      // if (Temp_p1 != NULL)
                      //      delete Temp_p1;
                         Left_Operator = NULL;
                       }
#endif
                  }

               p = p->Next_List_Element;
             }
        }
       else
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Statement NOT terminated using assignment (used replace, +=, -=, etc.  or something else -- so we skip any optimization)! \n");
#endif
        }

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
        {
          printf ("After Processing -- Display Statement #%d: ",Statement_ID);
          Display();
        }

     if (APP_DEBUG > 3)
          printf ("Leaving Array_Statement_Type::Optimize_Binary_Operators_To_Aggregate_Operators! \n");
#endif
   } 

#endif // for COMPILE_AGGREGATE_OPERATOR_OPTIMIZATIONS


