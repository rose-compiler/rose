// Bug in unparsing abstract_op.C

/*
Original code:
 doubleArray_Function_5 *Execution_Object = new doubleArray_Function_5 
	     ( Assignment , MDI_d_Assign_Array_Equals_Scalar_Accumulate_To_Operand ,
	       *this , x );

Unparsed code:
 class doubleArray_Function_5 * Execution_Object =
     new doubleArray_Function_5 (doubleArray::Assignment,
     ((void (* )(double * , double , int * , array_domain * , array_domain * , double * , double , int * , array_domain * , array_domain * ))MDI_d_Assign_Array_Equals_Scalar_Accumulate_To_Operand),
     *this,x); 
 */

int
main()
   {
     return 0;
   }

