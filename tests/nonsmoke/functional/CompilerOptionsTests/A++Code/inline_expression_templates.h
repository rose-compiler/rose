// Allow repeated includes of array.h without error 
#ifndef _APP_INLINE_EXPRESSION_TEMPLATES_H
#define _APP_INLINE_EXPRESSION_TEMPLATES_H

#if 0
// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#endif
#endif









/* MACRO EXPANSION BEGINS HERE */

// Comment this out for now
// define inline inline

// There are some functions we want defined in the header 
// file and othere we want to have be optionally defined in the 
// C++ source files so that the inlining will not generate
// too large of an intermediate source.

#define DOUBLEARRAY
#if defined(USE_EXPRESSION_TEMPLATES)
#if 0
  inline double InArray<double>::operator[](int i) const
     {
       printf ("Inside of InArray<double>::operator[](i=%d) \n",i);
       APP_ASSERT (doubleArrayPointer != NULL);
       return doubleArrayPointer->operator[](i);
     }
#endif

#if 1
// I don't think we need these shape access functions! WE DO!
  inline const Array_Domain_Type &InArray<double>::shape() const APP_RESTRICT_MACRO
     { 
#if COMPILE_DEBUG_STATEMENTS
    // APP_ASSERT (doubleArrayPointer != NULL);
#endif
    // return doubleArrayPointer->Array_Descriptor; 
       return Array_Descriptor.Array_Domain; 
     }

  inline const Array_Domain_Type *InArray<double>::shapePtr() const APP_RESTRICT_MACRO
     { 
#if COMPILE_DEBUG_STATEMENTS
    // APP_ASSERT (doubleArrayPointer != NULL);
#endif
       return &Array_Descriptor.Array_Domain; 
     }
#endif

  inline Boolean InArray<double>::usesIndirectAddressing() const APP_RESTRICT_MACRO
     {
#if COMPILE_DEBUG_STATEMENTS
    // printf ("Inside of InArray<double>::usesIndirectAddressing() \n");
    // APP_ASSERT (doubleArrayPointer != NULL);
#endif
    // return doubleArrayPointer->Array_Descriptor.Uses_Indirect_Addressing;
       return Array_Descriptor.usesIndirectAddressing();
     }

  inline int InArray<double>::internalNumberOfDimensions() const APP_RESTRICT_MACRO
     {
#if COMPILE_DEBUG_STATEMENTS
    // printf ("Inside of InArray<double>::internalNumberOfDimensions() \n");
    // APP_ASSERT (doubleArrayPointer != NULL);
#endif
    // return doubleArrayPointer->Array_Descriptor.internalNumberOfDimensions();
       return Array_Descriptor.internalNumberOfDimensions();
     }

  inline Boolean InArray<double>::containsArrayID ( int lhs_id ) const APP_RESTRICT_MACRO
     {
    // Used to check for loop dependence
#if COMPILE_DEBUG_STATEMENTS
    // APP_ASSERT (doubleArrayPointer != NULL);
       if (APP_DEBUG > 2)
            printf ("Inside of InArray<double>::containsArrayID(int lhs_id = %d) Array_Descriptor.Array_ID = %d \n",
                 lhs_id,Array_Descriptor.Array_Domain.Array_ID);
#endif
    // return (doubleArrayPointer->Array_Descriptor.Array_ID == lhs_id);
       return (Array_Descriptor.Array_Domain.Array_ID == lhs_id);
     }

#if 0
// We don't need these functions!
  inline Boolean doubleArray::usesIndirectAddressing() const
     {
       printf ("Inside of doubleArray::usesIndirectAddressing() \n");
       return FALSE;
     }

  inline Boolean doubleArray::containsArrayID ( int lhs_id ) const
     {
    // Used to check for loop dependence Array member function always returns FALSE.
    // This function MUST return false since it makes nosense for the Lhs to
    // have a loop dependence on the lhs this function is only used when there is no
    // RHS operand or it is a scalar.
       printf ("Inside of doubleArray::containsArrayID(int lhs_id) \n");
       return FALSE;
     }
#endif

  inline void InArray<double>::setupIndirectAddressingSupport() const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray<double>::setupIndirectAddressingSupport() \n");
#endif
 // doubleArrayPointer->setupIndirectAddressingSupport();
 // printf ("Are we supposed to call recursively? \n");
 // setupIndirectAddressingSupport();
    Array_Descriptor.setupIndirectAddressingSupport();
  }

 
  inline double InArray<double>::expand(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i) \n");
 // return doubleArrayPointer->getDataPointer()[doubleArrayPointer->Array_Descriptor.offset0(i)];
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i)];
  }


  inline double InArray<double>::indirect_expand_1D ( int i ) const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::indirect_expand_1D(int i) \n");
#endif
    return Array_Descriptor.Array_View_Pointer0[Array_Descriptor.indirectOffset_1D(i)];
  }

#if (MAX_ARRAY_DIMENSION >= 2)
  inline double InArray<double>::expand ( int i, int j ) const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::expand(int i, int j) value = %f Array_Data = %p \n",
              Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j)],
              Array_Descriptor.getDataPointer());
#endif
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j)];
  }

#if 0
  inline int doubleArray::indirectOffset_2D ( int i ) const
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::indirectOffset_2D(int i) \n");
#endif
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i));
  }
#endif

  inline double InArray<double>::indirect_expand_2D(int i) const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::indirect_expand(int i, int j) \n");
#endif
    return Array_Descriptor.Array_View_Pointer1[Array_Descriptor.indirectOffset_2D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
  inline double InArray<double>::expand(int i, int j, int k) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k)];
 // return Array_Descriptor.ExpressionTemplateDataPointer[Array_Descriptor.offset(i,j,k)];
  }

#if 0
  inline int doubleArray::indirectOffset_3D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_3D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i));
  }
#endif

  inline double InArray<double>::indirect_expand_3D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_3D(int i) \n");
    return Array_Descriptor.Array_View_Pointer2[Array_Descriptor.indirectOffset_3D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
  inline double InArray<double>::expand(int i, int j, int k, int l) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l)];
  }

#if 0
  inline int doubleArray::indirectOffset_4D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_4D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i));
  }
#endif

  inline double InArray<double>::indirect_expand_4D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_4D(int i) \n");
    return Array_Descriptor.Array_View_Pointer3[Array_Descriptor.indirectOffset_4D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
  inline double InArray<double>::expand(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m)];
  }

#if 0
  inline int doubleArray::indirectOffset_5D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_5D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i));
  }
#endif

  inline double InArray<double>::indirect_expand_5D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_5D(int i) \n");
    return Array_Descriptor.Array_View_Pointer4[Array_Descriptor.indirectOffset_5D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
  inline double InArray<double>::expand(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m, int n) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m,n)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m,n)];
  }

#if 0
  inline int doubleArray::indirectOffset_6D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_6D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i),
                                   computeSubscript(5,i));
  }
#endif

  inline double InArray<double>::indirect_expand_6D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_6D(int i) \n");
    return Array_Descriptor.Array_View_Pointer5[Array_Descriptor.indirectOffset_6D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
  inline double InArray<double>::expand(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m, int n, int o) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m,n,o)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m,n,o)];
  }

#if 0
  inline int doubleArray::indirectOffset_7D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_7D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i),
                                   computeSubscript(5,i),
                                   computeSubscript(6,i));
  }
#endif

  inline double InArray<double>::indirect_expand_7D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_7D(int i) \n");
    return Array_Descriptor.Array_View_Pointer6[Array_Descriptor.indirectOffset_7D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
  inline double InArray<double>::expand(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m, int n, int o, int p) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m,n,o,p)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m,n,o,p)];
  }
#if 0
  inline int doubleArray::indirectOffset_8D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_8D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i),
                                   computeSubscript(5,i),
                                   computeSubscript(6,i),
                                   computeSubscript(7,i));
  }
#endif

  inline double InArray<double>::indirect_expand_8D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_8D(int i) \n");
    return Array_Descriptor.Array_View_Pointer7[Array_Descriptor.indirectOffset_8D(i)];
  }
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif

#undef DOUBLEARRAY

#define FLOATARRAY
#if defined(USE_EXPRESSION_TEMPLATES)
#if 0
  inline float InArray<float>::operator[](int i) const
     {
       printf ("Inside of InArray<float>::operator[](i=%d) \n",i);
       APP_ASSERT (floatArrayPointer != NULL);
       return floatArrayPointer->operator[](i);
     }
#endif

#if 1
// I don't think we need these shape access functions! WE DO!
  inline const Array_Domain_Type &InArray<float>::shape() const APP_RESTRICT_MACRO
     { 
#if COMPILE_DEBUG_STATEMENTS
    // APP_ASSERT (floatArrayPointer != NULL);
#endif
    // return floatArrayPointer->Array_Descriptor; 
       return Array_Descriptor.Array_Domain; 
     }

  inline const Array_Domain_Type *InArray<float>::shapePtr() const APP_RESTRICT_MACRO
     { 
#if COMPILE_DEBUG_STATEMENTS
    // APP_ASSERT (floatArrayPointer != NULL);
#endif
       return &Array_Descriptor.Array_Domain; 
     }
