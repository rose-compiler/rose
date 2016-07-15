// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.

// #include "nameQuery.h"
#include "rose.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;
using namespace rose;

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class OpenMPInheritedAttribute
   {
     public:
      //! Specific constructors are required
          OpenMPInheritedAttribute () {};
   };

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class OpenMPSynthesizedAttribute : public HighLevelRewrite::SynthesizedAttribute
   {
     public:
          OpenMPSynthesizedAttribute() {};
   };

// tree traversal to test the rewrite mechanism
 /*! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
  */
class OpenMPTraversal
   : public HighLevelRewrite::RewriteTreeTraversal<OpenMPInheritedAttribute,OpenMPSynthesizedAttribute>
   {
     public:
          OpenMPTraversal() {};

       // Functions required by the rewrite mechanism
          OpenMPInheritedAttribute evaluateRewriteInheritedAttribute (
             SgNode* astNode,
             OpenMPInheritedAttribute inheritedAttribute );

          OpenMPSynthesizedAttribute evaluateRewriteSynthesizedAttribute (
             SgNode* astNode,
             OpenMPInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// Functions required by the tree traversal mechanism
OpenMPInheritedAttribute
OpenMPTraversal::evaluateRewriteInheritedAttribute (
     SgNode* astNode,
     OpenMPInheritedAttribute inheritedAttribute )
   {
     return inheritedAttribute;
   }

SgStatement*
getPreviousStatement ( SgStatement *targetStatement )
   {
     SgStatement *previousStatement = rose::getPreviousStatement(targetStatement);
     return previousStatement;
   }

string
buildUniqueFunctionID ()
   {
     return "_0_0";
   }

class OmpUtility
   {
  // This class organizes all variables in a subtree into either shared 
  // or private variables for OpenMP.

     public:
          list<string> privateVariables;
          list<string> sharedVariables;

       // constructor
          OmpUtility ( SgNode* astNode );

          list<string> getPrivateVariables ();
          list<string> getSharedVariables ();

          static bool isOmpParallelFor(SgNode* astNode);

          string generatePrivateVariableDeclaration();
          string generateSharedVariableFunctionParameterDeclarations();
          string generateSharedVariableFunctionParameters();

          string getOmpForLoopIndex();
          string getLowerBound();
          string getUpperBound();
          string getStepSize();
          string getChunkSize();
          string getSchedule();
          string getNumberOfParameters();
   };

bool
OmpUtility::isOmpParallelFor ( SgNode* astNode )
   {
     bool returnValue = false;

  // Now search for the attached pragma if it exists (will be the previous statement)
     SgForStatement *forStatement = isSgForStatement(astNode);
     if (forStatement != NULL)
        {
          SgStatement *previousStatement = getPreviousStatement(forStatement);
          SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(previousStatement);
          if (pragmaDeclaration != NULL)
             {
               string pragmaString = pragmaDeclaration->get_pragma()->get_pragma();
               if( pragmaString.find("omp parallel for",0) != string::npos)
                    returnValue = true;
             }
        }

     return returnValue;
   }

list<string>
OmpUtility::getPrivateVariables ()
   {
     return privateVariables;
   }

list<string>
OmpUtility::getSharedVariables ()
   {
     return sharedVariables;
   }

string
OmpUtility::generatePrivateVariableDeclaration()
   {
  // string used for variable declaration in generated function body
     string returnString = "int _1, _2, _3;";
     return returnString;
   }

string
OmpUtility::generateSharedVariableFunctionParameterDeclarations()
   {
  // string used for variable declaration in generated function declaration
     string returnString = "float* AIJKpointer, int _length1, int _length2, int _size1, int _size2";
     return returnString;
   }

string
OmpUtility::generateSharedVariableFunctionParameters()
   {
  // string used for variable declaration in generated function declaration
     string returnString = "AIJKpointer,_length1,_length2,_size1,_size2";
     return returnString;
   }

string 
OmpUtility::getOmpForLoopIndex()
   {
  // string used for variable declaration in generated function declaration
     string returnString = "_3";
     return returnString;
   }

string
OmpUtility::getLowerBound()
   {
  // string used for variable declaration in generated function declaration
     string returnString = "0";
     return returnString;
   }

string
OmpUtility::getUpperBound()
   {
     string returnString = "100";
     return returnString;
   }

string
OmpUtility::getStepSize()
   {
     string returnString = "10";
     return returnString;
   }

string
OmpUtility::getChunkSize()
   {
     string returnString = "5";
     return returnString;
   }

string
OmpUtility::getSchedule()
   {
  // string used for OpenMP schedules
     string returnString = "DYNAMIC";
     return returnString;
   }

string
OmpUtility::getNumberOfParameters()
   {
  // string used for OpenMP schedules
     string returnString = "5";
     return returnString;
   }

OmpUtility::OmpUtility ( SgNode* astNode )
   {
  // list<string> typeNameStringList = NameQuery::getTypeNamesQuery ( astNode );
     list<string> typeNameStringList = NameQuery::querySubTree ( astNode, NameQuery::VariableTypeNames );

  // There should be at least one type in the statement
     ROSE_ASSERT (typeNameStringList.size() > 0);
  // printf ("typeNameStringList.size() = %zu \n",typeNameStringList.size());

  // printf ("This statement has a dependence upon a variable of some type \n");

  // Loop over all the types and get list of variables of each type
  // (so they can be declared properly when the transformation is compiled)
     list<string>::iterator typeListStringElementIterator;
     for (typeListStringElementIterator = typeNameStringList.begin();
          typeListStringElementIterator != typeNameStringList.end();
          typeListStringElementIterator++)
        {
       // printf ("Type = %s \n",(*typeListStringElementIterator).c_str());

       // list<string> operandNameStringList =
       //      NameQuery::getVariableNamesWithTypeNameQuery
       //         ( astNode, *typeListStringElementIterator );
          list<string> operandNameStringList =
               NameQuery::querySubTree ( 
                    astNode,
                    *typeListStringElementIterator, 
                    NameQuery::VariableNamesWithTypeName );

       // There should be at least one variable of that type in the statement
          ROSE_ASSERT (operandNameStringList.size() > 0);
       // printf ("operandNameStringList.size() = %zu \n",operandNameStringList.size());

       // Loop over all the types and get list of variable of each type
          list<string>::iterator variableListStringElementIterator;
          for (variableListStringElementIterator = operandNameStringList.begin();
               variableListStringElementIterator != operandNameStringList.end();
               variableListStringElementIterator++)
             {
#if 0
               printf ("Type = %s Variable = %s \n",
                    (*typeListStringElementIterator).c_str(),
                    (*variableListStringElementIterator).c_str());
#endif
               string variableName = *variableListStringElementIterator;
               string typeName     = *typeListStringElementIterator;
             }
        }
   }

OpenMPSynthesizedAttribute
OpenMPTraversal::evaluateRewriteSynthesizedAttribute (
     SgNode* astNode, OpenMPInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     OpenMPSynthesizedAttribute returnAttribute;

     if ( OmpUtility::isOmpParallelFor(astNode) )
        {
          SgForStatement *forStatement = isSgForStatement(astNode);
          ROSE_ASSERT (forStatement != NULL);

          string supportingFunction = " \n\
void supportingOpenMPFunction_$ID ( $SHARED_VAR_DECLARATION_LIST ) \n\
   { \n\
     $PRIVATE_VAR_LIST; \n\
     int intone_start, intone_end, intone_last /* ,intone_loopid */; \n\
     Nanos::begin_for($LB,$UB,$STEP,$CHUNKSIZE,$SCHEDULETYPE /* ,intone_loopid */ ); \n\
     while ( Nanos::next_iters(intone_start,intone_end,intone_last)) \n\
        { \n\
          for ($LOOPINDEX = intone_start; $LOOPINDEX <= intone_end; $LOOPINDEX += $STEP) \n\
             { \n\
               $LOOP_BODY; \n\
             } \n\
        } \n\
     Nanos::end_for(true); \n\
   } \n\
\n\
void wrapper_supportingOpenMPFunction_$ID ( int* intone_me_01, int* intone_nprocs_01, int* intone_master01, $SHARED_VAR_DECLARATION_LIST) \n\
   { \n\
     supportingOpenMPFunction_$ID($SHARED_VAR_PARAMETER_LIST); \n\
   } \n\
\n\
";

          string transformationVariables = " \
     int intone_lme_01; \n\
     int intone_master01; \n\
     int intone_me_01; \n\
     int intone_nprocs_01; \n\
     intone_nprocs_01 = Nanos::numberOfCPUs();"; 

          string spawnParallel = " \
     extern void wrapper_supportingOpenMPFunction_$ID(); \n\
     int intone_nargs = $NUMBER_OF_PARAMETERS; \n\
     Nanos::spawn(wrapper_supportingOpenMPFunction_$ID,intone_nargs,intone_nprocs_01,$SHARED_VAR_PARAMETER_LIST); \n\
";

       // Edit the function name and define a unique number as an identifier
          string uniqueID = buildUniqueFunctionID();
          supportingFunction =
             StringUtility::copyEdit(supportingFunction, "$ID",uniqueID);
          spawnParallel = StringUtility::copyEdit( spawnParallel, "$ID",uniqueID);

       // Edit the loop parameters into place
          string loopBody =
             forStatement->get_loop_body()->unparseToString();
          supportingFunction =
             StringUtility::copyEdit(supportingFunction, "$LOOP_BODY",loopBody);

       // similar copyEdits for $LOOPINDEX, $LB, $UB, $STEP, $CHUNKSIZE, $SCHEDULETYPE

       // Edit the OpenMP parameters into place
       // ompParallelFor *ompParallelFor = isOmpParallelFor(pragmaStatement);
       // privateVarList = ompParallelFor->privateVarList();
       // sharedVarList = ompParallelFor->sharedVarList();

          OmpUtility ompData (astNode);
          string privateVarDeclarationList =
             ompData.generatePrivateVariableDeclaration();
          string sharedVarFunctionParameterList =
             ompData.generateSharedVariableFunctionParameters();
          string sharedVarFunctionParameterDeclarationList =
             ompData.generateSharedVariableFunctionParameterDeclarations();

          string loopIndex    = ompData.getOmpForLoopIndex();
          string lowerBound   = ompData.getLowerBound();
          string upperBound   = ompData.getUpperBound();
          string stepSize     = ompData.getStepSize();
          string chunkSize    = ompData.getChunkSize();
          string scheduleType = ompData.getSchedule();

          string numberOfParameters = ompData.getNumberOfParameters();

          supportingFunction =
             StringUtility::copyEdit(supportingFunction,
             "$SHARED_VAR_DECLARATION_LIST",sharedVarFunctionParameterDeclarationList);
          supportingFunction =
             StringUtility::copyEdit(supportingFunction,
             "$SHARED_VAR_PARAMETER_LIST",sharedVarFunctionParameterList);
          spawnParallel = StringUtility::copyEdit(spawnParallel,
             "$SHARED_VAR_PARAMETER_LIST",sharedVarFunctionParameterList);
          supportingFunction =
             StringUtility::copyEdit(supportingFunction,
             "$PRIVATE_VAR_LIST",privateVarDeclarationList);

          supportingFunction = StringUtility::copyEdit(supportingFunction, "$LOOPINDEX",loopIndex);
          supportingFunction = StringUtility::copyEdit(supportingFunction, "$LB",lowerBound);
          supportingFunction = StringUtility::copyEdit(supportingFunction, "$UB",upperBound);
          supportingFunction = StringUtility::copyEdit(supportingFunction, "$STEP",stepSize);
          supportingFunction = StringUtility::copyEdit(supportingFunction, "$CHUNKSIZE",chunkSize);
          supportingFunction = StringUtility::copyEdit(supportingFunction, "$SCHEDULETYPE",scheduleType);

          spawnParallel = StringUtility::copyEdit(spawnParallel, "$NUMBER_OF_PARAMETERS",numberOfParameters);

          returnAttribute.insert( forStatement,
                         "#include \"nanos.h\"",
                         OpenMPSynthesizedAttribute::GlobalScope,
                         OpenMPSynthesizedAttribute::TopOfScope);

          returnAttribute.insert( forStatement,
                         supportingFunction,
                         OpenMPSynthesizedAttribute::GlobalScope,
                         OpenMPSynthesizedAttribute::BeforeCurrentPosition);

          returnAttribute.insert( forStatement,
                         transformationVariables,
                         OpenMPSynthesizedAttribute::SurroundingScope,
                         OpenMPSynthesizedAttribute::TopOfScope);

          returnAttribute.insert( forStatement,
                         spawnParallel,
                         OpenMPSynthesizedAttribute::SurroundingScope,
                         OpenMPSynthesizedAttribute::ReplaceCurrentPosition);
        }

     return returnAttribute;
   }

int
main ( int argc, char** argv )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

     SgProject* project = frontend(argc,argv);

     printf ("Generate the pdf output of the SAGE III AST \n");
     generatePDF ( *project );

     OpenMPTraversal treeTraversal;

     OpenMPInheritedAttribute inheritedAttribute;

  // Ignore the return value since we don't need it
     treeTraversal.traverseInputFiles(project,inheritedAttribute);

     return backend(project);
  // return backend(frontend(argc,argv));
   }













