#include "sage3basic.h"
#include "fixupInitializers.h"

#include "rose_config.h"

using namespace std;

// void fixupInitializersUsingIncludeFiles( SgNode* node )
void fixupInitializersUsingIncludeFiles( SgProject* node )
   {
  // DQ (3/11/2006): Introduce tracking of performance of ROSE.
     TimingPerformance timer1 ("Fixup known self-referential macros:");

  // This inherited attribute is used for all traversals (within the iterative approach we define)
     FixupInitializersUsingIncludeFilesInheritedAttribute inheritedAttribute;

     FixupInitializersUsingIncludeFilesTraversal astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
#if 0
  // DQ (8/27/2020): This is a more brutal test to make sure that we have 
  // supported the more genereal cases of expresssions and scopes.
     printf ("In fixupInitializersUsingIncludeFiles(): This is for testing only! \n");
     astFixupTraversal.traverse(node,inheritedAttribute);
#else
  // DQ (8/27/2020): We will only want to address files that we will unparse.
     astFixupTraversal.traverseInputFiles(node,inheritedAttribute);
#endif
   }




FixupInitializersUsingIncludeFilesInheritedAttribute::FixupInitializersUsingIncludeFilesInheritedAttribute()
   {
     isInsideVariableDeclaration = false;
     variableDeclaration         = NULL;

     isInsideInitializer         = false;
     initializedName             = NULL;
   }

FixupInitializersUsingIncludeFilesInheritedAttribute::FixupInitializersUsingIncludeFilesInheritedAttribute( const FixupInitializersUsingIncludeFilesInheritedAttribute & X )
   {
     isInsideVariableDeclaration = X.isInsideVariableDeclaration;
     variableDeclaration         = X.variableDeclaration;

     isInsideInitializer         = X.isInsideInitializer;
     initializedName             = X.initializedName;
   }


FixupInitializersUsingIncludeFilesSynthesizedAttribute::FixupInitializersUsingIncludeFilesSynthesizedAttribute()
   {
#if 0
     printf ("In FixupInitializersUsingIncludeFilesSynthesizedAttribute constructor: node = %p = %s \n",node,node->class_name().c_str());
#endif
   }

FixupInitializersUsingIncludeFilesSynthesizedAttribute::FixupInitializersUsingIncludeFilesSynthesizedAttribute( const FixupInitializersUsingIncludeFilesSynthesizedAttribute & X )
   {
   }


FixupInitializersUsingIncludeFilesTraversal::FixupInitializersUsingIncludeFilesTraversal()
   {
#if 0
     printf ("In AddIncludeDirectivesTraversal constructor: include_file->get_filename() = %s \n",include_file->get_filename().str());
#endif

   }


