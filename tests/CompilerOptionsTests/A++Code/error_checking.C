#include "A++.h"

// This turns on the calls to the bounds checking function
#define BOUNDS_ERROR_CHECKING TRUE

// It helps to set this to FALSE sometimes for debugging code
// this enambles the A++/P++ operations in the bounds checking function
#define TURN_ON_BOUNDS_CHECKING   TRUE


/* divert(1) */


// ***************************************************************************************
//     Array_Descriptor constructors put here to allow inlining in Indexing operators!
// ***************************************************************************************

// ****************************************************************************
// ****************************************************************************
// **************  ERROR CHECKING FUNCTION FOR INDEXING OPERATORS  ************
// ****************************************************************************
// ****************************************************************************


/* divert */




#if 0
// This code was moved to the desciptor.C file so that it would be in the 
// correct place for the C++ compilers that have rules about where
// they find the templated code (i.e. they look in the files 
// with .C if the declarations are in a .h file).

void Array_Descriptor_Type::Error_Checking_For_Index_Operators (
          const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ) const
   {
  // This function provides error checking for the indexing operators. It is only called
  // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Descriptor_Type::Error_Checking_For_Index_Operators \n");
#endif

  // This array is used when the values of the input Index objects or intArray objects
  // can be simplified to be integers (used in mixed indexing)
     int Integer_List [MAX_ARRAY_DIMENSION];

     int i = 0;

  // ... code change (8/20/96, kdb) new scalar indexing changes
  //   meaning of correct range for indexing. The range now must
  //   be between User_Base and User_Base + Count ...
  // Not sure if this is the correct defaults
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          Integer_List[i] = User_Base[i];
          //Integer_List[i] = Base[i] + Data_Base[i];;

     APP_ASSERT(Internal_Index_List != NULL);

  // Initialize the list of intArray parameters -- note that the Internal_Indirect_Addressing_Index
  // objects can be either an intArray or a scalar (integer) so we have to process the list

     bool Indirect_Addressing_In_Use = FALSE;
     bool Index_Object_In_Use        = FALSE;
     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Internal_Index_List[i] != NULL)
               Index_Object_In_Use = TRUE;
        }

     if ( Index_Object_In_Use && Indirect_Addressing_In_Use )
        {
          printf ("ERROR: Can't mix Index objects with intArray object (indirect addressing) indexing! \n");
          APP_ABORT();
        }

  // ... code change (8/20/96, kdb) new scalar indexing changes ...
  // Compute Local_Base and Local_Bound
     int Local_Base  [MAX_ARRAY_DIMENSION];
     int Local_Bound [MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          Local_Base [i] = User_Base[i];
          Local_Bound[i] = User_Base[i]+(Bound[i]-Base[i])/Stride[i];
          //Local_Base [i] = Data_Base[i]+Base[i];
          //Local_Bound[i] = Data_Base[i]+Bound[i];
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             printf ("Local_Base[%d] = %d Local_Bound[%d] = %d \n",i,Local_Base[i],i,Local_Bound[i]);
#endif
        }

 // Optionally turn off the bounds checking since it adds to the output when debugging!
