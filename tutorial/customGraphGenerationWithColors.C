#include "rose.h"

using namespace std;

class EdgeType
   {
  // EdgeType holdes information required to specify an edge to DOT

     public:
          SgNode* start;
          SgNode* end;
          string optionString;

          EdgeType (SgNode* n1, SgNode* n2, string options )
             : start(n1), end(n2), optionString(options) {}
   };

class NodeType
   {
  // NodeType holdes information required to specify additional info for any node to DOT

     public:
          SgNode* target;
          string labelString;
          string optionString;

          NodeType (SgNode* node, string label, string options )
             : target(node), labelString(label), optionString(options) {}

       // These are implemented to support the STL find function (STL algorithm)
       // bool operator= (const NodeType & node) const { return node.target == target; }
          bool operator!=(const NodeType & node) const { return node.target != target; }
          bool operator==(const NodeType & node) const { return node.target == target; }
   };


class myAstDOTGeneration : public AstDOTGeneration
   {
  // Specialization of AST DOT graphics output class to support the overloaded operators

     public:
          list<NodeType> nodeList;
          list<EdgeType> edgeList;

          myAstDOTGeneration ( list<NodeType> & nodes, list<EdgeType> & edges )
             : nodeList(nodes), edgeList(edges) {}

          void generate(string filename, SgProject* node, traversalType tt=TOPDOWNBOTTOMUP);
          void generateInputFiles(SgProject* node, traversalType tt=TOPDOWNBOTTOMUP, string filenamePostfix="");

          void addEdges ( list<EdgeType> & edgeList );
          string additionalNodeInfo(SgNode* node);
          string additionalNodeOptions(SgNode* node);
   };

void 
myAstDOTGeneration::addEdges ( list<EdgeType> & edgeList )
   {
  // Add list of edges to the dot graph

     for (list<EdgeType>::iterator i = edgeList.begin(); i != edgeList.end(); i++)
        {
          ROSE_ASSERT ((*i).start != NULL);
          ROSE_ASSERT ((*i).end   != NULL);
       // printf ("Adding an edge from a %s to a %s with options = %s \n",
       //      (*i).start->sage_class_name(),(*i).end->sage_class_name(),(*i).optionString.c_str());
          dotrep.addEdge((*i).start,(*i).end,(*i).optionString);
        }
   }

string
myAstDOTGeneration::additionalNodeInfo(SgNode* node)
   {
  // This only modifies the node label not the options list that would 
  // follow the node's label in the final dot output file.
     string returnString;

     const NodeType tmp (node,"","");
     list<NodeType>::iterator i = nodeList.end();
     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
          returnString = (*i).labelString;
       // printf ("Adding node label info (%s) to %s \n",returnString.c_str(),node->sage_class_name());
        }

     return returnString;
   }

string
myAstDOTGeneration::additionalNodeOptions(SgNode* node)
   {
  // This function adds options (after the node label) in the output DOT file
     string returnString;

     const NodeType tmp (node,"","");
     list<NodeType>::iterator i = nodeList.end(); 
     i = find(nodeList.begin(),nodeList.end(),tmp);
     if (i != nodeList.end())
        {
          returnString = (*i).optionString;
       // printf ("Adding node option info (%s) to %s \n",returnString.c_str(),node->sage_class_name());
        }

     return returnString;
   }

string
unparseToCompleteStringForDOT( SgNode* astNode )
   {
  // Generate the string (pass a SgUnparse_Info object)
     SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
     inputUnparseInfoPointer->unset_SkipComments();    // generate comments
     inputUnparseInfoPointer->unset_SkipWhitespaces(); // generate all whitespaces to format the code
     string outputString = globalUnparseToString(astNode,inputUnparseInfoPointer);
     string returnString = StringUtility::escapeNewLineCharaters(outputString);
     return returnString;
   }

void
myAstDOTGeneration::generate ( string filename, SgProject* project, traversalType tt )
   {
     init();
     traversal = TOPDOWNBOTTOMUP;
     DOTInheritedAttribute ia;

  // traverse(node,ia);
     traverseInputFiles(project,ia);

  // Put extra code here
     addEdges(edgeList);

  // Output the source code to a new node (we need a function to escape all the """ and "\n"
     string sourceCodeString = unparseToCompleteStringForDOT(project);
     dotrep.addNode(project,sourceCodeString,"shape=box");

     dotrep.writeToFileAsGraph(filename+".dot");
   }


// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class ConstantFoldingColoringInheritedAttribute
   {
     public:
          bool isConstantFoldedValue;
          bool isPartOfFoldedExpression;

      //! Specific constructors are required
          ConstantFoldingColoringInheritedAttribute ()
             : isConstantFoldedValue(false), isPartOfFoldedExpression(false) {};
          ConstantFoldingColoringInheritedAttribute ( const ConstantFoldingColoringInheritedAttribute & X )
             : isConstantFoldedValue(X.isConstantFoldedValue), isPartOfFoldedExpression(X.isPartOfFoldedExpression) {};
   };

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class ConstantFoldingColoringSynthesizedAttribute
   {
     public:
         ConstantFoldingColoringSynthesizedAttribute() {};
   };

// tree traversal to test the rewrite mechanism
 /*! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
  */
class ConstantFoldingColoringTraversal
   : public SgTopDownBottomUpProcessing<ConstantFoldingColoringInheritedAttribute,
                                        ConstantFoldingColoringSynthesizedAttribute>
   {
     public:
       // This value is a temporary data member to allow us to output the number of 
       // nodes traversed so that we can relate this number to the numbers printed 
       // in the AST graphs output via DOT.
          int traversalNodeCounter;

       // list of types that have been traversed
          list<NodeType> listOfNodes;
          list<EdgeType> listOfEdges;
         
          ConstantFoldingColoringTraversal (): traversalNodeCounter(0) {};

       // Functions required by the rewrite mechanism
          ConstantFoldingColoringInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode,
             ConstantFoldingColoringInheritedAttribute inheritedAttribute );

          ConstantFoldingColoringSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             ConstantFoldingColoringInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// Allocation of space for listOfTraversedTypes declared in ConstantFoldingColoringTraversal
// list<SgNode*> ConstantFoldingColoringTraversal::listOfTraversedTypes;

// Functions required by the tree traversal mechanism
ConstantFoldingColoringInheritedAttribute
ConstantFoldingColoringTraversal::evaluateInheritedAttribute (
     SgNode* astNode,
     ConstantFoldingColoringInheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("!!!!! In evaluateInheritedAttribute() \n");
     printf ("     (traversalNodeCounter=%d) astNode->sage_class_name() = %s \n",
          traversalNodeCounter,astNode->sage_class_name());
     printf ("     astNode->unparseToString() = %s \n",astNode->unparseToString().c_str());
#endif

     if (inheritedAttribute.isConstantFoldedValue == true)
        {
          inheritedAttribute.isConstantFoldedValue = false;
          inheritedAttribute.isPartOfFoldedExpression = true;
        }
       else
        {
          SgValueExp* valueExpression = isSgValueExp(astNode);
       // Check for constant folded value
          if (valueExpression != NULL && valueExpression->get_originalExpressionTree() != NULL)
             {
               inheritedAttribute.isConstantFoldedValue = true;
             }
        }     

     return inheritedAttribute;
   }

ConstantFoldingColoringSynthesizedAttribute
ConstantFoldingColoringTraversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     ConstantFoldingColoringInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     ConstantFoldingColoringSynthesizedAttribute returnAttribute;

  // string additionalNodeOptions = "color=\"LightBlue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
  // string additionalNodeOptions = "shape=polygon,sides=5,peripheries=3,color=\"LightBlue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
  // string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=3,color=\"LightBlue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
  // string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=4,peripheries=1,color=\"Blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
  // string associatedOperatorNodeOptions = "shape=polygon,sides=5,color=\"Blue\" ";

     string additionalEdgeOptions = "style=dashed,weight=1,color=blue ";

  // Demonstrate something special for statements
     SgStatement* currentStatement = isSgStatement(astNode);
     if (currentStatement != NULL)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=4,peripheries=1,color=\"Blue\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this statement different in the generated dot graph
       // string labelWithSourceCode = string("\\n  ") + currentStatement->unparseToString() + "  ";
          string labelWithSourceCode;
          if (isSgScopeStatement(currentStatement) == NULL)
               labelWithSourceCode = string("\\n  ") + currentStatement->unparseToString() + "  ";
          NodeType graphNode(currentStatement,labelWithSourceCode,additionalNodeOptions);
          listOfNodes.push_back(graphNode);
#if 0
       // And an edge back to the scope
          if (isSgGlobal(currentStatement) == NULL)
             {
               EdgeType edge(currentStatement,currentStatement->get_scope(),additionalEdgeOptions);
               listOfEdges.push_back(edge);
             }
