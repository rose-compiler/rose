
// Generate a patch to segregate code into a base code and a code with enhansed capabilities.
// Note generation of the patch requires output using the token stream (-rose:unparse_tokens).

#include "whiteSpaceTransformation.h"

using namespace std;

using namespace SageBuilder;
using namespace SageInterface;

string prefixForFunctions = "abc_function_";
string prefixForVariables = "abc_var_";

WhiteSpaceInheritedAttribute::WhiteSpaceInheritedAttribute ()
   : foundSegragationBoundary_start(false),
     astNode(NULL),
     segregateThisNode(false),
     foundSegregatedNode(false)
   {
   }

WhiteSpaceInheritedAttribute::WhiteSpaceInheritedAttribute ( const WhiteSpaceInheritedAttribute & X )
   {
     foundSegragationBoundary_start = X.foundSegragationBoundary_start;
     astNode                        = X.astNode;
     segregateThisNode              = X.segregateThisNode;
     foundSegregatedNode            = X.foundSegregatedNode;
   }


WhiteSpaceSynthesizedAttribute::WhiteSpaceSynthesizedAttribute ()
   : foundSegragationBoundary_start(false),
     foundSegragationBoundary_end(false),
     astNode(NULL),
     segregateThisNode(false),
     containsSegregatedNode(false)
   {
#if 0
     printf ("WhiteSpaceSynthesizedAttribute default constructor \n");
#endif
   }

WhiteSpaceSynthesizedAttribute::WhiteSpaceSynthesizedAttribute ( SgNode* input_astNode )
   : foundSegragationBoundary_start(false),
     foundSegragationBoundary_end(false),
     astNode(input_astNode),
     segregateThisNode(false),
     containsSegregatedNode(false)
   {
#if 0
     printf ("WhiteSpaceSynthesizedAttribute constructor taking SgNode* \n");
#endif
   }

WhiteSpaceSynthesizedAttribute::WhiteSpaceSynthesizedAttribute ( const WhiteSpaceSynthesizedAttribute & X )
   : foundSegragationBoundary_start(X.foundSegragationBoundary_start),
     foundSegragationBoundary_end(X.foundSegragationBoundary_end),
     astNode(X.astNode),
     segregateThisNode(X.segregateThisNode),
     containsSegregatedNode(X.containsSegregatedNode)
   {
#if 0
     printf ("WhiteSpaceSynthesizedAttribute copy constructor astNode = %p \n",astNode);
#endif
   }

WhiteSpaceSynthesizedAttribute &
WhiteSpaceSynthesizedAttribute::operator= ( const WhiteSpaceSynthesizedAttribute & X )
   {
     foundSegragationBoundary_start = X.foundSegragationBoundary_start;
     foundSegragationBoundary_end   = X.foundSegragationBoundary_end;
     astNode                        = X.astNode;
     segregateThisNode              = X.segregateThisNode;
     containsSegregatedNode         = X.containsSegregatedNode;

#if 0
     printf ("WhiteSpaceSynthesizedAttribute operator=() astNode = %p \n",astNode);
#endif

     return *this;
   }


Segregation_Attribute::Segregation_Attribute()
   {
#if 0
     printf ("In Segregation_Attribute default constructor \n");
#endif
   }


Segregation_Attribute::~Segregation_Attribute()
   {
#if 0
     printf ("In Segregation_Attribute destructor \n");
#endif
   }


std::string
Segregation_Attribute::toString()
   {
#if 0
     printf ("In Segregation_Attribute::toString() \n");
#endif
     return "";
   }


std::string
Segregation_Attribute::additionalNodeOptions()
   {
#if 0
     printf ("In Segregation_Attribute::additionalNodeOptions() \n");
#endif
     return "";
   }


std::vector<AstAttribute::AttributeEdgeInfo>
Segregation_Attribute::additionalEdgeInfo()
   {
     std::vector<AstAttribute::AttributeEdgeInfo> returnValue;

#if 0
     printf ("ERROR: not clear how to return a std::vector<AstAttribute::AttributeEdgeInfo> \n");
#endif

     return returnValue;
   }


std::string
Segregation_Attribute::get_name()
   {
     return "color";
   }


std::string
Segregation_Attribute::get_color()
   {
     return "blue";
   }






