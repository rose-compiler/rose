#include "sage3basic.h"
#include "fixupInitializers.h"
#include "rose_config.h"

using namespace std;

void fixupInitializersUsingIncludeFiles( SgProject* node )
   {
     TimingPerformance timer1 ("Fixup known self-referential macros:");

  // This inherited attribute is used for all traversals (within the iterative approach we define)
     FixupInitializersUsingIncludeFilesInheritedAttribute inheritedAttribute;
     FixupInitializersUsingIncludeFilesTraversal astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
  // DQ (8/27/2020): We will only want to address files that we will unparse.
     astFixupTraversal.traverseInputFiles(node,inheritedAttribute);
   }

FixupInitializersUsingIncludeFilesInheritedAttribute::FixupInitializersUsingIncludeFilesInheritedAttribute()
   {
     isInsideVariableDeclaration = false;
     variableDeclaration = nullptr;
     isInsideInitializer = false;
     initializedName = nullptr;
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
   }

FixupInitializersUsingIncludeFilesSynthesizedAttribute::FixupInitializersUsingIncludeFilesSynthesizedAttribute(const FixupInitializersUsingIncludeFilesSynthesizedAttribute &)
   {
   }

FixupInitializersUsingIncludeFilesTraversal::FixupInitializersUsingIncludeFilesTraversal()
   {
   }

void
FixupInitializersUsingIncludeFilesTraversal::findAndRemoveMatchingInclude(SgStatement* statement, SgExpression* expression, PreprocessingInfo::RelativePositionType location_to_search)
   {
  // DQ (8/27/2020): At least this code can likely be refactored with the code above to a seperate function.
     AttachedPreprocessingInfoType* comments = statement->getAttachedPreprocessingInfo();
     if (comments != nullptr)
        {
          AttachedPreprocessingInfoType includeDirectiveToRemove;

       // AttachedPreprocessingInfoType::iterator i;
          for (AttachedPreprocessingInfoType::iterator i = comments->begin(); i != comments->end(); i++)
             {
               ASSERT_not_null(*i);
               if ((*i)->getRelativePosition() == location_to_search)
                  {
                 // These are possible canidates for include directives to remove.
                    if ((*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration)
                       {
                      // Get the filename from include directive.
                         std::string filenameFromIncludeDirective = (*i)->get_filename_from_include_directive();
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
                              SgValueExp* valueExpression = isSgValueExp(expression);
                              if (valueExpression == nullptr)
                                {
                                  printf ("Error: valueExpression == NULL: expression = %p = %s name = %s \n",
                                       expression,expression->class_name().c_str(),SageInterface::get_name(expression).c_str());
                                  printf (" --- statement = %p = %s \n",statement,statement->class_name().c_str());
                                  statement->get_file_info()->display("Error: valueExpression == NULL");
                                }
                              ASSERT_not_null(valueExpression);

                              string value_as_string = valueExpression->get_constant_folded_value_as_string();
                              bool knownSourcePositionUnavailable = (stringValue != nullptr);
                              if (knownSourcePositionUnavailable == true)
                                 {
                                // Open the file defined in the include directive.
                                   bool includingSelf = false;
                                   ASSERT_not_null(statement);
                                   SgSourceFile* sourceFile = SageInterface::getEnclosingSourceFile(statement,includingSelf);
                                   ASSERT_not_null(sourceFile);

                                   std::string source_directory = sourceFile->getSourceDirectory();
                                   std::string expectedSourceFileLocation = source_directory + "/" + filenameFromIncludeDirective;

                                   ifstream infile;
                                   infile.open(expectedSourceFileLocation.c_str());
                                   if (infile.is_open())
                                      {
                                        std::string file_as_string;

                                        infile.seekg(0, std::ios::end);   
                                        file_as_string.reserve(infile.tellg());
                                        infile.seekg(0, std::ios::beg);

                                        file_as_string.assign((std::istreambuf_iterator<char>(infile)),std::istreambuf_iterator<char>());

                                        infile.close();

                                     // Search file_as_string to find the expression (as text).
                                        size_t position_of_value = file_as_string.find(value_as_string);
                                        if (position_of_value != std::string::npos)
                                           {
                                             foundCorrectHeaderFileToRemove = true;
                                           }
                                      }
                                     else
                                      {
                                        printf ("Could not file include file at: filenameFromIncludeDirective = %s \n",filenameFromIncludeDirective.c_str());
                                      }
                                 }
                                else
                                 {
                                   printf ("Note: Source position information is unavailable for this expression = %p = %s name = %s \n",
                                        expression,expression->class_name().c_str(),SageInterface::get_name(expression).c_str());
                                 }
                            }

                      // This logic is not quite correct yet, we need to search for the filenameFromIncludeDirective 
                      // in the expressionFilename (and even that might be problematic). Cases such as:
                      // #include "../header.h" are not addressed bu this logic. and there are more complex cases.
                      // ultimately we would have to implement the header file lookup rules using the compiler's logic.
                         size_t includeFilenameLocationInExpressionFilename = expressionFilename.find(filenameFromIncludeDirective);

                         bool files_match = (includeFilenameLocationInExpressionFilename != string::npos) || (foundCorrectHeaderFileToRemove == true);
                         if (files_match == true)
                            {
                           // We do need to remove the redundant include directive.
                              includeDirectiveToRemove.push_back(*i);
                            }
                           else
                            {
                           // Then we don't have to removed the include directive.
                              printf ("Note: Can't match include file to initializer: \n");
                              printf (" --- expressionFilename                          = %s \n",expressionFilename.c_str());
                              printf (" --- filenameFromIncludeDirective                = %s \n",filenameFromIncludeDirective.c_str());
                            }
                       }
                  }
             }

          for (AttachedPreprocessingInfoType::iterator i = includeDirectiveToRemove.begin(); i != includeDirectiveToRemove.end(); i++)
             {
               AttachedPreprocessingInfoType::iterator position = std::find(comments->begin(), comments->end(), *i);
               if (position != comments->end())
                  {
                    comments->erase(position);
                  }
             }
        }
   }


