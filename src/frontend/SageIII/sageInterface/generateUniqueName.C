#include "sage3basic.h"

// DQ (10/14/2006): Added supporting help functions
#include "transformationSupport.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;


// This function is local to this file
string
SageInterface::declarationPositionString( SgDeclarationStatement* declaration )
   {
  // This function generates a unique string for a declaration
  // and is used in the generateUniqueName() function.

     ROSE_ASSERT(declaration != NULL);
     Sg_File_Info* fileInfo = declaration->get_file_info();
     ROSE_ASSERT(fileInfo != NULL);

     int file_id       = fileInfo->get_file_id();
     int line_number   = fileInfo->get_line();
     int column_number = fileInfo->get_col();

     // Liao, 11/9/2009
     // for translation generated SgNode, the file_id might be negative, like -3
     // But naively inserting "-3" into the return string will break the rule for identifier in C/C++.
     // In this case, we convert it to N3
     string fileIdString;
     if (file_id<0)
     {
       fileIdString+="N";
       file_id = -1* file_id;
     }
     fileIdString+=StringUtility::numberToString(file_id);

     string returnString = "_F" + fileIdString + "_L" + StringUtility::numberToString(line_number) + "_C" +  StringUtility::numberToString(column_number);
     //string returnString = "_F" + StringUtility::numberToString(file_id) + "_L" + StringUtility::numberToString(line_number) + "_C" +  StringUtility::numberToString(column_number);
     
     // Liao, 11/9/2009
     // the returned string should not have '-', which will break the rule for identifier in C/C++
     size_t pos1 = returnString.find("-"); 
     ROSE_ASSERT (pos1 == string::npos);

     return returnString;
   }