#if TURN_ON_BOUNDS_CHECKING
  // ****************************************************************************
  // Now find the min and max access values of the array operation (then check
  // them against the local base and local bound to determine if the acess is
  // out of bounds).
  // ****************************************************************************

  // Bug fix (9/23/94) this broke the interpolation in overture for the case of
  // nozero bases of the Target and Source (for a 2D grid containing 2 components and
  // having nonzero base in the 3rd axis) (reference A++/BUGS/bug44.C).
  // The fix is to use *Parm0_int instead of *Parm0_int+Data_Base[0] along all axes.

     int Min[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        Min[i] = (Internal_Index_List[i]) ? (Internal_Index_List[i]->Index_Mode == Index_Triplet) ? Internal_Index_List[i]->Base : Local_Base[i] : Integer_List[i];
       // Min[i] = (intArrayList[i]) ? min(*intArrayList[i]) : (Internal_Index_List[i]) ? (Internal_Index_List[i]->Index_Mode == Index_Triplet) ? Internal_Index_List[i]->Base : Local_Base[i] : Integer_List[i];

     int Max[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          Max[i] = (Internal_Index_List[i]) ? (Internal_Index_List[i]->Index_Mode == Index_Triplet) ? Internal_Index_List[i]->Bound : Local_Bound[i] : Integer_List[i];
       // Max[i] = (intArrayList[i]) ? max(*intArrayList[i]) : (Internal_Index_List[i]) ? (Internal_Index_List[i]->Index_Mode == Index_Triplet) ? Internal_Index_List[i]->Bound : Local_Bound[i] : Integer_List[i];

  // Bugfix (3/30/95) added test for Null_Array since P++ requires that we allow indexing
  // of the global indexspace outside of a given partition
     if (Is_A_Null_Array == FALSE)
        {
       // Check bounds!
          for (i=0; i < MAX_ARRAY_DIMENSION; i++)
             {
               if ( (Min[i] < Local_Base[i]) || (Max[i] > Local_Bound[i]) )
                  {
                    printf ("\n");
                    printf ("**************************************************************** \n");
                    printf ("    ERROR: (DEAD FUNCTION) INDEXING OUT OF BOUNDS                      \n");
                    printf ("**************************************************************** \n");
                    printf ("ERROR: in Array Indexing using: \n");
                    int j;
                    for (j=0; j < MAX_ARRAY_DIMENSION; j++)
                       {
                         printf ("     (Min[%d] = %d,Max[%d] = %d) \n",j,Min[j],j,Max[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("Original Array object base and Bounds are: \n");
                    for (j=0; j < MAX_ARRAY_DIMENSION; j++)
                       {
                         printf ("     (Local_Base[%d] = %d,Local_Bound[%d] = %d) \n",j,Local_Base[j],j,Local_Bound[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("\n");
                    APP_ABORT();
                  }
             }
        }
#else
     printf ("WARNING: bounds checking of indirect addressing turned off! \n");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving Array_Descriptor_Type::Error_Checking_For_Index_Operators! \n");
#endif
   }

void Array_Descriptor_Type::Error_Checking_For_Index_Operators (
          const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List ) const
   {
  // This function provides error checking for the indexing operators. It is only called
  // if the user specifies runtime bounds checking!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Array_Descriptor_Type::Error_Checking_For_Index_Operators(Indirect_Index_List) \n");
#endif

  // This array is used when the values of the input Index objects or intArray objects
  // can be simplified to be integers (used in mixed indexing)
     int Integer_List [MAX_ARRAY_DIMENSION];
     Internal_Index* Index_List [MAX_ARRAY_DIMENSION];

     int i = 0;

  // ... code change (8/20/96, kdb) new scalar indexing changes ...
  // Not sure if this is the correct defaults
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          Integer_List[i] = User_Base[i];;
          //Integer_List[i] = Base[i] + Data_Base[i];;

     APP_ASSERT( Indirect_Index_List != NULL );

     intArray* intArrayList[MAX_ARRAY_DIMENSION];
     for (i=0; i<MAX_ARRAY_DIMENSION; i++)
        {
          Index_List  [i] = NULL;
          intArrayList[i] = NULL;
        }

  // Initialize the list of intArray parameters -- note that the Internal_Indirect_Addressing_Index
  // objects can be either an intArray or a scalar (integer) so we have to process the list
     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Indirect_Index_List[i] != NULL) 
             { 
               if (Indirect_Index_List[i]->intArrayInStorage)
                    intArrayList[i] = Indirect_Index_List[i]->IndirectionArrayPointer;
                 else
                  {
                    if (Indirect_Index_List[i]->IndexInStorage)
                         Index_List[i]   = &(Indirect_Index_List[i]->Index_Data);
                      else
                         Integer_List[i] = Indirect_Index_List[i]->Scalar;
                  }
             }
        }

     int Size_Of_Parameter [MAX_ARRAY_DIMENSION];
     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // Error checking for conformable indirect addressing
       // Size_Of_Parameter[i] = (intArrayList[i] != NULL) ? intArrayList[i]->elementCount() : 1;
          if (intArrayList[i] != NULL)
             {
               Size_Of_Parameter[i] = intArrayList[i]->elementCount();
             }
            else
             {
               if (Index_List[i] != NULL)
                  {
                    Size_Of_Parameter[i] = Index_List [i]->length();
                  }
                 else
                  {
                    Size_Of_Parameter[i] = 1;
                  }
             }
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Size_Of_Parameter[%d] = %d \n",i,Size_Of_Parameter[i]);
#endif
        }

     int Max_Size = 0;
     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
          Max_Size = (Size_Of_Parameter[i] > Max_Size) ? Size_Of_Parameter[i] : Max_Size;

     for (i = 0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // We allow the mixing of scalars and intArray indexing (indirect addressing)
          if ( (Size_Of_Parameter[i] != Max_Size) && (Size_Of_Parameter[i] != 1) )
             {
               printf ("\n");
               printf ("******************************************************************** \n");
               printf ("ERROR - intArrays used for indirect addressing must be the same size \n");
               printf ("******************************************************************** \n");
               printf ("MORE INFO: Inside of indexing operator error checking -- non conformable indirect addressing Max_Size = %d  Size_Of_Parameter list - \n",Max_Size);
               int j;
               for (j = 0; j < MAX_ARRAY_DIMENSION; j++)
                    printf ("%d ",Size_Of_Parameter[i]);
               printf ("\n");
               printf ("******************************************************************** \n");
               printf ("\n");
               APP_ABORT();
             }
        }

  // ... code change (8/20/96, kdb) new scalar indexing changes ...
  // Compute Local_Base and Local_Bound
     int Local_Base  [MAX_ARRAY_DIMENSION];
     int Local_Bound [MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          Local_Base [i] = User_Base[i];
          Local_Bound[i] = User_Base[i]+(Bound[i]-Base[i])/Stride[i];
          //Local_Base [i] = Data_Base[i]+Base[i];
          //Local_Bound[i] = Data_Base[i]+Bound[i];
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
             printf ("Local_Base[%d] = %d Local_Bound[%d] = %d \n",i,Local_Base[i],i,Local_Bound[i]);
#endif
        }

 // Optionally turn off the bounds checking since it adds to the output when debugging!
#if TURN_ON_BOUNDS_CHECKING
     bool intArrayTemporaryList[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          intArrayTemporaryList[i] = (intArrayList[i]) ? intArrayList[i]->Array_Descriptor.Is_A_Temporary : FALSE;
        }

  // Set Array destriptor as a non-temporary!
  // Otherwise the temporaries will be deleted by the min max functions!
  // Bug fix (12/14/94) We have to set the Is_A_Temporary flag consistently in A++ and P++
  // so in P++ we have to change the flag status for both P++ and the internal A++ array.
  // and then reset them both in the case of P++.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (intArrayTemporaryList[i] == TRUE)
             {
               intArrayList[i]->Array_Descriptor.Is_A_Temporary = FALSE;
#if defined(PPP)
               intArrayList[i]->SerialArray->Array_Descriptor.Is_A_Temporary = FALSE;
#endif
             }
        }
     
  // ****************************************************************************
  // Now find the min and max access values of the array operation (then check
  // them against the local base and local bound to determine if the acess is
  // out of bounds).
  // ****************************************************************************

  // Bug fix (9/23/94) this broke the interpolation in overture for the case of
  // nozero bases of the Target and Source (for a 2D grid containing 2 components and
  // having nonzero base in the 3rd axis) (reference A++/BUGS/bug44.C).
  // The fix is to use *Parm0_int instead of *Parm0_int+Data_Base[0] along all axes.

#if COMPILE_DEBUG_STATEMENTS
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
          if (intArrayList[i] != NULL)
               intArrayList[i]->Test_Consistency("Array_Descriptor_Type::Error_Checking_For_Index_Operators (intArrayList[i])");
#endif

     int Min[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // Min[i] = (intArrayList[i]) ? min(*intArrayList[i]) : Integer_List[i];
          if (intArrayList[i] != NULL)
               Min[i] = min(*intArrayList[i]);
            else
             {
               if (Index_List[i] != NULL)
                    Min[i] = Index_List [i]->Base;
                 else
                    Min[i] = Integer_List[i];
             }
        }


     int Max[MAX_ARRAY_DIMENSION];
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // Max[i] = (intArrayList[i]) ? max(*intArrayList[i]) : Integer_List[i];
          if (intArrayList[i] != NULL)
               Max[i] = max(*intArrayList[i]);
            else
             {
               if (Index_List[i] != NULL)
                    Max[i] = Index_List [i]->Bound;
                 else
                    Max[i] = Integer_List[i];
             }
        }

  // Reset Array destriptor as a temporary!
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (intArrayTemporaryList[i] == TRUE)
             {
               intArrayList[i]->Array_Descriptor.Is_A_Temporary = TRUE;
#if defined(PPP)
               intArrayList[i]->SerialArray->Array_Descriptor.Is_A_Temporary = TRUE;
#endif
             }
        }

  // Bugfix (3/30/95) added test for Null_Array since P++ requires that we allow indexing
  // of the global indexspace outside of a given partition
     if (Is_A_Null_Array == FALSE)
        {
       // Check bounds!
          for (i=0; i < MAX_ARRAY_DIMENSION; i++)
             {
               if ( (Min[i] < Local_Base[i]) || (Max[i] > Local_Bound[i]) )
                  {
                    printf ("\n");
                    printf ("**************************************************************** \n");
                    printf ("              ERROR: (DEAD FUNCTION 2) INDEXING OUT OF BOUNDS                      \n");
                    printf ("**************************************************************** \n");
                    printf ("ERROR: in Array Indexing using: \n");
                    int j;
                    for (j=0; j < MAX_ARRAY_DIMENSION; j++)
                       {
                         printf ("     (Min[%d] = %d,Max[%d] = %d) \n",j,Min[j],j,Max[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("Original Array object base and Bounds are: \n");
                    for (j=0; j < MAX_ARRAY_DIMENSION; j++)
                       {
                         printf ("     (Local_Base[%d] = %d,Local_Bound[%d] = %d) \n",j,Local_Base[j],j,Local_Bound[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("\n");
                    APP_ABORT();
                  }
             }
        }
#else
     printf ("WARNING: bounds checking of indirect addressing turned off! \n");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Leaving Array_Descriptor_Type::Error_Checking_For_Index_Operators! \n");
#endif
   }

// **************************************************************************
// This function does the bounds checking for the scalar indexing of A++
// array objects.  Its purpose is to localize all the error checking for
// scalar indexing.
// **************************************************************************
// Old prototype
// void ""Array::Range_Checking ( int *i_ptr , int *j_ptr , int *k_ptr , int *l_ptr ) const
void Array_Descriptor_Type::Error_Checking_For_Scalar_Index_Operators ( 
     const Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ) const
   {
  // Set maximum dimension of indexing
     int Problem_Dimension = MAX_ARRAY_DIMENSION;

  // Lower the dimension as indicated by the input

     int i;
     for (i = MAX_ARRAY_DIMENSION-1; i >= 0; i--)
          if (Integer_List[i] == NULL) 
               Problem_Dimension--;

  // No support for the scalar indexing of views obtained through indirect addressing
     if (Uses_Indirect_Addressing)
        {
       // printf ("Error Checking for scalar Indexing operations on views built through indirect addressing not implemented yet! \n");
       // APP_ABORT();
          APP_ASSERT (Problem_Dimension == 1);
          for (i=0; i < MAX_ARRAY_DIMENSION; i++)
             {
               if (Index_Array[i] != NULL)
                    Index_Array[i]->Array_Descriptor.Error_Checking_For_Scalar_Index_Operators(Integer_List);
             } 
        }
       else
        {
	  // ... first check dimensionality ...
	  if (Descriptor_Dimension > Problem_Dimension)
	  {
	    for (i=Problem_Dimension;i<Descriptor_Dimension;i++)
	    {
	      if (Base[i] != Bound[i])
	      {
	        printf ("ERROR: Not enough subscripts, \n");
	        printf ("  Descriptor_Dimension = %d, \n",
			Descriptor_Dimension);
                printf ("  Number of subscripts is %d.\n",
		        Problem_Dimension);
                APP_ABORT();
	      }
	    }
	  }
	  else if (Descriptor_Dimension < Problem_Dimension)
	  {
	    for (i=Descriptor_Dimension;i<Problem_Dimension;i++)
	    {
  // ... code change (8/20/96, kdb) new scalar indexing changes ...
	      //if ((Base[i] != Bound[i]) && 
	      //  ((Base[i]+Data_Base[i]) != *Integer_List[i]))
	      if ((Base[i] != Bound[i]) && 
		  ((User_Base[i]) != *Integer_List[i]))
              {
		printf ("ERROR: too many subscripts, \n");
                printf ("  Number of subscripts is %d.\n",
		        Problem_Dimension);
	        printf ("  Descriptor_Dimension = %d, \n",
			Descriptor_Dimension);
                APP_ABORT();
	      }
	    }
	  }
          for (i=0; i < Problem_Dimension; i++)
             {
  // ... code change (8/20/96, kdb) new scalar indexing changes ...
               //if ( (*Integer_List[i] < Base[i]  + Data_Base[i]) ||
               //   (*Integer_List[i] > Bound[i] + Data_Base[i]) )
               if ( (*Integer_List[i] < User_Base[i]) ||
                    (*Integer_List[i] > 
			User_Base[i]+ (Bound[i]-Base[i])/Stride[i]) )
                  {
                    printf ("ERROR: Scalar Index for dimension(%d) = %d out of bounds! \n",i,*Integer_List[i]);
                    printf ("********************************************************* \n");
                    printf ("Scalar values used in indexing: \n");
                    int j;
                    for (j=0; j < Problem_Dimension; j++)
                       {
                         printf ("     Scalar Index (along dimension %d) = %d \n",j,*Integer_List[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("Original Array object base and Bounds are: \n");
                    for (j=0; j < MAX_ARRAY_DIMENSION; j++)
                       {
                        //printf ("    (Base[%d] = %d,Bound[%d] = %d)\n",
			//j,Base[j]+Data_Base[j],j,Bound[j]+Data_Base[j]);
                         printf ("     (Base[%d] = %d,Bound[%d] = %d)\n",
			   j,User_Base[j],j,
			   ((Bound[j]-Base[j])/Stride[j])+Data_Base[j]);
                       }
                    printf ("**************************************************************** \n");
                    printf ("\n");
                    APP_ABORT();
                  }
             }
        }
   }
#endif