#endif

  inline Boolean InArray<float>::usesIndirectAddressing() const APP_RESTRICT_MACRO
     {
#if COMPILE_DEBUG_STATEMENTS
    // printf ("Inside of InArray<float>::usesIndirectAddressing() \n");
    // APP_ASSERT (floatArrayPointer != NULL);
#endif
    // return floatArrayPointer->Array_Descriptor.Uses_Indirect_Addressing;
       return Array_Descriptor.usesIndirectAddressing();
     }

  inline int InArray<float>::internalNumberOfDimensions() const APP_RESTRICT_MACRO
     {
#if COMPILE_DEBUG_STATEMENTS
    // printf ("Inside of InArray<float>::internalNumberOfDimensions() \n");
    // APP_ASSERT (floatArrayPointer != NULL);
#endif
    // return floatArrayPointer->Array_Descriptor.internalNumberOfDimensions();
       return Array_Descriptor.internalNumberOfDimensions();
     }

  inline Boolean InArray<float>::containsArrayID ( int lhs_id ) const APP_RESTRICT_MACRO
     {
    // Used to check for loop dependence
#if COMPILE_DEBUG_STATEMENTS
    // APP_ASSERT (floatArrayPointer != NULL);
       if (APP_DEBUG > 2)
            printf ("Inside of InArray<float>::containsArrayID(int lhs_id = %d) Array_Descriptor.Array_ID = %d \n",
                 lhs_id,Array_Descriptor.Array_Domain.Array_ID);
#endif
    // return (floatArrayPointer->Array_Descriptor.Array_ID == lhs_id);
       return (Array_Descriptor.Array_Domain.Array_ID == lhs_id);
     }

#if 0
// We don't need these functions!
  inline Boolean floatArray::usesIndirectAddressing() const
     {
       printf ("Inside of floatArray::usesIndirectAddressing() \n");
       return FALSE;
     }

  inline Boolean floatArray::containsArrayID ( int lhs_id ) const
     {
    // Used to check for loop dependence Array member function always returns FALSE.
    // This function MUST return false since it makes nosense for the Lhs to
    // have a loop dependence on the lhs this function is only used when there is no
    // RHS operand or it is a scalar.
       printf ("Inside of floatArray::containsArrayID(int lhs_id) \n");
       return FALSE;
     }
#endif

  inline void InArray<float>::setupIndirectAddressingSupport() const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray<float>::setupIndirectAddressingSupport() \n");
#endif
 // floatArrayPointer->setupIndirectAddressingSupport();
 // printf ("Are we supposed to call recursively? \n");
 // setupIndirectAddressingSupport();
    Array_Descriptor.setupIndirectAddressingSupport();
  }

 
  inline float InArray<float>::expand(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i) \n");
 // return floatArrayPointer->getDataPointer()[floatArrayPointer->Array_Descriptor.offset0(i)];
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i)];
  }


  inline float InArray<float>::indirect_expand_1D ( int i ) const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::indirect_expand_1D(int i) \n");
#endif
    return Array_Descriptor.Array_View_Pointer0[Array_Descriptor.indirectOffset_1D(i)];
  }

#if (MAX_ARRAY_DIMENSION >= 2)
  inline float InArray<float>::expand ( int i, int j ) const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::expand(int i, int j) value = %f Array_Data = %p \n",
              Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j)],
              Array_Descriptor.getDataPointer());
#endif
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j)];
  }

#if 0
  inline int floatArray::indirectOffset_2D ( int i ) const
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::indirectOffset_2D(int i) \n");
#endif
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i));
  }
#endif

  inline float InArray<float>::indirect_expand_2D(int i) const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::indirect_expand(int i, int j) \n");
#endif
    return Array_Descriptor.Array_View_Pointer1[Array_Descriptor.indirectOffset_2D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
  inline float InArray<float>::expand(int i, int j, int k) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k)];
 // return Array_Descriptor.ExpressionTemplateDataPointer[Array_Descriptor.offset(i,j,k)];
  }

#if 0
  inline int floatArray::indirectOffset_3D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_3D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i));
  }
#endif

  inline float InArray<float>::indirect_expand_3D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_3D(int i) \n");
    return Array_Descriptor.Array_View_Pointer2[Array_Descriptor.indirectOffset_3D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
  inline float InArray<float>::expand(int i, int j, int k, int l) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l)];
  }

#if 0
  inline int floatArray::indirectOffset_4D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_4D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i));
  }
#endif

  inline float InArray<float>::indirect_expand_4D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_4D(int i) \n");
    return Array_Descriptor.Array_View_Pointer3[Array_Descriptor.indirectOffset_4D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
  inline float InArray<float>::expand(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m)];
  }

#if 0
  inline int floatArray::indirectOffset_5D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_5D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i));
  }
#endif

  inline float InArray<float>::indirect_expand_5D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_5D(int i) \n");
    return Array_Descriptor.Array_View_Pointer4[Array_Descriptor.indirectOffset_5D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
  inline float InArray<float>::expand(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m, int n) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m,n)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m,n)];
  }

#if 0
  inline int floatArray::indirectOffset_6D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_6D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i),
                                   computeSubscript(5,i));
  }
#endif

  inline float InArray<float>::indirect_expand_6D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_6D(int i) \n");
    return Array_Descriptor.Array_View_Pointer5[Array_Descriptor.indirectOffset_6D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
  inline float InArray<float>::expand(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m, int n, int o) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m,n,o)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m,n,o)];
  }

#if 0
  inline int floatArray::indirectOffset_7D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_7D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i),
                                   computeSubscript(5,i),
                                   computeSubscript(6,i));
  }
#endif

  inline float InArray<float>::indirect_expand_7D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_7D(int i) \n");
    return Array_Descriptor.Array_View_Pointer6[Array_Descriptor.indirectOffset_7D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
  inline float InArray<float>::expand(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m, int n, int o, int p) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m,n,o,p)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m,n,o,p)];
  }
#if 0
  inline int floatArray::indirectOffset_8D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_8D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i),
                                   computeSubscript(5,i),
                                   computeSubscript(6,i),
                                   computeSubscript(7,i));
  }
#endif

  inline float InArray<float>::indirect_expand_8D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_8D(int i) \n");
    return Array_Descriptor.Array_View_Pointer7[Array_Descriptor.indirectOffset_8D(i)];
  }
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif

#undef FLOATARRAY

#define INTARRAY
#if defined(USE_EXPRESSION_TEMPLATES)
#if 0
  inline int InArray<int>::operator[](int i) const
     {
       printf ("Inside of InArray<int>::operator[](i=%d) \n",i);
       APP_ASSERT (intArrayPointer != NULL);
       return intArrayPointer->operator[](i);
     }
#endif

#if 1
// I don't think we need these shape access functions! WE DO!
  inline const Array_Domain_Type &InArray<int>::shape() const APP_RESTRICT_MACRO
     { 
#if COMPILE_DEBUG_STATEMENTS
    // APP_ASSERT (intArrayPointer != NULL);
#endif
    // return intArrayPointer->Array_Descriptor; 
       return Array_Descriptor.Array_Domain; 
     }

  inline const Array_Domain_Type *InArray<int>::shapePtr() const APP_RESTRICT_MACRO
     { 
#if COMPILE_DEBUG_STATEMENTS
    // APP_ASSERT (intArrayPointer != NULL);
#endif
       return &Array_Descriptor.Array_Domain; 
     }
#endif

  inline Boolean InArray<int>::usesIndirectAddressing() const APP_RESTRICT_MACRO
     {
#if COMPILE_DEBUG_STATEMENTS
    // printf ("Inside of InArray<int>::usesIndirectAddressing() \n");
    // APP_ASSERT (intArrayPointer != NULL);
#endif
    // return intArrayPointer->Array_Descriptor.Uses_Indirect_Addressing;
       return Array_Descriptor.usesIndirectAddressing();
     }

  inline int InArray<int>::internalNumberOfDimensions() const APP_RESTRICT_MACRO
     {
#if COMPILE_DEBUG_STATEMENTS
    // printf ("Inside of InArray<int>::internalNumberOfDimensions() \n");
    // APP_ASSERT (intArrayPointer != NULL);
#endif
    // return intArrayPointer->Array_Descriptor.internalNumberOfDimensions();
       return Array_Descriptor.internalNumberOfDimensions();
     }

  inline Boolean InArray<int>::containsArrayID ( int lhs_id ) const APP_RESTRICT_MACRO
     {
    // Used to check for loop dependence
#if COMPILE_DEBUG_STATEMENTS
    // APP_ASSERT (intArrayPointer != NULL);
       if (APP_DEBUG > 2)
            printf ("Inside of InArray<int>::containsArrayID(int lhs_id = %d) Array_Descriptor.Array_ID = %d \n",
                 lhs_id,Array_Descriptor.Array_Domain.Array_ID);
#endif
    // return (intArrayPointer->Array_Descriptor.Array_ID == lhs_id);
       return (Array_Descriptor.Array_Domain.Array_ID == lhs_id);
     }