string
SageInterface::generateUniqueName ( SgNode* node, bool ignoreDifferenceBetweenDefiningAndNondefiningDeclarations )
   {
  // This function handles details in the differences between 
  // the unique names that we require for declarations and where the
  // mangled name mechanism would map them to be the same.
  // Examples include:
  //    1) Function declarations (e.g. forward declarations and the defining declaration) 
  //       with and without default parameters specified all have the same mangled name.
  //    2) Forward function declarations and the defining function declaration all have 
  //       the same mangled name.
  //    3) Namespace declarations all mangle to the same namespace name but there can be 
  //       many of them (since they are re-entrant) so these need a more precise definition 
  //       of uniqueness.

     ROSE_ASSERT(node != NULL);
#if 0
     printf ("generateUniqueName(): node = %p = %s ignoreDifferenceBetweenDefiningAndNondefiningDeclarations = %s \n",
          node,node->class_name().c_str(),ignoreDifferenceBetweenDefiningAndNondefiningDeclarations ? "true" : "false");
#endif

     string key;
     string additionalSuffix;

     SgType* type = isSgType(node);
     if (type != NULL)
        {
          switch(type->variantT())
             {
               case V_SgClassType:
               case V_SgEnumType:
               case V_SgTypedefType:
               case V_SgNamedType:
                  {
                 // Handle case of named types which should be shared within the merged AST.
                 // These are multiply represented within the generated AST from EDG.  
                 // Note that a fixup pass on the AST (fixupTypes.[hC]) forces the same declaration
                 // (defining or nondefining) to be used for all SgNamedType objects referencing 
                 // the same declaration.
                    SgNamedType* namedType = isSgNamedType(node);
                    ROSE_ASSERT(namedType != NULL);
                    SgDeclarationStatement* declaration = namedType->get_declaration();
                    ROSE_ASSERT(declaration != NULL);
                    key = generateUniqueName(declaration,true);
                    additionalSuffix = "__namedType";
#if 0
                 // If we can make all SgClassTypes for an associated definition equivalent then 
                 // we can merge them using the AST merge mechanism.

                 // For now keep track of just the number of typedefs, later we will do better.
                 // I think maybe the fixup should fixup step should handle the merging of the lists.
                    int numberOfTypedefs = namedType->get_typedefs()->get_typedefs().size();
                    additionalSuffix = additionalSuffix + StringUtility::numberToString(numberOfTypedefs);
#endif
                    break;
                  }
#if 0
            // DQ (6/28/2010): I think we need to make this unique so that it will not be shared for now!
               case V_SgPointerType:
            // case V_SgReferenceType:
                  {
                    SgPointerType* pointerType = isSgPointerType(node);
                    ROSE_ASSERT(pointerType != NULL);

                    key = "__pointer_"; // + StringUtility::numberToString(pointerType);
                    additionalSuffix = additionalSuffix + StringUtility::numberToString(pointerType);
                    break;
                  }
#endif
            // All other types
               default:
                  {
                 // Note difference in function name get_mangled_name() and get_mangled()
                    key = type->get_mangled();

                 // DQ (7/4/2010): Use the original setting which allowed types to be shared.
                 // DQ (6/28/2010): I think we need to make this unique so that it will not be shared for now!
                 // additionalSuffix = "__type";
                 // additionalSuffix = "__type"+ StringUtility::numberToString(type);
                    additionalSuffix = "__type";

                 // printf ("default case of SgType: key = %s \n",key.c_str());

                    if (key.empty() == true)
                       {
                         printf ("Generated empty string from type->get_mangled() type = %p = %s \n",type,type->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                    break;
                  }
             }


       // DQ (6/25/2010): This should be less important now that we normalize the SgTypedefSeq IR nodes to have the same lists.
#if 0
       // DQ (7/1/2010): Note that this causes about 7 test codes to fail because if name qualification issues of 
       // global scoping operator. Not clear why, but this code is what restores thos test codes (in Cxx_tests) to 
       // working order.

       // Append a list of pointers to the types in the typedef sequence.
          SgTypePtrList & typeList = type->get_typedefs()->get_typedefs();
          SgTypePtrList::iterator i = typeList.begin();
          while (i != typeList.end())
             {
            // DQ (3/28/2007): Modified "=" to be "+="
            // Use a method that is independent of pointer values.
            // key = key + StringUtility::numberToString(*i) + "_";
            // key = generateUniqueName(*i,true);
               key += generateUniqueName(*i,true);
               i++;
             }
#endif
       // printf ("In SageInterface::generateUniqueName(): after adding typedefs for type = %s \n",key.c_str());

#if 0
       // DQ (7/4/2010): This is preventing newly built types from being used as base-types in the merge.
       // I think that we may not require this level of detail, but we migh have to fixup the base types
       // afterward.  Either that of rnormalize all the base-types so that we CAN use this code and still
       // generate matching names to permit the merge to work properly.

       // DQ (6/28/2010): I think we want to ignore this difference where possible.
       // DQ (3/28/2007): Add the information in get_ptr_to() and get_ref_to() member functions (if they have valid pointers)
          if (type->get_ptr_to() != NULL || type->get_ref_to() != NULL)
             {
               if (type->get_ptr_to() != NULL)
                  {
#if 0
                    printf ("In generateUniqueName(%p = %s): Adding in the type->get_ptr_to() = %p = %s to the generation of a unique name \n",type,type->class_name().c_str(),type->get_ptr_to(),type->get_ptr_to()->class_name().c_str());
#endif
                    key += "_with_pointer_to_type_";
                    key += generateUniqueName(type->get_ptr_to(),true);
                  }
               if (type->get_ref_to() != NULL)
                  {
#if 0
                    printf ("In generateUniqueName(%p = %s): Adding in the type->get_ref_to() = %p = %s to the generation of a unique name \n",type,type->class_name().c_str(),type->get_ref_to(),type->get_ref_to()->class_name().c_str());
#endif
                    key += "_with_reference_to_type_";
                    key += generateUniqueName(type->get_ref_to(),true);
                  }
             }
#endif

#if 0
          printf ("In SageInterface::generateUniqueName(): key for type = %s \n",key.c_str());
#endif
        }

     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
       // SgScopeStatement*       scopeStatement = isSgScopeStatement(node);
       // SgDeclarationStatement* declaration    = isSgDeclarationStatement(node);
#if 0
       // DQ (5/21/2006): skip computing the key for a SgFunctionParameterList (get_mangled_name() fails)
       // key = declaration->get_mangled_name();
          SgFunctionParameterList* parameterList = isSgFunctionParameterList(statement);
          if (parameterList == NULL)
             {
               SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
               if (declaration != NULL)
                    key = declaration->get_mangled_name();
             }
#endif
          switch(statement->variantT())
             {
               case V_SgFunctionTypeTable:
                  {
                    SgFunctionTypeTable* symbolTable = isSgFunctionTypeTable(node);
                    ROSE_ASSERT(symbolTable->get_parent() != NULL);
                    key = generateUniqueName(symbolTable->get_parent(),false);
                    additionalSuffix = "__function_type_table";
                    break;
                  }

            // ScopeStatements
               case V_SgClassDefinition:
               case V_SgTemplateInstantiationDefn:
                  {
                    SgClassDefinition* classDefinition = isSgClassDefinition(statement);
                    ROSE_ASSERT(classDefinition != NULL);
                 // SgClassDeclaration* classDeclaration = isSgClassDeclaration(classDefinition->get_parent());
                    SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
                    ROSE_ASSERT(classDeclaration != NULL);
                    key = classDeclaration->get_mangled_name();

                 // DQ (6/22/2006): Added support for SgTemplateInstantiationDefn
                    if (isSgTemplateInstantiationDefn(classDefinition) != NULL)
                         additionalSuffix = "__class_template_instantiation_definition";
                      else
                         additionalSuffix = "__class_definition";

                 // printf ("In case V_SgClassDefinition: key = %s \n",key.c_str());
                    if (key.empty() == true)
                       {
                      // printf ("In case V_SgClassDefinition: empty key calling generateUniqueName on declaration \n");
                         key = generateUniqueName(classDeclaration,true);
                         ROSE_ASSERT(key.empty() == false);
                       }

                 // printf ("case V_SgClassDefinition: scopeStatement = %p = %s classDeclaration = %p = %s \n",scopeStatement,scopeStatement->class_name().c_str(),classDeclaration,classDeclaration->class_name().c_str());
                 // printf ("case V_SgClassDefinition: key = %s additionalSuffix = %s \n",key.c_str(),additionalSuffix.c_str());
                 // ROSE_ASSERT(false);

                    break;
                  }

               case V_SgFunctionDefinition:
                  {
                    SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(statement);
                    ROSE_ASSERT(functionDefinition != NULL);
                 // SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionDefinition->get_parent());
                    SgFunctionDeclaration* functionDeclaration = functionDefinition->get_declaration();
                    ROSE_ASSERT(functionDeclaration != NULL);
                    key = functionDeclaration->get_mangled_name();
                    additionalSuffix = "__function_definition";

                 // printf ("In case V_SgFunctionDefinition: key = %s \n",key.c_str());
                    if (key.empty() == true)
                       {
                      // printf ("In case V_SgFunctionDefinition: empty key calling generateUniqueName on declaration \n");
                         key = generateUniqueName(functionDeclaration,true);
                         ROSE_ASSERT(key.empty() == false);
                       }
                    break;
                  }

            // Declarations

            // Generate a name for SgGlobal
               case V_SgGlobal:
                  {
                    key = "__global_file_id_";

                 // Make the key unique for each file!
                    SgGlobal* globalScope = isSgGlobal(statement);
                    int fileId = globalScope->get_file_info()->get_file_id();
                    key = key + StringUtility::numberToString(fileId);
                    break;
                  }

            // For now I would like to avoid sharing ASM statements so we make
            // them unique by using their pointer value in the generateed name.
               case V_SgAsmStmt:
                  {
                    key = "__asm_statement_";

                 // Make the key unique for each asm statement declaration!
                    key = key + StringUtility::numberToString(statement);
                    break;
                  }

               case V_SgFunctionDeclaration:
               case V_SgMemberFunctionDeclaration:
               case V_SgTemplateInstantiationFunctionDecl:
               case V_SgTemplateInstantiationMemberFunctionDecl:
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(statement);
                    key = functionDeclaration->get_mangled_name();

                 // if ( functionDeclaration == functionDeclaration->get_definingDeclaration() )
                    if (functionDeclaration->get_definition() != NULL)
                       {
#if 0
                      // This is a defining definition (but check to make sure)
                         if ( functionDeclaration != functionDeclaration->get_definingDeclaration() )
                            {
                              printf ("Error: functionDeclaration != functionDeclaration->get_definingDeclaration() \n");
                              printf ("     functionDeclaration = %p \n",functionDeclaration);
                              printf ("     functionDeclaration->get_definingDeclaration() = %p \n",functionDeclaration->get_definingDeclaration());
                              functionDeclaration->get_file_info()->display("functionDeclaration");
                              functionDeclaration->get_definingDeclaration()->get_file_info()->display("functionDeclaration->get_definingDeclaration()");
                            }
                         ROSE_ASSERT( functionDeclaration == functionDeclaration->get_definingDeclaration() );
#endif
                      // To avoid "int foo();" having the smae name as "int foo() {}" we need to fixup the unique names.
                      // This may be the technique used to handle function prototypes with defalt valued parameters and 
                      // namespaces as well.
                         additionalSuffix = "__function_declaration_with_definition";
                       }
                      else
                       {
                      // Check for default parameters (function protypes with default parameter should
                      // not be mixed with function prototypes that don't have default arguments).
                         bool defaultArgumentsSpecified = false;
                         SgInitializedNamePtrList::iterator i = functionDeclaration->get_args().begin();
                         while (i != functionDeclaration->get_args().end())
                            {
                              SgInitializedName* functionParameter = *i;
                              if (functionParameter->get_initptr() != NULL)
                                 {
                                // The presence of an initializer implies a default argument has been specified.
                                   defaultArgumentsSpecified = true;
                                 }

                              i++;
                            }

                      // Make the unique string different if the function contains default arguments
                         if (defaultArgumentsSpecified == true)
                            {
                              additionalSuffix = "__default_function_parameters_specified";
                            }
                       }

                    break;
                  }

               case V_SgFunctionParameterList:
                  {
                    SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(statement);
                    ROSE_ASSERT(functionParameterList != NULL);

                    key = functionParameterList->get_mangled_name();
                 // printf ("In case V_SgFunctionParameterList: key (mangled name) = %s \n",key.c_str());

                    additionalSuffix = "__function_parameter_list";

                 // ROSE_ASSERT(false);
                    ROSE_ASSERT(key.empty() == false);
#if 0
                    if (key.empty() == true)
                       {
                      // If the enum declaration is unnamed then give it a name in terms of its parent 
                      // (e.g. "typedef enum {} _G_fpos64_t;" could get a name that reflected it as an 
                      // enum_declaration (additionalSuffix) in a typedef specific to _G_fpos64_t).
                         SgNode* parentNode = functionParameterList->get_parent();
                         ROSE_ASSERT(parentNode != NULL);
                         switch(parentNode->variantT())
                            {
                           // These case handle where an enum declaration is embedded in a avriable or typedef declaration.
                              case V_SgFunctionDeclaration:
                                 {
                                   printf ("Error in generateUniqueName(): case V_SgFunctionDeclaration in case V_SgFunctionParameterList \n");
                                // key = generateUniqueName(parentNode);
                                   key = "function_for_which_mangling_fails";
                                   break;
                                 }

                              default:
                                 {
                                   printf ("Parent of SgFunctionParameterList = %p = %s is not handled \n",parentNode,parentNode->class_name().c_str());
                                   ROSE_ASSERT(false);
                                 }
                            }
                       }
#endif
                    break;
                  }

               case V_SgVariableDeclaration:
                  {
                    additionalSuffix = "__variable";

                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(statement);
                    key = variableDeclaration->get_mangled_name();
                 // printf ("In case V_SgVariableDeclaration: key (mangled name) = %s \n",key.c_str());

                 // DQ (5/10/2007): Fixed linkage to be a std::string instead of char*
                 // DQ (6/20/2006): Fixup to avoid "int x;" from being confused with "extern int x;"
                 // if ( (variableDeclaration->get_declarationModifier().get_storageModifier().isExtern() == true) && (variableDeclaration->get_linkage() == NULL) )
                    if ( (variableDeclaration->get_declarationModifier().get_storageModifier().isExtern() == true) && (variableDeclaration->get_linkage().empty()) )
                       {
                         additionalSuffix += "__extern_declaration ";
                       }

                 // DQ (6/1/2006): Mangled names are unique to a scope if the scope is named

                 // DQ (6/1/2006): Add the variables to the declaration (including types) to avoid 
                 // variable declarations from different unnamed scopes from clashing.
                    string variableNames;
                    SgInitializedNamePtrList::const_iterator p = variableDeclaration->get_variables().begin();
                    while ( p != variableDeclaration->get_variables().end() )
                       {
                      // variableNames += "_variable_name_" + (*p)->get_mangled_name() + (*p)->get_type()->get_mangled();
                         variableNames += "_variable_name_" + (*p)->get_mangled_name();
#if 1
                      // This causes problems when the initialize name objects are orphaned.  It seems we either over share or we oprhan IR nodes.
                      // DQ (2/17/2007): Initializers can make that initializedName different and difficult to 
                      // share since we can't generate unique names fro expressions.
                         if ( (*p)->get_initptr() != NULL )
                            {
                           // printf ("This is a SgInitializedName in a SgVariableDeclaration with an initializer so we can't share it! \n");
                           // variableNames = variableNames + StringUtility::numberToString(node);
                              variableNames = variableNames + generateUniqueName(*p,ignoreDifferenceBetweenDefiningAndNondefiningDeclarations);
                            }
#endif
                         p++;
                       }

                    key = key + variableNames;
                    break;
                  }

               case V_SgVariableDefinition:
                  {
                    SgVariableDefinition* variableDefinition = isSgVariableDefinition(statement);
                    key = variableDefinition->get_mangled_name();
                    additionalSuffix = "__variable_definition";
                    break;
                  }

               case V_SgTypedefDeclaration:
                  {
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(statement);
                    key = typedefDeclaration->get_mangled_name();
                    additionalSuffix = "__typedef_declaration";
                    break;
                  }

               case V_SgEnumDeclaration:
                  {
                 // additionalSuffix = "__enum_declaration";

                    SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(statement);
                    key = enumDeclaration->get_mangled_name();

                 // DQ (2/21/2007): Added to support AST merge.
                    if (enumDeclaration->get_name().getString().find_first_of("__generatedName_") != string::npos)
                       {
                         key += "__uniqueValue_" + declarationPositionString(enumDeclaration) + "_";

                      // printf ("Found an un-named class = %p = %s key = %s \n",enumDeclaration,enumDeclaration->get_name().str(),key.c_str());
                      // ROSE_ASSERT(false);
                       }

                 // printf ("case V_SgEnumDeclaration: key = %s \n",key.c_str());

                    if (ignoreDifferenceBetweenDefiningAndNondefiningDeclarations == true)
                       {
                      // Not sure which one to use (but enums at least always have a defining declaration)
                         additionalSuffix = "__enum_defining_declaration";
                       }
                      else
                       {
                         if ( enumDeclaration == enumDeclaration->get_definingDeclaration() )
                            {
                              additionalSuffix = "__enum_defining_declaration";
                            }
                           else
                            {
                              additionalSuffix = "__enum_nondefining_declaration";
                            }
                       }

                 // DQ (7/10/2010): Found a case where this fails, need more information about it.
                 // DQ (2/20/2007): I think that since we have reset the names of un-named enum declarations this should be possible to assert now!
                 // ROSE_ASSERT (key.empty() == false);

                 // DQ (7/10/2010): Found a case where this fails, need more information about it.
#if 1
                 // DQ (7/23/2010): We need to generate some sort of name to support generation 
                 // of mangled names for types to be put into the new type table.
                    if (key.empty() == true)
                       {
                         key = "__generated_name_";
                       }
#else
                    if (key.empty() == true)
                       {
                      // DQ (7/11/2010): This is not an error since we handle this case explicitly 
                      // by addating the pointer value to make it non-sharable.
                         printf ("WARNING: detected case of empty key constructed \n");
                         if (enumDeclaration->get_file_info() != NULL)
                            {
                              enumDeclaration->get_file_info()->display("WARNING: detected case of empty key constructed");
                            }

                      // If the enum declaration is unnamed then give it a name in terms of its parent 
                      // (e.g. "typedef enum {} _G_fpos64_t;" could get a name that reflected it as an 
                      // enum_declaration (additionalSuffix) in a typedef specific to _G_fpos64_t).
                         SgNode* parentNode = enumDeclaration->get_parent();
                         ROSE_ASSERT(parentNode != NULL);

                         switch(parentNode->variantT())
                            {
                           // These cases handle where an enum declaration is embedded in a variable or typedef declaration.
                              case V_SgVariableDeclaration:
                              case V_SgTypedefDeclaration:
                                 {
                                   key = generateUniqueName(parentNode,false);
                                   break;
                                 }

                              default:
                                 {
                                   printf ("Parent of SgEnumDeclaration = %p = %s is not handled \n",parentNode,parentNode->class_name().c_str());
                                   ROSE_ASSERT(false);
                                 }
                            }
#if 0
                      // DQ (7/11/2010): Commented out this assertion since we handle it above for now.
                         printf ("ERROR: detected case of empty key constructed \n");
                         ROSE_ASSERT (false);
#endif
                       }
#endif
                 // DQ (7/23/2010): I think that we can assert this here!
                    ROSE_ASSERT (key.empty() == false);
                    break;
                  }

               case V_SgTemplateDeclaration:
                  {
                    SgTemplateDeclaration* declaration = isSgTemplateDeclaration(statement);

                 // DQ (2/18/2007): Note that for template declarations built for member functions of a templated 
                 // class (EDG kind: templk_member_function) the declaration only has the template name (or the member 
                 // function and not its mangled name so we can't distinguish multiple overloaded functions of a 
                 // templated class.  This causes a bug for the AST merge (since the symbols are different, and all 
                 // but the first is orphaned).  The only way through this, that I can think of, is to number the
                 // functions in the template delcarations and permit the mangled names to reflect this!
                 // for now we will only add this feature to the "generateUniqueName()" function.
                    key = declaration->get_mangled_name();
                    if (declaration->get_template_kind() == SgTemplateDeclaration::e_template_m_function)
                       {
                      // printf ("Found a SgTemplateDeclaration which is a SgTemplateDeclaration::e_template_m_function \n");
                         SgTemplateInstantiationDefn* templateInstantiationDefnition = isSgTemplateInstantiationDefn(declaration->get_scope());
                      // printf ("Parent scope is templateInstantiationDefnition = %p \n",templateInstantiationDefnition);
                         if (templateInstantiationDefnition != NULL)
                            {
                              SgDeclarationStatementPtrList & declarationList = templateInstantiationDefnition->get_members();
                           // printf ("declarationList.size() = %ld \n",declarationList.size());

                              SgDeclarationStatementPtrList::iterator i = declarationList.begin();
                              int counter = 0;
                              bool found = false;
                              if (declarationList.begin() == declarationList.end())
                                   found = true;
                              while (found == false && i != declarationList.end())
                                 {
#if 0
                                   printf ("counter = %d *i = %p = %s \n",counter,*i,(*i)->class_name().c_str());
#endif
                                   counter++;
                                   SgTemplateInstantiationMemberFunctionDecl* memberFunctionDeclaration = isSgTemplateInstantiationMemberFunctionDecl(*i);
                                   if (memberFunctionDeclaration != NULL)
                                      {
                                     // See if the current SgTemplateDeclaration is the SgTemplateDeclaration 
                                     // associated with this SgTemplateInstantiationMemberFunctionDecl.
                                     // if (memberFunctionDeclaration->get_templateDeclaration() == declaration)
#if 0
                                        printf ("declaration->get_definingDeclaration()                                          = %p \n",declaration->get_definingDeclaration());
                                        printf ("memberFunctionDeclaration->get_templateDeclaration()->get_definingDeclaration() = %p \n",memberFunctionDeclaration->get_templateDeclaration()->get_definingDeclaration());
#endif
                                        ROSE_ASSERT(declaration->get_definingDeclaration() != NULL);
                                        ROSE_ASSERT(memberFunctionDeclaration->get_templateDeclaration()->get_definingDeclaration() != NULL);
                                        if (memberFunctionDeclaration->get_templateDeclaration()->get_definingDeclaration() == declaration->get_definingDeclaration())
                                           {
                                             found = true;
                                           }
                                      }
                                   i++;
                                 }

                           // Make sure that we really found the current declaration!
                              if (found == false)
                                 {
                                   printf ("Error: case SgTemplateDeclaration: declaration = %p name = %s \n",declaration,declaration->get_name().str());
                                   declaration->get_file_info()->display("Error: case SgTemplateDeclaration: debug");
                                 }
                              ROSE_ASSERT(found == true);
                              key += "__position_" + StringUtility::numberToString(counter) + "_";
                            }
                       }
                      else
                       {
                      // Not clear if there might be other nested declarations in a SgTemplateDeclaration that require similar handling!
                      // printf ("Must we handle other sorts of SgTemplateDeclaration declaration->get_template_kind() = %d \n",declaration->get_template_kind());
                       }

                    additionalSuffix = "__template_declaration";

                 // DQ (2/11/2007): distinquish between defining and nondefining declarations.
#if 0
                    printf ("ignoreDifferenceBetweenDefiningAndNondefiningDeclarations = %s \n",ignoreDifferenceBetweenDefiningAndNondefiningDeclarations ? "true" : "false");
                    printf ("declaration (case  SgTemplateDeclaration)      = %p \n",declaration);
                    printf ("declaration->get_definingDeclaration()         = %p \n",declaration->get_definingDeclaration());
                    printf ("declaration->get_firstNondefiningDeclaration() = %p \n",declaration->get_firstNondefiningDeclaration());
#endif
                    if (ignoreDifferenceBetweenDefiningAndNondefiningDeclarations == true)
                       {
                      // Make all the declarations the same as the defining declaration
                         additionalSuffix += "__defining_declaration";
                       }
                      else
                       {
                         if (declaration == declaration->get_definingDeclaration())
                            {
                              additionalSuffix += "__defining_declaration";
                            }
                           else
                            {
                              additionalSuffix += "__nondefining_declaration";
                            }
                       }

                    SgTemplateDeclaration::template_type_enum template_kind = declaration->get_template_kind();
                    switch(template_kind)
                       {
                         case SgTemplateDeclaration::e_template_none:
                              additionalSuffix += "_none";
                              break;
                         case SgTemplateDeclaration::e_template_class:
                              additionalSuffix += "_class";
                              break;
                         case SgTemplateDeclaration::e_template_m_class:
                              additionalSuffix += "_member_class";
                              break;
                         case SgTemplateDeclaration::e_template_function:
                              additionalSuffix += "_function";
                              break;
                         case SgTemplateDeclaration::e_template_m_function:
                              additionalSuffix += "_member_function";
                              break;
                         case SgTemplateDeclaration::e_template_m_data:
                              additionalSuffix += "_member_data";
                              break;

                         default:
                            {
                              printf ("Error: default reached \n");
                              ROSE_ASSERT(false);
                            }
                       }

                 // DQ (2/4/2007): To distinguish between template forward declaration and a template definition 
                 // I think we need the string to be appended.  Else we have to find something in EDG that will 
                 // help us identify this more explicitly.

                 // DQ (2/24/2007): To force overloaded non-defining declarations of templates to be distinct we 
                 // add the template string and have modified the IR so that even non-defining declaration carry 
                 // the template string of the defining declaration.
                 // printf ("declaration = %p declaration->get_string() = %s \n",declaration,declaration->get_string().str());
                    if (declaration->get_template_kind() == SgTemplateDeclaration::e_template_m_function)
                       {
                      // DQ (2/24/2007): If this is a member function of a templated class then we don't have the 
                      // template string and we have a counter based approach implemented above (best I could do 
                      // under the circumstances).  However, if the template declaration is outside of the class
                      // then the string is available (precisely the case that might not be handled well by the 
                      // mechanism that counts the location in the template scope, but likely redundant.
                      // In this case we can (and to be uniform should) include the template string.
#if 0
                         if (declaration->get_string().is_null() == false)
                            {
                              printf ("Error: declaration->get_string().is_null() == false declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                              declaration->get_file_info()->display("Error: declaration->get_string().is_null() == false");
                            }
                         ROSE_ASSERT(declaration->get_string().is_null() == true);
#endif
                         additionalSuffix += string("_member_function_template_string_not always_available_") + declaration->get_string();
                       }
                      else
                       {
                      // All other case we hold the template string in both the defining and non-defining declarations
                      // so that we can have the function parameters required to distinguish overloaded functions.
                         if (declaration->get_string().is_null() == true)
                            {
                              printf ("Error: declaration->get_string().is_null() == true declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                              declaration->get_file_info()->display("Error: declaration->get_string().is_null() == true");

                           // DQ (7/11/2010): This will make sure the IR node is unshared
                              additionalSuffix += string("_empty_template_string_") + StringUtility::numberToString(node);
                            }
                      // DQ (7/11/2010): Commenting out this assertion (triggered by astFileIO test test-read-large).
                      // ROSE_ASSERT(declaration->get_string().is_null() == false);

                         additionalSuffix += string("_template_string_") + declaration->get_string();
                       }
                    break;
                  }

               case V_SgUsingDeclarationStatement:
                  {
                    SgUsingDeclarationStatement* declaration = isSgUsingDeclarationStatement(statement);
                    key = declaration->get_mangled_name();
                    additionalSuffix = "__using_declaration";
                    break;
                  }

               case V_SgUsingDirectiveStatement:
                  {
                    SgUsingDirectiveStatement* declaration = isSgUsingDirectiveStatement(statement);
                    key = declaration->get_mangled_name();
                    additionalSuffix = "__using_directive";
                    break;
                  }

               case V_SgClassDeclaration:
               case V_SgTemplateInstantiationDecl:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(statement);
                    key = classDeclaration->get_mangled_name();

                 // if (classDeclaration->get_definition() != NULL)
                 // printf ("### classDeclaration = %p = %s = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
                    if (ignoreDifferenceBetweenDefiningAndNondefiningDeclarations == true)
                       {
                      // Not sure which one to use (but classes at least always have a nondefining declaration)
                         additionalSuffix = "__class_nondefining_declaration";
                       }
                      else
                       {
                         if ( classDeclaration == classDeclaration->get_definingDeclaration() )
                            {
                              additionalSuffix = "__class_defining_declaration";
                            }
                           else
                            {
                              additionalSuffix = "__class_nondefining_declaration";
                            }
                       }

                 // DQ (2/9/2007): Need to make sure the SgTemplateInstantiationDecl is different from SgClassDeclaration 
                 // to avoid sharing within AST merge.
                    SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(statement);
                    if (templateInstantiationDeclaration != NULL)
                       {
                         additionalSuffix += "__template_instantiation";
                       }
#if 1
                 // DQ (2/21/2007): If we use the symbol tabel to generate names then as we manipulate the symbol
                 // table the names will change which is a disaster for the AST merge algorithm!  So use this technique
                 // to generate the name that is stored in the declaration directly rather than generated dynamically.

                 // This code allows us to add a position counter to the un-named class which prevents it from 
                 // being merged with something else in the same scoep which has a different symbol!
                 // if (classDeclaration->get_name().is_null() == true)
                    if (classDeclaration->get_name().getString().find_first_of("__generatedName_") != string::npos)
                       {
                      // DQ (2/21/2007): Note that the classDeclaration may not match on in the scope (e.g. "typedef struct { int x; } Y;"
                      // where the "struct { int x; }" generates a SgClassSymbol in the symbol table but the declaration is not explicit
                      // in the scope where the SgTypedefDeclaration is declared.  So search the symbol table instead of the scope.
                      // unsigned int uniqueScopeValue = classDeclaration->get_scope()->generateUniqueStatementNumberForScope(classDeclaration);
                      // key += "__uniqueScopeValue_" + StringUtility::numberToString(uniqueScopeValue) + "_";
                      // unsigned int uniqueSymbolValue = classDeclaration->get_scope()->get_symbol_table()->generateUniqueNumberForMatchingSymbol(classDeclaration);
                      // key += "__uniqueSymbolValue_" + StringUtility::numberToString(uniqueSymbolValue) + "_";
                         key += "__uniqueValue_" + declarationPositionString(classDeclaration) + "_";

                      // printf ("Found an un-named class = %p = %s key = %s \n",classDeclaration,classDeclaration->get_name().str(),key.c_str());
                      // ROSE_ASSERT(false);
                       }
                      else
                       {
                      // Classes with normal names are not a problem.
                       }
#endif
#if 0
                 // We no longer generate names with pointer values
                 // Sometimes empty class names have internally generated name (e.g. "__generatedName_135418184")
                 // these will be a problem since they contain pointer values embedded as a suffix.
                    if (key.find_first_of("__generatedName_") != string::npos)
                       {
                         printf ("In generateUniqueName(): Found a generated name as a key = %s (reset to empty string) \n",key.c_str());
                      // ROSE_ASSERT(false);
                         key = "";
                       }
#endif

#if 1
                 // DQ (2/20/2007): I think that since we have reset the names of un-named class declarations this should be possible to assert now!
                    if (key.empty() == true)
                       {
                      // ROSE_ASSERT(classDeclaration->get_firstNondefiningDeclaration() != NULL);
                      // key = "__generatedName_" + StringUtility::numberToString(classDeclaration->get_firstNondefiningDeclaration());
                      // key = "__generatedName_" + StringUtility::numberToString(classDeclaration);
                         key = "__generatedName_";
                       }
                    ROSE_ASSERT (key.empty() == false);
#else
                 // DQ (7/23/2010): This is required again since I need it to generate names for keys in the new type table.
                    if (key.empty() == true)
                       {
                      // If the class declaration is unnamed then give it a name in terms of its parent 
                      // (e.g. "typedef struct {int __pos; int __state;} _G_fpos64_t;" could get a name
                      // that reflected it as a class_declaration (additionalSuffix) in a typedef specific 
                      // to _G_fpos64_t).
                         SgNode* parentNode = classDeclaration->get_parent();
                         ROSE_ASSERT(parentNode != NULL);

                      // DQ (2/9/2007):  Make sure that un-named classes can't be confused with there parent classes 
                      // so add this to distinguish the parent from the current IR node in what name is generated.
                         additionalSuffix += "__unnamed_";

#if 0
                         printf ("Note: could not generate a name from node = %p = %s recursing through parent = %p = %s to generate a unique name \n",
                             node,node->class_name().c_str(),parentNode,parentNode->class_name().c_str());
#endif
                         switch(parentNode->variantT())
                            {
                           // These cases handle where a class declaration is embedded in a variable or typedef declaration.
                              case V_SgVariableDeclaration:
                              case V_SgTypedefDeclaration:
                                 {
                                   key = generateUniqueName(parentNode,false);
                                   break;
                                 }
#if 1
                           // I don't know why this is required in stdio.h (for 
                           // "typedef struct _IO_FILE __FILE; extern int fsetpos (FILE *__stream, __const fpos_t *__pos);")
                              case V_SgFunctionParameterList:
                                 {
                                   key = generateUniqueName(parentNode,false);
                                   break;
                                 }
#endif
                           // DQ (2/8/2007): Required for name mangling!
                              case V_SgGlobal:
                              case V_SgClassDefinition:
                                 {
                                   key = generateUniqueName(parentNode,false);
                                   break;
                                 }

                              default:
                                 {
                                   printf ("Parent of SgClassDeclaration = %p = %s is not a handled \n",parentNode,parentNode->class_name().c_str());
                                   classDeclaration->get_file_info()->display("classDeclaration: Parent of SgClassDeclaration is not a handled");
                                   parentNode->get_file_info()->display("parentNode: Parent of SgClassDeclaration is not a handled");
                                   ROSE_ASSERT(false);
                                 }
                            }
                       }
#endif

#if 1
                 // DQ (6/23/2010): Added support to make classes with the same name different when they are
                 // different classes containing different members.
                    string memberNames = "_class_members_";
                    if (classDeclaration->get_definition() != NULL)
                       {
                         SgDeclarationStatementPtrList::const_iterator p = classDeclaration->get_definition()->get_members().begin();
                         while ( p != classDeclaration->get_definition()->get_members().end() )
                            {
                           // DQ (2/22/2007): Added type to generated mangled name for each variable (supports AST merge generation of name for un-named classes)
                           // memberNames += SgName("_member_type_") + (*p)->get_type()->get_mangled() + SgName("_member_name_") + (*p)->get_mangled_name();
                              memberNames += string("_member_name_") + SageInterface::get_name(*p);

                              p++;
                            }
                       }

                    additionalSuffix += memberNames;
#endif

                 // need to check for use of extern in declaration!
                 // additionalSuffix = "__class_declaration";
                    break;
                  }

            // There might be a longer list of special cases than this!
               case V_SgNamespaceAliasDeclarationStatement:
               case V_SgNamespaceDeclarationStatement:
                  {
                    SgDeclarationStatement* declaration = isSgDeclarationStatement(statement);
                    key = declaration->get_mangled_name();
                    break;
                  }

               case V_SgNamespaceDefinitionStatement:
                  {
                    SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(statement);
                    key = namespaceDefinition->get_namespaceDeclaration()->get_mangled_name();
                    break;
                  }

            // DQ (2/10/2007): Added support for unique names from template instatiation directives
               case V_SgTemplateInstantiationDirectiveStatement:
                  {
                    SgTemplateInstantiationDirectiveStatement* templateInstantiationDirective = isSgTemplateInstantiationDirectiveStatement(statement);
                    ROSE_ASSERT(templateInstantiationDirective->get_declaration() != NULL);
                    key = templateInstantiationDirective->get_declaration()->get_mangled_name();
                    additionalSuffix = "__template_instantiation_directive_declaration";
                    break;
                  }


            // DQ (5/6/2007): Added more cases
               case V_SgDefaultOptionStmt:

            // DQ (2/20/2007): Added more cases
               case V_SgSwitchStatement:
               case V_SgDoWhileStmt:
               case V_SgCaseOptionStmt:

               case V_SgCtorInitializerList:
               case V_SgTryStmt:
               case V_SgExprStatement:
               case V_SgCatchStatementSeq:
               case V_SgCatchOptionStmt:
               case V_SgNullStatement:
               case V_SgReturnStmt:
               case V_SgBreakStmt:
               case V_SgContinueStmt:
               case V_SgBasicBlock:
               case V_SgForStatement:
               case V_SgForInitStatement:
               case V_SgIfStmt:
               case V_SgWhileStmt:
               case V_SgLabelStatement: // RV 05/11/2007
               case V_SgGotoStatement:
                  {
                 // We don't want to generate a unique string for these statements
                    break;
                  }

               case V_SgPragmaDeclaration:
                  {
                    SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(node);
                    key = "__pragma_declaration_";
                    ROSE_ASSERT(pragmaDeclaration->get_pragma() != NULL);
                    key = key + generateUniqueName(pragmaDeclaration->get_pragma(),ignoreDifferenceBetweenDefiningAndNondefiningDeclarations);
                    break;
                  }
#if 0
               case V_SgDefaultOptionStmt:
                  {
                    SgDefaultOptionStmt* defaultOptionDeclaration = isSgDefaultOptionStmt(node);
                    key = "__default_option_declaration_";
                    ROSE_ASSERT(defaultOptionDeclaration->get_xxx() != NULL);
                    key = key + generateUniqueName(pragmaDeclaration->get_pragma(),ignoreDifferenceBetweenDefiningAndNondefiningDeclarations);
                    break;
                  }
#endif

               default:
                  {
                 // ignore these cases
                    printf ("Default reached in case of SgStatement in generateUniqueName(node = %p = %s) \n",node,node->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }

          SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(statement);
          if (declarationStatement != NULL)
             {
            // Build a prefix to contain the access permissions (and friend specifier)
               bool isFriend = declarationStatement->get_declarationModifier().isFriend();
               SgAccessModifier::access_modifier_enum accessPermisions = declarationStatement->get_declarationModifier().get_accessModifier().get_modifier();
               string accessString;
               switch (accessPermisions)
                  {
                    case SgAccessModifier::e_unknown: 
                       {
                         accessString = "__unknown_access_";
                         break;
                       }

                    case SgAccessModifier::e_private: 
                       {
                         accessString = "__private_access_";
                         break;
                       }

                    case SgAccessModifier::e_protected: 
                       {
                         accessString = "__protected_access_";
                         break;
                       }

                    case SgAccessModifier::e_public: 
                       {
                         accessString = "__public_access_";
                         break;
                       }
#if 0
                 // This case is equal to SgAccessModifier::e_public (so it is redundant to list it here)
                    case SgAccessModifier::e_default: 
                       {
                         accessString = "__default_access_";
                         break;
                       }
#endif
                    default:
                       {
                         printf ("Error: default reached in SageInterface::generateUniqueName (declaration prefix) \n");
                         ROSE_ASSERT(false);
                       }
                  }

               if (isFriend == true)
                  {
                    string friendString = (isFriend == true) ? "__friend_" : "";

                    key += friendString;

                 // DQ (2/6/2007): Processing stream header file appears to demonstrate that we do also need the accessString
                    key += accessString;
                  }
                 else
                  {
                    key += accessString;
                  }

            // DQ (7/16/2010): To simplify debugging add the line number to the generated string.
            // DQ (7/4/2010): Tests across separate files that are actually different programs are a problem if we don't
            // also include the filename.  The same "class X" might not be able to be merged across different programs.
            // We would not have to include this if we were merging files within a single program.
               string filename;
               string linenumber;
               if (declarationStatement->get_file_info() != NULL)
                  {
                 // ROSE_ASSERT(declarationStatement->get_file_info() != NULL);
                    filename = declarationStatement->get_file_info()->get_filename();
                    linenumber = StringUtility::numberToString(declarationStatement->get_file_info()->get_line());
                  }
                 else
                  {
                    filename = "unknown_file_and_pointer";
                    linenumber = StringUtility::numberToString(declarationStatement);
                  }
            // key += filename;
               key += filename + "_" + linenumber;
             }
        }

     SgSymbol* symbol = isSgSymbol(node);
     if (symbol != NULL)
        {
          switch(symbol->variantT())
             {
               case V_SgVariableSymbol:
                  {
                    SgVariableSymbol* valiableSymbol = isSgVariableSymbol(symbol);
                    SgInitializedName* initializedName = valiableSymbol->get_declaration();
                 // key = initializedName->get_mangled_name();
                    key = generateUniqueName(initializedName,false);
                    additionalSuffix = "__variable_symbol";
                    break;
                  }

               case V_SgClassSymbol:
                  {
                    SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                    SgDeclarationStatement* declaration = classSymbol->get_declaration();
                 // key = declaration->get_mangled_name();
                    key = generateUniqueName(declaration,false);
                    additionalSuffix = "__class_symbol";
                    break;
                  }

               case V_SgTypedefSymbol:
                  {
                    SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(symbol);
                    SgDeclarationStatement* declaration = typedefSymbol->get_declaration();
                    ROSE_ASSERT(declaration != NULL);
                    key = generateUniqueName(declaration,false);
                    additionalSuffix = "__typedef_symbol";
                    break;
                  }

               case V_SgFunctionSymbol:
                  {
                    SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
                    SgDeclarationStatement* declaration = functionSymbol->get_declaration();
                    key = generateUniqueName(declaration,false);
                    additionalSuffix = "__function_symbol";
                    break;
                  }

               case V_SgMemberFunctionSymbol:
                  {
                    SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(symbol);
                    SgDeclarationStatement* declaration = memberFunctionSymbol->get_declaration();
                    key = generateUniqueName(declaration,false);
                    additionalSuffix = "__member_function_symbol";
                    break;
                  }

               case V_SgEnumSymbol:
                  {
                    SgEnumSymbol* enumSymbol = isSgEnumSymbol(symbol);
                    SgDeclarationStatement* declaration = enumSymbol->get_declaration();
                 // key = declaration->get_mangled_name();
                    key = generateUniqueName(declaration,false);
                    additionalSuffix = "__enum_symbol";
                    break;
                  }

               case V_SgNamespaceSymbol:
                  {
                    SgNamespaceSymbol* namespaceSymbol = isSgNamespaceSymbol(symbol);
                    ROSE_ASSERT(namespaceSymbol != NULL);
                    SgDeclarationStatement* declaration = namespaceSymbol->get_declaration();
#if 1
                 // DQ (7/4/1020): Unclear why we can have a declaration == NULL, so allow this to generate a unique name for now!
                 // Perhaps this is for a "using namespace std" without "std defined" (which is allows in C++).
                 // Or it coudle the for an un-named namespace (also allowed in C++).
                    if (declaration == NULL)
                       {
                         key = "__namespace_with_null_declaration_" + StringUtility::numberToString(symbol);
                       }
                      else
                       {
                         key = generateUniqueName(declaration,false);
                       }
#else
                    ROSE_ASSERT(declaration != NULL);
                    key = generateUniqueName(declaration,false);
#endif
                    additionalSuffix = "__namespace_symbol";
                    break;
                  }

               case V_SgEnumFieldSymbol:
                  {
                    SgEnumFieldSymbol* enumFieldSymbol = isSgEnumFieldSymbol(symbol);
                    SgInitializedName* declaration = enumFieldSymbol->get_declaration();
                 // key = declaration->get_mangled_name();
                    key = generateUniqueName(declaration,false);
                    additionalSuffix = "__enum_field_symbol";
                    break;
                  }

               case V_SgTemplateSymbol:
                  {
                    SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(symbol);
                    SgTemplateDeclaration* declaration = templateSymbol->get_declaration();
                    key = generateUniqueName(declaration,false);
                    additionalSuffix = "__template_symbol";
                    break;
                  }

               case V_SgFunctionTypeSymbol:
                  {
                    SgFunctionTypeSymbol* functionTypeSymbol = isSgFunctionTypeSymbol(symbol);
                    key = functionTypeSymbol->get_name();
                    additionalSuffix = "__function_type_symbol";
                    break;
                  }

            // RV: 05/11/2007
               case V_SgLabelSymbol:
                  {
                    SgLabelSymbol* labelSymbol = isSgLabelSymbol (symbol);
                    key = labelSymbol->get_name();
                    additionalSuffix = "__label_symbol";
                    break;
                  }

            // DQ (7/4/2010): It might be that these should not be shared!
               case V_SgAliasSymbol:
                  {
                    SgAliasSymbol* aliasSymbol = isSgAliasSymbol (symbol);
                    key = aliasSymbol->get_name();
                    additionalSuffix = "__alias_symbol";
                    break;
                  }

            // All other SgSymbols
               default:
                  {
                    printf ("Error: default reached in generateUniqueName() symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
                  }
             }
        }

  // Never share expressions within the merge process
     SgExpression* expression = isSgExpression(node);
     if (expression != NULL)
        {
       // printf ("expression = %p = %s \n",expression,expression->class_name().c_str());

          switch(expression->variantT())
             {
            // All other SgSymbols
               default:
                  {
                    key = "__expression_";
                 // Make the key unique for each file info object!
                    key = key + StringUtility::numberToString(node);
                    break;
                  }
             }
        }

     SgSupport* support = isSgSupport(node);
     if (support != NULL)
        {
       // printf ("support = %p = %s \n",support,support->class_name().c_str());

          switch(support->variantT())
             {
               case V_SgProject:
                  {
                    key = "__project";
                 // Make the key unique for each file info object!
                    key = key + StringUtility::numberToString(node);
                    break;
                  }

            // case V_SgFile:
               case V_SgSourceFile:
               case V_SgBinaryComposite:
                  {
                    key = "__sourceFile_file_id_";

                 // Make the key unique for each file!
                    SgFile* file = isSgFile(node);
                    int fileId = file->get_file_info()->get_file_id();
                    key = key + StringUtility::numberToString(fileId);
                    break;
                  }

               case V_SgInitializedName:
                  {
                    SgInitializedName* initializedName = isSgInitializedName(node);
                 // Make the mangled name from a SgInitializedName unique (not finished yet).
                 // This case will handle "extern A::x" vs. "namespace A { int x; }" which
                 // I expect will gnerate the same unique name but which are clearly different!

                 // Check for use of extern keyword (I think this is enough!)
                    key = initializedName->get_mangled_name();

                 // DQ (6/1/2006): Add the type to avoid variable declarations from different unnamed scopes from clashing.
                    string type = initializedName->get_type()->get_mangled();
                    key = key + type;

                    additionalSuffix = "__initialized_name";

                 // DQ (6/20/2006): Fixup to avoid "int x;" from being confused with "extern int x;"
                    SgStorageModifier & storage = initializedName->get_storageModifier();
                    if (storage.isExtern())
                       {
                         additionalSuffix += "__extern_initialized_name";
                       }
#if 1
                 // DQ (2/17/2007): This causes problems and does not avoid orphaned initializers, 
                 // however without it SgInitializedName objects would be over shared!
                 // DQ (2/17/2007): Initializers can make that initializedName different and difficult to 
                 // share since we can't generate unique names fro expressions.
                    if (initializedName->get_initptr() != NULL)
                       {
                      // printf ("This is a SgInitializedName with an initializer so we can't share it! \n");
                      // key = key + StringUtility::numberToString(node);
                         key = key + initializedName->get_initptr()->unparseToString();
                       }
#else
                 // Maybe we need to unparse the initializer expressions so that they can be a part of the unique name that we generate!
                 // This would allow the perfect level of sharing (withouth oversharing or undersharing).
                    printf (" Possible over sharing of SgInitializedName objects because we don't account for initializer expressions \n");
#endif

                 // DQ (3/3/2007): If this is part of a function parameter list then we want to record if it is a defining or 
                 // non-defining function (symbols for parameters of non-defining functions are not placed into the symbol table 
                 // and parameters of defining functions are placed into the defining function's function scope's symbol table.  
                 // The unique name we genetate can effect the symbols so we want to avoid having symbols moved inappropriately 
                 // due to over sharing from parameter names that are not sufficiently unique!
                    SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(node->get_parent());
                    if (functionParameterList != NULL)
                       {
                      // This is a function parameter
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionParameterList->get_parent());
                         ROSE_ASSERT(functionDeclaration != NULL);
                         if (functionDeclaration == functionDeclaration->get_definingDeclaration())
                            {
                              key = key + "_parameter_of_defining_declaration";
                            }
                           else
                            {
                              key = key + "_parameter_of_nondefining_declaration";
                            }
                       }

                 // DQ (3/7/2007): This could be a static variable in a class which will make two appearences in the source code.
                 // The first will be in the class as a declaration and the second will be outside the class (as a declaration 
                 // from the point of view of ROSE, but as a means of allocating space from teh pointof view of C++).
                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node->get_parent());
                    if (variableDeclaration != NULL)
                       {
                      // To make this unique, append the mangled name of the scope of the variableDeclaration
                         key += "_in_scope_" + variableDeclaration->get_scope()->get_mangled_name();
                       }
                    break;
                  }

            // DQ (7/24/2010): Added to support local and global type tables.
               case V_SgTypeTable:
                  {
                    SgTypeTable* symbolTable = isSgTypeTable(node);
                 // ROSE_ASSERT(symbolTable->get_parent() != NULL);
                 // key = generateUniqueName(symbolTable->get_parent(),false);
                    key = "__type_table_" + StringUtility::numberToString(node);
                    additionalSuffix = "__type_table";
                    break;
                  }

               case V_SgSymbolTable:
                  {
                    SgSymbolTable* symbolTable = isSgSymbolTable(node);
                    ROSE_ASSERT(symbolTable->get_parent() != NULL);
                    key = generateUniqueName(symbolTable->get_parent(),false);
                    additionalSuffix = "__symbol_table";
                    break;
                  }

               case V_SgStorageModifier:
                  {
                    SgStorageModifier* storageModifier = isSgStorageModifier(node);
                    ROSE_ASSERT(storageModifier->get_parent() != NULL);
                    key = generateUniqueName(storageModifier->get_parent(),false);
                    additionalSuffix = "__storage_modifier";
                    break;
                  }

               case V_Sg_File_Info:
                  {
                    key = "__file_info_";
                 // Make the key unique for each file info object!
                    key = key + StringUtility::numberToString(node);
                    break;
                  }

               case V_SgFunctionParameterTypeList:
                  {
                    key = "__function_parameter_type_list_";
                 // Make the key unique for each file info object!
                    key = key + StringUtility::numberToString(node);
                    break;
                  }

               case V_SgTypedefSeq:
                  {
#if 0
                    SgSymbolTable* symbolTable = isSgSymbolTable(node);
                    ROSE_ASSERT(symbolTable->get_parent() != NULL);
                    key = generateUniqueName(symbolTable->get_parent(),false);
                    additionalSuffix = "__symbol_table";
#else
                    key = "__typedef_sequence_";
                 // Make the key unique for each file info object!
                    key = key + StringUtility::numberToString(node);
#endif
                    break;
                  }

               case V_SgTemplateArgument:
                  {
                    SgTemplateArgument* templateArgument = isSgTemplateArgument(node);
                    key = "__template_argument_";
                 // Make the key unique for each file info object!
                 // key = key + StringUtility::numberToString(node);
                    switch(templateArgument->get_argumentType())
                       {
                         case SgTemplateArgument::argument_undefined:
                            {
                              printf ("Error: SgTemplateArgument::argument_undefined reached in switch \n");
                              ROSE_ASSERT(false);
                              break;
                            }
                         case SgTemplateArgument::type_argument:
                            {
                              key += templateArgument->get_type()->get_mangled().str();
                              break;
                            }
                         case SgTemplateArgument::nontype_argument:
                            {
#if 1
                           // printf ("Error: SgTemplateArgument::nontype_argument reached (not implemented yet, currently generating pointer value into return string) \n");
                           // printf ("     templateArgument->get_expression() = %p = %s \n",
                           //      templateArgument->get_expression(),templateArgument->get_expression()->class_name().c_str());
                           // printf ("     templateArgument->get_parent() = %p \n",templateArgument->get_expression()->get_parent());

                           // DQ (3/7/2007): Use the value in the generated name so that it can be shared.
                              SgExpression* subExpression = templateArgument->get_expression();
                              ROSE_ASSERT(subExpression != NULL);
                              SgValueExp* valueExpression = isSgValueExp(subExpression);
                              if (valueExpression != NULL)
                                 {
                                   key += "__" + valueExpression->class_name() + "__" + valueExpression->unparseToString();
                                 }
                                else
                                 {
                                // We can maybe do this as well, but also append the pointer value just to make sure this will not be shared
                                   key += "__" + subExpression->class_name() + "__" + subExpression->unparseToString();

                                // This will make sure the IR node is unshared
                                   key += StringUtility::numberToString(node);
                                 }
#else
                           // printf ("     templateArgument->get_expression() = %s \n",templateArgument->get_expression()->unparseToString().c_str());
                           // ROSE_ASSERT(false);
                              key += StringUtility::numberToString(node);
#endif
                              break;
                            }

                      // DQ (7/11/2010): In astFileIO test test-read-large we demonstrate an example of this case.
                         case SgTemplateArgument::template_template_argument:
                            {
                           // This will make sure the IR node is unshared
                              key += StringUtility::numberToString(node);
#if 1
                              printf ("Warning: SgTemplateArgument::template_template_argument reached (not implemented yet) \n");
#else
                              printf ("Error: SgTemplateArgument::template_template_argument reached (not implemented yet) \n");
                              ROSE_ASSERT(false);
#endif
                              break;
                            }

                         default:
                            {
                              printf ("Error: default reached \n");
                              ROSE_ASSERT(false);
                            }
                       }
                    break;
                  }

               case V_SgQualifiedName:
                  {
                    key = "__qualified_name_";
                 // Make the key unique for each file info object!
                    key = key + StringUtility::numberToString(node);
                    break;
                  }

            // DQ (1/19/2007): previously unhandled case ...
               case V_SgBaseClassModifier:
                  {
                    key = "__base_class_modifier_";
                 // Make the key unique for each file info object!
                    key = key + StringUtility::numberToString(node);
                    break;
                  }

            // DQ (1/19/2007): previously unhandled case ...
               case V_SgBaseClass:
                  {
                    SgBaseClass* baseClass = isSgBaseClass(node);
                    key = "__base_class_";
                    ROSE_ASSERT(baseClass->get_base_class() != NULL);
                    key = key + generateUniqueName(baseClass->get_base_class(),false);
                    break;
                  }

               case V_SgPragma:
                  {
                    SgPragma* pragma = isSgPragma(node);
                    key = "__pragma_";
                 // Make the key unique for each file info object!
                 // key = key + StringUtility::numberToString(node);
                    key = key + pragma->get_name();
                    break;
                  }

            // DQ (1/23/2010): previously unhandled case ...
               case V_SgFileList:
                  {
                    key = "__file_list_";
                 // Make the key unique for each SgFileList object!
                    key = key + StringUtility::numberToString(node);
                    break;
                  }

            // DQ (1/23/2010): previously unhandled case ...
               case V_SgDirectoryList:
                  {
                    key = "__directory_list_";
                 // Make the key unique for each SgDirectoryList object!
                    key = key + StringUtility::numberToString(node);
                    break;
                  }

               default:
                  {
                    printf ("Error: default reached in generateUniqueName() node = %p = %s \n",node,node->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
                  }
             }
        }



  // Add the suffix that makes this IR nodes key different as required
     if (key.empty() == false)
        {
          key += additionalSuffix;
        }

  // I would like to avoid putting empty strings into the mangled name map!
  // ROSE_ASSERT(key.empty() == false);
     if (key.empty() == true)
        {
#if 0
          printf ("Empty key generated (should be ignored): node = %p = %s \n",node,node->class_name().c_str());
#endif
#if 0
          if (node->get_file_info() != NULL)
               node->get_file_info()->display("Empty key generated (should be ignored)");
#endif
        }

#if 0
     printf ("     In generateUniqueName: node = %p = %s additionalSuffix = %s generated key = %s \n",node,node->class_name().c_str(),additionalSuffix.c_str(),key.c_str());
#endif

  // ROSE_ASSERT(key.empty() == false);

     return key;
   }

