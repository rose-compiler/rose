/* unparse_stmt.C
 * Contains functions that unparse statements
 *
 * FORMATTING WILL BE DONE IN TWO WAYS:
 * 1. using the file_info object to get information from line and column number 
 *    (for original source code)
 * 2. following a specified format that I have specified with indentations of
 *    length TABINDENT (for transformations)
 * 
 * REMEMBER: For types and symbols, we still call the original unparse function 
 * defined in sage since they dont have file_info. For expressions, 
 * Unparse_ExprStmt::unparse is called, and for statements, 
 * Unparse_ExprStmt::unparseStatement is called.
 *
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"

#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_DEBUGGING_FUNCTION_INTERNALS  0
#define OUTPUT_DEBUGGING_UNPARSE_INFO        0

// Output the class name and function names as we unparse (for debugging)
#define OUTPUT_DEBUGGING_CLASS_NAME    0
#define OUTPUT_DEBUGGING_FUNCTION_NAME 0
#define OUTPUT_HIDDEN_LIST_DATA 0


Unparse_ExprStmt::Unparse_ExprStmt(Unparser* unp, std::string fname)
   : UnparseLanguageIndependentConstructs(unp,fname)
   {
  // Nothing to do here!
   }

Unparse_ExprStmt::~Unparse_ExprStmt()
   {
  // Nothing to do here!
   }

//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::unparseOneElemConInit
//
//  This function is used to only unparse constructor initializers with one 
//  element in their argument list. Because there is only one argument in the
//  list, the parenthesis around the arguments are not unparsed (to control
//  the excessive printing of parenthesis). 
//-----------------------------------------------------------------------------------
void 
Unparse_ExprStmt::unparseOneElemConInit(SgConstructorInitializer* con_init, SgUnparse_Info& info)
   {
     ROSE_ASSERT(con_init != NULL);
  /* code inserted from specification */

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("\n\nInside of Unparse_MOD_SAGE::unparseOneElemConInit (%p) \n",con_init);
     curprint( "\n /* Inside of Unparse_MOD_SAGE::unparseOneElemConInit */ \n");
#endif

  // taken from unparseConInit
     SgUnparse_Info newinfo(info);

#if 0
  // printf ("con_init->get_need_name()      = %s \n",(con_init->get_need_name() == true) ? "true" : "false");
  // printf ("con_init->get_need_qualifier() = %s \n",(con_init->get_need_qualifier() == true) ? "true" : "false");
  // printf ("con_init->get_declaration()    = %p \n",con_init->get_declaration());
     curprint( "\n /* con_init->get_need_name()        = %s " << (con_init->get_need_name() ? "true" : "false") << " */ \n");
     curprint( "\n /* con_init->get_is_explicit_cast() = %s " << (con_init->get_is_explicit_cast() ? "true" : "false") << " */ \n");
#endif

  // DQ (3/24/2005): added checking for is_explicit_cast flag
  // if (con_init->get_need_name() == true)
     if ((con_init->get_need_name() == true) && (con_init->get_is_explicit_cast() == true) )
        {
          SgName nm;
          if(con_init->get_declaration())
             {
            // DQ (11/12/2004)  Use the qualified name always (since get_need_qualifier() does
            //                  not appear to get set correctly (perhaps within EDG as before)
#if 0
               if(con_init->get_need_qualifier()) 
                    nm = con_init->get_declaration()->get_qualified_name();
                 else
                    nm = con_init->get_declaration()->get_name();
#else
               nm = con_init->get_declaration()->get_qualified_name();
#endif
             }
            else
             {
//             ROSE_ASSERT (con_init->get_class_decl() != NULL);
               if(con_init->get_class_decl())
                  {
                 // DQ (11/12/2004)  Use the qualified name always (since get_need_qualifier() does
                 //                  not appear to get set correctly (perhaps within EDG as before)
#if 0
                    if(con_init->get_need_qualifier()) 
                         nm = con_init->get_class_decl()->get_qualified_name();
                      else
                         nm = con_init->get_class_decl()->get_name();
#else
                    nm = con_init->get_class_decl()->get_qualified_name();
#endif
                  }
             }

          if ( unp->u_sage->printConstructorName(con_init))
             {
               curprint( nm.str());
             }
        }

  // curprint( "\n /* Done with name output in Unparse_MOD_SAGE::unparseOneElemConInit */ \n");

  // taken from unparseExprList
  // check whether the constructor name was printed. If so, we need to surround
  // the arguments of the constructor with parenthesis.
  // printf ("printConstructorName() = %s \n",(printConstructorName(con_init) == true) ? "true" : "false");
     if (con_init->get_need_name() && unp->u_sage->printConstructorName(con_init)) 
        {
          curprint( "("); 
          unp->u_debug->printDebugInfo("( from OneElemConInit", true);
        }

  // printf ("con_init->get_args() = %p \n",con_init->get_args());
     if (con_init->get_args())
        {
          SgExprListExp* expr_list = isSgExprListExp(con_init->get_args());
          ROSE_ASSERT(expr_list != NULL);
          SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();
          if (i != expr_list->get_expressions().end())
             {
               SgUnparse_Info newinfo(info);
               newinfo.set_SkipBaseType();
               unp->u_exprStmt->unparseExpression(*i, newinfo);
             }
        }

     if (con_init->get_need_name() && unp->u_sage->printConstructorName(con_init)) 
        {
          curprint( ")");
          unp->u_debug->printDebugInfo(") from OneElemConInit", true);
        }

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Leaving of Unparse_MOD_SAGE::unparseOneElemConInit \n\n\n");
     curprint( "\n /* Leaving of Unparse_MOD_SAGE::unparseOneElemConInit */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseFunctionParameterDeclaration ( 
   SgFunctionDeclaration* funcdecl_stmt, 
   SgInitializedName* initializedName,
   bool outputParameterDeclaration,
   SgUnparse_Info& info )
   {
     ROSE_ASSERT (funcdecl_stmt != NULL);

     SgName        tmp_name  = initializedName->get_name();
     SgInitializer *tmp_init = initializedName->get_initializer();
     SgType        *tmp_type = initializedName->get_type();

  // printf ("In unparseFunctionParameterDeclaration(): Argument name = %s \n",
  //      (tmp_name.str() != NULL) ? tmp_name.str() : "NULL NAME");

  // initializedName.get_storageModifier().display("New storage modifiers in unparseFunctionParameterDeclaration()");

     SgStorageModifier & storage = initializedName->get_storageModifier();
     if (storage.isExtern())
        {
          curprint( "extern ");
        }

  // DQ (7/202/2006): The isStatic() function in the SgStorageModifier held by the SgInitializedName object should always be false.
  // This is because the static-ness of a variable is held by the SgVariableDeclaration (and the SgStorageModified help in the SgDeclarationModifier).
  // printf ("In initializedName = %p test the return value of storage.isStatic() = %d = %d (should be boolean value) \n",initializedName,storage.isStatic(),storage.get_modifier());
     ROSE_ASSERT(storage.isStatic() == false);

  // This was a bug mistakenly reported by Isaac
     ROSE_ASSERT(storage.get_modifier() >= 0);

     if (storage.isStatic())
        {
          curprint( "static ");
        }

     if (storage.isAuto())
        {
       // DQ (4/30/2004): Auto is a default which is to be supressed 
       // in C old-style parameters and not really ever needed anyway?
       // curprint( "auto ");
        }

     if (storage.isRegister())
        {
       // DQ (12/10/2007): This is a fix for C_tests/test2007_177.c (this is only an issue if --edg:restrict is used on the commandline).
       // curprint( "register ");
          if ( (funcdecl_stmt->get_oldStyleDefinition() == false) || (outputParameterDeclaration == true) )
             {
               curprint( "register ");
             }
        }

     if (storage.isMutable())
        {
          curprint( "mutable ");
        }

     if (storage.isTypedef())
        {
          curprint( "typedef ");
        }

     if (storage.isAsm())
        {
          curprint( "asm ");
        }

  // Error checking, if we are using old style C function parameters, then I hope this is not C++ code!
     if (funcdecl_stmt->get_oldStyleDefinition() == true)
        {
          if (SageInterface::is_Cxx_language() == true)
             {
               printf ("Mixing old style C function parameters with C++ is not well defined, I think \n");
             }
          ROSE_ASSERT (SageInterface::is_Cxx_language() == false);
        }

     if ( (funcdecl_stmt->get_oldStyleDefinition() == false) || (outputParameterDeclaration == true) )
        {
       // output the type name for each argument
          if (tmp_type != NULL)
             {
#if 0
            // DQ (10/17/2004): This is now made more uniform and output in the unparseType() function
               if (isSgNamedType(tmp_type))
                  {
                    SgName theName;
                    theName = isSgNamedType(tmp_type)->get_qualified_name().str();
                    if (!theName.is_null())
                       {
                         curprint( theName.str() << "::");
                       }
                  }
#endif
               info.set_isTypeFirstPart();
            // curprint( "\n/* unparse_helper(): output the 1st part of the type */ \n");

            // DQ (8/6/2007): Skip forcing the output of qualified names now that we have a hidden list mechanism.
            // DQ (10/14/2006): Since function can appear anywhere types referenced in function 
            // declarations have to be fully qualified.  We can't tell from the type if it requires 
            // qualification we would need the type and the function declaration (and then some 
            // analysis).  So fully qualify all function parameter types.  This is a special case
            // (documented in the Unparse_ExprStmt::unp->u_name->generateNameQualifier() member function.
            // info.set_forceQualifiedNames();

               SgUnparse_Info ninfo_for_type(info);

#if 1
            // DQ (12/20/2006): This is used to specify global qualification separately from the more general name 
            // qualification mechanism.  Note that SgVariableDeclarations don't use the requiresGlobalNameQualificationOnType
            // on the SgInitializedNames in their list since the SgVariableDeclaration IR nodes is marked directly.
               if (initializedName->get_requiresGlobalNameQualificationOnType() == true)
                  {
                 // Output the name qualification for the type in the variable declaration.
                 // But we have to do so after any modifiers are output, so in unparseType().
                 // printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): This function parameter type requires a global qualifier \n");

                 // Note that general qualification of types is separated from the use of globl qualification.
                 // ninfo2.set_forceQualifiedNames();
                    ninfo_for_type.set_requiresGlobalNameQualification();
                  }
#endif

            // unparseType(tmp_type, info);
               unp->u_type->unparseType(tmp_type, ninfo_for_type);

            // curprint( "\n/* DONE - unparse_helper(): output the 1st part of the type */ \n");

            // forward declarations don't necessarily need the name of the argument
            // so we must check if not NULL before adding to chars_on_line
            // This is a more consistant way to handle the NULL string case
            // curprint( "\n/* unparse_helper(): output the name of the type */ \n");
                    curprint( tmp_name.str());

            // output the rest of the type
               info.set_isTypeSecondPart();

            // info.display("unparse_helper(): output the 2nd part of the type");

            // printf ("unparse_helper(): output the 2nd part of the type \n");
            // curprint( "\n/* unparse_helper(): output the 2nd part of the type */ \n");
               unp->u_type->unparseType(tmp_type, info);
            // printf ("DONE: unparse_helper(): output the 2nd part of the type \n");
            // curprint( "\n/* DONE: unparse_helper(): output the 2nd part of the type */ \n");
             }
            else
             {
               curprint( tmp_name.str()); // for ... case
             }
        }
       else
        {
          curprint( tmp_name.str()); // for ... case
        }
 
     SgUnparse_Info ninfo3(info);
     ninfo3.unset_inArgList();

  // DQ (6/16/2005): control output of initializers (should only be output once and never in 
  // generated declarations for member functions) member function function declaration declared 
  // outside of the class.  Avoid case of "X { public: X(int i = 0); }; X::X(int i = 0) {}"
  // see test2005_87.C for example and details.
     bool outputInitializer = true;
     if (funcdecl_stmt->get_scope() != funcdecl_stmt->get_parent())
        {
       // This function declaration is appearing in a different scope there where it was first 
       // declared so avoid output of the default initializers of any function parameters!
       // printf ("Skipping output of initializer since this is not the original declaration! \n");
          outputInitializer = false;
        }

  // Add an initializer if it exists
     if ( outputInitializer == true && tmp_init != NULL )
        {
       // DQ (6/14/2005): We only want to avoid the redefinition of function parameters.
       // DQ (4/20/2005): Removed from_template data member since it is redundant 
       //                 in design with handling of templates in ROSE.
       // if(!(funcdecl_stmt->get_from_template() && !funcdecl_stmt->isForward()))
       // if ( !funcdecl_stmt->isForward() )
          curprint( "=");
          unp->u_exprStmt->unparseExpression(tmp_init, ninfo3);
        }
   }

void
Unparse_ExprStmt::unparseFunctionArgs(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info)
   {
     ROSE_ASSERT (funcdecl_stmt != NULL);

#if 0
     printf ("funcdecl_stmt->get_args().size() = %zu \n",funcdecl_stmt->get_args().size());
     curprint( "\n/* funcdecl_stmt->get_args().size() = " << (int)(funcdecl_stmt->get_args().size()) << " */ \n");
#endif

     SgInitializedNamePtrList::iterator p = funcdecl_stmt->get_args().begin();
     while ( p != funcdecl_stmt->get_args().end() )
        {
          unparseFunctionParameterDeclaration (funcdecl_stmt,*p,false,info);

       // Move to the next argument
          p++;

       // Check if this is the last argument (output a "," separator if not)
          if (p != funcdecl_stmt->get_args().end())
             {
               curprint( ",");
             }
        }
   }

//-----------------------------------------------------------------------------------
//  void Unparse_ExprStmt::unparse_helper
//
//  prints out the function parameters in a function declaration or function
//  call. For now, all parameters are printed on one line since there is no
//  file information for each parameter.
//-----------------------------------------------------------------------------------
void
Unparse_ExprStmt::unparse_helper(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info)
   {
     ROSE_ASSERT (funcdecl_stmt != NULL);

  // DQ (9/8/2007): Friend function declaration should be qulified, if the associated function has already been seen.
  // See test2007_124.C for an example. Friend declarations of operators are not qualified, or at least should not
  // use global qualification. If this is the first declaration, then no qualification should be used (see test2004_117.C).
     SgUnparse_Info ninfo(info);
  // printf ("funcdecl_stmt = %p funcdecl_stmt->get_definingDeclaration() = %p funcdecl_stmt->get_firstNondefiningDeclaration() = %p \n",
  //      funcdecl_stmt,funcdecl_stmt->get_definingDeclaration(),funcdecl_stmt->get_firstNondefiningDeclaration());
     bool isFirstDeclaration = funcdecl_stmt == funcdecl_stmt->get_firstNondefiningDeclaration();
     if (funcdecl_stmt->get_declarationModifier().isFriend() == true && funcdecl_stmt->get_specialFunctionModifier().isOperator() == false && isFirstDeclaration == false )
        {
          ninfo.set_forceQualifiedNames();
        }

  // DQ (11/18/2004): Added support for qualified name of template declaration!
  // But it appears that the qualified name is included within the template text string so that 
  // we should not output the qualified name spearately!
#if 0
  // DQ (3/4/2009): This code fails for tutorial/rose_inputCode_InstrumentationTranslator.C
  // commented out this branch in favor of the other one!

  // printf ("Before calling generateNameQualifier(): ninfo.get_current_scope() = %p = %s \n",ninfo.get_current_scope(),ninfo.get_current_scope()->class_name().c_str());

  // DQ (2/22/2009): Added assertion.
  // ROSE_ASSERT(funcdecl_stmt->get_symbol_from_symbol_table() != NULL);
     if (funcdecl_stmt->get_firstNondefiningDeclaration() != NULL)
        {
          ROSE_ASSERT (funcdecl_stmt->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table() != NULL);
        }

     SgName nameQualifier = unp->u_name->generateNameQualifier( funcdecl_stmt , ninfo );
#else
  // DQ (10/24/2007): Added fix by Jeremiah (not well tested) This is suggested by Jeremiah, 
  // but it does not yet address friend functions which might require qualification.

     SgUnparse_Info ninfoForFunctionName(ninfo);
     if (isSgClassDefinition(funcdecl_stmt->get_parent()))
        {
       // JJW 10-23-2007 Never qualify a member function name
          ninfoForFunctionName.set_SkipQualifiedNames();
        }
     SgName nameQualifier = unp->u_name->generateNameQualifier( funcdecl_stmt , ninfoForFunctionName );
#endif
  // printf ("In unparse_helper(): nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());

  // DQ (10/12/2006): need to trim off the global scope specifier (I think).
  // curprint( "\n/* Calling trimGlobalScopeQualifier() */\n ");
  // curprint( "\n/* Skipping trimGlobalScopeQualifier() */\n ");
  // nameQualifier = trimGlobalScopeQualifier ( nameQualifier.str() ).c_str();

     curprint( nameQualifier.str());

  // output the function name
     curprint( funcdecl_stmt->get_name().str());

#if 0
  // DQ (2/16/2005): Function name has been modified instead.
  // DQ (2/15/2005): If this is a template function then we need to output the template parameters!
     SgTemplateInstantiationFunctionDecl* templateFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(funcdecl_stmt);
     if (templateFunctionDeclaration != NULL)
        {
          printf ("Found a template function: output the template parameters! \n");
          curprint( " /* < template parameters > */ ");
          SgTemplateArgumentPtrListPtr templateArguments = templateFunctionDeclaration->get_templateArguments();
          printf ("Number of template arguments for instantiated template function = %zu \n",templateArguments->size());
          if (templateArguments->size() > 0)
             {
             }
        }
#endif

     SgUnparse_Info ninfo2(info);
     ninfo2.set_inArgList();

  // DQ (5/14/2003): Never output the class definition in the argument list.
  // Using this C++ constraint avoids building a more complex mechanism to turn it off.
     ninfo2.set_SkipClassDefinition();

     curprint( "(");

     unparseFunctionArgs(funcdecl_stmt,ninfo2);
     
  // printf ("Adding a closing \")\" to the end of the argument list \n");
     curprint( ")");

     if ( funcdecl_stmt->get_oldStyleDefinition() )
        {
       // Output old-style C (K&R) function definition
       // printf ("Output old-style C (K&R) function definition \n");
       // curprint( "/* Output old-style C (K&R) function definition */ \n");

          SgInitializedNamePtrList::iterator p = funcdecl_stmt->get_args().begin();
          if (p != funcdecl_stmt->get_args().end())
               unp->u_sage->curprint_newline();
          while ( p != funcdecl_stmt->get_args().end() )
             {
            // Output declarations for function parameters (using old-style K&R syntax)
            // printf ("Output declarations for function parameters (using old-style K&R syntax) \n");
               unparseFunctionParameterDeclaration(funcdecl_stmt,*p,true,ninfo2);
               curprint( ";");
               unp->u_sage->curprint_newline();
               p++;
             }
        }

  // curprint( endl;
  // curprint( "Added closing \")\" to the end of the argument list \n");
  // curprint(flush();

  // printf ("End of function Unparse_ExprStmt::unparse_helper() \n");
   }



void
Unparse_ExprStmt::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
{
  // This function unparses the language specific parse not handled by the base class unparseStatement() member function

  ROSE_ASSERT(stmt != NULL);

#if 0
  // Debugging support
  SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(stmt);
  if (declarationStatement != NULL)
  {
    curprint( "/* In unparseLanguageSpecificStatement(): declarationStatement->get_declarationModifier().isFriend() = ");
    declarationStatement->get_declarationModifier().isFriend() ? curprint("true") : curprint("false");
    curprint( "*/ \n ");
  }
#endif

#if 0
  curprint ( string("\n/* Top of unparseLanguageSpecificStatement (Unparse_ExprStmt) " ) + stmt->class_name() + " */\n ");
  ROSE_ASSERT(stmt->get_startOfConstruct() != NULL);
  // ROSE_ASSERT(stmt->getAttachedPreprocessingInfo() != NULL);
  int numberOfComments = -1;
  if (stmt->getAttachedPreprocessingInfo() != NULL)
    numberOfComments = stmt->getAttachedPreprocessingInfo()->size();
  curprint ( string("/* startOfConstruct: file = " ) + stmt->get_startOfConstruct()->get_filenameString()
      + " raw filename = " + stmt->get_startOfConstruct()->get_raw_filename()
      + " raw line = "     + StringUtility::numberToString(stmt->get_startOfConstruct()->get_raw_line())
      + " raw column = "   + StringUtility::numberToString(stmt->get_startOfConstruct()->get_raw_col())
      + " #comments = "    + StringUtility::numberToString(numberOfComments)
      + " */\n ");
#endif

#if ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE
  printf ("In unparseLanguageSpecificStatement(): file = %s line = %d \n",stmt->get_startOfConstruct()->get_filenameString().c_str(),stmt->get_startOfConstruct()->get_line());
#endif

#if 0
  // Debugging support
  SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(stmt);
  if (declarationStatement != NULL)
  {
    curprint ( string("\n/* Inside of Unparse_ExprStmt::unparseLanguageSpecificStatement (" ) + StringUtility::numberToString(stmt) + "): sage_class_name() = " + stmt->sage_class_name() + " */ \n");
  }
#endif

#if 0
  // This is done in: UnparseLanguageIndependentConstructs::unparseStatement()
  // DQ (12/5/2007): Check if the call to unparse any construct changes the scope stored in info.
  SgScopeStatement* savedScope = info.get_current_scope();
#endif

  // DQ (12/16/2008): Added support for unparsing statements around C++ specific statements
  // unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::before);

  // DQ (12/26/2007): Moved from language independent handling to C/C++ specific handling 
  // because we don't want it to appear in the Fortran code generation.
  // DQ (added comments) this is where the new lines are introduced before statements.
  unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);

  switch (stmt->variantT())
  {
    // DQ (8/14/2007): Need to move the C and C++ specific unparse member functions from the base class to this function.

    // scope
    // case V_SgGlobal:                 unparseGlobalStmt(stmt, info); break;
    // case V_SgScopeStatement:         unparseScopeStmt(stmt, info); break;

    // pragmas
    // case V_SgPragmaDeclaration:      unparsePragmaDeclStmt(stmt, info); break;
    // scope
    // case V_SgGlobal:                 unparseGlobalStmt(stmt, info); break;
    //        case V_SgScopeStatement:         unparseScopeStmt (stmt, info); break;

    // program units
    // case V_SgModuleStatement:          unparseModuleStmt (stmt, info); break;
    // case V_SgProgramHeaderStatement:   unparseProgHdrStmt(stmt, info); break;
    // case V_SgProcedureHeaderStatement: unparseProcHdrStmt(stmt, info); break;
#if 1
    // declarations
    // case V_SgInterfaceStatement:     unparseInterfaceStmt(stmt, info); break;
    // case V_SgCommonBlock:            unparseCommonBlock  (stmt, info); break;
    case V_SgVariableDeclaration:    unparseVarDeclStmt  (stmt, info); break;
    case V_SgVariableDefinition:     unparseVarDefnStmt  (stmt, info); break;
                                     // case V_SgParameterStatement:     unparseParamDeclStmt(stmt, info); break;
                                     // case V_SgUseStatement:           unparseUseStmt      (stmt, info); break;

                                     // executable statements, control flow
    case V_SgBasicBlock:             unparseBasicBlockStmt (stmt, info); break;
    case V_SgIfStmt:                 unparseIfStmt         (stmt, info); break;
                                     // case V_SgFortranDo:              unparseDoStmt         (stmt, info); break;
    case V_SgWhileStmt:              unparseWhileStmt      (stmt, info); break;
    case V_SgSwitchStatement:        unparseSwitchStmt     (stmt, info); break;
    case V_SgCaseOptionStmt:         unparseCaseStmt       (stmt, info); break;
    case V_SgDefaultOptionStmt:      unparseDefaultStmt    (stmt, info); break;
    case V_SgBreakStmt:              unparseBreakStmt      (stmt, info); break;
    case V_SgLabelStatement:         unparseLabelStmt      (stmt, info); break;
    case V_SgGotoStatement:          unparseGotoStmt       (stmt, info); break;
                                     // case V_SgStopOrPauseStatement:   unparseStopOrPauseStmt(stmt, info); break;
    case V_SgReturnStmt:             unparseReturnStmt     (stmt, info); break;
#endif
                                     // executable statements, IO
                                     // case V_SgIOStatement:            unparseIOStmt    (stmt, info); break;
                                     // case V_SgIOControlStatement:     unparseIOCtrlStmt(stmt, info); break;

                                     // pragmas
    case V_SgPragmaDeclaration:      unparsePragmaDeclStmt(stmt, info); break;

                                     // case DECL_STMT:          unparseDeclStmt(stmt, info);         break;
                                     // case SCOPE_STMT:         unparseScopeStmt(stmt, info);        break;
                                     //        case V_SgFunctionTypeTable:      unparseFuncTblStmt(stmt, info);      break;
                                     // case GLOBAL_STMT:        unparseGlobalStmt(stmt, info);       break;
                                     // case V_SgBasicBlock:             unparseBasicBlockStmt(stmt, info);   break;
                                     // case IF_STMT:            unparseIfStmt(stmt, info);           break;
#if 1
    case V_SgForStatement:           unparseForStmt(stmt, info);          break; 
    case V_SgFunctionDeclaration:    unparseFuncDeclStmt(stmt, info);     break;
    case V_SgFunctionDefinition:     unparseFuncDefnStmt(stmt, info);     break;
    case V_SgMemberFunctionDeclaration: unparseMFuncDeclStmt(stmt, info); break;
                                        // case VAR_DECL_STMT:      unparseVarDeclStmt(stmt, info);      break;
                                        // case VAR_DEFN_STMT:      unparseVarDefnStmt(stmt, info);      break;
    case V_SgClassDeclaration:       unparseClassDeclStmt(stmt, info);    break;
    case V_SgClassDefinition:        unparseClassDefnStmt(stmt, info);    break;
    case V_SgEnumDeclaration:        unparseEnumDeclStmt(stmt, info);     break;
    case V_SgExprStatement:          unparseExprStmt(stmt, info);         break;
                                     // case LABEL_STMT:         unparseLabelStmt(stmt, info);        break;
                                     // case WHILE_STMT:         unparseWhileStmt(stmt, info);        break;
    case V_SgDoWhileStmt:            unparseDoWhileStmt(stmt, info);      break;
                                     // case SWITCH_STMT:        unparseSwitchStmt(stmt, info);       break;
                                     // case CASE_STMT:          unparseCaseStmt(stmt, info);         break;
    case V_SgTryStmt:                unparseTryStmt(stmt, info);          break;
    case V_SgCatchOptionStmt:        unparseCatchStmt(stmt, info);        break;
                                     // case DEFAULT_STMT:       unparseDefaultStmt(stmt, info);      break;
                                     // case BREAK_STMT:         unparseBreakStmt(stmt, info);        break;
    case V_SgContinueStmt:           unparseContinueStmt(stmt, info);     break;
                                     // case RETURN_STMT:        unparseReturnStmt(stmt, info);       break;
                                     // case GOTO_STMT:          unparseGotoStmt(stmt, info);         break;
    case V_SgAsmStmt:                unparseAsmStmt(stmt, info);          break;
                                     // case SPAWN_STMT:         unparseSpawnStmt(stmt, info);        break;
    case V_SgTypedefDeclaration:     unparseTypeDefStmt(stmt, info);      break;
    case V_SgTemplateDeclaration:    unparseTemplateDeclStmt(stmt, info); break;

    case V_SgTemplateInstantiationDecl:               unparseTemplateInstantiationDeclStmt(stmt, info); break;
    case V_SgTemplateInstantiationFunctionDecl:       unparseTemplateInstantiationFunctionDeclStmt(stmt, info); break;
    case V_SgTemplateInstantiationMemberFunctionDecl: unparseTemplateInstantiationMemberFunctionDeclStmt(stmt, info); break;
    case V_SgTemplateInstantiationDirectiveStatement: unparseTemplateInstantiationDirectiveStmt(stmt, info); break;
#endif

#if 0
    case PRAGMA_DECL:
                                                      // cerr + "WARNING: unparsePragmaDeclStmt not implemented in SAGE 3 (exiting ...)" + endl;
                                                      // This can't be an error since the A++ preprocessor currently processes #pragmas
                                                      // (though we can ignore unparsing them)
                                                      // ROSE_ABORT();

                                                      unparsePragmaDeclStmt(stmt, info);
                                                      break;
#endif

#if 1
    case V_SgForInitStatement:                   unparseForInitStmt(stmt, info); break;

                                                 // Comments could be attached to these statements
    case V_SgCatchStatementSeq:     // CATCH_STATEMENT_SEQ:
    case V_SgFunctionParameterList: // FUNCTION_PARAMETER_LIST:
    case V_SgCtorInitializerList:   // CTOR_INITIALIZER_LIST:
#if PRINT_DEVELOPER_WARNINGS
                                                 printf ("Ignore these newly implemented cases (case of %s) \n",stmt->sage_class_name());
                                                 printf ("WARNING: These cases must be implemented so that comments attached to them can be processed \n");
#endif
                                                 // ROSE_ABORT();
                                                 break;

    case V_SgNamespaceDeclarationStatement:      unparseNamespaceDeclarationStatement (stmt, info);      break;
    case V_SgNamespaceDefinitionStatement:       unparseNamespaceDefinitionStatement (stmt, info);       break;
    case V_SgNamespaceAliasDeclarationStatement: unparseNamespaceAliasDeclarationStatement (stmt, info); break;
    case V_SgUsingDirectiveStatement:            unparseUsingDirectiveStatement (stmt, info);            break;
    case V_SgUsingDeclarationStatement:          unparseUsingDeclarationStatement (stmt, info);          break;

                                                 // DQ (3/2/2005): Added support for unparsing template class definitions.  This is the case: TEMPLATE_INST_DEFN_STMT
    case V_SgTemplateInstantiationDefn:          unparseClassDefnStmt(stmt, info); break;
#endif
                                                 //        case V_SgNullStatement:                      unparseNullStatement(stmt, info); break;

//#if USE_UPC_IR_NODES
//#if UPC_EXTENSIONS_ALLOWED //TODO turn on by default?
// Liao, 6/13/2008: UPC support
    case V_SgUpcNotifyStatement:           	  unparseUpcNotifyStatement(stmt, info); break;
    case V_SgUpcWaitStatement:            	  unparseUpcWaitStatement(stmt, info); break;
    case V_SgUpcBarrierStatement:                 unparseUpcBarrierStatement(stmt, info); break;
    case V_SgUpcFenceStatement:                   unparseUpcFenceStatement(stmt, info); break;
    case V_SgUpcForAllStatement:                  unparseUpcForAllStatement(stmt, info);    break; 

//#endif 
// Liao, 5/31/2009, add OpenMP support, TODO refactor some code to language independent part
    case V_SgOmpForStatement:                      unparseOmpForStatement(stmt, info); break;
//    case V_SgOmpAtomicStatement:                  unparseOmpAtomicStatement(stmt, info);  break;
//    case V_SgOmpCriticalStatement:
//    case V_SgOmpMasterStatement:
//    case V_SgOmpOrderedStatement:
//    case V_SgOmpSectionStatement:
//    case V_SgOmpSectionsStatement:
//    case V_SgOmpParallelStatement:
//    case V_SgOmpForStatement:
//    case V_SgOmpSingleStatement:
//    case V_SgOmpTaskStatement:
//                                                  {
//                                                    unparseOmpBodyStatement(isSgOmpBodyStatement(stmt), info); break;
                                                    //unparseOmpParallelStatement(stmt, info); break;
//                                                  }
//    case V_SgOmpThreadprivateStatement:
//    {
//      unparseOmpThreadprivateStatement(isSgOmpThreadprivateStatement(stmt),info);
//      break;
//      }
    
//    case V_SgOmpBarrierStatement:                  unparseOmpBarrierStatement(isSgOmpBarrierStatement(stmt), info);    break; 
//    case V_SgOmpFlushStatement:                    unparseOmpFlushStatement(isSgOmpFlushStatement(stmt), info);    break; 
//    case V_SgOmpTaskwaitStatement:                 unparseOmpTaskwaitStatement(isSgOmpTaskwaitStatement(stmt), info);    break; 

    default:
                                                  {
                                                    printf("CxxCodeGeneration_locatedNode::unparseLanguageSpecificStatement: Error: No handler for %s (variant: %d)\n",stmt->sage_class_name(), stmt->variantT());
                                                    ROSE_ASSERT(false);
                                                    break;
                                                  }
  }

  // DQ (12/16/2008): Added support for unparsing statements around C++ specific statements
  // unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::after);