bool
nameHasPrefix( const string & name, const string & prefix )
   {
#if 0
     printf ("checking name = %s for prefix = %s \n",name.c_str(),prefix.c_str());
#endif

     bool returnValue = false;

     size_t prefixLength = prefix.length();

     if (prefixLength > 0)
        {
          size_t nameLength   = name.length();

       // This could be a valid prefix.
          if (nameLength > prefixLength)
             {
               size_t startOfName  = 0;
               string leadingPrefixRangeOfName = name.substr(startOfName,prefixLength);
               if (leadingPrefixRangeOfName == prefix)
                  {
                    returnValue = true;
                  }
             }
            else
             {
            // name not long enough to hold the prefix.
#if 0
               printf ("Detected name = %s not long enough to hold the specified prefix = %s \n",name.c_str(),prefix.c_str());
#endif
            // returnValue = false;
             }
        }
       else
        {
       // This is an invalid prefix.
#if 1
          printf ("Detected an invalid prefix string \n");
          ROSE_ASSERT(false);
#endif
       // returnValue = false;
        }

     return returnValue;
   }


WhiteSpaceInheritedAttribute
Traversal::evaluateInheritedAttribute (
     SgNode* astNode,
     WhiteSpaceInheritedAttribute inheritedAttribute )
   {
     WhiteSpaceInheritedAttribute result;

     result.astNode = astNode;

     ROSE_ASSERT(result.astNode != NULL);

     result.foundSegregatedNode = false;

#if 0
     printf ("In evaluateInheritedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
     if (functionDeclaration != NULL)
        {
       // Check for the function to have the prefix we are looking for.
          string functionName = functionDeclaration->get_name();
          bool isMatchingFunction = nameHasPrefix(functionName,prefixForFunctions);
          if (isMatchingFunction == true)
             {
               printf ("Found matching function name: name = %s \n",functionName.c_str());

            // result.foundSegragationBoundary_start = true;
               result.foundSegregatedNode = true;

            // Find the associatedSymbol
               SgFunctionSymbol* functionSymbol = NULL;

            // Save the associated symbol
               symbolSet.insert(functionSymbol);
             }
        }

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(astNode);
     if (variableDeclaration != NULL)
        {
       // Check for the function to have the prefix we are looking for.
          SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
          for (size_t i=0; i < variableList.size(); i++)
             {
               SgInitializedName* initializedName = variableList[i];
               ROSE_ASSERT(initializedName != NULL);

               string variableName = initializedName->get_name();
               bool isMatchingVariable = nameHasPrefix(variableName,prefixForVariables);
#if 0
               printf ("@@@ Found variable: initializedName #%zu name = %s isMatchingVariable = %s \n",i,variableName.c_str(),isMatchingVariable ? "true" : "false");
#endif
               if (isMatchingVariable == true)
                  {
#if 0
                    printf ("Found matching variable name: name = %s \n",variableName.c_str());
#endif
                 // result.foundSegragationBoundary_start = true;
                    result.foundSegregatedNode = true;

                 // DQ (11/24/2018): Distinguish between statements that should be segregated, 
                 // and statements containing nodes to be segregated.
                    result.segregateThisNode   = true;

                 // Find the associatedSymbol
                    SgVariableSymbol* variableSymbol = NULL;

                 // Save the associated symbol
                    symbolSet.insert(variableSymbol);
                  }
             }
        }

     SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(astNode);
     if (functionRefExp != NULL)
        {
       // Look for symbols associated with functions we put into the symbolSet.
       // Need to handle where we might not yet have seen the declaration (in classes) 
       // and so the symbol has not yet been collected.

          SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
          ROSE_ASSERT(functionSymbol != NULL);
          if (symbolSet.find(functionSymbol) != symbolSet.end())
             {
               result.foundSegregatedNode = true;
             }
            else
             {
            // Double check the associated declaration (in case we are in a class).
               string functionName = functionSymbol->get_name();
               bool isMatchingFunction = nameHasPrefix(functionName,prefixForFunctions);
               if (isMatchingFunction == true)
                  {
                    result.foundSegregatedNode = true;
                  }
             }
        }

     SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
     if (varRefExp != NULL)
        {
       // Look for symbols associated with variables we put into the symbolSet.
       // Need to handle where we might not yet have seen the declaration (in classes) 
       // and so the symbol has not yet been collected.

          SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
          ROSE_ASSERT(variableSymbol != NULL);
          if (symbolSet.find(variableSymbol) != symbolSet.end())
             {
               result.foundSegregatedNode = true;
             }
            else
             {
            // Double check the associated declaration (in case we are in a class).
               string variableName = variableSymbol->get_name();
               bool isMatchingVariable = nameHasPrefix(variableName,prefixForVariables);
               if (isMatchingVariable == true)
                  {
                    result.foundSegregatedNode = true;
                  }
             }
        }

     ROSE_ASSERT(result.astNode != NULL);

     if (result.foundSegregatedNode == true)
        {
#if 0
          printf ("Found a node that we will segregate: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     return result;
   }


WhiteSpaceSynthesizedAttribute
Traversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     WhiteSpaceInheritedAttribute inheritedAttribute,
     SynthesizedAttributesList childAttributes )
   {
  // SynthesizedAttribute localResult(astNode);
  // localResult.astNode = astNode;
  // ROSE_ASSERT(localResult.astNode != NULL);

     ROSE_ASSERT(astNode != NULL);
     WhiteSpaceSynthesizedAttribute localResult(astNode);

#if 0
     printf ("In evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
#if 0
     printf ("   --- inheritedAttribute.foundSegregatedNode = %s \n",inheritedAttribute.foundSegregatedNode ? "true" : "false");
     printf ("   --- inheritedAttribute.segregateThisNode   = %s \n",inheritedAttribute.segregateThisNode ? "true" : "false");
#endif

  // Even if there are no children, we need to transfer the state to the synthisized attribute.
     if (inheritedAttribute.foundSegregatedNode == true)
        {
#if 0
          printf ("   --- Mark SynthesizedAttribute as containing a segregated node \n");
#endif
          localResult.containsSegregatedNode = true;

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     if (inheritedAttribute.segregateThisNode == true)
        {
#if 0
          printf ("   --- Mark SynthesizedAttribute as to be a segregated node \n");
#endif
          localResult.segregateThisNode = true;
        }

#if 0
     printf ("   --- childAttributes.size() = %zu \n",childAttributes.size());
#endif

     bool contiguousRegion = false;

     pair<SgNode*,SgNode*>* interval = NULL;

  // We only want to mark intervales within scopes.
     SgScopeStatement* scope = isSgScopeStatement(astNode);
     if (scope != NULL)
        {
#if 0
          printf ("SCOPE = %s: childAttributes.size() = %zu \n",scope->class_name().c_str(),childAttributes.size());
#endif

          WhiteSpaceSynthesizedAttribute* previous_child = NULL;
          for (size_t i = 0; i < childAttributes.size(); i++)
             {
               WhiteSpaceSynthesizedAttribute & child = childAttributes[i];
#if 0
               printf ("   --- --- child #%zu \n",i);
            // printf ("   --- --- child = %p = %s \n",child.astNode,child.astNode != NULL ? child.astNode->class_name().c_str() : "null");
               printf ("   --- --- child = %p = %s \n",child.astNode,child.astNode != NULL ? SageInterface::get_name(child.astNode).c_str() : "null");
               printf ("   --- --- child.containsSegregatedNode = %s \n",child.containsSegregatedNode ? "true" : "false");
               printf ("   --- --- child.segregateThisNode      = %s \n",child.segregateThisNode ? "true" : "false");
#endif
            // If any of the child nodes have a detected segregated region, then this IR node is marked as containing a segregated region.
            // if (child.containsSegregatedNode == true)
               if (child.segregateThisNode == true)
                  {
#if 0
                 // printf ("   --- --- Child contains a segregated node! \n");
                    printf ("   --- --- Child is a node to segregate! \n");
#endif
                 // This synthesized attribute may contain more than one interval of segregated nodes (statements).
                    localResult.containsSegregatedNode = true;

                    if (contiguousRegion == false)
                       {
#if 0
                         printf ("   --- --- start of segregated region \n");
#endif
                         contiguousRegion = true;

                      // If we are starting an interval, then interval must be NULL.
                         ROSE_ASSERT(interval == NULL);
                         if (interval == NULL)
                            {
                              ROSE_ASSERT(child.astNode != NULL);
                              interval = new pair<SgNode*,SgNode*>(child.astNode,NULL);

                              ROSE_ASSERT((*interval).first != NULL);
                              if ((*interval).first != child.astNode)
                                 {
                                   printf ("(*interval).first = %p = %s \n",(*interval).first,(*interval).first->class_name().c_str());
                                   printf ("astNode = %p = %s \n",child.astNode,child.astNode->class_name().c_str());
                                 }
                              ROSE_ASSERT((*interval).first == child.astNode);
                            }

                         ROSE_ASSERT((*interval).first != NULL);
                         ROSE_ASSERT(astNode != NULL);

                         ROSE_ASSERT(interval != NULL);

                      // Look for the last elsment in the child list (
                         if (i == childAttributes.size() - 1)
                            {
#if 0
                              printf ("At first child to be segregated AND the last child in childAttributes list \n");
#endif
                              (*interval).second = child.astNode;

                              ROSE_ASSERT((*interval).first != NULL);
                              ROSE_ASSERT((*interval).second == child.astNode);
#if 0
                              printf ("Push the segregated interval onto a list \n");
#endif
                              intervalList.push_back(interval);

                           // Reset the interval used to store segregated regions.
                              interval = NULL;
                            }
                       }
                      else
                       {
#if 0
                         printf ("In interior of segregated region: i = %zu \n",i);
#endif
                         ROSE_ASSERT(interval != NULL);

                      // Look for the last elsment in the child list (
                         if (i == childAttributes.size() - 1)
                            {
#if 0
                              printf ("At last child in childAttributes list \n");
#endif
                              (*interval).second = child.astNode;

                              ROSE_ASSERT((*interval).first != NULL);
                              ROSE_ASSERT((*interval).second == child.astNode);

                              printf ("Push the segregated interval onto a list \n");
                              intervalList.push_back(interval);

                           // Reset the interval used to store segregated regions.
                              interval = NULL;
                            }
                       }

#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
                 else
                  {
#if 0
                    printf ("   --- --- end of segregated region \n");
#endif
                    contiguousRegion = false;

                 // Push interval.
                 // ROSE_ASSERT(interval != NULL);
                    if (interval != NULL)
                       {
                         ROSE_ASSERT(child.astNode != NULL);

                      // DQ (11/28/2018): We want the previous node that was a part of the interval, 
                      // not the first node that is NOT a part of the interval.
                      // (*interval).second = child.astNode;
                         ROSE_ASSERT(previous_child != NULL);
                         (*interval).second = previous_child->astNode;

                         ROSE_ASSERT((*interval).first != NULL);
                      // ROSE_ASSERT((*interval).second == child.astNode);
                         ROSE_ASSERT((*interval).second == previous_child->astNode);
#if 0
                         printf ("Push the segregated interval onto a list \n");
#endif
                         intervalList.push_back(interval);

                      // Reset the interval used to store segregated regions.
                         interval       = NULL;
                         previous_child = NULL;
                       }
#if 0
                    printf ("intervalList.size() = %zu \n",intervalList.size());
#endif
                  }

            // DQ (11/28/2018): Remember the previous child so that we can close off the interval where it is identified.
               previous_child = &child;
             }

#if 0
       // DQ (11/23/2018): Stop to debug global scope.
          SgGlobal* globalScope = isSgGlobal(astNode);
          if (globalScope != NULL)
             {
               printf ("At end of global scope: intervalList.size() = %zu \n",intervalList.size());
               for (size_t i = 0; i < intervalList.size(); i++)
                  {
                    printf ("intervalList[i]->first = %p second = %p \n",intervalList[i]->first,intervalList[i]->second);
                  }

               printf ("Exiting as a test at the end of evaluation of global scope! \n");
               ROSE_ASSERT(false);
             }
#endif
        }
       else
        {
#if 0
          printf ("Handle case of NOT a scope! \n");
#endif
          for (size_t i = 0; i < childAttributes.size(); i++)
             {
               WhiteSpaceSynthesizedAttribute & child = childAttributes[i];

               if (child.containsSegregatedNode == true)
                  {
#if 0
                    printf ("Child (of non-scope) contains a segregated node! \n");
#endif
                    localResult.containsSegregatedNode = true;

                 // Mark the SgExprStatement as (segregateThisNode == true) if child node is marked (containsSegregatedNode == true).
                    SgExprStatement* expressionStatement = isSgExprStatement(astNode);
                    if (expressionStatement != NULL)
                       {
#if 0
                         printf ("   --- Mark SynthesizedAttribute as to be a segregated node (case of SgExprStatement) \n");
#endif
                         localResult.segregateThisNode = true;
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
        }

#if 0
     printf ("Leaving evaluateSynthesizedAttribute(): astNode = %p = %s = %s \n",astNode,astNode->class_name().c_str(),SageInterface::get_name(astNode).c_str());
     printf ("   --- localResult.containsSegregatedNode = %s \n",localResult.containsSegregatedNode ? "true" : "false");
     printf ("   --- localResult.segregateThisNode      = %s \n",localResult.segregateThisNode ? "true" : "false");
#endif

     ROSE_ASSERT(localResult.astNode != NULL);

     return localResult;
   }


int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See Rose::initialize
     ROSE_INITIALIZE;

  // Build the abstract syntax tree
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

#if 0
     printf ("\n\n");
     printf ("################################################ \n");
     printf ("########### START MARKING_UP AST ############### \n");
     printf ("################################################ \n");
     printf ("\n\n");
#endif

  // Build the inherited attribute
     WhiteSpaceInheritedAttribute inheritedAttribute;

  // Define the traversal
     Traversal treeTraversal;

  // Call the traversal starting at the project (root) node of the AST
  // Note that this tool must traverse the whole AST (including all header files).
  // However, we will limit processing to the source files (and header files) 
  // that are within the specified source tree structure.
  // treeTraversal.traverseInputFiles(project,inheritedAttribute);
  // treeTraversal.traverseInputFiles(project,inheritedAttribute);
  // treeTraversal.traverse(project,inheritedAttribute);
     WhiteSpaceSynthesizedAttribute localResult = treeTraversal.traverse(project,inheritedAttribute);

#if 0
     printf ("AFTER TRAVERSAL: localResult.containsSegregatedNode = %s \n",localResult.containsSegregatedNode ? "true" : "false");

     printf ("treeTraversal.intervalList.size() = %zu \n",treeTraversal.intervalList.size());
     for (size_t i = 0; i < treeTraversal.intervalList.size(); i++)
        {
          printf ("intervalList[i]->first = %p second = %p \n",treeTraversal.intervalList[i]->first,treeTraversal.intervalList[i]->second);
        }
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 0
  // Trying to add colors to the AST DOT file generation.
     printf ("treeTraversal.intervalList.size() = %zu \n",treeTraversal.intervalList.size());
     for (size_t i = 0; i < treeTraversal.intervalList.size(); i++)
        {
          printf ("intervalList[i]->first = %p second = %p \n",treeTraversal.intervalList[i]->first,treeTraversal.intervalList[i]->second);

          SgNode* attributeNode = treeTraversal.intervalList[i]->first;
          ROSE_ASSERT(attributeNode != NULL);

          Segregation_Attribute* newAttribute = new Segregation_Attribute();
          attributeNode->addNewAttribute("color",newAttribute);

          AstAttributeMechanism* astAttributeContainer = attributeNode->get_attributeMechanism();
          if (astAttributeContainer != NULL)
             {
               printf ("   --- Found AstAttributeMechanism node \n");

            // Add an AST attribute.
            // Segregation_Attribute* newAttribute = Segregation_Attribute();
            // attributeNode->addNewAttribute("color",newAttribute);
             }
            else
             {
               printf ("   --- Can't find AstAttributeMechanism node \n");
             }
        }
#endif

#if 0
     printf ("treeTraversal.intervalList.size() = %zu \n",treeTraversal.intervalList.size());
#endif
     for (size_t i = 0; i < treeTraversal.intervalList.size(); i++)
        {
#if 0
          printf ("   --- intervalList[i]->first = %p second = %p \n",treeTraversal.intervalList[i]->first,treeTraversal.intervalList[i]->second);
#endif
          SgNode* startingNode = treeTraversal.intervalList[i]->first;
          ROSE_ASSERT(startingNode != NULL);

          SgNode* endingNode = treeTraversal.intervalList[i]->second;
          ROSE_ASSERT(endingNode != NULL);

          SgStatement* startingStatement = isSgStatement(startingNode);
          ROSE_ASSERT(startingStatement != NULL);

          SgStatement* endingStatement = isSgStatement(endingNode);
          ROSE_ASSERT(endingStatement != NULL);
#if 0
          printf ("   --- --- startingStatement = %p = %s = %s \n",startingStatement,startingStatement->class_name().c_str(),SageInterface::get_name(startingStatement).c_str());
          printf ("   --- --- endingStatement   = %p = %s = %s \n",endingStatement,endingStatement->class_name().c_str(),SageInterface::get_name(endingStatement).c_str());
#endif
          string starting_comment = "// START OF INTERVAL";
#if 0
          printf ("   --- --- Adding comment: %s \n",starting_comment.c_str());
#endif
          string ending_comment = "// END OF INTERVAL";
#if 0
          printf ("   --- --- Adding ending comment: %s \n",ending_comment.c_str());
#endif
#if 1
       // SageInterface::addMessageStatement(startingStatement,starting_comment);
       // SageInterface::addMessageStatement(endingStatement,ending_comment);

          int physical_fileName_id = startingStatement->get_file_info()->get_physical_file_id();
          string physical_fileName = startingStatement->get_file_info()->getFilenameFromID(physical_fileName_id);

          string opening_message = "#ifdef SEGREGATE_CODE";
          string closing_message = "#endif";

          PreprocessingInfo* opening_cpp_directive = new PreprocessingInfo(PreprocessingInfo::CpreprocessorIfdefDeclaration,opening_message,physical_fileName,0,0,1,PreprocessingInfo::before);
          PreprocessingInfo* closing_cpp_directive = new PreprocessingInfo(PreprocessingInfo::CpreprocessorIfdefDeclaration,closing_message,physical_fileName,0,0,1,PreprocessingInfo::after);

          startingStatement->addToAttachedPreprocessingInfo(opening_cpp_directive,PreprocessingInfo::before);
          endingStatement->addToAttachedPreprocessingInfo(closing_cpp_directive,PreprocessingInfo::after);
#endif
#if 0
       // Mark the nodes so that the token based unparsing will output updated surrounding whitespace.
       // Marking the containsTransformationToSurroundingWhitespace does not appear to work. It does not 
       // trigger the unparing from the token stream of just the surrounding whitespace of the associated 
       // SgStatement).
          startingStatement->set_containsTransformationToSurroundingWhitespace(true);
          endingStatement->set_containsTransformationToSurroundingWhitespace(true);
#endif
#if 1
       // Mark the nodes so that the token based unparsing will output updated surrounding whitespace.
          startingStatement->setTransformation();
          endingStatement->setTransformation();
#endif

#if 0
       // Mark the nodes so that the token based unparsing will output updated surrounding whitespace.
          startingStatement->get_file_info()->set_isModified(true);
          endingStatement->get_file_info()->set_isModified(true);
#endif

#if 1
       // Mark the nodes so that the token based unparsing will output updated surrounding whitespace.
          startingStatement->set_isModified(true);
          endingStatement->set_isModified(true);
#endif

#if 1
       // Mark the nodes so that the token based unparsing will output updated surrounding whitespace.
          startingStatement->setOutputInCodeGeneration();
          endingStatement->setOutputInCodeGeneration();
#endif
        }

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 0
     printf ("\n\n");
     printf ("############################################### \n");
     printf ("########### DONE MARKING_UP AST ############### \n");
     printf ("############################################### \n");
     printf ("\n\n");
#endif

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
  // generateDOT ( *project );
     generateDOT_withIncludes ( *project );
#endif

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

#if 0
     printf ("\n\n");
     printf ("########################################### \n");
     printf ("########### CALLING BACKEND ############### \n");
     printf ("########################################### \n");
     printf ("\n\n");
#endif

  // This program only does analysis, so it need not call the backend to generate code.
  // return 0;
  // Only output code if there was a transformation that was done.
     return backend(project);

#if 0
     if (treeTraversal.transformed == true)
          return backend(project);
       else
          return 0;
#endif
   }