#if 0
// We don't need these functions!
  inline Boolean intArray::usesIndirectAddressing() const
     {
       printf ("Inside of intArray::usesIndirectAddressing() \n");
       return FALSE;
     }

  inline Boolean intArray::containsArrayID ( int lhs_id ) const
     {
    // Used to check for loop dependence Array member function always returns FALSE.
    // This function MUST return false since it makes nosense for the Lhs to
    // have a loop dependence on the lhs this function is only used when there is no
    // RHS operand or it is a scalar.
       printf ("Inside of intArray::containsArrayID(int lhs_id) \n");
       return FALSE;
     }
#endif

  inline void InArray<int>::setupIndirectAddressingSupport() const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray<int>::setupIndirectAddressingSupport() \n");
#endif
 // intArrayPointer->setupIndirectAddressingSupport();
 // printf ("Are we supposed to call recursively? \n");
 // setupIndirectAddressingSupport();
    Array_Descriptor.setupIndirectAddressingSupport();
  }

 
  inline int InArray<int>::expand(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i) \n");
 // return intArrayPointer->getDataPointer()[intArrayPointer->Array_Descriptor.offset0(i)];
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i)];
  }


  inline int InArray<int>::indirect_expand_1D ( int i ) const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::indirect_expand_1D(int i) \n");
#endif
    return Array_Descriptor.Array_View_Pointer0[Array_Descriptor.indirectOffset_1D(i)];
  }

#if (MAX_ARRAY_DIMENSION >= 2)
  inline int InArray<int>::expand ( int i, int j ) const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::expand(int i, int j) value = %f Array_Data = %p \n",
              Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j)],
              Array_Descriptor.getDataPointer());
#endif
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j)];
  }

#if 0
  inline int intArray::indirectOffset_2D ( int i ) const
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::indirectOffset_2D(int i) \n");
#endif
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i));
  }
#endif

  inline int InArray<int>::indirect_expand_2D(int i) const APP_RESTRICT_MACRO
  {
#if COMPILE_DEBUG_STATEMENTS
    if (APP_DEBUG > 1)
         printf ("Inside of InArray::indirect_expand(int i, int j) \n");
#endif
    return Array_Descriptor.Array_View_Pointer1[Array_Descriptor.indirectOffset_2D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
  inline int InArray<int>::expand(int i, int j, int k) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k)];
 // return Array_Descriptor.ExpressionTemplateDataPointer[Array_Descriptor.offset(i,j,k)];
  }

#if 0
  inline int intArray::indirectOffset_3D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_3D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i));
  }
#endif

  inline int InArray<int>::indirect_expand_3D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_3D(int i) \n");
    return Array_Descriptor.Array_View_Pointer2[Array_Descriptor.indirectOffset_3D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
  inline int InArray<int>::expand(int i, int j, int k, int l) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l)];
  }

#if 0
  inline int intArray::indirectOffset_4D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_4D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i));
  }
#endif

  inline int InArray<int>::indirect_expand_4D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_4D(int i) \n");
    return Array_Descriptor.Array_View_Pointer3[Array_Descriptor.indirectOffset_4D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
  inline int InArray<int>::expand(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m)];
  }

#if 0
  inline int intArray::indirectOffset_5D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_5D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i));
  }
#endif

  inline int InArray<int>::indirect_expand_5D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_5D(int i) \n");
    return Array_Descriptor.Array_View_Pointer4[Array_Descriptor.indirectOffset_5D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
  inline int InArray<int>::expand(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m, int n) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m,n)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m,n)];
  }

#if 0
  inline int intArray::indirectOffset_6D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_6D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i),
                                   computeSubscript(5,i));
  }
#endif

  inline int InArray<int>::indirect_expand_6D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_6D(int i) \n");
    return Array_Descriptor.Array_View_Pointer5[Array_Descriptor.indirectOffset_6D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
  inline int InArray<int>::expand(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m, int n, int o) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m,n,o)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m,n,o)];
  }

#if 0
  inline int intArray::indirectOffset_7D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_7D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i),
                                   computeSubscript(5,i),
                                   computeSubscript(6,i));
  }
#endif

  inline int InArray<int>::indirect_expand_7D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_7D(int i) \n");
    return Array_Descriptor.Array_View_Pointer6[Array_Descriptor.indirectOffset_7D(i)];
  }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
  inline int InArray<int>::expand(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::expand(int i, int j, int k, int l, int m, int n, int o, int p) \n");
 // return Array_Descriptor.getDataPointer()[Array_Descriptor.offset0(i,j,k,l,m,n,o,p)];
    return Array_Descriptor.getExpressionTemplateDataPointer()[Array_Descriptor.offset(i,j,k,l,m,n,o,p)];
  }
#if 0
  inline int intArray::indirectOffset_8D ( int i ) const
  {
 // printf ("Inside of InArray::indirectOffset_8D(int i) \n");
    return Array_Descriptor.offset(computeSubscript(0,i),
                                   computeSubscript(1,i),
                                   computeSubscript(2,i),
                                   computeSubscript(3,i),
                                   computeSubscript(4,i),
                                   computeSubscript(5,i),
                                   computeSubscript(6,i),
                                   computeSubscript(7,i));
  }
#endif

  inline int InArray<int>::indirect_expand_8D(int i) const APP_RESTRICT_MACRO
  {
 // printf ("Inside of InArray::indirect_expand_8D(int i) \n");
    return Array_Descriptor.Array_View_Pointer7[Array_Descriptor.indirectOffset_8D(i)];
  }
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif

#undef INTARRAY

// This macro has no body!


#if defined(LOCATE_EXPRESSION_TEMPLATES_IN_HEADER_FILES)

#define DOUBLEARRAY
#if defined(USE_EXPRESSION_TEMPLATES)

#if defined(HAS_MEMBER_TEMPLATES)

#if defined(INTARRAY)
// error "In A++.C HAS_MEMBER_TEMPLATES is defined"
#endif

        template <class T1, class A>
     // inline doubleArray & doubleArray::operator=(const Indexable<T1, A> & Rhs)
        doubleArray & doubleArray::operator=(const Indexable<T1, A> & Rhs)
           {
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("TEST: Inside of doubleArray::operator=(const Indexable<T1, A> & Rhs) \n");
#endif

             if (Array_Descriptor.isNullArray() == TRUE)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 0)
                       printf ("Lhs is a NULL Array (redimensioning ...) \n");
#endif
               // WE NEED TO IMPROVE ON THIS!
               // This is a very expensive way to get the Lhs to be set to the right size
               // we need to generate the size info more efficiently
                  redim(doubleArray(Rhs));
                }

             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of doubleArray & InArray<double>::operator== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // double* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             double* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             double* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             double* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             double* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             double* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             double* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             double* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             doubleArray* Temp_Lhs_Array_Pointer = NULL;
          // double *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new doubleArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<doubleArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] = Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] = Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] = Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] = Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] = Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] = Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] = Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] = Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] = Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] = Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] = Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] = Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] = Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] = Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] = Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] = Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] = Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] = Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] = Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] = Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] = Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] = Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] = Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] = Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }



#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Leaving doubleArray::operator=(const Indexable<T1, A> & Rhs) \n");
#endif

             return *this;
           }

     // The use of the inline is a problem.  It forces some strange errors.
        template <class T1, class A>
     // inline doubleArray::doubleArray(const Indexable<T1, A> &Rhs) : InArray<double>(this) , Array_Descriptor(Rhs.shape())
     // doubleArray::doubleArray(const Indexable<T1, A> &Rhs)
     // doubleArray::doubleArray(const Indexable<T1, A> &Rhs) : InArray<double>(this)
     // doubleArray::doubleArray(const Indexable<T1, A> &Rhs) : InArray<double>(this) , Array_Descriptor(Rhs.shape())
        doubleArray::doubleArray(const Indexable<T1, A> &Rhs) : InArray<double>(Rhs.shape())
           {
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of doubleArray::doubleArray(const Indexable<T1, A> &Rhs) \n");
#endif
          // The base of the new array object should be zero (or maybe the APP_Default_Base?)
          // setBase(0);
          // The base of the new array object should be zero (or maybe the APP_Default_Base?)
          // It is too complicated to reset the base if we have used indirect addressing.
             if (Rhs.shape().usesIndirectAddressing() == FALSE)
                  setBase(0);

          // Allocate array memory!
             Array_Descriptor.Array_Data = NULL;
             Allocate_Array_Data (TRUE);
             Array_Storage  = NULL;
             referenceCount = 0;
             POINTER_LIST_INITIALIZATION_MACRO;

          // Array_Descriptor.display("Inside of doubleArray::doubleArray(const Indexable<T1, A> &Rhs)");
             operator= (Rhs);
           }

        template <class T1, class A>
        inline doubleArray & doubleArray::operator+=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of doubleArray & InArray<double>::operator+== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // double* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             double* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             double* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             double* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             double* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             double* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             double* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             double* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             doubleArray* Temp_Lhs_Array_Pointer = NULL;
          // double *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new doubleArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<doubleArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] += Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] += Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] += Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] += Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] += Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] += Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] += Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] += Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] += Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] += Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] += Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] += Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] += Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] += Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] += Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] += Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] += Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] += Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] += Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] += Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] += Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] += Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] += Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] += Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline doubleArray & doubleArray::operator-=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of doubleArray & InArray<double>::operator-== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // double* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             double* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             double* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             double* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             double* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             double* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             double* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             double* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             doubleArray* Temp_Lhs_Array_Pointer = NULL;
          // double *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new doubleArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<doubleArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] -= Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] -= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] -= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] -= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] -= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] -= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] -= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] -= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] -= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] -= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] -= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] -= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] -= Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] -= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] -= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] -= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] -= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] -= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] -= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] -= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] -= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] -= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] -= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] -= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline doubleArray & doubleArray::operator*=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of doubleArray & InArray<double>::operator*== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // double* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             double* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             double* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             double* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             double* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             double* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             double* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             double* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             doubleArray* Temp_Lhs_Array_Pointer = NULL;
          // double *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new doubleArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<doubleArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] *= Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] *= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] *= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] *= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] *= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] *= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] *= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] *= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] *= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] *= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] *= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] *= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] *= Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] *= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] *= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] *= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] *= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] *= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] *= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] *= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] *= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] *= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] *= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] *= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline doubleArray & doubleArray::operator/=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of doubleArray & InArray<double>::operator/== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // double* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             double* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             double* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             double* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             double* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             double* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             double* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             double* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             doubleArray* Temp_Lhs_Array_Pointer = NULL;
          // double *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new doubleArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<doubleArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] /= Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] /= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] /= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] /= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] /= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] /= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] /= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] /= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] /= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] /= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] /= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] /= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] /= Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] /= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] /= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] /= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] /= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] /= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] /= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] /= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] /= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] /= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] /= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] /= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline doubleArray & doubleArray::operator%=(const Indexable<T1, A> &rhs)
           {
          /* Array_Indexable_MACRO(double,,%=) */
             printf ("Sorry, not implemented: doubleArray::operator%= not defined in expression template version! \n");
             APP_ABORT();
             return *this;
           }

// End of if defined(HAS_MEMBER_TEMPLATES)
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif

#undef DOUBLEARRAY

#define FLOATARRAY
#if defined(USE_EXPRESSION_TEMPLATES)

#if defined(HAS_MEMBER_TEMPLATES)

#if defined(INTARRAY)
// error "In A++.C HAS_MEMBER_TEMPLATES is defined"
#endif

        template <class T1, class A>
     // inline floatArray & floatArray::operator=(const Indexable<T1, A> & Rhs)
        floatArray & floatArray::operator=(const Indexable<T1, A> & Rhs)
           {
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("TEST: Inside of floatArray::operator=(const Indexable<T1, A> & Rhs) \n");
#endif

             if (Array_Descriptor.isNullArray() == TRUE)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 0)
                       printf ("Lhs is a NULL Array (redimensioning ...) \n");
#endif
               // WE NEED TO IMPROVE ON THIS!
               // This is a very expensive way to get the Lhs to be set to the right size
               // we need to generate the size info more efficiently
                  redim(floatArray(Rhs));
                }

             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of floatArray & InArray<float>::operator== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // float* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             float* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             float* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             float* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             float* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             float* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             float* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             float* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             floatArray* Temp_Lhs_Array_Pointer = NULL;
          // float *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new floatArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<floatArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] = Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] = Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] = Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] = Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] = Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] = Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] = Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] = Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] = Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] = Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] = Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] = Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] = Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] = Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] = Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] = Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] = Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] = Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] = Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] = Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] = Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] = Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] = Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] = Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }



#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Leaving floatArray::operator=(const Indexable<T1, A> & Rhs) \n");
#endif

             return *this;
           }

     // The use of the inline is a problem.  It forces some strange errors.
        template <class T1, class A>
     // inline floatArray::floatArray(const Indexable<T1, A> &Rhs) : InArray<float>(this) , Array_Descriptor(Rhs.shape())
     // floatArray::floatArray(const Indexable<T1, A> &Rhs)
     // floatArray::floatArray(const Indexable<T1, A> &Rhs) : InArray<float>(this)
     // floatArray::floatArray(const Indexable<T1, A> &Rhs) : InArray<float>(this) , Array_Descriptor(Rhs.shape())
        floatArray::floatArray(const Indexable<T1, A> &Rhs) : InArray<float>(Rhs.shape())
           {
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of floatArray::floatArray(const Indexable<T1, A> &Rhs) \n");
#endif
          // The base of the new array object should be zero (or maybe the APP_Default_Base?)
          // setBase(0);
          // The base of the new array object should be zero (or maybe the APP_Default_Base?)
          // It is too complicated to reset the base if we have used indirect addressing.
             if (Rhs.shape().usesIndirectAddressing() == FALSE)
                  setBase(0);

          // Allocate array memory!
             Array_Descriptor.Array_Data = NULL;
             Allocate_Array_Data (TRUE);
             Array_Storage  = NULL;
             referenceCount = 0;
             POINTER_LIST_INITIALIZATION_MACRO;

          // Array_Descriptor.display("Inside of floatArray::floatArray(const Indexable<T1, A> &Rhs)");
             operator= (Rhs);
           }

        template <class T1, class A>
        inline floatArray & floatArray::operator+=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of floatArray & InArray<float>::operator+== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // float* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             float* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             float* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             float* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             float* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             float* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             float* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             float* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             floatArray* Temp_Lhs_Array_Pointer = NULL;
          // float *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new floatArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<floatArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] += Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] += Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] += Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] += Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] += Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] += Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] += Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] += Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] += Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] += Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] += Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] += Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] += Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] += Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] += Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] += Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] += Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] += Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] += Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] += Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] += Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] += Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] += Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] += Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline floatArray & floatArray::operator-=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of floatArray & InArray<float>::operator-== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // float* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             float* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             float* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             float* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             float* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             float* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             float* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             float* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             floatArray* Temp_Lhs_Array_Pointer = NULL;
          // float *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new floatArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<floatArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] -= Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] -= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] -= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] -= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] -= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] -= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] -= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] -= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] -= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] -= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] -= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] -= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] -= Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] -= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] -= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] -= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] -= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] -= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] -= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] -= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] -= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] -= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] -= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] -= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline floatArray & floatArray::operator*=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of floatArray & InArray<float>::operator*== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // float* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             float* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             float* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             float* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             float* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             float* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             float* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             float* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             floatArray* Temp_Lhs_Array_Pointer = NULL;
          // float *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new floatArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<floatArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] *= Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] *= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] *= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] *= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] *= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] *= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] *= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] *= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] *= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] *= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] *= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] *= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] *= Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] *= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] *= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] *= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] *= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] *= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] *= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] *= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] *= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] *= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] *= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] *= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline floatArray & floatArray::operator/=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of floatArray & InArray<float>::operator/== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // float* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             float* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             float* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             float* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             float* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             float* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             float* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             float* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             floatArray* Temp_Lhs_Array_Pointer = NULL;
          // float *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new floatArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<floatArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] /= Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] /= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] /= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] /= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] /= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] /= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] /= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] /= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] /= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] /= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] /= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] /= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] /= Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] /= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] /= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] /= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] /= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] /= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] /= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] /= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] /= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] /= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] /= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] /= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline floatArray & floatArray::operator%=(const Indexable<T1, A> &rhs)
           {
          /* Array_Indexable_MACRO(float,,%=) */
             printf ("Sorry, not implemented: floatArray::operator%= not defined in expression template version! \n");
             APP_ABORT();
             return *this;
           }

// End of if defined(HAS_MEMBER_TEMPLATES)
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif

#undef FLOATARRAY

#define INTARRAY
#if defined(USE_EXPRESSION_TEMPLATES)

#if defined(HAS_MEMBER_TEMPLATES)

#if defined(INTARRAY)
// error "In A++.C HAS_MEMBER_TEMPLATES is defined"
#endif

        template <class T1, class A>
     // inline intArray & intArray::operator=(const Indexable<T1, A> & Rhs)
        intArray & intArray::operator=(const Indexable<T1, A> & Rhs)
           {
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("TEST: Inside of intArray::operator=(const Indexable<T1, A> & Rhs) \n");
#endif

             if (Array_Descriptor.isNullArray() == TRUE)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 0)
                       printf ("Lhs is a NULL Array (redimensioning ...) \n");
#endif
               // WE NEED TO IMPROVE ON THIS!
               // This is a very expensive way to get the Lhs to be set to the right size
               // we need to generate the size info more efficiently
                  redim(intArray(Rhs));
                }

             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of intArray & InArray<int>::operator== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // int* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             int* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             int* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             int* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             int* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             int* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             int* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             int* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             intArray* Temp_Lhs_Array_Pointer = NULL;
          // int *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new intArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<intArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] = Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] = Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] = Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] = Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] = Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] = Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] = Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] = Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] = Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] = Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] = Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] = Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] = Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] = Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] = Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] = Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] = Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] = Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] = Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] = Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] = Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] = Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] = Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] = Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }



#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Leaving intArray::operator=(const Indexable<T1, A> & Rhs) \n");
#endif

             return *this;
           }

     // The use of the inline is a problem.  It forces some strange errors.
        template <class T1, class A>
     // inline intArray::intArray(const Indexable<T1, A> &Rhs) : InArray<int>(this) , Array_Descriptor(Rhs.shape())
     // intArray::intArray(const Indexable<T1, A> &Rhs)
     // intArray::intArray(const Indexable<T1, A> &Rhs) : InArray<int>(this)
     // intArray::intArray(const Indexable<T1, A> &Rhs) : InArray<int>(this) , Array_Descriptor(Rhs.shape())
        intArray::intArray(const Indexable<T1, A> &Rhs) : InArray<int>(Rhs.shape())
           {
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of intArray::intArray(const Indexable<T1, A> &Rhs) \n");
#endif
          // The base of the new array object should be zero (or maybe the APP_Default_Base?)
          // setBase(0);
          // The base of the new array object should be zero (or maybe the APP_Default_Base?)
          // It is too complicated to reset the base if we have used indirect addressing.
             if (Rhs.shape().usesIndirectAddressing() == FALSE)
                  setBase(0);

          // Allocate array memory!
             Array_Descriptor.Array_Data = NULL;
             Allocate_Array_Data (TRUE);
             Array_Storage  = NULL;
             referenceCount = 0;
             POINTER_LIST_INITIALIZATION_MACRO;

          // Array_Descriptor.display("Inside of intArray::intArray(const Indexable<T1, A> &Rhs)");
             operator= (Rhs);
           }

        template <class T1, class A>
        inline intArray & intArray::operator+=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of intArray & InArray<int>::operator+== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // int* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             int* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             int* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             int* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             int* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             int* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             int* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             int* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             intArray* Temp_Lhs_Array_Pointer = NULL;
          // int *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new intArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<intArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] += Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] += Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] += Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] += Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] += Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] += Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] += Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] += Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] += Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] += Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] += Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] += Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] += Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] += Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] += Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] += Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] += Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] += Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] += Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] += Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] += Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] += Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] += Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] += Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline intArray & intArray::operator-=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of intArray & InArray<int>::operator-== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // int* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             int* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             int* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             int* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             int* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             int* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             int* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             int* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             intArray* Temp_Lhs_Array_Pointer = NULL;
          // int *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new intArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<intArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] -= Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] -= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] -= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] -= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] -= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] -= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] -= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] -= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] -= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] -= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] -= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] -= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] -= Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] -= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] -= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] -= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] -= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] -= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] -= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] -= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] -= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] -= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] -= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] -= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline intArray & intArray::operator*=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of intArray & InArray<int>::operator*== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // int* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             int* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             int* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             int* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             int* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             int* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             int* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             int* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             intArray* Temp_Lhs_Array_Pointer = NULL;
          // int *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new intArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<intArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] *= Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] *= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] *= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] *= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] *= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] *= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] *= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] *= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] *= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] *= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] *= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] *= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] *= Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] *= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] *= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] *= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] *= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] *= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] *= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] *= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] *= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] *= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] *= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] *= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline intArray & intArray::operator/=(const Indexable<T1, A> &rhs)
           {
             #if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of intArray & InArray<int>::operator/== ( const Indexable<T1, A> & Rhs ) \n");
#endif

             // Check for loop dependence (lhs array appearing on the rhs)
          // This conditional test might be a problem for where a scalar is used on the RHS!
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 2)
                  printf ("Rhs.containsArrayID( Array_ID() ) = %s \n",
                       Rhs.containsArrayID( Array_ID() ) ? "TRUE" : "FALSE");
#endif

          // We need these so that we can force the location of the Lhs within
          // the inner loop body to be different whenever we detect a loop dependence
          // int* restrict Lhs_Array_Data          = Array_Descriptor.getDataPointer();
             int* restrict Lhs_Array_Data          = Array_Descriptor.getExpressionTemplateDataPointer();
             int* restrict Lhs_Array_View_Pointer0 = Array_Descriptor.Array_View_Pointer0;
             int* restrict Lhs_Array_View_Pointer1 = Array_Descriptor.Array_View_Pointer1;
             int* restrict Lhs_Array_View_Pointer2 = Array_Descriptor.Array_View_Pointer2;
             int* restrict Lhs_Array_View_Pointer3 = Array_Descriptor.Array_View_Pointer3;
             int* restrict Lhs_Array_View_Pointer4 = Array_Descriptor.Array_View_Pointer4;
             int* restrict Lhs_Array_View_Pointer5 = Array_Descriptor.Array_View_Pointer5;

             intArray* Temp_Lhs_Array_Pointer = NULL;
          // int *SavedLhsSpace = NULL;
          // int Old_View_Offset = 0;
             bool LhsDependenceUponRhsFound = Rhs.containsArrayID( Array_ID() );
             if ( LhsDependenceUponRhsFound == TRUE )
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 2)
                       printf ("Lhs occures as an operand on the Rhs so we have to build a temporary: Lhs.Array_ID() = %d \n",Array_ID());
#endif
               // The trick here is to accumulate the result into a temporary (so we have to build one)
               // and then assign the temporary to the true LHS in a second step.
               // We need to substitute the Lhs with a temporary and do the assignment 
               // after the Rhs is evaluated.  The current form of the Lhs is
               // for example: Array_View_Pointer0[Array_Descriptor.offset0(i)] Array_Descriptor x;
               // To do this we have to:
               // 1. save the existing pointer and build a new workspace. 
               // 2. evaluate the Rhs and save it in the temporary.
               // 3. assign the temporary to the Lhs (or copy the pointer if possible)

#if COMPILE_DEBUG_STATEMENTS
                  int i;
               // The first solution only works for stride 1 Lhs data
                  for (i=0; i < MAX_ARRAY_DIMENSION; i++)
                       APP_ASSERT (getStride(i) == 1);
