
#include "rose.h"

#include "transformationSupport.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

void
TransformationSupport::getTransformationOptions ( SgNode* astNode, list<OptionDeclaration> & generatedList, string identifingTypeName )
   {
  // This function searches for variables of type ScopeBasedTransformationOptimization.  Variables
  // of type ScopeBasedTransformationOptimization are used to communicate optimizations from the
  // application to the preprocessor. If called from a project or file object it traverses down to
  // the global scope of the file and searches only the global scope, if called from and other
  // location within the AST it searches the current scope and then traverses the parent nodes to
  // find all enclosing scopes until in reaches the global scope.  At each scope it searches for
  // variables of type ScopeBasedTransformationOptimization.

  // printf ("######################### START OF TRANSFORMATION OPTION QUERY ######################## \n");

     ROSE_ASSERT (astNode != NULL);
     ROSE_ASSERT (identifingTypeName.c_str() != NULL);

#if 0
     printf ("In getTransformationOptions(): astNode->sage_class_name() = %s generatedList.size() = %d \n",
          astNode->sage_class_name(),generatedList.size());
     SgLocatedNode* locatedNode = isSgLocatedNode(astNode);
     if (locatedNode != NULL)
        {
          printf ("          locatedNode->get_file_info()->get_filename() = %s \n",locatedNode->get_file_info()->get_filename());
          printf ("          locatedNode->get_file_info()->get_line() = %d \n",locatedNode->get_file_info()->get_line());
        }
#endif

     switch (astNode->variant())
        {
          case ProjectTag:
             {
               SgProject* project = isSgProject(astNode);
               ROSE_ASSERT (project != NULL);

           //! Loop through all the files in the project and call the mainTransform function for each file
               int i = 0;
               for (i=0; i < project->numberOfFiles(); i++)
                  {
                    SgFile* file = &(project->get_file(i));

                 // printf ("Calling Query::traverse(SgFile,QueryFunctionType,QueryAssemblyFunctionType) \n");
                    getTransformationOptions ( file, generatedList, identifingTypeName );
                  }
               break;
             }

          case SourceFileTag:
             {
               SgSourceFile* file = isSgSourceFile(astNode);
               ROSE_ASSERT (file != NULL);
               SgGlobal* globalScope = file->get_globalScope();
               ROSE_ASSERT (globalScope != NULL);
               ROSE_ASSERT (isSgGlobal(globalScope) != NULL);
               getTransformationOptions ( globalScope, generatedList, identifingTypeName );
               break;
             }

       // Global Scope
          case GLOBAL_STMT:
             {
               SgGlobal* globalScope = isSgGlobal(astNode);
               ROSE_ASSERT (globalScope != NULL);

               SgSymbolTable* symbolTable = globalScope->get_symbol_table();
               ROSE_ASSERT (symbolTable != NULL);
               getTransformationOptions ( symbolTable, generatedList, identifingTypeName );

            // printf ("Processed global scope, exiting .. \n");
            // ROSE_ABORT();
               break;
             }

          case SymbolTableTag:
             {
            // List the variable in each scope
            // printf ("List all the variables in this symbol table! \n");
               SgSymbolTable* symbolTable = isSgSymbolTable(astNode);
               ROSE_ASSERT (symbolTable != NULL);

               bool foundTransformationOptimizationSpecifier = false;

            // printf ("Now print out the information in the symbol table for this scope: \n");
            // symbolTable->print();

#if 0
            // I don't know when a SymbolTable is given a name!
               printf ("SymbolTable has a name = %s \n",
                    (symbolTable->get_no_name()) ? "NO: it has no name" : "YES: it does have a name");
               if (!symbolTable->get_no_name())
                    printf ("SymbolTable name = %s \n",symbolTable->get_name().str());
                 else
                    ROSE_ASSERT (symbolTable->get_name().str() == NULL);
#endif

               if (symbolTable->get_table() != NULL)
                  {
                    SgSymbolTable::hash_iterator i = symbolTable->get_table()->begin();
                    int counter = 0;
                    while (i != symbolTable->get_table()->end())
                       {
                         ROSE_ASSERT ( isSgSymbol( (*i).second ) != NULL );

                      // printf ("Initial info: number: %d pair.first (SgName) = %s pair.second (SgSymbol) sage_class_name() = %s \n",
                      //      counter,(*i).first.str(),(*i).second->sage_class_name());

                         SgSymbol* symbol = isSgSymbol((*i).second);
                         ROSE_ASSERT ( symbol != NULL );
                         SgType* type = symbol->get_type();
                         ROSE_ASSERT ( type != NULL );

                         SgNamedType* namedType = isSgNamedType(type);
                         string typeName;
                         if (namedType != NULL)
                            {
                              SgName n = namedType->get_name();
                              typeName = namedType->get_name().str();
                           // char* nameString = namedType->get_name().str();
                           // printf ("Type is: (named type) = %s \n",nameString);
                              ROSE_ASSERT (identifingTypeName.c_str() != NULL);
                           // ROSE_ASSERT (typeName != NULL);
                           // printf ("In getTransformationOptions(): typeName = %s identifingTypeName = %s \n",typeName.c_str(),identifingTypeName.c_str());
                           // if ( (typeName != NULL) && ( typeName == identifingTypeName) )
                              if ( typeName == identifingTypeName )
                                 {
                                // Now look at the parameter list to the constructor and save the
                                // values into the list.

                                // printf ("Now save the constructor arguments! \n");

                                   SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);

                                   if ( variableSymbol != NULL )
                                      {
                                        SgInitializedName* initializedNameDeclaration = variableSymbol->get_declaration();
                                        ROSE_ASSERT (initializedNameDeclaration != NULL);

                                        SgDeclarationStatement* declarationStatement = initializedNameDeclaration->get_declaration();
                                        ROSE_ASSERT (declarationStatement != NULL);

                                        SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declarationStatement);
                                        ROSE_ASSERT (variableDeclaration != NULL);

                                        getTransformationOptionsFromVariableDeclarationConstructorArguments(variableDeclaration,generatedList);

                                        foundTransformationOptimizationSpecifier = true;

                                     // printf ("Exiting after saving the constructor arguments! \n");
                                     // ROSE_ABORT();
                                      }
                                     else
                                      {
#if 0
                                        printf ("Not a SgVariableSymbol: symbol->sage_class_name() = %s \n",
                                             symbol->sage_class_name());
#endif
                                      }
                                 }
                                else
                                 {
#if 0
                                   printf ("typeName != identifingTypeName : symbol->sage_class_name() = %s \n",
                                        symbol->sage_class_name());
#endif
#if 0
                                // I don't think this should ever be NULL (but it is sometimes)
                                   if (typeName != NULL)
                                        printf ("typeName == NULL \n");
#endif
                                 }
                            }
                           else
                            {
                              typeName = (char *)type->sage_class_name();
                            }

                      // printf ("In while loop at the base: counter = %d \n",counter);
                         i++;
                         counter++;
                       }
                  }
                 else
                  {
                 // printf ("Pointer to symbol table is NULL \n");
                  }

            // printf ("foundTransformationOptimizationSpecifier = %s \n",foundTransformationOptimizationSpecifier ? "true" : "false");

            // SgSymbolTable objects don't have a parent node (specifically they lack a get_parent
            // member function in the interface)!
               break;
             }

          case BASIC_BLOCK_STMT:
             {
            // List the variable in each scope
            // printf ("List all the variables in this scope! \n");
               SgBasicBlock* basicBlock = isSgBasicBlock(astNode);
               ROSE_ASSERT (basicBlock != NULL);

               SgSymbolTable* symbolTable = basicBlock->get_symbol_table();
               ROSE_ASSERT (symbolTable != NULL);
               getTransformationOptions ( symbolTable, generatedList, identifingTypeName );

            // Next go (fall through this case) to the default case so that we traverse the parent
            // of the SgBasicBlock.
            // break;
             }

          default:
            // Most cases will be the default (this is by design)
            // printf ("default in switch found in globalQueryGetListOperandStringFunction() (sage_class_name = %s) \n",astNode->sage_class_name());

            // Need to recursively backtrack through the parents until we reach the SgGlobal (global scope)
               SgStatement* statement = isSgStatement(astNode);
               if (statement != NULL)
                  {
                    SgNode* parentNode = statement->get_parent();
                    ROSE_ASSERT (parentNode != NULL);
//                  printf ("parent = %p parentNode->sage_class_name() = %s \n",parentNode,parentNode->sage_class_name());
                    SgStatement* parentStatement = isSgStatement(parentNode);
                    if (parentStatement == NULL)
                       {
                         printf ("parentStatement == NULL: statement (%p) is a %s \n",statement,statement->sage_class_name());
                         printf ("parentStatement == NULL: statement->get_file_info()->get_filename() = %s \n",statement->get_file_info()->get_filename());
                         printf ("parentStatement == NULL: statement->get_file_info()->get_line() = %d \n",statement->get_file_info()->get_line());
                       }
                    ROSE_ASSERT (parentStatement != NULL);

                 // Call this function recursively (directly rather than through the query mechanism)
                    getTransformationOptions ( parentStatement, generatedList, identifingTypeName );
                  }
                 else
                  {
                 // printf ("astNode is not a SgStatement! \n");
                  }

               break;
        }

