#ifndef WHOLE_AST_H
#define WHOLE_AST_H

#include "AstDOTGeneration.h"
#include <string>
#include <vector>

class CustomAstDOTGenerationEdgeType
   {
  // EdgeType holdes information required to specify an edge to DOT

     public:
          SgNode* start;
          SgNode* end;

       // DQ (3/5/2007): Added label string (mostly for symmetry with CustomAstDOTGenerationNodeType)
          std::string labelString;

          std::string optionString;

       // DQ (3/5/2007): Need to set the labelString, but not yet.
          CustomAstDOTGenerationEdgeType (SgNode* n1, SgNode* n2, std::string label = "", std::string options = "")
             : start(n1), end(n2), labelString(label), optionString(options) {}

       // DQ (3/6/2007): Modified to only focus on the start and end nodes, actually we need the label test
       // These are implemented to support the STL find function (STL algorithm)
       // bool operator!=(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start != start) || (edge.end != end) || (edge.optionString != optionString); }
       // bool operator==(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start == start) && (edge.end == end) && (edge.optionString == optionString); }
       // bool operator!=(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start != start) || (edge.end != end); }
       // bool operator==(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start == start) && (edge.end == end); }
          bool operator!=(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start != start) || (edge.end != end) || (edge.labelString != labelString); }
          bool operator==(const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start == start) && (edge.end == end) && (edge.labelString == labelString); }
       // bool operator< (const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start < start) || (edge.start == start) && (edge.end < end); }
          bool operator< (const CustomAstDOTGenerationEdgeType & edge) const { return (edge.start < start) || (((edge.start == start) && (edge.labelString == labelString)) && (edge.end < end)); }
   };

class CustomAstDOTGenerationNodeType
   {
  // NodeType holdes information required to specify additional info for any node to DOT

     public:
          SgNode* target;
          std::string labelString;
          std::string optionString;

          CustomAstDOTGenerationNodeType (SgNode* node, std::string label, std::string options )
             : target(node), labelString(label), optionString(options)
             {
#ifdef _MSC_VER
//#define __builtin_constant_p(exp) (0)
#endif
               ROSE_ASSERT(node != NULL);
               ROSE_ASSERT(labelString.size() < 4000);
               ROSE_ASSERT(optionString.size() < 4000);
             }

       // DQ (3/21/2007): Added copy constructor
          CustomAstDOTGenerationNodeType ( const CustomAstDOTGenerationNodeType & X )
             : target(X.target), labelString(X.labelString), optionString(X.optionString)
             {
               ROSE_ASSERT(X.optionString.size() < 4000);
               ROSE_ASSERT(optionString.size() < 4000);
             }

       // These are implemented to support the STL find function (STL algorithm)
          bool operator!=(const CustomAstDOTGenerationNodeType & node) const { return node.target != target; }
          bool operator==(const CustomAstDOTGenerationNodeType & node) const { return node.target == target; }
   };

class CustomAstDOTGenerationData : public AstDOTGeneration
   {
  // Specialization of AST DOT graphics output class to support the overloaded operators
  // Note that this class is derived from a traversal and so can not be the class that is
  // used in the multiple inhertance design where CustomDOTGeneration is used.

     public:
          typedef CustomAstDOTGenerationNodeType NodeType;
          typedef CustomAstDOTGenerationEdgeType EdgeType;

          typedef std::list<NodeType> NodeListType;
          typedef std::list<EdgeType> EdgeListType;

          NodeListType nodeList;
          EdgeListType edgeList;

       // Call add functions in AstDOTGeneration class
          void addNode(NodeType n);
          void addEdge(EdgeType e);

          void addEdges();

       // void generate(string filename, SgProject* node);
       // void generateInputFiles(SgProject* node, traversalType tt=TOPDOWNBOTTOMUP, std::string filenamePostfix="");

       // void addEdges ( std::list<EdgeType> & edgeList );
          virtual std::string additionalNodeInfo(SgNode* node);
          virtual std::string additionalNodeOptions(SgNode* node);

       // DQ (3/5/2007) added mechanism to add options (to add color, etc.)
          virtual std::string additionalEdgeInfo    ( SgNode* from, SgNode* to, std::string label );
          virtual std::string additionalEdgeOptions ( SgNode* from, SgNode* to, std::string label );

       // This generates code for the DOT graph node
          std::string unparseToCompleteStringForDOT( SgNode* astNode );

       // Wrapup details and output the file (generates the rest of the nodes in the AST)
          void internalGenerateGraph( std::string filename, SgProject* project );
   };