#endif

               // Now allocate the storage for the tempoaray (where we will store the Rhs)
                  Temp_Lhs_Array_Pointer = new intArray;
                  Temp_Lhs_Array_Pointer->redim(*this);
             
               // Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getDataPointer();
                  Lhs_Array_Data          = Temp_Lhs_Array_Pointer->Array_Descriptor.getExpressionTemplateDataPointer();
                  Lhs_Array_View_Pointer0 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer0;
                  Lhs_Array_View_Pointer1 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer1;
                  Lhs_Array_View_Pointer2 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer2;
                  Lhs_Array_View_Pointer3 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer3;
                  Lhs_Array_View_Pointer4 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer4;
                  Lhs_Array_View_Pointer5 = Temp_Lhs_Array_Pointer->Array_Descriptor.Array_View_Pointer5;
                }


          // Later we put this with the P++ specific section below.  For now this
          // allows us to test the buildListOfOperands function.
          // Here we have to build a list of array objects that appear on the Rhs
             list<intArray*> ArrayList;
             Rhs.buildListOfArrayOperands(ArrayList);
          // printf ("Number of Array Operands = %d \n",ArrayList.size());

             #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


          // Use the loop macro to consolidate all the looping constructs
             // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = Rhs. internalNumberOfDimensions();
             int Lhs_Problem_Dimension = internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(Array_Descriptor.conformsTo(Rhs. shape()));
          // assert(Array_Descriptor.conformsTo(shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 Lhs_Array_View_Pointer0[indirectOffset_1D(i)] /= Rhs.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("indirectOffset_2D(%d) = %d \n",i,indirectOffset_2D(i));
                                      Lhs_Array_View_Pointer1[indirectOffset_2D(i)] /= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           Lhs_Array_View_Pointer2[indirectOffset_3D(i)] /= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                Lhs_Array_View_Pointer3[indirectOffset_4D(i)] /= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     Lhs_Array_View_Pointer4[indirectOffset_5D(i)] /= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          Lhs_Array_View_Pointer5[indirectOffset_6D(i)] /= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,Array_Descriptor.offset0(i));
                                 Lhs_Array_Data[Array_Descriptor.offset(i)] /= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,Array_Descriptor.offset0(i,j));
                                           Lhs_Array_Data[Array_Descriptor.offset(i,j)] /= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,Array_Descriptor.offset0(i,j,k));
                                                     Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] /= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,Array_Descriptor.offset0(i,j,k,l));
                                                               Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] /= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,Array_Descriptor.offset0(i,j,k,l,m));
                                                                         Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] /= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] /= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("usesIndirectAddressing() = %s \n",usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("Rhs. usesIndirectAddressing() = %s \n",Rhs. usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = Rhs. usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( usesIndirectAddressing() || Rhs. usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       Rhs. setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("indirectOffset_1D(%d) = %d \n",i,indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    Lhs_Array_View_Pointer0[indirectOffset_1D(i)] /= Rhs.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           Lhs_Array_View_Pointer1[indirectOffset_2D(i)] /= Rhs.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                Lhs_Array_View_Pointer2[indirectOffset_3D(i)] /= Rhs.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     Lhs_Array_View_Pointer3[indirectOffset_4D(i)] /= Rhs.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          Lhs_Array_View_Pointer4[indirectOffset_5D(i)] /= Rhs.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               Lhs_Array_View_Pointer5[indirectOffset_6D(i)] /= Rhs.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = shape().getLength(0) - 1;
                    // int Bound_0 = Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      Lhs_Array_Data[Array_Descriptor.offset(i)] /= Rhs.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = shape().getLength(1) - 1;
                         // int Bound_1 = Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                Lhs_Array_Data[Array_Descriptor.offset(i,j)] /= Rhs.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = shape().getLength(2) - 1;
                              // int Bound_2 = Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          Lhs_Array_Data[Array_Descriptor.offset(i,j,k)] /= Rhs.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = shape().getLength(3) - 1;
                                   // int Bound_3 = Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l)] /= Rhs.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = shape().getLength(4) - 1;
                                        // int Bound_4 = Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m)] /= Rhs.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = shape().getLength(5) - 1;
                                             // int Bound_5 = Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        Lhs_Array_Data[Array_Descriptor.offset(i,j,k,l,m,n)] /= Rhs.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


             // This fixes up the Lhs with the assignment of the temporary
             if ( LhsDependenceUponRhsFound == TRUE )
                {
               // Now we have to copy back the temporary data evaluated from the Rhs
               // Do the assignment (lets hope that the inlining is not a problem here)
                  *(this) = *Temp_Lhs_Array_Pointer;

               // clean up the memory used for the temporary storage of the Rhs
                  delete Temp_Lhs_Array_Pointer;
                }


             return *this;
           }
        template <class T1, class A>
        inline intArray & intArray::operator%=(const Indexable<T1, A> &rhs)
           {
          /* Array_Indexable_MACRO(int,,%=) */
             printf ("Sorry, not implemented: intArray::operator%= not defined in expression template version! \n");
             APP_ABORT();
             return *this;
           }

// End of if defined(HAS_MEMBER_TEMPLATES)
#endif

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif

#undef INTARRAY

#if defined(USE_EXPRESSION_TEMPLATES)

template<class T, class A>
// inline T sum(const Indexable<T,A> & X)
T sum(const Indexable<T,A> & X)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("TOP Inside of  sum ( const Indexable<T,A> & X ) \n");
#endif

      val = 0;
  // Later we put this with the P++ specific section below.  For now this
  // allows us to test the buildListOfOperands function.
  // Here we have to build a list of array objects that appear on the Rhs

  // Since there is only one sum that takes an expression we can't have this list
  // be of any one type of array object.  This will be simplified once we template
  // the array class but we want to do that afterward in a staged approach.
  // So we make this list a list of pointers to the common BaseArray class.
  // list<Array*> ArrayList;
     list<BaseArray*> ArrayList;

     #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


  // Use the loop macro to consolidate all the looping constructs
     // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = X.internalNumberOfDimensions();
             int Lhs_Problem_Dimension = X.internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(X.Array_Descriptor.conformsTo(X.shape()));
          // assert(X.Array_Descriptor.conformsTo(X.shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = X.usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = X.usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       X.setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       X.setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("X.indirectOffset_1D(%d) = %d \n",i,X.indirectOffset_1D(i));
                                 val += X.indirect_expand_1D(i);
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("X.indirectOffset_2D(%d) = %d \n",i,X.indirectOffset_2D(i));
                                      val += X.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           val += X.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                val += X.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     val += X.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          val += X.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,X.Array_Descriptor.offset0(i));
                                 val += X.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = X.shape().getLength(1) - 1;
                         // int Bound_1 = X.Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,X.Array_Descriptor.offset0(i,j));
                                           val += X.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = X.shape().getLength(2) - 1;
                              // int Bound_2 = X.Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,X.Array_Descriptor.offset0(i,j,k));
                                                     val += X.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = X.shape().getLength(3) - 1;
                                   // int Bound_3 = X.Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,X.Array_Descriptor.offset0(i,j,k,l));
                                                               val += X.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = X.shape().getLength(4) - 1;
                                        // int Bound_4 = X.Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,X.Array_Descriptor.offset0(i,j,k,l,m));
                                                                         val += X.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = X.shape().getLength(5) - 1;
                                             // int Bound_5 = X.Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,X.Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   val += X.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = X.usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = X.usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( X.usesIndirectAddressing() || X.usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       X.setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       X.setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("X.indirectOffset_1D(%d) = %d \n",i,X.indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    val += X.indirect_expand_1D(i);
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           val += X.indirect_expand_2D(i);
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                val += X.indirect_expand_3D(i);
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     val += X.indirect_expand_4D(i);
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          val += X.indirect_expand_5D(i);
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               val += X.indirect_expand_6D(i);
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      val += X.expand(i);
                               }
                          }
                         else
                          {
                            int Bound_1 = X.shape().getLength(1) - 1;
                         // int Bound_1 = X.Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                val += X.expand(i,j);
                                         }
                               }
                              else
                               {
                                 int Bound_2 = X.shape().getLength(2) - 1;
                              // int Bound_2 = X.Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          val += X.expand(i,j,k);
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = X.shape().getLength(3) - 1;
                                   // int Bound_3 = X.Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    val += X.expand(i,j,k,l);
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = X.shape().getLength(4) - 1;
                                        // int Bound_4 = X.Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              val += X.expand(i,j,k,l,m);
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = X.shape().getLength(5) - 1;
                                             // int Bound_5 = X.Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        val += X.expand(i,j,k,l,m,n);
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


     return val;
   }
 
#include <float.h>
template<class T, class A>
// inline T min (const Indexable<T,A> & X)
T min(const Indexable<T,A> & X)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("TOP Inside of T min ( const Indexable<T,A> & X ) \n");
#endif

#define MAXIMUM_VALUE INT_MAX
     T val = MAXIMUM_VALUE;

  // Later we put this with the P++ specific section below.  For now this
  // allows us to test the buildListOfOperands function.
  // Here we have to build a list of array objects that appear on the Rhs
     list<BaseArray*> ArrayList;

     #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


  // Use the loop macro to consolidate all the looping constructs
     // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = X.internalNumberOfDimensions();
             int Lhs_Problem_Dimension = X.internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(X.Array_Descriptor.conformsTo(X.shape()));
          // assert(X.Array_Descriptor.conformsTo(X.shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = X.usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = X.usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       X.setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       X.setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("X.indirectOffset_1D(%d) = %d \n",i,X.indirectOffset_1D(i));
                                 val = (X.indirect_expand_1D(i) < val) ? X.indirect_expand_1D(i) : val;
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("X.indirectOffset_2D(%d) = %d \n",i,X.indirectOffset_2D(i));
                                      val = (X.indirect_expand_2D(i) < val) ? X.indirect_expand_2D(i) : val;
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           val = (X.indirect_expand_3D(i) < val) ? X.indirect_expand_3D(i) : val;
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                val = (X.indirect_expand_4D(i) < val) ? X.indirect_expand_4D(i) : val;
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     val = (X.indirect_expand_5D(i) < val) ? X.indirect_expand_5D(i) : val;
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          val = (X.indirect_expand_6D(i) < val) ? X.indirect_expand_6D(i) : val;
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,X.Array_Descriptor.offset0(i));
                                 val = (X.expand(i)     < val) ? X.expand(i) : val;
                               }
                          }
                         else
                          {
                            int Bound_1 = X.shape().getLength(1) - 1;
                         // int Bound_1 = X.Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,X.Array_Descriptor.offset0(i,j));
                                           val = (X.expand(i,j)   < val) ? X.expand(i,j) : val;
                                         }
                               }
                              else
                               {
                                 int Bound_2 = X.shape().getLength(2) - 1;
                              // int Bound_2 = X.Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,X.Array_Descriptor.offset0(i,j,k));
                                                     val = (X.expand(i,j,k) < val) ? X.expand(i,j,k) : val;
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = X.shape().getLength(3) - 1;
                                   // int Bound_3 = X.Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,X.Array_Descriptor.offset0(i,j,k,l));
                                                               val = (X.expand(i,j,k,l) < val) ? X.expand(i,j,k,l) : val;
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = X.shape().getLength(4) - 1;
                                        // int Bound_4 = X.Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,X.Array_Descriptor.offset0(i,j,k,l,m));
                                                                         val = (X.expand(i,j,k,l,m) < val) ? X.expand(i,j,k,l,m) : val;
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = X.shape().getLength(5) - 1;
                                             // int Bound_5 = X.Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,X.Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   val = (X.expand(i,j,k,l,m,n) < val) ? X.expand(i,j,k,l,m,n) : val;
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = X.usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = X.usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( X.usesIndirectAddressing() || X.usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       X.setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       X.setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("X.indirectOffset_1D(%d) = %d \n",i,X.indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    val = (X.indirect_expand_1D(i) < val) ? X.indirect_expand_1D(i) : val;
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           val = (X.indirect_expand_2D(i) < val) ? X.indirect_expand_2D(i) : val;
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                val = (X.indirect_expand_3D(i) < val) ? X.indirect_expand_3D(i) : val;
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     val = (X.indirect_expand_4D(i) < val) ? X.indirect_expand_4D(i) : val;
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          val = (X.indirect_expand_5D(i) < val) ? X.indirect_expand_5D(i) : val;
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               val = (X.indirect_expand_6D(i) < val) ? X.indirect_expand_6D(i) : val;
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      val = (X.expand(i)     < val) ? X.expand(i) : val;
                               }
                          }
                         else
                          {
                            int Bound_1 = X.shape().getLength(1) - 1;
                         // int Bound_1 = X.Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                val = (X.expand(i,j)   < val) ? X.expand(i,j) : val;
                                         }
                               }
                              else
                               {
                                 int Bound_2 = X.shape().getLength(2) - 1;
                              // int Bound_2 = X.Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          val = (X.expand(i,j,k) < val) ? X.expand(i,j,k) : val;
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = X.shape().getLength(3) - 1;
                                   // int Bound_3 = X.Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    val = (X.expand(i,j,k,l) < val) ? X.expand(i,j,k,l) : val;
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = X.shape().getLength(4) - 1;
                                        // int Bound_4 = X.Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              val = (X.expand(i,j,k,l,m) < val) ? X.expand(i,j,k,l,m) : val;
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = X.shape().getLength(5) - 1;
                                             // int Bound_5 = X.Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        val = (X.expand(i,j,k,l,m,n) < val) ? X.expand(i,j,k,l,m,n) : val;
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


     return val; 
   }

