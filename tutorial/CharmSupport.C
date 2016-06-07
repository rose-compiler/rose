// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it shows the design of a transformation to do a transformation specific for Charm++.

#include "rose.h"

using namespace std;

Rose_STL_Container<SgInitializedName*>
buildListOfGlobalVariables ( SgSourceFile* file )
   {
  // This function builds a list of global variables (from a SgFile).
     assert(file != NULL);

     Rose_STL_Container<SgInitializedName*> globalVariableList;

     SgGlobal* globalScope = file->get_globalScope();
     assert(globalScope != NULL);
     Rose_STL_Container<SgDeclarationStatement*>::iterator i = globalScope->get_declarations().begin();
     while(i != globalScope->get_declarations().end())
        {
          SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration(*i);
          if (variableDeclaration != NULL)
             {
               Rose_STL_Container<SgInitializedName*> & variableList = variableDeclaration->get_variables();
               Rose_STL_Container<SgInitializedName*>::iterator var = variableList.begin();
               while(var != variableList.end())
                  {
                    globalVariableList.push_back(*var);
                    var++;
                  }
             }
          i++;
        }

     return globalVariableList;
   }

// This function is not used, but is useful for 
// generating the list of all global variables
Rose_STL_Container<SgInitializedName*>
buildListOfGlobalVariables ( SgProject* project )
   {
  // This function builds a list of global variables (from a SgProject).

     Rose_STL_Container<SgInitializedName*> globalVariableList;

     const SgFilePtrList& fileList = project->get_fileList();
     SgFilePtrList::const_iterator file = fileList.begin();

  // Loop over the files in the project (multiple files exist 
  // when multiple source files are placed on the command line).
     while(file != fileList.end())
        {
          Rose_STL_Container<SgInitializedName*> fileGlobalVariableList = buildListOfGlobalVariables(isSgSourceFile(*file));

       // DQ (9/26/2007): Moved from std::list to std::vector
       // globalVariableList.merge(fileGlobalVariableList);
          globalVariableList.insert(globalVariableList.begin(),fileGlobalVariableList.begin(),fileGlobalVariableList.end());

          file++;
        }

     return globalVariableList;
   }

Rose_STL_Container<SgVarRefExp*>
buildListOfVariableReferenceExpressionsUsingGlobalVariables ( SgNode* node )
   {
  // This function builds a list of "uses" of variables (SgVarRefExp IR nodes) within the AST.

  // return variable
     Rose_STL_Container<SgVarRefExp*> globalVariableUseList;

  // list of all variables (then select out the global variables by testing the scope)
     Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree ( node, V_SgVarRefExp );

     Rose_STL_Container<SgNode*>::iterator i = nodeList.begin();
     while(i != nodeList.end())
        {
          SgVarRefExp *variableReferenceExpression = isSgVarRefExp(*i);
          assert(variableReferenceExpression != NULL);

          assert(variableReferenceExpression->get_symbol() != NULL);
          assert(variableReferenceExpression->get_symbol()->get_declaration() != NULL);
          assert(variableReferenceExpression->get_symbol()->get_declaration()->get_scope() != NULL);

       // Note that variableReferenceExpression->get_symbol()->get_declaration() returns the 
       // SgInitializedName (not the SgVariableDeclaration where it was declared)!
          SgInitializedName* variable = variableReferenceExpression->get_symbol()->get_declaration();

          SgScopeStatement* variableScope = variable->get_scope();

       // Check if this is a variable declared in global scope, if so, then save it
          if (isSgGlobal(variableScope) != NULL)
             {
               globalVariableUseList.push_back(variableReferenceExpression);
             }
          i++;
        }

     return globalVariableUseList;
   }


