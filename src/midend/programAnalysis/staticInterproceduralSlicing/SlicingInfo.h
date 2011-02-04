#ifndef _SLICING_INFO_H_
#define _SLICING_INFO_H_

#include <set>
#include <string>
#include <list>
/* ! \class SlicingInfo

   This class scans throught the AST for two different pragmas: First, a
   pragma calls "SliceFunction" which indicates that the immediately
   proceeding function is the function that the slicing algorithm should
   target. Second, it finds the statement immediately proceeding the pragma
   "SliceTarget" for use as the slicing criterion.

   Each of these pragmas are assumed to appear only once in the file.

   @todo Allow slicing criterion to be a set of statements rather than an
   individual statement.

 */

class SlicingInfo:public AstSimpleProcessing
{

  public:

    SlicingInfo();
                // manually add a node to the slicing set
                void addNode(SgNode * sgNode)
                {       
                        targets.push_back(sgNode);      
                }
                void setSliceTargetString(std::string target)
                {
                        sliceStatement=target;  
                };

    // ! Returns the SgFunctionDeclaration that we are targeting
    SgFunctionDeclaration *getTargetFunction()
    {
        return _func;
    };

    // ! Returns the statements that are part of the slicing criterion
    SgNode *getSlicingCriterion()
    {
        return _target;
    };

    std::list < SgNode * >getSlicingTargets()
    {
        return targets;
    };

  protected:

    //std::list < SgStatement * >targets;
    std::list < SgNode * >targets;
    virtual void visit(SgNode * node);

    // ! The target function which is to be sliced.
    SgFunctionDeclaration *_func;

    // ! The slicing criterion.
    SgNode *_target;

    /* ! \brief true when we need to mark the target function

       This is set to true when we see the pragma "SliceFunction." Once we
       find the next function declaration, we assign it to _func and set this
       to false again. */
    bool _markFunction;

    /* ! \brief true when we need to mark the slicing criterion

       This is set to true when we see the pragma "SliceTarget." Once we find
       the next SgStatement, we assign it to _target and set this to false
       again. */
    bool _markStatement;

                
                // string for pragma to identify for slicing for functions calls of given function
    std::string sliceFunctionCalls;
                // string for pragma to slice for the following statement
    std::string sliceStatement;
};


#endif