#if 0
  // This is done in: UnparseLanguageIndependentConstructs::unparseStatement()
  // DQ (12/5/2007): Check if the call to unparse any construct changes the scope stored in info.
  SgScopeStatement* scopeAfterUnparseStatement = info.get_current_scope();
  if (savedScope != scopeAfterUnparseStatement)
  {
    printf ("WARNING: scopes stored in SgUnparse_Info object have been changed \n");
  }
#endif
}


#if 0
// DQ (8/13/2007): This has been moved to the base class (language independent code)

void
Unparse_ExprStmt::unparseNullStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
  // Nothing to do here! (unless we need a ";" or something)
     SgNullStatement* nullStatement = isSgNullStatement(stmt);
     ROSE_ASSERT(nullStatement != NULL);

  // Not much to do here except output a ";", not really required however.
  // curprint ( string(";";
   }
#endif


void
Unparse_ExprStmt::unparseNamespaceDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
  // There is a SgNamespaceDefinition, but it is not unparsed except through the SgNamespaceDeclaration

     SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(stmt);
     ROSE_ASSERT (namespaceDeclaration != NULL);
     curprint ( string("namespace "));

  // This can be an empty string (in the case of an unnamed namespace)
     SgName name = namespaceDeclaration->get_name();
          curprint ( name.str());

     unparseStatement(namespaceDeclaration->get_definition(),info);
   }

void
Unparse_ExprStmt::unparseNamespaceDefinitionStatement ( SgStatement* stmt, SgUnparse_Info & info )
   {
     ROSE_ASSERT (stmt != NULL);
     SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(stmt);
     ROSE_ASSERT (namespaceDefinition != NULL);

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (namespaceDefinition);
#endif

     SgUnparse_Info ninfo(info);

  // DQ (11/6/2004): Added support for saving current namespace!
     ROSE_ASSERT(namespaceDefinition->get_namespaceDeclaration() != NULL);
     SgNamespaceDeclarationStatement *saved_namespace = ninfo.get_current_namespace();

#if 0
     if (saved_namespace != NULL)
          printf ("In unparseNamespaceDefinitionStatement(): saved_namespace = %p = %s \n",saved_namespace,saved_namespace->class_name().c_str());
#endif

  // DQ (6/13/2007): Set to null before resetting to non-null value 
     ninfo.set_current_namespace(NULL);
     ninfo.set_current_namespace(namespaceDefinition->get_namespaceDeclaration());

     unp->cur.format(namespaceDefinition, info, FORMAT_BEFORE_BASIC_BLOCK2);
     curprint ( string("{"));
     unp->cur.format(namespaceDefinition, info, FORMAT_AFTER_BASIC_BLOCK2);

  // unparse all the declarations
     SgDeclarationStatementPtrList & statementList = namespaceDefinition->get_declarations();
     SgDeclarationStatementPtrList::iterator statementIterator = statementList.begin();
     while ( statementIterator != statementList.end() )
        {
          SgStatement* currentStatement = *statementIterator;
          ROSE_ASSERT(currentStatement != NULL);

       // DQ (11/6/2004): use ninfo instead of info for nested declarations in namespace
          unparseStatement(currentStatement, ninfo);

       // Go to the next statement
          statementIterator++;
        }

  // DQ (3/17/2005): This helps handle cases such as void foo () { #include "constant_code.h" }
     unparseAttachedPreprocessingInfo(namespaceDefinition, info, PreprocessingInfo::inside);

     unp->cur.format(namespaceDefinition, info, FORMAT_BEFORE_BASIC_BLOCK2);
     curprint ( string("}\n"));
     unp->cur.format(namespaceDefinition, info, FORMAT_AFTER_BASIC_BLOCK2);

  // DQ (11/3/2007): Since "ninfo" will go out of scope shortly, this is not significant.
  // DQ (6/13/2007): Set to null before resetting to non-null value 
  // DQ (11/6/2004): Added support for saving current namespace!
     ninfo.set_current_namespace(NULL);
     ninfo.set_current_namespace(saved_namespace);

#if 0
     if (saved_namespace != NULL)
          printf ("In unparseNamespaceDefinitionStatement(): reset saved_namespace = %p = %s \n",saved_namespace,saved_namespace->class_name().c_str());
#endif
   }

void
Unparse_ExprStmt::unparseNamespaceAliasDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(stmt);
     ROSE_ASSERT (namespaceAliasDeclaration != NULL);

     curprint ( string("\nnamespace "));
     curprint ( namespaceAliasDeclaration->get_name().str());
     curprint ( string(" = "));
     ROSE_ASSERT(namespaceAliasDeclaration->get_namespaceDeclaration() != NULL);
     curprint ( namespaceAliasDeclaration->get_namespaceDeclaration()->get_name().str());
     curprint ( string(";\n"));
   }

void
Unparse_ExprStmt::unparseUsingDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDirectiveStatement* usingDirective = isSgUsingDirectiveStatement(stmt);
     ROSE_ASSERT (usingDirective != NULL);

  // DQ (8/26/2004): This should be "using namespace" instead of just "using"
     curprint ( string("\nusing namespace "));
     ROSE_ASSERT(usingDirective->get_namespaceDeclaration() != NULL);
#if 0
     printf ("In unparseUsingDirectiveStatement using namespace = %s qualified name = %s \n",
          usingDirective->get_namespaceDeclaration()->get_name().str(),
          usingDirective->get_namespaceDeclaration()->get_qualified_name().str());
#endif

  // DQ (6/7/2007): Compute the name qualification separately.
  // curprint ( usingDirective->get_namespaceDeclaration()->get_name().str();
  // curprint ( usingDirective->get_namespaceDeclaration()->get_qualified_name().str();
     SgName nameQualifier = unp->u_name->generateNameQualifier( usingDirective->get_namespaceDeclaration() , info );
     curprint ( nameQualifier);
     curprint ( usingDirective->get_namespaceDeclaration()->get_name().str());
     

     curprint ( string(";\n"));
   }

void
Unparse_ExprStmt::unparseUsingDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(stmt);
     ROSE_ASSERT (usingDeclaration != NULL);

     curprint ( string("\nusing "));

  // DQ (9/11/2004): We only save the declaration and get the name by unparsing the declaration
  // Might have to setup info1 to only output the name that we want!
     SgUnparse_Info info1(info);
  // info1.unset_CheckAccess();
  // info1.set_PrintName();
     info1.unset_isWithType();

  // DQ (7/21/2005): Either one or the other of these are valid. A using declaration can have either 
  // a reference to a declaration (SgDeclarationStatement) or a variable or enum file name (SgInitializedName).
     SgDeclarationStatement* declarationStatement = usingDeclaration->get_declaration();
     SgInitializedName*      initializedName      = usingDeclaration->get_initializedName();

  // Enforce that only one is a vaild pointer
     ROSE_ASSERT(declarationStatement != NULL || initializedName != NULL);
     ROSE_ASSERT(declarationStatement == NULL || initializedName == NULL);

  // printf ("In unparseUsingDeclarationStatement(): declarationStatement = %s \n",declarationStatement->sage_class_name());
  // unparseStatement(declarationStatement,info1);

     if (initializedName != NULL)
        {
       // DQ (9/12/2004): Add the qualification required to resolve the name
#if 1
          SgScopeStatement* scope = initializedName->get_scope();
          if (isSgGlobal(scope) == NULL)
               curprint ( string(scope->get_qualified_name().str()) + "::");
          curprint ( initializedName->get_name().str());
#else
       // DQ (8/22/2005): Added get_qualified_name() to SgInitializedName class
          curprint ( initializedName->get_qualified_name().str());
#endif
        }

     if (declarationStatement != NULL)
        {
       // DQ (9/12/2004): Add the qualification required to resolve the name
          SgScopeStatement* scope = declarationStatement->get_scope();
#if 1
       // DQ (8/25/2005) Switched back to implicit hanlding of "::" within qualified names
       // Since "::" is not generated in the computed qualified name, we have to add it 
       // explicitly here.
          if (isSgGlobal(scope) != NULL)
             {
            // Avoid ":: ::" as a generated qualified name (not that the qualified name for SgGlobal is "::")
               curprint ( string("::"));
             }
            else
             {
               curprint ( string(scope->get_qualified_name().str()) + "::");
             }
#else
       // DQ (8/25/2005): This is the case where we previously names the global scope as "::"
       // within name qualification.  This was done to handle test2005_144.C but it broke
       // test2004_80.C So we have moved to an explicit marking of IR nodes using global scope
       // qualification (since it clearly seems to be required).
          if (isSgGlobal(scope) != NULL)
               curprint ( string(scope->get_qualified_name().str()));
            else
               curprint ( string(scope->get_qualified_name().str()) + "::");
#endif
       // Handle the different sorts of declarations explicitly since the existing unparse functions for 
       // declarations are not setup for what the using declaration unparser requires.
          switch (declarationStatement->variantT())
             {
               case V_SgVariableDeclaration:
                  {
                 // DQ (7/21/2005): Now that we have added support for SgUsingDeclarations to reference a 
                 // SgDeclarationStatment or a SgInitializedName we could have the SgVariableDeclaration
                 // be implemented to more precisely reference the variable directly instead of the 
                 // declaration where the variable was defined.

                 // get the name of the variable in the declaration
                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declarationStatement);
                    ROSE_ASSERT(variableDeclaration != NULL);
                    SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
                 // using directives must be issued sepearately for each variable!
                    ROSE_ASSERT(variableList.size() == 1);
                    SgInitializedName* initializedName = *(variableList.begin());
                    ROSE_ASSERT(initializedName != NULL);
                    SgName variableName = initializedName->get_name();
                    curprint ( variableName.str());
                    break;
                  }

               case V_SgVariableDefinition:
                  {
                 // DQ (6/18/2006): Associated declaration can be a SgVariableDefinition,
                 // get the name of the variable using the variable definition
                    SgVariableDefinition* variableDefinition = isSgVariableDefinition(declarationStatement);
                    ROSE_ASSERT(variableDefinition != NULL);
                    SgInitializedName* initializedName = variableDefinition->get_vardefn();
                    ROSE_ASSERT(initializedName != NULL);
                    SgName variableName = initializedName->get_name();
                    curprint ( variableName.str());
                    break;
                  }

               case V_SgNamespaceDeclarationStatement:
                  {
                    SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(declarationStatement);
                    ROSE_ASSERT(namespaceDeclaration != NULL);
                    SgName namespaceName = namespaceDeclaration->get_name();
                    curprint ( namespaceName.str());
                    break;
                  }           

               case V_SgFunctionDeclaration:
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declarationStatement);
                    ROSE_ASSERT(functionDeclaration != NULL);
                    SgName functionName = functionDeclaration->get_name();
                    curprint ( functionName.str());
                    break;
                  }

               case V_SgTemplateInstantiationMemberFunctionDecl:
               case V_SgMemberFunctionDeclaration:
                  {
                    SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declarationStatement);
                    ROSE_ASSERT(memberFunctionDeclaration != NULL);
                    SgName memberFunctionName = memberFunctionDeclaration->get_name();
                    curprint ( memberFunctionName.str());
                    break;
                  }

               case V_SgClassDeclaration:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
                    ROSE_ASSERT(classDeclaration != NULL);
                    SgName className = classDeclaration->get_name();
                    curprint ( className.str());
                    break;
                  }

               case V_SgTypedefDeclaration:
                  {
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(declarationStatement);
                    ROSE_ASSERT(typedefDeclaration != NULL);
                    SgName typedefName = typedefDeclaration->get_name();
                    curprint ( typedefName.str());
                    break;
                  }

               case V_SgTemplateDeclaration:
                  {
                 // DQ (9/12/2004): This function outputs the default template name which is not correct, we need 
                 // to get more information out of EDG about the template name if we are to get this correct in 
                 // the future.  This could (and likely will) cause generated code to not compile, but I will 
                 // worry about that after we can compile Kull.
                    SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(declarationStatement);
                    ROSE_ASSERT(templateDeclaration != NULL);
                    SgName templateName = templateDeclaration->get_name();
                    curprint ( templateName.str());
                    break;
                  }

            // DQ (5/22/2007): Added support for enum types in using declaration (test2007_50.C).
               case V_SgEnumDeclaration:
                  {
                    SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(declarationStatement);
                    ROSE_ASSERT(enumDeclaration != NULL);
                    SgName enumName = enumDeclaration->get_name();
                    curprint ( enumName.str());
                    break;
                  }

               default:
                  {
                    printf ("Default reached in unparseUsingDeclarationStatement(): case is not implemented for %s \n",declarationStatement->sage_class_name());
                    ROSE_ASSERT(false);
                  }
             }
        }

     curprint ( string(";\n"));
   }

void 
Unparse_ExprStmt::unparseTemplateInstantiationDirectiveStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (4/16/2005): Added support for explicit template instatination directives
     SgTemplateInstantiationDirectiveStatement* templateInstantiationDirective = isSgTemplateInstantiationDirectiveStatement(stmt);
     ROSE_ASSERT(templateInstantiationDirective != NULL);

     SgDeclarationStatement* declarationStatement = templateInstantiationDirective->get_declaration();
     ROSE_ASSERT(declarationStatement != NULL);

  // printf ("Inside of unparseTemplateInstantiationDirectiveStmt declaration = %s \n",declarationStatement->sage_class_name());

  // curprint ( string("/* explicit template instantiation */ \n ";
  // curprint ( string("template ";

     ROSE_ASSERT(declarationStatement->get_file_info() != NULL);
  // declarationStatement->get_file_info()->display("Location of SgTemplateInstantiationDirectiveStatement \n");

  // unparseStatement(declaration,info);
     switch (declarationStatement->variantT())
        {
          case V_SgTemplateInstantiationDecl:
             {
#if 0
               printf ("Unparsing of SgTemplateInstantiationDecl in unparseTemplateInstantiationDirectiveStmt ... \n");
#endif
            // unparseClassDeclStmt(declarationStatement,info);
            // unparseTemplateInstantiationDeclStmt(declarationStatement,info);
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
               ROSE_ASSERT(classDeclaration != NULL);

#if 0
               printf ("classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
               printf ("classDeclaration->get_parent() = %p = %s \n",classDeclaration->get_parent(),classDeclaration->get_parent()->class_name().c_str());
#endif
            // DQ (8/29/2005): "template" keyword now output by Unparse_ExprStmt::outputTemplateSpecializationSpecifier()
            // curprint ( string("template ";
               unparseClassDeclStmt(classDeclaration,info);
               break;
             }

          case V_SgTemplateInstantiationFunctionDecl:
             {
            // printf ("Unparsing of SgTemplateInstantiationFunctionDecl in unparseTemplateInstantiationDirectiveStmt ... \n");
            // ROSE_ASSERT(false);
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declarationStatement);
               ROSE_ASSERT(functionDeclaration != NULL);
            // DQ (8/29/2005): "template" keyword now output by Unparse_ExprStmt::outputTemplateSpecializationSpecifier()
            // curprint ( string("template ";
               unparseFuncDeclStmt(functionDeclaration,info);
               break;
             }

          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
            // printf ("Unparsing of SgTemplateInstantiationMemberFunctionDecl in unparseTemplateInstantiationDirectiveStmt ... \n");
            // ROSE_ASSERT(false);
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declarationStatement);
               ROSE_ASSERT(memberFunctionDeclaration != NULL);

            // DQ (5/31/2005): for now we will only output directives for template member functions and not non-template 
            // member functions.  In the case of a template class NOT output as a specialization then the template 
            // instantiation directive for a non-templated member function is allows (likely is just instatiates the class).
               if (memberFunctionDeclaration->isTemplateFunction() == true)
                  {
                 // DQ (8/29/2005): "template" keyword now output by Unparse_ExprStmt::outputTemplateSpecializationSpecifier()
                 // curprint ( string("template ";
#if 0
                     printf ("memberFunctionDeclaration = %p = %s = %s \n",
                          memberFunctionDeclaration,
                          memberFunctionDeclaration->class_name().c_str(),
                          memberFunctionDeclaration->get_name().str());
#endif
                    unparseMFuncDeclStmt(memberFunctionDeclaration,info);
                  }
                 else
                  {
                 // It seems that if the class declaration is not specialized then the non-member function template 
                 // instantiation directive is allowed. But we don't at this point know if the class declaration has 
                 // been output so skip all template instantiations of non-template member functions (in general).
                 // Issue a warning message for now!
#if PRINT_DEVELOPER_WARNINGS
                    printf ("Warning: Skipping output of directived to build non-template member functions! \n");
                    curprint ( string("\n/* Warning: Skipping output of directived to build non-template member functions! */"));
#endif
                  }
               break;
             }

          case V_SgVariableDeclaration:
             {
               printf ("Unparsing of SgVariableDeclaration in unparseTemplateInstantiationDirectiveStmt not implemented \n");
               ROSE_ASSERT(false);
               break;
             }

       // DQ (8/13/2005): Added this case because it comes up in compiling KULL (KULL/src/transport/CommonMC/Particle/mcapm.cc)
          case V_SgMemberFunctionDeclaration:
             {
            // DQ (8/31/2005): This should be an error now!  Template instantiations never generate
            // a SgMemberFunctionDeclaration and always generate a SgTemplateInstantiationMemberFunctionDecl
               printf ("Error: SgMemberFunctionDeclaration case found in unparseTemplateInstantiationDirectiveStmt ... (exiting) \n");
               ROSE_ASSERT(false);
#if 0
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declarationStatement);
               ROSE_ASSERT(memberFunctionDeclaration != NULL);

               ROSE_ASSERT (memberFunctionDeclaration->isTemplateFunction() == false);

            // curprint ( string("\n/* Skipped unparsing of SgMemberFunctionDeclaration in unparseTemplateInstantiationDirectiveStmt untested */ \n ";
            // curprint ( string("template ";
            // unparseMFuncDeclStmt(memberFunctionDeclaration,info);
#endif
               break;
             }

          default:
             {
               printf ("Error: default reached in switch (declarationStatement = %s) \n",declarationStatement->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }
   }


void
Unparse_ExprStmt::unparseTemplateInstantiationDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (2/29/2004): New function to support templates
     SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(stmt);
     ROSE_ASSERT(templateInstantiationDeclaration != NULL);

     SgClassDeclaration* classDeclaration = isSgClassDeclaration(templateInstantiationDeclaration);
     ROSE_ASSERT(classDeclaration != NULL);

#if OUTPUT_DEBUGGING_CLASS_NAME
     printf ("Inside of unparseTemplateInstantiationDeclStmt() stmt = %p/%p name = %s  templateName = %s transformed = %s/%s prototype = %s compiler-generated = %s compiler-generated and marked for output = %s \n",
          classDeclaration,templateInstantiationDeclaration,
          templateInstantiationDeclaration->get_name().str(),
          templateInstantiationDeclaration->get_templateName().str(),
          unp->isTransformed (templateInstantiationDeclaration) ? "true" : "false",
          (templateInstantiationDeclaration->get_file_info()->isTransformation() == true) ? "true" : "false",
          (templateInstantiationDeclaration->get_definition() == NULL) ? "true" : "false",
          (templateInstantiationDeclaration->get_file_info()->isCompilerGenerated() == true) ? "true" : "false",
          (templateInstantiationDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() == true) ? "true" : "false");
#endif

  // Call the unparse function for a class declaration
  // If the template has not been modified then don't output the template specialization 
  // (force the backend compiler to handle the template specialization and instantiation).

     bool outputClassTemplateInstantiation = true;
     if (isTransformed (templateInstantiationDeclaration) == true )
        {
       // If the template has been transformed then we have to output the special version 
       // of the template as a template specialization.

       // If a class template has been modified then we need to make sure that all the 
       //      static data members, and 
       //      member functions 
       // are instantiated (on the next pass through the prelinker). The process should 
       // involve a call to the EDG function:
       //      static void set_instantiation_required_for_template_class_members (a_type_ptr class_type)
       // I am not currently sure how to make this happen, but it should involve the *.ti 
       // files (I guess).

       // printf ("Calling unparseClassDeclStmt to unparse the SgTemplateInstantiationDecl \n");

#if 0
       // This case is not supported if member functions or static data members are present in the class
       // (could generate code which would compile but not link!).
          printf ("\n\n");
          printf ("WARNING: Transformations on templated classes can currently generate code which \n");
          printf ("         will not compile, since member function and static data members are not \n");
          printf ("         presently generated for the new specialization of the transformed template class. \n");
          printf ("\n");
#endif

#if 0
       // DQ (5/8/2004): Make this an explicit specialization (using the newer C++ syntax to support this)
       // DQ (3/2/2005): Uncommented output of "template <>"
          bool locatedInNamespace = isSgNamespaceDefinitionStatement(classDeclaration->get_scope()) != NULL;
          printf ("locatedInNamespace = %s \n",locatedInNamespace ? "true" : "false");

          if (locatedInNamespace == true)
             {
               curprint ( string("namespace std {"));
             }

       // curprint ( string("\n /* unparseTemplateInstantiationDeclStmt */ ";

       // DQ (8/29/2005): This is now output by the Unparse_ExprStmt::outputTemplateSpecializationSpecifier() member function
       // curprint ( string("\ntemplate <> \n";

          unparseClassDeclStmt(classDeclaration,info);

          if (locatedInNamespace == true)
             {
               curprint ( string("   }"));
             }
#endif

       // DQ (8/19/2005): If transformed then always output the template instantiation (no matter where it is from)
          outputClassTemplateInstantiation = true;
        }
       else
        {
          if ( templateInstantiationDeclaration->get_file_info()->isOutputInCodeGeneration() == true )
             {
#if PRINT_DEVELOPER_WARNINGS
               printf ("Class template is marked for output in the current source file. \n");
#endif
               outputClassTemplateInstantiation = true;
             }
            else
             {
#if PRINT_DEVELOPER_WARNINGS
               printf ("Class template is NOT marked for output in the current source file. \n");
#endif
             }
#if 0
       // If not transformed then we only want to output the template (and usually only the 
       // name of the template specialization) in variable declarations and the like.
       // These locations control the output of the template specialization explicitly 
       // through the SgUnparse_Info object (default for outputClassTemplateName is false). 
       // printf ("info.outputClassTemplateName() = %s \n",info.outputClassTemplateName() ? "true" : "false");
          if ( info.outputClassTemplateName() == true )
             {
            // printf ("Calling unparseClassDeclStmt to unparse the SgTemplateInstantiationDecl \n");

            // DQ (9/8/2004):
            // The unparseClassDeclStmt does much more that what we require and is not setup to 
            // handle templates well, it generally works well if the template name required does 
            // not need any qualification (e.g. std::templateName<int>).  Thus don't use the 
            // unparseClassDeclStmt and just output the qualified template name.
            // unparseClassDeclStmt(classDeclaration,info);

            // Output the qualified template name
               curprint ( templateInstantiationDeclaration->get_qualified_name().str());
             }
#if 0
            else
             {
               printf ("Skipping call to unparse the SgTemplateInstantiationDecl = %p \n",
                    templateInstantiationDeclaration);
             }
#endif
#endif
        }

     if (outputClassTemplateInstantiation == true)
        {
       // DQ (5/8/2004): Make this an explicit specialization (using the newer C++ syntax to support this)
       // DQ (3/2/2005): Uncommented output of "template <>"

       // DQ (9/1/2005): This is a temporary fix to handle a bug in g++ (3.3.x and 3.4.x) which 
       // requires class specialization declared in a namespace to be output in a namespace instead 
       // of with the alternative proper namespace name qualifiers.
       // Note: If this is in a nested namespace then it might be that this will not work 
       // (since namespace names can't be name qualified).  A loop over the nested namespaces might 
       // be required to handle this case, better yet would be to transform the AST in a special pass
       // to fix this up to handle backend compiler limitations (as we currently do for other backend 
       // compiler bugs).
          SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(classDeclaration->get_scope());
          bool locatedInNamespace = (namespaceDefinition != NULL);
       // printf ("locatedInNamespace = %s \n",locatedInNamespace ? "true" : "false");
          if (locatedInNamespace == true)
             {
               string namespaceName = namespaceDefinition->get_namespaceDeclaration()->get_name().str();
            // curprint ( string("namespace std /* temporary fix for g++ bug in namespace name qualification */ \n   {";
               curprint ( string("namespace " ) + namespaceName + " /* temporary fix for g++ bug in namespace name qualification */ \n   {");
             }

       // curprint ( string("\n /* unparseTemplateInstantiationDeclStmt */ ";
       // DQ (8/29/2005): This is now output by the Unparse_ExprStmt::outputTemplateSpecializationSpecifier() member function
       // curprint ( string("\ntemplate <> \n";
          unparseClassDeclStmt(classDeclaration,info);

          if (locatedInNamespace == true)
             {
               curprint ( string("   }"));
             }
        }
       else
        {
          curprint ( string("/* Skipped output of template class declaration (name = " ) + templateInstantiationDeclaration->get_qualified_name().str() + ") */ \n");

       // If not transformed then we only want to output the template (and usually only the 
       // name of the template specialization) in variable declarations and the like.
       // These locations control the output of the template specialization explicitly 
       // through the SgUnparse_Info object (default for outputClassTemplateName is false). 
       // printf ("info.outputClassTemplateName() = %s \n",info.outputClassTemplateName() ? "true" : "false");
          if ( info.outputClassTemplateName() == true )
             {
            // printf ("Calling unparseClassDeclStmt to unparse the SgTemplateInstantiationDecl \n");

            // DQ (9/8/2004):
            // The unparseClassDeclStmt does much more that what we require and is not setup to 
            // handle templates well, it generally works well if the template name required does 
            // not need any qualification (e.g. std::templateName<int>).  Thus don't use the 
            // unparseClassDeclStmt and just output the qualified template name.
            // unparseClassDeclStmt(classDeclaration,info);

            // Output the qualified template name
               curprint ( templateInstantiationDeclaration->get_qualified_name().str());
             }
        }
   }


#if 0
// DQ (8/13/2007): This has been moved to the base class (language independent code)

bool 
Unparse_ExprStmt::isTransformed(SgStatement* stmt)
   {
  // This function must traverse the AST and look for any sign that 
  // the subtree has been transformed.  This might be a difficult 
  // function to write.  We might have to force transformations to
  // do something to make their presence better known (e.g. removing
  // a statement will leave no trace in the AST of the transformation).

  // DQ (3/2/2005): Change this to see if we can output each specialization 
  // as if we were transforming each template specialization
  // Assume no transformation at the moment while we debug templates.

  // DQ (6/29/2005): return false while we try to return to compiling KULL
#if 1
     return false;
#else
     return true;
#endif
   }
#endif


void
Unparse_ExprStmt::unparseTemplateInstantiationFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (6/8/2005): If this is an inlined function, we need to make sure that 
  // the function has not been used anywhere before where we output it here.


  // DQ (3/24/2004): New function to support templates
     SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = 
          isSgTemplateInstantiationFunctionDecl(stmt);
     ROSE_ASSERT(templateInstantiationFunctionDeclaration != NULL);
     ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_file_info() != NULL);

     SgFunctionDeclaration* functionDeclaration = 
          isSgFunctionDeclaration(templateInstantiationFunctionDeclaration);

     ROSE_ASSERT(functionDeclaration != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_NAME
     printf ("Inside of unparseTemplateInstantiationFunctionDeclStmt() name = %s  transformed = %s prototype = %s static = %s compiler generated = %s transformed = %s output = %s \n",
       // templateInstantiationFunctionDeclaration->get_name().str(),
          templateInstantiationFunctionDeclaration->get_qualified_name().str(),
          isTransformed (templateInstantiationFunctionDeclaration) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_definition() == NULL) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_declarationModifier().get_storageModifier().isStatic() == true) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_file_info()->isCompilerGenerated() == true) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_file_info()->isTransformation() == true) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true) ? "true" : "false");
