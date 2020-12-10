// Author: Markus Schordan
// $Id: AstStatistics.h,v 1.3 2008/01/08 02:56:38 dquinlan Exp $

#ifndef AST_STATISTICS_H
#define AST_STATISTICS_H

#include <string>
#include <iomanip>
#include "AstProcessing.h"

/*! \brief This is a mechanism for reporting statistical data about the AST, subtrees, 
           and IR nodes.

    This is where interesting (hopefully relevant) information about the AST 
    may be data-mined and reported).  Such information includes:
    -#) the total number of nodes,
    -#) the number of nodes organized by IR node type
    -#) ...

 */

// This function causes output to cout and as a result should not be called
// when using the options that are sensative to output (-E, -H, etc.).
// Unfortunately this is not a great design and we should have a policy about
// output from the compiler tools built by ROSE.

// Future Design:
//   1) Different levels of output
//   2) External file output (for more complete statistics)
//   3) A mechanism to report on hotspots, performance data, etc. ???
//   4) Number of functions and the length of each function
//   5) Complexity (numerous software metrics) ???

namespace ROSE_Statistics
{
  class AstNodeTraversalStatistics : public SgSimpleProcessing {
  public:
    typedef unsigned int ElementType;
    typedef std::vector<ElementType> StatisticsContainerType;
    
    AstNodeTraversalStatistics();
    virtual ~AstNodeTraversalStatistics();
    virtual std::string toString(SgNode* node);
    
  protected:
    virtual void visit(SgNode* node);
    std::string singleStatistics();
    std::string cmpStatistics(AstNodeTraversalStatistics& q);
    std::string generateCMPStatisticsValueString(std::string name, ElementType v1, ElementType v2);
    StatisticsContainerType getStatisticsData();
    StatisticsContainerType numNodeTypes; // MS 2020-02-11: changed data representation
  
  private:
  };
 
// MS 2020: ROSE-2529
 class AstNodeTraversalCSVStatistics : public AstNodeTraversalStatistics {
 public:
   AstNodeTraversalCSVStatistics();

   // generate CSV format for each entry (2 columns): <AST Node Name>, <Node Count>
   virtual std::string toString(SgNode* node) ROSE_OVERRIDE;

   // set minimum node count to show in CSV file (default is 1)
   // example: when setting it to 0, all entries (including those with count 0) are shown.
   void setMinCountToShow(int minValue);
   unsigned int getMinCountToShow();

 private:
   unsigned int minCountToShow;
 };

#if 0
class AstNodeMemoryPoolStatistics : public ROSE_VisitTraversal
   {
     public:
          int counter;
          void visit ( SgNode* node);

          AstNodeMemoryPoolStatistics() : counter(0) {}
   };
#endif

class AstNodeMemoryPoolStatistics : public ROSE_VisitTraversal
   {
     public:
          class ElementType
             {
               public:
                    std::string name;
                    int count;
                    int memoryRequirement;

                    ElementType(std::string name, int count, int memoryRequirement);

                    bool operator<(const ElementType & x);
             };

          typedef std::vector<ElementType> StatisticsContainerType;

          int counter;

       // DQ (9/1/2009): Added support to computer percentage of total memory for each IR node
          int totalMemoryUsed; // = SgNode::memoryUsage();

          AstNodeMemoryPoolStatistics();
          virtual ~AstNodeMemoryPoolStatistics();
       // virtual std::string toString(SgNode* node); // to become obsolete

     protected:
          void visit(SgNode* node);
          StatisticsContainerType getStatisticsData();

     private:
          StatisticsContainerType numNodeTypes;
   };

// end of "ROSE_Statistics" namespace
}

/*! \brief Statistics about IR nodes used.

    This class contains a number of static functions which can be called (return strings) 
    to report on the fequency, memory usage, etc. of IR nodes.

    \internal These functions are implemented using a range of traversals supported by ROSE for either 
    traversing a specified subtree of the AST, the memory pools using to hold the AST, or traversals
    of representative IR nodes (visits one type or IR node only in the AST).
 */
class ROSE_DLL_API AstNodeStatistics
   {
     public:
     //! This outputs the types and count of IR nodes used in a traversal of the input file (excluding header files)
         static std::string traversalStatistics( SgNode* node );

     //! This outputs the types, counts, and memory useage of IR nodes appearing in the memory pools (whole AST).
         static std::string IRnodeUsageStatistics();
   };

#endif