#endif
        }

     SgExpression* currentExpression = isSgExpression(astNode);
     if (currentExpression != NULL)
        {
          string additionalNodeOptions;
       // Make this expression different in the generated dot graph
          string labelWithSourceCode;
#if 0
          SgValueExp* valueExpression = isSgValueExp(currentExpression);
       // Check for constant folded value
          if (valueExpression != NULL && valueExpression->get_valueExpressionTree() != NULL)
             {
            // This is a constant folded value (the extression tree from the original expression is obtained by get_valueExpressionTree()
               additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=3,peripheries=1,color=\"Blue\",fillcolor=yellow1,fontname=\"7x13bold\",fontcolor=black,style=filled";
               SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
               inputUnparseInfoPointer->set_SkipConstantFoldedExpressions(); // generate folded values instead of the original expression trees
               labelWithSourceCode = string("\\n  ") + valueExpression->unparseToString(inputUnparseInfoPointer) + "  ";
               delete inputUnparseInfoPointer;
               inputUnparseInfoPointer = NULL;
             }
            else
             {
               labelWithSourceCode = string("\\n  ") + currentExpression->unparseToString() + "  ";
               additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
             }
#else
       // See if this is part of constant folding within the AST
          if ( (inheritedAttribute.isConstantFoldedValue == true) || (inheritedAttribute.isPartOfFoldedExpression == true) )
             {
            // Check what part of constant folding this is (folded value)
               if (inheritedAttribute.isConstantFoldedValue == true)
                  {
                 // This is a constant folded value (the extression tree from the original expression is obtained by get_valueExpressionTree()
                    additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=3,peripheries=1,color=\"Blue\",fillcolor=yellow1,fontname=\"7x13bold\",fontcolor=black,style=filled";
                 // SgValueExp* valueExpression = isSgValueExp(currentExpression);
                    SgUnparse_Info* inputUnparseInfoPointer = new SgUnparse_Info();
                    inputUnparseInfoPointer->set_SkipConstantFoldedExpressions(); // generate folded values instead of the original expression trees
                    labelWithSourceCode = string("\\n  ") + currentExpression->unparseToString(inputUnparseInfoPointer) + "  ";
                    delete inputUnparseInfoPointer;
                    inputUnparseInfoPointer = NULL;
                  }
            // Check what part of constant folding this is (folded expression)
               if (inheritedAttribute.isPartOfFoldedExpression == true)
                  {
                 // This is a constant folded value (the extression tree from the original expression is obtained by get_valueExpressionTree()
                    additionalNodeOptions = "shape=ellipse,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",peripheries=1,color=\"Blue\",fillcolor=orange,fontname=\"7x13bold\",fontcolor=black,style=filled";
                    labelWithSourceCode = string("\\n  ") + currentExpression->unparseToString() + "  ";
                  }
             }
            else
             {
               labelWithSourceCode = string("\\n  ") + currentExpression->unparseToString() + "  ";
               additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
             }
#endif
          NodeType graphNode(currentExpression,labelWithSourceCode,additionalNodeOptions);
          listOfNodes.push_back(graphNode);
        }

     SgInitializedName* initializedName = isSgInitializedName(astNode);
     if (initializedName != NULL)
        {
          string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=8,peripheries=1,color=\"Blue\",fillcolor=violet,fontname=\"7x13bold\",fontcolor=black,style=filled";
       // Make this SgInitializedName different in the generated dot graph
       // string labelWithSourceCode = string("\\n  ") + initializedName->unparseToString() + "  ";
          string labelWithSourceCode = string("\\n  ") + initializedName->get_qualified_name().getString() + "  ";
          NodeType graphNode(initializedName,labelWithSourceCode,additionalNodeOptions);
          listOfNodes.push_back(graphNode);
        }

     return returnAttribute;
   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

     SgProject* project = frontend(argc,argv);

  // AstTests::runAllTests(const_cast<SgProject*>(project));
     AstTests::runAllTests(project);

  // printf ("Generate the pdf output of the SAGE III AST \n");
  // generatePDF ( project );

#if 1
     ConstantFoldingColoringTraversal treeTraversal;
     ConstantFoldingColoringInheritedAttribute inheritedAttribute;

  // Ignore the return value since we don't need it
     treeTraversal.traverseInputFiles(project,inheritedAttribute);
#endif

#if 0
     printf ("Generate the DOT output of the SAGE III AST \n");
     generateDOT ( *project );
#endif

#if 1
  // myAstDOTGeneration graphics;
     myAstDOTGeneration graphics(treeTraversal.listOfNodes,treeTraversal.listOfEdges);

     graphics.generate("customGraphGenerationWithColors",project);
  // graphics.generate("overloadedTest",project,treeTraversal.listOfNodes,treeTraversal.listOfEdges);
  // graphics.generateInputFiles(project);
  // graphics.generateInputFiles(project);
  // graphics.addEdges(treeTraversal.listOfEdges);
#endif

#if 0
     AstDOTGeneration astdotgen;
  // SgProject & nonconstProject = (SgProject &) project;
     astdotgen.generateInputFiles(project);
#endif

  // return backend(project);
  // return backend(frontend(argc,argv));
     return 0;
   }