#endif

     bool outputInstantiatedTemplateFunction = false;
     if ( isTransformed (templateInstantiationFunctionDeclaration) == true )
        {
       // DQ (5/16/2005): This is an attempt to remove explicit declarations of specializations which 
       // are preventing template instantiations of function definitions within the prelinking process.
          bool skipforwardDeclarationOfTemplateSpecialization = 
               (templateInstantiationFunctionDeclaration->get_file_info()->isCompilerGenerated() == true) && 
               (templateInstantiationFunctionDeclaration->get_definition() == NULL) &&
               (templateInstantiationFunctionDeclaration->get_definingDeclaration() == NULL);
          if (skipforwardDeclarationOfTemplateSpecialization == true)
             {
            // This is a compiler generated forward function declaration of a template instatiation, so skip it!
#if PRINT_DEVELOPER_WARNINGS
               printf ("This is a compiler generated forward function declaration of a template instatiation, so skip it! \n");
               curprint ( string("\n/* Skipping output of compiler generated forward function declaration of a template specialization */"));
#endif
               return;
             }

          bool skipInlinedTemplates = templateInstantiationFunctionDeclaration->get_functionModifier().isInline();
          if (skipInlinedTemplates == true)
             {
            // skip output of inlined templates since these are likely to have been used 
            // previously and would be defined too late if provided as an inline template 
            // specialization output in the source code.
#if PRINT_DEVELOPER_WARNINGS
               printf ("This is an inlined template which might have been used previously (skipping output of late specialization) \n");
               curprint ( string("\n/* Skipping output of inlined template specialization */"));
#endif
               return;
             }

#if PRINT_DEVELOPER_WARNINGS
          curprint ( string("\n/* In unparseTemplateInstantiationFunctionDeclStmt(): part of transformation - output the template function declaration */ \n "));
#endif
          outputInstantiatedTemplateFunction = true;
        }
       else
        {
       // Also output the template member function declaration the template declaration appears in the source file.
          string currentFileName = getFileName();
          ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_templateDeclaration() != NULL);
          ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_templateDeclaration()->get_file_info() != NULL);
          ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_templateDeclaration()->get_file_info()->get_filename() != NULL);
          string declarationFileName = templateInstantiationFunctionDeclaration->get_templateDeclaration()->get_file_info()->get_filename();
#if 0
          printf ("In unparseTemplateInstantiationFunctionDeclStmt(): currentFileName     = %s \n",currentFileName.c_str());
          printf ("In unparseTemplateInstantiationFunctionDeclStmt(): declarationFileName = %s \n",declarationFileName.c_str());
          printf ("templateInstantiationFunctionDeclaration source position information: \n");
          templateInstantiationFunctionDeclaration->get_file_info()->display("debug");
#endif
       // if ( declarationFileName == currentFileName )
       // if ( declarationFileName == currentFileName && templateInstantiationMemberFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true)
          if ( templateInstantiationFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true )
             {
            // printf ("Declaration appears in the current source file. \n");
#if PRINT_DEVELOPER_WARNINGS
               curprint ( string("\n/* In unparseTemplateInstantiationFunctionDeclStmt(): output the template function declaration */ \n "));
#endif
               outputInstantiatedTemplateFunction = true;
             }
            else
             {
            // printf ("Declaration does NOT appear in the current source file. \n");
            // curprint ( string("\n/* In unparseTemplateInstantiationFunctionDeclStmt(): skip output of template function declaration */ \n ";
#if PRINT_DEVELOPER_WARNINGS
               curprint ( string("/* Skipped output of template function declaration (name = " ) + templateInstantiationFunctionDeclaration->get_qualified_name().str() + ") */ \n");
#endif
             }
        }


     if (outputInstantiatedTemplateFunction == true)
       {
       // const SgTemplateArgumentPtrList& templateArgListPtr = templateInstantiationFunctionDeclaration->get_templateArguments();
#if 0
       // DQ (8/29/2005): This is now output by the Unparse_ExprStmt::outputTemplateSpecializationSpecifier() member function

       // DQ (3/2/2005): Comment out use of "template<>"
       // DQ (5/8/2004): Make this an explicit specialization (using the newer C++ syntax to support this)
       // Output: "template <type>" before function declaration.
          ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_templateArguments() != NULL);
          if (templateInstantiationFunctionDeclaration->get_templateArguments()->size() > 0)
             {
            // printf ("Declaration is a prototype functionDeclaration->get_definition() = %p \n",functionDeclaration->get_definition());
               curprint ( string("\n/* ROSE generated template specialization " ) + 
                      ( (functionDeclaration->get_definition() == NULL) ? "(prototype)" : "(explicit definition)") + 
                      " */");
               curprint ( string("\ntemplate <> "));
             }
       // unparseTemplateArguments(templateArgListPtr,info);
#endif

       // Now output the function declaration
       // curprint ( string("/* Now output the function declaration */\n ";
          unparseFuncDeclStmt(functionDeclaration,info);
        }
   }

void
Unparse_ExprStmt::unparseTemplateInstantiationMemberFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // Rules for output of member templates functions:
  //  1) When we unparse the template declaration as a string EDG removes the member 
  //     function definitions so we are forced to output all template member functions.
  //  2) If the member function is specified outside of the class then we don't have to
  //     explicitly output the instantiation.

  // DQ (3/24/2004): New function to support templates
     SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = 
          isSgTemplateInstantiationMemberFunctionDecl(stmt);
     ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_NAME
     printf ("Inside of unparseTemplateInstantiationMemberFunctionDeclStmt() = %p name = %s  transformed = %s prototype = %s static = %s compiler generated = %s transformation = %s output = %s \n",
       // templateInstantiationMemberFunctionDeclaration->get_templateName().str(),
          templateInstantiationMemberFunctionDeclaration,
          templateInstantiationMemberFunctionDeclaration->get_qualified_name().str(),
          isTransformed (templateInstantiationMemberFunctionDeclaration) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_definition() == NULL) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_declarationModifier().get_storageModifier().isStatic() == true) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_file_info()->isCompilerGenerated() == true) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_file_info()->isTransformation() == true) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true) ? "true" : "false");
#endif

  // DQ (6/1/2005): Use this case when PROTOTYPE_INSTANTIATIONS_IN_IL is to true in EDG's host_envir.h
     bool outputMemberFunctionTemplateInstantiation = false;
     if ( isTransformed (templateInstantiationMemberFunctionDeclaration) == true )
        {
       // Always output the template member function declaration if they are transformed.
       // printf ("templateInstantiationMemberFunctionDeclaration has been transformed \n");

          SgDeclarationStatement* definingDeclaration = templateInstantiationMemberFunctionDeclaration->get_definingDeclaration();
       // ROSE_ASSERT(definingDeclaration != NULL);
          SgMemberFunctionDeclaration* memberFunctionDeclaration = 
               (definingDeclaration == NULL) ? NULL : isSgMemberFunctionDeclaration(definingDeclaration);
       // ROSE_ASSERT(memberFunctionDeclaration != NULL);

       // SgTemplateDeclaration* templateDeclaration = templateInstantiationMemberFunctionDeclaration->get_templateDeclaration();
       // ROSE_ASSERT(templateDeclaration != NULL);

          bool hasDefinition = (memberFunctionDeclaration != NULL && memberFunctionDeclaration->get_definition() != NULL);

       // printf ("hasDefinition = %s \n",hasDefinition ? "true" : "false");

          if (hasDefinition == true)
             {
            // printf ("Output this member function \n");
               outputMemberFunctionTemplateInstantiation = true;
             }
        }
       else
        {
       // Also output the template member function declaration the template declaration appears in the source file.
          string currentFileName = getFileName();
          ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration->get_templateDeclaration() != NULL);
          ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration->get_templateDeclaration()->get_file_info() != NULL);
          ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration->get_templateDeclaration()->get_file_info()->get_filename() != NULL);
          string declarationFileName = templateInstantiationMemberFunctionDeclaration->get_templateDeclaration()->get_file_info()->get_filename();
#if 0
          printf ("In unparseTemplateInstantiationMemberFunctionDeclStmt(): currentFileName     = %s \n",currentFileName.c_str());
          printf ("In unparseTemplateInstantiationMemberFunctionDeclStmt(): declarationFileName = %s \n",declarationFileName.c_str());
          printf ("templateInstantiationMemberFunctionDeclaration source position information: \n");
          templateInstantiationMemberFunctionDeclaration->get_file_info()->display("template instantiation: debug");
          templateInstantiationMemberFunctionDeclaration->get_templateDeclaration()->get_file_info()->display("template declaration: debug");
#endif

       // DQ (8/19/2005): We only have to test for if the template instantiation
       // is marked for output!
       // DQ (8/17/2005): We have to additionally mark the member function 
       // instantiation for output since the correct specialization would 
       // disqualify the member function for output! The rules are simple:
       //    1) The member function must be defined in the current source file 
       //       (else it will be defined in a header file and we need not output 
       //       it explicitly (since we handle only non-transformed template 
       //       instantiations in this branch).  And,
       //    2) The member function must be marked for output to avoid the output
       //       of the member function in the case of a template specialiazation.
       // if ( declarationFileName == currentFileName )
       // if ( declarationFileName == currentFileName && templateInstantiationMemberFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true)
          if ( templateInstantiationMemberFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true )
             {
            // printf ("Declaration appears in the current source file. \n");
               outputMemberFunctionTemplateInstantiation = true;
             }
            else
             {
#if 0
               printf ("Declaration does NOT appear in the current source file (templateInstantiationMemberFunctionDeclaration = %p = %s) \n",
                    templateInstantiationMemberFunctionDeclaration,
                    templateInstantiationMemberFunctionDeclaration->get_qualified_name().str());
               printf ("   isSpecialization() = %s \n",templateInstantiationMemberFunctionDeclaration->isSpecialization() ? "true" : "false");
#endif
             }
        }

#if 0
     printf ("outputMemberFunctionTemplateInstantiation = %s \n",outputMemberFunctionTemplateInstantiation ? "true" : "false");
#endif

     if (outputMemberFunctionTemplateInstantiation == true )
        {
          SgFunctionDeclaration* memberFunctionDeclaration = 
               isSgMemberFunctionDeclaration(templateInstantiationMemberFunctionDeclaration);
          ROSE_ASSERT(memberFunctionDeclaration != NULL);

       // DQ (3/3/2005): Commented out since it was a problem in test2004_36.C
       // DQ (5/8/2004): Make this an explicit specialization (using the newer C++ syntax to support this)
       // curprint ( string("template <> \n";
       // ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration->get_templateArguments() != NULL);
       // if (templateInstantiationMemberFunctionDeclaration->get_templateArguments()->size() > 0)
          if (templateInstantiationMemberFunctionDeclaration->isSpecialization() == true)
             {
               if ( (templateInstantiationMemberFunctionDeclaration->get_file_info()->isCompilerGenerated() == true) &&
                    (templateInstantiationMemberFunctionDeclaration->isForward() == true) )
                  {
                 // This is a ROSE generated forward declaration of a ROSE specialized member function (required).
                 // It is built in ROSE/src/roseSupport/templateSupport.C void fixupInstantiatedTemplates ( SgProject* project ).
                 // The forward declaration is placed directly after the template declaration so that no uses of the function can exist
                 // prior to its declaration.  Output a message into the gnerated source code identifying this transformation.
#if PRINT_DEVELOPER_WARNINGS
                    curprint ( string("\n/* ROSE generated forward declaration of the ROSE generated member template specialization */"));
#endif
                  }
                 else
                  {
                 // This is the ROSE generated template specialization for the template member function 
                 // (required to be defined since the function is used (called)).  This function is defined 
                 // at the end of file and may be defined there because a forward declaration for the 
                 // specialization was output directly after the template declaration (before any use of 
                 // the function could have been made ???).
#if PRINT_DEVELOPER_WARNINGS
                    curprint ( string("\n/* ROSE generated member template specialization */"));
#endif
                  }

            // DQ (8/27/2005): This might be required for g++ 3.4.x and optional for g++ 3.3.x
            // DQ (8/19/2005): It is incorrect when used for non-template member functions on templated
            // classes defined in the class
            // Output the syntax for template specialization (appears to be largely optional (at least with GNU g++)
            // curprint ( string("\ntemplate <> ";
             }

       // DQ (8/29/2005): This is now output by the Unparse_ExprStmt::outputTemplateSpecializationSpecifier() member function
       // curprint ( string("\ntemplate <> ";

       // printf ("Calling unparseMFuncDeclStmt() \n");
          unparseMFuncDeclStmt(memberFunctionDeclaration,info);
        }
       else
        {
#if 0
          curprint ( string("/* Skipped output of member function declaration (name = ") + templateInstantiationMemberFunctionDeclaration->get_templateName().getString() + string(") */ \n") );
#endif
#if PRINT_DEVELOPER_WARNINGS
          curprint ( string("/* Skipped output of template member function declaration (name = " ) + templateInstantiationMemberFunctionDeclaration->get_qualified_name().str() + ") */ \n");
#endif
        }
   }

void
Unparse_ExprStmt::unparsePragmaDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(stmt);
     ROSE_ASSERT(pragmaDeclaration != NULL);

     SgPragma* pragma = pragmaDeclaration->get_pragma();
     ROSE_ASSERT(pragma != NULL);

  // Request from Boyanna at ANL:
  // DQ (6/22/2006): Start all pragmas at the start of the line.  Since these are
  // handled as IR nodes (#pragma is part of the C and C++ grammar afterall)they 
  // are indented for as any other sort of statements.  I have added a CR
  // to put the pragma at the start of the next line.  A better solution might be to 
  // have the indent mechanism look ahead to see any upcoming SgPragmaDeclarations
  // so that the indentation (insertion of extra spaces) could be skipped.  This would
  // avoid the insertion of empty lines in the generated code.
  // curprint ( string("#pragma " + pragma->get_pragma() + "\n";

     string pragmaString = pragma->get_pragma();
     string identSubstring = "ident";

  // Test for and ident string (which has some special quoting rules that apply to some compilers)
     if (pragmaString.substr(0,identSubstring.size()) == identSubstring)
        {
          curprint ( string("\n#pragma ident \"" ) + pragmaString.substr(identSubstring.size()+1) + "\"\n");
        }
       else
        {
          curprint ( string("\n#pragma " ) + pragma->get_pragma() + "\n");
        }

  // printf ("Output the pragma = %s \n",pragma->get_pragma());
  // ROSE_ASSERT (0);
   }


#if 0
// DQ (8/13/2007): This has been moved to the base class (language independent code)
// Since I think it is not specific to any one language.

void
Unparse_ExprStmt::unparseGlobalStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgGlobal* globalScope = isSgGlobal(stmt);
     ROSE_ASSERT(globalScope != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("\n ***** Unparsing the global Scope ***** \n\n");
#endif

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (globalScope);
#endif

  // curprint ( string(" /* global scope size = " + globalScope->get_declarations().size() + " */ \n ";

  // Setup an iterator to go through all the statements in the top scope of the file.
     SgDeclarationStatementPtrList & globalStatementList = globalScope->get_declarations();
     SgDeclarationStatementPtrList::iterator statementIterator = globalStatementList.begin();
     while ( statementIterator != globalStatementList.end() )
        {
          SgStatement* currentStatement = *statementIterator;
          ROSE_ASSERT(currentStatement != NULL);

//        unp->opt.display("Before unp->statementFromFile");
          bool unparseStatementIntoSourceFile = statementFromFile (currentStatement,getFileName());

          if (ROSE_DEBUG > 3)
             {
               cout << "unparseStatementIntoSourceFile = " 
                    << ( (unparseStatementIntoSourceFile == true) ? "true" : "false" ) << endl;

               printf ("block scope statement = %p = %s \n",currentStatement,currentStatement->class_name().c_str());
               currentStatement->get_file_info()->display("global scope statement: debug");
             }
#if 0
          curprint ( string("/* global scope (" ) + currentStatement->class_name() + "): unparseStatementIntoSourceFile = " + ( (unparseStatementIntoSourceFile == true) ? "true" : "false" ) + " */ ");
#endif

          if (unparseStatementIntoSourceFile == true)
             {
               if (ROSE_DEBUG > 3)
                  {
                 // (*primary_os)
                    cout << "In run_unparser(): getLineNumber(currentStatement) = "
#if 1
                         << currentStatement->get_file_info()->displayString()
#else
                         << ROSE::getLineNumber(currentStatement)
                         << " getFileName(currentStatement) = " 
                         << ROSE::getFileName(currentStatement)
#endif
                         << " unp->cur_index = " 
                         << unp->cur_index
                         << endl;
                  }

            // DQ (6/4/2007): Make a new SgUnparse_Info object for each statement in global scope
            // This should permit children to set the current_scope and not effect other children
            // see test2007_56.C for example "namespace A { extern int x; } int A::x = 42;"
            // Namespace definition scope should not effect scope set in SgGlobal.
            // unparseStatement(currentStatement, info);
               SgUnparse_Info infoLocal(info);
               unparseStatement(currentStatement, infoLocal);
             }
            else
             {
            // printf ("Skipped unparsing %s (global declaration) \n",currentStatement->sage_class_name());
             }

       // Go to the next statement
          statementIterator++;
        }

  // DQ (5/27/2005): Added support for compiler-generated statements that might appear at the end of the applications
  // printf ("At end of unparseGlobalStmt \n");
  // outputCompilerGeneratedStatements(info);

  // DQ (4/21/2005): Output a new line at the end of the file (some compilers complain if this is not present)
     unp->cur.insert_newline(1);
   }
#endif


#if 0
// DQ (11/15/2004): This is a class from which all declarations are built so it should not
// ever have to be unparsed (since no IR nodes exist of this base class type).
void
Unparse_ExprStmt::unparseDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // SgDeclarationStatement* decl_stmt = isSgDeclarationStatement(stmt);
  // unparseStatement(decl_stmt, info);
   }
#endif

#if 0
// DQ (8/13/2007): This is a class from which all scopes are built so it should not
// ever have to be unparsed (since no IR nodes exist of this base class type).
void Unparse_ExprStmt::unparseScopeStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgScopeStatement* scope_stmt = isSgScopeStatement(stmt);
     ROSE_ASSERT(scope_stmt != NULL);

     stringstream  out;
     scope_stmt->print_symboltable("UNPARSE", out);
     curprint ( out.str());
   }
#endif


#if 0
// DQ (8/13/2007): This has been moved to the base class (language independent code)

void Unparse_ExprStmt::unparseFuncTblStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgFunctionTypeTable* functbl_stmt = isSgFunctionTypeTable(stmt);
     ROSE_ASSERT(functbl_stmt != NULL);

     stringstream  out;
     functbl_stmt->print_functypetable(out);
     curprint ( out.str());
   }
#endif

void
Unparse_ExprStmt::unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgBasicBlock* basic_stmt = isSgBasicBlock(stmt);
     ROSE_ASSERT(basic_stmt != NULL);

  // unparseAttachedPreprocessingInfo(basic_stmt, info, PreprocessingInfo::before);

     unp->cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);
     curprint ( string("{"));
     unp->cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);

     if (basic_stmt->get_asm_function_body().empty() == false)
        {
       // This is an asm function body.
          curprint (basic_stmt->get_asm_function_body());

       // Make sure this is a function definition.
          ROSE_ASSERT(isSgFunctionDefinition(basic_stmt->get_parent()) != NULL);
        }

  // DQ (1/9/2007): This is useful for understanding which blocks are marked as compiler generated.
  // curprint ( string(" /* block compiler generated = " + (basic_stmt->get_startOfConstruct()->isCompilerGenerated() ? "true" : "false") + " */ \n ";

  // curprint ( string(" /* block size = " + basic_stmt->get_statements().size() + " */ \n ";

  // printf ("block scope = %p = %s \n",basic_stmt,basic_stmt->class_name().c_str());
  // basic_stmt->get_file_info()->display("basic_stmt block scope: debug");

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (basic_stmt);
#endif

     SgStatementPtrList::iterator p = basic_stmt->get_statements().begin();
     while(p != basic_stmt->get_statements().end())
        { 
          ROSE_ASSERT((*p) != NULL);

          unparseStatement((*p), info);

          p++;
        }

  // DQ (3/17/2005): This helps handle cases such as void foo () { #include "constant_code.h" }
     unparseAttachedPreprocessingInfo(basic_stmt, info, PreprocessingInfo::inside);

     unp->cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
     curprint ( string("}"));
     unp->cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
   }

#if 0
// DQ (8/13/2007): This has been moved to the base class (language independent code)

//--------------------------------------------------------------------------------
//  void Unparse_ExprStmt::num_stmt_in_block
//
//  returns the number of statements in the basic block
//--------------------------------------------------------------------------------  
int Unparse_ExprStmt::num_stmt_in_block(SgBasicBlock* basic_stmt) {
  //counter to keep number of statements in the block
  int num_stmt = 0;
  SgStatementPtrList::iterator p = basic_stmt->get_statements().begin();
  while (p != basic_stmt->get_statements().end()) {
    num_stmt++;
    p++;
  }

  return num_stmt;
}
#endif

// Determine how many "else {}"'s an outer if that has an else clause needs to
// prevent dangling if problems
static size_t countElsesNeededToPreventDangling(SgStatement* s) {
  // The basic rule here is that anything that has a defined end marker
  // (i.e., cannot end with an unmatched if statement) returns 0, everything
  // else (except if) gets the correct number of elses from its body
  switch (s->variantT()) {
    case V_SgCaseOptionStmt: return countElsesNeededToPreventDangling(isSgCaseOptionStmt(s)->get_body());
    case V_SgCatchStatementSeq: {
      SgCatchStatementSeq* cs = isSgCatchStatementSeq(s);
      const SgStatementPtrList& seq = cs->get_catch_statement_seq();
      ROSE_ASSERT (!seq.empty());
      return countElsesNeededToPreventDangling(seq.back());
    }
    case V_SgDefaultOptionStmt: return countElsesNeededToPreventDangling(isSgCaseOptionStmt(s)->get_body());
    case V_SgLabelStatement: return countElsesNeededToPreventDangling(isSgLabelStatement(s)->get_statement());
    case V_SgCatchOptionStmt: return countElsesNeededToPreventDangling(isSgCatchOptionStmt(s)->get_body());
    case V_SgForStatement: return countElsesNeededToPreventDangling(isSgForStatement(s)->get_loop_body());
    case V_SgIfStmt: {
      SgIfStmt* ifs = isSgIfStmt(s);
      if (ifs->get_false_body() != NULL) {
        return 0;
      } else {
        return countElsesNeededToPreventDangling(ifs->get_true_body()) + 1;
      }
    }
    case V_SgWhileStmt: return countElsesNeededToPreventDangling(isSgWhileStmt(s)->get_body());
    case V_SgSwitchStatement: ROSE_ASSERT(isSgBasicBlock(isSgSwitchStatement(s)->get_body())); return 0;
    default: return 0;
  }
}

void Unparse_ExprStmt::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (12/13/2005): I don't like this implementation with the while loop...

  // printf ("Unparse if statement stmt = %p \n",stmt);

     SgIfStmt* if_stmt = isSgIfStmt(stmt);
     assert (if_stmt != NULL);

     while (if_stmt != NULL)
        {
          SgStatement *tmp_stmt = NULL;
          curprint ( string("if ("));
          SgUnparse_Info testInfo(info);
          testInfo.set_SkipSemiColon();
          testInfo.set_inConditional();
       // info.set_inConditional();
          if ( (tmp_stmt = if_stmt->get_conditional()) )
             {
            // Unparse using base class function so we get any required comments and CPP directives.
            // unparseStatement(tmp_stmt, testInfo);
               UnparseLanguageIndependentConstructs::unparseStatement(tmp_stmt, testInfo);
             }
          testInfo.unset_inConditional();
          curprint ( string(") "));

          if ( (tmp_stmt = if_stmt->get_true_body()) ) 
             {
            // printf ("Unparse the if true body \n");
            // curprint ( string("\n/* Unparse the if true body */ \n") );
               unp->cur.format(tmp_stmt, info, FORMAT_BEFORE_NESTED_STATEMENT);

            // Unparse using base class function so we get any required comments and CPP directives.
            // unparseStatement(tmp_stmt, info);
               UnparseLanguageIndependentConstructs::unparseStatement(tmp_stmt, info);

               unp->cur.format(tmp_stmt, info, FORMAT_AFTER_NESTED_STATEMENT);
            // curprint ( string("\n/* DONE: Unparse the if true body */ \n") );
             }

          if ( (tmp_stmt = if_stmt->get_false_body()) )
             {
               size_t elsesNeededForInnerIfs = countElsesNeededToPreventDangling(if_stmt->get_true_body());
               for (size_t i = 0; i < elsesNeededForInnerIfs; ++i) {
                 curprint ( string(" else {}") ); // Ensure this else does not match an inner if statement
               }
               unp->cur.format(if_stmt, info, FORMAT_BEFORE_STMT);
               curprint ( string("else "));
               if_stmt = isSgIfStmt(tmp_stmt);
               if (if_stmt == NULL) {
                 unp->cur.format(tmp_stmt, info, FORMAT_BEFORE_NESTED_STATEMENT);

              // curprint ( string("\n/* Unparse the if false body */ \n") );
              // Unparse using base class function so we get any required comments and CPP directives.
              // unparseStatement(tmp_stmt, info);
                 UnparseLanguageIndependentConstructs::unparseStatement(tmp_stmt, info);
              // curprint ( string("\n/* DONE: Unparse the if false body */ \n") );

                 unp->cur.format(tmp_stmt, info, FORMAT_AFTER_NESTED_STATEMENT);
               }
             }
            else
             {
               if_stmt = NULL;
             }

       // DQ (12/16/2008): Need to process any associated CPP directives and comments
          if (if_stmt != NULL)
               unparseAttachedPreprocessingInfo(if_stmt, info, PreprocessingInfo::before);
        }
   }

#if 1
// DQ (8/13/2007): This is no longer used, I think, however it might be required for the A++/P++ array optimizer.

//--------------------------------------------------------------------------------
//  void Unparse_ExprStmt::unparseWhereStmt
//
//  This special function unparses where and elsewhere statements. Where 
//  statements are actually represented as for statements in the Sage program
//  tree. Thus, the type of the where_stmt is SgForStatement. The part that
//  we are interested in unparsing is in the initializer statement of the 
//  for statement. In particular, we want to unparse the arguments of the
//  rhs of the initializer. The rhs should be a function call expression.
//  The same applies for elsewhere statements.
//--------------------------------------------------------------------------------
void
Unparse_ExprStmt::unparseWhereStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgForStatement* where_stmt = isSgForStatement(stmt);
     ROSE_ASSERT(where_stmt != NULL);

     printf ("In Unparse_ExprStmt::unparseWhereStmt() \n");

     SgStatement *tmp_stmt;
  // DQ (4/7/2001) we don't want the unparser to depend on the array grammar
  // (so comment this out and introduce the A++ "where" statment in some other way)
#if 0
     if (ArrayClassSageInterface::isROSEWhereStatement(stmt))
        {
          curprint ( string("where ("));
        }
       else
        {
       // isROSEElseWhereStatement
          curprint ( string("elsewhere ("));
        }
#else
          curprint ( string("elsewhere ("));
#endif

     SgUnparse_Info newinfo(info);
     newinfo.set_SkipSemiColon();
  // if(where_stmt->get_init_stmt() != NULL ) {
     if(where_stmt->get_init_stmt().size() > 0 )
        {
          SgStatementPtrList::iterator i=where_stmt->get_init_stmt().begin();
          if ((*i) != NULL && (*i)->variant() == EXPR_STMT)
             {
               SgExprStatement* pExprStmt = isSgExprStatement(*i);
            // SgAssignOp* pAssignOp = isSgAssignOp(pExprStmt->get_the_expr());
               SgAssignOp* pAssignOp = isSgAssignOp(pExprStmt->get_expression());
               if ( pAssignOp != NULL )
                  {
                    SgFunctionCallExp* pFunctionCallExp = isSgFunctionCallExp(pAssignOp->get_rhs_operand());
                    if(pFunctionCallExp != NULL)
                       {
                         if(pFunctionCallExp->get_args())
                            {
                              SgExpressionPtrList& list = pFunctionCallExp->get_args()->get_expressions();
                              SgExpressionPtrList::iterator arg = list.begin();
                              while (arg != list.end())
                                 {
                                   unparseExpression((*arg), newinfo);
                                   arg++;
                                   if (arg != list.end())
                                      {
                                      curprint ( string(","));
                                      }
                                 }
                            }
                       } //pFunctionCallExp != NULL 
                  } //pAssignOp != NULL
             } //(*i).irep() != NULL && (*i).irep()->variant() == EXPR_STMT
        } //where_stmt->get_init_stmt() != NULL

     curprint ( string(")"));

     if ( (tmp_stmt = where_stmt->get_loop_body()) )
        {
          unparseStatement(tmp_stmt, info);
        }
       else
        {
          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
        }
   }
#endif


void
Unparse_ExprStmt::unparseForInitStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (7/11/2004): Added to simplify debugging for everyone (requested by Willcock)

  // printf ("Function not implemented yet! \n");

  // printf ("Unparse for loop initializers \n");
     SgForInitStatement* forInitStmt = isSgForInitStatement(stmt);
     ROSE_ASSERT(forInitStmt != NULL);

     SgStatementPtrList::iterator i = forInitStmt->get_init_stmt().begin();

  // DQ (12/8/2004): Build a new info object so that we can supress the unparsing of 
  // the base type once the first variable has been unparsed.
     SgUnparse_Info newinfo(info);

     while(i != forInitStmt->get_init_stmt().end())
        {
       // curprint(" /* unparseForInitStmt: " + (*i)->class_name() + " */ ");
          unparseStatement(*i, newinfo);
          i++;

       // After unparsing the first variable declaration with the type 
       // we want to unparse the rest without the base type.
          newinfo.set_SkipBaseType();

          if (i != forInitStmt->get_init_stmt().end())
             {
               curprint ( string(", "));
             }
        }

     curprint ( string("; "));
   }

void
Unparse_ExprStmt::unparseForStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Unparse for loop \n");
     SgForStatement* for_stmt = isSgForStatement(stmt);
     ROSE_ASSERT(for_stmt != NULL);

     curprint ( string("for ("));
     SgUnparse_Info newinfo(info);
     newinfo.set_SkipSemiColon();
     newinfo.set_inConditional();  // set to prevent printing line and file information

  // curprint(" /* initializer */ ");
     SgStatement *tmp_stmt = for_stmt->get_for_init_stmt();
  // curprint(" /* initializer: " + tmp_stmt->class_name() + " */ ");
  // ROSE_ASSERT(tmp_stmt != NULL);
     if (tmp_stmt != NULL)
        {
          unparseStatement(tmp_stmt,newinfo);
        }
       else
        {
          printf ("Warning in unparseForStmt(): for_stmt->get_for_init_stmt() == NULL \n");
          curprint ( string("; "));
        }
     newinfo.unset_inConditional();

#if 0
     SgExpression *tmp_expr = NULL;
     if ( (tmp_expr = for_stmt->get_test_expr()))
          unparseExpression(tmp_expr, info);
