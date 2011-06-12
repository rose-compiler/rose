// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupTypes.h"
#include "fixupNames.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

#if 0
// DQ (2/21/2007): This does not work since the second file's SgClassSymbol would be different!
// DQ (2/21/2007): Note that this could have been a template functions (but we only require two cases)
int
entryInClassSymbolMemoryPool( SgClassSymbol* classSymbol )
   {
  // This function traverses the memory pool for an IR node and
  // returns the position of the input IR node in the list.
  // The function is clearly linear in complexity, but is only 
  // required to help us rename un-named declarations.

     int count = 0;
     if (Memory_Block_List.empty() == false)
        {
       // Generate an array of memory pools
          SgClassSymbol** objectArray = (SgClassSymbol**) &(SgClassSymbol::Memory_Block_List[0]);

       // Build a local variable for better performance
          const SgNode* IS_VALID_POINTER = AST_FileIO::IS_VALID_POINTER();

       // Iterate over the memory pools
          for (unsigned int i=0; i < Memory_Block_List.size(); i++)
             {
            // objectArray[i] is a single memory pool
               for (int j=0; j < SgClassSymbol::CLASS_ALLOCATION_POOL_SIZE; j++)
                  {
                    if (objectArray[i][j].p_freepointer == IS_VALID_POINTER)
                       {
                         count++;

                      // DQ (2/21/2007): This detail is different from the numberOfNodes function
                         if ( classSymbol == &(objectArray[i][j]) )
                            break;
                       }
                  }
             }
        }

     return count;
   }

#endif

#if 0
string
declarationPositionString( SgDeclarationStatement* declaration )
   {
  // This function generates a unique string for a declaration.

     ROSE_ASSERT(declaration != NULL);
     Sg_File_Info* fileInfo = declaration->get_file_info();
     ROSE_ASSERT(fileInfo != NULL);

     int line_number   = fileInfo->get_line();
     int column_number = fileInfo->get_col();

     string returnString = StringUtility::numberToString(line_number) + ":" +  StringUtility::numberToString(column_number);

     return returnString;
   }
#endif


void
resetNamesInAST()
   {
  // DQ (3/17/2007): This should be empty
     ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

  // Fixup empty names used in declarations containing multiple variables
  // (or types for typedefs). See test2006_150.C.
     ResetEmptyNames t1;
     t1.traverseMemoryPool();

  // DQ (3/17/2007): This should be empty
     ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

  // Fixup any inconsistant names between defining vs. nondefining declarations
  // for SgClassDeclaration (and test SgFunctionDeclaration for consistancy).
  // This step must be called after any empty names have been reset.
     ResetInconsistantNames t2;
     t2.traverseMemoryPool();

  // DQ (3/17/2007): This should be empty
     ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);
   }

