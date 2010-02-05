// DQ (3/20/2005): Added to support non-standard feature used in KULL (roughly supported in both EDG 
// and GNU g++, but with some differences).  This fixup modifies the AST so that it can be output
// code that will compile with GNU g++.  I hate the idea of doing thing, but at least the fixup
// is being handled here rather than allowing the unparser to modifiy the AST.
/*! \brief Handles not standard in-class initialization used in EDG and GNU g++ (but supported differently)
 */
#ifndef __fixupClassData
#define __fixupClassData
void fixupInClassDataInitialization( SgNode* node );

/*! \brief Modified const double variable initialized in-class to be static const double with initializer.

    \internal This is a non-standard feature which g++ supports different than g++.

 */
class FixupInClassDataInitialization : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };

#endif

