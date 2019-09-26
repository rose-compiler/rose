// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

using namespace std;

// Class declaration
class visitorTraversal : public AstSimpleProcessing
   {
     private:
          ofstream & dataFile;
          string filename;

     public:
          visitorTraversal (ofstream & inputDataFile ) : dataFile(inputDataFile) {}
          virtual void visit(SgNode* n);
   };



string
directiveTypeColor ( const PreprocessingInfo::DirectiveType & directive )
   {
  // Let comments and CPP directives be shades of green.

     string returnString;
     switch (directive)
        {
          case PreprocessingInfo::C_StyleComment:
          case PreprocessingInfo::CplusplusStyleComment:
             returnString = "lightgreen";
             break;
          case PreprocessingInfo::CpreprocessorIncludeDeclaration:
          case PreprocessingInfo::CpreprocessorDefineDeclaration:
          case PreprocessingInfo::CpreprocessorUndefDeclaration:
          case PreprocessingInfo::CpreprocessorIfdefDeclaration:
          case PreprocessingInfo::CpreprocessorElseDeclaration:
          case PreprocessingInfo::CpreprocessorElifDeclaration:
          case PreprocessingInfo::CpreprocessorIfndefDeclaration:
          case PreprocessingInfo::CpreprocessorIfDeclaration:
          case PreprocessingInfo::CpreprocessorEndifDeclaration:
          case PreprocessingInfo::CpreprocessorLineDeclaration:
          case PreprocessingInfo::ClinkageSpecificationStart:
          case PreprocessingInfo::ClinkageSpecificationEnd:
          case PreprocessingInfo::CpreprocessorErrorDeclaration:
          case PreprocessingInfo::CpreprocessorWarningDeclaration:
             returnString = "green";
             break;
          case PreprocessingInfo::CpreprocessorEmptyDeclaration:
          case PreprocessingInfo::CSkippedToken:
          case PreprocessingInfo::CMacroCall:
          case PreprocessingInfo::LineReplacement:
             returnString = "olivegreen";
             break;

          case PreprocessingInfo::CpreprocessorUnknownDeclaration:
          default:
             returnString = "ERROR DEFAULT REACHED";
             printf ("Default reached in directiveTypeColor() exiting ... (%s) \n",PreprocessingInfo::directiveTypeName(directive).c_str());
             ROSE_ASSERT(false);
             break;
        }

     return returnString;
   }