#include <float.h>
template<class T, class A>
// inline T max(const Indexable<T,A> & X)
T max(const Indexable<T,A> & X)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("TOP Inside of T max ( const Indexable<T,A> & X ) \n");
#endif

#define MINIMUM_VALUE -INT_MAX
     T val = MINIMUM_VALUE;

  // Later we put this with the P++ specific section below.  For now this
  // allows us to test the buildListOfOperands function.
  // Here we have to build a list of array objects that appear on the Rhs
     list<BaseArray*> ArrayList;

     #if defined(PPP)
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
                  Parallel_Conformability_Enforcement (*this,ArrayList,LhsSerialArrayPointer,RhsSerialArrayPointer);

                  printf ("ERROR: Need to look at this case more before executing it! \n");
                  APP_ABORT();
                  intArray *SerialWhereMaskPointer = NULL;
                  Parallel_Conformability_Enforcement 
                       (*this,Rhs,*Where_Statement_Support::Where_Statement_Mask,
                        LhsSerialArrayPointer,RhsSerialArrayPointer,SerialWhereMaskPointer);
                }
#endif


  // Use the loop macro to consolidate all the looping constructs
     // internalNumberOfDimensions does not just return 1 for indirect addressing
             int Rhs_Problem_Dimension = X.internalNumberOfDimensions();
             int Lhs_Problem_Dimension = X.internalNumberOfDimensions();
             int Problem_Dimension = (Lhs_Problem_Dimension > Rhs_Problem_Dimension) ? Lhs_Problem_Dimension : Rhs_Problem_Dimension;

          // Allow for additional operands to contribute to the Problem_Dimension (useful for the replace function)
             