// In order to get the CustomAstDOTGenerationData data member into the traversal
// we will represent it in a class and use multiple inheritance.
class CustomAstDOTGeneration
   {
     public:
          typedef CustomAstDOTGenerationData::NodeType NodeType;
          typedef CustomAstDOTGenerationData::EdgeType EdgeType;

       // Store a DOT graph (really just a list of nodes and edges in a traversal)
          CustomAstDOTGenerationData DOTgraph;

         ~CustomAstDOTGeneration();

       // Call add functions in CustomAstDOTGenerationData class
          void addNode(NodeType n);
          void addEdge(EdgeType e);

       // Wrapup details and output the file (generates the rest of the nodes in the AST)
          void internalGenerateGraph( std::string filename, SgProject* project );
   };

// Build a similar coloring mechanism as for the ROSE traversals developed by Markus,
// but do this for the Memory Pool based traversals.
class CustomMemoryPoolDOTGenerationData : public ROSE_VisitTraversal
   {
     public:
          typedef CustomAstDOTGenerationNodeType NodeType;
          typedef CustomAstDOTGenerationEdgeType EdgeType;

          typedef std::list<NodeType> NodeListType;
          typedef std::list<EdgeType> EdgeListType;

          typedef std::set<EdgeType> EdgeSetType;

          NodeListType nodeList;
          EdgeListType edgeList;

       // Nodes to ignore when building the DOT graph, we have to remove it from the nodeList if it is there.
          std::set<SgNode*> skipNodeList;

       // Edges to ignore when building whole AST graph
          EdgeSetType skipEdgeSet;

          virtual ~CustomMemoryPoolDOTGenerationData();

          void internalGenerateGraph(std::string dotfilename);

  // protected:
          void visit(SgNode* node);

       // Call the functions in the DOTRepresentation
          void addNode(NodeType n);
          void addEdge(EdgeType e);

          void skipNode(SgNode* n);
          void skipEdge(EdgeType e);

          void addEdges();

          virtual std::string additionalNodeInfo(SgNode* node);

       // DQ (11/1/2003) added mechanism to add options (to add color, etc.)
          virtual std::string additionalNodeOptions(SgNode* node);

       // DQ (3/5/2007) added mechanism to add options (to add color, etc.)
          virtual std::string additionalEdgeInfo    ( SgNode* from, SgNode* to, std::string label );
          virtual std::string additionalEdgeOptions ( SgNode* from, SgNode* to, std::string label );

          DOTRepresentation<SgNode*> dotrep;

     protected:
          std::string filename;
          std::set<SgNode*> visitedNodes;
   };