void
ResetEmptyNames::visit(SgNode* node)
   {
#if 0
     printf ("##### ResetEmptyNames::visit(node = %p = %s) \n",node,node->sage_class_name());
#endif

  // Handle the case of un-named classes, structs, and unions.
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
     if (classDeclaration != NULL)
        {
          SgClassDeclaration* declaration            = isSgClassDeclaration(node);
          SgClassDeclaration* definingDeclaration    = isSgClassDeclaration(declaration->get_definingDeclaration());
       // SgClassDeclaration* nondefiningDeclaration = isSgClassDeclaration(declaration->get_firstNondefiningDeclaration());
#if 0
          printf ("declaration = %p = %s = %s \n",declaration,declaration->class_name().c_str(),declaration->get_name().str());
          if (definingDeclaration != NULL)
             {
               printf ("definingDeclaration = %p = %s = %s \n",definingDeclaration,definingDeclaration->class_name().c_str(),definingDeclaration->get_name().str());
             }
#endif
       // DQ (2/11 & 23/2007): I think we need to reset the names stored in all the declarations (defining and non-defining!
       // Only reset names where the defining declarations exists, then reuse that name from the defining declaaration 
       // for the other declarations, this force the name change to happen only once so that the mangled names of data 
       // member are not using the new name in any subsequent renaming of the same declaration's non-defining declaration.

       // DQ (3/11/2007): test2006_59.C demonstrates use of "typedef struct {} a,*b;" which requires that SgClassDeclaration
       // in typedefs be renames (no special exemption just for being in a typedef).
       // DQ (3/10/2007): Ignore this problem for un-named declarations defined in a SgTypedefDeclaration
       // if (definingDeclaration != NULL && (definingDeclaration->get_name().is_null() == true) )
          if (definingDeclaration != NULL)
       // if ( (definingDeclaration != NULL) && (isSgTypedefDeclaration(definingDeclaration->get_parent()) == NULL) )
          if (definingDeclaration != NULL)
             {
               if (definingDeclaration->get_name().is_null() == true)
            // if (declaration->get_name().is_null() == true)
                  {
#if 0
                    SgClassDeclaration* nondefiningDeclaration = isSgClassDeclaration(declaration->get_firstNondefiningDeclaration());
                    printf ("ResetEmptyNames::visit(SgClassDeclaration): declaration = %p \n",declaration);
                    printf ("ResetEmptyNames::visit(SgClassDeclaration): definingDeclaration = %p \n",definingDeclaration);
                    printf ("ResetEmptyNames::visit(SgClassDeclaration): nondefiningDeclaration = %p \n",nondefiningDeclaration);
                    declaration->get_file_info()->display("Location of SgClassDeclaration:");
#endif

                 // DQ (3/11/2007): The case of "typedef struct {} a,*b;" invalidates this argument!
                 // DQ (3/10/2007): Note un-named class declarations appearing in typedef declarations should not be changed since they cannot 
                 // be referenced more than the one time they appear and as un-named class and enum declarations they can appear in 
                 // multiple typedef declarations (repeated).  So make sure this is not an un-name class declaration or enum 
                 // declaration in a typedef.
                 // ROSE_ASSERT(isSgTypedefDeclaration(definingDeclaration->get_parent()) == NULL);

                    if (definingDeclaration != NULL)
                       {
#if 0
                         printf ("definingDeclaration = %p = %s = %s \n",definingDeclaration,definingDeclaration->class_name().c_str(),definingDeclaration->get_name().str());
#endif
                         ROSE_ASSERT(definingDeclaration->get_name().getString().find("__unnamed_class") != string::npos || definingDeclaration->get_name().is_null() == true);
                       }

                 // Remove the old name and reinset the symbol under the new name

                 // DQ (2/11/2007): This is now simpler because we have better a better symbol table API
                 // definingDeclaration->get_scope()->remove_symbol(definingDeclaration->get_symbol_from_symbol_table());
#if 0
                    printf ("Searching for a SgClassSymbol for the definingDeclaration = %p = %s \n",definingDeclaration,SageInterface::get_name(definingDeclaration).c_str());
#endif
                    SgSymbol* symbol = declaration->get_symbol_from_symbol_table();
                    SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                    if (classSymbol != NULL)
                       {
#if 0
                         printf ("classSymbol has an associated declartion: classSymbol->get_declaration() = %p \n",classSymbol->get_declaration());
                         printf ("Calling remove(classSymbol = %p) \n",classSymbol);
#endif
                         definingDeclaration->get_scope()->remove_symbol(classSymbol);
                       }
#if 0
                 // DQ (2/9/2007): This technique for generating names causes the same name in different 
                 // files to be different which is not so great when we try to do the AST merge.

                 // Rename the declaration
                    static int counter = 0;
                    string new_name = "__rose_generated_structure_tag_name_" + StringUtility::numberToString(counter);
                    definingDeclaration->set_name(new_name);
                 // printf ("Found empty name at definingDeclaration = %p counter = %d new_name = %s \n",definingDeclaration,counter,new_name.c_str());
                    counter++;
#else
                 // DQ (2/11/2007): This mechanism generates the same name for the same structure (more suitable for use with AST merge)
                    SgName new_name = "__unnamed_class__";

                 // DQ (3/10/2007): Added a unique string based on the source position information so that the different un-named declarations would not be confused.
                 // See test2004_105.C for an example of where this is important.
                    new_name += SageInterface::declarationPositionString(definingDeclaration) + string("_");

                 // DQ (2/21/2007): This has been moved back to the generateUniqueName() function.
                 // DQ (2/21/2007): Modified to use the position of the SgClassSymbol in the memory pool.
                 // Using the statement position would have been more complex since it would have been 
                 // difficult to handle function pointers that can generate multiple declarations.
                 // DQ (2/21/2007): We need to make the name in the declaration unique (can't be done dynamically
                 // since the manipulation of the symbol table will cause the name to be computed inconsistantly).
                 // unsigned int uniqueSymbolValue = declaration->get_scope()->get_symbol_table()->generateUniqueNumberForMatchingSymbol(declaration);
                 // unsigned int uniqueSymbolValue = entryInClassSymbolMemoryPool(declaration);
                 // new_name += "__uniqueSymbolValue_" + StringUtility::numberToString(uniqueSymbolValue) + "_";
                 // printf ("declaration = %p uniqueSymbolValue = %d new_name = %s \n",declaration,uniqueSymbolValue,new_name.str());
                 // new_name += "__uniqueSymbolValue_" + declarationPositionString(declaration) + "_";

                 // printf ("declaration = %p new_name = %s \n",declaration,new_name.str());

                 // ROSE_ASSERT(false);

                    if (definingDeclaration != NULL)
                       {
                         SgClassDefinition* classDefinition = definingDeclaration->get_definition();
                         const SgDeclarationStatementPtrList & declarationList = classDefinition->get_members();

#if 0
                      // DQ (2/18/2007): This is not an error, but I would like to be able to investigate it further.
                      // printf ("ResetEmptyNames::visit(SgClassDeclaration): declarationList.size() = %ld \n",declarationList.size());
                      // ROSE_ASSERT(declarationList.empty() == false);
                         if (declarationList.empty() == true)
                            {
                              printf ("WARNING in ResetEmptyNames::visit(SgClassDeclaration): declarationList.empty() == true \n");
                            }
#endif
                         SgDeclarationStatementPtrList::const_iterator i = declarationList.begin();

                         while(i != declarationList.end())
                            {
                           // printf ("Before calling (*i)->get_mangled_name() on class member declaration = %p = %s \n",*i,(*i)->class_name().c_str());

                           // DQ (2/22/2007): Need to make sure that the type is included, so use the generateUniqueName() function
                           // name += (*i)->get_mangled_name();
                           // new_name += SageInterface::get_name(*i);
                              new_name += (*i)->get_mangled_name();
                           // new_name += SageInterface::generateUniqueName(*i,true);
#if 0
                              printf ("(*i)->get_mangled_name() = %s \n",(*i)->get_mangled_name().str());
                              printf ("After calling SageInterface::generateUniqueName(*i,true) on class member declaration = %p = %s new_name = %s \n",*i,(*i)->class_name().c_str(),new_name.str());
#endif
                              i++;
                              if (i != declarationList.end())
                                 {
                                   new_name += "__DELIMITER__";
                                 }
                            }
                       }
                      else
                       {
                         new_name += "__without_definition__";
                       }

                 // printf ("In ResetEmptyNames::visit(SgClassDeclaration): name = %s \n",name.str());

                    ROSE_ASSERT(new_name.is_null() == false);

                 // DQ (2/23/2007): Switch this back to the defining declaration so that we reset this once per class and not redundently for each nondefining declaration.
                 // DQ (2/11/2007): This should be the declaration instead of the definingDeclaration, so that we will rename them all equivalently
                 // as they are visited (there can be many).
                 // definingDeclaration->set_name(name);
                 // declaration->set_name(new_name);
                    definingDeclaration->set_name(new_name);

                 // DQ (3/10/2007): Mark this explicitly as an un-named class declaration.
                 // This explicit marking makes the process of code generation safer.
                    definingDeclaration->set_isUnNamed(true);

                 // DQ (3/17/2007): This should not be in the map, if it is then it was previously accessed and the name there is wrong.
                    if (SgNode::get_globalMangledNameMap().find(definingDeclaration) != SgNode::get_globalMangledNameMap().end())
                       {
                      // printf ("Note: un-named declartion (new_name = %s) was found in the global mangled name map, clearing map of ALL entries! \n",new_name.str());
                         SgNode::clearGlobalMangledNameMap();
                       }
                    ROSE_ASSERT(SgNode::get_globalMangledNameMap().find(definingDeclaration) == SgNode::get_globalMangledNameMap().end());

#if 0
                    printf ("After resetting the name in declaration = %p declaration->get_name() = %s \n",declaration,declaration->get_name().str());
#endif

#endif
                 // Now reinsert the classSymbol (using the new name)
#if 0
                    printf ("Now reinsert the classSymbol = %p into the symbol table using name = %s \n",classSymbol,new_name.str());
#endif
                 // ROSE_ASSERT(classSymbol != NULL);
                    if (classSymbol != NULL)
                       {
#if 1
                         printf ("Calling insert(classSymbol = %p) using name = %s even if classSymbol->get_name() = %s in scope = %p = %s \n",
                              classSymbol,new_name.str(),classSymbol->get_name().str(),definingDeclaration->get_scope(),definingDeclaration->get_scope()->class_name().c_str());
#endif
                      // definingDeclaration->get_scope()->insert_symbol(classSymbol->get_name(),classSymbol);
                      // We need to reinsert it using the new name and not the one in the declaration referenced by the symbol since we might not have changed the declaration's name yet!
                         definingDeclaration->get_scope()->insert_symbol(new_name,classSymbol);

                      // DQ (3/5/2007): This will be caught later if we don't catch it now.
                         ROSE_ASSERT(classSymbol->get_parent() == definingDeclaration->get_scope()->get_symbol_table());
                         ROSE_ASSERT(classSymbol->get_parent() != NULL);
                         ROSE_ASSERT(isSgSymbolTable(classSymbol->get_parent()) != NULL);
                         ROSE_ASSERT (isSgSymbolTable(classSymbol->get_parent())->exists(classSymbol) == true);
                       }

                 // printf ("After resetting name for class declaration = %p = %s new_name = %s \n",declaration,declaration->class_name().c_str(),new_name.str());
                 // ROSE_ASSERT(false);
                  }
                 else
                  {
                 // DQ (3/3/2007): We might have a problem if the non-defining declaration were to be visited before the defining declaration.
                 // So the assert at least makes sure that the defining declaration name has been reset, but that would just cause us to end 
                 // in an assertion not get the name changed and the symbol unloaded and loaded into the associated symbol table correctly.
                 // Actually this is not a problem since the defining declaration is reset independent of the order in which the declarations are visited.
                    ROSE_ASSERT(definingDeclaration != NULL);
                    ROSE_ASSERT(definingDeclaration->get_name().is_null() == false);

                 // printf ("In ResetEmptyNames: non-empty definingDeclaration name (at %p) = %s \n",definingDeclaration,definingDeclaration->get_name().str());
                    if (declaration->get_name().is_null() == true)
                       {
#if 0
                         printf ("Updating the non-defining declaration = %p with current name = %s to use name on defining declaration = %p with name = %s \n",
                              declaration,declaration->get_name().str(),definingDeclaration,definingDeclaration->get_name().str());
#endif
                      // DQ (2/23/2007): In case it was a different declaration (there are defining and non-defining
                      // and there can be multiple non-defining declarations) search for the old symbol and remove it.
                      // It might not have been removed when the name was changed, because we used the wrong declaration 
                      // to lookup the symbol.
                         SgSymbol* symbol = declaration->get_symbol_from_symbol_table();
                         SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                         if (classSymbol != NULL)
                            {
#if 0
                              printf ("removing the classSymbol using declaration = %p classSymbol->get_declaration() = %p \n",declaration,classSymbol->get_declaration());
#endif
                           // printf ("Calling remove(classSymbol = %p) \n",classSymbol);
                           // Always use the scope of the definingDeclaration for consistancy, but it should not make a difference.
                              definingDeclaration->get_scope()->remove_symbol(classSymbol);
                            }

                      // This case appears imposible, since the defining declaration is reset independent of the order the declarations are visited.
                      // printf ("Resetting class declaration = %p using the name in the defining declaration: name = %s \n",declaration,definingDeclaration->get_name().str());
                         ROSE_ASSERT(definingDeclaration->get_name().is_null() == false);

                         declaration->set_name(definingDeclaration->get_name());

                      // DQ (3/10/2007): Mark this explicitly as an un-named class declaration.
                      // This explicit marking makes the process of code generation safer.
                         declaration->set_isUnNamed(true);

                      // DQ (3/17/2007): This should not be in the map, if it is then it was previously accessed and the name there is wrong.
                         ROSE_ASSERT(SgNode::get_globalMangledNameMap().find(declaration) == SgNode::get_globalMangledNameMap().end());

                      // DQ (3/3/2007): If this is the declaration that casued the associated SgClassSymbol to be removed then we have to add it back after the name is changed.
                         if (classSymbol != NULL)
                            {
#if 1
                              printf ("After reseting the name to = %s insert classSymbol using declaration = %p classSymbol->get_declaration() = %p scope = %p = %s \n",
                                   definingDeclaration->get_name().str(),declaration,classSymbol->get_declaration(),definingDeclaration->get_scope(),definingDeclaration->get_scope()->class_name().c_str());
#endif
                              definingDeclaration->get_scope()->insert_symbol(definingDeclaration->get_name(),classSymbol);

                           // DQ (3/5/2007): This will be caught later if we don't catch it now.
                              ROSE_ASSERT(classSymbol->get_parent() == definingDeclaration->get_scope()->get_symbol_table());
                              ROSE_ASSERT (definingDeclaration->get_scope()->symbol_exists(classSymbol) == true);
                              ROSE_ASSERT(classSymbol->get_parent() != NULL);
                              ROSE_ASSERT(isSgSymbolTable(classSymbol->get_parent()) != NULL);
                              ROSE_ASSERT (isSgSymbolTable(classSymbol->get_parent())->exists(classSymbol) == true);
                            }
                       }
                  }
             }
        }

  // Handle the case of un-named enums.
     SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(node);
     if (enumDeclaration != NULL)
        {
          SgEnumDeclaration* declaration            = isSgEnumDeclaration(node);
          SgEnumDeclaration* definingDeclaration    = isSgEnumDeclaration(declaration->get_definingDeclaration());
       // SgClassDeclaration* nondefiningDeclaration = isSgClassDeclaration(declaration->get_firstNondefiningDeclaration());

       // printf("Enum declaration = %p definingDeclaration = %p \n",declaration,definingDeclaration);

       // Only reset names where the defining declarations exists
       // if (definingDeclaration != NULL && (definingDeclaration->get_name().is_null() == true) )
       // if (definingDeclaration != NULL)
          if (declaration->get_name().is_null() == true)
             {
            // if (definingDeclaration->get_name().is_null() == true)

            // DQ (3/10/2007): Ignore this problem for un-named declarations defined in a SgTypedefDeclaration
            // if (declaration->get_name().is_null() == true)
               if ( (definingDeclaration != NULL) && (isSgTypedefDeclaration(definingDeclaration->get_parent()) == NULL) )
                  {
#if 0
                    SgEnumDeclaration* nondefiningDeclaration = isSgEnumDeclaration(declaration->get_firstNondefiningDeclaration());
                    printf ("ResetEmptyNames::visit(SgEnumDeclaration): declaration = %p \n",declaration);
                    printf ("ResetEmptyNames::visit(SgEnumDeclaration): definingDeclaration = %p \n",definingDeclaration);
                    printf ("ResetEmptyNames::visit(SgEnumDeclaration): nondefiningDeclaration = %p \n",nondefiningDeclaration);

                    printf ("ResetEmptyNames::visit(): definingDeclaration->get_name() = %s \n",definingDeclaration->get_name().str());
#endif

                 // DQ (3/10/2007): Note un-named class declarations appearing in typedef declarations should not be changed since they cannot 
                 // be referenced more than the one time they appear and as un-named class and enum declarations they can appear in 
                 // multiple typedef declarations (repeated).  So make sure this is not an un-name class declaration or enum 
                 // declaration in a typedef.
                    ROSE_ASSERT(isSgTypedefDeclaration(definingDeclaration->get_parent()) == NULL);

#if 0
                    printf ("USE THE NEW SYMBOL TABLE API (for SgEnumStatement un-named enum renaming) \n");
                 // Remove the old name and reinset the symbol under the new name
                    SgEnumSymbol* enumSymbol = definingDeclaration->get_scope()->lookup_enum_symbol(definingDeclaration->get_name());
                    if (enumSymbol == NULL)
                       {
                         definingDeclaration->get_startOfConstruct()->display("Error: SgClassDeclaration definingDeclaration located at:");
                         printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                         SageInterface::outputLocalSymbolTables(definingDeclaration->get_scope());
                         printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                       }
                    ROSE_ASSERT(enumSymbol != NULL);

                 // Since there could be many symbols (even of the same symbol type, e.g. un-named classes and enums)
                 // we have to find the symbol that matches the declaration.
                 // printf ("enumSymbol->get_declaration() = %p \n",enumSymbol->get_declaration());

                 // Note: for an "enum" there are no forward or non-defining declarations so we test against the definingDeclaration.
                    while (enumSymbol != NULL && enumSymbol->get_declaration() != definingDeclaration)
                       {
                      // printf ("Looking for the enumSymbol that is associated with the correct declaration classSymbol->get_declaration() = %p \n",enumSymbol->get_declaration());
                         enumSymbol = definingDeclaration->get_scope()->next_enum_symbol();
                       }

                    ROSE_ASSERT(enumSymbol != NULL);
                    if (enumSymbol->get_declaration() != definingDeclaration)
                       {
                         printf ("Can't find the correct SgEnumSymbol \n");
                         definingDeclaration->get_startOfConstruct()->display("Error: SgClassDeclaration definingDeclaration located at:");
                         printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                         SageInterface::outputLocalSymbolTables(definingDeclaration->get_scope());
                         printf ("************************ OUTPUT SYMBOL TABLE *********************\n");
                       }
                    ROSE_ASSERT(enumSymbol->get_declaration() == definingDeclaration);

                 // Now remove the old enumSymbol
                    if (enumSymbol != NULL)
                       {
                      // printf ("Calling remove(classSymbol) \n");
                         definingDeclaration->get_scope()->remove_symbol(enumSymbol);
                       }
#else
                 // DQ (2/11/2007): This is now simpler because we have better a better symbol table API
                 // definingDeclaration->get_scope()->remove_symbol(definingDeclaration->get_symbol_from_symbol_table());
                 // printf ("Searching for a SgEnumSymbol for the definingDeclaration = %p = %s \n",definingDeclaration,SageInterface::get_name(definingDeclaration).c_str());
                    SgSymbol* symbol = declaration->get_symbol_from_symbol_table();
                    SgEnumSymbol* enumSymbol = isSgEnumSymbol(symbol);
                    if (enumSymbol != NULL)
                       {
                      // printf ("enumSymbol has an associated declartion: enumSymbol->get_declaration() = %p \n",enumSymbol->get_declaration());
                      // printf ("Calling remove(enumSymbol = %p) \n",enumSymbol);
                         definingDeclaration->get_scope()->remove_symbol(enumSymbol);
                       }
#endif

#if 0
                 // Rename the declaration
                    static int counter = 0;
                    string new_name = "__rose_generated_enum_tag_name_" + StringUtility::numberToString(counter);
                    definingDeclaration->set_name(new_name);
                    printf ("Found empty name at definingDeclaration = %p counter = %d new_name = %s \n",definingDeclaration,counter,new_name.c_str());
                    counter++;
#else
                    const SgInitializedNamePtrList & fieldList = definingDeclaration->get_enumerators();
                 // printf ("In SgEnumDeclaration::get_mangled_name(): fieldList.size() = %ld \n",fieldList.size());
                 // ROSE_ASSERT(fieldList.empty() == false);

                 // This is not a serious problem, but warn about it for now
                    if (fieldList.empty() == true)
                       {
                         printf ("WARNING in ResetEmptyNames resetting un-named enum: fieldList.empty() == true \n");
                       }
                    SgInitializedNamePtrList::const_iterator i = fieldList.begin();
                    SgName new_name = "__unnamed_enum__";

                 // DQ (3/10/2007): Added a unique string based on the source position infomation so that the different un-named declarations would not be confused.
                 // See test2004_105.C for an example of where this is important.
                    new_name += SageInterface::declarationPositionString(definingDeclaration) + string("_");

                 // DQ (2/21/2007): This has been moved back to the generateUniqueName() function.
                 // DQ (2/21/2007): Modified to use the position of the SgClassSymbol in the memory pool.
                 // Using the statement position would have been more complex since it would have been 
                 // difficult to handle function pointers that can generate multiple declarations.
                 // DQ (2/21/2007): We need to make the name in the declaration unique (can't be done dynamically
                 // since the manipulation of the symbol table will cause the name to be computed inconsistantly).
                 // unsigned int uniqueSymbolValue = enumDeclaration->get_scope()->get_symbol_table()->generateUniqueNumberForMatchingSymbol(enumDeclaration);
                 // unsigned int uniqueSymbolValue = entryInEnumSymbolMemoryPool(declaration);
                 // new_name += "__uniqueSymbolValue_" + StringUtility::numberToString(uniqueSymbolValue) + "_";
                 // new_name += "__uniqueSymbolValue_" + declarationPositionString(declaration) + "_";

                    while(i != fieldList.end())
                       {
                         new_name += (*i)->get_name();
                         i++;
                         if (i != fieldList.end())
                            {
                              new_name += "__COMMA__";
                            }
                       }

                 // definingDeclaration->set_name(new_name);
                    declaration->set_name(new_name);

                 // DQ (3/10/2007): Mark this explicitly as an un-named class declaration.
                 // This explicit marking makes the process of code generation safer.
                    declaration->set_isUnNamed(true);

                 // DQ (3/17/2007): This should not be in the map, if it is then it was previously accessed and the name there is wrong.
                    ROSE_ASSERT(SgNode::get_globalMangledNameMap().find(declaration) == SgNode::get_globalMangledNameMap().end());

                 // printf ("Found empty name at definingDeclaration = %p new_name = %s \n",definingDeclaration,new_name.str());
#endif

                 // Now reinsert the enumSymbol (using the new name)
                 // printf ("Now reinsert the enumSymbol = %p into the symbol table using name = %s \n",enumSymbol,new_name.str());
                    if (enumSymbol != NULL)
                       {
                         printf ("Calling insert(enumSymbol = %p) using name = %s even if enumSymbol->get_name() = %s \n",enumSymbol,new_name.str(),enumSymbol->get_name().str());
                      // definingDeclaration->get_scope()->insert_symbol(enumSymbol->get_name(),enumSymbol);
                         definingDeclaration->get_scope()->insert_symbol(new_name,enumSymbol);
                       }
                  }
                 else
                  {
                 // printf ("In ResetEmptyNames: non-empty definingDeclaration name (at %p) = %s \n",definingDeclaration,definingDeclaration->get_name().str());
                  }
             }
        }
   }