#if COMPILE_DEBUG_STATEMENTS
             APP_ASSERT (Problem_Dimension >= 0);
             if (APP_DEBUG > 1)
                  printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

          // Check conformability
          // assert(X.Array_Descriptor.conformsTo(X.shape()));
          // assert(X.Array_Descriptor.conformsTo(X.shape()));

          // The NULL Array has dimension of ZERO.  Operations on such arrays have no effect.
             if (Problem_Dimension > 0)
                {
             if (Where_Statement_Support::Where_Statement_Mask == NULL)
                {
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("NON-WHERE statement \n");
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
#endif

                  Boolean Lhs_usesIndirectAddressing = X.usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = X.usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
               // One dimensional array can be alternatively treated as using indirect addressing
               // so this is a good test of the indirect addressing and the support for mixed operations
               // (i.e. array statements that mix together direct and indirect addressing between different operands)
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing expression template support in use. \n");
#endif

                    // printf ("Indirect Addressing expression template support not implemented yet! \n");
                    // APP_ABORT();

                    // One problem is that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // 1. The first step is to recognize when a mixed operation is occuring
                    // 2. Then isolate what type of mixed operation.
                    //    a. indirect adressed and non-indirect addressed operands.
                    //    b. indirect addressing with operands using intArrays - Range objects and scalars.
                    // 3. Convert all non-indirect addressing to indirect addressing
                    // 4. Convert all indirect addressing using Range objects to use an intArray
                    //    a. For Range objects use an existing sequentially order integer array
                    //    b. For scalars use the same array as above but use a stride of ZERO (see if this works!!!)

                    // int Dimension = X.shape().numberOfDimensions();
                    // int Bound_0 = X.shape().Bound[0] - X.shape().Base[0];

                    // Setup the Lhs
                       X.setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       X.setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // if (APP_DEBUG > 1)
                              //      printf ("X.indirectOffset_1D(%d) = %d \n",i,X.indirectOffset_1D(i));
                                 val = (X.indirect_expand_1D(i) > val) ? X.indirect_expand_1D(i) : val;
                               }
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                   // if (APP_DEBUG > 1)
                                   //      printf ("X.indirectOffset_2D(%d) = %d \n",i,X.indirectOffset_2D(i));
                                      val = (X.indirect_expand_2D(i) > val) ? X.indirect_expand_2D(i) : val;
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           val = (X.indirect_expand_3D(i) > val) ? X.indirect_expand_3D(i) : val;
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                val = (X.indirect_expand_4D(i) > val) ? X.indirect_expand_4D(i) : val;
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     val = (X.indirect_expand_5D(i) > val) ? X.indirect_expand_5D(i) : val;
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          val = (X.indirect_expand_6D(i) > val) ? X.indirect_expand_6D(i) : val;
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                    // printf ("Bound_0 = %d \n",Bound_0);
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("index_value (i=%d) = %d \n",i,X.Array_Descriptor.offset0(i));
                                 val = (X.expand(i)     > val) ? X.expand(i) : val;
                               }
                          }
                         else
                          {
                            int Bound_1 = X.shape().getLength(1) - 1;
                         // int Bound_1 = X.Array_Descriptor.getLength(1) - 1;
                         // printf ("Bound_1 = %d \n",Bound_1);
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                        // printf ("index_value (i=%d,j=%d) = %d \n",i,j,X.Array_Descriptor.offset0(i,j));
                                           val = (X.expand(i,j)   > val) ? X.expand(i,j) : val;
                                         }
                               }
                              else
                               {
                                 int Bound_2 = X.shape().getLength(2) - 1;
                              // int Bound_2 = X.Array_Descriptor.getLength(2) - 1;
                              // printf ("Bound_2 = %d \n",Bound_2);
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                  // printf ("index_value (i=%d,j=%d,k=%d) = %d \n",i,j,k,X.Array_Descriptor.offset0(i,j,k));
                                                     val = (X.expand(i,j,k) > val) ? X.expand(i,j,k) : val;
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = X.shape().getLength(3) - 1;
                                   // int Bound_3 = X.Array_Descriptor.getLength(3) - 1;
                                   // printf ("Bound_3 = %d \n",Bound_3);
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                            // printf ("index_value (i=%d,j=%d,k=%d,l=%d) = %d \n",i,j,k,l,X.Array_Descriptor.offset0(i,j,k,l));
                                                               val = (X.expand(i,j,k,l) > val) ? X.expand(i,j,k,l) : val;
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = X.shape().getLength(4) - 1;
                                        // int Bound_4 = X.Array_Descriptor.getLength(4) - 1;
                                        // printf ("Bound_4 = %d \n",Bound_4);
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                      // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d) = %d \n",i,j,k,l,m,X.Array_Descriptor.offset0(i,j,k,l,m));
                                                                         val = (X.expand(i,j,k,l,m) > val) ? X.expand(i,j,k,l,m) : val;
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = X.shape().getLength(5) - 1;
                                             // int Bound_5 = X.Array_Descriptor.getLength(5) - 1;
                                             // printf ("Bound_5 = %d \n",Bound_5);
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                // printf ("index_value (i=%d,j=%d,k=%d,l=%d,m=%d,n=%d) = %d \n",i,j,k,l,m,n,X.Array_Descriptor.offset0(i,j,k,l,m,n));
                                                                                   val = (X.expand(i,j,k,l,m,n) > val) ? X.expand(i,j,k,l,m,n) : val;
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
               else
                {
               // Handle Where Statement
#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Case of WHERE statement \n");
#endif
                  intArray & WhereMask = *Where_Statement_Support::Where_Statement_Mask;
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
               // printf ("X.usesIndirectAddressing() = %s \n",X.usesIndirectAddressing() ? "TRUE" : "FALSE");
               // APP_ABORT();

                  int WhereMask_Problem_Dimension = WhereMask.internalNumberOfDimensions();
                  Problem_Dimension = (WhereMask_Problem_Dimension > Problem_Dimension) ? WhereMask_Problem_Dimension : Problem_Dimension;

#if COMPILE_DEBUG_STATEMENTS
                  if (APP_DEBUG > 1)
                       printf ("Problem_Dimension = %d \n",Problem_Dimension);
#endif

                  Boolean Lhs_usesIndirectAddressing = X.usesIndirectAddressing();
                  Boolean Rhs_usesIndirectAddressing = X.usesIndirectAddressing();
               // Allow for extra operand (useful for the replace function)
                  Boolean Extra_Operand = FALSE;

#if 0
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand || (Problem_Dimension == 1) )
#else
                  if (Lhs_usesIndirectAddressing || Rhs_usesIndirectAddressing || Extra_Operand )
#endif
                  APP_ASSERT (WhereMask.usesIndirectAddressing() == FALSE);
                  if ( X.usesIndirectAddressing() || X.usesIndirectAddressing() || WhereMask.usesIndirectAddressing() )
                     {
#if COMPILE_DEBUG_STATEMENTS
                       if (APP_DEBUG > 1)
                            printf ("Indirect Addressing/Where expression template support in use. \n");
#endif

                    // One problem this that intArrays used for indirect addressing can be mixed
                    // with Range objects. This makes it more complex to generalize the indexing.

                    // Setup the Lhs
                       X.setupIndirectAddressingSupport();

                    // Setup the Rhs (all the operands if it is an expression)
                       X.setupIndirectAddressingSupport();

                    // Allow for an extra operand (useful for the replace function)
                       

                    // Setup the Where Mask
                       WhereMask.setupIndirectAddressingSupport();

                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       if ( Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                              // printf ("X.indirectOffset_1D(%d) = %d \n",i,X.indirectOffset_1D(i));
                                 if (WhereMask.Array_Descriptor.Array_View_Pointer0[WhereMask.indirectOffset_1D(i)])
                                    val = (X.indirect_expand_1D(i) > val) ? X.indirect_expand_1D(i) : val;
                               }
                            
                          }
                         else
                          {
                            if ( Problem_Dimension == 2)
                               {
                                 for (int i = 0; i <= Bound_0; i++)
                                    {
                                      if (WhereMask.Array_Descriptor.Array_View_Pointer1[WhereMask.indirectOffset_2D(i)]) 
                                           val = (X.indirect_expand_2D(i) > val) ? X.indirect_expand_2D(i) : val;
                                    }
                               }
                              else
                               {
                                 if ( Problem_Dimension == 3)
                                    {
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.Array_Descriptor.Array_View_Pointer2[WhereMask.indirectOffset_3D(i)]) 
                                                val = (X.indirect_expand_3D(i) > val) ? X.indirect_expand_3D(i) : val;
                                         }
                                    }
                                   else
                                    {
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int i = 0; i <= Bound_0; i++)
                                              {
                                                if (WhereMask.Array_Descriptor.Array_View_Pointer3[WhereMask.indirectOffset_4D(i)]) 
                                                     val = (X.indirect_expand_4D(i) > val) ? X.indirect_expand_4D(i) : val;
                                              }
                                         }
                                        else
                                         {
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.Array_Descriptor.Array_View_Pointer4[WhereMask.indirectOffset_5D(i)]) 
                                                          val = (X.indirect_expand_5D(i) > val) ? X.indirect_expand_5D(i) : val;
                                                   }
                                              }
                                             else
                                              {
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int i = 0; i <= Bound_0; i++)
                                                        {
                                                          if (WhereMask.Array_Descriptor.Array_View_Pointer5[WhereMask.indirectOffset_6D(i)]) 
                                                               val = (X.indirect_expand_6D(i) > val) ? X.indirect_expand_6D(i) : val;
                                                        }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                    else
                     {
                       int Bound_0 = X.shape().getLength(0) - 1;
                    // int Bound_0 = X.Array_Descriptor.getLength(0) - 1;
                       if (Problem_Dimension == 1)
                          {
                            for (int i = 0; i <= Bound_0; i++)
                               {
                                 if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i)]) 
                                      val = (X.expand(i)     > val) ? X.expand(i) : val;
                               }
                          }
                         else
                          {
                            int Bound_1 = X.shape().getLength(1) - 1;
                         // int Bound_1 = X.Array_Descriptor.getLength(1) - 1;
                            if (Problem_Dimension == 2)
                               {
                                 for (int j = 0; j <= Bound_1; j++)
                                      for (int i = 0; i <= Bound_0; i++)
                                         {
                                           if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j)]) 
                                                val = (X.expand(i,j)   > val) ? X.expand(i,j) : val;
                                         }
                               }
                              else
                               {
                                 int Bound_2 = X.shape().getLength(2) - 1;
                              // int Bound_2 = X.Array_Descriptor.getLength(2) - 1;
                                 if (Problem_Dimension == 3)
                                    {
                                      for (int k = 0; k <= Bound_2; k++)
                                           for (int j = 0; j <= Bound_1; j++)
                                                for (int i = 0; i <= Bound_0; i++)
                                                   {
                                                     if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k)]) 
                                                          val = (X.expand(i,j,k) > val) ? X.expand(i,j,k) : val;
                                                   }
                                    }
                                   else
                                    {
                                      int Bound_3 = X.shape().getLength(3) - 1;
                                   // int Bound_3 = X.Array_Descriptor.getLength(3) - 1;
                                      if ( Problem_Dimension == 4)
                                         {
                                           for (int l = 0; l <= Bound_3; l++)
                                                for (int k = 0; k <= Bound_2; k++)
                                                     for (int j = 0; j <= Bound_1; j++)
                                                          for (int i = 0; i <= Bound_0; i++)
                                                             {
                                                               if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l)]) 
                                                                    val = (X.expand(i,j,k,l) > val) ? X.expand(i,j,k,l) : val;
                                                             }
                                         }
                                        else
                                         {
                                           int Bound_4 = X.shape().getLength(4) - 1;
                                        // int Bound_4 = X.Array_Descriptor.getLength(4) - 1;
                                           if ( Problem_Dimension == 5)
                                              {
                                                for (int m = 0; m <= Bound_4; m++)
                                                     for (int l = 0; l <= Bound_3; l++)
                                                          for (int k = 0; k <= Bound_2; k++)
                                                               for (int j = 0; j <= Bound_1; j++)
                                                                    for (int i = 0; i <= Bound_0; i++)
                                                                       {
                                                                         if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m)]) 
                                                                              val = (X.expand(i,j,k,l,m) > val) ? X.expand(i,j,k,l,m) : val;
                                                                       }
                                              }
                                             else
                                              {
                                                int Bound_5 = X.shape().getLength(5) - 1;
                                             // int Bound_5 = X.Array_Descriptor.getLength(5) - 1;
                                                if ( Problem_Dimension == 6)
                                                   {
                                                     for (int n = 0; n <= Bound_5; n++)
                                                          for (int m = 0; m <= Bound_4; m++)
                                                               for (int l = 0; l <= Bound_3; l++)
                                                                    for (int k = 0; k <= Bound_2; k++)
                                                                         for (int j = 0; j <= Bound_1; j++)
                                                                              for (int i = 0; i <= Bound_0; i++)
                                                                                 {
                                                                                   if (WhereMask.getExpressionTemplateDataPointer()[WhereMask.Array_Descriptor.offset(i,j,k,l,m,n)]) 
                                                                                        val = (X.expand(i,j,k,l,m,n) > val) ? X.expand(i,j,k,l,m,n) : val;
                                                                                 }
                                                   }
                                                  else
                                                   {
                                                     printf ("ERROR: Problem_Dimension > 6 --- Problem_Dimension = %d \n",
                                                          Problem_Dimension);
                                                     APP_ABORT();
                                                   }
                                              }
                                         }
                                    }
                               }
                          }
                     }
                }
                }


     return val; 
   }

// End of if defined(USE_EXPRESSION_TEMPLATES)
#endif


/* MACRO EXPANSION ENDS HERE */

#endif /* defined(LOCATE_EXPRESSION_TEMPLATES_IN_HEADER_FILES) */

// Comment this out for now!
// define inline

#endif  /* !defined(_APP_INLINE_EXPRESSION_TEMPLATES_H) */




// Note that the use of the inline allows us to build
// this code either as a header file or as a source file
// templating within C++ compilers sometimes requires one or the other
// plus the use of the code in this file in a source file make the
// compilation process more efficient because it breaks up the size
// of the resulting application source.
/* define(INLINE_MACRO,inline) */
/* define(INLINE_MACRO, ) */







/* Use a MACRO to consolidate all the looping constructs */




 





 



