// In order to get the CustomAstDOTGenerationData data member into the traversal
// we will represent it in a class and use multiple inheritance.
// This is a copy of CustomDOTGeneration using a CustomMemoryPoolDOTGenerationData
// instead of a CustomAstDOTGenerationData data member
class CustomMemoryPoolDOTGeneration
   {
     public:
          typedef CustomAstDOTGenerationData::NodeType NodeType;
          typedef CustomAstDOTGenerationData::EdgeType EdgeType;

        // Liao, 10/23/2009, use flags to turn on/off filters, 0 : off, 1: on
         class s_Filter_Flags
         {
           public:
             int m_asmFileFormat;          /*asmFileFormatFilter()*/
             int m_asmType;            /* asmTypeFilter()*/
             int m_binaryExecutableFormat; /*binaryExecutableFormatFilter()*/
             int m_commentAndDirective;    /* commentAndDirectiveFilter()*/  
             int m_ctorInitializer;    /*ctorInitializerListFilter()*/

             int m_default; /* defaultFilter ()*/
             int m_defaultColor;           /*defaultColorFilter()*/
             int m_edge;   /* edgeFilter ()*/
             int m_emptySymbolTable;  /*emptySymbolTableFilter()*/ 
             int m_expression; /* expressionFilter ()*/

             int m_fileInfo;               /* fileInfoFilter ()*/
             int m_frontendCompatibility;  /* frontendCompatibilityFilter()*/
             int m_symbol;  /*symbolFilter ()*/
             int m_type;   /* typeFilter ()*/
             int m_variableDeclaration; /*variableDeclarationFilter()*/

             int m_variableDefinition; /*variableDefinitionFilter()*/

          // DQ (3/2/2010): Added option to turn off all filtering.
             int m_noFilter;            /* no filtering */

             //! Default constructor
             s_Filter_Flags ();
             //! Constructor from command line options
             s_Filter_Flags (std::vector <std::string>& argvList);
             //! Print out help about command options to set flags
             void print_commandline_help();
             ~s_Filter_Flags ();
             void print_filter_flags();

           private:
         //! Set default values for filter flags
             void setDefault();
         };
        
         s_Filter_Flags* filterFlags; // each instance has its own flag set

       // Store a DOT graph (really just a list of nodes and edges in a traversal)
          CustomMemoryPoolDOTGenerationData DOTgraph;
          // Constructors
          CustomMemoryPoolDOTGeneration();
          CustomMemoryPoolDOTGeneration(s_Filter_Flags* f = NULL);
      // ~CustomMemoryPoolDOTGeneration();

       // Call add functions in CustomMemoryPoolDOTGenerationData class
          void addNode(NodeType n);
          void addEdge(EdgeType e);

          void skipNode(SgNode* n);
          void skipEdge(EdgeType e);

       // Wrapup details and output the file (generates the rest of the nodes in the AST)
          void internalGenerateGraph( std::string filename );


       // DQ (5/11/2006): This filters out the gnu compatibility IR nodes (which tend to confuse everyone!)
          void frontendCompatibilityFilter(SgNode* n);
          void typeFilter(SgNode* n);
          void commentAndDirectiveFilter(SgNode* n);

       // DQ (5/14/2006): General mechanism to remove edges from generated DOT graph
          void edgeFilter(SgNode* nodeSource, SgNode* nodeSink, std::string edgeName );

       // DQ (5/11/2006): This adds colors to the whole AST graph
          void defaultColorFilter(SgNode* n);

       // Default filter to simplify the whole AST graph
          void defaultFilter(SgNode* n);

       // DQ (8/14/2008): Added to reduce the size and complexity of graphs of the executable format.
          void binaryExecutableFormatFilter(SgNode* node);

       // Mechanism to ignore all SgFile Info IR nodes (often they just get in the way)
          void fileInfoFilter(SgNode* node);

       // DQ (3/2/2009): Ignore empty symbol tables
          void emptySymbolTableFilter(SgNode* n);

       // DQ (3/2/2009): Ignore expression IR nodes
          void expressionFilter(SgNode* n);

       // DQ (3/2/2009): Ignore SgVariableDefinition IR nodes
          void variableDefinitionFilter(SgNode* n);
          void variableDeclarationFilter(SgNode* n);
          void ctorInitializerListFilter(SgNode* n);
          void symbolFilter(SgNode* n);

       // DQ (10/18/2009): Added support to skip output of binary file format in generation of AST visualization.
          void asmFileFormatFilter(SgNode* n);

       // DQ (10/18/2009): Added support to skip output of binary expression type information in generation of AST visualization.
          void asmTypeFilter(SgNode* n);
      private:
         void internal_init(s_Filter_Flags* f = NULL);
   };

#endif //WHOLE_AST_H

