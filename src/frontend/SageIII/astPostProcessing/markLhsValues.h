#ifndef MARK_LHS_VALUES_H
#define MARK_LHS_VALUES_H

// DQ (1/19/2008):
/*! \brief Mark relavant expressions as l-values explicitly.

    \internal Nodes of specific operators are l-values and it is less clear in other cases.
 */
ROSE_DLL_API void markLhsValues( SgNode* node );

/*! \brief Explicitly mark lhs operands of specific operators as lvalues.

    \internal Nodes of specific operators are l-values and it is less clear in other cases.
 */
class MarkLhsValues : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };

// endif for MARK_LHS_VALUES_H
#endif