void
FixupInitializersUsingIncludeFilesTraversal::findAndRemoveMatchingInclude(SgStatement* statement, SgExpression* expression, PreprocessingInfo::RelativePositionType location_to_search)
   {
  // DQ (8/27/2020): At least this code can likely be refactored with the code above to a seperate function.
     AttachedPreprocessingInfoType* comments = statement->getAttachedPreprocessingInfo();
     if (comments != NULL)
        {
#if 0
          printf ("In findAndRemoveMatchingInclude(): Found attached comments (at %p of type: %s): name = %s \n",statement,
               statement->class_name().c_str(),SageInterface::get_name(statement).c_str());
#endif
       // DQ (8/27/2020): Save the CPP directive and delete after the processing via the loop to avoid iterator invalidation.
       // std::vector<AttachedPreprocessingInfoType::iterator> includeDirectiveToRemove;
          AttachedPreprocessingInfoType includeDirectiveToRemove;

       // AttachedPreprocessingInfoType::iterator i;
          for (AttachedPreprocessingInfoType::iterator i = comments->begin(); i != comments->end(); i++)
             {
               ROSE_ASSERT((*i) != NULL);
#if 0
               printf ("Attached Comment (relativePosition=%s): %s\n",
                    ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                    (*i)->getString().c_str());
               printf ("Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
#endif
            // if ((*i)->getRelativePosition() == PreprocessingInfo::before)
               if ((*i)->getRelativePosition() == location_to_search)
                  {
                 // These are possible canidates for include directives to remove.
#if 0
                    printf ("Attached Comment (before) (relativePosition=%s): %s\n",
                         ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                         (*i)->getString().c_str());
#endif
                    if ((*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration)
                       {
#if 0
                         printf ("*********** Found include directive (%s, on %p = %s name = %s): %s \n",
                              PreprocessingInfo::relativePositionName(location_to_search).c_str(),statement,
                              statement->class_name().c_str(),SageInterface::get_name(statement).c_str(),(*i)->getString().c_str());
#endif
                      // Get the filename from include directive.
                         std::string filenameFromIncludeDirective = (*i)->get_filename_from_include_directive();
#if 0
                         printf ("filenameFromIncludeDirective = %s \n",filenameFromIncludeDirective.c_str());
#endif
                         string expressionFilename = expression->get_file_info()->get_filename();

                         bool foundCorrectHeaderFileToRemove = false;

                      // Lookup the file (need to search include paths) and search for the 
                      // associated strings used to initialize the variable.
                         if (expressionFilename == "compilerGenerated")
                            {
                           // This is the more complicated case, since we need to read the include file to check 
                           // if the initializers are present. At present, this is a known issue for initializers 
                           // that are strings, because EDG fails to provide source position information for 
                           // string constants.
                              SgStringVal* stringValue = isSgStringVal(expression);

                           // string value_as_string = stringValue.value();
                              SgValueExp* valueExpression = isSgValueExp(expression);
                              if (valueExpression == NULL)
                                {
                                  printf ("Error: valueExpression == NULL: expression = %p = %s name = %s \n",
                                       expression,expression->class_name().c_str(),SageInterface::get_name(expression).c_str());
                                  printf (" --- statement = %p = %s \n",statement,statement->class_name().c_str());
                                  statement->get_file_info()->display("Error: valueExpression == NULL");
                                }
                              ROSE_ASSERT(valueExpression != NULL);

                              string value_as_string = valueExpression->get_constant_folded_value_as_string();
#if 0
                              printf ("value_as_string = %s \n",value_as_string.c_str());
#endif
                              bool knownSourcePositionUnavailable = (stringValue != NULL);
                              if (knownSourcePositionUnavailable == true)
                                 {
#if 0
                                   printf ("Known case of source position unavailable for expression = %p = %s name = %s \n",
                                        expression,expression->class_name().c_str(),SageInterface::get_name(expression).c_str());
#endif
                                // Open the file defined in the include directive.
#if 0
                                   printf ("Open the file defined by: filenameFromIncludeDirective = %s \n",filenameFromIncludeDirective.c_str());
#endif
                                   bool includingSelf = false;
                                   ROSE_ASSERT(statement != NULL);
                                   SgSourceFile* sourceFile = SageInterface::getEnclosingSourceFile(statement,includingSelf);
                                   ROSE_ASSERT(sourceFile != NULL);

                                   std::string source_directory = sourceFile->getSourceDirectory();
#if 0
                                   printf ("source_directory = %s \n",source_directory.c_str());
#endif
                                   std::string expectedSourceFileLocation = source_directory + "/" + filenameFromIncludeDirective;
#if 0
                                   printf ("expectedSourceFileLocation = %s \n",expectedSourceFileLocation.c_str());
#endif
                                   ifstream infile;
                                // infile.open(filenameFromIncludeDirective.c_str());
                                   infile.open(expectedSourceFileLocation.c_str());
                                   if (infile.is_open())
                                      {
                                        std::string file_as_string;

                                        infile.seekg(0, std::ios::end);   
                                        file_as_string.reserve(infile.tellg());
                                        infile.seekg(0, std::ios::beg);

                                        file_as_string.assign((std::istreambuf_iterator<char>(infile)),std::istreambuf_iterator<char>());

                                        infile.close();
#if 0
                                        printf ("file_as_string = \n|%s|\n",file_as_string.c_str());
#endif
                                     // Search file_as_string to find the expression (as text).
                                        size_t position_of_value = file_as_string.find(value_as_string);
                                        if (position_of_value != std::string::npos)
                                           {
                                             foundCorrectHeaderFileToRemove = true;
                                           }
#if 0
                                        printf ("foundCorrectHeaderFileToRemove = %s \n",foundCorrectHeaderFileToRemove ? "true" : "false");
#endif
                                      }
                                     else
                                      {
                                        printf ("Could not file include file at: filenameFromIncludeDirective = %s \n",filenameFromIncludeDirective.c_str());
                                     // list the possible places to search:
                                     // ROSE_DLL_API SgSourceFile* getEnclosingSourceFile(SgNode* n, const bool includingSelf=false);
                                      }
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                                else
                                 {
#if 1
                                   printf ("Note: Source position information is unavailable for this expression = %p = %s name = %s \n",
                                        expression,expression->class_name().c_str(),SageInterface::get_name(expression).c_str());
#endif
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                            }

                      // DQ (8/27/2020): This is a bit more complex than I expected it to be.
                      // printf ("This logic is not quite correct yet \n");
#if 0
                         printf ("expressionFilename = %s \n",expressionFilename.c_str());
                         printf ("filenameFromIncludeDirective = %s \n",filenameFromIncludeDirective.c_str());
#endif
                      // This logic is not quite correct yet, we need to search for the filenameFromIncludeDirective 
                      // in the expressionFilename (and even that might be problematic). Cases such as:
                      // #include "../header.h" are not addressed bu this logic. and there are more complex cases.
                      // ultimately we would have to implement the header file lookup rules using the compiler's logic.
                         size_t includeFilenameLocationInExpressionFilename = expressionFilename.find(filenameFromIncludeDirective);

                      // bool files_match = (includeFilenameLocationInExpressionFilename != string::npos);
                         bool files_match = (includeFilenameLocationInExpressionFilename != string::npos) || (foundCorrectHeaderFileToRemove == true);
#if 0
                         printf ("files_match = %s \n",files_match ? "true" : "false");
#endif
                      // if (filenameFromIncludeDirective == expressionFilename)
                         if (files_match == true)
                            {
                           // We do need to remove the redundant include directive.
#if 0
                              printf ("We do need to remove the redundant include directive \n");
#endif
                              includeDirectiveToRemove.push_back(*i);
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                           else
                            {
                           // Then we don't have to removed the include directive.
                              printf ("Note: Can't match include file to initializer: \n");
                              printf (" --- expressionFilename                          = %s \n",expressionFilename.c_str());
                              printf (" --- filenameFromIncludeDirective                = %s \n",filenameFromIncludeDirective.c_str());
                           // printf (" --- includeFilenameLocationInExpressionFilename = %s \n",includeFilenameLocationInExpressionFilename.c_str());
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }

#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
#if 0
                         if (filenameFromIncludeDirective == "foobar.h")
                            {
                              printf ("Found filename from include directive: filenameFromIncludeDirective = %s \n",filenameFromIncludeDirective.c_str());
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
#endif
                       }
                  }

#if 0
               (*i)->get_file_info()->display("comment/directive location");
#endif
             }

       // AttachedPreprocessingInfoType::iterator i;
          for (AttachedPreprocessingInfoType::iterator i = includeDirectiveToRemove.begin(); i != includeDirectiveToRemove.end(); i++)
             {
#if 0
               printf ("Removing the redundant include directive: *i = %s \n",(*i)->getString().c_str());
#endif
            // AttachedPreprocessingInfoType::iterator location = comments.find(*i);
            // comments.erase(location);
            // comments.erase(std::remove(comments.begin(), comments.end(), *i), comments.end());
            // std::remove(comments.begin(), comments.end(), *i);
               AttachedPreprocessingInfoType::iterator position = std::find(comments->begin(), comments->end(), *i);
               if (position != comments->end())
                  {
                    comments->erase(position);
                  }
             }
#if 0
          printf ("After removing redundent include declaration the comments are: \n");
          SageInterface::printOutComments(statement);
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
#if 0
          printf ("No attached comments (%s) (at %p of type: %s): name = %s \n",PreprocessingInfo::relativePositionName(location_to_search).c_str(),statement,
               statement->class_name().c_str(),SageInterface::get_name(statement).c_str());
#endif
        }
   }


FixupInitializersUsingIncludeFilesInheritedAttribute
FixupInitializersUsingIncludeFilesTraversal::evaluateInheritedAttribute ( SgNode* node, FixupInitializersUsingIncludeFilesInheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("In FixupInitializersUsingIncludeFilesTraversal::evaluateInheritedAttribute: node = %p = %s \n",node,node->class_name().c_str());
#endif

     FixupInitializersUsingIncludeFilesInheritedAttribute return_attribute(inheritedAttribute);

     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(node);
     if (declarationStatement != NULL)
        {
          SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declarationStatement);
          if (variableDeclaration != NULL)
             {
#if 0
               printf ("Found a SgVariableDeclaration: variableDeclaration = %p = %s \n",
                    variableDeclaration,variableDeclaration->class_name().c_str());
#endif
               return_attribute.isInsideVariableDeclaration = true;
               return_attribute.variableDeclaration         = variableDeclaration;
             }
        }

     SgInitializedName* initializedName = isSgInitializedName(node);
     if (inheritedAttribute.isInsideVariableDeclaration == true)
        {
          if (initializedName != NULL)
             {
#if 0
               printf ("Found a target initializedName = %p = %s name = %s \n",initializedName,initializedName->class_name().c_str(),initializedName->get_name().str());
#endif
               SgInitializer* initializer = initializedName->get_initializer();
               if (initializer != NULL)
                  {
#if 0
                    printf ("Found an initializer = %p = %s \n",initializer,initializer->class_name().c_str());
#endif
                    return_attribute.isInsideInitializer = true;
                    return_attribute.initializedName     = initializedName;
                  }
             }
        }

     if (inheritedAttribute.isInsideInitializer == true)
        {
          SgExpression* expression = isSgExpression(node);

       // DQ (8/26/2020): Ignore the initailizer since what we want in at least one level deeper.
       // also the initializer is always compiler generated, so it doesn't tell us anything about 
       // the location of what could come from a header file.
          SgInitializer* initializer = isSgInitializer(expression);

          SgExprListExp* exprListExp = isSgExprListExp(expression);
          SgCastExp*     castExp     = isSgCastExp(expression);

          bool ignoreSomeExpressions = (exprListExp != NULL) || (castExp != NULL);

          if ( (expression != NULL) && (initializer == NULL) && (ignoreSomeExpressions == false) )
             {
               string expressionFilename = expression->get_file_info()->get_filename();
#if 0
               printf ("initalizer expression = %p = %s expressionFilename = %s \n",expression,expression->class_name().c_str(),expressionFilename.c_str());
               printf (" --- expression name = %s \n",SageInterface::get_name(expression).c_str());
#endif
               ROSE_ASSERT(inheritedAttribute.variableDeclaration != NULL);

            // string variableDeclarationFilename = inheritedAttribute.variableDeclaration->get_file_info()->get_filename();
               string variableDeclarationFilename = inheritedAttribute.variableDeclaration->get_file_info()->get_physical_filename();
#if 0
               printf ("variableDeclarationFilename = %s \n",variableDeclarationFilename.c_str());
#endif
            // Note that SgStringVal will not be assigned a source position (it will be marked as: "compilerGenerated".
            // if (expressionFilename != variableDeclarationFilename || expressionFilename == "compilerGenerated")
               if (expressionFilename != variableDeclarationFilename)
                  {
#if 0
                    printf ("######### Found a filename use as a basis for removing an include directive including this file: expressionFilename = %s \n",expressionFilename.c_str());
#endif
                 // Search for the CPP include directive in either the variableDeclaration (attached after the statement) 
                 // or the following statement (attached before the statement).
#if 0
                    SageInterface::printOutComments(inheritedAttribute.variableDeclaration);
#endif
                 // DQ (8/27/2020): At least this code can likely be refactored with the code below to a seperate function.
                    AttachedPreprocessingInfoType* comments = inheritedAttribute.variableDeclaration->getAttachedPreprocessingInfo();
                    if (comments != NULL)
                       {
#if 0
                         printf ("Found attached comments (at %p of type: %s): name = %s \n",
                              inheritedAttribute.variableDeclaration,inheritedAttribute.variableDeclaration->class_name().c_str(),
                              SageInterface::get_name(inheritedAttribute.variableDeclaration).c_str());
#endif
                      // DQ (8/27/2020): Refactored code.
                         findAndRemoveMatchingInclude(inheritedAttribute.variableDeclaration,expression,PreprocessingInfo::after);
                       }
                      else
                       {
#if 0
                         printf ("No attached comments (after) (at %p of type: %s): name = %s \n",inheritedAttribute.variableDeclaration,
                              inheritedAttribute.variableDeclaration->class_name().c_str(),SageInterface::get_name(inheritedAttribute.variableDeclaration).c_str());
#endif
                       }

                    SgStatement* varDecl = inheritedAttribute.variableDeclaration;
                    SgScopeStatement* scope = varDecl->get_scope();

                 // DQ (8/27/2020): Some scopes don't work for the SageInterface::getNextStatement() function.
                    SgForStatement* forStatementScope = isSgForStatement(scope);
                    SgIfStmt*       ifStatementScope = isSgIfStmt(scope);
                    SgRangeBasedForStatement* rangeBasedForStatement = isSgRangeBasedForStatement(scope);

                    bool ignoreThisScope = (forStatementScope != NULL) || (rangeBasedForStatement != NULL) || (ifStatementScope != NULL);

                    if (ignoreThisScope == false)
                       {
#if 0
                         printf ("inheritedAttribute.variableDeclaration = %p = %s name = %s \n",varDecl,varDecl->class_name().c_str(),SageInterface::get_name(varDecl).c_str());
                         printf ("scope = %p = %s \n",scope,scope->class_name().c_str());
                         varDecl->get_file_info()->display("inheritedAttribute.variableDeclaration");
#endif
                         SgStatement* next_statement = SageInterface::getNextStatement(inheritedAttribute.variableDeclaration);
                         if (next_statement == NULL)
                            {
                           // This can happen if there is no next statement (in this scope).
#if 0
                              printf ("Note: should have found include directive attached to the variableDeclaration: initializedName = %s \n",inheritedAttribute.initializedName->get_name().str());
#endif
                            }
                           else
                            {
#if 0
                              printf ("next_statement = %p = %s name = %s \n",next_statement,next_statement->class_name().c_str(),SageInterface::get_name(next_statement).c_str());
#endif
                              findAndRemoveMatchingInclude(next_statement,expression,PreprocessingInfo::before);
                            }
                       }
                  }
             }
        }

     return return_attribute;
   }


FixupInitializersUsingIncludeFilesSynthesizedAttribute
FixupInitializersUsingIncludeFilesTraversal::evaluateSynthesizedAttribute (
     SgNode* node,
     FixupInitializersUsingIncludeFilesInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
#if 0
     printf ("In FixupInitializersUsingIncludeFilesTraversal::evaluateSynthesizedAttribute: node = %p = %s \n",node,node->class_name().c_str());
#endif

     FixupInitializersUsingIncludeFilesSynthesizedAttribute return_attribute;

     return return_attribute;
   }