#else
  // DQ (12/13/2005): New code for handling the test (which could be a declaration!)
  // printf ("Output the test in the for statement format newinfo.inConditional() = %s \n",newinfo.inConditional() ? "true" : "false");
  // curprint (" /* test */ ");
     SgStatement *test_stmt = for_stmt->get_test();
     ROSE_ASSERT(test_stmt != NULL);
  // if ( test_stmt != NULL )
     SgUnparse_Info testinfo(info);
     testinfo.set_SkipSemiColon();
     testinfo.set_inConditional();
  // printf ("Output the test in the for statement format testinfo.inConditional() = %s \n",testinfo.inConditional() ? "true" : "false");
     unparseStatement(test_stmt, testinfo);
#endif
     curprint ( string("; "));

  // curprint ( string(" /* increment */ ";
  // SgExpression *increment_expr = for_stmt->get_increment_expr();
     SgExpression *increment_expr = for_stmt->get_increment();
     ROSE_ASSERT(increment_expr != NULL);
     if ( increment_expr != NULL )
          unparseExpression(increment_expr, info);
     curprint ( string(") "));

  // Added support to output the header without the body to support the addition 
  // of more context in the prefix used with the AST Rewrite Mechanism.
  // if ( (tmp_stmt = for_stmt->get_loop_body()) )

     SgStatement* loopBody = for_stmt->get_loop_body();
     ROSE_ASSERT(loopBody != NULL);
  // printf ("loopBody = %p         = %s \n",loopBody,loopBody->class_name().c_str());
  // printf ("info.SkipBasicBlock() = %s \n",info.SkipBasicBlock() ? "true" : "false");

  // if ( (tmp_stmt = for_stmt->get_loop_body()) && !info.SkipBasicBlock())
     if ( (loopBody != NULL) && !info.SkipBasicBlock())
        {
       // printf ("Unparse the for loop body \n");
       // curprint ( string("\n/* Unparse the for loop body */ \n";
       // unparseStatement(tmp_stmt, info);

          unp->cur.format(loopBody, info, FORMAT_BEFORE_NESTED_STATEMENT);
          unparseStatement(loopBody, info);
          unp->cur.format(loopBody, info, FORMAT_AFTER_NESTED_STATEMENT);
       // curprint ( string("\n/* DONE: Unparse the for loop body */ \n";
        }
       else
        {
       // printf ("No for loop body to unparse! \n");
       // curprint ( string("\n/* No for loop body to unparse! */ \n";
          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
        }
   }


void
Unparse_ExprStmt::unparseExceptionSpecification(const SgTypePtrList& exceptionSpecifierList, SgUnparse_Info& info)
   {
  // DQ (6/27/2006): Added support for throw modifier and its exception specification lists

#if 0
     printf ("Inside of unparseExceptionSpecification() \n");
#endif

     curprint ( string(" throw("));
     if (!exceptionSpecifierList.empty())
        {
          SgTypePtrList::const_iterator i = exceptionSpecifierList.begin();
          while (i != exceptionSpecifierList.end())
             {
            // Handle class type as a special case to make sure the names are always output (see test2004_91.C).
            // unparseType(*i,info);
            // printf ("Note: Type found in function throw specifier type = %p = %s \n",*i,i->class_name().c_str());

               ROSE_ASSERT(*i != NULL);
#if 0
               SgClassType* classType = isSgClassType(*i);
               if (classType != NULL)
                  {
                    SgDeclarationStatement* declaration = classType->get_declaration();
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                    ROSE_ASSERT(classDeclaration != NULL);
                 // printf ("     exception specification parameter: classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
                    ROSE_ASSERT(classDeclaration->get_name().is_null() == false);
                    curprint ( classDeclaration->get_name().str());
                  }
                 else
                  {
                 // Handle other types using the normal unparsing
                    unparseType(*i,info);
                  }
#else
               unp->u_type->unparseType(*i,info);
#endif
               i++;
               if (i != exceptionSpecifierList.end())
                  curprint ( string(","));
             }
        }
       else
        {
       // There was no exception specification list of types
        }
     curprint ( string(")"));
   }


// DQ (11/7/2007): Make this a more general function so that we can use it for the unparsing of SgClassDeclaration objects too.
// void fixupScopeInUnparseInfo ( SgUnparse_Info& ninfo , SgFunctionDeclaration* functionDeclaration )
void
fixupScopeInUnparseInfo ( SgUnparse_Info& ninfo , SgDeclarationStatement* declarationStatement )
   {
  // DQ (11/3/2007): This resets the current scope stored in the SgUnparse_Info object so that the name qualification will work properly.
  // It used to be that this would be the scope of the caller (which for unparsing the function prototype would be the outer scope (OK), 
  // but for function definitions would be the scope of the function definition (VERY BAD).  Because of a previous bug (just fixed) in the 
  // SgStatement::get_scope() function, the scope of the SgFunctionDefinition would be set to the parent of the SgFunctionDeclaration 
  // (structural) instead of the scope of the SgFunctionDeclaration (semantics). It is the perfect example of two bugs working together
  // to be almost always correct :-).  Note that "ninfo" will go out of scope, so we don't have to reset it at the end of this function.
  // Note that that it is FROM this scope that the name qualification is computed, so this is structural, not semantic.

  // DQ (11/9/2007): If we want to force the use of qualified names then don't reset the internal scope (required for new ROSE Doxygen 
  // documentation generator).
     if (ninfo.forceQualifiedNames() == false)
        {
          SgScopeStatement* currentScope = isSgScopeStatement(declarationStatement->get_parent());

          if (currentScope == NULL)
             {
            // printf ("In fixupScopeInUnparseInfo(): declarationStatement = %p = %s = %s \n",declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str());
               SgNode* parentOfFunctionDeclaration = declarationStatement->get_parent();
               ROSE_ASSERT(parentOfFunctionDeclaration != NULL);

               switch (parentOfFunctionDeclaration->variantT())
                  {
                 // This is one way that the funcdecl_stmt->get_parent() can not be a SgScopeStatement (there might be a few more!)
                    case V_SgTemplateInstantiationDirectiveStatement:
                       {
                         SgTemplateInstantiationDirectiveStatement* directive = isSgTemplateInstantiationDirectiveStatement(parentOfFunctionDeclaration);
                      // currentScope = isSgScopeStatement(funcdecl_stmt->get_parent()->get_parent());
                         currentScope = directive->get_scope();
                         break;
                       }

                    case V_SgVariableDeclaration:
                       {
                         SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(parentOfFunctionDeclaration);
                      // currentScope = isSgScopeStatement(funcdecl_stmt->get_parent()->get_parent());
                         currentScope = variableDeclaration->get_scope();
                         break;
                       }

                    default:
                       {
                         printf ("Error: default reached in evaluation of function declaration structural location parentOfFunctionDeclaration = %s \n",parentOfFunctionDeclaration->class_name().c_str());
                         printf ("     declarationStatement = %p = %s = %s \n",declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str());
                         declarationStatement->get_startOfConstruct()->display("default reached: debug");
                         ROSE_ASSERT(false);
                       }
                  }
             }

       // printf ("In fixupScopeInUnparseInfo(): currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
          ROSE_ASSERT(currentScope != NULL);

          ninfo.set_current_scope(currentScope);
        }

  // printf ("Set current scope (stored in ninfo): currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
   }
 
 
void
Unparse_ExprStmt::unparseFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseFuncDeclStmt() \n");
  // curprint ( string("/* Inside of Unparse_ExprStmt::unparseFuncDeclStmt */";
     curprint ( string("\n/* Inside of Unparse_ExprStmt::unparseFuncDeclStmt (" ) + StringUtility::numberToString(stmt) 
         + "): sage_class_name() = " + stmt->sage_class_name() + " */ \n");

     stmt->get_startOfConstruct()->display("Inside of unparseFuncDeclStmt()");

  // info.display("Inside of unparseFuncDeclStmt()");
#endif

  // printf ("In unparseFuncDeclStmt(): info.get_current_scope() = %p = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str());

//    if (stmt->get_startOfConstruct()->isOutputInCodeGeneration()==false)
//      return;

     SgFunctionDeclaration* funcdecl_stmt = isSgFunctionDeclaration(stmt);
     ROSE_ASSERT(funcdecl_stmt != NULL);

  // Liao, 9/25/2009, skip the compiler generated forward declaration for a SgTemplateInstantiationFunctionDecl
  // see bug 369: https://outreach.scidac.gov/tracker/index.php?func=detail&aid=369&group_id=24&atid=185
    if (funcdecl_stmt->isForward()) 
    {
      SgFunctionDeclaration* def_decl = isSgFunctionDeclaration(funcdecl_stmt->get_definingDeclaration());
      if (def_decl)
      {
        if (isSgTemplateInstantiationFunctionDecl(def_decl))
        {
         // cout<<"Skipping a forward declaration of a template instantiation function declaration..."<<endl;
          return;
        }
      }
    }

#if 0
     printf ("funcdecl_stmt = %p = %s \n",funcdecl_stmt,funcdecl_stmt->get_name().str());
     funcdecl_stmt->get_startOfConstruct()->display("Inside of unparseFuncDeclStmt()");
     if (funcdecl_stmt->get_definingDeclaration() != NULL)
          funcdecl_stmt->get_definingDeclaration()->get_startOfConstruct()->display("Inside of unparseFuncDeclStmt(): definingDeclaration");
     if (funcdecl_stmt->get_firstNondefiningDeclaration() != NULL)
          funcdecl_stmt->get_firstNondefiningDeclaration()->get_startOfConstruct()->display("Inside of unparseFuncDeclStmt(): firstNondefiningDeclaration");
#endif

#if OUTPUT_DEBUGGING_FUNCTION_NAME
  // Avoid output for both definition and declaration (twice) which unparsing the defining declaration.
     if (info.SkipFunctionDefinition() == false)
        {
          printf ("Inside of unparseFuncDeclStmt() name = %s  isTransformed() = %s fileInfo->isTransformed() = %s definition = %p isForward() = %s \n",
               funcdecl_stmt->get_qualified_name().str(),
               isTransformed (funcdecl_stmt) ? "true" : "false",
               funcdecl_stmt->get_file_info()->isTransformation() ? "true" : "false",
               funcdecl_stmt->get_definition(),
               funcdecl_stmt->isForward() ? "true" : "false");
        }
#endif

#if 0
  // printf ("Inside of Unparse_ExprStmt::unparseFuncDeclStmt(): funcdecl_stmt->get_from_template() = %s \n",
  //      funcdecl_stmt->get_from_template() ? "true" : "false");
  // if (funcdecl_stmt->get_from_template() == true)
  //      curprint ( string("/* Unparser comment: Templated Function */";
  // curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_from_template() = " + 
  //        funcdecl_stmt->get_from_template() + " */";
     curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_definition() = " ) + 
            StringUtility::numberToString(funcdecl_stmt->get_definition()) + " */");
#if 0
     curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_definition_ref() = " ) + 
            StringUtility::numberToString(funcdecl_stmt->get_definition_ref()) + " */");
     curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_forwardDefinition() = " ) + 
            StringUtility::numberToString(funcdecl_stmt->get_forwardDefinition()) + " */");
