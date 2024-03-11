#ifndef ROSE_sageCopy_H
#define ROSE_sageCopy_H

#include <map>
#include <vector>

/*! \brief Supporting class from copy mechanism within ROSE.

    This class forms a base class for the SgShallowCopy and SgTreeCopy
    classes to support the control of copying of subtrees (what IR nodes
    are copied by a deep or shallow copy).

    To tailor a specific type of copy operation derive a class from this
    abstract class and define the clone_node function as required.
    context information if required muyst be obtained through local
    analysis inside the function using the parent or scope information
    at each IR node.

    \note This is an abstract class.

    \internal Used in ROSETTA generated copy functions on each IR node.
              The location of this code should perhaps be changed to be in the
              generate copy code rather than Node.code in ROSETTA/Grammar.
 */
class SgCopyHelp
   {
     public:
      // STL map type
         typedef std::map<const SgNode*,SgNode*> copiedNodeMapType;

     private:
      // DQ (10/8/2007): Added support for the depth to be kept track of in the AST copy mechansim.
     //! This data member records the depth of the traversal (root node == 0)
         int depth;

      // This is mostly used for SgSymbol IR nodes (but will likely be used for other IR nodes in the future.
     //! This is internal state used to asscociate pairs of IR nodes copied instead of shared.
         copiedNodeMapType copiedNodeMap;

     public:

      // STL map iterator type
         typedef copiedNodeMapType::iterator copiedNodeMapTypeIterator;

          virtual SgNode *copyAst( const SgNode *n ) = 0;

       // Default constructor (required to initialize the depth
          SgCopyHelp() : depth(0) {}

       // Defined the virtual destructor
          virtual ~SgCopyHelp() {}

       // DQ (10/8/2007): Access functions for the depth (inlined for performance).
          int get_depth() { return depth; }
          void incrementDepth() { depth++; }
          void decrementDepth() { depth--; }
          copiedNodeMapType & get_copiedNodeMap() { return copiedNodeMap; }

       // Reset the state saved
          void clearState() { depth = 0; copiedNodeMap.clear(); }

       // Added things that generate symbols to the state (could be SgDeclarationStatement or SgInitializedName objects).
          void insertCopiedNodePair( const SgNode* key, SgNode* value );

   };


/*! \brief Supporting class for "Shallow" copies of the AST.
    This class supports only shallow copies of the AST.
 */
class SgShallowCopy : public SgCopyHelp
   {
     public:
          SgNode *copyAst( const SgNode *n ) { return const_cast<SgNode *>(n); }
   };


/*! \brief Supporting class for "Deep" copies of the AST.

    This class supports only deep copies of the AST.

\internal The name of this class should perhaps be changed to reflect that it is a "deep" copy.

 */
class ROSE_DLL_API SgTreeCopy : public SgCopyHelp
   {
     public:
          SgNode *copyAst( const SgNode *n );
   };

// PC (8/3/2006): Support for subnode capturing
/*! \brief Supporting template class for "capturing" copies of the AST.

This class allows for retrieval of the clone of arbitrary source nodes
underneath the copied node.

*/
template <class CopyType>
class SgCapturingCopy : public CopyType
   {
     protected:
          std::vector<SgNode *> origList;
          std::vector<SgNode *> copyList;
     public:
          SgCapturingCopy( std::vector<SgNode *> origList, const CopyType &base = CopyType() )
          // : origList(origList), CopyType(base)
             :  CopyType(base), origList(origList), copyList(origList.size())
             { }
          virtual ~SgCapturingCopy() { }
          SgNode *copyAst( const SgNode *n )
             {
               SgNode *newNode = CopyType::copyAst(n);
               int listSize = origList.size();
               for (int i = 0; i < listSize; i++)
                  {
                    if (origList[i] == n)
                       {
                         copyList[i] = newNode;
                       }
                  }
               return newNode;
             }
          const std::vector<SgNode *> &get_copyList()
             {
               return copyList;
             }
   };

#endif
