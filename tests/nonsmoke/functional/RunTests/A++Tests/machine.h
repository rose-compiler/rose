/* Allow repeated includes of machine.h without error */
#ifndef _APP_MDI_MACHINE_H
#define _APP_MDI_MACHINE_H

/* GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef __GNUC__
#pragma interface
#endif

#ifdef	__cplusplus
// error "__cplusplus is defined"
extern "C" {
#else
// error "__cplusplus is NOT defined"
#endif

#include <stdio.h>
#include <stdlib.h>


#ifdef sun
#define MDI_MALLOC(size) valloc(size)
#else
#define MDI_MALLOC(size) malloc(size)
#endif

/* funtions defined in common_func.c */
#ifdef CRAY
double asinh(double x);
double acosh(double x);
double atanh(double x);
double APP_cray_pow (double x, double y);
#endif

/*
#include "constants.h"
*/
#include "array_domain.h"

/* int MDI_Compute_Dimension ( array_domain* Descriptor , int Axis ); */
int MDI_Compute_Block_Size_Using_Base_And_Array_Size ( int Base , int Array_Size , int Sub_Block_Size );
void MDI_Compute_Block_Size_Along_Axis ( array_domain* Descriptor , int Axis );
void MDI_Compute_Block_Sizes ( array_domain* Descriptor );
int *MDI_Get_Default_Index_Map_Data ( int Local_Min_Base,
                                      int Local_Max_Bound );

void MDI_Setup_Index_Pointers_Result ( int* , int** , int** , int** , int** ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  , 
    int*  , int*  , int*  , int*  , 
    int*  , int*  , int*  , int*  , int*  , int*  );

void MDI_Setup_Index_Pointers_To_Static_Maps_Result ( int* , int* ,
    int** , int** , int** , int** , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int   , int );

void MDI_Setup_Index_Pointers_Lhs ( int* , int** , int** , int** , int** ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  , 
    int*  , int*  , int*  , int*  , 
    int*  , int*  , int*  , int*  , int*  , int*  );

void MDI_Setup_Index_Pointers_To_Static_Maps_Lhs ( int* , int* ,
    int** , int** , int** , int** , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int   , int );

/*
void MDI_Setup_Index_Pointers_To_Static_Maps_Lhs (
    int** , int** , int** , int** , int   , int   , int   , int   ,
    int   , int   , int   , int   , int   , int );
*/

void MDI_Setup_Index_Pointers_Rhs ( int* , int** , int** , int** , int** ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  , 
    int*  , int*  , int*  , int*  , 
    int*  , int*  , int*  , int*  , int*  , int*  );

void MDI_Setup_Index_Pointers_To_Static_Maps_Rhs ( int* , int* ,
    int** , int** , int** , int** , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int   , int );

void MDI_Setup_Index_Pointers_Mask ( int* , int** , int** , int** , int** ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  , 
    int*  , int*  , int*  , int*  , 
    int*  , int*  , int*  , int*  , int*  , int*  );

void MDI_Setup_Index_Pointers_To_Static_Maps_Mask ( int* , int* ,
    int** , int** , int** , int** , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int*  , int*  , int*  , int*  ,
    int*  , int*  , int*  , int*  , int   , int );

void cleanup_after_MDI ( );





/*  HERE IS WHERE THE FINAL MACROS ARE EXPANDED!!! */
void MDI_double_Print_Array ( double *Result , array_domain *Result_Descriptor , int Display_Format );


