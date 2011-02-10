// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"
#include "grammarString.h"
#include <sstream>

using namespace std;

void
Grammar::markNodeForConstructorWithoutSourcePositionInformation ( Terminal & node )
   {
  // DQ (11/7/2006): Where we find the SgLocatedNode mark a data member so that we can use the
  // same code generation mechanisms to generate different constructors.

  // string dataMemberSpecificString = node.token->buildReturnDataMemberPointers();
     string nodeName = node.name;

  // printf ("In markNodeForConstructorWithoutSourcePositionInformation(): nodeName = %s \n",nodeName.c_str());

     if (nodeName == "SgLocatedNode")
        {
       // printf ("Found SgLocatedNode, look for the Sg_File_Info object startOfConstructor \n");
#if 0
          const list<GrammarString*> & terminalList = node.token->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
          list<GrammarString*>::const_iterator i = terminalList.begin();
          while (i != terminalList.end())
             {
               printf ("GrammarString i = %s \n",(*i)->variableNameString);
               string variableName = (*i)->variableNameString;
               if (variableName == "startOfConstruct")
                  {
                    printf ("Resetting from (*i)->isInConstructorParameterList = %d to NO_CONSTRUCTOR_PARAMETER (=0)\n",(*i)->isInConstructorParameterList);
                    (*i)->isInConstructorParameterList = NO_CONSTRUCTOR_PARAMETER;
                  }
               i++;
             }
#else
          GrammarString* returnValue = getNamedDataMember ( node, "startOfConstruct" );
          returnValue->isInConstructorParameterList = NO_CONSTRUCTOR_PARAMETER;
#endif
       // ROSE_ASSERT(false);
        }
   }

void
Grammar::markNodeForConstructorWithoutSourcePositionInformationSupport( Terminal & node )
   {
     markNodeForConstructorWithoutSourcePositionInformation(node);

#if 1
  // Call this function recursively on the children of this node in the tree
     vector<Terminal *>::const_iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin();
          treeNodeIterator != node.subclasses.end();
          treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          markNodeForConstructorWithoutSourcePositionInformationSupport(**treeNodeIterator);
        }
#endif
   }

GrammarString* 
Grammar::getNamedDataMember ( Terminal & node, const string & name )
   {
     GrammarString* returnValue = NULL;
     const vector<GrammarString*> & terminalList = node.getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
     vector<GrammarString*>::const_iterator i = terminalList.begin();
     while (i != terminalList.end())
        {
       // printf ("GrammarString i = %s \n",(*i)->variableNameString);
          string variableName = (*i)->variableNameString;
          if (variableName == "startOfConstruct")
             {
            // printf ("Resetting from (*i)->isInConstructorParameterList = %d to NO_CONSTRUCTOR_PARAMETER (=0)\n",(*i)->isInConstructorParameterList);
            // (*i)->isInConstructorParameterList = NO_CONSTRUCTOR_PARAMETER;
               returnValue = *i;
             }
          i++;
        }

     return returnValue;
   }

Terminal* 
Grammar::getNamedNode ( Terminal & node, const string & name )
   {
  // This function only operates on the parent chain.

  // We only want to output non-source-position dependent constructors for SgLocatedNodes. Test for this.
     Terminal* parentNode = node.getBaseClass();
#if 0
     string parentNodeName;
     if (parentNode != NULL)
        {
          parentNodeName = parentNode->getName();
       // printf ("parentNodeName = %s \n",parentNodeName.c_str());
        }
  // printf ("In Grammar::getNamedNode(): name = %s \n",name.c_str());
#endif

     while ( parentNode != NULL && string(parentNode->getName()) != name )
  // while ( (parentNode != NULL) && (parentNodeName != name) )
        {
       // printf ("node %s parent node: %s \n",name.c_str(),parentNode->getName().c_str());
          parentNode = parentNode->getBaseClass();
        }

     if (parentNode != NULL)
        {
       // printf ("Found node %s parent node: %s \n",name.c_str(),parentNode->getName().c_str());
        }

     return parentNode;
   }