#endif
     curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_definingDeclaration() = " ) + 
            StringUtility::numberToString(funcdecl_stmt->get_definingDeclaration()) + " */");
     curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_firstNondefiningDeclaration() = " ) + 
            StringUtility::numberToString(funcdecl_stmt->get_firstNondefiningDeclaration()) + " */");
     curprint ( string("\n/* */");
#endif

  /* EXCEPTION HANDLING: Forward Declarations */
  // DO NOT use the file information sage gives us because the line information
  // refers to the function definition (if it is defined in the same file). Instead, 
  // the line of the forward declaration is set one after the line number of the 
  // previous node or directive. By doing this, any comments around the forward 
  // declaration will come after the declaration, since I'm setting the line number 
  // to be right after the previous thing we unparsed.

  // the following is a HACK. I want to know if this statement is in a header or
  // C++ file. If it is in a header file, then I proceed as normal because header 
  // files provide correct information. If the statement is in a C++ file and is a
  // forward declaration, then I must follow this HACK.
  /* EXCEPTION HANDLING: Forward Declarations */

     SgUnparse_Info ninfo(info);

     fixupScopeInUnparseInfo (ninfo,funcdecl_stmt);
    

  // DQ (10/10/2006): Do output any qualified names (particularly for non-defining declarations).
  // ninfo.set_forceQualifiedNames();

  // if (!funcdecl_stmt->isForward() && funcdecl_stmt->get_definition() && !info.SkipFunctionDefinition())
     if ( (funcdecl_stmt->isForward() == false) && (funcdecl_stmt->get_definition() != NULL) && (info.SkipFunctionDefinition() == false) )
        {
#if 0
       // printf ("Not a forward function (normal function) \n");
          curprint ( string("\n/* Not a forward function (normal function) */ \n");
#endif

       // DQ (12/5/2007): This call to unparse the definition can change the scope in info, so save it and restore it
       // SgScopeStatement* savedScope = info.get_current_scope();

       // DQ (12/3/2007): We want the changes to the access state to be saved in
       // the info object. See test2007_172.C for example of why this as a problem,
       // though it is not clear that a private friend is any different than a 
       // public friend.
       // unparseStatement(funcdecl_stmt->get_definition(), ninfo);
          unparseStatement(funcdecl_stmt->get_definition(), info);

       // printf ("After call to unparseStatement(funcdecl_stmt->get_definition(), info): info.get_current_scope() = %p = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str());
       // info.set_current_scope(savedScope);
       // printf ("After restoring the saved scope: info.get_current_scope() = %p = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str());

          if (funcdecl_stmt->isExternBrace())
             {
               curprint ( string(" }"));
             }
        }
       else
        {
#if 0
          printf ("Forward function (function prototype) \n");
          curprint ( string("\n/* Forward function (function prototype) */ \n"));
#endif
          SgClassDefinition *cdefn = isSgClassDefinition(funcdecl_stmt->get_parent());

          if (cdefn && cdefn->get_declaration()->get_class_type()==SgClassDeclaration::e_class)
               ninfo.set_CheckAccess();

       // printf ("Comment out call to get_suppress_atomic(funcdecl_stmt) \n");
#if 0
          if (get_suppress_atomic(funcdecl_stmt))
               ninfo.set_SkipAtomic();   // attributes.h
#endif

       // DQ (10/17/2004): Skip output of class definition for function declaration! C++ standard does not permit 
       // a defining declaration within a return type, function parameter, or sizeof expression. And by extention 
       // any function declaration!
          ninfo.set_SkipClassDefinition();
          ninfo.set_SkipEnumDefinition();

       // DQ (3/4/2010): Added support for asm functions (see test2010_12.C).
          SgStorageModifier & storage = funcdecl_stmt->get_declarationModifier().get_storageModifier();
       // printf ("storage.isAsm() = %s \n",storage.isAsm() ? "true" : "false");
          if (storage.isAsm() == true)
             {
               curprint( "asm ");
             }

          unp->u_sage->printSpecifier(funcdecl_stmt, ninfo);

          ninfo.unset_CheckAccess();

       // DQ (11/10/2007): Modified from info.set_access_attribute(...) --> ninfo.set_access_attribute(...)
          info.set_access_attribute(ninfo.get_access_attribute());

          SgType *rtype = funcdecl_stmt->get_orig_return_type();
          if (!rtype)
               rtype = funcdecl_stmt->get_type()->get_return_type();
          ninfo.set_isTypeFirstPart();

          SgUnparse_Info ninfo_for_type(ninfo);

#if 1
       // DQ (12/20/2006): This is used to specify global qualification separately from the more general name 
       // qualification mechanism.  Note that SgVariableDeclarations don't use the requiresGlobalNameQualificationOnType
       // on the SgInitializedNames in their list since the SgVariableDeclaration IR nodes is marked directly.
       // printf ("funcdecl_stmt->get_requiresNameQualificationOnReturnType() = %s \n",funcdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false");
       // curprint ( string("\n/* funcdecl_stmt->get_requiresNameQualificationOnReturnType() = " + (funcdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false") + " */ \n";
          if (funcdecl_stmt->get_requiresNameQualificationOnReturnType() == true)
             {
            // Output the name qualification for the type in the variable declaration.
            // But we have to do so after any modifiers are output, so in unp->u_type->unparseType().
            // printf ("In Unparse_ExprStmt::unparseFunctionDeclaration(): This return type requires a global qualifier \n");

            // Note that general qualification of types is separated from the use of globl qualification.
            // ninfo2.set_forceQualifiedNames();
               ninfo_for_type.set_requiresGlobalNameQualification();
             }
#endif

       // output the return type
#define OUTPUT_FUNCTION_DECLARATION_DATA 0
#if OUTPUT_FUNCTION_DECLARATION_DATA
          curprint ( string("\n/* output the return type */ \n"));
#endif

       // unp->u_type->unparseType(rtype, ninfo);
          unp->u_type->unparseType(rtype, ninfo_for_type);

       // output the rest of the function declaration
#if OUTPUT_FUNCTION_DECLARATION_DATA
          curprint ( string("/* calling unparse_helper */"));
#endif

       // DQ (10/15/2006): Mark that we are unparsing a function declaration (or member function declaration)
       // this will help us know when to trim the "::" prefix from the name qualiciation.  The "::" global scope
       // qualifier is not used in function declarations, but is used for function calls.
          ninfo.set_declstatement_ptr(NULL);
          ninfo.set_declstatement_ptr(funcdecl_stmt);

       // DQ (2/22/2009): Added assertion.
          if (funcdecl_stmt->get_firstNondefiningDeclaration() != NULL)
             {
               SgFunctionDeclaration* firstNondefiningFunction = isSgFunctionDeclaration(funcdecl_stmt->get_firstNondefiningDeclaration());
               ROSE_ASSERT(firstNondefiningFunction != NULL);
               ROSE_ASSERT(firstNondefiningFunction->get_firstNondefiningDeclaration() != NULL);
#if 0
               printf ("firstNondefiningFunction                                    = %p \n",firstNondefiningFunction);
               printf ("firstNondefiningFunction->get_firstNondefiningDeclaration() = %p \n",firstNondefiningFunction->get_firstNondefiningDeclaration());
               printf ("firstNondefiningFunction->get_definingDeclaration()         = %p \n",firstNondefiningFunction->get_definingDeclaration());
               printf ("firstNondefiningFunction->get_scope()                       = %p \n",firstNondefiningFunction->get_scope());
#endif
#if 0
               SgSourceFile* sourceFile = TransformationSupport::getSourceFile(funcdecl_stmt);
               printf ("sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());
#endif
               if (firstNondefiningFunction != NULL)
                  {
                    ROSE_ASSERT(TransformationSupport::getSourceFile(funcdecl_stmt) != NULL);
                    ROSE_ASSERT(TransformationSupport::getSourceFile(firstNondefiningFunction) != NULL);
                    if (TransformationSupport::getSourceFile(funcdecl_stmt) != TransformationSupport::getSourceFile(firstNondefiningFunction))
                       {
                         printf ("firstNondefiningFunction = %p \n",firstNondefiningFunction);
                         printf ("funcdecl_stmt = %p funcdecl_stmt->get_scope()                                        = %p \n",funcdecl_stmt,funcdecl_stmt->get_scope());
                         printf ("funcdecl_stmt = %p funcdecl_stmt->get_declarationModifier().isFriend()               = %s \n",funcdecl_stmt,funcdecl_stmt->get_declarationModifier().isFriend() ? "true" : "false");
                         printf ("firstNondefiningFunction = %p firstNondefiningFunction->get_declarationModifier().isFriend() = %s \n",firstNondefiningFunction,firstNondefiningFunction->get_declarationModifier().isFriend() ? "true" : "false");
                         printf ("TransformationSupport::getSourceFile(funcdecl_stmt = %p)->getFileName()              = %s \n",funcdecl_stmt,TransformationSupport::getSourceFile(funcdecl_stmt)->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(funcdecl_stmt->get_scope() = %p)->getFileName() = %s \n",funcdecl_stmt->get_scope(),TransformationSupport::getSourceFile(funcdecl_stmt->get_scope())->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(firstNondefiningFunction = %p)->getFileName()   = %s \n",firstNondefiningFunction,TransformationSupport::getSourceFile(firstNondefiningFunction)->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(firstNondefiningFunction->get_scope() = %p)->getFileName() = %s \n",firstNondefiningFunction->get_scope(),TransformationSupport::getSourceFile(firstNondefiningFunction->get_scope())->getFileName().c_str());

                         printf ("TransformationSupport::getSourceFile(funcdecl_stmt = %p)->getFileName()            = %s \n",funcdecl_stmt,TransformationSupport::getSourceFile(funcdecl_stmt)->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(firstNondefiningFunction = %p)->getFileName() = %s \n",firstNondefiningFunction,TransformationSupport::getSourceFile(firstNondefiningFunction)->getFileName().c_str());
                       }
                    ROSE_ASSERT(TransformationSupport::getSourceFile(funcdecl_stmt) == TransformationSupport::getSourceFile(firstNondefiningFunction));

                 // DQ (2/26/2009): Commented out because moreTest3.cpp fails for outlining to a separate file.
                    if (TransformationSupport::getSourceFile(funcdecl_stmt->get_scope()) != TransformationSupport::getSourceFile(firstNondefiningFunction))
                       {
                         printf ("firstNondefiningFunction = %p \n",firstNondefiningFunction);
                         printf ("funcdecl_stmt = %p funcdecl_stmt->get_scope()                                        = %p \n",funcdecl_stmt,funcdecl_stmt->get_scope());
                         printf ("funcdecl_stmt = %p funcdecl_stmt->get_declarationModifier().isFriend()               = %s \n",funcdecl_stmt,funcdecl_stmt->get_declarationModifier().isFriend() ? "true" : "false");
                         printf ("firstNondefiningFunction = %p firstNondefiningFunction->get_declarationModifier().isFriend() = %s \n",firstNondefiningFunction,firstNondefiningFunction->get_declarationModifier().isFriend() ? "true" : "false");
                         printf ("TransformationSupport::getSourceFile(funcdecl_stmt = %p)->getFileName()              = %s \n",funcdecl_stmt,TransformationSupport::getSourceFile(funcdecl_stmt)->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(funcdecl_stmt->get_scope() = %p)->getFileName() = %s \n",funcdecl_stmt->get_scope(),TransformationSupport::getSourceFile(funcdecl_stmt->get_scope())->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(firstNondefiningFunction = %p)->getFileName()   = %s \n",firstNondefiningFunction,TransformationSupport::getSourceFile(firstNondefiningFunction)->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(firstNondefiningFunction->get_scope() = %p)->getFileName() = %s \n",firstNondefiningFunction->get_scope(),TransformationSupport::getSourceFile(firstNondefiningFunction->get_scope())->getFileName().c_str());
                       }
                    ROSE_ASSERT(TransformationSupport::getSourceFile(funcdecl_stmt->get_scope()) == TransformationSupport::getSourceFile(firstNondefiningFunction));
                  }
#if 0
               printf ("Unparser: firstNondefiningFunction = %p \n",firstNondefiningFunction);
#endif

            // DQ (3/4/2009): This test appear to only fail for tutorial/rose_inputCode_InstrumentationTranslator.C 
               if (firstNondefiningFunction->get_symbol_from_symbol_table() == NULL)
                  {
                    printf ("Warning failing test: firstNondefiningFunction->get_symbol_from_symbol_table() != NULL, apepars to happen for tutorial/rose_inputCode_InstrumentationTranslator.C \n");
                  }
            // ROSE_ASSERT(firstNondefiningFunction->get_symbol_from_symbol_table() != NULL);
             }

#if 0
          printf ("Unparser: funcdecl_stmt = %p \n",funcdecl_stmt);
#endif

#if 0
       // DQ (3/4/2009): Comment out as a test!

       // DQ (2/24/2009): If this is the defining declaration and when there is no non-defining 
       // declaration the symbol references the defining declaration.
          if (funcdecl_stmt->get_firstNondefiningDeclaration() == NULL)
             {
               if (funcdecl_stmt->get_definingDeclaration() != funcdecl_stmt)
                  {
                    printf ("funcdecl_stmt = %p = %s = %s \n",funcdecl_stmt,funcdecl_stmt->class_name().c_str(),SageInterface::get_name(funcdecl_stmt).c_str());
                    printf ("funcdecl_stmt = %p != funcdecl_stmt->get_definingDeclaration() = %p \n",funcdecl_stmt,funcdecl_stmt->get_definingDeclaration());
                    printf ("funcdecl_stmt = %p != funcdecl_stmt->get_scope() = %p = %s \n",funcdecl_stmt,funcdecl_stmt->get_scope(),funcdecl_stmt->get_scope()->class_name().c_str());
                    printf ("Error: funcdecl_stmt = %p != funcdecl_stmt->get_definingDeclaration() = %p \n",funcdecl_stmt,funcdecl_stmt->get_definingDeclaration());
                  }
            // DQ (3/4/2009): This fails for test2006_78.C because the only non-defining declaration 
            // is a declaration in the scope of a function which does not provide enough information 
            // to associate the scope.
            // ROSE_ASSERT (funcdecl_stmt->get_definingDeclaration() == funcdecl_stmt);
            // ROSE_ASSERT (funcdecl_stmt->get_definingDeclaration() == funcdecl_stmt || isSgBasicBlock(funcdecl_stmt->get_scope()) != NULL);
               ROSE_ASSERT (funcdecl_stmt->get_definingDeclaration() == funcdecl_stmt || isSgBasicBlock(funcdecl_stmt->get_parent()) != NULL);
#if 0
               printf ("TransformationSupport::getSourceFile(funcdecl_stmt = %p)->getFileName() = %s \n",funcdecl_stmt,TransformationSupport::getSourceFile(funcdecl_stmt)->getFileName().c_str());
#endif
               ROSE_ASSERT (funcdecl_stmt->get_symbol_from_symbol_table() != NULL);
             }
            else
             {
               ROSE_ASSERT (funcdecl_stmt->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table() != NULL);
             }
#endif

          unparse_helper(funcdecl_stmt, ninfo);

       // DQ (10/15/2006): Matching call to unset the stored declaration.
          ninfo.set_declstatement_ptr(NULL);

       // curprint ( string("/* DONE: calling unparse_helper */";

          ninfo.set_isTypeSecondPart();

#if OUTPUT_FUNCTION_DECLARATION_DATA
          curprint ( string("/* output the second part of the type */"));
#endif

          unp->u_type->unparseType(rtype, ninfo);

#if OUTPUT_FUNCTION_DECLARATION_DATA
          curprint ( string("/* DONE: output the second part of the type */"));
#endif

       // DQ (4/28/2004): Added support for throw modifier
          if (funcdecl_stmt->get_declarationModifier().isThrow())
             {
            // printf ("Output throw modifier (incomplete implementation) \n");
            // curprint ( string(" throw( /* from unparseFuncDeclStmt() type list output not implemented */ )";
               const SgTypePtrList& exceptionSpecifierList = funcdecl_stmt->get_exceptionSpecification();
               unparseExceptionSpecification(exceptionSpecifierList,info);
             }

       // DQ (1/25/2009): Function can be defined using asm function names. The name is held as a string.
          if (funcdecl_stmt->get_asm_name().empty() == false)
             {
            // an asm ("<function name>") is in use
               curprint ( string(" asm (\""));
               curprint ( funcdecl_stmt->get_asm_name() );
               curprint ( string("\")"));
             }

          if (funcdecl_stmt->isForward() && !ninfo.SkipSemiColon())
             {
               curprint ( string(";"));
               if (funcdecl_stmt->isExternBrace())
                  {
                    curprint ( string(" }"));
                  }
             }
        }

#if 0
  // DQ (4/28/2004): Added support for throw modifier
     if (info.SkipClassDefinition() && funcdecl_stmt->get_declarationModifier().isThrow())
        {
          curprint ( string(" throw()"));
        }
#endif

  // DQ (1/23/03) Added option to support rewrite mechanism (generation of declarations)
     if (info.AddSemiColonAfterDeclaration())
        {
          curprint ( string(";"));
        }

#if 0
  // curprint ( string("/* End of Unparse_ExprStmt::unparseFuncDeclStmt */";
     curprint ( string("\n/* End of Unparse_ExprStmt::unparseFuncDeclStmt (" ) + StringUtility::numberToString(stmt) 
         + "): sage_class_name() = " + stmt->sage_class_name() + " */ \n");
     printf ("End of Unparse_ExprStmt::unparseFuncDeclStmt() \n");
#endif
   }


// NOTE: Bug in Sage: No file information provided for FuncDeclStmt. 
void
Unparse_ExprStmt::unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseFuncDefnStmt() \n");
     curprint ( string("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt */"));
#endif

     SgFunctionDefinition* funcdefn_stmt = isSgFunctionDefinition(stmt);
     ROSE_ASSERT(funcdefn_stmt != NULL);

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (funcdefn_stmt);
#endif

  // Unparse any comments of directives attached to the SgFunctionParameterList
     ROSE_ASSERT (funcdefn_stmt->get_declaration() != NULL);
     if (funcdefn_stmt->get_declaration()->get_parameterList() != NULL)
         unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::before);

     info.set_SkipFunctionDefinition();
     SgStatement *declstmt = funcdefn_stmt->get_declaration();

  // DQ (3/24/2004): Need to permit SgMemberFunctionDecl and SgTemplateInstantiationMemberFunctionDecl
  // if (declstmt->variant() == MFUNC_DECL_STMT)

  // DQ (5/8/2004): Any generated specialization needed to use the 
  // C++ syntax for explicit specification of specializations.
  // if (isSgTemplateInstantiationMemberFunctionDecl(declstmt) != NULL)
  //      curprint ( string("template<> ";

#if 0
     printf ("Inside of Unparse_ExprStmt::unparseFuncDefnStmt: calling unparseMFuncDeclStmt or unparseFuncDeclStmt \n");
     curprint ( string("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: calling unparseMFuncDeclStmt or unparseFuncDeclStmt */"));
#endif

  // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
  // info.set_SkipQualifiedNames();

  // DQ (10/15/2006): Mark that we are unparsing a function declaration (or member function declaration)
  // this will help us know when to trim the "::" prefix from the name qualiciation.  The "::" global scope
  // qualifier is not used in function declarations, but is used for function calls.
     info.set_declstatement_ptr(NULL);
     info.set_declstatement_ptr(funcdefn_stmt->get_declaration());

     if ( isSgMemberFunctionDeclaration(declstmt) != NULL )
        unparseMFuncDeclStmt( declstmt, info);
     else
        unparseFuncDeclStmt( declstmt, info);

  // DQ (10/15/2006): Also un-mark that we are unparsing a function declaration (or member function declaration)
     info.set_declstatement_ptr(NULL);

  // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
  // info.unset_SkipQualifiedNames();

#if 0
     printf ("Inside of Unparse_ExprStmt::unparseFuncDefnStmt: output the function body \n");
     curprint ( string("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: output the function body */"));
#endif

     info.unset_SkipFunctionDefinition();
     SgUnparse_Info ninfo(info);
  
  // now the body of the function
     if (funcdefn_stmt->get_body())
        {
          unparseStatement(funcdefn_stmt->get_body(), ninfo);
        }
       else
        {
          curprint ( string("{}"));

       // DQ (9/22/2004): I think this is an error!
          printf ("Error: Should be an error to not have a function body in the AST \n");
          ROSE_ASSERT(false);
        }

  // Unparse any comments of directives attached to the SgFunctionParameterList
     unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::after);
   }


string
Unparse_ExprStmt::trimGlobalScopeQualifier ( string qualifiedName )
   {
  //! DQ (10/12/2006): Support for qualified names (function names can't have global scope specifier in GNU, or so it seems).

  // DQ (10/11/2006): Now that we use fully qualified names in most places we need this 
  // editing to remove the leading global qualifier (once again!).

  // DQ (8/25/2005): This is the case where we previously named the global scope as "::"
  // within name qualification.  This was done to handle test2005_144.C but it broke
  // test2004_80.C So we have moved to an explicit marking of IR nodes using global scope
  // qualification (since it clearly seems to be required).
  // For member functions we need to remove the leading "::" since GNU g++ can't always handle it for member functions
     string s = qualifiedName;
     size_t subStringLocationOfScopeQualifier = s.find("::");
  // printf ("Location of member function substring = %d \n",subStringLocationOfScopeQualifier);
     if (subStringLocationOfScopeQualifier == 0)
        {
       // printf ("Found global scope qualifier at start of function or member function name qualification \n");
          s.replace(s.find("::"),2,"");

       // reset the string in scopename!
          qualifiedName = s.c_str();
        }

     return qualifiedName;
   }


void
Unparse_ExprStmt::unparseMFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  SgMemberFunctionDeclaration* mfuncdecl_stmt = isSgMemberFunctionDeclaration(stmt);
  ROSE_ASSERT(mfuncdecl_stmt != NULL);

#if 0
  curprint ( string("\n/* Inside of Unparse_ExprStmt::unparseMFuncDeclStmt */ \n") ); 
#endif

#if OUTPUT_DEBUGGING_FUNCTION_NAME
  printf ("Inside of unparseMFuncDeclStmt() name = %s  transformed = %s prototype = %s \n",
      mfuncdecl_stmt->get_qualified_name().str(),
      isTransformed (mfuncdecl_stmt) ? "true" : "false",
      (mfuncdecl_stmt->get_definition() == NULL) ? "true" : "false");
#endif

#if 0
  // printf ("Inside of Unparse_ExprStmt::unparseMFuncDeclStmt(): mfuncdecl_stmt->get_from_template() = %s \n",
  //      mfuncdecl_stmt->get_from_template() ? "true" : "false");
  // if (mfuncdecl_stmt->get_from_template() == true)
  //      curprint ( string("/* Unparser comment: Templated Member Function */";
  // curprint ( string("\n/* Unparser comment: mfuncdecl_stmt->get_from_template() = " + 
  //        mfuncdecl_stmt->get_from_template() + " */";
  curprint ( string("\n/* Unparser comment: mfuncdecl_stmt->get_definition() = " ) + 
      StringUtility::numberToString(mfuncdecl_stmt->get_definition()) + " */");
#if 0
  curprint ( string("\n/* Unparser comment: mfuncdecl_stmt->get_definition_ref() = " ) + 
      StringUtility::numberToString(mfuncdecl_stmt->get_definition_ref()) + " */");
  curprint ( string("\n/* Unparser comment: mfuncdecl_stmt->get_forwardDefinition() = " ) + 
      StringUtility::numberToString(mfuncdecl_stmt->get_forwardDefinition()) + " */");
#endif
  curprint ( string("\n/* Unparser comment: mfuncdecl_stmt->get_definingDeclaration() = " ) + 
      StringUtility::numberToString(mfuncdecl_stmt->get_definingDeclaration()) + " */");
  curprint ( string("\n/* Unparser comment: mfuncdecl_stmt->get_firstNondefiningDeclaration() = " ) + 
      StringUtility::numberToString(mfuncdecl_stmt->get_firstNondefiningDeclaration()) + " */");
  curprint ( string("\n/* */");
#endif

      // DQ (12/3/2007): This causes a bug in the output of access level (public, protected, private)
      // because the inforamtion change in ninfo is not propogated to info.
      // DQ (11/3/2007): Moved construction of ninfo to start of function!
      SgUnparse_Info ninfo(info);

      fixupScopeInUnparseInfo (ninfo,mfuncdecl_stmt);

      // Unparse any comments of directives attached to the SgCtorInitializerList
      if (mfuncdecl_stmt->get_CtorInitializerList() != NULL)
      {
      // unparseAttachedPreprocessingInfo(mfuncdecl_stmt->get_CtorInitializerList(), info, PreprocessingInfo::before);
      unparseAttachedPreprocessingInfo(mfuncdecl_stmt->get_CtorInitializerList(), ninfo, PreprocessingInfo::before);
      }

      // DQ (11/23/2004): Experiment with skipping the output of specialized template member functions!
      // this needs to be handled better in the future!
      if (isSgTemplateInstantiationMemberFunctionDecl(mfuncdecl_stmt) != NULL)
      {
#if PRINT_DEVELOPER_WARNINGS
        printf ("DQ (11/23/2004): Experiment with skipping the output of specialized template member functions! \n");
#endif

        SgStatement* parentStatement = isSgStatement(mfuncdecl_stmt->get_parent());
        ROSE_ASSERT(parentStatement != NULL);
        if ( isSgTemplateInstantiationDirectiveStatement(parentStatement) != NULL )
        {
          // output the member function declaration if it is part of an explicit instatiation directive
          // Note this code is confusiong becase what we are doing is NOT returning!
          // printf ("This template member function is part of an explicit template directive! \n");
          // curprint ( string("\n/* This template member function is part of an explicit template directive! */";
        }
        else
        {
#if 0
          if ( isTransformed (mfuncdecl_stmt) == false )
          {
            // DQ (3/2/2005): I think we are ready to output the member functions of template classes (or even templated member functions).
            // This should be made depedent upon the evaluation of bool Unparse_ExprStmt::isTransformed(SgStatement* stmt) so that it matches the 
            // output of specialized class declarations!  And the output of template functions should then be made consistant.
            curprint ( string("\n /* DQ (11/23/2004): Experiment with skipping the output of specialized template member functions! */ \n"));
            return;
          }
#else
          // curprint ( string("\n /* Output of template member functions turned on 3/2/2005 */ \n";
#endif
        }
      }

  // DQ (4/21/2005): This is a very old comment and I think its information 
  // is now incorrect.  Likely this comment could be removed!
  /* EXCEPTION HANDLING: Member Forward Declarations in *.C file */
  // DO NOT use the file information sage gives us because the line information
  // refers to the function definition (if it is defined in the same file). Instead, 
  // the line of the forward declaration is set one after the line number of the 
  // previous node or directive. By doing this, any comments around the forward 
  // declaration will come after the declaration, since I'm setting the line number 
  // to be right after the previous thing we unparsed.

  // the following is a HACK. I want to know if this statement is in a header or
  // C++ file. If it is in a header file, then I proceed as normal. If the
  // statement is in a C++ file and is a forward declaration, then I must 
  // follow this HACK. When the Sage bug of forward declaration is fixed, remove
  // this code.
  /* EXCEPTION HANDLING: Member Forward Declarations in *.C file */

#if 0
  printf ("mfuncdecl_stmt->isForward()      = %s \n",mfuncdecl_stmt->isForward() ? "true" : "false");
  printf ("mfuncdecl_stmt->get_definition() = %s \n",mfuncdecl_stmt->get_definition() ? "true" : "false");
  printf ("info.SkipFunctionDefinition()    = %s \n",info.SkipFunctionDefinition() ? "true" : "false");
#endif

  // if ( !mfuncdecl_stmt->isForward() && mfuncdecl_stmt->get_definition() && !info.SkipFunctionDefinition() )
  if ( !mfuncdecl_stmt->isForward() && mfuncdecl_stmt->get_definition() && !ninfo.SkipFunctionDefinition() )
  {
#if 0
    printf ("Unparsing special case of non-forward, valid definition and !skip function definition \n");
    // curprint ( string("\n/* Unparsing special case of non-forward, valid definition and !skip function definition */ \n"; 
#endif

    // DQ (12/3/2007): We want the changes to the access state to be saved in
    // the info object. See test2007_171.C for example of why this is critical.
    // unparseStatement(mfuncdecl_stmt->get_definition(), info);
    // unparseStatement(mfuncdecl_stmt->get_definition(), ninfo);
    unparseStatement(mfuncdecl_stmt->get_definition(), info);

    if (mfuncdecl_stmt->isExternBrace())
    {
      unp->cur.format(mfuncdecl_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
      curprint ( string(" }"));
      unp->cur.format(mfuncdecl_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
    }
  } 
  else 
  {
#if 0
    printf ("Normal case for unparsing member function declarations! \n");
    // curprint ( string("\n/* Normal case for unparsing member function declarations */ \n"; 
#endif
#if 0
    // DQ (5/8/2004): Any generated specialization needed to use the 
    // C++ syntax for explicit specification of specializations.
    SgTemplateInstantiationMemberFunctionDecl* templateMemberFunction = isSgTemplateInstantiationMemberFunctionDecl(mfuncdecl_stmt);
    if (templateMemberFunction != NULL && templateMemberFunction->get_templateArguments()->size() > 0)
    {
      curprint ( string("/* explicit template specialization */ "));
      curprint ( string("template<> "));
    }
#endif
    ROSE_ASSERT (mfuncdecl_stmt->get_parent() != NULL);

    // DQ (11/15/2004): It might be safer to always force the qualified function name to be output!
    // Better yet we should use the scope that is now explicitly stored in the Sage III AST!
#if PRINT_DEVELOPER_WARNINGS
    printf ("IMPLEMENTATION NOTE: This does not test if the parent class is the correct class in deeply nested class structures \n");
#endif
    SgClassDefinition *parent_class = isSgClassDefinition(mfuncdecl_stmt->get_parent());
    //        printf ("In unparseMFuncDeclStmt(): parent_class of member function declaration = %p = %s \n",
    //             mfuncdecl_stmt->get_parent(),mfuncdecl_stmt->get_parent()->sage_class_name());

    // printf ("Commented out call to get_suppress_atomic(mfuncdecl_stmt) \n");
#if 0
    if (get_suppress_atomic(mfuncdecl_stmt))
      info.set_SkipAtomic();
#endif

    // DQ and PC (6/1/2006): Added Peter's suggested fixes to support unparsing fully qualified names (supporting auto-documentation).
    // if (parent_class && parent_class->get_declaration()->get_class_type() == SgClassDeclaration::e_class) 
    if (parent_class && parent_class->get_declaration()->get_class_type() == SgClassDeclaration::e_class && !info.skipCheckAccess())
    {
      info.set_CheckAccess();
    }

    // printDebugInfo("entering unp->u_sage->printSpecifier1", true);
    unp->u_sage->printSpecifier1(mfuncdecl_stmt, info);

    // printDebugInfo("entering unp->u_sage->printSpecifier2", true);
    unp->u_sage->printSpecifier2(mfuncdecl_stmt, info);
    info.unset_CheckAccess();

    // DQ (11/3/2007): Moved construction of ninfo to start of function!
    // SgUnparse_Info ninfo(info);
    SgType *rtype = NULL;

    // DQ (10/10/2006): Do output any qualified names (particularly for non-defining declarations).
    // ninfo.set_forceQualifiedNames();
#if 0
    curprint ( string("/* force output of qualified names */\n ") );
#endif
    // DQ (10/17/2004): Added code to form skipping enum definitions.
    // DQ (10/7/2004): Skip output of class definition for return type! C++ standard does not permit 
    // a defining declaration within a return type, function parameter, or sizeof expression. 

#if 1
    // This is a test for if the member function is structurally in the class where it is defined.
    // printf ("parent_class = %p mfuncdecl_stmt->get_scope() = %p \n",parent_class,mfuncdecl_stmt->get_scope());

    // DQ (11/5/2007): This test is not good enough (does not handle case of nested classes and the definition
    // of member function outside of the nested class and inside of another class.
    // if (parent_class)
    if (parent_class == mfuncdecl_stmt->get_scope())
    {
      // JJW 10-23-2007 This member function is declared inside the
      // class, so its name should never be qualified

      // printf ("mfuncdecl_stmt->get_declarationModifier().isFriend() = %s \n",mfuncdecl_stmt->get_declarationModifier().isFriend() ? "true" : "false");
      if (mfuncdecl_stmt->get_declarationModifier().isFriend() == false)
      {
        // printf ("Setting SkipQualifiedNames (this is a member function located in its own class) \n");
        ninfo.set_SkipQualifiedNames();
      }
    }
#endif
    ninfo.set_SkipClassDefinition();
    ninfo.set_SkipEnumDefinition();

    // DQ (6/10/2007): set the declaration pointer so that the name qualification can see if this is 
    // the declaration (so that exceptions to qualification can be tracked).
    ninfo.set_declstatement_ptr(NULL);
    ninfo.set_declstatement_ptr(mfuncdecl_stmt);

    // if (!(mfuncdecl_stmt->isConstructor() || mfuncdecl_stmt->isDestructor() || mfuncdecl_stmt->isConversion()))
    if ( !( mfuncdecl_stmt->get_specialFunctionModifier().isConstructor() || 
          mfuncdecl_stmt->get_specialFunctionModifier().isDestructor()  ||
          mfuncdecl_stmt->get_specialFunctionModifier().isConversion() ) )
    {
      // printf ("In unparser: NOT a constructor, destructor or conversion operator \n");

      // printf ("mfuncdecl_stmt->get_orig_return_type() = %p \n",mfuncdecl_stmt->get_orig_return_type());

      if (mfuncdecl_stmt->get_orig_return_type() != NULL)
      {
        // printf ("mfuncdecl_stmt->get_orig_return_type() = %p = %s \n",mfuncdecl_stmt->get_orig_return_type(),mfuncdecl_stmt->get_orig_return_type()->sage_class_name());
        rtype = mfuncdecl_stmt->get_orig_return_type();
      }
      else
      {
        printf ("In unparseMFuncDeclStmt: (should not happen) mfuncdecl_stmt->get_type()->get_return_type() = %p = %s \n",
            mfuncdecl_stmt->get_type()->get_return_type(),mfuncdecl_stmt->get_type()->get_return_type()->sage_class_name());
        rtype = mfuncdecl_stmt->get_type()->get_return_type();
      }
      // printf ("rtype = %p = %s \n",rtype,rtype->sage_class_name());

      ninfo.set_isTypeFirstPart();
      ninfo.set_SkipClassSpecifier();

      SgUnparse_Info ninfo_for_type(ninfo);

      // DQ (6/10/2007): set the declaration pointer so that the name qualification can see if this is 
      // the declaration (so that exceptions to qualification can be tracked).
      ROSE_ASSERT(ninfo_for_type.get_declstatement_ptr() != NULL);

#if 1
      // DQ (12/20/2006): This is used to specify global qualification separately from the more general name 
      // qualification mechanism.  Note that SgVariableDeclarations don't use the requiresGlobalNameQualificationOnType
      // on the SgInitializedNames in their list since the SgVariableDeclaration IR nodes is marked directly.
      // printf ("mfuncdecl_stmt->get_requiresNameQualificationOnReturnType() = %s \n",mfuncdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false");
      // curprint ( string("\n/* funcdecl_stmt->get_requiresNameQualificationOnReturnType() = " + (mfuncdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false") + " */ \n";
      if (mfuncdecl_stmt->get_requiresNameQualificationOnReturnType() == true)
      {
        // Output the name qualification for the type in the variable declaration.
        // But we have to do so after any modifiers are output, so in unp->u_type->unparseType().
        // printf ("In Unparse_ExprStmt::unparseMemberFunctionDeclaration(): This return type requires a global qualifier \n");

        // Note that general qualification of types is separated from the use of globl qualification.
        // ninfo2.set_forceQualifiedNames();
        ninfo_for_type.set_requiresGlobalNameQualification();
      }
#endif

      // unp->u_type->unparseType(rtype, ninfo);
      unp->u_type->unparseType(rtype, ninfo_for_type);

      ninfo.unset_SkipClassSpecifier();
      // printf ("In unparser: DONE with NOT a constructor, destructor or conversion operator \n");
    }
    else
    {
      // DQ (9/17/2004): What can we assume about the return type of a constructor, destructor, or conversion operator?
      if (mfuncdecl_stmt->get_orig_return_type() == NULL)
      {
        printf ("mfuncdecl_stmt->get_orig_return_type() == NULL mfuncdecl_stmt = %p = %s = %s \n",
            mfuncdecl_stmt,mfuncdecl_stmt->class_name().c_str(),mfuncdecl_stmt->get_name().str());
      }
      ROSE_ASSERT(mfuncdecl_stmt->get_orig_return_type() != NULL);
      ROSE_ASSERT(mfuncdecl_stmt->get_type()->get_return_type() != NULL);
    }

    ROSE_ASSERT (mfuncdecl_stmt != NULL);

    // printf ("In unparser: parent_class = %p \n",parent_class);
    // printf ("In unparser: mfuncdecl_stmt->get_name() = %s \n",mfuncdecl_stmt->get_name().str());

    // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
    // ninfo.set_SkipQualifiedNames();

    // ninfo.display("unparse SgMemberFunction: ninfo");
#if 0
#error "DEAD CODE!"
    // DQ (10/11/2006): Switch this back for now while I debug the use of a new qualified name mechanism.
    // DQ and PC (6/1/2006): Added Peter's suggested fixes to support unparsing fully qualified names (supporting auto-documentation).
    // if (parent_class != NULL)
    // if (parent_class != NULL && !info.forceQualifiedNames())
    // if (parent_class != NULL)
    // if (parent_class == mfuncdecl_stmt->get_scope() && info.forceQualifiedNames())
#error "DEAD CODE!"
    if (ninfo.forceQualifiedNames())
    {
      // In a class structure (class definition) don't output the 
      // class name qualification before each function name.

      // curprint ( string("/* In a class structure (class definition) don't output the class name qualification */\n ";

      // DQ (8/1/2007): Added code to force output of qualified names (if specificed by unparseToString()).
      SgName scopename = mfuncdecl_stmt->get_class_scope()->get_declaration()->get_qualified_name();
      scopename = trimGlobalScopeQualifier ( scopename.str() ).c_str();
      if (scopename.is_null() == false)
      {
        curprint(scopename.str());
        curprint("::");
      }

#error "DEAD CODE!"
      curprint (  mfuncdecl_stmt->get_name().str() );
    }
    else
    {
      // curprint ( string("/* at global scope, for example, output the qualified name <class name>::f() */\n ";

      // at global scope, for example, output the qualified name s::f()
      SgName scopename;
#error "DEAD CODE!"
      printf ("mfuncdecl_stmt->get_isModified() = %s \n",(mfuncdecl_stmt->get_isModified() == true) ? "true" : "false");

      // DQ (12/5/2004): Check if this is a modified member function IR node which does not have it's scope set properly
      // Allows unparser to output a name with a comment about it being incorrect!
      SgScopeStatement *classScope = mfuncdecl_stmt->get_scope();
      SgClassDefinition* classDefinition = isSgClassDefinition(classScope);
      if ((classDefinition == NULL) && (mfuncdecl_stmt->get_isModified() == true))
      {
        // DQ (12/5/2004): Modified nodes (from the rewrite system at least) don't have the correct scope 
        // information required to generate the qualifier!
        // output an unqualified name (which is quite incorrect but sufficient while we fix the originating problem).
        // scopename = string(" /* modified IR node is missing qualifier */ ") + mfuncdecl_stmt->get_name();
        // curprint ( string(" /* modified IR node is missing qualifier */ ";
      }
      else
      {
        // DQ (11/17/2004): Interface modified, use get_class_scope() if we want a
        // SgClassDefinition, else use get_scope() if we want a SgScopeStatement.
        ROSE_ASSERT (mfuncdecl_stmt->get_class_scope() != NULL);
        ROSE_ASSERT (mfuncdecl_stmt->get_class_scope()->get_declaration() != NULL);

#error "DEAD CODE!"
        scopename = mfuncdecl_stmt->get_class_scope()->get_declaration()->get_qualified_name();
#if 0
        printf ("In unparseMFuncDeclStmt(): mfuncdecl_stmt->get_scope() = %p = %s \n",
            mfuncdecl_stmt->get_scope(),mfuncdecl_stmt->get_scope()->class_name().c_str());
#endif
#if 0
        printf ("In unparseMFuncDeclStmt(): mfuncdecl_stmt->get_scope()->get_declaration() = %p = %s \n",
            mfuncdecl_stmt->get_scope()->get_declaration(),mfuncdecl_stmt->get_scope()->get_declaration()->class_name().c_str());
        printf ("In unparseMFuncDeclStmt(): mfuncdecl_stmt->get_scope()->get_declaration() = %p scopename = %s \n",
            mfuncdecl_stmt->get_scope()->get_declaration(),scopename.str());
#endif
        printf ("(before trimGlobalScopeQualifier) scopename = %s \n",scopename.str());
#error "DEAD CODE!"

        // DQ (10/12/2006): need to trim off the global scope specifier (I think).
        // curprint ( string("\n/* Calling trimGlobalScopeQualifier() */\n ";
        scopename = trimGlobalScopeQualifier ( scopename.str() ).c_str();

        printf ("(after trimGlobalScopeQualifier) scopename = %s \n",scopename.str());

#error "DEAD CODE!"
        curprint (  scopename.str());
        curprint (  "::");
      }

      // printf ("In unparser: mfuncdecl_stmt->get_name() = %s \n",mfuncdecl_stmt->get_name().str());
      curprint ( mfuncdecl_stmt->get_name().str());
    }
#else
    // Generate the qualified name
    SgName nameQualifier = unp->u_name->generateNameQualifier( mfuncdecl_stmt , ninfo );
    // printf ("nameQualifier for member function = %s \n",nameQualifier.str());
    curprint ( nameQualifier.str() );
    curprint ( mfuncdecl_stmt->get_name().str());
#endif

    SgUnparse_Info ninfo2(info);

    // DQ (10/10/2006): Do output any qualified names (particularly for non-defining declarations).
    // ninfo2.set_forceQualifiedNames();

    // DQ (10/17/2004): Skip output of enum and class definitions for return type! C++ standard does not permit 
    // a defining declaration within a return type, function parameter, or sizeof expression. 
    ninfo2.set_SkipClassDefinition();
    ninfo2.set_SkipEnumDefinition();

    // DQ (6/10/2007): set the declaration pointer so that the name qualification can see if this is 
    // the declaration (so that exceptions to qualification can be tracked).
    ninfo2.set_declstatement_ptr(NULL);
    ninfo2.set_declstatement_ptr(mfuncdecl_stmt);

    ninfo2.set_inArgList();
    SgName tmp_name;

    // DQ (10/7/2004): Skip output of class definition for return type! C++ standard does not permit 
    // a defining declaration within a return type, function parameter, or sizeof expression. 
    ninfo2.set_SkipClassDefinition();

    curprint ( string("("));
    // Unparse the function arguments
    unparseFunctionArgs(mfuncdecl_stmt,ninfo2);
    curprint ( string(")"));

    if (rtype != NULL)
    {
      SgUnparse_Info ninfo3(ninfo);
      ninfo3.set_isTypeSecondPart();

      unp->u_type->unparseType(rtype, ninfo3);
    }

    SgMemberFunctionType *mftype = isSgMemberFunctionType(mfuncdecl_stmt->get_type());
    if (!info.SkipFunctionQualifier() && mftype )
    {
      if (mftype->isConstFunc())
      {
        curprint ( string(" const"));
      }
      if (mftype->isVolatileFunc())
      {
        curprint ( string(" volatile"));
      }
    }

    // DQ (4/28/2004): Added support for restrict modifier
    if (mfuncdecl_stmt->get_declarationModifier().get_typeModifier().isRestrict())
    {
      curprint ( string(" restrict"));
    }

    // DQ (4/28/2004): Added support for throw modifier
    if (mfuncdecl_stmt->get_declarationModifier().isThrow())
    {
      // Unparse SgThrow
      // unparseThrowExp(mfuncdecl_stmt->get_throwExpression,info);
      // printf ("Incomplete implementation of throw specifier on function \n");
      // curprint ( string(" throw( /* from unparseMFuncDeclStmt() type list output not implemented */ )";
      const SgTypePtrList& exceptionSpecifierList = mfuncdecl_stmt->get_exceptionSpecification();
      unparseExceptionSpecification(exceptionSpecifierList,ninfo);
    }

    // if (mfuncdecl_stmt->isPure())
    if (mfuncdecl_stmt->get_functionModifier().isPureVirtual())
    {
      curprint ( string(" = 0"));
    }

    if (mfuncdecl_stmt->isForward() && !info.SkipSemiColon())
    {
      curprint ( string(";"));
      if (mfuncdecl_stmt->isExternBrace())
      {
        curprint ( string(" }"));
      }
    }
    else
    {
      int first = 1;
      SgInitializedNamePtrList::iterator p = mfuncdecl_stmt->get_ctors().begin();

      while (p != mfuncdecl_stmt->get_ctors().end())
      {
        if (first)
        {
          curprint ( string(" : "));
          first = 0;
        }
        else
        {
          curprint ( string(", "));
        }
        ROSE_ASSERT ((*p) != NULL);
        curprint (  (*p)->get_name().str());

        // DQ (8/4/2005): Removed the use of "()" here since it breaks test2005_123.C
        // DQ (8/2/2005): Added "()" to constructor initialization list (better here than for all SgAssignInitializer's expressions)
        SgExpression* initializer = (*p)->get_initializer();
        if (initializer != NULL)
        {
          // DQ (8/6/2005): Remove "()" when SgConstructorInitializer is used in preinitialization list (since it will output a "()" already)
          // printf ("Output parenthesis for all but constructor initializers \n");
          bool outputParenthesis =  (isSgConstructorInitializer(initializer) == NULL);
          if (outputParenthesis == true)
          {
            curprint ( string("("));
          }

          // unparseExpression((*p)->get_initializer(), info);
          unparseExpression(initializer, info);

          if (outputParenthesis == true)
          {
            curprint ( string(")"));
          }
        }
        p++;
      }
    }
  }

  // DQ (1/23/03) Added option to support rewrite mechanism (generation of declarations)
  if (info.AddSemiColonAfterDeclaration())
  {
    curprint ( string(";"));
  }

  // Unparse any comments of directives attached to the SgCtorInitializerList
  if (mfuncdecl_stmt->get_CtorInitializerList() != NULL)
    unparseAttachedPreprocessingInfo(mfuncdecl_stmt->get_CtorInitializerList(), info, PreprocessingInfo::after);
}

void
Unparse_ExprStmt::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseVarDeclStmt(%p) \n",stmt);
     ROSE_ASSERT(info.get_current_scope() != NULL);
     printf ("An the current scope is (from info): info.get_current_scope() = %p = %s = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str(),SageInterface::get_name(info.get_current_scope()).c_str());
     curprint ( string("\n /* Inside of unparseVarDeclStmt() */ \n"));
#endif

     SgVariableDeclaration* vardecl_stmt = isSgVariableDeclaration(stmt);
     ROSE_ASSERT(vardecl_stmt != NULL);

#if 0
     vardecl_stmt->get_declarationModifier().display("Called from unparseVarDeclStmt()");
#endif

  // printf ("In unparseVarDeclStmt(): info.get_current_scope() = %p = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str());

  // curprint ( string("\n/* In unparseVarDeclStmt(): vardecl_stmt->get_need_name_qualifier() = " + (vardecl_stmt->get_need_name_qualifier() == true ? "true" : "false") + " */ \n";

  // 3 types of output
  //    var1=2, var2=3 (enum list)
  //    int var1=2, int var2=2 (arg list)
  //    int var1=2, var2=2 ; (vardecl list)
  // must also allow for this
  //    void (*set_foo)()=doo

  // Build a new SgUnparse_Info object to represent formatting options for this statement
     SgUnparse_Info ninfo(info);

  // ninfo.display ("At top of Unparse_ExprStmt::unparseVarDeclStmt");

  // DQ (10/14/2006): Set the context to record the variable declaration being generated!
  // We can use this later if we have to query the AST for if a variable using a class 
  // declaration appears before or after it's definition (required to know how to qualify 
  // the SgClassType)
     ninfo.set_declstatement_ptr(NULL);
     ninfo.set_declstatement_ptr(vardecl_stmt);

  // curprint ( string("\n/* After being set (unparseVarDeclStmt): ninfo.get_declstatement_ptr() = " + ((ninfo.get_declstatement_ptr() != NULL) ? ninfo.get_declstatement_ptr()->class_name() : "no declaration statement defined") + " */\n ";
     ROSE_ASSERT(ninfo.get_declstatement_ptr() != NULL);

  // DQ (11/29/2004): This code is required for the output of the access specifier 
  // (public, protected, private) and applies only within classes.  Use get_parent()
  // instead of get_scope() since we are looking for the structural position of the 
  // variable declaration (is it is a class).
     SgClassDefinition *classDefinition = isSgClassDefinition(vardecl_stmt->get_parent());
     if (classDefinition != NULL)
        {
       // Don't output an access specifier in this is a struct or union!
       // printf ("Don't output an access specifier in this is a struct or union! \n");

       // DQ and PC (6/1/2006): Added Peter's suggested fixes to support unparsing fully qualified names (supporting auto-documentation).
       // if (classDefinition->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
          if (classDefinition->get_declaration()->get_class_type() == SgClassDeclaration::e_class && !info.skipCheckAccess())
               ninfo.set_CheckAccess();
       // inClass = true;
       // inCname = isSgClassDefinition(vardecl_stmt->get_parent())->get_declaration()->get_name();
        }

     ROSE_ASSERT(ninfo.get_declstatement_ptr() != NULL);

  // printf ("Calling unp->u_sage->printSpecifier1() \n");
  // curprint ( string("\n/* Calling unp->u_sage->printSpecifier1() */ \n";
  // printDebugInfo("entering unp->u_sage->printSpecifier1", true);
     unp->u_sage->printSpecifier1(vardecl_stmt, ninfo); 
  // printf ("DONE: Calling unp->u_sage->printSpecifier1() \n");
  // curprint ( string("\n/* DONE: Calling unp->u_sage->printSpecifier1() */ \n";

  // Save the input information
     SgUnparse_Info saved_ninfo(ninfo); 
  // this call has been moved below, after we indent
  // unp->u_sage->printSpecifier2(vardecl_stmt, ninfo);

  // Setup the SgUnparse_Info object for this statement
     ninfo.unset_CheckAccess();
     info.set_access_attribute(ninfo.get_access_attribute());

  // DQ (11/28/2004): There should be at least a single variable here!
     ROSE_ASSERT(vardecl_stmt->get_variables().size() > 0);
  // printf ("Number of variables = %zu \n",vardecl_stmt->get_variables().size());

  // SgInitializedNamePtrList::iterator p = vardecl_stmt->get_variables().begin();
  // ROSE_ASSERT(p != vardecl_stmt->get_variables().end());

     SgName tmp_name;
     SgType *tmp_type        = NULL;
  // SgPointerMemberType *pm_type = NULL;
     SgInitializer *tmp_init = NULL;

     SgInitializedName *decl_item      = NULL;
  // SgInitializedName *prev_decl_item = NULL;

  // DQ (11/28/2004): Is this even used in enum declarations! (I don't think so!)
     ROSE_ASSERT(ninfo.inEnumDecl() == false);

  // DQ (11/28/2004): Within an enum declaration there should not be a type associated with the variables???
     if (ninfo.inEnumDecl())
          ninfo.unset_isWithType();
       else
          ninfo.set_isWithType();

  // DQ (10/5/2004): This is the explicitly set boolean value which indicates that a class declaration is buried inside
  // the current variable declaration (e.g. struct A { int x; } a;).  In this case we have to output the base type with
  // its definition.
     bool outputTypeDefinition = vardecl_stmt->get_variableDeclarationContainsBaseTypeDefiningDeclaration();
  // printf ("outputTypeDefinition = %s \n",(outputTypeDefinition == true) ? "true" : "false");

  // if (p != vardecl_stmt->get_variables().end())
     SgInitializedNamePtrList::iterator p = vardecl_stmt->get_variables().begin();
     ROSE_ASSERT(p != vardecl_stmt->get_variables().end());

     while (p != vardecl_stmt->get_variables().end())
        {
          decl_item = *p;
          ROSE_ASSERT(decl_item != NULL);

          tmp_init = NULL;


          tmp_type = decl_item->get_type();

       // DQ (5/11/2007): This fails in astCopy_tests for copyExample using copyExampleInput.C
          ROSE_ASSERT(isSgType(tmp_type) != NULL);

#if 0
          printf ("SgInitializedName decl_item = %p \n",decl_item);
          decl_item->get_startOfConstruct()->display("SgInitializedName decl_item");
#endif

#define REWRITE_MACRO_HANDLING
#ifdef REWRITE_MACRO_HANDLING
       // DQ (9/17/2003) feature required for rewrite mechanism (macro handling)

       // Search for special coded declaration which are specific to the 
       // handling of MACROS in ROSE.  variable declarations of a specific form:
       //      char * rose_macro_declaration_1 = "<macro string>";
       // are unparsed as:
       //      macro string
       // to permit macros to be passed unevaluated through the transformation 
       // process (unevaluated in the intermediate files generated to process 
       // the transformations).

          if (!vardecl_stmt->isForward() && !isSgFunctionType(tmp_type))
             {
               ROSE_ASSERT (decl_item != NULL);
               tmp_init = decl_item->get_initializer();

               if (tmp_init != NULL)
                  {
                    SgAssignInitializer *assignInitializer = isSgAssignInitializer(tmp_init);
                    if (assignInitializer != NULL)
                       {
                         SgStringVal *stringValueExpression = isSgStringVal(assignInitializer->get_operand());
                         if (stringValueExpression != NULL)
                            {
#ifndef CXX_IS_ROSE_CODE_GENERATION
                           // DQ (3/25/2006): Finally we can use the C++ string class
                              string targetString = "ROSE-MACRO-CALL:";
                              int targetStringLength = targetString.size();
                              string stringValue = stringValueExpression->get_value();
                              string::size_type location = stringValue.find(targetString);
                              if (location != string::npos)
                                 {
                                // unparse the string without the surrounding quotes and with a new line at the end
                                   string remainingString = stringValue.replace(location,targetStringLength,"");
                                // printf ("Specify a MACRO: remainingString = %s \n",remainingString.c_str());
                                   remainingString.replace(remainingString.find("\\\""),4,"\"");
                                   curprint ( string("\n" ) + remainingString + "\n");
                                   return;
                                 }
#endif
                            }
                       }
                  }
             }
#endif

          bool first = true;

       // DQ (11/28/2004): I hate it when "while(true)" is used!
       // while(true)
          if (p == vardecl_stmt->get_variables().begin())
             {
            // If this is the first variable then output the base type

               decl_item = *p;
               ROSE_ASSERT(decl_item != NULL);

            // printf ("In unparseVarDeclStmt(): cname = decl_item->get_name() = %s \n",decl_item->get_name().str());

               tmp_name = decl_item->get_name();
               tmp_type = decl_item->get_type();
               ROSE_ASSERT(isSgType(tmp_type) != NULL);

            // DQ (11/28/2004): Added to support new design
               tmp_init = decl_item->get_initializer();

            // printf ("tmp_type = %p = %s \n",tmp_type,tmp_type->sage_class_name());

               SgNamedType *namedType = isSgNamedType(tmp_type->findBaseType());
            // SgDeclarationStatement* declStmt = (namedType) ? namedType->get_declaration() : NULL;
               SgDeclarationStatement* declStmt = NULL;

               ROSE_ASSERT(ninfo.get_declstatement_ptr() != NULL);

            // printf ("namedType = %p \n",namedType);
               if (namedType != NULL)
                  {
                 // DQ (10/5/2004): This controls the unparsing of the class definition
                 // when unparsing the type within this variable declaration.
                 // printf ("In unparseVarDeclStmt(): outputTypeDefinition = %s \n",outputTypeDefinition ? "true" : "false");
                    if (outputTypeDefinition == true)
                       {
                      // printf ("In unparseVarDeclStmt(): Use the defining declaration as a basis for the variable declaration \n");
                         ROSE_ASSERT(namedType->get_declaration() != NULL);
                         declStmt = namedType->get_declaration()->get_definingDeclaration();
                      // printf ("outputTypeDefinition == true: declStmt = %p \n",declStmt);
                         if (declStmt == NULL)
                            {
#if 0
                              printf ("namedType->get_declaration() = %p = %s \n",
                                   namedType->get_declaration(),namedType->get_declaration()->sage_class_name());
#endif
                           // it is likely an enum declaration which does not yet use the defing vs. non-defining mechanisms
                           // so just set it to the currently available declaration (since for enums there is only one!)
                              declStmt = namedType->get_declaration();
                            }
                         ROSE_ASSERT(declStmt != NULL);
                         ROSE_ASSERT(declStmt->isForward() == false);

                      // DQ (10/9/2006): Don't output the qualified name of a defining declaration.
                      // ninfo.unset_forceQualifiedNames();

                      // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
                         ninfo.set_SkipQualifiedNames();
#if 1
                      // DQ (5/23/2007): Commented these out since they are not applicable for statement expressions (see test2007_51.C).
                      // DQ (10/5/2004): If this is a defining declaration then make sure that we don't skip the definition
                         ROSE_ASSERT(ninfo.SkipClassDefinition() == false);
                         ROSE_ASSERT(ninfo.SkipEnumDefinition()  == false);
                         ROSE_ASSERT(ninfo.SkipDefinition()      == false);
#endif
                       }
                      else
                       {
                      // printf ("Use what is surely a non-defining declaration as a basis for the variable declaration \n");
                         declStmt = namedType->get_declaration();
                         ROSE_ASSERT(declStmt != NULL);
                      // printf ("outputTypeDefinition == false: declStmt = %p \n",declStmt);
                      // Only implement test for class declarations
                         if (isSgClassDeclaration(declStmt) != NULL)
                            {
                           // DQ (10/8/2004): If this is not a enum then expect a forward declaration!
                           // Enum declarations are an exception since forward declarations of enums 
                           // are not permitted in C and C++ according to the standard!
                              if (declStmt->isForward() == false)
                                 {
                                   printf ("Warning about declStmt = %p = %s \n",declStmt,declStmt->sage_class_name());
                                   declStmt->get_file_info()->display("Warning: declStmt->isForward() == false (should be true)");
                                 }

#if PRINT_DEVELOPER_WARNINGS
                             printf ("Commented out assertion in unparser to handle AST Merge bug: declStmt->isForward() == true \n");
#endif
                          // ROSE_ASSERT(declStmt->isForward() == true);
                            }

                      // DQ (8/6/2007): Skip forcing the output of qualified names now that we have a hidden list mechanism.
                      // DQ (10/9/2006): Force output any qualified names (particularly for non-defining declarations).
                      // This is a special case for types of variable declarations.
                      // ninfo.set_forceQualifiedNames();

                      // curprint ( string("/* outputTypeDefinition = false and calling ninfo.set_forceQualifiedNames() */ ";

                      // DQ (10/5/2004): If this is a non-defining declaration then skip the definition
                      // ninfo.set_SkipClassDefinition();
                         ninfo.set_SkipDefinition();
                         ROSE_ASSERT(ninfo.SkipClassDefinition() == true);
                         ROSE_ASSERT(ninfo.SkipEnumDefinition() == true);
                       }
                  }

            // printf ("Calling unp->u_sage->printSpecifier2 \n");
            // curprint ( string("\n/* Calling unp->u_sage->printSpecifier2() */ \n";
            // printDebugInfo("entering unp->u_sage->printSpecifier2", true);
               unp->u_sage->printSpecifier2(vardecl_stmt, saved_ninfo);
            // curprint ( string("\n/* DONE: Calling unp->u_sage->printSpecifier2() */ \n";

            // DQ (11/28/2004): Are these true! No! declStmt is really the declaration of any parent scope (excluding global scope!)
            // ROSE_ASSERT(declStmt != NULL);
            // ROSE_ASSERT(isSgTypedefDeclaration(declStmt) == NULL);

#if 0
               printf ("first = %s \n",(first == true) ? "true" : "false");
               printf ("declStmt = %p \n",declStmt);
               printf ("isSgTypedefDeclaration(declStmt) = %p \n",isSgTypedefDeclaration(declStmt));
#endif

            // DQ (11/28/2004): Is this always true?  If so then we can simplify the code!
               ROSE_ASSERT(tmp_type != NULL);

               SgUnparse_Info ninfo_for_type(ninfo);

               ROSE_ASSERT(ninfo_for_type.get_declstatement_ptr() != NULL);

            // printf ("At TOP: ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");

            // if (vardecl_stmt->skipElaborateType())
               if ( (vardecl_stmt->skipElaborateType()) && (declStmt != NULL) && (isSgTypedefDeclaration(declStmt) == NULL) )
                  {
                 // ninfo2.set_SkipClassSpecifier();
                    ninfo_for_type.set_SkipClassSpecifier();
                  }

            // printf ("After isNameOnly() test: ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");
#if 1
            // DQ (8/23/2006): This was used to specify global qualification separately from the more general name 
            // qualification mechanism, however having two mechanisms is a silly level of redundancy so we now use 
            // just one (the more general one) even though it is only used for global name qualification.
            // DQ (8/20/2006): We can't mark the SgType (since it is shared), and we can't mark the SgInitializedName,
            // so we have to carry the information that we should mark the type in the SgVariableDeclaration.
            // printf ("vardecl_stmt->get_requiresNameQualification() = %s \n",vardecl_stmt->get_requiresNameQualification() ? "true" : "false");
               if (vardecl_stmt->get_requiresGlobalNameQualificationOnType() == true)
                  {
                 // Output the name qualification for the type in the variable declaration.
                 // But we have to do so after any modifiers are output, so in unp->u_type->unparseType().
                 // printf ("In Unparse_ExprStmt::unparseVarDeclStmt(): This variable declaration requires a global qualifier \n");
                 // ninfo2.set_forceQualifiedNames();
                    ninfo_for_type.set_requiresGlobalNameQualification();
                  }
#endif
            // ninfo2.set_isTypeFirstPart();
               ninfo_for_type.set_isTypeFirstPart();

            // printf ("ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");
            // curprint ( string("\n/* START: output using unp->u_type->unparseType (1st part) */ \n";
            // printf ("Calling 1st part of unp->u_type->unparseType for %s \n",tmp_type->sage_class_name());

            // ROSE_ASSERT(ninfo2.get_declstatement_ptr() != NULL);
               ROSE_ASSERT(ninfo_for_type.get_declstatement_ptr() != NULL);

            // unp->u_type->unparseType(tmp_type, ninfo2);
               ROSE_ASSERT(isSgType(tmp_type) != NULL);
               unp->u_type->unparseType(tmp_type, ninfo_for_type);

            // ROSE_ASSERT(ninfo2.get_declstatement_ptr() != NULL);
               ROSE_ASSERT(ninfo_for_type.get_declstatement_ptr() != NULL);

            // curprint ( string("\n/* END: output using unp->u_type->unparseType (1st part) */ \n";
            // DQ (11/28/2004): Added qualifier to variable name.

            // DQ (10/6/2004): Changed this back to the previous ordering so that we could handle test2004_104.C
            // DQ (9/28/2004): Output the variable name after the first and second parts of the type!
               if (tmp_name.is_null() == false)
                  {
                 // printf ("Before output --- Variable Name: tmp_name = %s \n",tmp_name.str());
                 // Only output the name qualifier if we are going to output the variable name!
                    ROSE_ASSERT(decl_item != NULL);
                 // SgInitializedName* decl_item_in_scope = decl_item->get_prev_decl_item();
                 // printf ("Should be valid pointer: decl_item->get_prev_decl_item() = %p \n",decl_item->get_prev_decl_item());

                 // DQ (11/28/2004): Find the scope of the current SgInitializedName object and get the list of 
                 // declarations in that scope.  Get the first declaration and use it as input to the unp->u_type->unp->u_name->generateNameQualifier()
                 // function (which requires a declaration).  We can't use the current SgVariableDeclaration we 
                 // are processing since it might be from a different scope!
                    ROSE_ASSERT(decl_item != NULL);
                    ROSE_ASSERT(decl_item->get_parent() != NULL);

                 // DQ (5/1/2005): Note clear if we can assert this!
                 // ROSE_ASSERT(decl_item->get_prev_decl_item() != NULL);
                    if (decl_item->get_prev_decl_item() != NULL)
                       {
                      // printf ("decl_item->get_prev_decl_item() = %p get_name() = %s \n",
                      //      decl_item->get_prev_decl_item(),decl_item->get_prev_decl_item()->get_name().str());
                         ROSE_ASSERT(decl_item->get_prev_decl_item()->get_parent() != NULL);
                       }

                 // DQ (10/10/2006): Only do name qualification for C++
                    if (SageInterface::is_Cxx_language() == true)
                       {
                         SgUnparse_Info ninfo_for_variable(ninfo);
#if 0
                      // DQ (10/24/2007): Added fix by Jeremiah (not well tested)
                         if (classDefinition != NULL) {
                           // JJW 10-23-2007
                           // Never qualify a member variable of a class
                           ninfo_for_variable.set_SkipQualifiedNames();
                         }
#endif
                      // DQ (1/5/2007): C++ can be more complex and we can be initializing a variable in global scope initially declared in a class or namespace.
                      // printf ("variable name = %s decl_item->get_prev_decl_item() = %p \n",tmp_name.str(),decl_item->get_prev_decl_item());

                      // SgScopeStatement* parentScope = decl_item->get_scope();
                      // ROSE_ASSERT(ninfo2.get_declstatement_ptr() != NULL);
                         ROSE_ASSERT(ninfo_for_variable.get_declstatement_ptr() != NULL);

                      // SgName nameQualifier = unp->u_type->unp->u_name->generateNameQualifier(decl_item,ninfo2);
                         SgName nameQualifier = unp->u_name->generateNameQualifier(decl_item,ninfo_for_variable);
                      // printf ("variable declaration name = %s nameQualifier = %s \n",tmp_name.str(),(nameQualifier.is_null() == false) ? nameQualifier.str() : "NULL");
                         if (nameQualifier.is_null() == false)
                            {
                              curprint ( nameQualifier.str());
                            }
                       }

                 // printf ("Variable Name: tmp_name = %s \n",tmp_name.str());
                    curprint ( tmp_name.str());

                 // DQ (7/25/2006): Support for asm register naming within variable declarations (should also be explicitly marked as "register")
                 // ROSE_ASSERT(decl_item->get_register_name() == 0);
                    if (decl_item->get_register_name_code() != SgInitializedName::e_invalid_register)
                       {
                      // an asm ("<register name>") is in use
                          curprint ( string(" asm (\""));
                       // curprint ( string("<unparse register name>";
                          curprint ( unparse_register_name(decl_item->get_register_name_code()));
                          curprint ( string("\")"));
                       }

                 // DQ (1/25/2009): If we are not using the Assembly Register codes then we might be using the string 
                 // mechanism (stored in SgInitializedName::p_register_name_string). The new EDG/Sage interface can
                 // support the use of either Assembly Register codes or raw strings.
                    if (decl_item->get_register_name_string().empty() == false)
                       {
                      // an asm ("<register name>") is in use
                          curprint ( string(" asm (\""));
                          curprint ( decl_item->get_register_name_string() );
                          curprint ( string("\")"));
                       }
                  }

            // ninfo2.set_isTypeSecondPart();
               ninfo_for_type.set_isTypeSecondPart();
#if 0
               curprint ( string("\n/* START: output using unp->u_type->unparseType (2nd part) */ \n"));
               printf ("Calling 2nd part of unp->u_type->unparseType for %s \n",tmp_type->sage_class_name());
               printf ("ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");
#endif
#if 0
            // DQ (9/28/2004): Now that we have defining and non-defining declarations we should be able to simplify this code!
            // if (isSgArrayType(tmp_type) != NULL)
            // if ( (isSgArrayType(tmp_type) != NULL) && (declStmt != NULL) && (isSgTypedefDeclaration(declStmt) == NULL) )
               if ( (declStmt != NULL) && (isSgTypedefDeclaration(declStmt) == NULL) )
                  {
                    printf ("(declStmt != NULL) && (isSgTypedefDeclaration(declStmt) == NULL) == true \n");
                    printf ("tmp_type = %p = %s \n",tmp_type,tmp_type->class_name().c_str());
#if 0
                    if (isSgArrayType(tmp_type) != NULL)
                       {
                         printf ("This is an array type so use special handling ... \n");
                         unp->u_type->unparseType(tmp_type, ninfo2);
                       }
#else
                    unp->u_type->unparseType(tmp_type, ninfo2);
#endif
                  }
                 else
                  {
                    printf ("(declStmt != NULL) && (isSgTypedefDeclaration(declStmt) == NULL) == false \n");
                    unp->u_type->unparseType(tmp_type, ninfo2);
                  }
#else
                 // DQ (7/31/2006): I think that we can simplify to just this code.
                 // unp->u_type->unparseType(tmp_type, ninfo2);
                    unp->u_type->unparseType(tmp_type, ninfo_for_type);
#endif

            // Mark that we are no longer processing the first entry 
            // (first variable in a declaration containing multiple "," separated names)
               first = false;
#if 0
               curprint ( string("\n/* Handle initializers (if any) */ \n"));
#endif
            // Unparse the initializers if any exist
            // if(tmp_init)
               if ( (tmp_init != NULL) && !ninfo.SkipInitializer())
                  {
                 // printf ("Initializer tmp_init = %s \n",tmp_init->class_name().c_str());
#if 0
                    if ( tmp_init->variant() == ASSIGN_INIT || tmp_init->variant() == AGGREGATE_INIT )
                       {
                         curprint ( string(" = "));
                       }
#else
                 // DQ (8/5/2005): generate more faithful representation of assignment operator!
#if 0
                    ninfo.display ("In Unparse_ExprStmt::unparseVarDeclStmt --- handling the initializer");
#endif

                    SgConstructorInitializer* constructor = isSgConstructorInitializer(tmp_init);
#if 0
                    if (constructor != NULL)
                       {
                      // curprint(" /* unparseVarDeclStmt (initializer constructor): " + constructor->class_name() + " */ ");
                         printf ("constructor->get_need_name()                   = %s \n",constructor->get_need_name()                   ? "true" : "false");
                         printf ("constructor->get_is_explicit_cast()            = %s \n",constructor->get_is_explicit_cast()            ? "true" : "false");
                         printf ("constructor->get_need_parenthesis_after_name() = %s \n",constructor->get_need_parenthesis_after_name() ? "true" : "false");
                         printf ("constructor->get_associated_class_unknown()    = %s \n",constructor->get_associated_class_unknown()    ? "true" : "false");

                      // DQ (11/9/2009): Added support for debugging test2009_40.C.
                         SgMemberFunctionDeclaration * associatedConstructor = constructor->get_declaration();
                         printf ("associatedConstructor = %p \n",associatedConstructor);
                         if (associatedConstructor != NULL)
                            {
                              printf ("associatedConstructor = %s \n",associatedConstructor->class_name().c_str());
                              printf ("associatedConstructor is private   = %s \n",associatedConstructor->get_declarationModifier().get_accessModifier().isPrivate()   ? "true" : "false");
                              printf ("associatedConstructor is protected = %s \n",associatedConstructor->get_declarationModifier().get_accessModifier().isProtected() ? "true" : "false");
                              printf ("associatedConstructor is public    = %s \n",associatedConstructor->get_declarationModifier().get_accessModifier().isPublic()    ? "true" : "false");

                           // printf ("associatedConstructor is public    = %s \n",associatedConstructor->get_declarationModifier().get_accessModifier().isPublic()    ? "true" : "false");
#if 0
                              SgUnparse_Info local_info;
                              printf ("stmt is = %s \n",stmt->unparseToString(&local_info).c_str());
                              printf ("constructor is = %s \n",constructor->unparseToString(&local_info).c_str());
                              printf ("associatedConstructor is = %s \n",associatedConstructor->unparseToString(&local_info).c_str());
#endif
                            }
                       }

                    curprint(" /* unparseVarDeclStmt (initializer): " + tmp_init->class_name() + " */ ");
#endif
                    if ( (tmp_init->variant() == ASSIGN_INIT) ||
                         (tmp_init->variant() == AGGREGATE_INIT) ||
                      // ( (constructor != NULL) && constructor->get_need_name() && constructor->get_is_explicit_cast() ) )
                      // DQ (7/12/2006): Bug fix reported by Peter Collingbourne
                      // ( (constructor != NULL) && (constructor->get_need_name() || constructor->get_associated_class_unknown()) ) )

                      // DQ (11/9/2009): Turn off this optional handling since "IntStack::Iter z = (&x)" is not interpreted 
                      // the same as "IntStack::Iter z(&x)" at least in a SgForInitializationStatement (see test2009_40.C).
                      // I think that this was always cosmetic anyway.
                      // ( (constructor != NULL) && (constructor->get_need_name() || constructor->get_associated_class_unknown() || ninfo.inConditional()) ) )
                      // ( false ) )
                         ( (constructor != NULL) && (constructor->get_need_name() || constructor->get_associated_class_unknown() || ninfo.inConditional()) ) )
                       {
                      // DQ (11/9/2009): Skip the case of when we are in a isSgForInitStmt, since this is a bug in GNU g++ (at least version 4.2)
                      // See test2009_40.C test2009_41.C, and test2009_42.C
                      // curprint ( string(" = "));
                         if ( constructor != NULL && isSgForInitStatement(stmt->get_parent()) != NULL )
                            {
                           // DQ (2/9/2010): Previous code had this commented out to fix test2009_40.C.
                           // curprint (" = ");

                           // DQ (2/9/2010): See test2010_05.C
                              if (constructor->get_need_name() == true && constructor->get_is_explicit_cast() == true )
                                 {
                                // This is the syntax: class X = X(arg)
                                   curprint (" = ");
                                 }
                                else
                                 {
                                // This is the alternative syntax: class X(arg)
                                // So don't output a "="
                                 }
                            }
                           else
                            {
                              curprint (" = ");
                            }
                       }
#endif

                    SgUnparse_Info statementInfo(ninfo);
                    statementInfo.set_SkipClassDefinition();

                 // DQ (1/7/2007): Unset the declstatement_ptr so that we can know that we 
                 // are processing initalizers which might require name qualification!
                    statementInfo.set_declstatement_ptr(NULL);

                 // statementInfo.display("Debugging the initializer (set SkipClassDefinition");

                 // printf ("Unparse the initializer \n");
                 // curprint ( string("/* Unparse the initializer */ \n";
                 // unparseExpression(tmp_init, ninfo);
                    unparseExpression(tmp_init, statementInfo);
                 // printf ("DONE: Unparse the initializer \n");
                 // curprint ( string("/* DONE: Unparse the initializer */ \n";
                  }
             }

       // Increment the iterator through the list of variables within a single variable declaration.
       // Currently each variable declaration contains only a single variable!
          p++;

       // DQ (11/28/2004): Within the current design this is always true. Since we normalize 
       // multiple variable declarations into single variable declarations.
          ROSE_ASSERT (p == vardecl_stmt->get_variables().end());

       // DQ (11/28/2004): This this is always false within the current design.
          if (p != vardecl_stmt->get_variables().end())
             {
               if (!ninfo.inArgList())
                    ninfo.set_SkipBaseType();
               curprint ( string(","));
             }
        }

  // curprint ( string("\n/* Handle bit fields specifiers (if any) */ \n";

  // DQ (11/28/2004): Bit fields specifiers should be associated with the SgInitializedName 
  // and not the SgVariableDeclaration!  However this works because variable declarations 
  // with multiple variables within a single declaration are separated out as single variable 
  // declarations (so including the bitfield width with the variable declaration works).  
  // If we ever permit unparsing of multiple variables within a single variable declaration 
  // then this would be a problem within the current design.

  // Bit fields appear as "a_bit_field autonomous_tag_decl:1;"
     SgVariableDefinition *defn = vardecl_stmt->get_definition();
     if (defn != NULL)
        {
#if 1
       // DQ (2/3/2007): Implement this unparse functions so that we can refactor this code
       // and so that the unparseToString will work properly for the SgVariableDefinition IR node.
          unparseVarDefnStmt(vardecl_stmt->get_definition(),ninfo);
#else
          SgUnsignedLongVal *bitfield = vardecl_stmt->get_definition()->get_bitfield();
          if (bitfield != NULL)
             {
               curprint ( string(":"));
               unparseExpression(bitfield, ninfo);
             }
#endif
        }

  // ninfo.display("Close off the statement with a \";\"?");

  // curprint ( string("\n/* START: Close off the statement with a \";\" */ \n";

  // Close off the statement with a ";"
  // DQ (7/12/2006): Bug fix reported by Peter Collingbourne
  // if (!ninfo.inEnumDecl() && !ninfo.inArgList() && !ninfo.SkipSemiColon())
     if (!ninfo.inEnumDecl() && !ninfo.inArgList() && !ninfo.inConditional() && !ninfo.SkipSemiColon())
        {
          curprint ( string(";"));
        }

  // curprint ( string("\n/* END: Close off the statement with a \";\" */ \n";

#if 0
     printf ("Leaving unparseVarDeclStmt() \n");
     curprint ( string("/* Leaving unparseVarDeclStmt() */ \n";
#endif
   }


void
Unparse_ExprStmt::unparseVarDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgVariableDefinition* vardefn_stmt = isSgVariableDefinition(stmt);
     ROSE_ASSERT(vardefn_stmt != NULL);

  // DQ: (9/17/2003)
  // Although I have not seen it in any of our tests of ROSE the SgVariableDefinition
  // does appear to be used in the declaration of bit fields!  Note the comment at the
  // end of the unparseVarDeclStmt() function where the bit field is unparsed! Though 
  // it appears that the unparseVarDefnStmt is not required to the unparsing of the 
  // bit field, so this function is never called!

  // DQ (2/3/2007): However, for the ODR check in the AST merge we require something to
  // be generated for everything that could be shared.  So we should unparse something,
  // perhaps the variable declaration?
#if 0
  // DQ: added 3/25/2001
     printf("unparseVarDefnStmt not implemented (should it be?) \n");
     ROSE_ASSERT(false);
#else
     SgUnsignedLongVal *bitfield = vardefn_stmt->get_bitfield();
     if (bitfield != NULL)
        {
          curprint ( string(":"));
          unparseExpression(bitfield, info);
        }
#endif
   }


void
Unparse_ExprStmt::initializeDeclarationsFromParent ( 
   SgDeclarationStatement* declarationStatement,
   SgClassDefinition* & cdefn,
   SgNamespaceDefinitionStatement* & namespaceDefn, int debugSupport )
   {
  // DQ (11/18/2004): Now that we store the scope explicitly we don't have to interprete the parent pointer!
     ROSE_ASSERT(declarationStatement != NULL);
     SgScopeStatement* parentScope = declarationStatement->get_scope();
     ROSE_ASSERT(parentScope != NULL);

#if 0
     if (debugSupport > 0)
          printf ("In initializeDeclarationsFromParent(): parentScope = %p = %s \n",parentScope,parentScope->sage_class_name());
#endif

     cdefn         = isSgClassDefinition(parentScope);
     namespaceDefn = isSgNamespaceDefinitionStatement(parentScope);      

#if 0
     if (debugSupport > 0)
          printf ("In initializeDeclarationsFromParent(): cdefn = %p namespaceDefn = %p \n",cdefn,namespaceDefn);
#endif
   }



void
Unparse_ExprStmt::unparseClassDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgClassDeclaration* classdecl_stmt = isSgClassDeclaration(stmt);
     ROSE_ASSERT(classdecl_stmt != NULL);

#if 0
  // printf ("Inside of Unparse_ExprStmt::unparseClassDeclStmt(): classdecl_stmt->get_from_template() = %s \n",
  //      classdecl_stmt->get_from_template() ? "true" : "false");
     if (classdecl_stmt->get_from_template() == true)
          curprint ( string("/* Unparser comment: Templated Class Declaration Function */"));
     Sg_File_Info* classDeclarationfileInfo = classdecl_stmt->get_file_info();
     ROSE_ASSERT ( classDeclarationfileInfo != NULL );
     if ( classDeclarationfileInfo->isCompilerGenerated() == false)
          curprint ( string("\n/* file: " ) + classDeclarationfileInfo->get_filename() + " line: " + classDeclarationfileInfo->get_line() + " col: " + classDeclarationfileInfo->get_col() + " */ \n");
#endif

  // info.display("Inside of unparseClassDeclStmt");

  // printf ("At top of unparseClassDeclStmt name = %s \n",classdecl_stmt->get_name().str());

#if 0
     printf ("In Unparse_ExprStmt::unparseClassDeclStmt(): classdecl_stmt = %p isForward() = %s info.SkipClassDefinition() = %s name = %s \n",
          classdecl_stmt,(classdecl_stmt->isForward() == true) ? "true" : "false",
          (info.SkipClassDefinition() == true) ? "true" : "false",classdecl_stmt->get_name().str());
#endif

  // DQ (11/7/2007): Fixup the SgUnparse_Info object to store the correct scope.
     SgUnparse_Info class_info(info);
     fixupScopeInUnparseInfo (class_info,classdecl_stmt);

     if (!classdecl_stmt->isForward() && classdecl_stmt->get_definition() && !info.SkipClassDefinition())
        {
          SgUnparse_Info ninfox(class_info);

          ninfox.unset_SkipSemiColon();

       // DQ (6/13/2007): Set to null before resetting to non-null value 
          ninfox.set_declstatement_ptr(NULL);
          ninfox.set_declstatement_ptr(classdecl_stmt);

       // printf ("Calling unparseStatement(classdecl_stmt->get_definition(), ninfox); for %s \n",classdecl_stmt->get_name().str());
          unparseStatement(classdecl_stmt->get_definition(), ninfox);

          if (!info.SkipSemiColon())
             {
               curprint (  string(";"));
             }
        }
       else
        {
          if (!info.inEmbeddedDecl())
             {
               SgUnparse_Info ninfo(class_info);
               if (classdecl_stmt->get_parent() == NULL)
                  {
                    printf ("classdecl_stmt->isForward() = %s \n",(classdecl_stmt->isForward() == true) ? "true" : "false");
                  }

            // DQ (5/20/2006): This is false within "stdio.h"
               if (classdecl_stmt->get_parent() == NULL)
                  {
                    classdecl_stmt->get_file_info()->display("In Unparse_ExprStmt::unparseClassDeclStmt(): classdecl_stmt->get_parent() == NULL");
                  }
            // ROSE_ASSERT (classdecl_stmt->get_parent() != NULL);
               SgClassDefinition *cdefn = isSgClassDefinition(classdecl_stmt->get_parent());

               if(cdefn && cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
                    ninfo.set_CheckAccess();

            // DQ (8/19/2004): Removed functions using old attribute mechanism (old CC++ mechanism)
            // printf ("Commented out get_suppress_global(classdecl_stmt) \n");
            // if (get_suppress_global(classdecl_stmt))
            //      ninfo.set_SkipGlobal(); //attributes.h

            // printDebugInfo("entering unp->u_sage->printSpecifier", true);
               unp->u_sage->printSpecifier(classdecl_stmt, ninfo);
               info.set_access_attribute(ninfo.get_access_attribute());
             }

          info.unset_inEmbeddedDecl();

          if (!info.SkipClassSpecifier())
             {
               switch (classdecl_stmt->get_class_type()) 
                  {
                    case SgClassDeclaration::e_class : 
                       {
                         curprint ( string("class "));
                         break;
                       }
                    case SgClassDeclaration::e_struct :
                       {
                         curprint ( string("struct "));
                         break;
                       }
                    case SgClassDeclaration::e_union :
                       {
                         curprint ( string("union "));
                         break;
                       }

                 // DQ (4/17/2007): Added this enum value to the switch cases.
                    case SgClassDeclaration::e_template_parameter :
                       {
                      // skip type elaboration here.
                         curprint ( string(" "));
                         break;
                       }

                 // DQ (4/17/2007): Added this enum value to the switch cases.
                    default:
                       {
                         printf ("Error: default reached in unparseClassDeclStmt() \n");
                         ROSE_ASSERT(false);
                         break;
                       }
                  }
              }

       /* have to make sure if it needs qualifier or not */

          SgName nm = classdecl_stmt->get_name();
#if 0
          if (nm == "Zone")
             {
               printf ("In unparseClassDeclStmt() class Zone: unp->u_type->unp->u_name->generateNameQualifier( cdecl , info ) = %s \n",
                    unp->u_type->unp->u_name->generateNameQualifier( classdecl_stmt , info ).str());
             }
#endif

       // DQ (10/11/2006): Don't generate qualified names for the class name a forward declaration
       // curprint ( string("/* In unparseClassDeclStmt: (skip qualified name if true) classdecl_stmt->get_forward() = " + ((classdecl_stmt->get_forward() == true) ? "true" : "false") + " */\n ";
          if (classdecl_stmt->get_forward() == false)
             {
            // ninfo.set_SkipQualifiedNames();

            // SgName nameQualifier = unp->u_name->generateNameQualifier( classdecl_stmt , info );
               SgName nameQualifier = unp->u_name->generateNameQualifier( classdecl_stmt , class_info );
            // printf ("In unparseClassDeclStmt() nameQualifier (from unp->u_type->unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());

               curprint ( nameQualifier.str());
             }

       // printf ("Output className = %s \n",classdecl_stmt->get_name().str());
       // curprint ( (classdecl_stmt->get_name() + " ").str();
          curprint ( (nm + " ").str());

       // curprint ( string("/* After name in Unparse_ExprStmt::unparseClassDeclStmt */ \n";

          if (classdecl_stmt->isForward() && !info.SkipSemiColon())
             {
               curprint ( string(";"));
               if (classdecl_stmt->isExternBrace())
                  {
                    curprint ( string(" }"));
                  }
             }
        }
   }

void
Unparse_ExprStmt::unparseClassDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Inside of unparseClassDefnStmt \n");
  // curprint ( string("/* Inside of unparseClassDefnStmt */ \n";

     SgClassDefinition* classdefn_stmt = isSgClassDefinition(stmt);
     ROSE_ASSERT(classdefn_stmt != NULL);

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (classdefn_stmt);
#endif

     SgUnparse_Info ninfo(info);

  // curprint ( string("/* Print out class declaration */ \n";

     ninfo.set_SkipClassDefinition();

  // DQ (10/13/2006): test2004_133.C demonstrates where we need to unparse qualified names for class definitions (defining declaration).
  // DQ (10/11/2006): Don't generate qualified names for the class name of a defining declaration
  // ninfo.set_SkipQualifiedNames();

  // DQ (7/19/2003) skip the output of the semicolon
     ninfo.set_SkipSemiColon();

  // printf ("Calling unparseClassDeclStmt = %p isForward = %s from unparseClassDefnStmt = %p \n",
  //      classdefn_stmt->get_declaration(),(classdefn_stmt->get_declaration()->isForward() == true) ? "true" : "false",classdefn_stmt);
     ROSE_ASSERT(classdefn_stmt->get_declaration() != NULL);
     unparseClassDeclStmt( classdefn_stmt->get_declaration(), ninfo);

  // DQ (7/19/2003) unset the specification to skip the output of the semicolon
     ninfo.unset_SkipSemiColon();

  // DQ (10/11/2006): Don't generate qualified names for the class name of a defining declaration
  // ninfo.unset_SkipQualifiedNames();

     ninfo.unset_SkipClassDefinition();

  // curprint ( string("/* END: Print out class declaration */ \n";

     SgNamedType *saved_context = ninfo.get_current_context();

  // DQ (11/29/2004): The use of a primary and secondary declaration casue two SgClassType nodes to be generated 
  // (which should be fixed) since this is compared to another SgClassType within the generateQualifiedName() 
  // function we have to get the the type from the non-defining declaration uniformally. Same way each time so that
  // the pointer test will be meaningful.
  // ninfo.set_current_context(classdefn_stmt->get_declaration()->get_type());
     ROSE_ASSERT(classdefn_stmt->get_declaration()->get_firstNondefiningDeclaration() != NULL);
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(classdefn_stmt->get_declaration()->get_firstNondefiningDeclaration());
     ROSE_ASSERT(classDeclaration->get_type() != NULL);

  // DQ (6/13/2007): Set to null before resetting to non-null value 
  // ninfo.set_current_context(classdefn_stmt->get_declaration()->get_firstNondefiningDeclaration()->get_type());
     ninfo.set_current_context(NULL);
     ninfo.set_current_context(classDeclaration->get_type());

  // curprint ( string("/* Print out inheritance */ \n";

  // print out inheritance
     SgBaseClassPtrList::iterator p = classdefn_stmt->get_inheritances().begin();
  // int tmp_spec = 0;
     SgClassDeclaration *tmp_decl;
     if (p != classdefn_stmt->get_inheritances().end())
        {
          curprint ( string(": "));
          while(true)
             {
            // DQ (4/25/2004): Use the new modifier interface
            // tmp_spec = (*p).get_base_specifier();
            // SgBaseClassModifier & baseClassModifier = (*p).get_baseClassModifier();
               SgBaseClassModifier & baseClassModifier = (*p)->get_baseClassModifier();
            // tmp_decl = (*p).get_base_class();
               tmp_decl = (*p)->get_base_class();
            // specifier
            // if (tmp_spec & SgDeclarationStatement::e_virtual)
            // if (tmp_spec & SgDeclarationStatement::e_virtual)
               if (baseClassModifier.isVirtual())
                  {
                    curprint ( string("virtual "));
                  }
            // if (tmp_spec & SgDeclarationStatement::e_public)
               if (baseClassModifier.get_accessModifier().isPublic())
                  {
                    curprint ( string("public "));
                  }
            // if (tmp_spec & SgDeclarationStatement::e_private)
               if (baseClassModifier.get_accessModifier().isPrivate())
                  {
                    curprint ( string("private "));
                  }
            // if (tmp_spec & SgDeclarationStatement::e_protected)
               if (baseClassModifier.get_accessModifier().isProtected())
                  {
                    curprint ( string("protected "));
                  }
  
            // base name
               curprint ( tmp_decl->get_name().str());
               p++;
               if (p != classdefn_stmt->get_inheritances().end())
                  {
                    curprint ( string(","));
                  }
                 else
                    break;
             }
        }

#if 0
  // curprint ( string("\n/* After specification of base classes unparse the declaration body */ \n";
     printf ("After specification of base classes unparse the declaration body  info.SkipBasicBlock() = %s \n",
          (info.SkipBasicBlock() == true) ? "true" : "false");
#endif

  // DQ (9/28/2004): Turn this back on as the only way to prevent this from being unparsed!
  // DQ (11/22/2003): Control unparsing of the {} part of the definition
  // if ( !info.SkipBasicBlock() )
     if ( info.SkipBasicBlock() == false )
        {
       // curprint ( string("\n/* Unparsing class definition within unparseClassDefnStmt */ \n";

       // DQ (6/14/2006): Add packing pragma support (explicitly set the packing 
       // alignment to the default, part of packing pragma normalization).
          unsigned int packingAlignment = classdefn_stmt->get_packingAlignment();
          if (packingAlignment != 0)
             {
               curprint ( string("\n#pragma pack(") + StringUtility::numberToString(packingAlignment) + string(")"));
             }

          ninfo.set_isUnsetAccess();
          unp->cur.format(classdefn_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);
          curprint ( string("{"));
          unp->cur.format(classdefn_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);

          SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();

          while ( pp != classdefn_stmt->get_members().end() )
             {
               unparseStatement((*pp), ninfo);
               pp++;
             }

       // DQ (3/17/2005): This helps handle cases such as class foo { #include "constant_code.h" }
          ROSE_ASSERT(classdefn_stmt->get_startOfConstruct() != NULL);
          ROSE_ASSERT(classdefn_stmt->get_endOfConstruct() != NULL);
#if 0
          printf ("classdefn_stmt range %d - %d \n",
               classdefn_stmt->get_startOfConstruct()->get_line(),
               classdefn_stmt->get_endOfConstruct()->get_line());
#endif
          unparseAttachedPreprocessingInfo(classdefn_stmt, info, PreprocessingInfo::inside);

          unp->cur.format(classdefn_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
          curprint ( string("}"));

       // DQ (6/14/2006): Add packing pragma support (reset the packing 
       // alignment to the default, part of packing pragma normalization).
          if (packingAlignment != 0)
             {
               curprint ( string("\n#pragma pack()"));
             }

          unp->cur.format(classdefn_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
        }

  // DQ (6/13/2007): Set to null before resetting to non-null value 
     ninfo.set_current_context(NULL);
     ninfo.set_current_context(saved_context);

  // curprint ( string("/* Leaving unparseClassDefnStmt */ \n";
  // printf ("Leaving unparseClassDefnStmt \n");
   }


void
Unparse_ExprStmt::unparseEnumDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEnumDeclaration* enum_stmt = isSgEnumDeclaration(stmt);
     ROSE_ASSERT(enum_stmt != NULL);

  // info.display("Called inside of unparseEnumDeclStmt()");

  // Check if this enum declaration appears imbedded within another declaration
     if ( !info.inEmbeddedDecl() )
        {
       // This is the more common declaration of an enum with the definition attached.
       // printf ("In unparseEnumDeclStmt(): common declaration of an enum with the definition attached \n");

       // If this is part of a class definition then get the access information
          SgClassDefinition *cdefn = isSgClassDefinition(enum_stmt->get_parent());
          if (cdefn && cdefn->get_declaration()->get_class_type()==SgClassDeclaration::e_class)
             {
               info.set_CheckAccess();
             }
       // printDebugInfo("entering unp->u_sage->printSpecifier", true);
          unp->u_sage->printSpecifier(enum_stmt, info);
          info.unset_CheckAccess();
          curprint ( string("enum " ) + enum_stmt->get_name().str() + " ");
        }
       else
        { 
       // This is a declaration of an enum appearing within another declaration (e.g. function declaration as a return type).
          SgClassDefinition *cdefn = NULL;

       // DQ (9/9/2004): Put this message in place at least for now!
          printf ("Need logic to handle enums defined in namespaces and which require qualification \n");

       // ck if defined within a var decl
          int v = GLOBAL_STMT;
       // SgStatement *cparent=enum_stmt->get_parent();
          SgStatement *cparent = isSgStatement(enum_stmt->get_parent());
          v = cparent->variant();
          if (v == VAR_DECL_STMT || v == TYPEDEF_STMT)
               cdefn = isSgClassDefinition(cparent->get_parent());
            else
               if ( v == CLASS_DEFN_STMT )
                    cdefn = isSgClassDefinition(cparent);

          if ( cdefn )
             {
               SgNamedType *ptype=isSgNamedType(cdefn->get_declaration()->get_type());
               if (!ptype || (info.get_current_context() == ptype))
                  {
                    curprint ( string("enum " ) +  enum_stmt->get_name().str() + " ");
                  }
                 else
                  {
                 // add qualifier of current types to the name
                    SgName nm= cdefn->get_declaration()->get_qualified_name();
                    if ( !nm.is_null() )
                       {
                         curprint ( string(nm.str()) + "::" + enum_stmt->get_name().str() + " ");
                       }
                      else
                       { 
                         curprint ( string("enum " ) + enum_stmt->get_name().str() + " ");
                       }
                  }
             }
            else
             {
               curprint ( string("enum " ) + enum_stmt->get_name().str() + " ");
             }
        }

#if 0
     printf ("info.SkipEnumDefinition()  = %s \n",info.SkipEnumDefinition()  ? "true" : "false");
     printf ("enum_stmt->get_embedded()  = %s \n",enum_stmt->get_embedded()  ? "true" : "false");
     printf ("enum_stmt->isExternBrace() = %s \n",enum_stmt->isExternBrace() ? "true" : "false");
#endif

  // DQ (6/26/2005): Support for empty enum declarations!
     if (enum_stmt == enum_stmt->get_definingDeclaration())
        {
       // printf ("In the unparser this is the Enum's defining declaration! \n");
          curprint ( string("{")); 
        }

  // if (!info.SkipDefinition()
     if (!info.SkipEnumDefinition()
    /* [BRN] 4/19/2002 --  part of the fix in unparsing var decl including enum definition */
                  || enum_stmt->get_embedded())
        {
          SgUnparse_Info ninfo(info);
          ninfo.set_inEnumDecl();
          SgInitializer *tmp_init=NULL;
          SgName tmp_name;
     //TODO wrap into a function and to be called by all
          SgInitializedNamePtrList::iterator p = enum_stmt->get_enumerators().begin();
          SgInitializedNamePtrList::iterator p_last = enum_stmt->get_enumerators().end();
          p_last --;
          for (; p!=enum_stmt->get_enumerators().end(); p++)
          {
            // Liao, 5/14/2009
            // enumerators may come from another included file
            // have to tell if it matches the current declaration's file before unparsing it!!
            // See test case: tests/CompileTests/C_test/test2009_05.c
            // TODO: still need work on mixed cases: part of elements are in the original file and others are from a header
            SgInitializedName* field = *p;
            ROSE_ASSERT(field !=NULL);
            bool isInSameFile = (field->get_file_info()->get_filename()==enum_stmt->get_file_info()->get_filename());
            if (isInSameFile)
            {
              unparseAttachedPreprocessingInfo(field, info, PreprocessingInfo::before);
              // unparse the element   
              ROSE_ASSERT((*p) != NULL);
              tmp_name=(*p)->get_name();
              tmp_init=(*p)->get_initializer();
              curprint ( tmp_name.str());
              if (tmp_init != NULL)
              {
                curprint ( string("="));
                unparseExpression(tmp_init, ninfo);
              }

              //if (p != (enum_stmt->get_enumerators().end()))
              if (p != p_last)
              {
                curprint ( string(","));
              }
 
            } // end same file
          } // end for

          if  (enum_stmt->get_enumerators().size()!=0)
            // DQ (3/17/2005): This helps handle cases such as void foo () { #include "constant_code.h" }
            unparseAttachedPreprocessingInfo(enum_stmt, info, PreprocessingInfo::inside);

#if 0
          if (!info.SkipSemiColon())
             {
            // curprint ( string(" /* output a ; */ ";
               curprint ( string(";"));
               if (enum_stmt->isExternBrace())
                  {
                    curprint ( string(" }"));
                  }
             }
#endif
       /* [BRN] 4/19/2002 -- part of fix in unparsing var decl including enum definition */
          if (enum_stmt->get_embedded())
             {
               curprint ( string(" "));
             }
          enum_stmt->set_embedded(false);
       /* [BRN] end */
        } /* if */

  // DQ (6/26/2005): Support for empty enum declarations!
     if (enum_stmt == enum_stmt->get_definingDeclaration())
        {
          curprint ( string("}"));
        }

  // DQ (6/26/2005): Moved to location after output of closing "}" from enum definition
     if (!info.SkipSemiColon())
        {
       // curprint ( string(" /* output a ; */ ";
          curprint ( string(";"));
          if (enum_stmt->isExternBrace())
             {
               curprint ( string(" }"));
             }
        }
   }

void
Unparse_ExprStmt::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgExprStatement* expr_stmt = isSgExprStatement(stmt);
     ROSE_ASSERT(expr_stmt != NULL);

     SgUnparse_Info newinfo(info);

  // Expressions are another place where a class definition should NEVER be unparsed
  // DQ (5/23/2007): Note that statement expressions can have class definition
  // (so they are exceptions, see test2007_51.C).
     newinfo.set_SkipClassDefinition();

  // if (expr_stmt->get_the_expr())
     if (expr_stmt->get_expression())
        {
       // printDebugInfo(getSgVariant(expr_stmt->get_the_expr()->variant()), true); 
       // unparseExpression(expr_stmt->get_the_expr(), newinfo);
          unparseExpression(expr_stmt->get_expression(), newinfo);
        }
       else
          assert(false);

     if (newinfo.inVarDecl())
        {
          curprint ( string(","));
        }
       else
          if (!newinfo.inConditional() && !newinfo.SkipSemiColon())
             {
               curprint ( string(";"));
             }
   }

void Unparse_ExprStmt::unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgLabelStatement* label_stmt = isSgLabelStatement(stmt);
     ROSE_ASSERT(label_stmt != NULL);

     curprint ( string(label_stmt->get_label().str()) + ":");

  // DQ (3/15/2006): Remove the "/* empty statement */" comment (leave the ";").
  // DQ (10/20/2005): Unparse an empty statement with each label
  // curprint ( string(" /* empty statement */ ;";

  // DQ (3/18/2006): I don't think we need this and if we do then we need an example of where we need it.
  // test2005_164.C demonstrates that we need the ";" if the label is the last statment in the block.
  // EDG is more accepting and does not require a ";" for a label appearing at the end of the block,
  // but g++ is particular on this subtle point.  So we should make the unparser figure this out.
  // curprint ( string(" ;";

  // DQ (10/25/2007): Modified handling of labels so that they explicitly include a scope set to the 
  // SgFunctionDefinition. SgLabelSymbol objects are also now placed into the SgFunctionDefinition's 
  // symbol table.
  // SgScopeStatement* scope = label_stmt->get_scope();
     SgScopeStatement* scope = isSgScopeStatement(label_stmt->get_parent());
     ROSE_ASSERT(scope != NULL);

  // In C and C++, labels can't be places where only declarations are allowed so we know to get the 
  // Statement list instead of the declaration list.
     ROSE_ASSERT(scope->containsOnlyDeclarations() == false);
     SgStatementPtrList & statementList = scope->getStatementList();

  // Find the label in the parent scope
     SgStatementPtrList::iterator positionOfLabel = find(statementList.begin(),statementList.end(),label_stmt);

  // Verify that we found the label in the scope
  // ROSE_ASSERT(positionOfLabel != SgStatementPtrList::npos);
     if (positionOfLabel == statementList.end())
        {
          printf ("Found label = %p = %s at end of scope! \n",label_stmt,label_stmt->get_label().str());
          label_stmt->get_startOfConstruct()->display("positionOfLabel == statementList.end()");
          ROSE_ASSERT(positionOfLabel != statementList.end());
        }
       else
        {
       // ROSE_ASSERT(positionOfLabel != statementList.end());
          ROSE_ASSERT(*positionOfLabel == label_stmt);

       // printf ("label_stmt->get_label() = %p = %s \n",label_stmt,label_stmt->get_label().str());

       // Increment past the label to see what is next (but don't count SgNullStatements)
          positionOfLabel++;
          while ( ( positionOfLabel != statementList.end() ) && ( (*positionOfLabel)->variantT() == V_SgNullStatement ) )
             {
            // printf ("Found a SgNullStatement (skipping over it) *positionOfLabel = %p = %s \n",*positionOfLabel,(*positionOfLabel)->class_name().c_str());
               positionOfLabel++;
             }

       // If we are at the end (just past the last statement) then we need the ";" 
          if (positionOfLabel == statementList.end())
             {
               curprint ( string(";"));
             }
        }
   }

void
Unparse_ExprStmt::unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
  ROSE_ASSERT(while_stmt != NULL);

  curprint ( string("while" ) + "(");
  info.set_inConditional();
  
  unparseStatement(while_stmt->get_condition(), info);
  info.unset_inConditional();
  curprint ( string(")"));
  if(while_stmt->get_body()) {
    unp->cur.format(while_stmt->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
    unparseStatement(while_stmt->get_body(), info);
    unp->cur.format(while_stmt->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
  }
  else if (!info.SkipSemiColon()) { curprint ( string(";")); }

}

void
Unparse_ExprStmt::unparseDoWhileStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgDoWhileStmt* dowhile_stmt = isSgDoWhileStmt(stmt);
  ROSE_ASSERT(dowhile_stmt != NULL);

  curprint ( string("do "));
  unp->cur.format(dowhile_stmt->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
  unparseStatement(dowhile_stmt->get_body(), info);
  unp->cur.format(dowhile_stmt->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
  curprint ( string("while " ) + "(");
  SgUnparse_Info ninfo(info);
  ninfo.set_inConditional();

  //we need to keep the properties of the prevnode (The next prevnode will set the
  //line back to where "do" was printed) 
// SgLocatedNode* tempnode = prevnode;

  unparseStatement(dowhile_stmt->get_condition(), ninfo);
  ninfo.unset_inConditional();
  curprint ( string(")")); 
  if (!info.SkipSemiColon()) { curprint ( string(";")); }
}

void
Unparse_ExprStmt::unparseSwitchStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt);
  
     ROSE_ASSERT(switch_stmt != NULL);

     curprint ( string("switch("));
  // unparseExpression(switch_stmt->get_item_selector(), info);
     
     SgUnparse_Info ninfo(info);
     ninfo.set_SkipSemiColon();
     ninfo.set_inConditional();
     unparseStatement(switch_stmt->get_item_selector(), ninfo);
     curprint ( string(")"));

  // DQ (11/5/2003): Support for skipping basic block added to support 
  //                 prefix generation for AST Rewrite Mechanism
  // if(switch_stmt->get_body())
     if ( (switch_stmt->get_body() != NULL) && !info.SkipBasicBlock())
          unparseStatement(switch_stmt->get_body(), info);
   }

void
Unparse_ExprStmt::unparseCaseStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCaseOptionStmt* case_stmt = isSgCaseOptionStmt(stmt);
     ROSE_ASSERT(case_stmt != NULL);

     curprint ( string("case "));
     unparseExpression(case_stmt->get_key(), info);
     curprint ( string(":"));

  // if(case_stmt->get_body())
     if ( (case_stmt->get_body() != NULL) && !info.SkipBasicBlock())
          unparseStatement(case_stmt->get_body(), info);
   }

void
Unparse_ExprStmt::unparseTryStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTryStmt* try_stmt = isSgTryStmt(stmt);
     ROSE_ASSERT(try_stmt != NULL);

     curprint ( string("try "));
  
     unp->cur.format(try_stmt->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
     unparseStatement(try_stmt->get_body(), info);
     unp->cur.format(try_stmt->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
  
     SgStatementPtrList::iterator i=try_stmt->get_catch_statement_seq().begin();
     while (i != try_stmt->get_catch_statement_seq().end())
        {
          unparseStatement(*i, info);
          i++;
        }
   }

void
Unparse_ExprStmt::unparseCatchStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCatchOptionStmt* catch_statement = isSgCatchOptionStmt(stmt);
     ROSE_ASSERT(catch_statement != NULL);

     curprint ( string("catch " ) + "(");
     if (catch_statement->get_condition())
        {
          SgUnparse_Info ninfo(info);
          ninfo.set_inVarDecl();

       // DQ (5/6/2004): this does not unparse correctly if the ";" is included
          ninfo.set_SkipSemiColon();
          ninfo.set_SkipClassSpecifier();
          unparseStatement(catch_statement->get_condition(), ninfo);
        }

     curprint ( string(")"));
  // if (catch_statement->get_condition() == NULL) prevnode = catch_statement;

     unp->cur.format(catch_statement->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
     unparseStatement(catch_statement->get_body(), info);
     unp->cur.format(catch_statement->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
   }

void
Unparse_ExprStmt::unparseDefaultStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDefaultOptionStmt* default_stmt = isSgDefaultOptionStmt(stmt);
     ROSE_ASSERT(default_stmt != NULL);

     curprint ( string("default:"));
  // if(default_stmt->get_body()) 
     if ( (default_stmt->get_body() != NULL) && !info.SkipBasicBlock())
          unparseStatement(default_stmt->get_body(), info);
   }

void
Unparse_ExprStmt::unparseBreakStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgBreakStmt* break_stmt = isSgBreakStmt(stmt);
  ROSE_ASSERT(break_stmt != NULL);

  curprint ( string("break; "));
}

void
Unparse_ExprStmt::unparseContinueStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgContinueStmt* continue_stmt = isSgContinueStmt(stmt);
  ROSE_ASSERT(continue_stmt != NULL);

  curprint ( string("continue; "));
}

void
Unparse_ExprStmt::unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgReturnStmt* return_stmt = isSgReturnStmt(stmt);
     ROSE_ASSERT(return_stmt != NULL);

     curprint ( string("return "));
     SgUnparse_Info ninfo(info);

     if (return_stmt->get_expression())
        {
          unparseExpression(return_stmt->get_expression(), ninfo);
        }

     if (!ninfo.SkipSemiColon())
        {
          curprint ( string(";"));
        }
   }