/* Build the operator= related functions! */
void MDI_d_Assign_Array_Equals_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Assign_Array_Equals_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_d_Add_Array_Plus_Array
          ( double *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_Add_Array_Plus_Scalar
          ( double *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Add_Scalar_Plus_Array
          ( double *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Add_Scalar_Plus_Array_Accumulate_To_Operand
          ( double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Subtract_Array_Minus_Array
          ( double *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_Subtract_Array_Minus_Scalar
          ( double *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Subtract_Scalar_Minus_Array
          ( double *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Subtract_Scalar_Minus_Array_Accumulate_To_Operand
          ( double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Multiply_Array_Times_Array
          ( double *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_Multiply_Array_Times_Scalar
          ( double *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Multiply_Scalar_Times_Array
          ( double *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Multiply_Scalar_Times_Array_Accumulate_To_Operand
          ( double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Divide_Array_Divided_By_Array
          ( double *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_Divide_Array_Divided_By_Scalar
          ( double *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Divide_Scalar_Divided_By_Array
          ( double *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Divide_Scalar_Divided_By_Array_Accumulate_To_Operand
          ( double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* D e f i n e  these seperately so that we can correctly  d e f i n e  the relational operators! */
void MDI_d_Add_Array_Plus_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Add_Array_Plus_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Subtract_Array_Minus_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Subtract_Array_Minus_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Multiply_Array_Times_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Multiply_Array_Times_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Divide_Array_Divided_By_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Divide_Array_Divided_By_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_d_LT_Array_LT_Array
          ( int *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_LT_Array_LT_Scalar
          ( int *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_LT_Scalar_LT_Array
          ( int *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_LT_Scalar_LT_Array_Accumulate_To_Operand
          ( int *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_GT_Array_GT_Array
          ( int *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_GT_Array_GT_Scalar
          ( int *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_GT_Scalar_GT_Array
          ( int *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_GT_Scalar_GT_Array_Accumulate_To_Operand
          ( int *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_LTEQ_Array_LTEQ_Array
          ( int *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_LTEQ_Array_LTEQ_Scalar
          ( int *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_LTEQ_Scalar_LTEQ_Array
          ( int *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_LTEQ_Scalar_LTEQ_Array_Accumulate_To_Operand
          ( int *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_GTEQ_Array_GTEQ_Array
          ( int *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_GTEQ_Array_GTEQ_Scalar
          ( int *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_GTEQ_Scalar_GTEQ_Array
          ( int *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_GTEQ_Scalar_GTEQ_Array_Accumulate_To_Operand
          ( int *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_EQ_Array_EQ_Array
          ( int *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_EQ_Array_EQ_Scalar
          ( int *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_EQ_Scalar_EQ_Array
          ( int *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_EQ_Scalar_EQ_Array_Accumulate_To_Operand
          ( int *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_NOT_EQ_Array_NOT_EQ_Array
          ( int *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_NOT_EQ_Array_NOT_EQ_Scalar
          ( int *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_NOT_EQ_Scalar_NOT_EQ_Array
          ( int *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_NOT_EQ_Scalar_NOT_EQ_Array_Accumulate_To_Operand
          ( int *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_AND_Array_AND_Array
          ( int *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_AND_Array_AND_Scalar
          ( int *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_AND_Scalar_AND_Array
          ( int *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_AND_Scalar_AND_Array_Accumulate_To_Operand
          ( int *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_OR_Array_OR_Array
          ( int *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_d_OR_Array_OR_Scalar
          ( int *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_OR_Scalar_OR_Array
          ( int *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_OR_Scalar_OR_Array_Accumulate_To_Operand
          ( int *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* D e f i n e  these seperately since these is no <== operator which would be 
   built otherwise!  The functions are not called but are required to support the
   binary operations for the logical functions.
*/
#if (1)
void MDI_d_LT_Array_LT_Array_Accumulate_To_Operand 
          ( int *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_LT_Array_LT_Scalar_Accumulate_To_Operand
          ( int *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_GT_Array_GT_Array_Accumulate_To_Operand 
          ( int *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_GT_Array_GT_Scalar_Accumulate_To_Operand
          ( int *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_LTEQ_Array_LTEQ_Array_Accumulate_To_Operand 
          ( int *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_LTEQ_Array_LTEQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_GTEQ_Array_GTEQ_Array_Accumulate_To_Operand 
          ( int *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_GTEQ_Array_GTEQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_EQ_Array_EQ_Array_Accumulate_To_Operand 
          ( int *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_EQ_Array_EQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_NOT_EQ_Array_NOT_EQ_Array_Accumulate_To_Operand 
          ( int *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_NOT_EQ_Array_NOT_EQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_AND_Array_AND_Array_Accumulate_To_Operand 
          ( int *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_AND_Array_AND_Scalar_Accumulate_To_Operand
          ( int *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_OR_Array_OR_Array_Accumulate_To_Operand 
          ( int *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_OR_Array_OR_Scalar_Accumulate_To_Operand
          ( int *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

#endif

void MDI_d_Unary_Minus_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Unary_Minus_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Cos_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Cos_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Sin_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Sin_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Tan_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Tan_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Cos_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Cos_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Sin_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Sin_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Tan_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Tan_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Cosh_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Cosh_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Sinh_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Sinh_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Tanh_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Tanh_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Cosh_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Cosh_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Sinh_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Sinh_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Tanh_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Tanh_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_d_NOT_Array_Returning_IntArray
          ( int *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_NOT_Array_Accumulate_To_Operand_Returning_IntArray
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* (2/5/2000) New conversion operator support */
void MDI_d_Array_convertTo_intArray_Array_Accumulate_To_Operand 
          ( int *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Array_convertTo_intArray_Scalar_Accumulate_To_Operand
          ( int *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Array_convertTo_floatArray_Array_Accumulate_To_Operand 
          ( float *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Array_convertTo_floatArray_Scalar_Accumulate_To_Operand
          ( float *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Array_convertTo_doubleArray_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Array_convertTo_doubleArray_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_d_Log_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Log_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Log10_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Log10_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Exp_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Exp_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Sqrt_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Sqrt_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Fabs_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Fabs_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Abs_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Abs_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Ceil_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Ceil_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Floor_Array 
          ( double *Result , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Floor_Array_Accumulate_To_Operand
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

 
void MDI_d_If_Array_Use_Array
          ( double *Result , int *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_If_Array_Use_Scalar
          ( double *Result , int *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_If_Scalar_Use_Array
          ( double *Result , double *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_d_Fmod_Array_Modulo_Array
          ( double *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Fmod_Array_Modulo_Scalar
          ( double *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Fmod_Scalar_Modulo_Array
          ( double *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Fmod_Scalar_Modulo_Array_Accumulate_To_Operand
          ( double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Fmod_Array_Modulo_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Fmod_Array_Modulo_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Pow_Array_Raised_To_Array
          ( double *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Pow_Array_Raised_To_Scalar
          ( double *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Pow_Scalar_Raised_To_Array
          ( double *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Pow_Scalar_Raised_To_Array_Accumulate_To_Operand
          ( double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Pow_Array_Raised_To_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Pow_Array_Raised_To_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Tan2_Array_ArcTan2_Array
          ( double *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Tan2_Array_ArcTan2_Scalar
          ( double *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Arc_Tan2_Scalar_ArcTan2_Array
          ( double *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Arc_Tan2_Scalar_ArcTan2_Array_Accumulate_To_Operand
          ( double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Tan2_Array_ArcTan2_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Arc_Tan2_Array_ArcTan2_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Min_Array_And_Array
          ( double *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Min_Array_And_Scalar
          ( double *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Min_Scalar_And_Array
          ( double *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Min_Scalar_And_Array_Accumulate_To_Operand
          ( double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Min_Array_And_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Min_Array_And_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Max_Array_And_Array
          ( double *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Max_Array_And_Scalar
          ( double *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Max_Scalar_And_Array
          ( double *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Max_Scalar_And_Array_Accumulate_To_Operand
          ( double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Max_Array_And_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Max_Array_And_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_d_Sign_Array_Of_Array
          ( double *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Sign_Array_Of_Scalar
          ( double *Result , double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Sign_Scalar_Of_Array
          ( double *Result , double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_d_Sign_Scalar_Of_Array_Accumulate_To_Operand
          ( double *Rhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Sign_Array_Of_Array_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Sign_Array_Of_Scalar_Accumulate_To_Operand
          ( double *Lhs , double x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


double MDI_d_Sum_Array_Returning_Scalar
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

double MDI_d_Max_Array_Returning_Scalar
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

double MDI_d_Min_Array_Returning_Scalar
          ( double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_d_Sum_Array_Along_Axis
          ( int Axis , double *Result , double *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* ***********************************************************************************
// We provide specific support for this "Where_Mask = !Where_Mask && Else_Where_Mask"
// since it is an important part of the efficiency of the "where" statement support
// which we provide in thei array class library.
// ***********************************************************************************
*/

void MDI_d_Else_Where_NOT_X_AND_Y_Operator
          ( double *Result , double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_d_Else_Where_NOT_X_AND_Y_Operator_Accumulate_To_Operand 
          ( double *Lhs , double *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

/* Machine dependent memory allocation and deallocation functions! */
double* MDI_double_Allocate ( array_domain* Descriptor );
void MDI_double_Deallocate ( double* Data_Pointer , array_domain* Descriptor );
void MDI_double_Change_Base ( double** Array_Data_Pointer , array_domain* Descriptor , int Axis , int New_Base );
void MDI_double_Change_All_Bases ( double** Array_Data_Pointer , array_domain* Descriptor , 
                               int New_Base_I , int New_Base_J , int New_Base_K , int New_Base_L );
void MDI_double_Zero_Raw_Data_Pointer ( double** Array_Data_Pointer , array_domain* Descriptor );
void MDI_double_Adjust_Raw_Data_Pointer ( double** Array_Data_Pointer , array_domain* Descriptor , int* Old_Base );

void MDI_float_Print_Array ( float *Result , array_domain *Result_Descriptor , int Display_Format );


/* Build the operator= related functions! */
void MDI_f_Assign_Array_Equals_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Assign_Array_Equals_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_f_Add_Array_Plus_Array
          ( float *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_Add_Array_Plus_Scalar
          ( float *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Add_Scalar_Plus_Array
          ( float *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Add_Scalar_Plus_Array_Accumulate_To_Operand
          ( float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Subtract_Array_Minus_Array
          ( float *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_Subtract_Array_Minus_Scalar
          ( float *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Subtract_Scalar_Minus_Array
          ( float *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Subtract_Scalar_Minus_Array_Accumulate_To_Operand
          ( float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Multiply_Array_Times_Array
          ( float *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_Multiply_Array_Times_Scalar
          ( float *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Multiply_Scalar_Times_Array
          ( float *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Multiply_Scalar_Times_Array_Accumulate_To_Operand
          ( float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Divide_Array_Divided_By_Array
          ( float *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_Divide_Array_Divided_By_Scalar
          ( float *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Divide_Scalar_Divided_By_Array
          ( float *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Divide_Scalar_Divided_By_Array_Accumulate_To_Operand
          ( float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* D e f i n e  these seperately so that we can correctly  d e f i n e  the relational operators! */
void MDI_f_Add_Array_Plus_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Add_Array_Plus_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Subtract_Array_Minus_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Subtract_Array_Minus_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Multiply_Array_Times_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Multiply_Array_Times_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Divide_Array_Divided_By_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Divide_Array_Divided_By_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_f_LT_Array_LT_Array
          ( int *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_LT_Array_LT_Scalar
          ( int *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_LT_Scalar_LT_Array
          ( int *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_LT_Scalar_LT_Array_Accumulate_To_Operand
          ( int *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_GT_Array_GT_Array
          ( int *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_GT_Array_GT_Scalar
          ( int *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_GT_Scalar_GT_Array
          ( int *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_GT_Scalar_GT_Array_Accumulate_To_Operand
          ( int *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_LTEQ_Array_LTEQ_Array
          ( int *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_LTEQ_Array_LTEQ_Scalar
          ( int *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_LTEQ_Scalar_LTEQ_Array
          ( int *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_LTEQ_Scalar_LTEQ_Array_Accumulate_To_Operand
          ( int *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_GTEQ_Array_GTEQ_Array
          ( int *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_GTEQ_Array_GTEQ_Scalar
          ( int *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_GTEQ_Scalar_GTEQ_Array
          ( int *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_GTEQ_Scalar_GTEQ_Array_Accumulate_To_Operand
          ( int *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_EQ_Array_EQ_Array
          ( int *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_EQ_Array_EQ_Scalar
          ( int *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_EQ_Scalar_EQ_Array
          ( int *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_EQ_Scalar_EQ_Array_Accumulate_To_Operand
          ( int *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_NOT_EQ_Array_NOT_EQ_Array
          ( int *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_NOT_EQ_Array_NOT_EQ_Scalar
          ( int *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_NOT_EQ_Scalar_NOT_EQ_Array
          ( int *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_NOT_EQ_Scalar_NOT_EQ_Array_Accumulate_To_Operand
          ( int *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_AND_Array_AND_Array
          ( int *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_AND_Array_AND_Scalar
          ( int *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_AND_Scalar_AND_Array
          ( int *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_AND_Scalar_AND_Array_Accumulate_To_Operand
          ( int *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_OR_Array_OR_Array
          ( int *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_f_OR_Array_OR_Scalar
          ( int *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_OR_Scalar_OR_Array
          ( int *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_OR_Scalar_OR_Array_Accumulate_To_Operand
          ( int *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* D e f i n e  these seperately since these is no <== operator which would be 
   built otherwise!  The functions are not called but are required to support the
   binary operations for the logical functions.
*/
#if (1)
void MDI_f_LT_Array_LT_Array_Accumulate_To_Operand 
          ( int *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_LT_Array_LT_Scalar_Accumulate_To_Operand
          ( int *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_GT_Array_GT_Array_Accumulate_To_Operand 
          ( int *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_GT_Array_GT_Scalar_Accumulate_To_Operand
          ( int *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_LTEQ_Array_LTEQ_Array_Accumulate_To_Operand 
          ( int *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_LTEQ_Array_LTEQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_GTEQ_Array_GTEQ_Array_Accumulate_To_Operand 
          ( int *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_GTEQ_Array_GTEQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_EQ_Array_EQ_Array_Accumulate_To_Operand 
          ( int *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_EQ_Array_EQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_NOT_EQ_Array_NOT_EQ_Array_Accumulate_To_Operand 
          ( int *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_NOT_EQ_Array_NOT_EQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_AND_Array_AND_Array_Accumulate_To_Operand 
          ( int *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_AND_Array_AND_Scalar_Accumulate_To_Operand
          ( int *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_OR_Array_OR_Array_Accumulate_To_Operand 
          ( int *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_OR_Array_OR_Scalar_Accumulate_To_Operand
          ( int *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

#endif

void MDI_f_Unary_Minus_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Unary_Minus_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Cos_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Cos_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Sin_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Sin_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Tan_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Tan_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Cos_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Cos_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Sin_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Sin_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Tan_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Tan_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Cosh_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Cosh_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Sinh_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Sinh_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Tanh_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Tanh_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Cosh_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Cosh_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Sinh_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Sinh_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Tanh_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Tanh_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_f_NOT_Array_Returning_IntArray
          ( int *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_NOT_Array_Accumulate_To_Operand_Returning_IntArray
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* (2/5/2000) New conversion operator support */
void MDI_f_Array_convertTo_intArray_Array_Accumulate_To_Operand 
          ( int *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Array_convertTo_intArray_Scalar_Accumulate_To_Operand
          ( int *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Array_convertTo_floatArray_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Array_convertTo_floatArray_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Array_convertTo_doubleArray_Array_Accumulate_To_Operand 
          ( double *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Array_convertTo_doubleArray_Scalar_Accumulate_To_Operand
          ( double *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_f_Log_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Log_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Log10_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Log10_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Exp_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Exp_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Sqrt_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Sqrt_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Fabs_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Fabs_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Abs_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Abs_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Ceil_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Ceil_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Floor_Array 
          ( float *Result , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Floor_Array_Accumulate_To_Operand
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

 
void MDI_f_If_Array_Use_Array
          ( float *Result , int *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_If_Array_Use_Scalar
          ( float *Result , int *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_If_Scalar_Use_Array
          ( float *Result , float *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_f_Fmod_Array_Modulo_Array
          ( float *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Fmod_Array_Modulo_Scalar
          ( float *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Fmod_Scalar_Modulo_Array
          ( float *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Fmod_Scalar_Modulo_Array_Accumulate_To_Operand
          ( float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Fmod_Array_Modulo_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Fmod_Array_Modulo_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Pow_Array_Raised_To_Array
          ( float *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Pow_Array_Raised_To_Scalar
          ( float *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Pow_Scalar_Raised_To_Array
          ( float *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Pow_Scalar_Raised_To_Array_Accumulate_To_Operand
          ( float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Pow_Array_Raised_To_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Pow_Array_Raised_To_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Tan2_Array_ArcTan2_Array
          ( float *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Tan2_Array_ArcTan2_Scalar
          ( float *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Arc_Tan2_Scalar_ArcTan2_Array
          ( float *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Arc_Tan2_Scalar_ArcTan2_Array_Accumulate_To_Operand
          ( float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Tan2_Array_ArcTan2_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Arc_Tan2_Array_ArcTan2_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Min_Array_And_Array
          ( float *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Min_Array_And_Scalar
          ( float *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Min_Scalar_And_Array
          ( float *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Min_Scalar_And_Array_Accumulate_To_Operand
          ( float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Min_Array_And_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Min_Array_And_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Max_Array_And_Array
          ( float *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Max_Array_And_Scalar
          ( float *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Max_Scalar_And_Array
          ( float *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Max_Scalar_And_Array_Accumulate_To_Operand
          ( float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Max_Array_And_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Max_Array_And_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_f_Sign_Array_Of_Array
          ( float *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Sign_Array_Of_Scalar
          ( float *Result , float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Sign_Scalar_Of_Array
          ( float *Result , float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_f_Sign_Scalar_Of_Array_Accumulate_To_Operand
          ( float *Rhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Sign_Array_Of_Array_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Sign_Array_Of_Scalar_Accumulate_To_Operand
          ( float *Lhs , float x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


float MDI_f_Sum_Array_Returning_Scalar
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

float MDI_f_Max_Array_Returning_Scalar
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

float MDI_f_Min_Array_Returning_Scalar
          ( float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_f_Sum_Array_Along_Axis
          ( int Axis , float *Result , float *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* ***********************************************************************************
// We provide specific support for this "Where_Mask = !Where_Mask && Else_Where_Mask"
// since it is an important part of the efficiency of the "where" statement support
// which we provide in thei array class library.
// ***********************************************************************************
*/

void MDI_f_Else_Where_NOT_X_AND_Y_Operator
          ( float *Result , float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_f_Else_Where_NOT_X_AND_Y_Operator_Accumulate_To_Operand 
          ( float *Lhs , float *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

/* Machine dependent memory allocation and deallocation functions! */
float* MDI_float_Allocate ( array_domain* Descriptor );
void MDI_float_Deallocate ( float* Data_Pointer , array_domain* Descriptor );
void MDI_float_Change_Base ( float** Array_Data_Pointer , array_domain* Descriptor , int Axis , int New_Base );
void MDI_float_Change_All_Bases ( float** Array_Data_Pointer , array_domain* Descriptor , 
                               int New_Base_I , int New_Base_J , int New_Base_K , int New_Base_L );
void MDI_float_Zero_Raw_Data_Pointer ( float** Array_Data_Pointer , array_domain* Descriptor );
void MDI_float_Adjust_Raw_Data_Pointer ( float** Array_Data_Pointer , array_domain* Descriptor , int* Old_Base );

void MDI_int_Print_Array ( int *Result , array_domain *Result_Descriptor , int Display_Format );


/* Build the operator= related functions! */

void MDI_i_Assign_Array_Equals_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Assign_Array_Equals_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_i_Add_Array_Plus_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_Add_Array_Plus_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Add_Scalar_Plus_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Add_Scalar_Plus_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Subtract_Array_Minus_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_Subtract_Array_Minus_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Subtract_Scalar_Minus_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Subtract_Scalar_Minus_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Multiply_Array_Times_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_Multiply_Array_Times_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Multiply_Scalar_Times_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Multiply_Scalar_Times_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Divide_Array_Divided_By_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_Divide_Array_Divided_By_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Divide_Scalar_Divided_By_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Divide_Scalar_Divided_By_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* D e f i n e  these seperately so that we can correctly  d e f i n e  the relational operators! */
void MDI_i_Add_Array_Plus_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Add_Array_Plus_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Subtract_Array_Minus_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Subtract_Array_Minus_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Multiply_Array_Times_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Multiply_Array_Times_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Divide_Array_Divided_By_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Divide_Array_Divided_By_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_i_LT_Array_LT_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_LT_Array_LT_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_LT_Scalar_LT_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_LT_Scalar_LT_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_GT_Array_GT_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_GT_Array_GT_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_GT_Scalar_GT_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_GT_Scalar_GT_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_LTEQ_Array_LTEQ_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_LTEQ_Array_LTEQ_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_LTEQ_Scalar_LTEQ_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_LTEQ_Scalar_LTEQ_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_GTEQ_Array_GTEQ_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_GTEQ_Array_GTEQ_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_GTEQ_Scalar_GTEQ_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_GTEQ_Scalar_GTEQ_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_EQ_Array_EQ_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_EQ_Array_EQ_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_EQ_Scalar_EQ_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_EQ_Scalar_EQ_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_NOT_EQ_Array_NOT_EQ_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_NOT_EQ_Array_NOT_EQ_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_NOT_EQ_Scalar_NOT_EQ_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_NOT_EQ_Scalar_NOT_EQ_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_AND_Array_AND_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_AND_Array_AND_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_AND_Scalar_AND_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_AND_Scalar_AND_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_OR_Array_OR_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_OR_Array_OR_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_OR_Scalar_OR_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_OR_Scalar_OR_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* (2/5/2000) New bitwise operators (defined for intArrays only) */
void MDI_i_BIT_COMPLEMENT_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_COMPLEMENT_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_AND_Array_BitwiseAND_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_BIT_AND_Array_BitwiseAND_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_BIT_AND_Scalar_BitwiseAND_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_BIT_AND_Scalar_BitwiseAND_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_OR_Array_BitwiseOR_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_BIT_OR_Array_BitwiseOR_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_BIT_OR_Scalar_BitwiseOR_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_BIT_OR_Scalar_BitwiseOR_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_XOR_Array_BitwiseXOR_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_BIT_XOR_Array_BitwiseXOR_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_BIT_XOR_Scalar_BitwiseXOR_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_BIT_XOR_Scalar_BitwiseXOR_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_LSHIFT_Array_BitwiseLShift_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_BIT_LSHIFT_Array_BitwiseLShift_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_BIT_LSHIFT_Scalar_BitwiseLShift_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_BIT_LSHIFT_Scalar_BitwiseLShift_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_RSHIFT_Array_BitwiseRShift_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor , 
            array_domain *Mask_Descriptor );

void MDI_i_BIT_RSHIFT_Array_BitwiseRShift_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_BIT_RSHIFT_Scalar_BitwiseRShift_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_BIT_RSHIFT_Scalar_BitwiseRShift_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_i_BIT_AND_Array_BitwiseAND_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_AND_Array_BitwiseAND_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_OR_Array_BitwiseOR_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_OR_Array_BitwiseOR_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_XOR_Array_BitwiseXOR_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_XOR_Array_BitwiseXOR_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* Build these functions so to maintain symetry in the Abstract operator interface
   but don't try to have them define the <<= and >>= operators since these don't exist
   in C or C++.  We need the functions to exist so that we can simplify the design of A++/P++.
   Here we just define their implementation to be the "=" operator (we need something that will compile).
*/
void MDI_i_BIT_LSHIFT_Array_BitwiseLShift_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_LSHIFT_Array_BitwiseLShift_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_RSHIFT_Array_BitwiseRShift_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_BIT_RSHIFT_Array_BitwiseRShift_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* (2/5/2000) New conversion operator support */
void MDI_i_Array_convertTo_intArray_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Array_convertTo_intArray_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Array_convertTo_floatArray_Array_Accumulate_To_Operand 
          ( float *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Array_convertTo_floatArray_Scalar_Accumulate_To_Operand
          ( float *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Array_convertTo_doubleArray_Array_Accumulate_To_Operand 
          ( double *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Array_convertTo_doubleArray_Scalar_Accumulate_To_Operand
          ( double *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );



/* D e f i n e  these seperately since these is no <== operator which would be 
   built otherwise!  The functions are not called but are required to support the
   binary operations for the logical functions.
*/
#if (1)
void MDI_i_LT_Array_LT_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_LT_Array_LT_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_GT_Array_GT_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_GT_Array_GT_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_LTEQ_Array_LTEQ_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_LTEQ_Array_LTEQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_GTEQ_Array_GTEQ_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_GTEQ_Array_GTEQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_EQ_Array_EQ_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_EQ_Array_EQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_NOT_EQ_Array_NOT_EQ_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_NOT_EQ_Array_NOT_EQ_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_AND_Array_AND_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_AND_Array_AND_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_OR_Array_OR_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_OR_Array_OR_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

#endif

void MDI_i_Unary_Minus_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Unary_Minus_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

#if (0)
void MDI_i_Cos_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Cos_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Sin_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Sin_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Tan_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Tan_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Cos_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Cos_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Sin_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Sin_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Tan_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Tan_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Cosh_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Cosh_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Sinh_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Sinh_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Tanh_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Tanh_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Cosh_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Cosh_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Sinh_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Sinh_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Tanh_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Arc_Tanh_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

#endif

void MDI_i_NOT_Array_Returning_IntArray
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_NOT_Array_Accumulate_To_Operand_Returning_IntArray
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


#if (0)
void MDI_i_Log_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Log_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Log10_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Log10_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Exp_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Exp_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Sqrt_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Sqrt_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Fabs_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Fabs_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Ceil_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Ceil_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Floor_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Floor_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

#endif
 
void MDI_i_Abs_Array 
          ( int *Result , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Abs_Array_Accumulate_To_Operand
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


/*
void MDI_i_If_Array_Use_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_If_Array_Use_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_If_Scalar_Use_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_If_Scalar_Use_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_If_Array_Use_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_If_Array_Use_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

*/
void MDI_i_If_Array_Use_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_If_Array_Use_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_If_Scalar_Use_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_i_Fmod_Array_Modulo_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Fmod_Array_Modulo_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Fmod_Scalar_Modulo_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Fmod_Scalar_Modulo_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Fmod_Array_Modulo_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Fmod_Array_Modulo_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_i_Pow_Array_Raised_To_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Pow_Array_Raised_To_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Pow_Scalar_Raised_To_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Pow_Scalar_Raised_To_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Pow_Array_Raised_To_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Pow_Array_Raised_To_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Min_Array_And_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Min_Array_And_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Min_Scalar_And_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Min_Scalar_And_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Min_Array_And_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Min_Array_And_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Max_Array_And_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Max_Array_And_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Max_Scalar_And_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Max_Scalar_And_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Max_Array_And_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Max_Array_And_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_i_Sign_Array_Of_Array
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain* Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Sign_Array_Of_Scalar
          ( int *Result , int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Sign_Scalar_Of_Array
          ( int *Result , int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );
 
void MDI_i_Sign_Scalar_Of_Array_Accumulate_To_Operand
          ( int *Rhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Sign_Array_Of_Array_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Sign_Array_Of_Scalar_Accumulate_To_Operand
          ( int *Lhs , int x ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


int MDI_i_Sum_Array_Returning_Scalar
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

int MDI_i_Max_Array_Returning_Scalar
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );

int MDI_i_Min_Array_Returning_Scalar
          ( int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


void MDI_i_Sum_Array_Along_Axis
          ( int Axis , int *Result , int *Lhs ,
            int *Mask_Array_Pointer ,
            array_domain *Result_Descriptor ,
            array_domain *Lhs_Descriptor ,
            array_domain *Mask_Descriptor );


/* ***********************************************************************************
// We provide specific support for this "Where_Mask = !Where_Mask && Else_Where_Mask"
// since it is an important part of the efficiency of the "where" statement support
// which we provide in thei array class library.
// ***********************************************************************************
*/

void MDI_i_Else_Where_NOT_X_AND_Y_Operator
          ( int *Result , int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Descriptor ,
            array_domain *Lhs_Descriptor , 
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

void MDI_i_Else_Where_NOT_X_AND_Y_Operator_Accumulate_To_Operand 
          ( int *Lhs , int *Rhs ,
            int *Mask_Array_Pointer ,
            array_domain *Lhs_Descriptor ,
            array_domain *Rhs_Descriptor ,
            array_domain *Mask_Descriptor );

/* Machine dependent memory allocation and deallocation functions! */
int* MDI_int_Allocate ( array_domain* Descriptor );
void MDI_int_Deallocate ( int* Data_Pointer , array_domain* Descriptor );
void MDI_int_Change_Base ( int** Array_Data_Pointer , array_domain* Descriptor , int Axis , int New_Base );
void MDI_int_Change_All_Bases ( int** Array_Data_Pointer , array_domain* Descriptor , 
                               int New_Base_I , int New_Base_J , int New_Base_K , int New_Base_L );
void MDI_int_Zero_Raw_Data_Pointer ( int** Array_Data_Pointer , array_domain* Descriptor );
void MDI_int_Adjust_Raw_Data_Pointer ( int** Array_Data_Pointer , array_domain* Descriptor , int* Old_Base );

void MDI_Build_Index_Map_Array ( int *Result , int *Lhs , int *Mask_Array_Pointer , 
                                 array_domain *Descriptor , array_domain *Lhs_Descriptor , array_domain *Mask_Descriptor );

#ifdef	__cplusplus
}
#endif

#endif  /* !defined(_APP_MDI_MACHINE_H) */



/* **************************************************************** 
// **************************************************************** 
// Define the macros we will use for all the operator's expansions!
// **************************************************************** 
// **************************************************************** 
//
// Define the marcos used by the operator= and operators +,-,*, and /
// 
// Use the d i v e r t  m4 function to avoid output of macro definition!
*/


/*
//
// D e f i n e  macro for use by operators +,-,*, and /
*/

/*
//
// D e f i n e  macro for use by min, max, pow, sign, fmod, etc.
*/


/* 
// 
// Macro for Unary operations! (like unary minus, cos, sin, arc_sin, tan, etc.)
*/ 
 
/* 
// 
// Macro for Unary operations returning intArray! (like $1Array::operator! )
*/ 
 







/* 
// 
// Macro expansions!
*/ 