SgClassDeclaration*
buildClassDeclarationAndDefinition (string name, SgScopeStatement* scope)
   {
  // This function builds a class declaration and definition 
  // (both the defining and nondefining declarations as required).

  // Build a file info object marked as a transformation
     Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
     assert(fileInfo != NULL);

  // This is the class definition (the fileInfo is the position of the opening brace)
     SgClassDefinition* classDefinition   = new SgClassDefinition(fileInfo);
     assert(classDefinition != NULL);

  // Set the end of construct explictly (where not a transformation this is the location of the closing brace)
     classDefinition->set_endOfConstruct(fileInfo);

  // This is the defining declaration for the class (with a reference to the class definition)
     SgClassDeclaration* classDeclaration = new SgClassDeclaration(fileInfo,name.c_str(),SgClassDeclaration::e_struct,NULL,classDefinition);
     assert(classDeclaration != NULL);

  // Set the defining declaration in the defining declaration!
     classDeclaration->set_definingDeclaration(classDeclaration);

  // Set the non defining declaration in the defining declaration (both are required)
     SgClassDeclaration* nondefiningClassDeclaration = new SgClassDeclaration(fileInfo,name.c_str(),SgClassDeclaration::e_struct,NULL,NULL);
     assert(classDeclaration != NULL);
     nondefiningClassDeclaration->set_scope(scope);
     nondefiningClassDeclaration->set_type(SgClassType::createType(nondefiningClassDeclaration));

  // Set the internal reference to the non-defining declaration
     classDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);
     classDeclaration->set_type(nondefiningClassDeclaration->get_type());

  // Set the defining and no-defining declarations in the non-defining class declaration!
     nondefiningClassDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);
     nondefiningClassDeclaration->set_definingDeclaration(classDeclaration);

  // Set the nondefining declaration as a forward declaration!
     nondefiningClassDeclaration->setForward();

  // Don't forget the set the declaration in the definition (IR node constructors are side-effect free!)!
     classDefinition->set_declaration(classDeclaration);

  // set the scope explicitly (name qualification tricks can imply it is not always the parent IR node!)
     classDeclaration->set_scope(scope);

  // some error checking
     assert(classDeclaration->get_definingDeclaration() != NULL);
     assert(classDeclaration->get_firstNondefiningDeclaration() != NULL);
     assert(classDeclaration->get_definition() != NULL);

  // DQ (9/8/2007): Need to add function symbol to global scope!
     printf ("Fixing up the symbol table in scope = %p = %s for class = %p = %s \n",scope,scope->class_name().c_str(),classDeclaration,classDeclaration->get_name().str());
     SgClassSymbol* classSymbol = new SgClassSymbol(classDeclaration);
     scope->insert_symbol(classDeclaration->get_name(),classSymbol);
     ROSE_ASSERT(scope->lookup_class_symbol(classDeclaration->get_name()) != NULL);

     return classDeclaration;
   }



SgVariableSymbol* 
putGlobalVariablesIntoClass (Rose_STL_Container<SgInitializedName*> & globalVariables, SgClassDeclaration* classDeclaration )
   {
  // This function iterates over the list of global variables and inserts them into the iput class definition

     SgVariableSymbol* globalClassVariableSymbol = NULL;

     for (Rose_STL_Container<SgInitializedName*>::iterator var = globalVariables.begin(); var != globalVariables.end(); var++)
        {
       // printf ("Appending global variable = %s to new globalVariableContainer \n",(*var)->get_name().str());
          SgVariableDeclaration* globalVariableDeclaration = isSgVariableDeclaration((*var)->get_parent());
          assert(globalVariableDeclaration != NULL);

       // Get the global scope from the global variable directly
          SgGlobal* globalScope = isSgGlobal(globalVariableDeclaration->get_scope());
          assert(globalScope != NULL);

          if (var == globalVariables.begin())
             {
            // This is the first time in this loop, replace the first global variable with 
            // the class declaration/definition containing all the global variables!
            // Note that initializers in the global variable declarations require modification 
            // of the preinitialization list in the class's constructor!  I am ignoring this for now!
               globalScope->replace_statement(globalVariableDeclaration,classDeclaration);

            // Build source position informaiton (marked as transformation)
               Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
               assert(fileInfo != NULL);

            // Add the variable of the class type to the global scope!
               SgClassType* variableType = new SgClassType(classDeclaration->get_firstNondefiningDeclaration());
               assert(variableType != NULL);
               SgVariableDeclaration* variableDeclaration = new SgVariableDeclaration(fileInfo,"AMPI_globals",variableType);
               assert(variableDeclaration != NULL);

               globalScope->insert_statement(classDeclaration,variableDeclaration,false);

               assert(variableDeclaration->get_variables().empty() == false);
               SgInitializedName* variableName = *(variableDeclaration->get_variables().begin());
               assert(variableName != NULL);

            // DQ (9/8/2007): Need to set the scope of the new variable.
               variableName->set_scope(globalScope);

            // build the return value
               globalClassVariableSymbol = new SgVariableSymbol(variableName);

            // DQ (9/8/2007): Need to add the symbol to the global scope (new testing requires this).
               globalScope->insert_symbol(variableName->get_name(),globalClassVariableSymbol);
               ROSE_ASSERT(globalScope->lookup_variable_symbol(variableName->get_name()) != NULL);
             }
            else
             {
            // for all other iterations of this loop ... 
            // remove variable declaration from the global scope
               globalScope->remove_statement(globalVariableDeclaration);
             }

       // add the variable declaration to the class definition
          classDeclaration->get_definition()->append_member(globalVariableDeclaration);
        }

     return globalClassVariableSymbol;
   }


