// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

// Notes on fonts and colors
//   list all fonts on linux: "xlsfonts"
//   list all colors on linux: "man colors"
#include "rose.h"
#include <string>
#include <iomanip>

#include "AstConsistencyTests.h"

#include <algorithm>
// #include <algo.h>

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#include "nodeAndEdgeTypes.h"

// Support for overloaded operators
#include "AbstractionAttribute.h"

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
escapeNewLineCharaters ( const string & X )
   {
     string returnString;
     int stringLength = X.length();

     for (int i=0; i < stringLength; i++)
        {
          if ( X[i] == '\n' )
             {
               returnString += "\\l";
             }
            else
             {
               if ( X[i] == '\"' )
                  {
                    returnString += "\\\"";
                  }
                 else
                  {
                    returnString += X[i];
                  }
             }
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
     string returnString = escapeNewLineCharaters(outputString);
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
  // string sourceCodeString = unparseToCompleteStringForDOT(project);
  // dotrep.addNode(project,sourceCodeString,"shape=box");

     dotrep.writeToFileAsGraph(filename+".dot");
   }


// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class overloadedOperatorRepresentationInheritedAttribute
   {
     public:
      //! Specific constructors are required
          overloadedOperatorRepresentationInheritedAttribute () {};
          overloadedOperatorRepresentationInheritedAttribute ( const overloadedOperatorRepresentationInheritedAttribute & X ) {};
   };

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class overloadedOperatorRepresentationSynthesizedAttribute
   {
     public:
         overloadedOperatorRepresentationSynthesizedAttribute() {};
   };

// tree traversal to test the rewrite mechanism
 /*! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
  */
class overloadedOperatorRepresentationTraversal
   : public SgTopDownBottomUpProcessing<overloadedOperatorRepresentationInheritedAttribute,
                                        overloadedOperatorRepresentationSynthesizedAttribute>
   {
     public:
       // This value is a temporary data member to allow us to output the number of 
       // nodes traversed so that we can relate this number to the numbers printed 
       // in the AST graphs output via DOT.
          int traversalNodeCounter;

       // list of types that have been traversed
          list<NodeType> listOfNodes;
          list<EdgeType> listOfEdges;
         
          overloadedOperatorRepresentationTraversal (): traversalNodeCounter(0) {};

       // Functions required by the rewrite mechanism
          overloadedOperatorRepresentationInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode,
             overloadedOperatorRepresentationInheritedAttribute inheritedAttribute );

          overloadedOperatorRepresentationSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             overloadedOperatorRepresentationInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// Allocation of space for listOfTraversedTypes declared in overloadedOperatorRepresentationTraversal
// list<SgNode*> overloadedOperatorRepresentationTraversal::listOfTraversedTypes;

// Functions required by the tree traversal mechanism
overloadedOperatorRepresentationInheritedAttribute
overloadedOperatorRepresentationTraversal::evaluateInheritedAttribute (
     SgNode* astNode,
     overloadedOperatorRepresentationInheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("!!!!! In evaluateInheritedAttribute() \n");
     printf ("     (traversalNodeCounter=%d) astNode->sage_class_name() = %s \n",
          traversalNodeCounter,astNode->sage_class_name());
     printf ("     astNode->unparseToString() = %s \n",astNode->unparseToString().c_str());
#endif

#if 0
     vector<string> successorNames = astNode->get_traversalSuccessorNamesContainer();
     printf ("     #of successors = %zu successorNames = ",successorNames.size());
     for (vector<string>::iterator s = successorNames.begin(); s != successorNames.end(); s++)
        {
           printf ("%s ",(*s).c_str());
        }
     printf ("\n");
#endif

     traversalNodeCounter++;

#if 0
  // DQ (10/17/2005): There is no point to this code (I think)
     switch(astNode->variantT())
        {
          case V_SgFunctionCallExp:
             {
            // printf ("Found a SgFunctionCallExp \n");
               break;
             }

          default:
             {
            // everything else
             }
        }
#endif

     return inheritedAttribute;
   }

overloadedOperatorRepresentationSynthesizedAttribute
overloadedOperatorRepresentationTraversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     overloadedOperatorRepresentationInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     overloadedOperatorRepresentationSynthesizedAttribute returnAttribute;

  // Use an accumulator attribute and define it to be a stack of lists.
  //    1) each stack element represents a different operator (unary, binary, etc)
  //    2) each stack element is a list of operands (SgExpression*)
  //    3) each operand, function or overloaded function is placed onto the 
  //       list at the top of the current stack
  //    4) At each overloade function we:
  //         a) Build a unary or binary operator abstraction object
  //         b) attach the operands in top level list (of the stack of lists) to the operator abstraction object
  //         c) attach the operator abstraction object as an attribute to the current function node of the AST
  //    5) Pop the top of stack off the stack

  // Additional requirements:
  //    1) Graphics support via DOT
  //         a) Use Markus's DOT interface to represent extra nodes in graph (or just the expre edges)
  //    2) Traversal
  //         a) not too hard since only the expressions tree is traversed and it is always a tree by definition???.
  //         b) preorder, postorder traversals

  // string additionalNodeOptions = "color=\"LightBlue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
  // string additionalNodeOptions = "shape=polygon,sides=5,peripheries=3,color=\"LightBlue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
     string additionalNodeOptions = "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=5,peripheries=3,color=\"LightBlue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
     string associatedOperatorNodeOptions = "shape=polygon,sides=5,color=\"Blue\" ";

     string additionalEdgeOptions = "style=dashed,weight=1,color=blue ";

     switch(astNode->variantT())
        {
          case V_SgExprStatement:
             {
            // find the expression

               SgExprStatement* expressionStatement = isSgExprStatement(astNode);
               ROSE_ASSERT (expressionStatement != NULL);

               ExpressionStatementAttribute* overloadedExpressionStatement = new ExpressionStatementAttribute();
               ROSE_ASSERT (overloadedExpressionStatement != NULL);

               SgExpression* exp = overloadedExpressionStatement->getExpression(expressionStatement);
               ROSE_ASSERT (exp != NULL);

               EdgeType edge(expressionStatement,exp,additionalEdgeOptions);
               listOfEdges.push_back(edge);

               string labelWithSourceCode = string("\\n  ") + exp->unparseToString() + "  ";
               NodeType graphNode(expressionStatement,labelWithSourceCode,additionalNodeOptions);
               listOfNodes.push_back(graphNode);

            // Mark (color) all the nodes associated with this operator
               list<SgNode*> operatorNodeList = overloadedExpressionStatement->getOperatorNodes(expressionStatement);
               for (list<SgNode*>::iterator i = operatorNodeList.begin(); i != operatorNodeList.end(); i++)
                  {
                    NodeType tmpGraphNode(*i,"",associatedOperatorNodeOptions);
                    listOfNodes.push_back(tmpGraphNode);
                  }

            // Example of how to build an AST attribute
            // DQ (1/2/2006): Added support for new attribute interface.
               printf ("overloadedOperatorRepresentationTraversal::evaluateSynthesizedAttribute(): using new attribute interface \n");
#if 0
               if (expressionStatement->get_attribute() == NULL)
                  {
                    AstAttributeMechanism* attributePtr = new AstAttributeMechanism();
                    ROSE_ASSERT(attributePtr != NULL);
                    expressionStatement->set_attribute(attributePtr);
                  }
#endif
            // expressionStatement->attribute.set("overloadedOperator",overloadedExpressionStatement);
            // expressionStatement->attribute().set("overloadedOperator",overloadedExpressionStatement);
               expressionStatement->addNewAttribute("overloadedOperator",overloadedExpressionStatement);
               break;
             }
             
          case V_SgFunctionCallExp:
             {
            // Error checking: Verify that we have a SgFunctionCallExp object
               SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(astNode);
               ROSE_ASSERT (functionCallExpression != NULL);

               string operatorName = TransformationSupport::getFunctionName ( functionCallExpression );
               ROSE_ASSERT (operatorName.length() > 0);
            // printf ("CASE FUNC_CALL: Overloaded operator = %s \n",operatorName.c_str());

               string functionTypeName = TransformationSupport::getFunctionTypeName(functionCallExpression);
            // printf ("overloaded operator is of type = %s \n",functionTypeName.c_str());

               string additionalNodeLabelInfo = string("\\n  ") + functionTypeName + "::" + operatorName + "  ";

               TransformationSupport::operatorCodeType operatorCodeVariant =
                    TransformationSupport::classifyOverloadedOperator (operatorName.c_str());
            // printf ("CASE FUNC_CALL: operatorCodeVariant = %d \n",operatorCodeVariant);


            // Separating this case into additional cases makes up to some 
            // extent for using a more specific higher level grammar.
               switch (operatorCodeVariant)
                  {
                 // normal function call (not an overloaded function) (used as the default value)
                    case TransformationSupport::FUNCTION_CALL_OPERATOR_CODE:
                         printf ("Found a normal function as a parent: operatorName = %s operatorCodeVariant = %d \n",
                              operatorName.c_str(),operatorCodeVariant);
                         break;

                    case TransformationSupport::PARENTHESIS_OPERATOR_CODE:
                       {
                      // printf ("Found an operator()() (operator code = %d)  \n",operatorCodeVariant);
                         SgFunctionCallExp* functionCall = isSgFunctionCallExp(astNode);
                         ROSE_ASSERT (functionCall != NULL);
#if 1
                      // escape the "\n" so that it is not placed into the dot output as a "\n"
//                       string additionalLabelInfo = string("\\n") + functionTypeName + "::" + operatorName;
                         NodeType graphNode(functionCall,additionalNodeLabelInfo,additionalNodeOptions);
                         listOfNodes.push_back(graphNode);

                         ParenthesisOperatorAttribute* overloadedParenthesisOperator = new ParenthesisOperatorAttribute();
                         ROSE_ASSERT (overloadedParenthesisOperator != NULL);

                      // Draw the edges
                         SgExpression* indexedOperand = overloadedParenthesisOperator->getIndexedOperand(functionCall);
                         EdgeType graphEdge(functionCall,indexedOperand,additionalEdgeOptions);
                         listOfEdges.push_back(graphEdge);

                         int numberOfIndexObjects = overloadedParenthesisOperator->getNumberOfIndexOperands(functionCall);
                         for (int i = 0; i < numberOfIndexObjects; i++)
                            {
                              SgExpression* indexNode = overloadedParenthesisOperator->getIndex(functionCall,i);
                              EdgeType tmpEdge(functionCall,indexNode,additionalEdgeOptions);
                              listOfEdges.push_back(tmpEdge);
                            }

                      // Mark (color) all the nodes associated with this operator
                         list<SgNode*> operatorNodeList = 
                              overloadedParenthesisOperator->getOperatorNodes(functionCall);
                         for (list<SgNode*>::iterator i = operatorNodeList.begin(); i != operatorNodeList.end(); i++)
                            {
                              NodeType tmpGraphNode(*i,"",associatedOperatorNodeOptions);
                              listOfNodes.push_back(tmpGraphNode);
                            }

                      // Example of how to build an AST attribute
                      // DQ (1/2/2006): Added support for new attribute interface.
                         printf ("overloadedOperatorRepresentationTraversal::evaluateSynthesizedAttribute(): using new attribute interface \n");
#if 0
                         if (functionCall->get_attribute() == NULL)
                            {
                              AstAttributeMechanism* attributePtr = new AstAttributeMechanism();
                              ROSE_ASSERT(attributePtr != NULL);
                              functionCall->set_attribute(attributePtr);
                            }
#endif
                      // functionCall->attribute.set("overloadedOperator",overloadedParenthesisOperator);
                      // functionCall->attribute().set("overloadedOperator",overloadedParenthesisOperator);
                         functionCall->addNewAttribute("overloadedOperator",overloadedParenthesisOperator);
#endif
                         break;
                       }
                       

#if 0
                    case TransformationSupport::ASSIGN_OPERATOR_CODE:
                         printf ("Found an ASSIGNMENT OPERATOR: operatorName = %s operatorCodeVariant = %d \n",
                              operatorName.c_str(),operatorCodeVariant);
                         break;
#endif

                 // Overloaded binary operators: These are implemented a friend functions taking 2
                 // parameters instead of member functions taking only 1 parameter, as a result they
                 // have to be handled differently.
                    case TransformationSupport::ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::ADD_OPERATOR_CODE:
                    case TransformationSupport::SUBT_OPERATOR_CODE:
                    case TransformationSupport::MULT_OPERATOR_CODE:
                    case TransformationSupport::DIV_OPERATOR_CODE:
                    case TransformationSupport::INTEGER_DIV_OPERATOR_CODE:
                    case TransformationSupport::MOD_OPERATOR_CODE:
                    case TransformationSupport::AND_OPERATOR_CODE:
                    case TransformationSupport::OR_OPERATOR_CODE:
                    case TransformationSupport::BITXOR_OPERATOR_CODE:
                    case TransformationSupport::BITAND_OPERATOR_CODE:
                    case TransformationSupport::BITOR_OPERATOR_CODE:
                    case TransformationSupport::EQ_OPERATOR_CODE:
                    case TransformationSupport::LT_OPERATOR_CODE:
                    case TransformationSupport::GT_OPERATOR_CODE:
                    case TransformationSupport::NE_OPERATOR_CODE:
                    case TransformationSupport::LE_OPERATOR_CODE:
                    case TransformationSupport::GE_OPERATOR_CODE:
                    case TransformationSupport::PLUS_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::MINUS_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::AND_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::IOR_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::MULT_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::DIV_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::MOD_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::XOR_ASSIGN_OPERATOR_CODE:
                       {
                      // printf ("Found an OVERLOADED BINARY OPERATOR expression: operatorName = %s operatorCodeVariant = %d \n",
                      //      operatorName.c_str(),operatorCodeVariant);
                         SgFunctionCallExp* functionCall = isSgFunctionCallExp(astNode);
                         ROSE_ASSERT (functionCall != NULL);

                      // escape the "\n" so that it is not placed into the dot output as a "\n"
                      // options: fontname="xxx",fontcolor=red,style=filled
                      // string additionalOptions = "color=RED ";
//                       string additionalLabelInfo = string("\\n") + functionTypeName + "::" + operatorName;
//                       string additionalOptions = "color=\"LightBlue\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
                         
                         NodeType graphNode(functionCall,additionalNodeLabelInfo,additionalNodeOptions);
                         listOfNodes.push_back(graphNode);

                      // We don't need to provide more then a pointer to the current node 
                      // to which the attribute is attached because the attribute defines 
                      // queries to determin the lhs and rhs.
                         BinaryOperatorAttribute* overloadedBinaryOperator = new BinaryOperatorAttribute();
                         ROSE_ASSERT (overloadedBinaryOperator != NULL);

                         SgExpression* lhs = overloadedBinaryOperator->getLhsOperand(functionCall);
                         SgExpression* rhs = overloadedBinaryOperator->getRhsOperand(functionCall);

                         list<SgNode*> operatorNodeList = overloadedBinaryOperator->getOperatorNodes(functionCall);
                         for (list<SgNode*>::iterator i = operatorNodeList.begin(); i != operatorNodeList.end(); i++)
                            {
                              NodeType tmpGraphNode(*i,"",associatedOperatorNodeOptions);
                              listOfNodes.push_back(tmpGraphNode);
                            }

                         ROSE_ASSERT (lhs != NULL);
                         ROSE_ASSERT (rhs != NULL);

                      // style=(solid,dashed,dotted,bold,invis)
//                       string additionalEdgeOptions = "style=dashed,weight=10,color=blue ";
                         EdgeType lhsEdge(functionCall,lhs,additionalEdgeOptions);
                         EdgeType rhsEdge(functionCall,rhs,additionalEdgeOptions);

                         listOfEdges.push_back(lhsEdge);
                         listOfEdges.push_back(rhsEdge);

                      // Example of how to build an AST attribute
                      // DQ (1/2/2006): Added support for new attribute interface.
                         printf ("overloadedOperatorRepresentationTraversal::evaluateSynthesizedAttribute(): using new attribute interface \n");
#if 0
                         if (functionCall->get_attribute() == NULL)
                            {
                              AstAttributeMechanism* attributePtr = new AstAttributeMechanism();
                              ROSE_ASSERT(attributePtr != NULL);
                              functionCall->set_attribute(attributePtr);
                            }
#endif
                      // functionCall->attribute.set("overloadedOperator",overloadedBinaryOperator);
                      // functionCall->attribute().set("overloadedOperator",overloadedBinaryOperator);
                         functionCall->addNewAttribute("overloadedOperator",overloadedBinaryOperator);
                         break;
                       }

                 // unary operators:
                    case TransformationSupport::NOT_OPERATOR_CODE:
                    case TransformationSupport::DEREFERENCE_OPERATOR_CODE:
                    case TransformationSupport::ADDRESS_OPERATOR_CODE:
                    case TransformationSupport::LSHIFT_OPERATOR_CODE:
                    case TransformationSupport::RSHIFT_OPERATOR_CODE:
                    case TransformationSupport::LSHIFT_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::RSHIFT_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::PREFIX_PLUSPLUS_OPERATOR_CODE:
                    case TransformationSupport::POSTFIX_PLUSPLUS_OPERATOR_CODE:
                    case TransformationSupport::PREFIX_MINUSMINUS_OPERATOR_CODE:
                    case TransformationSupport::POSTFIX_MINUSMINUS_OPERATOR_CODE:
                       {
                         printf ("Found an OVERLOADED UNARY OPERATOR expression \n");

                         printf ("Sorry, unary operators not yet implemented \n");
                         ROSE_ABORT();

                         break;
                       }

                    default:
                         printf ("default in switch found in variant = %d \n",operatorCodeVariant);
                         ROSE_ABORT();
                  }
             }

       // DQ (10/17/2005): default case added to eliminate compiler warnings
          default:
             {
            // Nothing to do there
             }
        }

     return returnAttribute;
   }

int
main ( int argc, char* argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

     SgProject* project = frontend(argc,argv);

     AstTests::runAllTests(const_cast<SgProject*>(project));

  // printf ("Generate the pdf output of the SAGE III AST \n");
  // generatePDF ( project );

#if 1
     overloadedOperatorRepresentationTraversal treeTraversal;
     overloadedOperatorRepresentationInheritedAttribute inheritedAttribute;

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

     graphics.generate("overloadedTest",project);
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

     return backend(project);
  // return backend(frontend(argc,argv));
   }




