FixupInitializersUsingIncludeFilesInheritedAttribute
FixupInitializersUsingIncludeFilesTraversal::evaluateInheritedAttribute ( SgNode* node, FixupInitializersUsingIncludeFilesInheritedAttribute inheritedAttribute )
   {
     FixupInitializersUsingIncludeFilesInheritedAttribute return_attribute(inheritedAttribute);

     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(node);
     if (declarationStatement != nullptr)
        {
          SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declarationStatement);
          if (variableDeclaration != nullptr)
             {
               return_attribute.isInsideVariableDeclaration = true;
               return_attribute.variableDeclaration         = variableDeclaration;
             }
        }

     SgInitializedName* initializedName = isSgInitializedName(node);
     if (inheritedAttribute.isInsideVariableDeclaration == true)
        {
          if (initializedName != nullptr)
             {
               SgInitializer* initializer = initializedName->get_initializer();
               if (initializer != nullptr)
                  {
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

          bool ignoreSomeExpressions = (exprListExp != nullptr) || (castExp != nullptr);

          if ( (expression != nullptr) && (initializer == nullptr) && (ignoreSomeExpressions == false) )
             {
               string expressionFilename = expression->get_file_info()->get_filename();

               ASSERT_not_null(inheritedAttribute.variableDeclaration);

               string variableDeclarationFilename = inheritedAttribute.variableDeclaration->get_file_info()->get_physical_filename();
               if (expressionFilename != variableDeclarationFilename)
                  {
                 // Search for the CPP include directive in either the variableDeclaration (attached after the statement) 
                 // or the following statement (attached before the statement).

                 // DQ (8/27/2020): At least this code can likely be refactored with the code below to a seperate function.
                    AttachedPreprocessingInfoType* comments = inheritedAttribute.variableDeclaration->getAttachedPreprocessingInfo();
                    if (comments != nullptr)
                       {
                         findAndRemoveMatchingInclude(inheritedAttribute.variableDeclaration,expression,PreprocessingInfo::after);
                       }

                    SgStatement* varDecl = inheritedAttribute.variableDeclaration;
                    SgScopeStatement* scope = varDecl->get_scope();

                 // DQ (8/27/2020): Some scopes don't work for the SageInterface::getNextStatement() function.
                    SgForStatement* forStatementScope = isSgForStatement(scope);
                    SgIfStmt*       ifStatementScope = isSgIfStmt(scope);
                    SgRangeBasedForStatement* rangeBasedForStatement = isSgRangeBasedForStatement(scope);

                    bool ignoreThisScope = (forStatementScope != nullptr) || (rangeBasedForStatement != nullptr) || (ifStatementScope != nullptr);

                    if (ignoreThisScope == false)
                       {
                         SgStatement* next_statement = SageInterface::getNextStatement(inheritedAttribute.variableDeclaration);
                         if (next_statement == nullptr)
                            {
                           // This can happen if there is no next statement (in this scope).
                            }
                           else
                            {
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
     SgNode* /*node*/,
     FixupInitializersUsingIncludeFilesInheritedAttribute /*inheritedAttribute*/,
     SubTreeSynthesizedAttributes /*synthesizedAttributeList*/ )
   {
     FixupInitializersUsingIncludeFilesSynthesizedAttribute return_attribute;
     return return_attribute;
   }