void
fixupReferencesToGlobalVariables ( Rose_STL_Container<SgVarRefExp*> & variableReferenceList, SgVariableSymbol* globalClassVariableSymbol)
   {
  // Now fixup the SgVarRefExp to reference the global variables through a struct
     for (Rose_STL_Container<SgVarRefExp*>::iterator var = variableReferenceList.begin(); var != variableReferenceList.end(); var++)
        {
          assert(*var != NULL);
       // printf ("Variable reference for %s \n",(*var)->get_symbol()->get_declaration()->get_name().str());

          SgNode* parent = (*var)->get_parent();
          assert(parent != NULL);

       // If this is not an expression then is likely a meaningless statement such as ("x;")
          SgExpression* parentExpression = isSgExpression(parent);
          assert(parentExpression != NULL);

       // Build the reference through the global class variable ("x" --> "AMPI_globals.x")

       // Build source position informaiton (marked as transformation)
          Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
          assert(fileInfo != NULL);

       // Build "AMPI_globals"
          SgExpression* lhs = new SgVarRefExp(fileInfo,globalClassVariableSymbol);
          assert(lhs != NULL);
       // Build "AMPI_globals.x" from "x"
          SgDotExp* globalVariableReference = new SgDotExp(fileInfo,lhs,*var);
          assert(globalVariableReference != NULL);

          if (parentExpression != NULL)
             {
            // Introduce reference to *var through the data structure

            // case of binary operator
               SgUnaryOp* unaryOperator = isSgUnaryOp(parentExpression);
               if (unaryOperator != NULL)
                  {
                    unaryOperator->set_operand(globalVariableReference);
                  }
                 else
                  {
                 // case of binary operator
                    SgBinaryOp* binaryOperator = isSgBinaryOp(parentExpression);
                    if (binaryOperator != NULL)
                       {
                      // figure out if the *var is on the lhs or the rhs
                         if (binaryOperator->get_lhs_operand() == *var)
                            {
                              binaryOperator->set_lhs_operand(globalVariableReference);
                            }
                           else
                            {
                              assert(binaryOperator->get_rhs_operand() == *var);
                              binaryOperator->set_rhs_operand(globalVariableReference);
                            }
                       }
                      else
                       {
                      // ignore these cases for now!
                         switch(parentExpression->variantT())
                            {
                           // Where the variable appers in the function argument list the parent is a SgExprListExp
                              case V_SgExprListExp:
                                 {
                                   printf ("Sorry not implemented, case of global variable in function argument list ... \n");
                                   assert(false);
                                   break;
                                 }
                              case V_SgInitializer:
                              case V_SgRefExp:
                              case V_SgVarArgOp:
                              default:
                                 {
                                   printf ("Error: default reached in switch  parentExpression = %p = %s \n",parentExpression,parentExpression->class_name().c_str());
                                   assert(false);
                                 }
                            }
                       }
                  }
             }
        }
   }

#define OUTPUT_NAMES_OF_GLOBAL_VARIABLES           0
#define OUTPUT_NAMES_OF_GLOBAL_VARIABLE_REFERENCES 0

void
transformGlobalVariablesToUseStruct ( SgSourceFile *file )
   {
     assert(file != NULL);

  // These are the global variables in the input program (provided as helpful information)
     Rose_STL_Container<SgInitializedName*> globalVariables = buildListOfGlobalVariables(file);

#if OUTPUT_NAMES_OF_GLOBAL_VARIABLES
     printf ("global variables (declared in global scope): \n");
     for (Rose_STL_Container<SgInitializedName*>::iterator var = globalVariables.begin(); var != globalVariables.end(); var++)
        {
          printf ("   %s \n",(*var)->get_name().str());
        }
     printf ("\n");
#endif

  // get the global scope within the first file (currently ignoring all other files)
     SgGlobal* globalScope = file->get_globalScope();
     assert(globalScope != NULL);

  // Build the class declaration
     SgClassDeclaration* classDeclaration = buildClassDeclarationAndDefinition("AMPI_globals_t",globalScope);

  // Put the global variables intothe class
     SgVariableSymbol* globalClassVariableSymbol = putGlobalVariablesIntoClass(globalVariables,classDeclaration);

  // Their associated symbols will be located within the project's AST 
  // (where they occur in variable reference expressions).
     Rose_STL_Container<SgVarRefExp*> variableReferenceList = buildListOfVariableReferenceExpressionsUsingGlobalVariables(file);

#if OUTPUT_NAMES_OF_GLOBAL_VARIABLE_REFERENCES
     printf ("global variables appearing in the application: \n");
     for (Rose_STL_Container<SgVarRefExp*>::iterator var = variableReferenceList.begin(); var != variableReferenceList.end(); var++)
        {
          printf ("   %s \n",(*var)->get_symbol()->get_declaration()->get_name().str());
        }
     printf ("\n");
#endif

  // Fixup all references to global variable to access the variable through the class ("x" --> "AMPI_globals.x")
     fixupReferencesToGlobalVariables(variableReferenceList,globalClassVariableSymbol);
   }


void
transformGlobalVariablesToUseStruct ( SgProject *project )
   {
  // Call the transformation of each file (there are multiple SgFile 
  // objects when multiple files are specfied on the command line!).
     assert(project != NULL);

     const SgFilePtrList& fileList = project->get_fileList();
     SgFilePtrList::const_iterator file = fileList.begin();
     while(file != fileList.end())
        {
          transformGlobalVariablesToUseStruct(isSgSourceFile(*file));
          file++;
        }
   }

// ******************************************
//              MAIN PROGRAM
// ******************************************
int
main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
     assert(project != NULL);

  // transform application as required
     transformGlobalVariablesToUseStruct(project);

  // Code generation phase (write out new application "rose_<input file name>")
     return backend(project);
   }