StringUtility::FileWithLineNumbers
Grammar::buildConstructorWithoutSourcePositionInformation ( Terminal & node )
   {
  // DQ (11/6/2006): This function generates the code for a newer form of the constructor 
  // that has not source position information in it's parameter list.  This new form of the constructor
  // is easier to work with and separates the details of maintaining the source position information from
  // the construction of the IR node within the AST.


  // Build the constructors for each class
  // Example:
  // /* this is the generated constructor */
  // ClassDeclaration::ClassDeclaration 
  //    ( Name name, int class_type, ClassType* type, ClassDefinition* definition)
  //    : DeclarationStatement(NULL)
  //    {
  //      p_name = name;
  //      p_class_type = class_type;
  //      p_type = type;
  //      p_definition = definition;
  //   /* now a call to the user defined intialization function */
  //      post_construction_initialization();
  //    }

     string className = node.getName();

     StringUtility::FileWithLineNumbers returnString;


  // We only want to output non-source-position dependent constructors for SgLocatedNodes. Test for this.
     Terminal* parentNode = getNamedNode ( node, "SgLocatedNode" );

  // We only want to output non-source-position dependent constructors for SgLocatedNodes.
     if (parentNode != NULL && node.generateConstructor() == true)
        {
          string constructorTemplateFileName = "../Grammar/grammarConstructorDefinitionMacros.macro";
          StringUtility::FileWithLineNumbers constructorSourceCodeTemplate = Grammar::readFileWithPos (constructorTemplateFileName);

          bool complete  = false;
          ConstructParamEnum config = CONSTRUCTOR_PARAMETER;
          if  (node.getBuildDefaultConstructor())
             {
               config = NO_CONSTRUCTOR_PARAMETER;
             }

          StringUtility::FileWithLineNumbers constructorSource = constructorSourceCodeTemplate;
          if (node.getBaseClass() != NULL)
             {
               string parentClassName = node.getBaseClass()->getName();
            // printf ("In Grammar::buildConstructor(): parentClassName = %s \n",parentClassName);
            // printf ("Calling base class default constructor (should call paramtererized version) \n");

               string baseClassParameterString = "";
               bool withInitializers = false;
               bool withTypes        = false;
               baseClassParameterString = buildConstructorParameterListString (*node.getBaseClass(),withInitializers,withTypes, config);
               string preInitializationString = ": " + parentClassName + "($BASECLASS_PARAMETERS)";
               preInitializationString = StringUtility::copyEdit (preInitializationString,"$BASECLASS_PARAMETERS",baseClassParameterString);
               constructorSource = StringUtility::copyEdit (constructorSource,"$PRE_INITIALIZATION_LIST",preInitializationString);
             }
            else
             {
               constructorSource = StringUtility::copyEdit (constructorSource,"$PRE_INITIALIZATION_LIST","");
             }

          bool withInitializers         = false;
          bool withTypes                = true;
          string constructorParameterString = buildConstructorParameterListString (node,withInitializers,withTypes,config,&complete);
          constructorSource = StringUtility::copyEdit (constructorSource,"$CONSTRUCTOR_PARAMETER_LIST",constructorParameterString);
          constructorSource = StringUtility::copyEdit (constructorSource,"$CLASSNAME",className);

          if (config == NO_CONSTRUCTOR_PARAMETER)
             {
               constructorSource = StringUtility::copyEdit (constructorSource,"$CONSTRUCTOR_BODY","");
             }
            else
             {
               string constructorFunctionBody = node.buildConstructorBody(withInitializers, config);
               constructorSource = StringUtility::copyEdit (constructorSource,"$CONSTRUCTOR_BODY",constructorFunctionBody);
             }

          returnString.insert(returnString.end(), constructorSource.begin(), constructorSource.end());
        }

     returnString = StringUtility::copyEdit(returnString,"$GRAMMAR_PREFIX_","Sg");

     return returnString;
   }

void
Grammar::buildConstructorWithoutSourcePositionInformationSupport( Terminal & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     StringUtility::FileWithLineNumbers editString = buildConstructorWithoutSourcePositionInformation(node);

     editString = StringUtility::copyEdit (editString,"$CLASSNAME",node.getName());
     editString = StringUtility::copyEdit (editString,"$GRAMMAR_NAME",getGrammarName());  // grammarName string defined in Grammar class
  // Set these to NULL strings if they are still present within the string
     editString = StringUtility::copyEdit (editString,"$CLASSTAG",node.getTagName());

#if 1
  // Also output strings to single file
     outputFile += editString;
#endif

  // printf ("node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName(),node.nodeList.size());

#if 1
  // Call this function recursively on the children of this node in the tree
     vector<Terminal *>::const_iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin();
          treeNodeIterator != node.subclasses.end();
          treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          buildConstructorWithoutSourcePositionInformationSupport(**treeNodeIterator,outputFile);
        }
#endif
   }