void
Unparse_ExprStmt::unparseGotoStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgGotoStatement* goto_stmt = isSgGotoStatement(stmt);
  ROSE_ASSERT(goto_stmt != NULL);

  curprint ( string("goto " ) + goto_stmt->get_label()->get_label().str());
  if (!info.SkipSemiColon()) { curprint ( string(";")); }
}

static bool
isOutputAsmOperand(SgAsmOp* asmOp)
   {
  // There are two way of evaluating if an SgAsmOp is an output operand, 
  // depending of if we are using the specific mechanism that knows 
  // records register details or the more general mechanism that records 
  // the registers as strings.  The string based mechanism lack precision 
  // and would require parsing to retrive the instruction details, but it 
  // is instruction set independent.  The more precise mechanism records 
  // the specific register codes and could in the future be interpreted
  // to be a part of the binary analysis support in ROSE.

     return (asmOp->get_recordRawAsmOperandDescriptions() == true) ? (asmOp->get_isOutputOperand() == true) : (asmOp->get_modifiers() & SgAsmOp::e_output);
   }
                


void
Unparse_ExprStmt::unparseAsmStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function is called as part of the handling of the C "asm" 
  // statement.  The "asm" statement supports inline assembly in C.
  // These sorts of statements are not common in most user code 
  // (except embedded code), but are common in many system header files.

     SgAsmStmt* asm_stmt = isSgAsmStmt(stmt);
     ROSE_ASSERT(asm_stmt != NULL);

