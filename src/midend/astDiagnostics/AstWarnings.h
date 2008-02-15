// Author: Dan Quinlan
// $Id: AstWarnings.h,v 1.2 2006/04/24 00:21:31 dquinlan Exp $

#ifndef AST_WARNINGS_H
#define AST_WARNINGS_H

#include <string>
#include <iomanip>
#include "AstProcessing.h"
#include "AstNodePtrs.h"

/*! \brief This is a mechanism for reporting warnings about the AST, subtrees, 
           and IR nodes.

    Possible problems with the AST which constitute concerns but not fatal errors are reported.
    Such information includes: \n
    -#) shared nodes,
    -#) nodes with default file source position imformation
    -#) ...

 */

// This function causes output to cout and as a result should not be called
// when using the options that are sensative to output (-E, -H, etc.).

// Future Design:
//   1) Different levels of output
//   2) External file output (for more complete statistics)
//   3) A mechanism to report on hotspots, performance data, etc. ???

class AstWarnings : public SgSimpleProcessing
   {
     public:
          typedef SgNode* ElementType;
          typedef std::vector<ElementType> ElementListType;

          AstWarnings();
         ~AstWarnings();

     protected:
          virtual void visit(SgNode* node);
          std::string outputWarnings();
//        string cmpStatistics(AstWarnings & q);
//        string generateCMPStatisticsValueString(string name, ElementType v1, ElementType v2);
//        StatisticsContainerType getStatisticsData();

     private:
//        StatisticsContainerType& numNodeTypes;
   };

#endif