void
ResetInconsistantNames::visit(SgNode* node)
   {
#if 0
     printf ("##### ResetInconsistantNames::visit(node = %p = %s) \n",node,node->sage_class_name());
#endif

  // These are the only IR nodes that have names that can be held twice, and thus be inconsistant,
  // are declarations (which have defining and non-defining declarations.  Additionally,
  // there can be numerous non-defining declarations.  Sometimes these additional non-defining
  // declaration can hold the wrong name (e.g. "typedef struct { int state;} my_struct_typedef;"
  // where an added non-defining clss declaration has the name "my_struct_typedef" instead of
  // being empty.  This screws up the AST merge since then non-definining declarations of the 
  // same declaration will not merge to the same declaration.
     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);

  // SgStatement and SgExpression IR nodes should always have a valid parent (except for the SgProject)
     if (declaration != NULL)
        {
          switch (declaration->variantT())
             {
               case V_SgClassDeclaration:
                  {
                    SgClassDeclaration* declaration            = isSgClassDeclaration(node);
                    SgClassDeclaration* definingDeclaration    = isSgClassDeclaration(declaration->get_definingDeclaration());
                    SgClassDeclaration* nondefiningDeclaration = isSgClassDeclaration(declaration->get_firstNondefiningDeclaration());
#if 0
                    printf ("In ResetInconsistantNames: declaration            = %p = %s get_name() = %s \n",declaration,declaration->class_name().c_str(),declaration->get_name().str());
                    printf ("                           definingDeclaration    = %p get_name() = %s \n",definingDeclaration,(definingDeclaration != NULL) ? definingDeclaration->get_name().str() : "empty name");
                    printf ("                           nondefiningDeclaration = %p get_name() = %s \n",nondefiningDeclaration,(nondefiningDeclaration != NULL) ? nondefiningDeclaration->get_name().str() : "empty name");
#endif
                    if (definingDeclaration == NULL)
                       {
                      // The defining declaration of the current IR node (declaration) will be set later 
                      // in the AST post-processing, but for now we want to get the name if it is available.
                         if (nondefiningDeclaration != NULL && nondefiningDeclaration->get_definingDeclaration() != NULL)
                            {
                           // printf ("In ResetInconsistantNames: getting at the defining declaration via the nondefining declarations link to the defining declaration! \n");
                              definingDeclaration = isSgClassDeclaration(nondefiningDeclaration->get_definingDeclaration());
                              ROSE_ASSERT(definingDeclaration != NULL);
                            }
                       }

                    if (definingDeclaration != NULL)
                       {
#if 0
                         if (declaration->get_name() != definingDeclaration->get_name())
                            {
                              printf ("Error: declaration = %p = %s get_name() = %s \n",declaration,declaration->class_name().c_str(),declaration->get_name().str());
                              printf ("Error: definingDeclaration = %p get_name() = %s \n",definingDeclaration,definingDeclaration->get_name().str());
                            }
#endif
                      // Set the name to that in the defining declaration, it is the master declaration :-)
                         declaration->set_name(definingDeclaration->get_name());
                         ROSE_ASSERT(declaration->get_name() == definingDeclaration->get_name());

                      // DQ (3/10/2007): Mark this explicitly as an un-named class declaration.
                      // This explicit marking makes the process of code generation safer.
                         if (definingDeclaration->get_isUnNamed() == true)
                            {
                              declaration->set_isUnNamed(true);
                            }

                      // DQ (3/17/2007): This should not be in the map, if it is then it was previously accessed and the name there is wrong.
                         ROSE_ASSERT(SgNode::get_globalMangledNameMap().find(declaration) == SgNode::get_globalMangledNameMap().end());
                       }

#if 0
                 // DQ (10/8/2006): These may not have been set yet so we can't test them!
                    if (nondefiningDeclaration != NULL)
                         ROSE_ASSERT(declaration->get_name() == nondefiningDeclaration->get_name());
                    if ( (definingDeclaration != NULL) && (nondefiningDeclaration != NULL) )
                         ROSE_ASSERT(definingDeclaration->get_name() == nondefiningDeclaration->get_name());
#endif
                    break;
                  }

               case V_SgFunctionDeclaration:
                  {
                 // This case provides error checking only!
                    SgFunctionDeclaration* declaration            = isSgFunctionDeclaration(node);
                    SgFunctionDeclaration* definingDeclaration    = isSgFunctionDeclaration(declaration->get_definingDeclaration());
                    SgFunctionDeclaration* nondefiningDeclaration = isSgFunctionDeclaration(declaration->get_firstNondefiningDeclaration());

                    if (definingDeclaration != NULL)
                         ROSE_ASSERT(declaration->get_name() == definingDeclaration->get_name());
                    if (nondefiningDeclaration != NULL)
                         ROSE_ASSERT(declaration->get_name() == nondefiningDeclaration->get_name());
                    if ( (definingDeclaration != NULL) && (nondefiningDeclaration != NULL) )
                         ROSE_ASSERT(definingDeclaration->get_name() == nondefiningDeclaration->get_name());
                    break;
                  }

               default:
                  {
                    break;
                  }
             }
        }
   }