#define ASM_DEBUGGING 0

     SgSourceFile* sourceFile = TransformationSupport::getSourceFile(stmt);
     ROSE_ASSERT(sourceFile != NULL);

  // DQ (1/10/2009): The C language ASM statements are providing significant trouble, they are
  // frequently machine specific and we are compiling then on architectures for which they were 
  // not designed.  This option allows then to be read, constructed in the AST to support analysis
  // but not unparsed in the code given to the backend compiler, since this can fail. (See 
  // test2007_20.C from Linux Kernel for an example).
     if (sourceFile->get_skip_unparse_asm_commands() == true)
        {
       // This is a case were we skip the unparsing of the C language ASM statements, because while 
       // we can read then into the AST to support analysis, we can not always output them correctly.  
       // This subject requires additional work.

          printf ("Warning: Sorry, C language ASM statement skipped (parsed and built in AST, but not output by the code generation phase (unparser)) \n");

          curprint ( "/* C language ASM statement skipped (in code generation phase) */ ");
          return;
        }

  // Output the "asm" keyword.
     curprint ( string("asm "));

  // DQ (7/23/2006): Added support for volatile as modifier.
     if (asm_stmt->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isVolatile())
        {
          curprint ( string("volatile "));
        }

     curprint ( string("("));

  // DQ (7/22/2006): This IR node has been changed to have a list of SgAsmOp IR nodes
  // unparseExpression(asm_stmt->get_expr(), info);

  // printf ("unparsing asm statement = %ld \n",asm_stmt->get_operands().size());
  // Process the asm template (always the first operand)
     string asmTemplate = asm_stmt->get_assemblyCode();
     curprint("\"" + escapeString(asmTemplate) + "\"");

     if (asm_stmt->get_useGnuExtendedFormat())
        {
          size_t numOutputOperands = 0;
          size_t numInputOperands  = 0;

       // Count the number of input vs. output operands
          for (SgExpressionPtrList::const_iterator i = asm_stmt->get_operands().begin(); i != asm_stmt->get_operands().end(); ++i)
             {
               SgAsmOp* asmOp = isSgAsmOp(*i);
               ROSE_ASSERT (asmOp);
#if ASM_DEBUGGING
               printf ("asmOp->get_modifiers() = %d SgAsmOp::e_output = %d asmOp->get_isOutputOperand() = %s \n",(int)asmOp->get_modifiers(),(int)SgAsmOp::e_output,asmOp->get_isOutputOperand() ? "true" : "false");
               printf ("asmOp->get_recordRawAsmOperandDescriptions() = %s \n",asmOp->get_recordRawAsmOperandDescriptions() ? "true" : "false");
#endif
            // if (asmOp->get_modifiers() & SgAsmOp::e_output)
            // if ( (asmOp->get_modifiers() & SgAsmOp::e_output) || (asmOp->get_isOutputOperand() == true) )
               if ( isOutputAsmOperand(asmOp) == true )
                  {
                    ++numOutputOperands;
#if ASM_DEBUGGING
                    printf ("Marking as an output operand! \n");
#endif
                  }
                 else
                  {
                    ++numInputOperands;
#if ASM_DEBUGGING
                    printf ("Marking as an input operand! \n");
#endif
                  }
             }

          size_t numClobbers = asm_stmt->get_clobberRegisterList().size();

#if ASM_DEBUGGING
          printf ("numOutputOperands = %zu numInputOperands = %zu numClobbers = %zu \n",numOutputOperands,numInputOperands,numClobbers);
#endif

          bool first;
          if (numInputOperands == 0 && numOutputOperands == 0 && numClobbers == 0)
             {
               goto donePrintingConstraints;
             }
          curprint(" : "); // Start of output operands

#if ASM_DEBUGGING
          curprint(" /* asm output operands */ "); // Debugging output
#endif

       // Record if this is the first operand so that we can surpress the "," 
          first = true;
          for (SgExpressionPtrList::const_iterator i = asm_stmt->get_operands().begin(); i != asm_stmt->get_operands().end(); ++i)
             {
               SgAsmOp* asmOp = isSgAsmOp(*i);
               ROSE_ASSERT (asmOp != NULL);
            // if (asmOp->get_modifiers() & SgAsmOp::e_output)
            // if ( (asmOp->get_modifiers() & SgAsmOp::e_output) || (asmOp->get_isOutputOperand() == true) )
               if ( isOutputAsmOperand(asmOp) == true )
                  {
                    if (!first)
                         curprint(", ");
                    first = false;
                    unparseExpression(asmOp, info);
                  }
             }

          if (numInputOperands == 0 && numClobbers == 0)
             {
               goto donePrintingConstraints;
             }
          curprint(" : "); // Start of input operands
#if ASM_DEBUGGING
          curprint(" /* asm input operands */ "); // Debugging output
#endif
          first = true;
          for (SgExpressionPtrList::const_iterator i = asm_stmt->get_operands().begin(); i != asm_stmt->get_operands().end(); ++i)
             {
               SgAsmOp* asmOp = isSgAsmOp(*i);
               ROSE_ASSERT (asmOp != NULL);
            // if (!(asmOp->get_modifiers() & SgAsmOp::e_output))
               if ( isOutputAsmOperand(asmOp) == false )
                  {
                    if (!first)
                         curprint(", ");
                    first = false;
                    unparseExpression(asmOp, info);
                  }
             }

          if (numClobbers == 0)
               goto donePrintingConstraints;

          curprint(" : "); // Start of clobbers

#if ASM_DEBUGGING
          curprint(" /* asm clobbers */ "); // Debugging output
#endif
          first = true;
          for (SgAsmStmt::AsmRegisterNameList::const_iterator i = asm_stmt->get_clobberRegisterList().begin(); i != asm_stmt->get_clobberRegisterList().end(); ++i)
             {
               if (!first) curprint(", ");
               first = false;
               curprint("\"" + unparse_register_name(*i) + "\"");
             }

donePrintingConstraints: {}

        }

     curprint ( string(")"));

     if (!info.SkipSemiColon())
        {
          curprint ( string(";"));
        }
   }