string
nodeColor( SgStatement* statement )
   {
  /* color: colorCode:red:on
     color: colorCode:orange:on 
     color: colorCode:yellow:on 
     color: colorCode:blue:on 
     color: colorCode:green:on 
     color: colorCode:violet:on 
     color: colorCode:brown:on 
     color: colorCode:purple:on 
     color: colorCode:lightblue:on 
     color: colorCode:lightgreen:on 
     color: colorCode:lightred:on 
     color: colorCode:black:on 
     color: colorCode:darkblue:on 
     color: colorCode:grey:on 
     color: colorCode:darkgrey:on 
     color: colorCode:olivegreen:on 
     color: colorCode:darkgreen:on 
  */
     string returnString;

     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(statement);
     if (declarationStatement != NULL)
        {
          switch (declarationStatement->variantT())
             {
               case V_SgFunctionDeclaration:
               case V_SgMemberFunctionDeclaration:
               case V_SgTemplateInstantiationFunctionDecl:
               case V_SgTemplateInstantiationMemberFunctionDecl:
                    returnString = "orange";
                    break;

               case V_SgClassDeclaration:
               case V_SgTemplateInstantiationDecl:
                    returnString = "yellow";
                    break;

               case V_SgAsmStmt:
               case V_SgCtorInitializerList:
               case V_SgEnumDeclaration:
               case V_SgFunctionParameterList:
               case V_SgNamespaceAliasDeclarationStatement:
               case V_SgNamespaceDeclarationStatement:
               case V_SgPragmaDeclaration:
               case V_SgTemplateDeclaration:
               case V_SgTemplateInstantiationDirectiveStatement:
               case V_SgTypedefDeclaration:
               case V_SgUsingDeclarationStatement:
               case V_SgUsingDirectiveStatement:
               case V_SgVariableDeclaration:
               case V_SgVariableDefinition:
                    returnString = "lightred";
                    break;

            // DQ (11/11/2012): Added support for newer IR nodes in edg4x work.
               case V_SgTemplateMemberFunctionDeclaration:
               case V_SgTemplateClassDeclaration:
               case V_SgTemplateFunctionDeclaration:
               case V_SgTemplateVariableDeclaration:
                    returnString = "red";
                    break;

               default:
                    returnString = "ERROR DEFAULT REACHED";
                    printf ("Default reached in nodeColor() exiting ... (%s) \n",declarationStatement->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
             }
        }

     SgScopeStatement* scopeStatement = isSgScopeStatement(statement);
     if (scopeStatement != NULL)
        {
          switch (scopeStatement->variantT())
             {
               case V_SgBasicBlock:
                    returnString = "lightblue";
                    break;

               case V_SgClassDefinition:
                    returnString = "lightblue";
                    break;

               case V_SgTemplateInstantiationDefn:
               case V_SgFunctionDefinition:
                    returnString = "lightblue";
                    break;

               case V_SgWhileStmt:
               case V_SgDoWhileStmt:
               case V_SgForStatement:
                    returnString = "darkblue";
                    break;

               case V_SgGlobal:
               case V_SgIfStmt:
               case V_SgNamespaceDefinitionStatement:
               case V_SgSwitchStatement:
               case V_SgCatchOptionStmt:
                    returnString = "black";
                    break;

            // DQ (11/11/2012): Added support for newer IR nodes in edg4x work.
               case V_SgTemplateClassDefinition:
               case V_SgTemplateFunctionDefinition:
                    returnString = "red";
                    break;

               default:
                    returnString = "ERROR DEFAULT REACHED";
                    printf ("Default reached in nodeColor() exiting ... (%s) \n",scopeStatement->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
             }
        }

     if (scopeStatement == NULL && declarationStatement == NULL)
        {
          switch (statement->variantT())
             {
               case V_SgExprStatement:
                    returnString = "violet";
                    break;

          case V_SgBreakStmt:
          case V_SgCaseOptionStmt:
          case V_SgCatchStatementSeq:
          case V_SgContinueStmt:
          case V_SgDefaultOptionStmt:
          case V_SgClinkageStartStatement:
          case V_SgForInitStatement:
          case V_SgFunctionTypeTable:
          case V_SgGotoStatement:
          case V_SgLabelStatement:
          case V_SgNullStatement:
          case V_SgReturnStmt:
          case V_SgSpawnStmt:
          case V_SgTryStmt:
          case V_SgVariantStatement:
               returnString = "brown";
               break;

          default:
               returnString = "ERROR DEFAULT REACHED";
               printf ("Default reached in nodeColor() exiting ... (%s) \n",statement->class_name().c_str());
               ROSE_ASSERT(false);
               break;
        }
        }

     return returnString;
   }

string
nodeColor( SgExpression* expression )
   {
  /* color: colorCode:red:on
     color: colorCode:orange:on 
     color: colorCode:yellow:on 
     color: colorCode:blue:on 
     color: colorCode:green:on 
     color: colorCode:violet:on 
     color: colorCode:brown:on 
     color: colorCode:purple:on 
     color: colorCode:lightblue:on 
     color: colorCode:lightgreen:on 
     color: colorCode:lightred:on 
     color: colorCode:black:on 
     color: colorCode:darkblue:on 
     color: colorCode:grey:on 
     color: colorCode:darkgrey:on 
     color: colorCode:olivegreen:on 
     color: colorCode:darkgreen:on 
  */
     string returnString;

     SgBinaryOp* binaryOperator = isSgBinaryOp(expression);
     if (binaryOperator != NULL)
        {
          switch (binaryOperator->variantT())
             {
               case V_SgAddOp:
               case V_SgAndAssignOp:
               case V_SgAndOp:
               case V_SgArrowExp:
               case V_SgArrowStarOp:
               case V_SgAssignOp:
               case V_SgBitAndOp:
               case V_SgBitOrOp:
               case V_SgBitXorOp:
               case V_SgCommaOpExp:
               case V_SgDivAssignOp:
               case V_SgDivideOp:
               case V_SgDotExp:
               case V_SgDotStarOp:
               case V_SgMinusAssignOp:
               case V_SgModAssignOp:
               case V_SgModOp:
               case V_SgMultAssignOp:
               case V_SgMultiplyOp:
               case V_SgNotEqualOp:
               case V_SgOrOp:
               case V_SgPlusAssignOp:
               case V_SgPntrArrRefExp:
               case V_SgScopeOp:
               case V_SgSubtractOp:
               case V_SgXorAssignOp:
                    returnString = "orange";
                    break;

               case V_SgEqualityOp:
               case V_SgGreaterOrEqualOp:
               case V_SgGreaterThanOp:
               case V_SgIntegerDivideOp:
               case V_SgIorAssignOp:
               case V_SgLessOrEqualOp:
               case V_SgLessThanOp:
                    returnString = "yellow";
                    break;

               case V_SgLshiftAssignOp:
               case V_SgLshiftOp:
               case V_SgRshiftAssignOp:
               case V_SgRshiftOp:
                    returnString = "lightred";
                    break;

               default:
                    returnString = "ERROR DEFAULT REACHED";
                    printf ("Default reached in nodeColor() exiting ... (%s) \n",binaryOperator->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
             }
        }

     SgUnaryOp* unaryOperator = isSgUnaryOp(expression);
     if (unaryOperator != NULL)
        {
          switch (unaryOperator->variantT())
             {
               case V_SgAddressOfOp:
               case V_SgBitComplementOp:
               case V_SgPointerDerefExp:
               case V_SgThrowOp:
               case V_SgUnaryAddOp:
                    returnString = "lightblue";
                    break;

               case V_SgMinusMinusOp:
               case V_SgMinusOp:
               case V_SgNotOp:
               case V_SgPlusPlusOp:
                    returnString = "darkblue";
                    break;

               case V_SgCastExp:
               case V_SgExpressionRoot:
                    returnString = "black";
                    break;

               default:
                    returnString = "ERROR DEFAULT REACHED";
                    printf ("Default reached in nodeColor() exiting ... (%s) \n",unaryOperator->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
             }
        }

     SgInitializer* initializer = isSgInitializer(expression);
     if (initializer != NULL)
        {
          switch (initializer->variantT())
             {
               case V_SgAggregateInitializer:
               case V_SgAssignInitializer:
               case V_SgConstructorInitializer:
                    returnString = "lightblue";
                    break;

               default:
                    returnString = "ERROR DEFAULT REACHED";
                    printf ("Default reached in nodeColor() exiting ... (%s) \n",initializer->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
             }
        }

     SgValueExp* valueExpression = isSgValueExp(expression);
     if (valueExpression != NULL)
        {
          switch (valueExpression->variantT())
             {
               case V_SgComplexVal:
               case V_SgIntVal:
               case V_SgLongIntVal:
               case V_SgLongLongIntVal:
               case V_SgShortVal:
               case V_SgUnsignedCharVal:
               case V_SgUnsignedIntVal:
               case V_SgUnsignedLongLongIntVal:
               case V_SgUnsignedLongVal:
               case V_SgUnsignedShortVal:
                    returnString = "lightblue";
                    break;

               case V_SgFloatVal:
               case V_SgDoubleVal:
               case V_SgLongDoubleVal:
                    returnString = "darkblue";
                    break;

               case V_SgEnumVal:
               case V_SgBoolValExp:
               case V_SgCharVal:
               case V_SgWcharVal:
               case V_SgStringVal:
                    returnString = "black";
                    break;

            // DQ (11/11/2012): Added support for newer IR nodes in edg4x work.
               case V_SgTemplateParameterVal:
                    returnString = "red";
                    break;

               default:
                    returnString = "ERROR DEFAULT REACHED";
                    printf ("Default reached in nodeColor() exiting ... (%s) \n",valueExpression->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
             }
        }

     if (binaryOperator == NULL && unaryOperator == NULL && initializer == NULL && valueExpression == NULL)
        {
          switch (expression->variantT())
             {
               case V_SgFunctionCallExp:
               case V_SgFunctionRefExp:
               case V_SgMemberFunctionRefExp:
               case V_SgPseudoDestructorRefExp:
                    returnString = "violet";
                    break;

               case V_SgAsmOp:
               case V_SgClassNameRefExp:
               case V_SgConditionalExp:
               case V_SgDeleteExp:
               case V_SgExprListExp:
               case V_SgNewExp:
               case V_SgNullExpression:
               case V_SgRefExp:
               case V_SgSizeOfOp:
               case V_SgStatementExpression:
               case V_SgThisExp:
               case V_SgTypeIdOp:
               case V_SgVarArgCopyOp:
               case V_SgVarArgEndOp:
               case V_SgVarArgOp:
               case V_SgVarArgStartOneOperandOp:
               case V_SgVarArgStartOp:
               case V_SgVariantExpression:
               case V_SgVarRefExp:
                    returnString = "brown";
                    break;

            // DQ (1/23/2013): Added support for newer IR nodes in edg4x work.
               case V_SgTemplateMemberFunctionRefExp:
               case V_SgTemplateFunctionRefExp:
               case V_SgNonrealRefExp:
                    returnString = "brown";
                    break;

               default:
                    returnString = "ERROR DEFAULT REACHED";
                    printf ("Default reached in nodeColor() exiting ... (%s) \n",expression->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
             }
        }

     return returnString;
   }

string
nodeColor( SgInitializedName* initializedName )
   {
     string returnString = "orange";
     return returnString;
   }


void
visitorTraversal::visit(SgNode* n)
   {
     SgFile* file = isSgFile(n);
     if (file != NULL)
        {
          filename = file->get_sourceFileNameWithPath();
        }

  // On each statement node and output it's position.
     SgStatement* statement = isSgStatement(n);

     bool outputStatement = (statement != NULL) ? true : false;

  // Check for the statement to exist in the input source file
     outputStatement = outputStatement && (statement->get_file_info()->get_filenameString() == filename);

  // Skip SgGlobal IR nodes
     outputStatement = outputStatement && (isSgGlobal(statement) == NULL);

     if (outputStatement == true)
        {
          AttachedPreprocessingInfoType* comments = statement->getAttachedPreprocessingInfo();

          if (comments != NULL)
             {
            // printf ("Found attached comments (to IR node at %p of type: %s): \n",statement,statement->class_name().c_str());
            // int counter = 0;
               AttachedPreprocessingInfoType::iterator i;
               for (i = comments->begin(); i != comments->end(); i++)
                  {
#if 0
                    printf ("          Attached Comment #%d in file %s (relativePosition=%s): classification %s :\n%s\n",
                         counter++,(*i)->get_file_info()->get_filenameString().c_str(),
                         ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                         PreprocessingInfo::directiveTypeName((*i)->getTypeOfDirective()).c_str(),
                         (*i)->getString().c_str());
#endif

                 // Mark comments and CPP directives a few different colors.
                    int startingLineNumber   = (*i)->get_file_info()->get_line();
                    int startingColumnNumber = (*i)->get_file_info()->get_col();

                 // Subtract 1 from number of lines to avoid over counting the current line.
                    int endingLineNumber = startingLineNumber + ((*i)->getNumberOfLines() - 1);
                    int endingColumnNumber = (*i)->getColumnNumberOfEndOfString();

                    string color = directiveTypeColor((*i)->getTypeOfDirective());
#if 0
                    printf ("%d,%d,%s,%d,%d\n",startingLineNumber,startingColumnNumber,color.c_str(),endingLineNumber,endingColumnNumber);
#endif
                    dataFile << startingLineNumber << "," << startingColumnNumber << "," << color << "," << endingLineNumber << "," << endingColumnNumber << endl;
                  }
             }
            else
             {
            // printf ("No attached comments (at %p of type: %s): \n",statement,statement->sage_class_name());
             }

          ROSE_ASSERT(statement->get_startOfConstruct() != NULL);
          int startingLineNumber   = statement->get_startOfConstruct()->get_line();
          int startingColumnNumber = statement->get_startOfConstruct()->get_col();
          if (statement->get_endOfConstruct() == NULL)
             {
               printf ("Error: statement->get_endOfConstruct() == NULL (statement = %p = %s) \n",statement,statement->class_name().c_str());
             }
          ROSE_ASSERT(statement->get_endOfConstruct() != NULL);
          int endingLineNumber     = statement->get_endOfConstruct()->get_line();
          int endingColumnNumber   = statement->get_endOfConstruct()->get_col();

       // Catch errors (likely compiler generate IR node or NULL file)
          if (endingLineNumber == 0)
             {
               endingLineNumber   = startingLineNumber;
               endingColumnNumber = startingColumnNumber;
             }
#if 0
       // Mark all statements blue
          string color = "blue";
          if (isSgScopeStatement(statement) != NULL)
             color = "red";
#else
          string color = nodeColor(statement);
#endif
#if 0
          printf ("%d,%d,%s,%d,%d  %s = %p \n",startingLineNumber,startingColumnNumber,color.c_str(),endingLineNumber,endingColumnNumber,statement->class_name().c_str(),statement);
#endif
          dataFile << startingLineNumber << "," << startingColumnNumber << "," << color << "," << endingLineNumber << "," << endingColumnNumber << endl;

        }


  // On each statement node and output it's position.
     SgExpression* expression = isSgExpression(n);

     bool outputExpression = (expression != NULL) ? true : false;

  // Check for the statement to exist in the input source file
     outputExpression = outputExpression && (expression->get_file_info()->get_filenameString() == filename);

     if (outputExpression == true)
        {
       // Currently we don't attach comments to expressions (as I recall).
          AttachedPreprocessingInfoType* comments = expression->getAttachedPreprocessingInfo();
          ROSE_ASSERT(comments == NULL);

          ROSE_ASSERT(expression->get_startOfConstruct() != NULL);
          int startingLineNumber   = expression->get_startOfConstruct()->get_line();
          int startingColumnNumber = expression->get_startOfConstruct()->get_col();

       // For expressions I would like to be a bit more tollerant of a few mistakes.
          if (expression->get_endOfConstruct() == NULL)
             {
               printf ("Error: expression->get_endOfConstruct() == NULL (expression = %p = %s) \n",expression,expression->class_name().c_str());
             }
       // ROSE_ASSERT(expression->get_endOfConstruct() != NULL);
          int endingLineNumber     = startingLineNumber;
          int endingColumnNumber   = startingColumnNumber;
          if (expression->get_endOfConstruct() != NULL)
             {
               endingLineNumber     = expression->get_endOfConstruct()->get_line();
               endingColumnNumber   = expression->get_endOfConstruct()->get_col();
             }

       // Catch errors (likely compiler generate IR node or NULL file)
          if (endingLineNumber == 0)
             {
               endingLineNumber   = startingLineNumber;
               endingColumnNumber = startingColumnNumber;
             }

          string color = nodeColor(expression);
#if 0
          printf ("%d,%d,%s,%d,%d  %s = %p \n",startingLineNumber,startingColumnNumber,color.c_str(),endingLineNumber,endingColumnNumber,expression->class_name().c_str(),expression);
#endif
          dataFile << startingLineNumber << "," << startingColumnNumber << "," << color << "," << endingLineNumber << "," << endingColumnNumber << endl;

        }

  // On each statement node and output it's position.
     SgInitializedName* initializedName = isSgInitializedName(n);

     bool outputInitializedName = (initializedName != NULL) ? true : false;

  // Check for the statement to exist in the input source file
     outputInitializedName = outputInitializedName && (initializedName->get_file_info()->get_filenameString() == filename);

     if (outputInitializedName == true)
        {
       // Currently we don't attach comments to SgInitializedName IR nodes (as I recall).
       // AttachedPreprocessingInfoType* comments = initializedName->getAttachedPreprocessingInfo();
       // ROSE_ASSERT(comments == NULL);

          ROSE_ASSERT(initializedName->get_startOfConstruct() != NULL);
          int startingLineNumber   = initializedName->get_startOfConstruct()->get_line();
          int startingColumnNumber = initializedName->get_startOfConstruct()->get_col();
#if 0
       // For SgInitializedName I would like to be a bit more tollerant of a few mistakes.
          if (initializedName->get_endOfConstruct() == NULL)
             {
               printf ("Note: initializedName->get_endOfConstruct() == NULL is OK (initializedName = %p = %s) \n",initializedName,initializedName->class_name().c_str());
             }
       // ROSE_ASSERT(initializedName->get_endOfConstruct() != NULL);
#endif
          int endingLineNumber     = startingLineNumber;
          int endingColumnNumber   = startingColumnNumber;
          if (initializedName->get_endOfConstruct() != NULL)
             {
               endingLineNumber     = initializedName->get_endOfConstruct()->get_line();
               endingColumnNumber   = initializedName->get_endOfConstruct()->get_col();
             }

       // Catch errors (likely compiler generate IR node or NULL file)
          if (endingLineNumber == 0)
             {
               endingLineNumber   = startingLineNumber;
               endingColumnNumber = startingColumnNumber;
             }

          string color = nodeColor(initializedName);
#if 0
       // This is redundant I/O for debugging.
          printf ("%d,%d,%s,%d,%d  %s = %p \n",startingLineNumber,startingColumnNumber,color.c_str(),endingLineNumber,endingColumnNumber,initializedName->class_name().c_str(),initializedName);
#endif
          dataFile << startingLineNumber << "," << startingColumnNumber << "," << color << "," << endingLineNumber << "," << endingColumnNumber << endl;

        }
   }


void
outputStatementPositionInformation ( SgFile* file )
   {
  // This will likely cause the "<filename<.highlight" file to be put into the source tree!
  // This is likely OK for now!
     string highlightFilename = file->get_sourceFileNameWithPath();
     highlightFilename += ".highlight";

  // printf ("highlightFilename = %s \n",highlightFilename.c_str());

  // ofstream datafile ( highlightFilename.c_str() , ios::out | ios::app );
     ofstream datafile ( highlightFilename.c_str() , ios::out );
     if ( datafile.good() == false )
        {
          printf ("File failed to open \n");
        }

  // Build the traversal object
     visitorTraversal exampleTraversal(datafile);

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverse(file,preorder);

     datafile.close();
   }

void
outputStatementPositionInformation ( SgProject* project )
   {
     const SgFilePtrList& fileList = project->get_fileList();
     for (int i = 0; i < project->numberOfFiles(); i++)
        {
          SgFile* file = fileList[i];
          outputStatementPositionInformation ( file );
        }
   }


int main( int argc, char * argv[] )
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

#if 0
  // This fails for test2005_63.C but Rich has fixed this
  // by updating the pdf library we are using within ROSE.
     printf ("Generate the pdf output of the SAGE III AST \n");
     generatePDF ( *project );
#endif

#if 0
     printf ("In main(): Generate the dot output of the SAGE III AST \n");
  // generateDOT ( *project );

     AstDOTGeneration astdotgen;
  // SgProject & nonconstProject = (SgProject &) project;
     astdotgen.generateInputFiles(project);
  // astdotgen.generate(project);
#endif

     outputStatementPositionInformation(project);

     return backend(project);
   }