#if 0
     printf ("At BASE of getTransformationOptions(): astNode->sage_class_name() = %s size of generatedList = %d \n",
          astNode->sage_class_name(),generatedList.size());
#endif

  // printf ("######################### END OF TRANSFORMATION OPTION QUERY ######################## \n");
   }


void
TransformationSupport::getTransformationOptionsFromVariableDeclarationConstructorArguments (
   SgVariableDeclaration* variableDeclaration,
   list<OptionDeclaration> & returnEnumValueList )
   {
     ROSE_ASSERT (variableDeclaration != NULL);
     SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();

     printf ("Inside of getTransformationOptionsFromVariableDeclarationConstructorArguments() \n");

     for (SgInitializedNamePtrList::iterator i = variableList.begin(); i != variableList.end(); i++)
        {
       // We don't care about the name
       // SgName & name = (*i).get_name();

          ROSE_ASSERT ((*i) != NULL);
       // QY 11/10/04 removed named_item in SgInitializedName
       // printf ("Processing a variable in the list \n");
       //   if ((*i)->get_named_item() != NULL)
       //      {
       //        ROSE_ASSERT ((*i)->get_named_item() != NULL);
       //        SgInitializer *initializerOther = (*i)->get_named_item()->get_initializer();
               SgInitializer *initializerOther = (*i)->get_initializer();

            // This is not always a valid pointer
            // ROSE_ASSERT (initializerOther != NULL);

            // printf ("Test for initialized in variable \n");
               if (initializerOther != NULL)
                  {
                 // There are other things we could ask of the initializer
                    ROSE_ASSERT (initializerOther != NULL);

                 // printf ("Found a valid initialized in variable \n");

                    SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(initializerOther);
                    ROSE_ASSERT (constructorInitializer != NULL);

                    SgExprListExp* argumentList = constructorInitializer->get_args();
                    ROSE_ASSERT (argumentList != NULL);

                    SgExpressionPtrList & expressionPtrList = argumentList->get_expressions();
                    ROSE_ASSERT(expressionPtrList.empty() == false);
                    SgExpressionPtrList::iterator i = expressionPtrList.begin();

                    ROSE_ASSERT (*i != NULL);

                 // First value is a char* identifying the option
                    SgStringVal* charString = isSgStringVal(*i);
                    ROSE_ASSERT (charString != NULL);

#if 1
                    string optionString;
                    string valueString;

                    int counter = 0;
                    while (i != expressionPtrList.end())
                       {
                      // printf ("Expression List Element #%d of %d (total) (*i)->sage_class_name() = %s \n",
                      //      counter,expressionPtrList.size(),(*i)->sage_class_name());

                         switch ( (*i)->variant() )
                            {
                              case ENUM_VAL:
                                 {
                                   SgEnumVal* enumVal = isSgEnumVal(*i);
                                   ROSE_ASSERT (enumVal != NULL);

                                // int enumValue = enumVal->get_value();
                                   SgName enumName = enumVal->get_name();
                                // printf ("Name = %s value = %d \n",enumName.str(),enumValue);
                                // printf ("Name = %s \n",enumName.str());

                                   string name = enumName.str();
                                // char* name = rose::stringDuplicate( enumName.str() );

                                // Put the value at the start of the list so that the list can be processed in
                                // consecutive order to establish options for consecutive scopes (root to
                                // child scope). Order is not important if we are only ORing the operands!
                                // returnEnumValueList.push_front (name);
                                // returnEnumValueList.push_front (enumValue);

                                   ROSE_ASSERT (counter == 0);
                                   break;
                                 }

                              case STRING_VAL:
                                 {
                                // ROSE_ASSERT (counter == 1);

                                // printf ("Found a SgStringVal expression! \n");

                                   SgStringVal* stringVal = isSgStringVal(*i);
                                   ROSE_ASSERT (stringVal != NULL);

                                   if (counter == 0)
                                      {
                                        optionString = stringVal->get_value();
                                        valueString  = "";
                                     // printf ("optionString = %s \n",optionString);
                                      }

                                   if (counter == 1)
                                      {
                                        valueString = stringVal->get_value();
                                     // printf ("valueString = %s \n",valueString);
                                      }
                                   break;
                                 }

                              case DOUBLE_VAL:
                                 {
                                   ROSE_ASSERT (counter == 1);

                                // printf ("Found a SgStringVal expression! \n");

                                   SgDoubleVal* doubleVal = isSgDoubleVal(*i);
                                   ROSE_ASSERT (doubleVal != NULL);

                                   valueString = StringUtility::numberToString(doubleVal->get_value()).c_str();
                                // printf ("valueString = %s \n",valueString);
                                   break;
                                 }

                              default:
                                 {
                                   printf ("Default reached in switch in getTransformationOptionsFromVariableDeclarationConstructorArguments() (*i)->sage_class_name() = %s \n",(*i)->sage_class_name());
                                   ROSE_ABORT();
                                   break;
                                 }
                            }

                         i++;
                         counter++;
                       }

                 // printf ("optionString = %s valueString = %s \n",optionString,valueString);

                    OptionDeclaration optionSpecification(optionString.c_str(),valueString.c_str());
                    returnEnumValueList.push_front (optionSpecification);
#endif
                  }
                 else
                  {
                 // printf ("Valid initializer not found in variable \n");
                  }
/*
             }
            else
             {
            // printf ("Warning: In getTransformationOptionsFromVariableDeclarationConstructorArguments(): (*i).get_named_item() = NULL \n");
             }
*/
        }
   }