#if 0
// DQ (8/13/2007): this function is not used!

//never seen this function called yet
void
Unparse_ExprStmt::unparseSpawnStmt(SgStatement* stmt, SgUnparse_Info& info) {
  SgSpawnStmt* spawn_stmt = isSgSpawnStmt(stmt);
  ROSE_ASSERT(spawn_stmt != NULL);
  
  curprint ( string("spawn "));
  
  unparseExpression(spawn_stmt->get_the_func(), info);
  if (!info.SkipSemiColon()) curprint ( string(";"));
}
#endif

#if 0

void
Unparse_ExprStmt::unparseQualifiedNameList (const SgQualifiedNamePtrList & qualifiedNameList)
   {
     printf ("Error: inside of Unparse_ExprStmt::unparseQualifiedNameList \n");
     ROSE_ASSERT(false);

  // DQ (8/26/2005): Test of global scope was explicitly qualified (is really only liked to is 
  // qualification needed in EDG) so we might have to fix this better later once we store all 
  // explicitly names qualifiers.
  // This is copy by value (we mght do something better if ROSETTA could generate 
  // SgQualifiedNamePtrList qualifiedNameList = qualifiedNameType->get_qualifiedNameList();
     SgQualifiedNamePtrList::const_iterator i = qualifiedNameList.begin();
     while(i != qualifiedNameList.end())
        {
          ROSE_ASSERT(*i != NULL);
          ROSE_ASSERT(isSgQualifiedName(*i) != NULL);
          ROSE_ASSERT((*i)->get_scope() != NULL);

       // printf ("In unparseQualifiedNameList: Found a qualified name i = %p = %s \n",(*i)->get_scope(),(*i)->get_scope()->class_name().c_str());

          if (isSgGlobal((*i)->get_scope()) != NULL)
             {
            // printf ("In unparseQualifiedNameList(): Output the global scope qualifier \n");
            // curprint ( string("::";
               curprint ( string(":: /* from name qualification */ "));
             }
            else
             {
            // Use the generated name until we are ready to select between generated or stored qualified names
            // printf ("Use the generated name until we are ready to select between generated or stored qualified names \n");

               printf ("There should only be SgGlobal IR nodes in this list (later this will change). \n");
               ROSE_ASSERT(false);
             }
          i++;
        }
   }
#endif

void
Unparse_ExprStmt::unparseTypeDefStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTypedefDeclaration* typedef_stmt = isSgTypedefDeclaration(stmt);
     ROSE_ASSERT(typedef_stmt != NULL);

  // printf ("In unp->u_type->unparseTypeDefStmt() = %p \n",typedef_stmt);
  // curprint ( string("\n /* In unp->u_type->unparseTypeDefStmt() */ \n";

  // DQ (10/5/2004): This is the explicitly set boolean value which indicates that a class declaration is buried inside
  // the current variable declaration (e.g. struct A { int x; } a;).  In this case we have to output the base type with
  // its definition.
     bool outputTypeDefinition = typedef_stmt->get_typedefBaseTypeContainsDefiningDeclaration();
  // printf ("outputTypeDefinition = %s \n",(outputTypeDefinition == true) ? "true" : "false");

     if (!info.inEmbeddedDecl())
        {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* NOT an embeddedDeclaration */ \n"));
#endif
          SgClassDefinition *cdefn = isSgClassDefinition(typedef_stmt->get_parent());
          if (cdefn && cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
               info.set_CheckAccess();
       // printDebugInfo("entering unp->u_sage->printSpecifier", true);
          unp->u_sage->printSpecifier(typedef_stmt, info);
          info.unset_CheckAccess();
        }
       else
        {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* Found an embeddedDeclaration */ \n"));
#endif
        }
     
     SgUnparse_Info ninfo(info);

  // DQ (10/10/2006): Do output any qualified names (particularly for non-defining declarations).
  // ninfo.set_forceQualifiedNames();

  // DQ (10/5/2004): This controls the unparsing of the class definition
  // when unparsing the type within this variable declaration.
     if (outputTypeDefinition == true)
        {
       // printf ("Output the full definition as a basis for the typedef base type \n");
       // DQ (10/5/2004): If this is a defining declaration then make sure that we don't skip the definition
          ROSE_ASSERT(ninfo.SkipClassDefinition() == false);

       // DQ (12/22/2005): Enum definition should be handled here as well
          ROSE_ASSERT(ninfo.SkipEnumDefinition() == false);

       // DQ (10/14/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
          ninfo.set_SkipQualifiedNames();
       // curprint ( string("\n/* Case of typedefs for outputTypeDefinition == true  */\n ";
        }
       else
        {
       // printf ("Skip output of the full definition as a basis for the typedef base type \n");
       // DQ (10/5/2004): If this is a non-defining declaration then skip the definition
          ninfo.set_SkipClassDefinition();
          ROSE_ASSERT(ninfo.SkipClassDefinition() == true);

       // DQ (12/22/2005): Enum definition should be handled here as well
          ninfo.set_SkipEnumDefinition();
          ROSE_ASSERT(ninfo.SkipEnumDefinition() == true);

       // DQ (10/14/2006): Force output any qualified names (particularly for non-defining declarations).
       // This is a special case for types of variable declarations.
       // ninfo.set_forceQualifiedNames();
       // curprint ( string("\n/* Case of typedefs, should we forceQualifiedNames -- outputTypeDefinition == false  */\n ";
        }

  // Note that typedefs of function pointers and member function pointers 
  // are quite different from ordinary typedefs and so should be handled
  // separately.

  // First look for a pointer to a function
     SgPointerType* pointerToType = isSgPointerType(typedef_stmt->get_base_type());

     SgFunctionType* functionType = NULL;
     if (pointerToType != NULL)
          functionType = isSgFunctionType(pointerToType->get_base_type());

  // SgPointerMemberType* pointerToMemberType = isSgPointerMemberType(typedef_stmt->get_base_type());

  // DQ (9/15/2004): Added to support typedefs of member pointers
     SgMemberFunctionType* pointerToMemberFunctionType = isSgMemberFunctionType(typedef_stmt->get_base_type());

#if 0
     printf ("In unp->u_type->unparseTypedef: functionType                = %p \n",functionType);
  // printf ("In unp->u_type->unparseTypedef: pointerToMemberType         = %p \n",pointerToMemberType);
     printf ("In unp->u_type->unparseTypedef: pointerToMemberFunctionType = %p \n",pointerToMemberFunctionType);
#endif

  // DQ (9/22/2004): It is not clear why we need to handle this case with special code.
  // We are only putting out the return type of the function type (for functions or member functions).
  // It seems that the reason we handle function pointers separately is that typedefs of non function 
  // pointers could include the complexity of class declarations with definitions and separating the 
  // code for function pointers allows for easier debugging.  When typedefs of defining class 
  // declarations is fixed we might be able to unify these separate cases.

  // This handles pointers to functions and member function (but not pointers to members!)
  // if ( (functionType != NULL) || (pointerToMemberType != NULL) )
     if ( (functionType != NULL) || (pointerToMemberFunctionType != NULL) )
        {
       // Newly implemented case of typedefs for function and member function pointers
       // printf ("case of typedefs for function and member function pointers \n");
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* Case of typedefs for function and member function pointers */ \n"));
#endif
          ninfo.set_SkipFunctionQualifier();
          curprint ( string("typedef "));

       // Specify that only the first part of the type shold be unparsed 
       // (this will permit the introduction of the name into the member
       // function pointer declaration)
          ninfo.set_isTypeFirstPart();

#if OUTPUT_DEBUGGING_UNPARSE_INFO
          curprint ( string("\n/* " ) + ninfo.displayString("After return Type now output the base type (first part then second part)") + " */ \n");
#endif

       // The base type contains the function po9inter type
          SgType *btype = typedef_stmt->get_base_type();
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* Output base type (first part) */ \n"));
#endif

       // DQ (1/10/2007): Set the current declaration statement so that if required we can do
       // context dependent searches of the AST to determine if name qualification is required.
       // This is done now for the case of function and member function typedefs.
          SgUnparse_Info ninfo_for_type(ninfo);
          ninfo_for_type.set_declstatement_ptr(typedef_stmt);

       // Only pass the ninfo_for_type to support name qualification of the base type.
       // unp->u_type->unparseType(btype, ninfo);
          unp->u_type->unparseType(btype, ninfo_for_type);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* Done: Output base type (first part) */ \n"));
#endif

          curprint ( typedef_stmt->get_name().str());

       // Now unparse the second part of the typedef
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* Output base type (second part) */ \n"));
#endif

          ninfo.set_isTypeSecondPart();
          unp->u_type->unparseType(btype, ninfo);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* Done: Output base type (second part) */ \n"));
#endif

          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
        }
       else
        {
       // previously implemented case of unparsing the typedef does not handle 
       // function pointers properly (so they are handled explicitly above!)

       // printf ("Not a typedef for a function type or member function type \n");
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* Output a non function pointer typedef (Not a typedef for a function type or member function type) */ \n"));
#endif

          ninfo.set_SkipFunctionQualifier();
          curprint ( string("typedef "));

          ninfo.set_SkipSemiColon();
          SgType *btype = typedef_stmt->get_base_type();

          ninfo.set_isTypeFirstPart();

       // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
       // ninfo.set_SkipQualifiedNames();
       // curprint ( string("\n/* Commented out call to ninfo.set_SkipQualifiedNames() */\n ";

       // printf ("Before first part of base type (type = %p = %s) \n",btype,btype->sage_class_name());
       // ninfo.display ("Before first part of type in unp->u_type->unparseTypeDefStmt()");

          SgUnparse_Info ninfo_for_type(ninfo);

       // DQ (1/10/2007): Set the current declaration statement so that if required we can do
       // context dependent searches of the AST to determine if name qualification is required.
          ninfo_for_type.set_declstatement_ptr(NULL);
          ninfo_for_type.set_declstatement_ptr(typedef_stmt);

          if (typedef_stmt->get_requiresGlobalNameQualificationOnType() == true)
             {
#if 0
               printf ("In Unparse_ExprStmt::unp->u_type->unparseTypedefStmt(): This base type requires a global qualifier \n");
               curprint ( string("\n/* This base type requires a global qualifier, calling set_requiresGlobalNameQualification() */ \n"));
#endif
            // ninfo_for_type.set_forceQualifiedNames();
               ninfo_for_type.set_requiresGlobalNameQualification();
             }
#if 0
#if 0
       // DQ (8/23/2006): This was used to specify global qualification separately from the more general name 
       // qualification mechanism, however having two mechanisms is a silly level of redundancy so we now use 
       // just one (the more general one) even though it is only used for global name qualification.
       // DQ (8/20/2006): We can't mark the SgType (since it is shared), and we can't mark the SgInitializedName,
       // so we have to carry the information that we should mark the type in the SgVariableDeclaration.
       // printf ("vardecl_stmt->get_requiresNameQualification() = %s \n",vardecl_stmt->get_requiresNameQualification() ? "true" : "false");
          if (typedef_stmt->get_requiresGlobalNameQualificationOnType() == true)
             {
            // Output the name qualification for the type in the variable declaration.
            // But we have to do so after any modifiers are output, so in unp->u_type->unparseType().
               printf ("In Unparse_ExprStmt::unp->u_type->unparseTypedefStmt(): This base type requires a global qualifier \n");
            // ninfo2.set_forceQualifiedNames();
               ninfo_for_type.set_requiresGlobalNameQualification();

           // ninfo_for_type.display("This base type requires a global qualifier");
             }
#else
       // DQ (1/10/2007): Actually we can't do this since test2007_15.C demonstrates where 
       // for the __FILE_IO in a typedef is context sensitive as to if it requires or accepts 
       // name qualification.
       // DQ (1/10/2007): I think we want to force the use of qualified names generally 
       // (over qualification can't be avoided since it is sometime required).
       // printf ("SKIP Forcing the use of qualified names for the base type of typedefs (independent of the setting of the typedef_stmt->get_requiresGlobalNameQualificationOnType() \n");
       // ninfo_for_type.set_forceQualifiedNames();
#endif
#endif

#if 0
       // DQ (1/10/2007): If this is C++ then we can drop the class specifier (and it is good 
       // to do so to avoid having inappropriate name qualification cause generation of new 
       // types in the generated code which masks errors we want to trap).
          if (SageInterface::is_Cxx_language() == true)
             {
            // BUG: Currently we can't do this because the information in EDG is unavailable as to 
            // when the class specifier is used. So we have to always output it in the generated code.
            // At worst this appears to only mask errors in the name qualification of base types for 
            // typedefs. Eventually we want to fix this.
               ninfo_for_type.set_SkipClassSpecifier();
             }
#endif

       // curprint ( string("\n/* unp->u_type->unparseTypeDefStmt: Before first part of type */ \n";
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* Output base type (first part) */ \n"));
#endif

       // unp->u_type->unparseType(btype, ninfo);
          unp->u_type->unparseType(btype, ninfo_for_type);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* Done: Output base type (second part) */ \n"));
#endif

       // curprint ( string("\n/* unp->u_type->unparseTypeDefStmt: After first part of type */ \n";
       // printf ("After first part of type \n");

       // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
       // ninfo.unset_SkipQualifiedNames();

       // DQ (10/7/2004): Moved the output of the name to before the output of the second part of the type
       // to handle the case of "typedef A* A_Type[10];" (see test2004_104.C).

       // The name of the type (X, in the following example) has to appear after the 
       // declaration. Example: struct { int a; } X;
               curprint ( typedef_stmt->get_name().str());

          ninfo.set_isTypeSecondPart();

       // printf ("Before 2nd part of type \n");
       // curprint ( string("\n/* unp->u_type->unparseTypeDefStmt: Before second part of type */ \n";
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* Output base type (second part) */ \n"));
#endif

          unp->u_type->unparseType(btype, ninfo);
       // unp->u_type->unparseType(btype, ninfo_for_type);

#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
          curprint ( string("\n/* Done: Output base type (second part) */ \n"));
#endif
       // curprint ( string("\n/* unp->u_type->unparseTypeDefStmt: After second part of type */ \n";
       // printf ("After 2nd part of type \n");

          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
        }

  // info.display ("At base of unp->u_type->unparseTypeDefStmt()");
   }

// never seen this function called yet
void
Unparse_ExprStmt::unparseTemplateDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // By commenting this out we avoid the template declaration 
  // that is placed at the top of any A++ application.

     SgTemplateDeclaration* template_stmt = isSgTemplateDeclaration(stmt);
     ROSE_ASSERT(template_stmt != NULL);

  // printf ("In unparseTemplateDeclStmt(template_stmt = %p) \n",template_stmt);
  // template_stmt->get_declarationModifier().display("In unparseTemplateDeclStmt()");

  // SgUnparse_Info ninfo(info);

  // Check to see if this is an object defined within a class
     ROSE_ASSERT (template_stmt->get_parent() != NULL);
     SgClassDefinition *cdefn = isSgClassDefinition(template_stmt->get_parent());
     if (cdefn != NULL)
        {
          if (cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
             {
               info.set_CheckAccess();
             }
        }

  // SgUnparse_Info saved_ninfo(ninfo);
  // this call has been moved below, after we indent
  // unp->u_sage->printSpecifier2(vardecl_stmt, ninfo);

  // Setup the SgUnparse_Info object for this statement
  // ninfo.unset_CheckAccess();
  // info.set_access_attribute(ninfo.get_access_attribute());

  // info.display("In unparseTemplateDeclStmt()");

  // Output access modifiers
     unp->u_sage->printSpecifier1(template_stmt, info);

  // printf ("template_stmt->get_string().str() = %s \n",template_stmt->get_string().str());

  // DQ (1/21/2004): Use the string class to simplify the previous version of the code
     string templateString = template_stmt->get_string().str();

  // DQ (4/29/2004): Added support for "export" keyword (not supported by g++ yet)
     if (template_stmt->get_declarationModifier().isExport())
          curprint ( string("export "));

#if 0
  // DQ (11/18/2004): Added support for qualified name of template declaration!
  // But it appears that the qualified name is included within the template text string so that 
  // we should not output the qualified name spearately!
     SgName nameQualifier = unp->u_type->unp->u_name->generateNameQualifier( template_stmt , info );
     printf ("In unparseTemplateDeclStmt(): nameQualifier (from unp->u_type->unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
  // curprint ( nameQualifier.str();
#endif

  // printf ("template_stmt->get_template_kind() = %d \n",template_stmt->get_template_kind());
     switch (template_stmt->get_template_kind())
        {
          case SgTemplateDeclaration::e_template_class :
          case SgTemplateDeclaration::e_template_m_class :
          case SgTemplateDeclaration::e_template_function :
          case SgTemplateDeclaration::e_template_m_function :
          case SgTemplateDeclaration::e_template_m_data :
             {
            // printf ("debugging 64 bit bug: templateString = %s \n",templateString.c_str());
               if (templateString.empty() == true)
                  {
                 // DQ (12/22/2006): This is typically a template member class (see test2004_128.C and test2004_138.C).
                 // It is not clear to me why the names are missing, though perhaps they have not been computed yet 
                 // (until the templated clas is instantiated)!

                 // printf ("Warning: templateString name is empty in Unparse_ExprStmt::unparseTemplateDeclStmt() \n");
                 // printf ("     template_stmt->get_template_kind() = %d \n",template_stmt->get_template_kind());
                  }
            // ROSE_ASSERT(templateString.empty() == false);

               curprint ( string("\n" ) + templateString);
               break;
             }

          case SgTemplateDeclaration::e_template_none :
            // printf ("Do we need this extra \";\"? \n");
            // curprint ( templateString + ";";
               printf ("Error: SgTemplateDeclaration::e_template_none found (not sure what to do here) \n");
               ROSE_ASSERT (false);
               break;

          default:
               printf ("Error: default reached \n");
               ROSE_ASSERT (false);
        }
   }
 
//#if USE_UPC_IR_NODES //TODO need this?
//#if UPC_EXTENSIONS_ALLOWED
 // Liao, 6/13/2008, unparsing UPC nodes in the AST
void
Unparse_ExprStmt::unparseUpcNotifyStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUpcNotifyStatement* input = isSgUpcNotifyStatement(stmt);
     ROSE_ASSERT(input != NULL);

     curprint ( string("upc_notify "));
     SgUnparse_Info ninfo(info);

     if (input->get_notify_expression())
        {
          unparseExpression(input->get_notify_expression(), ninfo);
        }

     if (!ninfo.SkipSemiColon())
        {
          curprint ( string(";"));
        }
   }

void
Unparse_ExprStmt::unparseUpcWaitStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUpcWaitStatement* input = isSgUpcWaitStatement(stmt);
     ROSE_ASSERT(input != NULL);

     curprint ( string("upc_wait "));
     SgUnparse_Info ninfo(info);

     if (input->get_wait_expression())
        {
          unparseExpression(input->get_wait_expression(), ninfo);
        }
     if (!ninfo.SkipSemiColon())
        {
          curprint ( string(";"));
        }
   }

void
Unparse_ExprStmt::unparseUpcBarrierStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUpcBarrierStatement* input = isSgUpcBarrierStatement(stmt);
     ROSE_ASSERT(input != NULL);

     curprint ( string("upc_barrier "));
     SgUnparse_Info ninfo(info);

     if (input->get_barrier_expression())
        {
          unparseExpression(input->get_barrier_expression(), ninfo);
        }

     if (!ninfo.SkipSemiColon())
        {
          curprint ( string(";"));
        }
   }

void
Unparse_ExprStmt::unparseUpcFenceStatement(SgStatement* stmt, SgUnparse_Info& info)
 {
   SgUpcFenceStatement* input = isSgUpcFenceStatement(stmt);
   ROSE_ASSERT(input != NULL);

   curprint ( string("upc_fence "));
   SgUnparse_Info ninfo(info);

   if (!ninfo.SkipSemiColon())
        curprint ( string(";"));
 }
// Liao, 6/17/2008, unparse upc_forall 
// Most code is copied from Unparse_ExprStmt::unparseForStmt()
void
Unparse_ExprStmt::unparseUpcForAllStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Unparse for loop \n");
     SgUpcForAllStatement* for_stmt = isSgUpcForAllStatement(stmt);
     ROSE_ASSERT(for_stmt != NULL);

     curprint ( string("upc_forall ("));
     SgUnparse_Info newinfo(info);
     newinfo.set_SkipSemiColon();
     newinfo.set_inConditional();  // set to prevent printing line and file information

  // curprint ( string(" /* initializer */ ";
     SgStatement *tmp_stmt = for_stmt->get_for_init_stmt();
  // ROSE_ASSERT(tmp_stmt != NULL);
     if (tmp_stmt != NULL)
        {
          unparseStatement(tmp_stmt,newinfo);
        }
       else
        {
          printf ("Warning in unparseForStmt(): for_stmt->get_for_init_stmt() == NULL \n");
          curprint ( string("; "));
        }
     newinfo.unset_inConditional();

#if 0
     SgExpression *tmp_expr = NULL;
     if ( (tmp_expr = for_stmt->get_test_expr()))
          unparseExpression(tmp_expr, info);
#else
  // DQ (12/13/2005): New code for handling the test (which could be a declaration!)
  // printf ("Output the test in the for statement format newinfo.inConditional() = %s \n",newinfo.inConditional() ? "true" : "false");
  // curprint ( string(" /* test */ ";
     SgStatement *test_stmt = for_stmt->get_test();
     ROSE_ASSERT(test_stmt != NULL);
  // if ( test_stmt != NULL )
     SgUnparse_Info testinfo(info);
     testinfo.set_SkipSemiColon();
     testinfo.set_inConditional();
  // printf ("Output the test in the for statement format testinfo.inConditional() = %s \n",testinfo.inConditional() ? "true" : "false");
     unparseStatement(test_stmt, testinfo);
#endif
     curprint ( string("; "));

  // curprint ( string(" /* increment */ ";
  // SgExpression *increment_expr = for_stmt->get_increment_expr();
     SgExpression *increment_expr = for_stmt->get_increment();
     ROSE_ASSERT(increment_expr != NULL);
     if ( increment_expr != NULL )
          unparseExpression(increment_expr, info);

     curprint ( string("; "));
  // Liao, unparse the affinity expression
     SgExpression * affinity_expr = for_stmt->get_affinity();
     ROSE_ASSERT(affinity_expr != NULL);
     SgExpression * null_expr = isSgNullExpression(affinity_expr);
     if (null_expr)
       curprint (string("continue"));
     else
       unparseExpression(affinity_expr, info); 
     curprint ( string(") "));
   // Added support to output the header without the body to support the addition 
  // of more context in the prefix used with the AST Rewrite Mechanism.
  // if ( (tmp_stmt = for_stmt->get_loop_body()) )

     SgStatement* loopBody = for_stmt->get_loop_body();
     ROSE_ASSERT(loopBody != NULL);

  // if ( (tmp_stmt = for_stmt->get_loop_body()) && !info.SkipBasicBlock())
     if ( (loopBody != NULL) && !info.SkipBasicBlock())
        {
          unp->cur.format(loopBody, info, FORMAT_BEFORE_NESTED_STATEMENT);
          unparseStatement(loopBody, info);
          unp->cur.format(loopBody, info, FORMAT_AFTER_NESTED_STATEMENT);
        }
       else
        {
          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
        }
   }
// OpenMP support 
void Unparse_ExprStmt::unparseOmpPrefix(SgUnparse_Info& info)
{
  curprint(string ("#pragma omp "));
}


void Unparse_ExprStmt::unparseOmpForStatement (SgStatement* stmt,     SgUnparse_Info& info)
{
  ROSE_ASSERT (stmt != NULL);
  SgOmpForStatement * f_stmt = isSgOmpForStatement (stmt);
  ROSE_ASSERT (f_stmt != NULL);

  unparseOmpDirectivePrefixAndName(stmt, info);

  unparseOmpBeginDirectiveClauses(stmt, info);
  // TODO a better way to new line? and add indentation 
  curprint (string ("\n"));

  SgUnparse_Info ninfo(info);
  if (f_stmt->get_body())
  {
    unparseStatement(f_stmt->get_body(), ninfo);
  }
  else
  {
    cerr<<"Error: empty body for:"<<stmt->class_name()<<" is not allowed!"<<endl;
    ROSE_ASSERT(false);
  }
}

void
Unparse_ExprStmt::unparseOmpBeginDirectiveClauses (SgStatement* stmt,     SgUnparse_Info& info)
{
  ROSE_ASSERT (stmt != NULL);
  // optional clauses
  if (isSgOmpClauseBodyStatement(stmt))
  {
    const SgOmpClausePtrList& clause_ptr_list = isSgOmpClauseBodyStatement(stmt)->get_clauses();
    SgOmpClausePtrList::const_iterator i;
    for (i= clause_ptr_list.begin(); i!= clause_ptr_list.end(); i++)
    {
      SgOmpClause* c_clause = *i;
      unparseOmpClause(c_clause, info);
    }
  }
}

 // EOF



