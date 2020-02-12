// tps (01/14/2010) : Switching from rose.h to sage3
// test cases are put into tests/nonsmoke/functional/roseTests/astInterfaceTests
// Last modified, by Liao, Jan 10, 2008

// includes "sageBuilder.h"
#include "sage3basic.h"

#include <rose_config.h>

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "roseAdapter.h"
   #include "markLhsValues.h"
// #include "sageBuilder.h"
   #include <fstream>
   #include <boost/algorithm/string/trim.hpp>
   #include <boost/foreach.hpp>
   #include "Outliner.hh"
#else
// #include "sageBuilder.h"
   #include <fstream>
   #include <boost/algorithm/string/trim.hpp>
   #include <boost/foreach.hpp>

   #include "transformationSupport.h"
#endif


// DQ (4/3/2012): Added so that I can enforce some rules as the AST is constructed.
#include "AstConsistencyTests.h"

// DQ (2/27/2014): We need this feature to support the function: fixupCopyOfAstFromSeparateFileInNewTargetAst()
#include "RoseAst.h"

// DQ (3/31/2012): Is this going to be an issue for C++11 use with ROSE?
#define foreach BOOST_FOREACH



// DQ (2/17/2013): This is a operation on the global AST that we don't need to do too often
// depending on the grainularity sought for the debugging information.  It is done on the
// whole AST once after construction (in edgRose.C), but is not needed more than that
// since it is a performance issue.
#define BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS 0
#define BUILDER_MAKE_REDUNDANT_CALLS_TO_SYMBOL_TABLE_LOOKUP 0

using namespace std;
using namespace Rose;
using namespace SageInterface;


namespace EDG_ROSE_Translation
   {
  // DQ (6/3/2019): The case of outlining to a seperate file will have transformations
  // that this checking will fail on because it is for the typical case of checking the
  // AST for transformations after construction of the AST from an typical input file.
#if defined(ROSE_BUILD_CXX_LANGUAGE_SUPPORT) && !defined(ROSE_USE_CLANG_FRONTEND)
  // DQ (6/3/2019): Use the definition in the EDG edgRose.C file if C/C++ support IS defined.
     extern bool suppress_detection_of_transformations;
#else
  // DQ (6/3/2019): Allow this to be the definition if C/C++ support is NOT defined.
     bool suppress_detection_of_transformations;
#endif
   }

// MS 2015: utility functions used in the implementation of SageBuilder functions, but are not exposed in the SageBuilder-Interface.
namespace SageBuilder {

// DQ (3/24/2016): Adding Robb's message mechanism (data member and function).
Sawyer::Message::Facility mlog;
void
initDiagnostics()
   {
     static bool initialized = false;
     if (!initialized)
        {
          initialized = true;
          Rose::Diagnostics::initAndRegister(&mlog, "Rose::SageBuilder");
          mlog.comment("building abstract syntax trees");
        }
   }


template <class actualFunction>
actualFunction*
buildNondefiningFunctionDeclaration_T (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, bool isMemberFunction, SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, SgTemplateArgumentPtrList* templateArgumentsList, SgTemplateParameterPtrList* templateParameterList);

// DQ (8/11/2013): Note that the specification of the SgTemplateArgumentPtrList is somewhat redundant with the required parameter first_nondefinng_declaration (I think).
//! A template function for function declaration builders
template <class actualFunction>
actualFunction*
// buildDefiningFunctionDeclaration_T (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, bool isMemberFunction, SgScopeStatement* scope=NULL, SgExprListExp* decoratorList = NULL, unsigned int functionConstVolatileFlags = 0);
// buildDefiningFunctionDeclaration_T (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, bool isMemberFunction, SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, actualFunction* first_nondefinng_declaration);
buildDefiningFunctionDeclaration_T (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, bool isMemberFunction, SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, actualFunction* first_nondefinng_declaration, SgTemplateArgumentPtrList* templateArgumentsList);

//! Function to reset scopes in SgDeclarationStatement IR nodes.
// ROSE_DLL_API void resetDeclaration(SgDeclarationStatement* classDeclaration_copy, SgDeclarationStatement* classDeclaration_original);
template <class T> ROSE_DLL_API void resetDeclaration(T* classDeclaration_copy, T* classDeclaration_original, SgScopeStatement* targetScope);

}; // SageBuilder namespace

//---------------------------------------------
// scope stack interfaces
//   hide actual implementation of the stack
//---------------------------------------------

// DQ (1/18/2008): Added declaration in source file with Liao.
// std::list<SgScopeStatement*> SageBuilder::ScopeStack;
std::list<SgScopeStatement*> SageBuilder::ScopeStack(0);


// DQ (11/30/2010): Added support for building Fortran case insensitive symbol table handling.
//! Support for construction of case sensitive/insensitive symbol table handling in scopes.
bool SageBuilder::symbol_table_case_insensitive_semantics = false;


//! C++ SageBuilder namespace specific state for storage of the source code position state
// (used to control how the source code positon is defined for IR nodes built within the SageBuilder interface).
// Set the default to be to mark everything as a transformation.
// SageBuilder::SourcePositionClassification SageBuilder::SourcePositionClassificationMode = SageBuilder::e_sourcePositionError;
SageBuilder::SourcePositionClassification SageBuilder::SourcePositionClassificationMode = SageBuilder::e_sourcePositionTransformation;


//! Get the current source position classification (defines how IR nodes built by the SageBuilder interface will be classified).
SageBuilder::SourcePositionClassification
SageBuilder::getSourcePositionClassificationMode()
   {
#if 0
     printf ("In getSourcePositionClassificationMode(): returning mode = %s \n",display(SourcePositionClassificationMode).c_str());
#endif

     return SourcePositionClassificationMode;
   }

//! Set the current source position classification (defines how IR nodes built by the SageBuilder interface will be classified).
void
SageBuilder::setSourcePositionClassificationMode(SageBuilder::SourcePositionClassification X)
   {
     SourcePositionClassificationMode = X;
   }

string
SageBuilder::display(SourcePositionClassification & scp)
   {
  // DQ (11/19/2012): This function is build to support debugging the value of the statically defined mode.

     string s;
     switch(scp)
        {
          case e_sourcePositionError:                s = "e_sourcePositionError";                break;
          case e_sourcePositionDefault:              s = "e_sourcePositionDefault";              break;
          case e_sourcePositionTransformation:       s = "e_sourcePositionTransformation";       break;
          case e_sourcePositionCompilerGenerated:    s = "e_sourcePositionCompilerGenerated";    break;
          case e_sourcePositionNullPointers:         s = "e_sourcePositionNullPointers";         break;
          case e_sourcePositionFrontendConstruction: s = "e_sourcePositionFrontendConstruction"; break;
          case e_sourcePosition_last:                s = "e_sourcePosition_last";                break;

          default:
             {
               printf ("Error: default reached in SageBuilder::display(SourcePositionClassification & scp): scp = %d \n",scp);
               ROSE_ASSERT(false);
             }

        }

     return s;
   }


// DQ (5/21/2013): Added function to support hidding the implementation in the SgScopeStatement API.
// template <class T> SgFunctionSymbol* SgScopeStatement::find_symbol_by_type_of_function (const SgName & name, const SgType* func_type)
template <class T>
SgFunctionSymbol*
SgScopeStatement::find_symbol_by_type_of_function (const SgName & name, const SgType* func_type, SgTemplateParameterPtrList* templateParameterList, SgTemplateArgumentPtrList* templateArgumentsList)
   {
  // DQ (3/13/2012): This is to address the fact that there are 6 different types of functions in ROSE:
  //    1) SgFunctionDeclaration
  //    2) SgMemberFunctionDeclaration
  //    3) SgTemplateFunctionDeclaration
  //    4) SgTemplateMemberFunctionDeclaration
  //    5) SgTemplateFunctionInstntiationDeclaration
  //    6) SgTemplateMemberFunctionInstntiationDeclaration
  // And 4 different types of function symbols:
  //    1) SgFunctionSymbol
  //    2) SgMemberFunctionSymbol
  //    3) SgTemplateFunctionSymbol
  //    4) SgTemplateMemberFunctionSymbol
  // Note that both:
  //    SgTemplateFunctionInstntiationDeclaration
  //    SgTemplateMemberFunctionInstntiationDeclaration
  // map to
  //    SgFunctionSymbol
  //    SgMemberFunctionSymbol
  // respectively.

  // Check if there is a function symbol of any kind, then narrow the selection.
  // SgFunctionSymbol* func_symbol = lookup_function_symbol(name,func_type);
     SgFunctionSymbol* func_symbol = NULL;

  // if (func_symbol == NULL)
        {
       // Use the static variant as a selector.
          switch((VariantT)T::static_variant)
             {
               case V_SgFunctionDeclaration:
               case V_SgProcedureHeaderStatement:
               case V_SgTemplateInstantiationFunctionDecl:
                  {
#if 0
                 // printf ("In SgScopeStatement::find_symbol_by_type_of_function(): This is a SgFunctionDeclaration or SgTemplateInstantiationFunctionDecl function: name = %s \n",name.str());
                    printf ("In SgScopeStatement::find_symbol_by_type_of_function<%s>(): This is a SgFunctionDeclaration or SgTemplateInstantiationFunctionDecl function: name = %s \n",
                         Cxx_GrammarTerminalNames[T::static_variant].name.c_str(),name.str());
                    printf ("   --- templateArgumentsList = %p \n",templateArgumentsList);
#endif
                 // DQ (8/11/2013): Verify that the template arguments are provided for the correct cases and not for the incorrect cases.
                    if ((VariantT)T::static_variant == V_SgTemplateInstantiationFunctionDecl)
                       {
                         ROSE_ASSERT(templateArgumentsList != NULL);
                       }
                      else
                       {
                         ROSE_ASSERT(templateArgumentsList == NULL);
                       }

                 // DQ (5/21/2013): Calling the SgScopeStatement API.
                 // func_symbol = find_nontemplate_function(name,func_type);
                 // func_symbol = lookup_nontemplate_function_symbol(name,func_type);
                    func_symbol = lookup_nontemplate_function_symbol(name,func_type,templateArgumentsList);
#if 0
                 // DQ (3/20/2017): Comment this debugging code out, note that the assertion it supports in left in place.
                    if (isSgTemplateFunctionSymbol(func_symbol) != NULL)
                       {
                         printf ("ERROR: func_symbol == SgTemplateFunctionSymbol in find_symbol_by_type_of_function(): case V_SgFunctionDeclaration: \n");
                         SgFunctionDeclaration* functionDeclaration = func_symbol->get_declaration();
                         ROSE_ASSERT(functionDeclaration != NULL);
                         printf ("   --- functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());
                         ROSE_ASSERT(functionDeclaration->get_file_info() != NULL);
                         functionDeclaration->get_file_info()->display("func_symbol == SgTemplateFunctionSymbol");
                       }
#endif
                 // DQ (5/22/2013): This function symbol should not be a SgTemplateFunctionSymbol (associated with a template function.  It should be an instantiated template.
                    ROSE_ASSERT(isSgTemplateFunctionSymbol(func_symbol) == NULL);
                    break;
                  }

               case V_SgMemberFunctionDeclaration:
               case V_SgTemplateInstantiationMemberFunctionDecl:
                  {
#if 0
                    printf ("In SgScopeStatement::find_symbol_by_type_of_function(): This is a SgMemberFunctionDeclaration or SgTemplateInstantiationMemberFunctionDecl function \n");
#endif
                 // DQ (5/21/2013): there is no SgScopeStatement API that calls this function.
                 // printf ("WARNING: There is no SgScopeStatement API that calls this function: find_nontemplate_member_function(). \n");

                 // DQ (8/11/2013): Verify that the template arguments are provided for the correct cases and not for the incorrect cases.
                    if ((VariantT)T::static_variant == V_SgTemplateInstantiationMemberFunctionDecl)
                       {
                         ROSE_ASSERT(templateArgumentsList != NULL);
                       }
                      else
                       {
                         ROSE_ASSERT(templateArgumentsList == NULL);
                       }
#if 0
                 // DQ (8/11/2013): I think this should fail in cases were we should be handing the templateArgumentsList
                 // to the lookup_nontemplate_member_function_symbol() function.
                    ROSE_ASSERT(templateArgumentsList == NULL);
#endif
                 // func_symbol = find_nontemplate_member_function(name,func_type);
                 // ROSE_ASSERT(isSgNamespaceDefinitionStatement(this) == NULL);
                 // func_symbol = get_symbol_table()->find_nontemplate_member_function(name,func_type);
                 // func_symbol = lookup_nontemplate_member_function_symbol(name,func_type);
                    func_symbol = lookup_nontemplate_member_function_symbol(name,func_type,templateArgumentsList);
                    break;
                  }

               case V_SgTemplateFunctionDeclaration:
                  {
#if 0
                    printf ("In SgScopeStatement::find_symbol_by_type_of_function(): This is a SgTemplateFunctionDeclaration function \n");
#endif
                 // DQ (8/11/2013): I think this should fail in cases were we should be handing the templateArgumentsList
                 // to the lookup_template_function_symbol() function.
                    ROSE_ASSERT(templateArgumentsList == NULL);

                 // DQ (8/11/2013): I think this should always be non-null.
                    ROSE_ASSERT(templateParameterList != NULL);

                 // DQ (8/7/2013): Adding support to permit template function overloading on template parameters.
                 // Note that the template arguments are being handed in as templateSpecializationArgumentList since this is the matching list.
                 // However, we might expect template parameter.

                 // DQ (8/7/2013): Adding support for template function overloading using template parameters (info passed as template arguments for specialization).
                 // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
                 // In this case these are unavailable from this point.
                 // DQ (5/21/2013): Calling the SgScopeStatement API.
                 // func_symbol = find_template_function(name,func_type);
                 // func_symbol = lookup_template_function_symbol(name,func_type);
                 // func_symbol = lookup_template_function_symbol(name,func_type,NULL,NULL);
                 // func_symbol = lookup_template_function_symbol(name,func_type,NULL,templateArgumentsList);
                    func_symbol = lookup_template_function_symbol(name,func_type,templateParameterList);

                    break;
                  }

               case V_SgTemplateMemberFunctionDeclaration:
                  {
#if 0
                    printf ("In SgScopeStatement::find_symbol_by_type_of_function(): This is a SgTemplateMemberFunctionDeclaration function \n");
#endif
                 // DQ (8/11/2013): I think this should fail in cases were we should be handing the templateArgumentsList
                 // to the lookup_template_member_function_symbol() function.
                    ROSE_ASSERT(templateArgumentsList == NULL);

                 // DQ (8/11/2013): I think this sould always be non-null.
                    ROSE_ASSERT(templateParameterList != NULL);

                 // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
                 // In this case these are unavailable from this point.
                 // DQ (5/21/2013): Calling the SgScopeStatement API.
                 // func_symbol = find_template_member_function(name,func_type);
                 // func_symbol = lookup_template_member_function_symbol(name,func_type);
                    func_symbol = lookup_template_member_function_symbol(name,func_type,templateParameterList);
                    break;
                  }

               default:
                  {
                    printf ("In SgScopeStatement::find_symbol_by_type_of_function(): default reached --- variantT(T::static_variant) = %d \n",T::static_variant);
                    ROSE_ASSERT(false);
                  }
             }
        }

#if 0
     if (func_symbol != NULL)
          printf ("In SgScopeStatement::find_symbol_by_type_of_function(): func_symbol = %p = %s \n",func_symbol,func_symbol->class_name().c_str());
       else
          printf ("In SgScopeStatement::find_symbol_by_type_of_function(): func_symbol = %p \n",func_symbol);
#endif

  // return isSgFunctionSymbol(func_symbol);
     return func_symbol;
   }


// explicit instantiation of find_symbol_by_type_of_function
template SgFunctionSymbol* SgScopeStatement::find_symbol_by_type_of_function<SgTemplateFunctionDeclaration>(SgName const&, SgType const*, SgTemplateParameterPtrList*, SgTemplateArgumentPtrList*);
template SgFunctionSymbol* SgScopeStatement::find_symbol_by_type_of_function<SgTemplateInstantiationMemberFunctionDecl>(SgName const&, SgType const*, SgTemplateParameterPtrList*, SgTemplateArgumentPtrList*);
template SgFunctionSymbol* SgScopeStatement::find_symbol_by_type_of_function<SgTemplateInstantiationFunctionDecl>(SgName const&, SgType const*, SgTemplateParameterPtrList*, SgTemplateArgumentPtrList*);
template SgFunctionSymbol* SgScopeStatement::find_symbol_by_type_of_function<SgMemberFunctionDeclaration>(SgName const&, SgType const*, SgTemplateParameterPtrList*, SgTemplateArgumentPtrList*);
template SgFunctionSymbol* SgScopeStatement::find_symbol_by_type_of_function<SgTemplateMemberFunctionDeclaration>(SgName const&, SgType const*, SgTemplateParameterPtrList*, SgTemplateArgumentPtrList*);
template SgFunctionSymbol* SgScopeStatement::find_symbol_by_type_of_function<SgFunctionDeclaration>(SgName const&, SgType const*, SgTemplateParameterPtrList*, SgTemplateArgumentPtrList*);

void
SageBuilder::pushScopeStack (SgScopeStatement* stmt)
   {
     ROSE_ASSERT(stmt != NULL);

#if 0
  // DQ (9/28/2009): This is part of testing for GNU 4.0.x (other versions of g++ work fine).
     ROSE_ASSERT(stmt != NULL);
     if (stmt != NULL)
        {
       // Calling any member function is a way to test the pointer.
          stmt->class_name();
        }
#endif

     ScopeStack.push_back(stmt);

#if 0
  // Debugging code to output the scope stack.
     printf ("SageBuilder::pushScopeStack(): Scope stack: \n");
     int counter = 0;
     for (std::list<SgScopeStatement*>::iterator i = ScopeStack.begin(); i != ScopeStack.end(); i++)
        {
          printf ("   --- i = %d: %p = %s \n",counter,*i,(*i)->class_name().c_str());
          counter++;
        }
#endif
   }

#if 0
// DQ (3/20/2017): This function is not called (the function above is the more useful one that is used).
void SageBuilder::pushScopeStack (SgNode* node)
   {
     SgScopeStatement* stmt = isSgScopeStatement(node);

  // DQ (9/26/2012): Added assertion.
     ROSE_ASSERT(stmt != NULL);

#if 0
     printf ("Inside of SageBuilder::pushScopeStack(scope = %p = %s) \n",stmt,stmt->class_name().c_str());
#endif

     pushScopeStack(stmt);
   }
#endif

void SageBuilder::popScopeStack()
   {
  // we want to warning users  double freeing happens
  // if (!ScopeStack.empty())

  // DQ (7/30/2013): Added assertion.
     ROSE_ASSERT(ScopeStack.empty() == false);

#if 0
     printf ("Inside of SageBuilder::popScopeStack(): ScopeStack.back() = %p = %s \n",ScopeStack.back(),ScopeStack.back()->class_name().c_str());
#endif

     ScopeStack.pop_back();

#if 0
  // Debugging code to output the scope stack.
     printf ("SageBuilder::popScopeStack(): Scope stack: \n");
     int counter = 0;
     for (std::list<SgScopeStatement*>::iterator i = ScopeStack.begin(); i != ScopeStack.end(); i++)
        {
          printf ("   --- i = %d: %p = %s \n",counter,*i,(*i)->class_name().c_str());
          counter++;
        }
#endif
   }

SgScopeStatement* SageBuilder::topScopeStack()
   {
  // DQ (9/28/2009): Test if this is an empty stack, and if so return NULL (ScopeStack.back() should be undefined for this case).
     if (ScopeStack.empty() == true)
         return NULL;

  // DQ (9/28/2009): This is part of testing for GNU 4.0.x (other versions of g++ work fine).
     SgScopeStatement* tempScope = ScopeStack.back();
     if (tempScope != NULL)
        {
          tempScope->class_name();
        }

  // return ScopeStack.back();
     return tempScope;
   }


SgScopeStatement*
SageBuilder::getGlobalScopeFromScopeStack()
   {
  // This function adds new support within the internal scope stack mechanism.

  // DQ (3/20/2017): This branch is never taken and can be reported as an error (this improves code coverage).
  // DQ (3/11/2012): Test if this is an empty stack, and if so return NULL (ScopeStack.back() should be undefined for this case).
  // if (ScopeStack.empty() == true)
  //      return NULL;
     ROSE_ASSERT(ScopeStack.empty() == false);

  // The SgGlobal scope should be the first (front) element in the list (the current scope at the end (back) of the list).
     SgScopeStatement* tempScope = ScopeStack.front();
     ROSE_ASSERT(isSgGlobal(tempScope) != NULL);

     return tempScope;
   }

#if 0
// DQ (3/20/2017): This function is not used.
bool SageBuilder::isInScopeStack(SgScopeStatement * scope) {
  std::list<SgScopeStatement *>::const_iterator it_scope_stack = ScopeStack.begin();
  while (it_scope_stack != ScopeStack.end()) {
    if (*it_scope_stack == scope) return true;
    it_scope_stack++;
  }
  return false;
}
#endif

#if 0
// DQ (3/20/2017): This function is not used.
std::string SageBuilder::stringFromScopeStack() {
  std::ostringstream res;

  std::list<SgScopeStatement *>::const_iterator it_scope_stack = ScopeStack.begin();
  while (it_scope_stack != ScopeStack.end()) {
    res << *it_scope_stack << " = " << (*it_scope_stack)->class_name() << std::endl;
    it_scope_stack++;
  }

  return res.str();
}
#endif

bool SageBuilder::emptyScopeStack()
   {
     return ScopeStack.empty();
   }

void SageBuilder::clearScopeStack()
   {
     ScopeStack.clear();
   }

bool SageBuilder::inSwitchScope()
   {
  // DQ (11/26/2012): This is used to turn off some pragma processing which is a problem in switch statements.
     bool returnVar = false;
     std::list<SgScopeStatement*>::iterator i;
     for (i = ScopeStack.begin(); i != ScopeStack.end(); i++)
        {
          if (isSgSwitchStatement(*i) != NULL)
               returnVar = true;
        }

     return returnVar;
   }

#if 0
// DQ (3/20/2017): These functions are not used (suggest using the API in the symbol table initialization).
void SageBuilder::setCaseInsensitive()
   {
     symbol_table_case_insensitive_semantics = true;
   }

// DQ (3/20/2017): These functions are not used (suggest using the API in the symbol table initialization).
void SageBuilder::setCaseSensitive()
   {
     symbol_table_case_insensitive_semantics = false;
   }

// DQ (3/20/2017): These functions are not used (suggest using the API in the symbol table initialization).
void SageBuilder::setCaseFromScope(SgScopeStatement* scope)
   {
     ROSE_ASSERT(scope != NULL);

     symbol_table_case_insensitive_semantics = scope->isCaseInsensitive();
   }
#endif


// *******************************************************************************
// *******************************  Build Functions  *****************************
// *******************************************************************************
SgName
SageBuilder::appendTemplateArgumentsToName( const SgName & name, const SgTemplateArgumentPtrList & templateArgumentsList)
   {
  // DQ (7/23/2012): This function is somewhat redundant with the SgDeclarationStatement::resetTemplateNameSupport() in that
  // they both have to generate identical names.  this was a problem and thus this code is seneitive to " ," instead of ","
  // below.

  // DQ (7/23/2012): This is one of three locations where the template arguments are assembled and where
  // the name generated identically (in each case) is critical.  Not clear how to best refactor this code.
  // The other two are:
  //      Unparse_ExprStmt::unparseTemplateArgumentList()
  // and in:
  //      void SgDeclarationStatement::resetTemplateNameSupport ( bool & nameResetFromMangledForm, SgName & name )
  // It is less clear how to refactor this code.

#define DEBUG_APPEND_TEMPLATE_ARGUMENT_LIST 0

#if DEBUG_APPEND_TEMPLATE_ARGUMENT_LIST
     printf ("In SageBuilder::appendTemplateArgumentsToName(): CRITICAL FUNCTION TO BE REFACTORED (name = %s) \n",name.str());
#endif

  // DQ (3/10/2018): This is now partially redundant with SgTemplateArgumentList::unparseToStringSupport().
#if 0
     printf ("In SageBuilder::appendTemplateArgumentsToName: this is now partially redundant with SgTemplateArgumentList::unparseToStringSupport() \n");
#endif

     SgUnparse_Info *info = new SgUnparse_Info();
     ROSE_ASSERT(info != NULL);

     info->set_language(SgFile::e_Cxx_language);
     info->set_requiresGlobalNameQualification();

  // DQ (4/28/2017): For template arguments we never want to output the definitions of classes, and enums.
     info->set_SkipClassDefinition();
     info->set_SkipEnumDefinition();
     info->set_use_generated_name_for_template_arguments(true);

     bool emptyArgumentList = templateArgumentsList.empty();

  // DQ (9/24/2012): Don't add "< >" if there are no template arguments (see test2012_221.C).
  // SgName returnName = name + " < ";
     SgName returnName = name;
     if (emptyArgumentList == false)
          returnName += " < ";

     SgTemplateArgumentPtrList::const_iterator i = templateArgumentsList.begin();
     bool need_separator = false;
     while (i != templateArgumentsList.end())
        {
          if ((*i)->get_argumentType() == SgTemplateArgument::start_of_pack_expansion_argument)
             {
               i++;
               continue;
             }

          if (need_separator)
             {
               returnName += " , ";
             }

#if DEBUG_APPEND_TEMPLATE_ARGUMENT_LIST
          printf ("In SageBuilder::appendTemplateArgumentsToName(): (top of loop) templateArgumentsList element *i = %p = %s returnName = %s \n",*i,(*i)->class_name().c_str(),returnName.str());
#endif
#if 0
          string s = string("/* templateArgument is explicitlySpecified = ") + (((*i)->get_explicitlySpecified() == true) ? "true" : "false") + " */";
#endif

       // DQ (9/15/2012): We need to communicate that the language so that SgBoolVal will not be unparsed to "1" instead of "true" (see test2012_215.C).
       // Calling the unparseToString (SgUnparse_Info *info) function instead of the version not taking an argument.
          returnName += (*i)->unparseToString(info);

#if DEBUG_APPEND_TEMPLATE_ARGUMENT_LIST
          printf ("In SageBuilder::appendTemplateArgumentsToName(): (after appending template name) *i = %p returnName = %s \n",*i,returnName.str());
#endif
          need_separator = true;
          i++;

#if DEBUG_APPEND_TEMPLATE_ARGUMENT_LIST
          printf ("In SageBuilder::appendTemplateArgumentsToName(): (bottom of loop) returnName = %s \n",returnName.str());
#endif
        }

  // DQ (9/24/2012): Don't add "< >" if there are no templte arguments.
  // returnName += " > ";
     if (emptyArgumentList == false)
          returnName += " > ";

#if DEBUG_APPEND_TEMPLATE_ARGUMENT_LIST
     printf ("Leaving SageBuilder::appendTemplateArgumentsToName(): returnName = %s \n",returnName.str());
#endif

     delete info;
     info = NULL;

#if 0
  // DQ (4/2/2018): Debugging cae of template instantiation function appearing in std namespace with two symbols.
     if (name == "getline")
        {
          printf ("Leaving SageBuilder::appendTemplateArgumentsToName(): Found input function named: name = %s returnName = %s \n",name.str(),returnName.str());
        }
#endif

     return returnName;
   }


SgName
SageBuilder::unparseTemplateArgumentToString (SgTemplateArgument* templateArgument)
   {
  // DQ (3/10/2018): This is now redundant with SgTemplateArgument::unparseToStringSupport().
  // DQ (3/9/2018): Added function to support debugging.

     ROSE_ASSERT(templateArgument != NULL);

#if 0
     printf ("In SageBuilder::unparseTemplateArgumentToString: this is redundant with SgTemplateArgument::unparseToStringSupport() \n");
#endif

     SgUnparse_Info *info = new SgUnparse_Info();
     ROSE_ASSERT(info != NULL);

     info->set_language(SgFile::e_Cxx_language);

  // DQ (4/28/2017): For template arguments we never want to output the definitions of classes, and enums.
     info->set_SkipClassDefinition();
     info->set_SkipEnumDefinition();
     info->set_use_generated_name_for_template_arguments(true);

     SgName returnName = templateArgument->unparseToString(info);

     delete info;
     info = NULL;

     return returnName;
   }


SgTemplateArgumentPtrList*
SageBuilder::getTemplateArgumentList( SgDeclarationStatement* decl )
   {
  // DQ (9/13/2012): This function returns the SgTemplateArgumentPtrList. Both template declarations and template instanatiations have them.
  // In a template instantiation it is the templateArguments field and from template declarations it is the templateSpecializationArguments field.

     ROSE_ASSERT(decl != NULL);

#if 0
     printf ("In SageBuilder::getTemplateArgumentList(): decl = %p = %s \n",decl,decl->class_name().c_str());
#endif

     SgTemplateArgumentPtrList* templateArgumentsList = NULL;

     switch(decl->variantT())
        {
       // DQ (4/9/2018): Added support for namespace alias.
          case V_SgNamespaceAliasDeclarationStatement:
       // DQ (8/17/2013): These cases do not use templates.
       // This function has to handle these cases because it is called in a general context
       // on many types of declarations as part of the name qualification support.
          case V_SgNamespaceDeclarationStatement:
          case V_SgEnumDeclaration:
          case V_SgVariableDeclaration:
          case V_SgTypedefDeclaration:
             {
               templateArgumentsList = NULL;
               break;
             }

       // PC (10/11/13):  Added case of SgJavaPackageDeclaration
       // DQ (8/11/2013): Added cases for SgFunctionDeclaration and SgMemberFunctionDeclaration
       // I forget why we needed this case...
          case V_SgJavaPackageDeclaration:
          case V_SgFunctionDeclaration:
          case V_SgMemberFunctionDeclaration:
          case V_SgClassDeclaration:
             {
               templateArgumentsList = NULL;
               break;
             }

          case V_SgTemplateInstantiationDecl:
             {
#if 0
               printf ("In SageBuilder::getTemplateArgumentList(): templateArguments = %s \n",isSgTemplateInstantiationDecl(decl)->unparseNameToString().c_str());
#endif
               templateArgumentsList = &(isSgTemplateInstantiationDecl(decl)->get_templateArguments());
               break;
             }

          case V_SgTemplateClassDeclaration:
             {
               templateArgumentsList = &(isSgTemplateClassDeclaration(decl)->get_templateSpecializationArguments());
               break;
             }

          case V_SgTemplateInstantiationFunctionDecl:
             {
               templateArgumentsList = &(isSgTemplateInstantiationFunctionDecl(decl)->get_templateArguments());
               break;
             }

          case V_SgTemplateFunctionDeclaration:
             {
               templateArgumentsList = &(isSgTemplateFunctionDeclaration(decl)->get_templateSpecializationArguments());
               break;
             }

          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
               templateArgumentsList = &(isSgTemplateInstantiationMemberFunctionDecl(decl)->get_templateArguments());
               break;
             }

          case V_SgTemplateMemberFunctionDeclaration:
             {
               templateArgumentsList = &(isSgTemplateMemberFunctionDeclaration(decl)->get_templateSpecializationArguments());
               break;
             }

          case V_SgTemplateVariableDeclaration:
             {
               templateArgumentsList = &(isSgTemplateVariableDeclaration(decl)->get_templateSpecializationArguments());
               break;
             }

       // DQ (11/10/2014): Added support for template typedef declarations.
          case V_SgTemplateTypedefDeclaration:
             {
               templateArgumentsList = &(isSgTemplateTypedefDeclaration(decl)->get_templateSpecializationArguments());
               break;
             }

       // DQ (11/10/2014): Added support for template typedef declarations.
          case V_SgTemplateInstantiationTypedefDeclaration:
             {
               templateArgumentsList = &(isSgTemplateInstantiationTypedefDeclaration(decl)->get_templateArguments());
               break;
             }

          case V_SgTemplateDeclaration:
             {
               templateArgumentsList = NULL;
               break;
             }
#if 0
     // DQ (12/14/2016): Added new case
        case V_SgFunctionDeclaration:
             {
               SgFunctionDeclaration* function_declaration = isSgFunctionDeclaration(decl);
               printf ("In SageBuilder::getTemplateArgumentList(): function name = %s \n",function_declaration->get_name().str());

               function_declaration->get_file_info()->display("Not clear that we should need this case since there would not be any template arguments: debug");

               printf ("Error: Not clear that we should need this case since there would not be any template arguments \n");
               ROSE_ASSERT(false);
               break;
             }
#endif

          case V_SgNonrealDecl:
             {
               templateArgumentsList = &(isSgNonrealDecl(decl)->get_tpl_args());
               break;
             }

          default:
             {
               printf ("getTemplateArgumentList(): Default reached in switch: decl = %p = %s \n",decl,decl->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

  // DQ (9/16/2012): We would like to support this test, if possible.
  // ROSE_ASSERT(templateParameterList != NULL);

     return templateArgumentsList;
   }



SgTemplateParameterPtrList*
SageBuilder::getTemplateParameterList( SgDeclarationStatement* decl )
   {
  // DQ (9/16/2012): This function returns the SgTemplateParameterPtrList that is associated with template declarations.
  // For all other cases it returns NULL (or is an error).

     ROSE_ASSERT(decl != NULL);

     SgTemplateParameterPtrList* templateParameterList = NULL;

#if 0
     printf ("In SageBuilder::getTemplateParameterList(): decl = %p = %s \n",decl,decl->class_name().c_str());
#endif

     switch(decl->variantT())
        {
       // DQ (4/9/2018): Added support for namespace alias.
          case V_SgNamespaceAliasDeclarationStatement:
       // DQ (8/17/2013): These cases do not use templates.
       // This function has to handle these cases because it is called in a general context
       // on many types of declarations as part of the name qualification support.
          case V_SgNamespaceDeclarationStatement:
          case V_SgEnumDeclaration:
          case V_SgVariableDeclaration:
          case V_SgTypedefDeclaration:
             {
               templateParameterList = NULL;
               break;
             }

       // PC (10/11/13):  Added case of SgJavaPackageDeclaration
       // DQ (8/12/2013): This function has to be supported when called using any kind of declaration (at least SgFunctionDeclaration and SgClassDeclaration).
       // DQ (9/16/2012): I think it should be an error to call this function for a SgClassDeclaration.
          case V_SgJavaPackageDeclaration:
          case V_SgFunctionDeclaration:
          case V_SgMemberFunctionDeclaration:
          case V_SgClassDeclaration:
             {
               templateParameterList = NULL;
               break;
             }

       // DQ (11/10/2014): Added support for template typedef declarations.
          case V_SgTemplateInstantiationTypedefDeclaration:

       // DQ (8/12/2013): This function has to be supported when called using any kind of declaration (at least SgFunctionDeclaration and SgClassDeclaration).
       // DQ (9/16/2012): I think it should be an error to call this function for these types of declarations.
          case V_SgTemplateInstantiationDecl:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
               templateParameterList = NULL;
               break;
             }

          case V_SgTemplateClassDeclaration:
             {
               templateParameterList = &(isSgTemplateClassDeclaration(decl)->get_templateParameters());
               break;
             }

          case V_SgTemplateFunctionDeclaration:
             {
               templateParameterList = &(isSgTemplateFunctionDeclaration(decl)->get_templateParameters());
               break;
             }

          case V_SgTemplateMemberFunctionDeclaration:
             {
               templateParameterList = &(isSgTemplateMemberFunctionDeclaration(decl)->get_templateParameters());
               break;
             }

          case V_SgTemplateVariableDeclaration:
             {
               templateParameterList = &(isSgTemplateVariableDeclaration(decl)->get_templateParameters());
               break;
             }

       // DQ (11/10/2014): Added support for template typedef declarations.
          case V_SgTemplateTypedefDeclaration:
             {
               templateParameterList = &(isSgTemplateTypedefDeclaration(decl)->get_templateParameters());
               break;
             }

          case V_SgNonrealDecl:
             {
               templateParameterList = &(isSgNonrealDecl(decl)->get_tpl_params());
               break;
             }

          case V_SgTemplateDeclaration:
             {
               templateParameterList = &(isSgTemplateDeclaration(decl)->get_templateParameters());
               break;
             }

          default:
             {
               printf ("getTemplateParameterList(): Default reached in switch: decl = %p = %s \n",decl,decl->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

  // DQ (9/16/2012): We would like to support this test, if possible.
  // ROSE_ASSERT(templateParameterList != NULL);

     return templateParameterList;
   }



void
SageBuilder::setTemplateArgumentParents( SgDeclarationStatement* decl )
   {
  // DQ (9/13/2012): Set the parents of the template arguments (if not already set, to the first non-defining declaration).

     ROSE_ASSERT(decl != NULL);

#if 0
     printf ("In setTemplateArgumentParents(): decl = %p = %s \n",decl,decl->class_name().c_str());
#endif

     SgTemplateArgumentPtrList* templateArgumentsList = getTemplateArgumentList(decl);

     if (templateArgumentsList != NULL)
        {
          SgDeclarationStatement* first_decl = decl->get_firstNondefiningDeclaration();
          ROSE_ASSERT(first_decl != NULL);

          SgTemplateArgumentPtrList::iterator i = templateArgumentsList->begin();
          while (i != templateArgumentsList->end())
             {
               SgNode* parent = (*i)->get_parent();
               if (parent== NULL)
                  {
                 // (*i)->set_parent(decl);
                    (*i)->set_parent(first_decl);
                  }
                 else
                  {
                    SgScopeStatement* scope = isSgScopeStatement(parent);
                    if (scope != NULL)
                       {
                      // Template Arguments should have had there parents set to a scope when they were build, we want
                      // to refine that now that the declaration which we want them to be specified in has been build.
#if 0
                         printf ("In setTemplateArgumentParents(): Reset the template argument parent from scope = %p = %s to decl = %p = %s \n",scope,scope->class_name().c_str(),decl,decl->class_name().c_str());
#endif
                      // (*i)->set_parent(decl);
                         (*i)->set_parent(first_decl);
                       }
                      else
                       {
                         SgDeclarationStatement* declaration = isSgDeclarationStatement(parent);
                         if (declaration != NULL)
                            {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                              printf ("In setTemplateArgumentParents(): Template argument already set to declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
                            }
                           else
                            {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                              printf ("Error: In setTemplateArgumentParents(): I think it is an error for the template argument parent to be set to %p = %s \n",parent,parent->class_name().c_str());
#endif
                            }
                       }
                  }

               i++;
             }
        }

     testTemplateArgumentParents(decl);
   }


void
SageBuilder::setTemplateParameterParents( SgDeclarationStatement* decl )
   {
  // DQ (9/13/2012): Set the parents of the template arguments (if not already set, to the first non-defining declaration).

     ROSE_ASSERT(decl != NULL);

#if 0
     printf ("In setTemplateParameterParents(): decl = %p = %s \n",decl,decl->class_name().c_str());
#endif

     SgTemplateParameterPtrList* templateParameterList = getTemplateParameterList(decl);

#if 0
     printf ("In setTemplateParameterParents(): templateParameterList = %p \n",templateParameterList);
#endif

     if (templateParameterList != NULL)
        {
          SgDeclarationStatement* first_decl = decl->get_firstNondefiningDeclaration();
          ROSE_ASSERT(first_decl != NULL);

#if 0
          printf ("In setTemplateParameterParents(): first_decl = %p = %s \n",first_decl,first_decl->class_name().c_str());
#endif

          SgTemplateParameterPtrList::iterator i = templateParameterList->begin();
          while (i != templateParameterList->end())
             {
               SgNode* parent = (*i)->get_parent();
               if (parent == NULL)
                  {
#if 0
                    printf ("########### In setTemplateParameterParents(): Setting the template parameter to have a non-null parent: first_decl = %p = %s \n",first_decl,first_decl->class_name().c_str());
#endif
                 // (*i)->set_parent(decl);
                    (*i)->set_parent(first_decl);
                  }
                 else
                  {
                    SgScopeStatement* scope = isSgScopeStatement(parent);
                    if (scope != NULL)
                       {
                      // Template Arguments should have had their parents set to a scope when they were build, we want
                      // to refine that now that the declaration which we want them to be specified in has been build.
#if 0
                         printf ("Reset the template parameter parent from scope = %p = %s to decl = %p = %s \n",scope,scope->class_name().c_str(),decl,decl->class_name().c_str());
#endif
                      // (*i)->set_parent(decl);
                         (*i)->set_parent(first_decl);
                       }
                      else
                       {
#if 0
                         SgDeclarationStatement* declaration = isSgDeclarationStatement(parent);
                         if (declaration != NULL)
                            {
                              printf ("Template argument already set to declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                            }
                           else
                            {
                              printf ("Error: I think it is an error for the template argument parent to be set to %p = %s \n",parent,parent->class_name().c_str());
                            }
#endif
                       }
                  }

               i++;
             }
        }

     testTemplateParameterParents(decl);
   }


void
SageBuilder::testTemplateArgumentParents( SgDeclarationStatement* decl )
   {
  // DQ (9/13/2012): Set the parents of the template arguments (if not already set, to the first non-defining declaration).

     ROSE_ASSERT(decl != NULL);

#if 0
     printf ("In testTemplateArgumentParents(): decl = %p = %s \n",decl,decl->class_name().c_str());
#endif

     SgTemplateArgumentPtrList* templateArgumentsList = getTemplateArgumentList(decl);
  // ROSE_ASSERT(templateArgumentsList != NULL);

     if (templateArgumentsList != NULL)
        {
          SgTemplateArgumentPtrList::iterator i = templateArgumentsList->begin();
          while (i != templateArgumentsList->end())
             {
               SgNode* parent = (*i)->get_parent();
               if (parent == NULL)
                  {
                    printf ("Error: In testTemplateArgumentParents(): decl = %p = %s has template argument = %p with null parent \n",decl,decl->class_name().c_str(),*i);
                  }
               ROSE_ASSERT(parent != NULL);

            // DQ (9/16/2012): Adding new test.
               ROSE_ASSERT(decl->get_firstNondefiningDeclaration() != NULL);
#if 0
            // DQ (2/16/2014): This happens because the templates arguments are shared across multiple template instantiations and there parent pointers can only match a single template instantiation.
               if (parent != decl->get_firstNondefiningDeclaration())
                  {
                    printf ("Error: In testTemplateArgumentParents(): decl = %p = %s has template argument = %p with parent = %p = %s \n",decl,decl->class_name().c_str(),*i,parent,parent->class_name().c_str());
                    printf ("  --- decl                                    = %p = %s \n",decl,decl->class_name().c_str());
                    printf ("  --- decl->get_firstNondefiningDeclaration() = %p = %s \n",decl->get_firstNondefiningDeclaration(),decl->get_firstNondefiningDeclaration()->class_name().c_str());
                  }
#endif
            // DQ (1/30/2013): Commented this test out so that we could reuse SgTemplateArguments and
            // assure that the mapping from EDG a_template_arg_ptr's to SgTemplateArgument's was 1-to-1.
            // It is not clear if we can relax this constraint in the future.
            // ROSE_ASSERT(parent == decl->get_firstNondefiningDeclaration());

               i++;
             }
        }
   }


void
SageBuilder::testTemplateParameterParents( SgDeclarationStatement* decl )
   {
  // DQ (9/13/2012): Set the parents of the template arguments (if not already set, to the first non-defining declaration).

     ROSE_ASSERT(decl != NULL);

#if 0
     printf ("In testTemplateParameterParents(): decl = %p = %s \n",decl,decl->class_name().c_str());
#endif

     SgTemplateParameterPtrList* templateParameterList = getTemplateParameterList(decl);

     if (templateParameterList != NULL)
        {
          SgTemplateParameterPtrList::iterator i = templateParameterList->begin();
          while (i != templateParameterList->end())
             {
               SgNode* parent = (*i)->get_parent();
               if (parent == NULL)
                  {
                    printf ("Error: In testTemplateParameterParents(): decl = %p = %s has template argument = %p with null parent \n",decl,decl->class_name().c_str(),*i);
                  }
               ROSE_ASSERT(parent != NULL);

            // DQ (9/16/2012): Adding new test.
               ROSE_ASSERT(decl->get_firstNondefiningDeclaration() != NULL);
               if (parent != decl->get_firstNondefiningDeclaration())
                  {
#if 0
                 // DQ (2/7/2015): This message is output a lot for C++11 test projects/ShiftCalculus/simpleCNS.cpp (also test2014_83.C and test2014_84.C).
                    printf ("Error: In testTemplateParameterParents(): decl = %p = %s has template argument = %p with parent = %p = %s \n",decl,decl->class_name().c_str(),*i,parent,parent->class_name().c_str());
#endif
                  }

            // DQ (8/22/2013): Since these are now shared, it makes less sense to expect these to have such simple parent relationships.
            // This commit of work on ROSE added caching to template parameters so that we could support pointer equality for tests
            // of template parameter equality in the symbol table handling (this was a technique previously used for template arguments).
            // This test fails in the mergeTest_04.C test , but only on the GNU 4.4.x compiler (passes on the GNU 4.2.4 compiler).
            // ROSE_ASSERT(parent == decl->get_firstNondefiningDeclaration());

               i++;
             }
        }
   }


void
SageBuilder::setTemplateArgumentsInDeclaration( SgDeclarationStatement* decl, SgTemplateArgumentPtrList* templateArgumentsList_input )
   {
  // DQ (9/16/2012): Setup the template arguments for any type of template instantiation.

#if 0
     printf ("In setTemplateArgumentsInDeclaration(): decl = %p = %s \n",decl,decl->class_name().c_str());
  // printf ("   --- unparseNameToString() = %s \n",decl->unparseNameToString().c_str());
#endif

     ROSE_ASSERT(templateArgumentsList_input != NULL);

  // DQ (2/19/2018): Need to modify this function to take templated typedefs.
  //
  //    Do this in the morning...
  //

     ROSE_ASSERT(decl->variantT() == V_SgTemplateInstantiationDecl ||
                 decl->variantT() == V_SgTemplateInstantiationFunctionDecl ||
                 decl->variantT() == V_SgTemplateInstantiationMemberFunctionDecl ||
                 decl->variantT() == V_SgTemplateInstantiationTypedefDeclaration);

     SgTemplateArgumentPtrList* templateArgumentsList_from_declaration = getTemplateArgumentList(decl);

     if (templateArgumentsList_from_declaration != NULL)
        {
          *templateArgumentsList_from_declaration = *templateArgumentsList_input;

       // Set the parents.
          setTemplateArgumentParents(decl);
        }
       else
        {
#if 0
          printf ("In setTemplateArgumentsInDeclaration(): Setup of template arguments not supported for decl = %p = %s \n",decl,decl->class_name().c_str());
#endif
        }

     testTemplateArgumentParents(decl);

#if 0
  // printf ("Leaving setTemplateArgumentsInDeclaration(): unparseNameToString() = %s \n",decl->unparseNameToString().c_str());
#endif
   }


void
SageBuilder::setTemplateSpecializationArgumentsInDeclaration( SgDeclarationStatement* decl, SgTemplateArgumentPtrList* templateSpecializationArgumentsList_input )
   {
  // DQ (9/16/2012): Setup the template arguments for any type of template declaration

#if 0
     printf ("In setTemplateSpecializationArgumentsInDeclaration(): decl = %p = %s \n",decl,decl->class_name().c_str());
#endif

     ROSE_ASSERT(templateSpecializationArgumentsList_input != NULL);

     ROSE_ASSERT(decl->variantT() == V_SgTemplateClassDeclaration          || decl->variantT() == V_SgTemplateFunctionDeclaration ||
                 decl->variantT() == V_SgTemplateMemberFunctionDeclaration || decl->variantT() == V_SgTemplateVariableDeclaration );

     SgTemplateArgumentPtrList* templateSpecializationArgumentsList_from_declaration = getTemplateArgumentList(decl);

     if (templateSpecializationArgumentsList_from_declaration != NULL)
        {
          *templateSpecializationArgumentsList_from_declaration = *templateSpecializationArgumentsList_input;

       // Set the parents.
          setTemplateArgumentParents(decl);
        }
       else
        {
#if 0
          printf ("In setTemplateSpecializationArgumentsInDeclaration(): Setup of template arguments not supported for decl = %p = %s \n",decl,decl->class_name().c_str());
#endif
        }

     testTemplateArgumentParents(decl);
   }


void
SageBuilder::setTemplateParametersInDeclaration( SgDeclarationStatement* decl, SgTemplateParameterPtrList* templateParameterList_input )
   {
  // DQ (9/16/2012): Setup the template parameters for any type of template declaration.

#if 0
     printf ("In setTemplateParametersInDeclaration(): decl = %p = %s \n",decl,decl->class_name().c_str());
#endif

     ROSE_ASSERT(templateParameterList_input != NULL);

     ROSE_ASSERT(decl->variantT() == V_SgTemplateClassDeclaration          || decl->variantT() == V_SgTemplateFunctionDeclaration ||
                 decl->variantT() == V_SgTemplateMemberFunctionDeclaration || decl->variantT() == V_SgTemplateVariableDeclaration );

     SgTemplateParameterPtrList* templateParameterList_from_declaration = getTemplateParameterList(decl);

     if (templateParameterList_from_declaration != NULL)
        {
       // DQ (7/16/2017): Commented out this failing assertion (just now getting this function to be called).
       // ROSE_ASSERT(templateParameterList_from_declaration->empty() == true);

          *templateParameterList_from_declaration = *templateParameterList_input;

       // Set the parents.
          setTemplateParameterParents(decl);
        }
       else
        {
#if 0
          printf ("In setTemplateParameterInDeclaration(): Setup of template parameters not supported for decl = %p = %s \n",decl,decl->class_name().c_str());
#endif
        }

     testTemplateParameterParents(decl);
   }



// Only used to build parameter arguments for function ??
// should be transparently generated for most variable declaration builder
// deferred symbol insertion, scope setting , etc
// do them when it is actually used with the  parameterList!!
SgInitializedName *
SageBuilder::buildInitializedName ( const SgName & name, SgType* type, SgInitializer* init /* = NULL */)
{
#if 0
  // If the scope was not specified, then get it from the scope stack.
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();
//     ROSE_ASSERT(scope != NULL);
#endif
     //foo(int);  empty name is possible here!!
     //ROSE_ASSERT(name.is_null() == false);
     ROSE_ASSERT(type != NULL);

     SgInitializedName* initializedName = new SgInitializedName(name,type,init);
     ROSE_ASSERT(initializedName);
#if 0
    //TODO prototype parameter has no symbol associated!!
    //TODO  scope is also different: SgFunctionDefinition or scope of SgFunctionDeclaration
    SgVariableSymbol * symbol_1 = new SgVariableSymbol(initializedName);
    ROSE_ASSERT(symbol_1);
    scope->insert_symbol(name, symbol_1);
    initializedName->set_scope(scope);
#endif
    setSourcePositionAtRootAndAllChildren(initializedName);
    return initializedName;
}

SgInitializedName *
SageBuilder::buildInitializedName ( const std::string & name, SgType* type)
   {
     SgName var_name(name);
     return buildInitializedName(var_name,type);
   }

SgInitializedName *
SageBuilder::buildInitializedName ( const char* name, SgType* type)
   {
  // DQ (3/20/2017): Call the version of the function that takes a string as part of migration away from this function that takes a const char*.
  // This also provides a test of the string based version of this function (for code coverage).
  // SgName var_name(name);
     string var_name(name);
     return buildInitializedName(var_name,type);
   }

SgInitializedName *
SageBuilder::buildInitializedName_nfi ( const SgName & name, SgType* type, SgInitializer* init)
   {
     ROSE_ASSERT(type != NULL);

     SgInitializedName* initializedName = new SgInitializedName(name,type,init);
     ROSE_ASSERT(initializedName != NULL);

  // DQ (9/4/2013): Added test.
     ROSE_ASSERT(init == NULL || init->get_parent() == initializedName);

     setOneSourcePositionNull(initializedName);

     return initializedName;
   }

//-----------------------------------------------
// could have two declarations for a same variable
// extern int i;
//  int i;
SgVariableDeclaration*
SageBuilder::buildVariableDeclaration (const SgName & name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
 //(const SgName & name, SgType* type, SgInitializer * varInit= NULL, SgScopeStatement* scope = NULL)
{
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
//   ROSE_ASSERT(scope != NULL); // enable bottomup construction: scope can be unknown
   ROSE_ASSERT(name.is_null() == false);
   ROSE_ASSERT(type != NULL);

  SgVariableDeclaration * varDecl = new SgVariableDeclaration(name, type, varInit);
  ROSE_ASSERT(varDecl);

// DQ (8/21/2011): Note that the default is to set the declaration modifier's access modifier to be
// default (which is the same as public).  So the effect it to set it to be public.  This is ignored
// by the unparser for most languguages in ROSE.

// DQ (8/21/2011): Debugging declarations should have default settings (should not be marked as public).
// ROSE_ASSERT(varDecl->get_declarationModifier().get_accessModifier().isPublic() == false);

  varDecl->set_firstNondefiningDeclaration(varDecl);

  if (scope!=NULL)
  {
    // Liao 12/13/2010
    // Fortran subroutine/function parameters have corresponding variable declarations in the body
    // For this declaration, it should use the initialized names of the parameters instead of creating new ones
    // The symbol of the init name should be under SgFunctionDefinition, instead of the function body block
    bool isFortranParameter = false;
    if (SageInterface::is_Fortran_language())
    {
      SgFunctionDefinition * f_def = getEnclosingProcedure (scope);
      if (f_def != NULL)
      {
     // DQ (5/21/2013): Removed direct reference to symbol table (namespace handling is only supported at the SgScopeStatement level).
     // SgSymbolTable * st = f_def->get_symbol_table();
     // ROSE_ASSERT (st != NULL);
     // SgVariableSymbol * v_symbol = st->find_variable(name);
        SgVariableSymbol * v_symbol = f_def->lookup_variable_symbol(name);
        if (v_symbol != NULL) // find a function parameter with the same name
        {
          // replace the default one with the one from parameter
          SgInitializedName *default_initName = varDecl->get_decl_item (name);
          ROSE_ASSERT (default_initName != NULL);
          SgInitializedName * new_initName = v_symbol->get_declaration();
          ROSE_ASSERT (new_initName != NULL);
          ROSE_ASSERT (default_initName != new_initName);

          SgInitializedNamePtrList&  n_list= varDecl->get_variables();
          std::replace (n_list.begin(), n_list.end(),default_initName, new_initName );
          ROSE_ASSERT (varDecl->get_decl_item (name)==new_initName); //ensure the new one can be found

          // change the function argument's old parent to the variable declaration
          SgNode * old_parent = new_initName->get_parent();
          ROSE_ASSERT  (old_parent != NULL);
          ROSE_ASSERT  (isSgFunctionParameterList(old_parent) != NULL);
          new_initName->set_parent(varDecl); // adjust parent from SgFunctionParameterList to SgVariableDeclaration

       // DQ (1/25/2011): Deleting these causes problems if I use this function in the Fortran support...
       // delete (default_initName->get_declptr()); // delete the var definition
       // delete (default_initName->get_declptr()); // relink the var definition

          SgVariableDefinition * var_def = isSgVariableDefinition(default_initName->get_declptr()) ;
          ROSE_ASSERT (var_def != NULL);
          var_def->set_parent(new_initName);
          var_def->set_vardefn(new_initName);
          new_initName->set_declptr(var_def); // it was set to SgProcedureHeaderStatement as a function argument

          delete (default_initName); // must delete the old one to pass AST consistency test

       // DQ (12/13/2011): Is this executed...
          //printf ("Is this executed \n");
          //ROSE_ASSERT(false);

          isFortranParameter = true;
        }
      }
    }
    if (! isFortranParameter) // No need to add symbol to the function body if it is a Fortran parameter
                              // The symbol should already exist under function definition for the parameter
      fixVariableDeclaration(varDecl,scope);
  }

  SgInitializedName *initName = varDecl->get_decl_item (name);
  ROSE_ASSERT(initName != NULL);
  ROSE_ASSERT((initName->get_declptr())!=NULL);

  //bug 119, SgVariableDefintion's File_info is needed for deep copy to work
  // AstQuery based setSourcePositionForTransformation() cannot access all child nodes
  // have to set SgVariableDefintion explicitly
  SgDeclarationStatement* variableDefinition_original = initName->get_declptr();
  setOneSourcePositionForTransformation(variableDefinition_original);
  ROSE_ASSERT((variableDefinition_original->get_startOfConstruct()) !=NULL);
  ROSE_ASSERT((variableDefinition_original->get_endOfConstruct())!=NULL);

  setSourcePositionAtRootAndAllChildren(varDecl);
  //ROSE_ASSERT (isSgVariableDefinition(initName->get_declptr())->get_startOfConstruct()!=NULL);


  // DQ (4/16/2015): This is replaced with a better implementation.
  // DQ (4/15/2015): We should reset the isModified flags as part of the transforamtion
  // because we have added statements explicitly marked as transformations.
  // checkIsModifiedFlag(varDecl);
     unsetNodesMarkedAsModified(varDecl);

  return varDecl;
}

//-----------------------------------------------
// could have two declarations for a same variable
// extern int i;
//  int i;
// SgVariableDeclaration* SageBuilder::buildVariableDeclaration_nfi (const SgName & name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope, bool builtFromUseOnly)
SgVariableDeclaration*
SageBuilder::buildVariableDeclaration_nfi (const SgName & name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope, bool builtFromUseOnly)
 //(const SgName & name, SgType* type, SgInitializer * varInit= NULL, SgScopeStatement* scope = NULL)
   {

#define DEBUG_BUILD_VARIABLE_DECLARATION 0

#if DEBUG_BUILD_VARIABLE_DECLARATION
     printf ("In SageBuilder::buildVariableDeclaration_nfi(): name = %s scope = %p varInit = %p \n",name.str(),scope,varInit);
     if (scope != NULL)
        {
          printf (" --- scope = %p = %s \n",scope,scope->class_name().c_str());
        }
#endif

     if (scope == NULL)
        {
#if DEBUG_BUILD_VARIABLE_DECLARATION
          printf ("Scope determined from the SageBuilder::topScopeStack() \n");
#endif
          scope = SageBuilder::topScopeStack();
        }

     ROSE_ASSERT (scope != NULL);
     ROSE_ASSERT(type != NULL);

  // DQ (6/27/20`19): Older simpler version of code.
  // SgVariableDeclaration * varDecl = new SgVariableDeclaration(name, type, varInit);

  // DQ (7/18/2012): Added debugging code (should fail for test2011_75.C).
     SgVariableSymbol* variableSymbol = scope->lookup_variable_symbol(name);
  // ROSE_ASSERT(variableSymbol == NULL);

#if DEBUG_BUILD_VARIABLE_DECLARATION
     printf ("In SageBuilder::buildVariableDeclaration_nfi(): variableSymbol = %p \n",variableSymbol);
#endif

  // If there was a previous use of the variable, then there will be an existing symbol with it's declaration pointing to the SgInitializedName object.
     SgVariableDeclaration * varDecl = NULL;
     if (variableSymbol == NULL)
        {
          varDecl = new SgVariableDeclaration(name, type, varInit);
#if DEBUG_BUILD_VARIABLE_DECLARATION
          SgInitializedName* tmp_initializedName = getFirstInitializedName(varDecl);
          ROSE_ASSERT(tmp_initializedName != NULL);
          printf ("In SageBuilder::buildVariableDeclaration_nfi(): variableSymbol == NULL: varDecl = %p: initializedName = %p = %s \n",varDecl,tmp_initializedName,tmp_initializedName->get_name().str());
          printf (" --- tmp_initializedName->get_initptr() = %p \n",tmp_initializedName->get_initptr());
#endif
       // DQ (6/25/2019): This is a new feature to input the builtFromUseOnly function optional parameter.
          if (builtFromUseOnly == true)
             {
#if DEBUG_BUILD_VARIABLE_DECLARATION
               printf ("In buildVariableDeclaration_nfi(): this is the first reference to this variable: building a new SgVariableDeclaration: varDecl = %p name = %s \n",varDecl,name.str());
#endif
               varDecl->set_builtFromUseOnly(true);
             }
        }
       else
        {
          SgInitializedName* initializedName = variableSymbol->get_declaration();
          ROSE_ASSERT(initializedName != NULL);
          SgVariableDeclaration* associatedVariableDeclaration = isSgVariableDeclaration(initializedName->get_parent());

#if DEBUG_BUILD_VARIABLE_DECLARATION
          printf ("In SageBuilder::buildVariableDeclaration_nfi(): initializedName->get_parent() = %p \n",initializedName->get_parent());
          if (initializedName->get_parent() != NULL)
             {
               printf ("In SageBuilder::buildVariableDeclaration_nfi(): initializedName->get_parent() = %p = %s \n",initializedName->get_parent(),initializedName->get_parent()->class_name().c_str());
             }
          printf ("In SageBuilder::buildVariableDeclaration_nfi(): associatedVariableDeclaration = %p \n",associatedVariableDeclaration);
#endif
#if DEBUG_BUILD_VARIABLE_DECLARATION
       // DQ (6/24/2019): If this has been previously built as part of a variable use (in a class declaration),
       // then it should not be attached to the class definition as a variable declaration yet, and we should reuse it.
          if (associatedVariableDeclaration != NULL && associatedVariableDeclaration->get_parent() != NULL)
             {
               printf ("In SageBuilder::buildVariableDeclaration_nfi(): associatedVariableDeclaration->get_parent() = %p = %s \n",
                    associatedVariableDeclaration->get_parent(),associatedVariableDeclaration->get_parent()->class_name().c_str());
             }
#endif
       // DQ (6/25/2019): This is a new feature to input the builtFromUseOnly function optional parameter.
          if (builtFromUseOnly == true)
             {
#if 0
            // DQ (12/2/2019): Commented out output spew (from new tool).
               printf ("In buildVariableDeclaration_nfi(): this is a later reference to this variable (after the initial variable declaration and symbol): initializedName = %p name = %s \n",initializedName,name.str());
#endif
            // varDecl->set_builtFromUseOnly(true);
#if 0
            // DQ (7/12/2019): This may be overly conservative when used by the outlining to a seperate file.
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

#if DEBUG_BUILD_VARIABLE_DECLARATION
          printf ("associatedVariableDeclaration = %p \n",associatedVariableDeclaration);
          if (associatedVariableDeclaration != NULL)
             {
               printf ("associatedVariableDeclaration->get_builtFromUseOnly() = %s \n",associatedVariableDeclaration->get_builtFromUseOnly() ? "true" : "false");
             }
#endif

       // DQ (6/25/2019): Trigger the reuse of the available variable declaration.
       // bool reuseTheAssociatedVariableDeclaration = associatedVariableDeclaration->get_builtFromUseOnly();
          bool reuseTheAssociatedVariableDeclaration = ((associatedVariableDeclaration != NULL) && (associatedVariableDeclaration->get_builtFromUseOnly() == true));
       // if (associatedVariableDeclaration != NULL)
       // if (reuseTheAssociatedVariableDeclaration == true && associatedVariableDeclaration != NULL)
          if (reuseTheAssociatedVariableDeclaration == true)
             {
            // Build a seperate SgVariableDeclaration so that we can avoid sharing the SgInitializedName
            // (and it's possible initializer which would be an error for the secondary declaration
            // (the declaration in the class for the case of a static declaration))

               ROSE_ASSERT(associatedVariableDeclaration != NULL);

            // DQ (6/24/2019): Fix this to use the associatedVariableDeclaration.
            // varDecl = new SgVariableDeclaration(name, type, varInit);
               varDecl = associatedVariableDeclaration;

            // DQ (6/25/2019): Mark this variable declaration so that it will not be reused again.
               varDecl->set_builtFromUseOnly(false);

            // DQ (6/24/2019): Set the parent to NULL, since we are reusing this variable declaration and it would not have been set correctly before.
               varDecl->set_parent(NULL);

            // DQ (6/24/2019): this veriable declaration that is being reused, should not have had an initializer (check this).
               SgInitializedName* variable = getFirstInitializedName(varDecl);
               ROSE_ASSERT(variable != NULL);

            // DQ (6/25/2019): See Cxx11_tests/test2019_121.C and Cxx11_tests/test2019_482.C.
            // ROSE_ASSERT(variable->get_initptr() == NULL);
#if DEBUG_BUILD_VARIABLE_DECLARATION
               if (variable->get_initptr() != NULL)
                  {
                    printf ("Found initializer associated with variable declaration being reused: variable = %p name = %s \n",variable,variable->get_name().str());
                  }
#endif
            // DQ (7/3/2019): Reuse in a conditional will have a valid initializer.
            // ROSE_ASSERT(variable->get_initptr() == NULL);
             }
            else
             {
            // DQ (6/25/2019): We can't reuse the existing SgInitializedName, because it could have been initialized in the other SgVariableDeclaration.
#if 0
               printf ("In SageBuilder::buildVariableDeclaration_nfi(): We can't reuse the existing SgInitializedName: initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
               printf (" --- initializedName->get_initptr() = %p \n",initializedName->get_initptr());
#endif
            // DQ (6/26/2019): Added assertion.
               ROSE_ASSERT(reuseTheAssociatedVariableDeclaration == false);

            // DQ (6/27/2019): If the SgInitializedName was generated from the convert_variable_use() function in the
            // EDG/ROSE translation, then where was not associated SgVariableDeclaration built (an inconsistancy).
            // So we want to check for the parent being a scope statement (e.g. SgIfStmt or other statement that can
            // accept a conditional expression where in C++ it can alternatively declare a variable.
               if (associatedVariableDeclaration == NULL)
                  {
                    ROSE_ASSERT(initializedName->get_parent() != NULL);
                    SgScopeStatement* scopeStatement = isSgScopeStatement(initializedName->get_parent());
                    if (scopeStatement != NULL)
                       {
#if DEBUG_BUILD_VARIABLE_DECLARATION
                         printf ("scopeStatement = %p = %s \n",scopeStatement,scopeStatement->class_name().c_str());
#endif
                       }
                      else
                       {
#if DEBUG_BUILD_VARIABLE_DECLARATION
                         printf ("initializedName->get_parent() = %p = %s \n",initializedName->get_parent(),initializedName->get_parent()->class_name().c_str());
#endif
                       }
                  }

            // DQ (6/27/2019): In some case we want to reuse the associated SgInitializedName node.
            // For example: variable declarations in conditionals (e.g. SgIfStmt, and other scope statements).
            // DQ (6/26/2019): Build an additional variable to support another reference to the original variable.
            // Note: we need another one because either one can have an initializer that cannot be shared in the AST.
            // SgInitializedName* additional_variable = buildInitializedName_nfi(name,type,varInit);
               SgInitializedName* additional_variable = NULL;

#if DEBUG_BUILD_VARIABLE_DECLARATION
               printf ("In SageBuilder::buildVariableDeclaration_nfi(): initializedName->get_scope()     = %p = %s \n",initializedName->get_scope(),initializedName->get_scope()->class_name().c_str());
#endif
               SgScopeStatement* scopeStatement = isSgScopeStatement(initializedName->get_parent());
            // ROSE_ASSERT(additional_variable->get_scope() != NULL);
               if (scopeStatement != NULL)
                  {
                    additional_variable = initializedName;

                 // DQ (6/28/2019): Support case when the borrowed SgInitializedName has a valid initializer.
                 // DQ (6/26/2019): Adding the initializer.
                 // ROSE_ASSERT(additional_variable->get_initptr() == NULL);
                 // additional_variable->set_initptr(varInit);
                    if (additional_variable->get_initptr() != NULL)
                       {
#if DEBUG_BUILD_VARIABLE_DECLARATION
                         printf ("In SageBuilder::buildVariableDeclaration_nfi(): borrowed SgInitializedName is alread initialized \n");
                         printf (" --- additional_variable->get_initptr() = %p \n",additional_variable->get_initptr());
                         printf (" --- varInit = %p \n",varInit);
#endif
                      // DQ (6/28/2019): when this is assertion is false, we have constructed a redundant initializer (debugging this).
                         // PP (7/22/2019) faults in CUDA code
                         // ROSE_ASSERT(varInit == NULL);
                       }
                      else
                       {
                         additional_variable->set_initptr(varInit);
                       }

#if DEBUG_BUILD_VARIABLE_DECLARATION || 0
                    printf (" --- additional_variable->get_scope() = %p = %s \n",additional_variable->get_scope(),additional_variable->get_scope()->class_name().c_str());
                    printf (" --- Reusing the SgInitializedName (not associated with a previous SgVariableDeclaration where the parent is a SgScopeStatement) \n");
#endif
                  }
                 else
                  {
#if DEBUG_BUILD_VARIABLE_DECLARATION
                    printf (" --- Building a new SgInitializedName \n");
#endif
                    additional_variable = buildInitializedName_nfi(name,type,varInit);
                  }

#if DEBUG_BUILD_VARIABLE_DECLARATION
               ROSE_ASSERT(initializedName->get_scope() != NULL);
#endif
            // DQ (6/26/2019): Set the scopes to be the same (a symbol already exists at this point).
            // additional_variable->set_scope(initializedName->get_scope());
            // initializedName->set_scope(additional_variable->get_scope());
            // additional_variable->set_scope(initializedName->get_scope());

            // DQ (6/26/2019): Set the pointer to the original version of this variable (unless we reused the SgInitializedName above).
               if (additional_variable != initializedName)
                  {
                    additional_variable->set_prev_decl_item(initializedName);
                  }

            // If there is not an associated SgVariableDeclaration then reuse the existing SgInitializedName.
            // varDecl = new SgVariableDeclaration(initializedName);
               varDecl = new SgVariableDeclaration(additional_variable);
#if DEBUG_BUILD_VARIABLE_DECLARATION
               ROSE_ASSERT(initializedName->get_parent() != NULL);
               printf ("initializedName->get_parent()     = %p = %s \n",initializedName->get_parent(),initializedName->get_parent()->class_name().c_str());
               ROSE_ASSERT(additional_variable->get_parent() != NULL);
               printf ("additional_variable->get_parent() = %p = %s \n",additional_variable->get_parent(),additional_variable->get_parent()->class_name().c_str());
#endif
            // DQ (6/26/2019): Set the parent of the first SgInitializedName to that of the second SgInitializedName.
            // This is an issue for the range for initialization: see test2019_483.C.
            // initializedName->set_parent(additional_variable->get_parent());

#if 0
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(additional_variable->get_parent());
               if (variableDeclaration != NULL)
                  {
#if 0
                    printf ("Found SgVariableDeclaration at additional_variable->get_parent() \n");
                    ROSE_ASSERT(variableDeclaration->get_parent() != NULL);
                    printf ("  --- variableDeclaration->get_parent() = %p = %s \n",variableDeclaration->get_parent(),variableDeclaration->get_parent()->class_name().c_str());
#endif
                    SgRangeBasedForStatement* rangeBasedForStatement = isSgRangeBasedForStatement(variableDeclaration->get_parent());
                    if (rangeBasedForStatement != NULL)
                       {
#if 0
                         printf ("Found SgRangeBasedForStatement at variableDeclaration->get_parent(): set scope of initializedName \n");
#endif
                         initializedName->set_scope(rangeBasedForStatement);
                       }
                  }
#endif
            // DQ (6/26/2019): Adding assertion after setting of the SgRangeBasedForStatement before processing the children.
               ROSE_ASSERT(initializedName->get_scope() != NULL);

            // DQ (6/26/2019): Set the pointer to the original version of this variable (unless we reused the SgInitializedName above).
               if (additional_variable != initializedName)
                  {
                    additional_variable->set_scope(initializedName->get_scope());
                  }
#if 0
            // DQ (6/25/2019): This step overwrites the non-null pointer to the initializer with a null pointer to the initializer in Cxx11_tests/test2019_482.C.
               if (initializedName->get_initptr() != NULL && varInit == NULL)
                  {
                    printf ("ERROR: This step overwrites the non-null pointer to the initializer with a null pointer to the initializer in Cxx11_tests/test2019_482.C \n");
                    ROSE_ASSERT(false);
                  }
#endif
            // DQ (7/14/2014): Set the variable initialized (see test2014_107.C, also required for boost for_each support)).
            // initializedName->set_initptr(varInit);
#if DEBUG_BUILD_VARIABLE_DECLARATION
               printf ("In SageBuilder::buildVariableDeclaration_nfi(): After sharing the exisitng SgInitializedName: initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
               printf (" --- initializedName->get_initptr()     = %p \n",initializedName->get_initptr());
               printf (" --- additional_variable->get_initptr() = %p \n",additional_variable->get_initptr());
#endif
             }
        }

  // DQ (11/3/2012): The SgInitializedName inside the SgVariableDeclaration must have valid source position object (even if default initialized).
     SgInitializedName* variable = getFirstInitializedName(varDecl);
     ROSE_ASSERT(variable != NULL);
     setSourcePosition(variable);

     ROSE_ASSERT(varDecl != NULL);

     varDecl->set_firstNondefiningDeclaration(varDecl);

  // DQ (7/9/2012): Added test (parent should not be set yet; set in parse_statement).
     ROSE_ASSERT(varDecl->get_parent() == NULL);

     if (name != "")
        {
       // Anonymous bit fields should not have symbols
          fixVariableDeclaration(varDecl,scope);

       // DQ (7/9/2012): Added test (parent should not be set yet; set in parse_statement).
       // ROSE_ASSERT(varDecl->get_parent() == NULL);
        }
       else
        {
       // DQ (7/12/2012): This is not correct for C++ (to use the input scope), so don't set it here (unless we use the current scope instead of scope).
       // Yes, let's set it to the current top of the scope stack.  This might be a problem if the scope stack is not being used...

#if DEBUG_BUILD_VARIABLE_DECLARATION
       // DQ (6/25/2018): I think this is incorrect for test2018_109.C.
          SgScopeStatement* current_scope = topScopeStack();
          printf ("  --- Setting parent using topScopeStack() = %p = %s = %s \n",current_scope,current_scope->class_name().c_str(),SageInterface::get_name(current_scope).c_str());
#endif
          varDecl->set_parent(topScopeStack());
          ROSE_ASSERT(varDecl->get_parent() != NULL);
        }

     SgInitializedName *initName = varDecl->get_decl_item (name);
     ROSE_ASSERT(initName);
     ROSE_ASSERT(initName->get_declptr() != NULL);

  // DQ (5/17/2013): Added test.
     if (initName->get_scope() == NULL)
        {
       // Make this a warning for the few places where this fails.
#if DEBUG_BUILD_VARIABLE_DECLARATION
          printf ("WARNING: Note in buildVariableDeclaration_nfi(): initName->get_scope() == NULL \n");
#endif
        }
  // ROSE_ASSERT(initName->get_scope() != NULL);

  // DQ (7/9/2012): Added test (parent should not be set yet; set in parse_statement).
  // ROSE_ASSERT(varDecl->get_parent() == NULL);

#if 1
  // bug 119, SgVariableDefintion's File_info is needed for deep copy to work
  // AstQuery based setSourcePositionForTransformation() cannot access all child nodes
  // have to set SgVariableDefintion explicitly
     SgVariableDefinition* variableDefinition_original = isSgVariableDefinition(initName->get_declptr());
     ROSE_ASSERT(variableDefinition_original != NULL);
     setOneSourcePositionNull(variableDefinition_original);
#endif
     setOneSourcePositionNull(varDecl);

#if 0
  // DQ (1/2/2010): Set the defining declaration to itself.
     if (varDecl->get_definingDeclaration() == NULL)
          varDecl->set_definingDeclaration(varDecl);
#endif

  // DQ (7/12/2012): The parent should be set to the current scope (not the same as that specified
  // in the scope (since that applies to the variable (SgInitializedName) not the SgVariableDeclaration).
  // DQ (7/9/2012): Added test (parent should not be set yet; set in parse_statement).
  // ROSE_ASSERT(varDecl->get_parent() == NULL);
  // Liao, 1/23/2013, quick fix for now, this condition is a mirror to the code setting parent in SageInterface::fixVariableDeclaration()
     if (topScopeStack() != NULL)
        {
          ROSE_ASSERT(varDecl->get_parent() != NULL);
        }

  // ROSE_ASSERT (varDecl->get_declarationModifier().get_accessModifier().isPublic() == false);

  // DQ (4/16/2015): This is replaced with a better implementation.
  // DQ (4/15/2015): We should reset the isModified flags as part of the transforamtion
  // because we have added statements explicitly marked as transformations.
  // checkIsModifiedFlag(varDecl);
     unsetNodesMarkedAsModified(varDecl);

  // DQ (6/25/2018): Added assertion.
     ROSE_ASSERT(varDecl != NULL);

#if DEBUG_BUILD_VARIABLE_DECLARATION
     printf ("Leaving buildVariableDeclaration_nfi(): varDecl = %p varDecl->get_parent() = %p \n",varDecl,varDecl->get_parent());
#endif

     return varDecl;
   }


SgVariableDefinition*
SageBuilder::buildVariableDefinition_nfi (SgVariableDeclaration* decl, SgInitializedName* init_name,  SgInitializer *init)
{
// refactored from ROSETTA/Grammar/Statement.code SgVariableDeclaration::append_variable ()

  ROSE_ASSERT (decl!=NULL);
  ROSE_ASSERT (init_name !=NULL);
  // init can be NULL

  SgVariableDefinition *defn_stmt = NULL;
  if (!isSgFunctionType(init_name->get_type()))
  {
    Sg_File_Info* copyOfFileInfo = NULL;
    if (decl->get_file_info() != NULL)
    {
      copyOfFileInfo = new Sg_File_Info(*(decl->get_file_info()));
      ROSE_ASSERT (copyOfFileInfo != NULL);

      // Note that the SgVariableDefinition will connect the new IR node into the AST.
      defn_stmt = new SgVariableDefinition(copyOfFileInfo, init_name, init);
      assert (defn_stmt != NULL);

      copyOfFileInfo->set_parent(defn_stmt);

      // DQ (3/13/2007): We can't enforce that the endOfConstruct is set (if the interface using the startOfConstruct is used.
      // DQ (2/3/2007): Need to build the endOfConstruct position as well.
      // ROSE_ASSERT(this->get_endOfConstruct() != NULL);
      if (decl->get_endOfConstruct() != NULL)
      {
        Sg_File_Info* copyOfEndOfConstruct = new Sg_File_Info(*(decl->get_endOfConstruct()));
        defn_stmt->set_endOfConstruct(copyOfEndOfConstruct);
        copyOfEndOfConstruct->set_parent(defn_stmt);
      }
    }
    else
    {
      // Note that the SgVariableDefinition will connect the new IR node into the AST.
      defn_stmt = new SgVariableDefinition(init_name, init);
    }
    ROSE_ASSERT(defn_stmt != NULL);
  }
  else
    defn_stmt = NULL;
  return defn_stmt ;
}


// DQ (12/6/2011): Adding support for template declarations into the AST.
// SgTemplateDeclaration*
// SgVariableDeclaration*
// SgTemplateVariableDeclaration* buildTemplateVariableDeclaration_nfi(const SgName & name, SgType *type, SgInitializer *varInit, SgScopeStatement* scope);
SgTemplateVariableDeclaration*
SageBuilder::buildTemplateVariableDeclaration_nfi (const SgName & name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
   {
     ROSE_ASSERT (scope != NULL);
     ROSE_ASSERT(type != NULL);

     SgTemplateVariableDeclaration * varDecl = new SgTemplateVariableDeclaration(name, type, varInit);
     ROSE_ASSERT(varDecl);

     varDecl->set_firstNondefiningDeclaration(varDecl);

  // DQ (11/3/2012): The SgInitializedName inside the SgVariableDeclaration must have valid source position object (even if default initialized).
     SgInitializedName* variable = getFirstInitializedName(varDecl);
     ROSE_ASSERT(variable != NULL);
     setSourcePosition(variable);

     if (name != "")
        {
       // Anonymous bit fields should not have symbols
          fixVariableDeclaration(varDecl,scope);
        }

     SgInitializedName *initName = varDecl->get_decl_item (name);
     ROSE_ASSERT(initName);
     ROSE_ASSERT((initName->get_declptr())!=NULL);

  // bug 119, SgVariableDefintion's File_info is needed for deep copy to work
  // AstQuery based setSourcePositionForTransformation() cannot access all child nodes
  // have to set SgVariableDefintion explicitly
     SgVariableDefinition* variableDefinition_original = isSgVariableDefinition(initName->get_declptr());
     ROSE_ASSERT(variableDefinition_original != NULL);
     setOneSourcePositionNull(variableDefinition_original);

     setOneSourcePositionNull(varDecl);

#if 0
// DQ (1/2/2010): Set the defining declaration to itself.
     if (varDecl->get_definingDeclaration() == NULL)
          varDecl->set_definingDeclaration(varDecl);
#endif

  // ROSE_ASSERT (varDecl->get_declarationModifier().get_accessModifier().isPublic() == false);
     return varDecl;
   }

SgVariableDeclaration*
SageBuilder::buildVariableDeclaration(const std::string & name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
{
  SgName name2(name);
  return buildVariableDeclaration(name2,type, varInit,scope);
}

SgVariableDeclaration*
SageBuilder::buildVariableDeclaration(const char* name, SgType* type, SgInitializer * varInit, SgScopeStatement* scope)
{
  SgName name2(name);
  return buildVariableDeclaration(name2,type, varInit,scope);
}

//!Build a typedef declaration, such as: typedef int myint;
SgTypedefDeclaration*
SageBuilder::buildTypedefDeclaration(const std::string& name, SgType* base_type, SgScopeStatement* scope /*= NULL*/, bool has_defining_base/*= false*/)
{
  SgTypedefDeclaration* type_decl = buildTypedefDeclaration_nfi(name, base_type, scope, has_defining_base);
  setOneSourcePositionForTransformation(type_decl);

// DQ (1/2/2010): Set the defining declaration to itself.
// type_decl->set_definingDeclaration(type_decl);

  return type_decl;
}

//!Build a typedef declaration, such as: typedef int myint;
// The side effects include: creating SgTypedefType, SgTypedefSymbol, and add SgTypedefType to the base type
SgTypedefDeclaration*
SageBuilder::buildTypedefDeclaration_nfi(const std::string& name, SgType* base_type,  SgScopeStatement* scope /*= NULL*/, bool has_defining_base/*=false*/)
   {
     ROSE_ASSERT (base_type != NULL);

#if 0
     printf ("In buildTypedefDeclaration_nfi(): base_type = %p = %s \n",base_type,base_type->class_name().c_str());
#endif

     if (scope == NULL )
        {
          scope = SageBuilder::topScopeStack();
        }

  // We don't yet support bottom up construction for this node yet
     ROSE_ASSERT(scope!=NULL);

     SgDeclarationStatement* base_decl = NULL;

  // Handle the case where this is a pointer, reference, or typedef to another type.
  // if (isSgNamedType(base_type))
  // SgNamedType* namedType = isSgNamedType(base_type->stripType());
     SgType* stripedBaseType = base_type->stripType();
     ROSE_ASSERT(stripedBaseType != NULL);

#if 0
     printf ("In buildTypedefDeclaration_nfi(): stripedBaseType = %p = %s \n",stripedBaseType,stripedBaseType->class_name().c_str());
#endif

     SgNamedType* namedType = isSgNamedType(stripedBaseType);
     if (namedType != NULL)
        {
       // DQ (12/28/2019): the problem with getting the base declaration from the type is that it forces sharing 
       // of the base declaration when the typedef has a defining declaration for a base type in multiple files.
#if 0
          printf ("NOTE: Using the base declaration from the type forces sharing of the base declaration across multiple translation units \n");
#endif
       // DQ (3/20/2012): Use this to set the value of base_decl (which was previously unset).
       // isSgNamedType(base_type)->get_declaration();
       // base_decl = isSgNamedType(base_type)->get_declaration();
          base_decl = namedType->get_declaration();

       // DQ (3/20/2012): All named types should have a valid declaration!
          ROSE_ASSERT(base_decl != NULL);
#if 0
          printf ("In buildTypedefDeclaration_nfi(): base_decl = %p = %s \n",base_decl,base_decl->class_name().c_str());
          if (base_decl->get_definingDeclaration() != NULL)
             {
               printf ("base_decl->get_definingDeclaration() = %p parent = %p \n",base_decl->get_definingDeclaration(),base_decl->get_definingDeclaration()->get_parent());
             }
          if (base_decl == base_decl->get_definingDeclaration())
             {
               printf ("base_decl matches the defining declaration = %p parent = %p \n",base_decl,base_decl->get_parent());
             }

          if (base_decl->get_firstNondefiningDeclaration() != NULL)
             {
               printf ("base_decl->get_firstNondefiningDeclaration() = %p parent = %p \n",base_decl->get_firstNondefiningDeclaration(),base_decl->get_firstNondefiningDeclaration()->get_parent());
             }
          if (base_decl == base_decl->get_firstNondefiningDeclaration())
             {
               printf ("base_decl matches the firstNondefining declaration = %p parent = %p \n",base_decl,base_decl->get_parent());
             }
#endif
        }

#if 0
     printf ("In buildTypedefDeclaration_nfi(): base_decl = %p \n",base_decl);
#endif

  // DQ (3/20/2012): I don't remember why we need to provide the symbol for the scope of the
  // parent rather then the scope. But as I recall there was a special corner of C++ that
  // required this sort of support.
     SgSymbol* parent_scope = NULL;
#ifndef ROSE_USE_CLANG_FRONTEND
     if (scope != NULL)
        {
#if 0
          printf ("In buildTypedefDeclaration_nfi(): scope = %p = %s calling get_symbol_from_symbol_table() \n",scope,scope->class_name().c_str());
#endif
          ROSE_ASSERT(scope->get_parent() != NULL);
          SgDeclarationStatement* declaration = isSgDeclarationStatement(scope->get_parent());
#if 0
          printf ("declaration = %p \n",declaration);
#endif
          if (declaration != NULL)
             {
#if 0
               printf ("Found a valid declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
               ROSE_ASSERT(declaration->get_firstNondefiningDeclaration() != NULL);
            // parent_scope = declaration->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table();
               parent_scope = declaration->search_for_symbol_from_symbol_table();

               ROSE_ASSERT(parent_scope != NULL);
             }
        }
#endif

#if 0
     printf ("In buildTypedefDeclaration_nfi(): parent_scope = %p \n",parent_scope);
#endif

#if 0
     printf ("In buildTypedefDeclaration_nfi(): base_decl = %p \n",base_decl);
#endif

  // SgTypedefDeclaration (Sg_File_Info *startOfConstruct, SgName name="", SgType *base_type=NULL, SgTypedefType *type=NULL, SgDeclarationStatement *declaration=NULL, SgSymbol *parent_scope=NULL)
  // SgTypedefDeclaration (SgName name="", SgType *base_type=NULL, SgTypedefType *type=NULL, SgDeclarationStatement *declaration=NULL, SgSymbol *parent_scope=NULL)
  //
  // Create the first nondefining declaration (note that the typedef type is always a NULL input value).
  // SgTypedefDeclaration* type_decl = new SgTypedefDeclaration(SgName(name),base_type,NULL, NULL, NULL);
     SgTypedefDeclaration* type_decl = new SgTypedefDeclaration(SgName(name), base_type, NULL, base_decl, parent_scope);
     ROSE_ASSERT(type_decl != NULL);

  // TV (08/17/2018): moved it before building type as SgTypedefType::createType uses SgTemplateTypedefDeclaration::get_mangled_name which requires the scope to be set (else name of the associated type might not be unique)
     type_decl->set_scope(scope);
     type_decl->set_parent(scope);

#if 0
     printf ("In buildTypedefDeclaration_nfi(): type_decl = %p type_decl->get_declaration() = %p \n",type_decl,type_decl->get_declaration());
#endif

#if 0
     printf ("In buildTypedefDeclaration_nfi(): calling SgTypedefType::createType() using this = %p = %s \n",type_decl,type_decl->class_name().c_str());
#endif

  // DQ (2/27/2018): Add this call here to reflect change to the constructor semantics.
     type_decl->set_type(SgTypedefType::createType(type_decl));

#if 0
     printf ("In buildTypedefDeclaration_nfi(): DONE calling SgTypedefType::createType() using this = %p = %s \n",type_decl,type_decl->class_name().c_str());
#endif

#if 0
 // DQ (2/27/2018): The constructor semantics now require the type to be built after the IR node has been built.
 // Make this fail so that we can detect it an fix it properly later.
    printf ("In SageBuilder::buildTypedefDeclaration_nfi(): ERROR: The constructor semantics now require the type to be built after the IR node has been built \n");
    ROSE_ASSERT(false);
#endif
#if 0
     printf ("In buildTypedefDeclaration_nfi(): After SgTypedefDeclaration constructor: type_decl->get_scope() = %p \n",type_decl->get_scope());
#endif

  // DQ (3/20/2012): Comment ouly, these are always set this way. first defining is a self reference, and defining is always NULL (required for AST consistancy)).
     type_decl->set_firstNondefiningDeclaration (type_decl);
     type_decl->set_definingDeclaration(NULL);

  // Set the source code position information.
     setOneSourcePositionNull(type_decl);

  // Liao 11/29/2012, for typedef struct Frame {int x;} st_frame; We have to set parent for the struct.
  // AstPostProcessing() has resetParentPointers(). But it is kind of too late.
     if (SgClassDeclaration* base_class = isSgClassDeclaration(base_decl))
        {
          SgClassDeclaration* def_class = isSgClassDeclaration(base_class->get_definingDeclaration());
          SgClassDeclaration* nondef_class = isSgClassDeclaration(base_class->get_firstNondefiningDeclaration());
       // Dan and Liao, 12/3/2012, handle test2003_08.C nested typedef
          if (has_defining_base)
             {
               if (def_class != NULL)
                    if (def_class->get_parent() == NULL)
                         def_class->set_parent(type_decl);
             }
            else
             {
               if (nondef_class != NULL)
                    if (nondef_class->get_parent() == NULL)
                       {
                         nondef_class->set_parent(type_decl);
                       }
             }
        }

     SgTypedefSymbol* typedef_symbol = new SgTypedefSymbol(type_decl);
     ROSE_ASSERT(typedef_symbol);

     scope->insert_symbol(SgName(name),typedef_symbol);

#if 0
  // DQ (3/20/2012): This is always false...since base_decl is never reset from NULL (above).

  // TODO double check when to create defining declaration
  // I tried two cases so far and the simplest typedef int MYINT will not have defining typedef declaration
  // the complex typedef struct frame {} frame; has a defining typedef declaration
  // base declaration should be associated with defining typedef declaration
     if (base_decl != NULL)
        {
          SgTypedefDeclaration* def_type_decl = new SgTypedefDeclaration(SgName(name),base_type,type_decl->get_type(), base_decl, NULL);
          def_type_decl->set_firstNondefiningDeclaration(type_decl);
          type_decl->set_definingDeclaration(def_type_decl);
          setOneSourcePositionNull(def_type_decl);
        }
#endif

#if 0
  // DQ (3/20/2012): Test the base_decl.
     if (base_decl != NULL)
        {
          printf ("In buildTypedefDeclaration_nfi(): base_decl->get_definingDeclaration()         = %p \n",base_decl->get_definingDeclaration());
          if (base_decl->get_definingDeclaration() != NULL)
             {
               printf ("In buildTypedefDeclaration_nfi(): base_decl->get_definingDeclaration()->get_parent()         = %p \n",base_decl->get_definingDeclaration()->get_parent());
             }
          printf ("In buildTypedefDeclaration_nfi(): base_decl->get_firstNondefiningDeclaration() = %p \n",base_decl->get_firstNondefiningDeclaration());
          if (base_decl->get_firstNondefiningDeclaration() != NULL)
             {
               printf ("In buildTypedefDeclaration_nfi(): base_decl->get_firstNondefiningDeclaration()->get_parent() = %p \n",base_decl->get_firstNondefiningDeclaration()->get_parent());
             }
        }
#endif

#if 0
  // DQ (1/2/2010): Set the defining declaration to itself. (BAD IDEA).
     if (type_decl->get_definingDeclaration() == NULL)
          type_decl->set_definingDeclaration(type_decl);
#endif

#if 0
     printf ("Leaving buildTypedefDeclaration_nfi(): type_decl = %p type_decl->get_declaration() = %p \n",type_decl,type_decl->get_declaration());
#endif

     return type_decl;
   }


SgTemplateTypedefDeclaration*
SageBuilder::buildTemplateTypedefDeclaration_nfi(const SgName & name, SgType* base_type, SgScopeStatement* scope, bool has_defining_base )
   {
  // DQ (11/2/2014): Adding support for templated typedef.

     ROSE_ASSERT (base_type != NULL);

#if 0
     printf ("In buildTemplateTypedefDeclaration_nfi(): base_type = %p = %s \n",base_type,base_type->class_name().c_str());
#endif

     if (scope == NULL )
        {
          scope = SageBuilder::topScopeStack();
        }

  // We don't yet support bottom up construction for this node yet
     ROSE_ASSERT(scope != NULL);

     SgDeclarationStatement* base_decl = NULL;

  // DQ (3/20/2012): I don't remember why we need to provide the symbol for the scope of the
  // parent rather then the scope. But as I recall there was a special corner of C++ that
  // required this sort of support.
     SgSymbol* parent_scope = NULL;
     if (scope != NULL)
        {
#if 0
          printf ("In buildTemplateTypedefDeclaration_nfi(): scope = %p = %s calling get_symbol_from_symbol_table() \n",scope,scope->class_name().c_str());
#endif
          ROSE_ASSERT(scope->get_parent() != NULL);
          SgDeclarationStatement* declaration = isSgDeclarationStatement(scope->get_parent());
#if 0
          printf ("declaration = %p \n",declaration);
#endif
          if (declaration != NULL)
             {
#if 0
               printf ("Found a valid declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
               ROSE_ASSERT(declaration->get_firstNondefiningDeclaration() != NULL);
            // parent_scope = declaration->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table();
               parent_scope = declaration->search_for_symbol_from_symbol_table();

               ROSE_ASSERT(parent_scope != NULL);
             }
#if 0
          printf ("Check on the symbol associatd with the declaration associated with the base type \n");
#endif
       // DQ (2/28/2018): check out if the symbol associated with the declaration used for the base type used in this typedef has a symbol.
          SgNamedType* namedType = isSgNamedType(base_type);

       // DQ (3/4/2018): This might not always be true (sure enough it fails for Cxx11_tests/test2014_58.C).
       // ROSE_ASSERT(namedType != NULL);

          if (namedType != NULL)
             {
               SgDeclarationStatement* declarationStatement = namedType->get_declaration();

            // This might not always be true.
               ROSE_ASSERT(declarationStatement != NULL);
               if (declarationStatement != NULL)
                  {
#if 0
                    printf ("Found declaration for base_type = %p = %s declarationStatement = %p = %s \n",base_type,base_type->class_name().c_str(),declarationStatement,declarationStatement->class_name().c_str());
#endif
                    SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(declarationStatement);
                    if (templateInstantiationDecl != NULL)
                       {
                         SgName name = templateInstantiationDecl->get_name();
#if 0
                         printf ("templateInstantiationDecl = %p scope = %p = %s for name = %s \n",templateInstantiationDecl,scope,scope->class_name().c_str(),name.str());
#endif
                      // DQ (2/28/2018): Added debugging to track down redundnat symbol.
                         if (scope->lookup_template_typedef_symbol(name) != NULL)
                            {
                              printf ("Error: it appears that there is already a symbol in scope = %p = %s for name = %s \n",scope,scope->class_name().c_str(),name.str());
                            }
                         ROSE_ASSERT(scope->lookup_template_typedef_symbol(name) == NULL);
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
        }

#if 0
     printf ("In buildTemplateTypedefDeclaration_nfi(): parent_scope = %p \n",parent_scope);
     if (parent_scope != NULL)
        {
          printf ("parent_scope) = %p = %s \n",parent_scope,parent_scope->class_name().c_str());
        }
#endif

  // We need to add the template parameter and partial specialization support to the SgTemplateTypedefDeclaration IR node.
  // SgTemplateTypedefDeclaration* type_decl = new SgTemplateTypedefDeclaration(SgName(name), base_type, NULL, base_decl, parent_scope);
     SgTemplateTypedefDeclaration* type_decl = new SgTemplateTypedefDeclaration(name, base_type, NULL, base_decl, parent_scope);
     ROSE_ASSERT(type_decl != NULL);

  // TV (08/17/2018): moved it before building type as SgTypedefType::createType uses SgTemplateTypedefDeclaration::get_mangled_name which requires the scope to be set (else name of the associated type might not be unique)
     type_decl->set_scope(scope);
     type_decl->set_parent(scope);

  // DQ (2/27/2018): We now have to set the type explicitly.
     ROSE_ASSERT(type_decl->get_type() == NULL);

#if 0
     printf ("!!!!! In buildTemplateTypedefDeclaration_nfi(): Building the SgTypedefType for SgTemplateTypedefDeclaration type_decl = %p = %s \n",
          type_decl,type_decl->get_name().str());
#endif

     SgTypedefType* typedefType = SgTypedefType::createType(type_decl);
     ROSE_ASSERT(typedefType != NULL);

#if 0
     printf ("!!!!! In buildTemplateTypedefDeclaration_nfi(): DONE: building the SgTypedefType for SgTemplateTypedefDeclaration type_decl = %p = %s typedefType = %p \n",
          type_decl,type_decl->get_name().str(),typedefType);
#endif

  // DQ (2/27/2018): It is an inconsistancy for the type to be set here.
     type_decl->set_type(typedefType);

  // DQ (2/27/2018): This should be non-null, since we just built the new type.
     ROSE_ASSERT(type_decl->get_type() != NULL);

#if 0
     printf ("In buildTemplateTypedefDeclaration_nfi(): After SgTemplateTypedefDeclaration constructor: type_decl->get_scope() = %p \n",type_decl->get_scope());
#endif

  // DQ (3/20/2012): Comment ouly, these are always set this way. first defining is a self reference, and defining is always NULL (required for AST consistancy)).
     type_decl->set_firstNondefiningDeclaration (type_decl);
     type_decl->set_definingDeclaration(NULL);

  // Set the source code position information.
     setOneSourcePositionNull(type_decl);

//     if (scope != NULL)
//        {
#if 0
          printf ("Test 2: Check on the symbol associatd with the declaration associated with the base type \n");
#endif
       // DQ (2/28/2018): check out if the symbol associated with the declaration used for the base type used in this typedef has a symbol.
          SgNamedType* namedType = isSgNamedType(base_type);

       // DQ (3/4/2018): This might not always be true (sure enough it fails for Cxx11_tests/test2014_58.C).
       // ROSE_ASSERT(namedType != NULL);

          if (namedType != NULL)
             {
               SgDeclarationStatement* declarationStatement = namedType->get_declaration();

            // This might not always be true.
               ROSE_ASSERT(declarationStatement != NULL);
               if (declarationStatement != NULL)
                  {
#if 0
                    printf ("Found declaration for base_type = %p = %s declarationStatement = %p = %s \n",base_type,base_type->class_name().c_str(),declarationStatement,declarationStatement->class_name().c_str());
#endif
                    SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(declarationStatement);
                    if (templateInstantiationDecl != NULL)
                       {
                         SgName name = templateInstantiationDecl->get_name();
#if 0
                         printf ("templateInstantiationDecl = %p scope = %p = %s for name = %s \n",templateInstantiationDecl,scope,scope->class_name().c_str(),name.str());
#endif
                      // DQ (2/28/2018): Added debugging to track down redundnat symbol.
                         if (scope->lookup_template_typedef_symbol(name) != NULL)
                            {
                              printf ("Error: it appears that there is already a symbol in scope = %p = %s for name = %s \n",scope,scope->class_name().c_str(),name.str());
                            }
                         ROSE_ASSERT(scope->lookup_template_typedef_symbol(name) == NULL);
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }

       // SgTypedefSymbol* typedef_symbol = new SgTypedefSymbol(type_decl);
          SgTemplateTypedefSymbol* typedef_symbol = new SgTemplateTypedefSymbol(type_decl);
          ROSE_ASSERT(typedef_symbol);

       // DQ (5/16/2013): This is the code we want now that we have implemented the namespace support behind the scope symbol bable interface.
       // scope->insert_symbol(SgName(name),typedef_symbol);
          scope->insert_symbol(name, typedef_symbol);


          ROSE_ASSERT(scope->lookup_template_typedef_symbol(name) != NULL);
//        }

#if 0
  // We have to setup the template arguments (need specialization and partial specialization support).
     printf ("Template parameters not setup in buildTemplateTypedefDeclaration_nfi() \n");
#endif

#if 0
     printf ("Leaving buildTemplateTypedefDeclaration_nfi(): type_decl->get_name()             = %s \n",type_decl->get_name().str());
     printf ("Leaving buildTemplateTypedefDeclaration_nfi(): type_decl->get_type()             = %p \n",type_decl->get_type());
     printf ("Leaving buildTemplateTypedefDeclaration_nfi(): type_decl->get_type()->get_name() = %s \n",type_decl->get_type()->get_name().str());
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     return type_decl;
   }


// ROSE_DLL_API SgTemplateInstantiationTypedefDeclaration*
// SageBuilder::buildTemplateInstantiationTypedefDeclaration_nfi(SgName name, SgType* base_type, SgScopeStatement* scope, bool has_defining_base, SgTemplateTypedefDeclaration* templateTypedefDeclaration, SgTemplateArgumentPtrList templateArgumentList)
// ROSE_DLL_API SgTemplateInstantiationTypedefDeclaration*
// SageBuilder::buildTemplateInstantiationTypedefDeclaration_nfi(SgName name, SgType* base_type, SgScopeStatement* scope, bool has_defining_base, SgTemplateTypedefDeclaration* templateTypedefDeclaration)
// ROSE_DLL_API SgTemplateInstantiationTypedefDeclaration*
// SageBuilder::buildTemplateInstantiationTypedefDeclaration_nfi()
ROSE_DLL_API SgTemplateInstantiationTypedefDeclaration*
SageBuilder::buildTemplateInstantiationTypedefDeclaration_nfi(SgName & name, SgType* base_type, SgScopeStatement* scope, bool has_defining_base, SgTemplateTypedefDeclaration* templateTypedefDeclaration, SgTemplateArgumentPtrList & templateArgumentsList)
   {
  // DQ (11/2/2014): Adding support for instantiation of templated typedef.

#if 0
  // Temporary fix for problems in linking with this function parameter.
     SgName name = "ERROR_SgTemplateInstantiationTypedefDeclaration";
     SgType* base_type = NULL;
     SgScopeStatement* scope = NULL;
     bool has_defining_base = false;
     SgTemplateTypedefDeclaration* templateTypedefDeclaration = NULL;

  // Temporary fix for problems in linking with this function parameter.
     SgTemplateArgumentPtrList templateArgumentList;
#endif

     ROSE_ASSERT (base_type != NULL);
  // ROSE_ASSERT(templateArgumentList != NULL);

#if 0
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): name = %s base_type = %p = %s \n",name.str(),base_type,base_type->class_name().c_str());
#endif

     SgName nameWithoutTemplateArguments = name;
     SgName nameWithTemplateArguments    = nameWithoutTemplateArguments;
     nameWithTemplateArguments = appendTemplateArgumentsToName(nameWithoutTemplateArguments,templateArgumentsList);

#if 0
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): nameWithoutTemplateArguments = %s \n",nameWithoutTemplateArguments.str());
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): nameWithTemplateArguments    = %s \n",nameWithTemplateArguments.str());
#endif

  // We don't yet support bottom up construction for this node yet
     ROSE_ASSERT(scope != NULL);

     SgDeclarationStatement* base_decl = NULL;

  // DQ (3/20/2012): I don't remember why we need to provide the symbol for the scope of the
  // parent rather then the scope. But as I recall there was a special corner of C++ that
  // required this sort of support.
     SgSymbol* parent_scope = NULL;
     if (scope != NULL)
        {
#if 0
          printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): scope = %p = %s calling get_symbol_from_symbol_table() \n",scope,scope->class_name().c_str());
#endif
          ROSE_ASSERT(scope->get_parent() != NULL);
          SgDeclarationStatement* declaration = isSgDeclarationStatement(scope->get_parent());
#if 0
          printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): declaration = %p \n",declaration);
#endif
          if (declaration != NULL)
             {
#if 0
               printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): Found a valid declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
               ROSE_ASSERT(declaration->get_firstNondefiningDeclaration() != NULL);
            // parent_scope = declaration->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table();
               parent_scope = declaration->search_for_symbol_from_symbol_table();

               ROSE_ASSERT(parent_scope != NULL);
             }
        }

  // DQ (11/5/2014): I think this might be set afterward.
     ROSE_ASSERT(templateTypedefDeclaration != NULL);

#if 0
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): parent_scope = %p \n",parent_scope);
#endif

  // DQ (2/25/2018): Note that the constructor calls the post_construction initialization which sets up the type.
  // Since this happens for the SgtypedefDeclaration (calling the SgtypedefType::createType() function, we get a SgTypedefType.

  // DQ (2/28/2018): Added debugging to track down redundnat symbol.
  // if (scope->lookup_template_typedef_symbol(nameWithTemplateArguments) != NULL)
  // SgSymbol* prexisting_symbol = scope->lookup_template_typedef_symbol(nameWithTemplateArguments);
  // if (prexisting_symbol != NULL)
  //    {
  //      printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): prexisting_symbol = %p = %s \n",prexisting_symbol,prexisting_symbol->class_name().c_str());
  //    }
  // SgTemplateTypedefSymbol* prexisting_template_typedef_symbol = isSgTemplateTypedefSymbol(prexisting_symbol);
     SgTemplateTypedefSymbol* prexisting_template_typedef_symbol = scope->lookup_template_typedef_symbol(nameWithTemplateArguments);
     if (prexisting_template_typedef_symbol != NULL)
        {
#if 0
          printf ("Error: it appears that there is already a symbol in scope = %p = %s for name = %s \n",scope,scope->class_name().c_str(),name.str());

          printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): DONE: calling SgTemplateInstantiationTypedefDeclaration constructor \n");
       // printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl->get_name()         = %s \n",type_decl->get_name().str());
       // printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl->get_templateName() = %s \n",type_decl->get_templateName().str());
#endif
       // DQ (3/4/2018): If we have previously processed this typedef and there is a symbol in place, then return the declaration from that symbol.
          SgDeclarationStatement* declarationStatement = prexisting_template_typedef_symbol->get_declaration();
          ROSE_ASSERT(declarationStatement != NULL);
#if 0
          printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): Reusing the declaration from the symbol:\n");
          printf ("    declarationStatement = %p (%s)\n", declarationStatement, declarationStatement ? declarationStatement->class_name().c_str() : "");
#endif
          SgTemplateInstantiationTypedefDeclaration* return_declaration = isSgTemplateInstantiationTypedefDeclaration(declarationStatement);
          ROSE_ASSERT(return_declaration != NULL);
#if 0
          printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): Reusing the declaration from the symbol: return_declaration = %p \n",return_declaration);
          printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): return_declaration->get_firstNondefiningDeclaration()       = %p \n",return_declaration->get_firstNondefiningDeclaration());
          printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): return_declaration->get_definingDeclaration()               = %p \n",return_declaration->get_definingDeclaration());
#endif

          return return_declaration;
        }
     ROSE_ASSERT(scope->lookup_template_typedef_symbol(nameWithTemplateArguments) == NULL);

#if 0
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): calling SgTemplateInstantiationTypedefDeclaration constructor \n");
#endif

  // DQ (2/25/2018): Not clear if we want to use the template name with arguments.
  // Calling: SgTemplateInstantiationTypedefDeclaration(SgName, SgType*, SgTypedefType*, SgDeclarationStatement*, SgSymbol*, SgTemplateTypedefDeclaration*, SgTemplateArgumentPtrList)
     SgTypedefType* typedefType = NULL;
  // SgTemplateInstantiationTypedefDeclaration* type_decl =
  //      new SgTemplateInstantiationTypedefDeclaration(name, base_type, typedefType, base_decl, parent_scope, templateTypedefDeclaration, templateArgumentsList);
     SgTemplateInstantiationTypedefDeclaration* type_decl =
          new SgTemplateInstantiationTypedefDeclaration(nameWithTemplateArguments, base_type, typedefType, base_decl, parent_scope, templateTypedefDeclaration, templateArgumentsList);
     ROSE_ASSERT(type_decl != NULL);

  // DQ (2/27/2018): This is a change in the constructor semantics, we now have to build the type explicitly.
  // printf ("We now have to build the type explicitly \n");
     ROSE_ASSERT(type_decl->get_type() == NULL);

#if 0
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): DONE: calling SgTemplateInstantiationTypedefDeclaration constructor \n");
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl->get_name()         = %s \n",type_decl->get_name().str());
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl->get_templateName() = %s \n",type_decl->get_templateName().str());
#endif

  // DQ (2/27/2018): Set the template name that this instantiation is using.
     type_decl->set_templateName(nameWithoutTemplateArguments);

#if 0
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl->get_templateName() = %s \n",type_decl->get_templateName().str());
#endif
#if 0
     printf ("Calling type_decl->set_parent(parent_scope) nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
#endif

  // ROSE_ASSERT(parent_scope != NULL);
  // type_decl->set_parent(parent_scope);
     ROSE_ASSERT(scope != NULL);
  // type_decl->set_parent(scope);
     type_decl->set_scope(scope);

#if 0
     printf ("&&&&& Calling reset template nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
#endif
#if 0
     printf ("BEFORE: type_decl->get_name()         = %s \n",type_decl->get_name().str());
     printf ("BEFORE: type_decl->get_templateName() = %s \n",type_decl->get_templateName().str());
#endif

#if 0
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): Skipping call to type_decl->resetTemplateName() \n");
#endif
  // type_decl->resetTemplateName();

  // DQ (3/1/2018): A bug in the name of the template with arguments has been detected (extra spaces in
  // the name generated by type_decl->resetTemplateName()), make sure that we have a consistant naming.
     ROSE_ASSERT(type_decl->get_name() == nameWithTemplateArguments);

  // DQ (2/28/2018): Added debugging to track down redundnat symbol.
     if (scope->lookup_template_typedef_symbol(nameWithTemplateArguments) != NULL)
        {
          printf ("Error: it appears that there is already a symbol in scope = %p = %s for name = %s \n",scope,scope->class_name().c_str(),name.str());
        }
     ROSE_ASSERT(scope->lookup_template_typedef_symbol(nameWithTemplateArguments) == NULL);

#if 0
     printf ("AFTER: type_decl->get_name()         = %s \n",type_decl->get_name().str());
     printf ("AFTER: type_decl->get_templateName() = %s \n",type_decl->get_templateName().str());
#endif

  // DQ (4/15/2018): I don't think we want to reset the template name and certainly not to a name that
  // includes template arguments (which is inconsistant with all other usage).
  // type_decl->set_templateName(type_decl->get_name());

#if 0
     printf ("AFTER: type_decl->set_templateName(): type_decl->get_templateName() = %s \n",type_decl->get_templateName().str());
#endif
#if 0
     printf ("&&&&& DONE: Calling reset template nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
     ROSE_ASSERT(type_decl->get_templateName() != "");
#endif
#if 0
     printf ("##### Calling SgTypedefType::createType(type_decl) nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
#endif

  // DQ (2/26/2018): Can we assert this?
     ROSE_ASSERT(type_decl->get_type() == NULL);

#if 0
     printf ("!!!!! In buildTemplateInstantiationTypedefDeclaration_nfi(): DONE: building the SgTypedefType for SgTemplateTypedefDeclaration type_decl = %p = %s \n",
          type_decl,type_decl->get_name().str());
#endif

     SgTypedefType* new_typedefType = SgTypedefType::createType(type_decl);
     ROSE_ASSERT(new_typedefType != NULL);

#if 0
     printf ("!!!!! In buildTemplateInstantiationTypedefDeclaration_nfi(): DONE: building the SgTypedefType for SgTemplateTypedefDeclaration type_decl = %p = %s new_typedefType = %p \n",
          type_decl,type_decl->get_name().str(),new_typedefType);
#endif

     type_decl->set_type(new_typedefType);

#if 0
     printf ("##### DONE: Calling SgTypedefType::createType(templateInstantiationTypedefDeclaration) nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
     printf ("   --- new_typedefType = %p \n",new_typedefType);
#endif

  // DQ (2/25/2018): Adding the support to set the template name for the instantiation.
  // type_decl->set_templateName(nameWithTemplateArguments);

  // DQ (4/25/2018): The type should not be set yet, acutally it is set via the SgTemplateInstantiationTypedefDeclaration::post_construction_initialization() function.
  // ROSE_ASSERT(type_decl->get_type() == NULL);
     ROSE_ASSERT(type_decl->get_type() != NULL);

  // DQ (2/26/2018): This was already done above (to support the resetTemplateName() function.
  // ROSE_ASSERT(type_decl->get_scope() == NULL);
  // type_decl->set_scope(scope);

     ROSE_ASSERT(type_decl->get_scope() != NULL);

  // DQ (4/25/2018): The type should not be set yet.
  // ROSE_ASSERT(type_decl->get_type() == NULL);
     ROSE_ASSERT(type_decl->get_type() != NULL);

#if 0
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): After SgTemplateInstantiationTypedefDeclaration constructor: type_decl->get_scope() = %p \n",type_decl->get_scope());
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): After SgTemplateInstantiationTypedefDeclaration constructor: type_decl->get_type()  = %p \n",type_decl->get_type());
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): After SgTemplateInstantiationTypedefDeclaration constructor: typedefType  = %p \n",typedefType);
#endif

  // DQ (4/25/2018): The type should not be set yet.
  // ROSE_ASSERT(type_decl->get_type() == NULL);

  // DQ (3/20/2012): Comment ouly, these are always set this way. first defining is a self reference, and defining is always NULL (required for AST consistancy)).
     type_decl->set_firstNondefiningDeclaration (type_decl);
     type_decl->set_definingDeclaration(NULL);

  // Set the source code position information.
     setOneSourcePositionNull(type_decl);

     SgName mangled_name = type_decl->get_mangled_name();

#if 0
     printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): mangled_name                 = %s \n",mangled_name.str());
#endif

  // DQ (2/19/2018): Calling function to set the template arguments (before adding the symbol (in case that makes a difference).
     setTemplateArgumentsInDeclaration(type_decl,&templateArgumentsList);

  // DQ (2/24/2018): All typedefs are non-defining declarations (since they can appears redundantly in C or C++, including C++11).
  // DQ (2/20/2018): We also need to test some rules about the firstNondefiningDeclaration and definingDeclaration declarations as well.
  // ROSE_ASSERT(type_decl->get_firstNondefiningDeclaration() == NULL);
  // ROSE_ASSERT(type_decl->get_definingDeclaration() == type_decl);
     ROSE_ASSERT(type_decl->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(type_decl->get_definingDeclaration() == NULL);
     ROSE_ASSERT(type_decl->get_firstNondefiningDeclaration() == type_decl);

  // DQ (4/25/2018): The type should not be set yet.
  // ROSE_ASSERT(type_decl->get_type() == NULL);

#if 0
  // DQ (2/25/2018): The reason we have to rest this type is because the SgTypedefDeclaration::post_constructor_initialization() set the type and, maybe, to the wrong type.
  // I have fixed this by implementing a SgTemplateInstantiationTypedefDeclaration::post_constructor_initialization() (to set it to a different type).
     if (type_decl->get_type() != NULL)
        {
          printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl->get_type() = %p = %s \n",type_decl->get_type(),type_decl->get_type()->class_name().c_str());
          SgTypedefType* typedefType = isSgTypedefType(type_decl->get_type());
          ROSE_ASSERT(typedefType != NULL);
          printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): typedefType->get_name()         = %s \n",typedefType->get_name().str());
          SgDeclarationStatement* declarationStatement = typedefType->get_declaration();
          ROSE_ASSERT(declarationStatement != NULL);
          printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): typedefType->get_declaration() = %p = %s \n",declarationStatement,declarationStatement->class_name().c_str());

          SgTemplateTypedefDeclaration* templateTypedefDeclaration = isSgTemplateTypedefDeclaration(declarationStatement);
          printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): templateTypedefDeclaration = %p = %s \n",templateTypedefDeclaration,templateTypedefDeclaration->get_name().str());
       // printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): templateTypedefDeclaration templateName = %s \n",templateTypedefDeclaration->get_templateName().str());

       // printf ("In buildTemplateInstantiationTypedefDeclaration_nfi(): typedefType->get_templateName() = %s \n",typedefType->get_templateName().str());
        }
#endif

     ROSE_ASSERT(type_decl->get_type() != NULL);

#if 0
  // DQ (2/20/2018): This should not alread be set.
     ROSE_ASSERT(type_decl->get_type() == NULL);

  // DQ (2/20/2018): Note that the other build functions (e.g. for class declarations) all build the associated type, so we need to do the same.
  // nondefdecl->set_type(SgClassType::createType(firstNondefdecl));
  // type_decl->set_type(SgTemplateType::createType(type_decl));
  // SgType* templateTypedefType = SgTemplateType::createType(type_decl);
     SgTypedefType* templateTypedefType = SgTypedefType::createType(type_decl);
     ROSE_ASSERT(templateTypedefType != NULL);
     type_decl->set_type(templateTypedefType);

  // SgType* baseType = type_decl->get_baseType();
  // ROSE_ASSERT(baseType != NULL);
  // type_decl->set_type(baseType);
#endif

  // DQ (2/28/2018): Added assertion (I think we can do this).
     ROSE_ASSERT(scope != NULL);

     if (scope != NULL)
        {
       // DQ (2/28/2018): Added assertion (I think we can do this).

          if (scope->lookup_template_typedef_symbol(nameWithTemplateArguments) != NULL)
             {
               printf ("Error: it appears that there is already a symbol in scope = %p = %s for nameWithTemplateArguments = %s \n",scope,scope->class_name().c_str(),nameWithTemplateArguments.str());
             }
          ROSE_ASSERT(scope->lookup_template_typedef_symbol(nameWithTemplateArguments) == NULL);

       // SgTypedefSymbol* typedef_symbol = new SgTypedefSymbol(type_decl);
          SgTemplateTypedefSymbol* typedef_symbol = new SgTemplateTypedefSymbol(type_decl);
          ROSE_ASSERT(typedef_symbol);

       // DQ (5/16/2013): This is the code we want now that we have implemented the namespace support behind the scope symbol bable interface.
       // scope->insert_symbol(Sgname(name),typedef_symbol);
       // scope->insert_symbol(name,typedef_symbol);
       // scope->insert_symbol(name,typedef_symbol);
          scope->insert_symbol(nameWithTemplateArguments,typedef_symbol);
          type_decl->set_scope(scope);

       // DQ (3/20/2012): Comment added only: The parent is always the scope in this case, I think.
          type_decl->set_parent(scope);

       // DQ (2/19/2018): Added test (but I think we are missing the template arguments).
       // ROSE_ASSERT(scope->lookup_template_typedef_symbol(name) != NULL);
          ROSE_ASSERT(scope->lookup_template_typedef_symbol(nameWithTemplateArguments) != NULL);
        }

#if 0
  // We have to setup the template arguments (need specialization and partial specialization support).
     printf ("Template parameters not setup in buildTemplateTypedefDeclaration_nfi() \n");
#endif

#if 0
     printf ("Leaving buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl = %p = %s name = %s \n",type_decl,type_decl->class_name().c_str(),type_decl->get_name().str());
     printf ("Leaving buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl: name = %s templateName   = %s \n",type_decl->get_name().str(),type_decl->get_templateName().str());
     printf ("Leaving buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl: templateArguments.size() = %zu \n",type_decl->get_templateArguments().size());
     printf ("Leaving buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl: get_type()               = %p \n",type_decl->get_type());
     printf ("Leaving buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl: get_name()               = %s \n",type_decl->get_name().str());
     printf ("Leaving buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl: get_templateName()       = %s \n",type_decl->get_templateName().str());
     printf ("Leaving buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl: get_templateDeclaration()= %p \n",type_decl->get_templateDeclaration());
     ROSE_ASSERT(type_decl->get_templateDeclaration() != NULL);
     printf ("Leaving buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl: get_templateDeclaration()->get_type() = %p \n",type_decl->get_templateDeclaration()->get_type());
     printf ("Leaving buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl: get_templateDeclaration()->get_name() = %s \n",type_decl->get_templateDeclaration()->get_name().str());
#endif

#if 0
     printf ("Leaving buildTemplateInstantiationTypedefDeclaration_nfi(): type_decl: unparseNameToString() = %s \n",type_decl->unparseNameToString().c_str());
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     return type_decl;
   }





//-----------------------------------------------
// Assertion `definingDeclaration != NULL || firstNondefiningDeclaration != NULL'
SgFunctionParameterList *
SageBuilder::buildFunctionParameterList(SgInitializedName* in1, SgInitializedName* in2, SgInitializedName* in3, SgInitializedName* in4, SgInitializedName* in5, SgInitializedName* in6, SgInitializedName* in7, SgInitializedName* in8, SgInitializedName* in9, SgInitializedName* in10)
{
  SgFunctionParameterList *parameterList = new SgFunctionParameterList();
  ROSE_ASSERT (parameterList);

  parameterList->set_definingDeclaration (NULL);
  parameterList->set_firstNondefiningDeclaration (parameterList);

  setOneSourcePositionForTransformation(parameterList);

  if (in1) appendArg(parameterList, in1);
  if (in2) appendArg(parameterList, in2);
  if (in3) appendArg(parameterList, in3);
  if (in4) appendArg(parameterList, in4);
  if (in5) appendArg(parameterList, in5);
  if (in6) appendArg(parameterList, in6);
  if (in7) appendArg(parameterList, in7);
  if (in8) appendArg(parameterList, in8);
  if (in9) appendArg(parameterList, in9);
  if (in10) appendArg(parameterList, in10);

  return parameterList;
}

SgFunctionParameterList *
SageBuilder::buildFunctionParameterList_nfi() {
  SgFunctionParameterList *parameterList = new SgFunctionParameterList();
  ROSE_ASSERT (parameterList);
  parameterList->set_definingDeclaration (NULL);
  parameterList->set_firstNondefiningDeclaration (parameterList);

  setOneSourcePositionNull(parameterList);

  return parameterList;
}

//-----------------------------------------------
// no type vs. void type ?
SgFunctionParameterTypeList *
SageBuilder::buildFunctionParameterTypeList(SgFunctionParameterList* paralist)
   {
  // DQ (8/19/2012): I am not a fan of this sort of codeing style either (NULL pointers as inputs should be an error).
     if (paralist == NULL)
        {
          printf ("WARNING: In buildFunctionParameterTypeList(): Accepting NULL input and returning NULL pointer. \n");

          return NULL;
        }

  // DQ (8/18/2012): This is a problem, any valid list (even zero length) should result in a valid return list of types (even zero length).
  // if (paralist->get_args().size()==0)
  //      return NULL;

     SgFunctionParameterTypeList* typePtrList = new SgFunctionParameterTypeList;
     ROSE_ASSERT(typePtrList != NULL);

#if 0
     printf ("In SageBuilder::buildFunctionParameterTypeList(SgFunctionParameterList*): SgFunctionParameterTypeList* typePtrList = %p \n",typePtrList);
#endif

     SgInitializedNamePtrList args = paralist->get_args();
     SgInitializedNamePtrList::const_iterator i;
     for(i = args.begin(); i != args.end(); i++)
          (typePtrList->get_arguments()).push_back( (*i)->get_type() );

     setSourcePositionAtRootAndAllChildren(typePtrList);

     return typePtrList;
   }


SgFunctionParameterTypeList *
SageBuilder::buildFunctionParameterTypeList (SgExprListExp * expList)
   {
     if (expList ==NULL) return NULL;
     SgExpressionPtrList expPtrList = expList->get_expressions();

     SgFunctionParameterTypeList* typePtrList = new SgFunctionParameterTypeList;
     ROSE_ASSERT(typePtrList);

#if 0
     printf ("In SageBuilder::buildFunctionParameterTypeList(SgExprListExp*): SgFunctionParameterTypeList* typePtrList = %p \n",typePtrList);
#endif

     SgExpressionPtrList::const_iterator i;
     for (i=expPtrList.begin();i!=expPtrList.end();i++)
        {
          typePtrList->get_arguments().push_back( (*i)->get_type() );
        }

     setSourcePositionAtRootAndAllChildren(typePtrList);

     return typePtrList;
   }

SgFunctionParameterTypeList *
SageBuilder::buildFunctionParameterTypeList(SgType* type0, SgType* type1, SgType* type2, SgType* type3,
                                            SgType* type4, SgType* type5, SgType* type6, SgType* type7)
   {
     SgFunctionParameterTypeList* typePtrList = new SgFunctionParameterTypeList;
     ROSE_ASSERT(typePtrList);

#if 0
     printf ("In SageBuilder::buildFunctionParameterTypeList(SgType*,SgType*,...): SgFunctionParameterTypeList* typePtrList = %p \n",typePtrList);
#endif

     SgTypePtrList & types = typePtrList->get_arguments();

     if (type0 != NULL) types.push_back(type0);
     if (type1 != NULL) types.push_back(type1);
     if (type2 != NULL) types.push_back(type2);
     if (type3 != NULL) types.push_back(type3);
     if (type4 != NULL) types.push_back(type4);
     if (type5 != NULL) types.push_back(type5);
     if (type6 != NULL) types.push_back(type6);
     if (type7 != NULL) types.push_back(type7);

     return typePtrList;
   }

//-----------------------------------------------
// build function type,
//
// insert into symbol table when not duplicated
SgFunctionType *
SageBuilder::buildFunctionType(SgType* return_type, SgFunctionParameterTypeList* typeList)
   {
     ROSE_ASSERT(return_type != NULL);

  // DQ (8/19/2012): Can we enforce this?
     ROSE_ASSERT(typeList != NULL);

#if 0
     printf ("Inside of SageBuilder::buildFunctionType(SgType,SgFunctionParameterTypeList) \n");
     printf ("Inside of SageBuilder::buildFunctionType() return_type = %s \n",return_type->get_mangled().str());
     printf ("Inside of SageBuilder::buildFunctionType() typeList->get_arguments().size() = %" PRIuPTR " \n",typeList->get_arguments().size());
     for (size_t i=0; i < typeList->get_arguments().size(); i++)
        {
          ROSE_ASSERT(typeList->get_arguments()[i] != NULL);
          printf ("   --- typeList->get_arguments()[%zu] = %p = %s \n",i,typeList->get_arguments()[i],typeList->get_arguments()[i]->class_name().c_str());
        }
#endif
#if 0
  // DQ (1/21/2014): Activate this test to see how we are building SgFunctionType with return type as SgFunctionType (see test2014_53.c).
     printf ("Inside of SageBuilder::buildFunctionType() (activate test for return_type): return_type = %p = %s \n",return_type,return_type->class_name().c_str());
#endif
#if 0
  // DQ (1/21/2014): Activate this test to see how we are building SgFunctionType with return type as SgFunctionType (see test2014_53.c).
     if (isSgFunctionType(return_type) != NULL)
        {
       // Liao 12/14/2012. This is not true for some functions (e.g. findFunctionUsingDlopen() on top of dlopen()) returning a function type
          printf ("ERROR: Inside of SageBuilder::buildFunctionType(): function type can't be return type of function type (at least for debugging) \n");
          ROSE_ASSERT(false);
        }
#endif

     SgFunctionTypeTable * fTable = SgNode::get_globalFunctionTypeTable();
     ROSE_ASSERT(fTable);

  // This function make clever use of a static member function which can't be built
  // for the case of a SgMemberFunctionType (or at least not without more work).
     SgName typeName = SgFunctionType::get_mangled(return_type, typeList);

     SgFunctionType* funcType = isSgFunctionType(fTable->lookup_function_type(typeName));

#if 0
     printf ("In SageBuilder::buildFunctionType(): typeName = %s funcType = %p \n",typeName.str(),funcType);
     if (funcType != NULL)
        {
          printf ("In SageBuilder::buildFunctionType(): funcType->get_mangled_type() = %s \n",funcType->get_mangled_type().str());
        }
#endif

     if (funcType == NULL)
        {
       // Only build the new type if it can't be found in the global type table.
          funcType = new SgFunctionType(return_type, false);
          ROSE_ASSERT(funcType);

          if (typeList != NULL)
             {
            // DQ (12/5/2012): We want to avoid overwriting an existing SgFunctionParameterTypeList. Could be related to failing tests for AST File I/O.
               if (funcType->get_argument_list() != NULL)
                  {
                    delete funcType->get_argument_list();
                    funcType->set_argument_list(NULL);
                  }
               ROSE_ASSERT(funcType->get_argument_list() == NULL);

               funcType->set_argument_list(typeList);
               typeList->set_parent(funcType);
             }

#if 0
          printf ("Inserting function type = %s into function type table with type = %p \n",typeName.str(),funcType);
#endif

          fTable->insert_function_type(typeName,funcType);
        }
       else
        {
       // DQ (12/6/2012): Tracking down orphaned SgFunctionParameterTypeList objects.
#if 0
          printf ("In SageBuilder::buildFunctionType(): Note that the SgFunctionParameterTypeList* typeList = %p was not used (so should be deleted by which ever calling function allocated it) \n",typeList);
#endif
        }

#if 0
     printf ("Leaving buildFunctionType(): Returning function type = %p \n",funcType);
#endif

     return funcType;
   }



SgMemberFunctionType*
SageBuilder::buildMemberFunctionType(SgType* return_type, SgFunctionParameterTypeList* typeList, SgType *classType, unsigned int mfunc_specifier, unsigned int ref_qualifiers)
   {
  // DQ (8/19/2012): This is a refactored version of the buildMemberFunctionType() below so that we can
  // isolate out the part that uses a SgClassType from the version that uses the SgClassDefinition.

  // Maintain the global type table
     SgFunctionTypeTable* fTable = SgNode::get_globalFunctionTypeTable();
     ROSE_ASSERT(fTable != NULL);

  // DQ (12/6/2012): Added assertion.
  // ROSE_ASSERT(classType != NULL);

  // DQ (12/13/2012): Added assertion.
     ROSE_ASSERT(typeList != NULL);

  // DQ (12/6/2012): Newer simpler code (using static function SgMemberFunctionType::get_mangled()).
     SgName                typeName    = SgMemberFunctionType::get_mangled(return_type,typeList,classType,mfunc_specifier);
     SgType*               typeInTable = fTable->lookup_function_type(typeName);

#if 0
     printf ("In buildMemberFunctionType(SgType*,SgFunctionParameterTypeList*,SgType*,int,int): fTable->lookup_function_type(typeName = %s) = %p \n",typeName.str(),typeInTable);
     printf (" --- mfunc_specifier = %d ref_qualifiers = %d \n",mfunc_specifier,ref_qualifiers);
#endif

#if 1
  // DQ (1/10/2020): I think that these qualifiers are contained in the mfunc_specifier.
     if (ref_qualifiers > 0)
        {
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
        }
#endif

     SgMemberFunctionType* funcType = NULL;
     if (typeInTable == NULL)
        {
          bool has_ellipses = false;
          SgPartialFunctionType* partialFunctionType = new SgPartialFunctionType(return_type, has_ellipses, classType, mfunc_specifier, ref_qualifiers);
          ROSE_ASSERT(partialFunctionType != NULL);
#if 0
          printf ("Building a SgPartialFunctionType: partialFunctionType = %p \n",partialFunctionType);
          printf (" --- partialFunctionType->isLvalueReferenceFunc() = %s \n",partialFunctionType->isLvalueReferenceFunc() ? "true" : "false");
          printf (" --- partialFunctionType->isRvalueReferenceFunc() = %s \n",partialFunctionType->isRvalueReferenceFunc() ? "true" : "false");
#endif
       // DQ (12/5/2012): We want to avoid overwriting an existing SgFunctionParameterTypeList. Could be related to failing tests for AST File I/O.
          if (partialFunctionType->get_argument_list() != NULL)
             {
               delete partialFunctionType->get_argument_list();
               partialFunctionType->set_argument_list(NULL);
             }
          ROSE_ASSERT(partialFunctionType->get_argument_list() == NULL);

          typeList->set_parent(partialFunctionType);

       // DQ (12/6/2012): Set the SgFunctionParameterTypeList in the SgPartialFunctionType before trying
       // to build a SgMemberFunctionType (not critical that it be set before, but might be in the future,
       // but it is important that it be set).
          partialFunctionType->set_argument_list(typeList);

          ROSE_ASSERT(partialFunctionType->get_argument_list() != NULL);

       // The optional_fortran_type_kind is only required for Fortran support.
          SgExpression* optional_fortran_type_kind = NULL;
          funcType = SgMemberFunctionType::createType(partialFunctionType, optional_fortran_type_kind);

       // DQ (12/13/2012): Remove the SgPartialFunctionType after it has been used to build the SgMemberFunctionType.
       // I would rather modify the SgMemberFunctionType::createType() API so that we didn't use the SgPartialFunctionType IR nodes.
       // First we have to reset the pointer to the type argument list to NULL since it is shared with the SgMemberFunctionType.
          partialFunctionType->set_argument_list(NULL);

       // Then we can delete the SgPartialFunctionType.
          delete partialFunctionType;
          partialFunctionType = NULL;

       // This is perhaps redundant since it was set to a derived class (but might be an important distiction).
          typeList->set_parent(funcType);

          ROSE_ASSERT(funcType->get_argument_list() != NULL);
        }

#if 0
     printf ("########### In buildMemberFunctionType(): Looking in global function type table for member function type = %p name = %s typeInTable = %p \n",funcType,typeName.str(),typeInTable);
#endif

     if (typeInTable == NULL)
        {
#if 0
          printf ("########### In buildMemberFunctionType(): Adding funcType = %p = %s to global function type table \n",funcType,typeName.str());
#endif
          ROSE_ASSERT(funcType != NULL);
          fTable->insert_function_type(typeName,funcType);
        }
       else
        {
       // DQ (12/3/2011): Added this case to support reuse of function types (not handled by the createType functions).
       // Delete the one generated so that we could form the mangled name.

       // printf ("Deleting funcType = %p = %s \n",funcType,funcType->class_name().c_str());

       // DQ (3/22/2012): Added assertion.
          ROSE_ASSERT(typeInTable != funcType);

          delete funcType;
          funcType = NULL;
       // Return the one from the global type table.
          funcType = isSgMemberFunctionType(typeInTable);
          ROSE_ASSERT(funcType != NULL);
        }

#if 0
     fTable->get_function_type_table()->print("In buildMemberFunctionType(): globalFunctionTypeTable AFTER");
#endif

     return funcType;
   }


// DQ (1/4/2009): Need to finish this!!!
//-----------------------------------------------
// build member function type,
//
// insert into symbol table when not duplicated
SgMemberFunctionType*
SageBuilder::buildMemberFunctionType(SgType* return_type, SgFunctionParameterTypeList* typeList, SgScopeStatement * struct_name, unsigned int mfunc_specifier, unsigned int ref_qualifiers)
   {
  // This function has to first build a version of the SgMemberFunctionType so that it can generate a mangled name.
  // If the mangled name can be use to lookup a SgMemberFunctionType then the "just built" SgMemberFunctionType
  // is deleted and the one from the global function type table is returned.  This fixes a lot of subtle C++
  // specific issues with the build interface and it's use with the newer EDG 4.3 connection to ROSE.

     ROSE_ASSERT(return_type != NULL);

  // SgMemberFunctionType (SgType *return_type=NULL, bool has_ellipses=true, SgClassDefinition *struct_name=NULL, unsigned int mfunc_specifier=0)
  // SgMemberFunctionType * funcType = new SgMemberFunctionType(return_type, false);

     ROSE_ASSERT(struct_name != NULL);

#if 0
     printf("In buildMemberFunctionType():\n");
     printf("  - struct_name = %p (%s)\n", struct_name, struct_name->class_name().c_str());
#endif

#if 0
  // DQ (1/9/2020): Unclear why this function is not using the ref_qualifiers.
     printf ("SageBuilder::buildMemberFunctionType(SgType*,SgFunctionParameterTypeList*,SgScopeStatement*,int,int): This function does not use the input ref_qualifiers = %x \n",ref_qualifiers);
     printf (" --- mfunc_specifier = %d ref_qualifiers = %d \n",mfunc_specifier,ref_qualifiers);
#endif

#if 1
     if (ref_qualifiers > 0)
       {
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
       }
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     ROSE_ASSERT(struct_name->get_parent() != NULL);
  // ROSE_ASSERT(struct_name->get_declaration() != NULL);

#if 0
     printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
     printf ("In buildMemberFunctionType() return_type = %p typeList = %p struct_name = %p = %s = %s mfunc_specifier = %u \n",
          return_type,typeList,struct_name,struct_name->class_name().c_str(),struct_name->get_declaration()->get_name().str(),mfunc_specifier);
#endif

  // SgDeclarationStatement* declaration = struct_name->get_declaration();
     SgClassDefinition* classDefinition = isSgClassDefinition(struct_name);
     SgDeclarationScope* decl_scope = isSgDeclarationScope(struct_name);

     ROSE_ASSERT(classDefinition != NULL || decl_scope != NULL);

     SgDeclarationStatement* declaration = NULL;
     if (classDefinition != NULL)
        {
          declaration = classDefinition->get_declaration();
        }
       else if (decl_scope != NULL)
        {
          declaration = isSgDeclarationStatement(decl_scope->get_parent());
        }
       else
        {
          ROSE_ASSERT(false);
        }

     ROSE_ASSERT(declaration != NULL);

     if (typeList != NULL)
        {
#if 0
          SgTypePtrList & typeListArgs = typeList->get_arguments();
          for (SgTypePtrList::iterator i = typeListArgs.begin(); i != typeListArgs.end(); i++)
             {
               printf ("   --- type argument = %p = %s \n",*i,(*i)->class_name().c_str());
             }
#endif
        }
       else
        {
          printf ("WARNING: typeList == NULL \n");
        }

     SgClassDeclaration*         classDeclaration         = isSgClassDeclaration(declaration);
  // SgTemplateClassDeclaration* templateClassDeclaration = isSgTemplateClassDeclaration(declaration);
     SgNonrealDecl *             nrdecl                   = isSgNonrealDecl(declaration);

     ROSE_ASSERT(classDeclaration != NULL || nrdecl != NULL);

     SgMemberFunctionType* funcType = NULL;

  // DQ (12/4/2011): Modified SgClassType to support template declarations (SgTemplateClassDeclaration now contains a type set to SgClassType.
  // The SgClassType has been modified (browdened) to support a SgDeclarationStatement instead of a SgClassDeclaration.
  // SgClassType* classType = classDeclaration->get_type();
     SgType* classType = NULL;
     if (classDeclaration != NULL)
        {
          classType = classDeclaration->get_type();
        }
       else if (decl_scope != NULL)
        {
          classType = nrdecl->get_type();
        }
       else
        {
          ROSE_ASSERT(false);
        }

     ROSE_ASSERT(classType != NULL);

#if 0
     printf ("In buildMemberFunctionType(): Calling refactored function: buildMemberFunctionType(...,classType = %p,...) \n",classType);
#endif

  // DQ (8/19/2012): This code has been refactored.
     funcType = buildMemberFunctionType(return_type,typeList,classType,mfunc_specifier);

#if 0
     printf ("In buildMemberFunctionType(): DONE: Calling refactored function: buildMemberFunctionType(...,classType = %p,...) \n",classType);
#endif

     return funcType;
   }


//----------------------------------------------------
//! Build an opaque type with a name, useful when a type's details are unknown during transformation, especially for a runtime library's internal type.
SgType * SageBuilder::buildOpaqueType(std::string const name, SgScopeStatement * scope)
{
  // we require users to specify a target scope
  ROSE_ASSERT(scope);
  SgTypedefDeclaration* type_decl = NULL;
  SgTypedefType* result = NULL;

  // Liao and Greg Bronevetsky , 8/27/2009
  // patch up the symbol
  // and avoid duplicated creation
  // TODO  a function like fixTypeDeclaration() (similar to SageInterface::fixVariableDeclaration()) for this
  SgTypedefSymbol* type_symbol = scope->lookup_typedef_symbol(name);
  if (type_symbol == NULL)
  {
    type_decl =  new SgTypedefDeclaration(name,buildIntType(),NULL, NULL, NULL);
    ROSE_ASSERT(type_decl);

#if 0
     printf ("In SageBuilder::buildOpaqueType(): calling SgTypedefType::createType() using this = %p = %s \n",type_decl,type_decl->class_name().c_str());
#endif

  // DQ (2/27/2018): Add this call here to reflect change to the constructor semantics.
     type_decl->set_type(SgTypedefType::createType(type_decl));

#if 0
     printf ("In SageBuilder::buildOpaqueType(): DONE calling SgTypedefType::createType() using this = %p = %s \n",type_decl,type_decl->class_name().c_str());
#endif

#if 0
 // DQ (3/28/2018): Commented out sinnce this fails for the documentation generation step in ROSE.
 // DQ (2/27/2018): The constructor semantics now require the type to be built after the IR node has been built.
 // Make this fail so that we can detect it an fix it properly later.
    printf ("In SageBuilder::buildOpaqueType(): ERROR: The constructor semantics now require the type to be built after the IR node has been built \n");
    ROSE_ASSERT(false);
#endif

    type_symbol = new SgTypedefSymbol(type_decl);
    ROSE_ASSERT(type_symbol);
    SgName n = name;

 // DQ (5/21/2013): The symbol table should only be accessed through the SgScopeStatement interface.
 // scope->get_symbol_table()->insert(n, type_symbol);
    scope->insert_symbol(n,type_symbol);

    type_decl->set_firstNondefiningDeclaration (type_decl);
    setOneSourcePositionForTransformation(type_decl);
    prependStatement(type_decl,scope);
    // Hide it from unparser
    Sg_File_Info* file_info = type_decl->get_file_info();
    file_info->unsetOutputInCodeGeneration ();
    result = new SgTypedefType(type_decl);
  }
  else
  {
    type_decl = type_symbol->get_declaration();
    result = type_decl->get_type();
  }
  ROSE_ASSERT(result);
  return result;
}


//----------------- function type------------
// same function declarations (defining or nondefining) should share the same function type!
SgFunctionType*
SageBuilder::buildFunctionType(SgType* return_type, SgFunctionParameterList * argList)
   {
#if 0
     printf ("Inside of SageBuilder::buildFunctionType(SgType,SgFunctionParameterList) \n");
#endif

  // DQ (8/19/2012): Can we assert this?
     ROSE_ASSERT(argList != NULL);

     SgFunctionParameterTypeList* typeList  = buildFunctionParameterTypeList(argList);

  // DQ (8/19/2012): Can we assert this?
     ROSE_ASSERT(typeList != NULL);

     SgFunctionType* func_type = buildFunctionType(return_type, typeList);

     if (func_type->get_argument_list() != typeList)
        {
#if 0
          printf ("WARNING: the generated SgFunctionParameterTypeList* typeList = %p was not used and should be deleted \n",typeList);
#endif
          delete typeList;
          typeList = NULL;
        }

     return func_type;
   }


#if 0
// DQ (3/20/2017): This function is not used.

// DQ (12/1/2011): Added similar function for SgMemberFunctionType as for SgFunctionType
// (required for use in template function buildNondefiningFunctionDeclaration_T<T>()).
SgMemberFunctionType*
SageBuilder::buildMemberFunctionType(SgType* return_type, SgFunctionParameterList* argList, SgClassDefinition* classDefinition, /* const, volatile, restrict support */ unsigned int mfunc_specifier)
   {
#if 0
     printf ("Inside of SageBuilder::buildMemberFunctionType(SgType*,SgFunctionParameterList*,SgClassDefinition*,unsigned int) \n");
#endif

     SgFunctionParameterTypeList* typeList  = buildFunctionParameterTypeList(argList);
     SgMemberFunctionType*        func_type = buildMemberFunctionType(return_type, typeList, classDefinition, mfunc_specifier);

     return func_type;
   }
#endif

void
checkThatNoTemplateInstantiationIsDeclaredInTemplateDefinitionScope ( SgDeclarationStatement* func, SgScopeStatement* scope )
   {
  // DQ (12/14/2011): We need the parent to be set so that we can call some of the test functions
  // (e.g assert that get_class_scope() for member functions).  So we set the parent to the scope
  // by default and see if this will work, else we could disable to assertion that the parent is
  // non-null in the get_class_scope() member function.

     if (isSgMemberFunctionDeclaration(func) != NULL)
        {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
          printf ("WARNING: setting parent of function to match scope by default \n");
#endif
          func->set_parent(scope);

          ROSE_ASSERT(scope != NULL);

          if (isSgTemplateInstantiationMemberFunctionDecl(func) != NULL)
             {
            // DQ (12/14/2011): We should not have a member function template instantiation in a template class definition.
#if 0
            // DQ (8/25/2014): Allow non-template functions in a template class declaration (see test2014_161.C).
               if (isSgTemplateClassDefinition(scope) != NULL)
                  {
                    printf ("In checkThatNoTemplateInstantiationIsDeclaredInTemplateDefinitionScope(): p->source_corresp.is_class_member == true: Allow non-template functions in a template class declaration \n");
                  }
#endif

            // DQ (8/25/2014): Un-Commented out to revert to previous working state.
            // DQ (8/25/2014): Commented out to test new logic at base of isTemplateDeclaration(a_routine_ptr).
            // DQ (8/25/2014): Un-Commented out to revert to previous working state.
            // DQ (8/25/2014): Allow non-template functions in a template class declaration (see test2014_161.C).
#if !ENFORCE_NO_FUNCTION_TEMPLATE_DECLARATIONS_IN_TEMPLATE_CLASS_INSTANTIATIONS
            // printf ("In checkThatNoTemplateInstantiationIsDeclaredInTemplateDefinitionScope(): This is the wrong scope that is associated with this function (because EDG uses a single pointeer for a scope that maps to two different scopes in ROSE (and the scope_cache is not reset) \n");
            // ROSE_ASSERT(isSgTemplateClassDefinition(scope) == NULL);
#endif
             }
        }
       else
        {
          if (isSgTemplateFunctionDeclaration(func) != NULL)
             {
               if (isSgTemplateInstantiationMemberFunctionDecl(func) != NULL)
                  {
                    ROSE_ASSERT(isSgTemplateClassDefinition(scope) != NULL);
                  }
             }
        }
   }

//----------------- function declaration------------
// considering
// 1. fresh building
// 2. secondary building after another nondefining functiondeclaration
// 3. secondary building after another defining function declaration
// 4. fortran ?
template <class actualFunction>
actualFunction*
SageBuilder::buildNondefiningFunctionDeclaration_T (const SgName & XXX_name, SgType* return_type, SgFunctionParameterList * paralist, bool isMemberFunction,
   SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, SgTemplateArgumentPtrList* templateArgumentsList, SgTemplateParameterPtrList* templateParameterList)
   {
  // DQ (11/25/2011): This function has been modified to work when used with a SgTemplateFunctionDeclaration as a template argument.
  // It was originally designed to work with only SgFunctionDeclaration and SgMemberFunctionDeclaration, it now works with these
  // plus SgTemplateFunctionDeclaration and SgTemplateMemberonDeclaration IR nodes. This is part of providing new support for template
  // declarations in the AST and a general update of this function to support this expanded use.

  // DQ (11/27/2011) Note: it is not clear if we need the newly added input paramter: buildTemplateInstantiation; since this is represented in the template parameter.

#if 0
  // FMZ (3/23/2009): We need this for the  coarray translator
     if (SageInterface::is_Fortran_language() == true)
        {
       // We don't expect this is being called for Fortran
          cerr<<"Building nondefining function in Fortran is not allowed!"<<endl;
          ROSE_ASSERT(false);
        }
#endif

#if 0
     printf ("In buildNondefiningFunctionDeclaration_T(): XXX_name = %s\n", XXX_name.str());
#endif

  // argument verification
     if (scope == NULL)
        {
          scope = SageBuilder::topScopeStack();
#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): isMemberFunction = %s scope == NULL resetting the scope = %p = %s \n",isMemberFunction ? "true" : "false",scope,scope->class_name().c_str());
#endif
        }
     ROSE_ASSERT (scope !=NULL);

#if 0
     printf ("In buildNondefiningFunctionDeclaration_T(): scope = %p = %s templateArgumentsList = %p templateParameterList = %p \n",
          scope,scope != NULL ? scope->class_name().c_str() : "NULL",templateArgumentsList,templateParameterList);
#endif

  // ROSE_ASSERT(XXX_name.is_null() == false);
     if (XXX_name.is_null() == true)
        {
       // DQ (4/2/2013): This case is generated for test2013_86.C.
          mprintf ("NOTE: In buildNondefiningFunctionDeclaration_T(): XXX_name.is_null() == true: This is a function with an empty name (allowed as compiler generated initializing constructors to un-named classes, structs, and unions in C++ \n");
        }

     SgName nameWithoutTemplateArguments = XXX_name;
     SgName nameWithTemplateArguments    = nameWithoutTemplateArguments;

     bool buildTemplateInstantiation = ((VariantT)actualFunction::static_variant == V_SgTemplateInstantiationFunctionDecl || (VariantT)actualFunction::static_variant == V_SgTemplateInstantiationMemberFunctionDecl);

  // DQ (8/7/2013): Added support for template declarations.
     bool buildTemplateDeclaration   = ((VariantT)actualFunction::static_variant == V_SgTemplateFunctionDeclaration || (VariantT)actualFunction::static_variant == V_SgTemplateMemberFunctionDeclaration);

#if 0
     printf ("In buildNondefiningFunctionDeclaration_T(): buildTemplateInstantiation = %s \n",buildTemplateInstantiation ? "true" : "false");
#endif

  // DQ (8/7/2013): Added support for template declarations (need to handle template names overloaded on template parameters).
  // We want to use the template arguments in the symbol table lookup, but not in the name generation.
  // if (buildTemplateInstantiation == true)
  // if (buildTemplateInstantiation == true || buildTemplateDeclaration == true)
     if (buildTemplateInstantiation == true)
        {
          ROSE_ASSERT(templateArgumentsList != NULL);
          nameWithTemplateArguments = appendTemplateArgumentsToName(nameWithoutTemplateArguments,*templateArgumentsList);
#if 0
          printf ("Building a non-defining function: buildNondefiningFunctionDeclaration_T() nameWithTemplateArguments = %s buildTemplateInstantiation = %s \n",nameWithTemplateArguments.str(),buildTemplateInstantiation ? "true:" : "false");
#endif
#if 0
          if (nameWithTemplateArguments == "insert < __normal_iterator< SgInitializedName ** , __type  >  > ")
             {
               printf ("In buildNondefiningFunctionDeclaration_T(): Found function nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
             }
#endif
        }

#if 0
     printf ("In buildNondefiningFunctionDeclaration_T(): nameWithTemplateArguments = |%s| \n",nameWithTemplateArguments.str());
#endif

  // printf ("Building non-defining function for scope = %p in file = %s \n",scope,TransformationSupport::getSourceFile(scope)->getFileName().c_str());

  // DQ (2/25/2009): I think I added this recently but it is overly restrictive.
  // ROSE_ASSERT(scope != NULL);

  // ROSE_ASSERT(scope->containsOnlyDeclarations());
  // this function is also called when building a function reference before the function declaration exists.  So, skip the check
  // ROSE_ASSERT(nameWithTemplateArguments.is_null() == false);
     if (nameWithTemplateArguments.is_null() == true)
        {
       // DQ (3/25/2017): Modified to use message logging.
       // DQ (4/2/2013): This case is generated for test2013_86.C.
          mprintf ("NOTE: In buildNondefiningFunctionDeclaration_T(): nameWithTemplateArguments.is_null() == true: This is a function with an empty name (allowed as compiler generated initializing constructors to un-named classes, structs, and unions in C++ \n");
        }

  // ROSE_ASSERT(nameWithoutTemplateArguments.is_null() == false);
     if (nameWithoutTemplateArguments.is_null() == true)
        {
       // DQ (3/25/2017): Modified to use message logging.
       // DQ (4/2/2013): This case is generated for test2013_86.C.
          mprintf ("NOTE: In buildNondefiningFunctionDeclaration_T(): nameWithoutTemplateArguments.is_null() == true: This is a function with an empty name (allowed as compiler generated initializing constructors to un-named classes, structs, and unions in C++ \n");
        }

     ROSE_ASSERT(return_type != NULL);
     ROSE_ASSERT(paralist != NULL);

  // DQ (7/27/2012): Note that the input name should not have template argument syntax.
  // I think this could still fail for a function with a name such as "X<Y>"  strange converstion operators.
  // ROSE_ASSERT(SageInterface::hasTemplateSyntax(XXX_name) == false);
     if (SageInterface::hasTemplateSyntax(nameWithoutTemplateArguments) == true)
        {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
          printf ("Warning: In buildNondefiningFunctionDeclaration_T(): nameWithoutTemplateArguments = %s nameWithTemplateArguments = %s \n",nameWithoutTemplateArguments.str(),nameWithTemplateArguments.str());
#endif
        }
  // DQ (7/27/2012): There are reasons why this can fail: e.g. for functions with names such as:
  // "operator std::auto_ptr_ref<_Tp1>" which is a user defined conversion operator to one class from another.
  // ROSE_ASSERT(SageInterface::hasTemplateSyntax(nameWithoutTemplateArguments) == false);

  // tentatively build a function type, since it is shared
  // by all prototypes and defining declarations of a same function!
  // SgFunctionType * func_type = buildFunctionType(return_type,paralist);

#if 0
     printf ("In buildNondefiningFunctionDeclaration_T(): isMemberFunction = %s scope = %p = %s \n",isMemberFunction ? "true" : "false",scope,scope->class_name().c_str());
#endif

     SgFunctionType* func_type = NULL;
     if (isMemberFunction == true)
        {
       // func_type = buildMemberFunctionType(return_type,paralist,NULL,0);
       // func_type = buildFunctionType(return_type,paralist);
#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): scope = %p = %s\n",scope,scope->class_name().c_str());
#endif
          SgFunctionParameterTypeList * typeList = buildFunctionParameterTypeList(paralist);

       // DQ (1/10/2020): This was a default argument that was initialized to zero, I would like to remove the 
       // use of the default argument to better support debugging new regerence qualifiers for member functions.
       // func_type = buildMemberFunctionType(return_type,typeList,scope, functionConstVolatileFlags);
          unsigned int reference_modifiers = 0;
          func_type = buildMemberFunctionType(return_type,typeList,scope, functionConstVolatileFlags,reference_modifiers);
#if 0
          printf ("Using zero as value for reference_modifiers for member function type = %p = %s \n",func_type,func_type->class_name().c_str());
#endif
#if 0
       // DQ (1/11/2020): Debugging Cxx11_tests/test2020_27.C.
          SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(func_type);
          if (memberFunctionType != NULL)
             {
               printf (" --- memberFunctionType->isLvalueReferenceFunc() = %s \n",memberFunctionType->isLvalueReferenceFunc() ? "true" : "false");
               printf (" --- memberFunctionType->isRvalueReferenceFunc() = %s \n",memberFunctionType->isRvalueReferenceFunc() ? "true" : "false");
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
#endif

       // printf ("Error: SgFunctionType built instead of SgMemberFunctionType \n");
       // ROSE_ASSERT(false);
        }
       else
        {
          func_type = buildFunctionType(return_type,paralist);
        }

     ROSE_ASSERT(func_type != NULL);

#if 0
     printf ("In buildNondefiningFunctionDeclaration_T(): func_type = %p = %s \n",func_type,func_type->class_name().c_str());
#endif
#if 0
  // printf ("In buildNondefiningFunctionDeclaration_T(): paralist->get_args()[0]->unparseToString() = %s \n",paralist->get_args()[0]->unparseToString().c_str());
     printf ("In buildNondefiningFunctionDeclaration_T(): func_type->unparseToString() = %s \n",func_type->unparseToString().c_str());
#endif

  // function declaration
     actualFunction* func = NULL;

  // search before using the function type to create the function declaration
  // TODO only search current scope or all ancestor scope?? (DQ: Only current scope!)
  // We don't have lookup_member_function_symbol  yet
  // SgFunctionSymbol *func_symbol = scope->lookup_function_symbol(name,func_type);

  // DQ (12/27/2011): Under the new design we can make the symbol type SgFunctionSymbol instead of the less specific SgSymbol.
  // DQ (11/25/2011): We want to add the support for template function declarations,
  // so this should be a SgSymbol so that we can have it be either a SgFunctionSymbol
  // or a SgTemplateSymbol.
  // SgFunctionSymbol *func_symbol = NULL;
  // SgSymbol *func_symbol = NULL;
     SgFunctionSymbol* func_symbol = NULL;

     if (scope != NULL)
        {
       // DQ (3/13/2012): Experiment with new function to support only associating the right type of symbol with the
       // function being built.  I don't think I like the design of this interface, but we might change that later.
       // func_symbol = scope->lookup_function_symbol(name,func_type);
       // SgSymbol* symbol = scope->get_symbol_table()->find_symbol_by_type_of_function<actualFunction>(name,func_type);
       // func_symbol = symbol;
#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): name = %s func_type = %p = %s templateParameterList->size() = %" PRIuPTR " templateArgumentsList->size() = %" PRIuPTR " \n",
               nameWithTemplateArguments.str(),func_type,func_type->get_mangled().str(),
               templateParameterList != NULL ? templateParameterList->size() : 999,
               templateArgumentsList != NULL ? templateArgumentsList->size() : 999);
          printf(" --- scope = %p (%s)\n", scope, scope ? scope->class_name().c_str() : "");
#endif
#if 0
       // Debugging output.
          if (templateArgumentsList != NULL)
             {
               printf ("Output of type chains for template arguments: \n");
               for (size_t i = 0; i < templateArgumentsList->size(); i++)
                  {
                    string s = "template argument: " + StringUtility::numberToString(i) + " ";
                 // templateArgumentsList->get_args()[i]->display(s);
                 // templateArgumentsList->[i]->display(s);
                    templateArgumentsList->operator[](i)->display(s);
                  }
             }
#endif

       // DQ (8/7/2013): We need to use the template arguments in the symbol table lookup for template functions to permit template function overloading on template perameters.
       // func_symbol = scope->get_symbol_table()->find_symbol_by_type_of_function<actualFunction>(name,func_type);
       // func_symbol = scope->get_symbol_table()->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type);
       // func_symbol = scope->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type);
       // func_symbol = scope->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type,templateArgumentsList);
       // func_symbol = scope->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type,NULL,templateArgumentsList);
          func_symbol = scope->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type,templateParameterList,templateArgumentsList);

#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): func_symbol from scope->find_symbol_by_type_of_function<actualFunction>(name = %s) = %p \n",nameWithTemplateArguments.str(),func_symbol);
          if (func_symbol != NULL) {
            printf ("In buildNondefiningFunctionDeclaration_T(): func_symbol->get_declaration() = %p \n", func_symbol->get_declaration());
          }
#endif
#if 0
          if (nameWithoutTemplateArguments == "getline")
             {
               printf ("In buildNondefiningFunctionDeclaration_T(): func_symbol from scope->find_symbol_by_type_of_function<actualFunction>(name = %s) = %p \n",nameWithTemplateArguments.str(),func_symbol);
             }
#endif

       // If not a proper function (or instantiated template function), then check for a template function declaration.
          if (func_symbol == NULL)
             {
            // Note that a template function does not have a function type (just a name).
#if 0
               printf ("NOTE: Maybe template symbols for template function declarations should use the function type \n");
#endif
               ROSE_ASSERT(func_type != NULL);
             }
            else
             {
#if 0
               printf ("In buildNondefiningFunctionDeclaration_T(): func_symbol = %p was found using name = %s \n",func_symbol,nameWithTemplateArguments.str());
#endif
             }
        }

#if 1
  // DQ (3/13/2012): I want to introduce error checking on the symbol matching the template parameter.
     if (func_symbol != NULL)
        {
#if 0
          printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): scope = %p func_symbol = %p = %s \n",scope,func_symbol,func_symbol->class_name().c_str());
#endif
#if 0
          if ((VariantT)actualFunction::static_variant == V_SgTemplateMemberFunctionDeclaration)
               printf ("This is a SgTemplateMemberFunctionDeclaration function \n");
            else
               printf ("variantT(actualFunction::static_variant) = %d \n",actualFunction::static_variant);
#endif
          switch((VariantT)actualFunction::static_variant)
             {
               case V_SgFunctionDeclaration:
               case V_SgProcedureHeaderStatement:
               case V_SgTemplateInstantiationFunctionDecl:
                  {
#if 0
                    printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): This is a SgFunctionDeclaration function \n");
#endif
                    ROSE_ASSERT(isSgFunctionSymbol(func_symbol) != NULL);
                    break;
                  }
               case V_SgMemberFunctionDeclaration:
               case V_SgTemplateInstantiationMemberFunctionDecl:
                  {
#if 0
                    printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): This is a SgMemberFunctionDeclaration function \n");
#endif
                    ROSE_ASSERT(isSgMemberFunctionSymbol(func_symbol) != NULL);
                    break;
                  }
               case V_SgTemplateFunctionDeclaration:
                  {
#if 0
                    printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): This is a SgTemplateFunctionDeclaration function \n");
#endif
                    ROSE_ASSERT(isSgTemplateFunctionSymbol(func_symbol) != NULL);
                    break;
                  }
               case V_SgTemplateMemberFunctionDeclaration:
                  {
#if 0
                    printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): This is a SgTemplateMemberFunctionDeclaration function \n");
#endif
                    ROSE_ASSERT(isSgTemplateMemberFunctionSymbol(func_symbol) != NULL);
                    break;
                  }

               default:
                  {
                    printf ("default reach in buildNondefiningFunctionDeclaration_T(): variantT(actualFunction::static_variant) = %d \n",actualFunction::static_variant);
                    ROSE_ASSERT(false);
                  }
             }

          // TV (2/5/14): Found symbol might come from another file, in this case we need to insert it in the current scope.
          //              Can only happen when scope is a global scope
             ROSE_ASSERT(scope != NULL);
             if (  isSgGlobal(scope) != NULL
                && scope != func_symbol->get_scope()
                && !SageInterface::isAncestor(scope, func_symbol->get_scope())
             && !scope->symbol_exists(nameWithTemplateArguments, func_symbol) )
             {
#if 0
               printf ("In buildNondefiningFunctionDeclaration_T(): Calling scope->insert_symbol(): for global scope: using nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
#endif
               scope->insert_symbol(nameWithTemplateArguments, func_symbol);
             }
        }
#endif

     if (func_symbol == NULL)
        {
       // first prototype declaration
       // func = new actualFunction (name,func_type,NULL);
#if 0
       // DQ (10/11/2014): Added argument to resolve ambiguity caused by Aterm support.
          func = new actualFunction (nameWithTemplateArguments,NULL,func_type,NULL);
#else
          func = new actualFunction (nameWithTemplateArguments,func_type,NULL);
#endif
          ROSE_ASSERT(func != NULL);

#if 0
       // DQ (2/10/2016): Adding support for C99 function parameters used as variable references in the function parameter list.
          ROSE_ASSERT(func->get_functionParameterScope() == NULL);
          SgFunctionParameterScope* functionParameterScope = new SgFunctionParameterScope();
          ROSE_ASSERT(functionParameterScope != NULL);
#if 0
          printf ("NOTE: In buildNondefiningFunctionDeclaration_T(): building new functionParameterScope for nondefining function declaration: name = %s functionParameterScope = %p = %s \n",
               nameWithTemplateArguments.str(),functionParameterScope,functionParameterScope->class_name().c_str());
#endif
          func->set_functionParameterScope(functionParameterScope);
          ROSE_ASSERT(func->get_functionParameterScope() != NULL);
#endif

       // DQ (5/1/2012): This should always be true.
          ROSE_ASSERT(func->get_file_info() == NULL);

#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): case of func_symbol == NULL: constructor called to build func = %p = %s \n",func,func->class_name().c_str());
#endif
#if 0
          if (isSgMemberFunctionDeclaration(func) != NULL)
             {
               printf ("In buildNondefiningFunctionDeclaration_T(): SgCtorInitializerList = %p \n",isSgMemberFunctionDeclaration(func)->get_CtorInitializerList());
             }
#endif
#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): scope = %p = %s \n",scope,scope->class_name().c_str());
#endif
       // DQ (12/14/2011): Moved this from lower in this function.
          func->set_scope(scope);

       // DQ (12/15/2011): Added test.
          checkThatNoTemplateInstantiationIsDeclaredInTemplateDefinitionScope(func,scope);

       // This fails below for a SgTemplateFunctionDeclaration, so test it here.
          ROSE_ASSERT(func->get_parameterList() != NULL);

       // NOTE: we want to allow the input scope to be NULL (and even the SageBuilder::topScopeStack() == NULL)
       // so that function can be built bottom up style.  However this means that the symbol tables in the
       // scope of the returned function declaration will have to be setup separately.
          if (scope != NULL)
             {
            // function symbol table
               if (isSgMemberFunctionDeclaration(func))
                  {
                 // DQ (11/23/2011): This change allows this to compile for where SgTemplateFunctionDeclarations are used. I have
                 // not decided if template declarations should cause symbols to be generated for functions and member functions.
                 // func_symbol = new SgMemberFunctionSymbol(func);
                    if (isSgTemplateMemberFunctionDeclaration(func) != NULL)
                       func_symbol = new SgTemplateMemberFunctionSymbol(isSgTemplateMemberFunctionDeclaration(func));
                    else
                     {
                       SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(func);
                       ROSE_ASSERT(memberFunctionDeclaration != NULL);
                       func_symbol = new SgMemberFunctionSymbol(memberFunctionDeclaration);
                     }

                    ROSE_ASSERT(func_symbol != NULL);
                    ROSE_ASSERT(func_symbol->get_symbol_basis() != NULL);
                  }
                 else
                  {
                 // if (isSgFunctionDeclaration(func))
                    if (isSgTemplateFunctionDeclaration(func))
                       {
                      // How should we handled template functions in the symbol table???
                      // DQ (11/24/2011): After some thought, I think that template declarations for function are more template declarations
                      // than functions.  So all template function declarations will be handled as SgTemplateSymbols and not SgFunctionSymbols.mplate function declarations.
                         SgTemplateFunctionDeclaration* templatedeclaration = isSgTemplateFunctionDeclaration(func);
                         ROSE_ASSERT(templatedeclaration != NULL);
                         SgTemplateFunctionSymbol* template_symbol = new SgTemplateFunctionSymbol(templatedeclaration);
                         ROSE_ASSERT(template_symbol != NULL);
                         ROSE_ASSERT(template_symbol->get_symbol_basis() != NULL);
                         func_symbol = template_symbol;
                       }
                      else
                       {
                         func_symbol = new SgFunctionSymbol(isSgFunctionDeclaration(func));
                         ROSE_ASSERT(func_symbol->get_symbol_basis() != NULL);
                       }
                  }

               ROSE_ASSERT(func_symbol != NULL);
               ROSE_ASSERT(func_symbol->get_symbol_basis() != NULL);
#if 0
               printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): scope = %p = %s nameWithTemplateArguments = %s func_symbol = %p = %s = %s \n",
                    scope,scope->class_name().c_str(),nameWithTemplateArguments.str(),func_symbol,func_symbol->class_name().c_str(),SageInterface::get_name(func_symbol).c_str());
#endif
            // scope->insert_symbol(name, func_symbol);
               scope->insert_symbol(nameWithTemplateArguments, func_symbol);

            // DQ (3/8/2012): Added assertion.
               ROSE_ASSERT(func->get_symbol_from_symbol_table() != NULL);

            // ROSE_ASSERT(scope->lookup_template_member_function_symbol(name,result->get_type()) != NULL);

               if (isSgFunctionDeclaration(func) == NULL)
                  {
                 // DQ (12/18/2011): If this is a SgTemplateDeclaration, then we shuld be able to find the name in the associated scope.
#if 0
                    printf ("In buildNondefiningFunctionDeclaration_T(): Looking up nameWithTemplateArguments = %s in scope = %p = %s \n",nameWithTemplateArguments.str(),scope,scope->class_name().c_str());
#endif
                 // DQ (8/12/2013): Added template parameter list.
                 // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
                 // In this case these are unavailable from this point.
                 // ROSE_ASSERT(scope->lookup_template_symbol(name) != NULL);
                 // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments) != NULL);
                 // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments,NULL,NULL) != NULL);
                 // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments,templateParameterList,NULL) != NULL);
                    ROSE_ASSERT(scope->lookup_template_function_symbol(nameWithTemplateArguments,func_type,templateParameterList) != NULL);
                  }

            // ROSE_ASSERT(scope->lookup_function_symbol(name,func_type) != NULL);

            // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
            // In this case these are unavailable from this point.
            // DQ (11/25/2011): Added support for template functions.
            // DQ (2/26/2009): uncommented assertion.
            // ROSE_ASSERT(scope->lookup_function_symbol(name) != NULL); // Did not pass for member function? Should we have used the mangled name?
            // ROSE_ASSERT(scope->lookup_function_symbol(name) != NULL || scope->lookup_template_symbol(name) != NULL); // Did not pass for member function? Should we have used the mangled name?
            // ROSE_ASSERT(scope->lookup_function_symbol(nameWithTemplateArguments) != NULL || scope->lookup_template_symbol(nameWithTemplateArguments) != NULL); // Did not pass for member function? Should we have used the mangled name?
            // Did not pass for member function? Should we have used the mangled name?
            // ROSE_ASSERT(scope->lookup_function_symbol(nameWithTemplateArguments) != NULL || scope->lookup_template_symbol(nameWithTemplateArguments,NULL,NULL) != NULL);
            // ROSE_ASSERT(scope->lookup_function_symbol(nameWithTemplateArguments) != NULL || scope->lookup_template_symbol(nameWithTemplateArguments,templateParameterList,NULL) != NULL);
#if 0
               printf ("\n ############################### \n");
               printf ("In buildNondefiningFunctionDeclaration_T(): lookup_symbol: templateParameterList = %p \n",templateParameterList);
               printf ("In buildNondefiningFunctionDeclaration_T(): func = %p = %s \n",func,func->class_name().c_str());
#endif
               ROSE_ASSERT(buildTemplateDeclaration == false || templateParameterList != NULL);
#if 0
               SgSymbol* s0 = scope->lookup_template_member_function_symbol(nameWithTemplateArguments,func_type,templateParameterList);

               printf ("In buildNondefiningFunctionDeclaration_T(): lookup_template_member_function_symbol: s0 = %p \n",s0);

               SgSymbol* s1 = scope->lookup_template_function_symbol(nameWithTemplateArguments,func_type,templateParameterList);

               printf ("In buildNondefiningFunctionDeclaration_T(): lookup_template_function_symbol: s1 = %p \n",s1);

               SgSymbol* s2 = scope->lookup_function_symbol(nameWithTemplateArguments);

               printf ("In buildNondefiningFunctionDeclaration_T(): lookup_template_function_symbol: s1 = %p lookup_function_symbol: s2 = %p \n",s1,s2);
#endif
            // DQ (8/13/2013): We need to test for function symbols (which will include member function symbols),
            // template functions and template member functions. Each must be tested for seperately because template
            // functions and template member functions are not connected to derivation which non-template functions
            // and non-template member functions are connected through derivation.
            // ROSE_ASSERT(scope->lookup_function_symbol(nameWithTemplateArguments) != NULL || scope->lookup_template_function_symbol(nameWithTemplateArguments,func_type,templateParameterList) != NULL);
               ROSE_ASSERT(scope->lookup_function_symbol(nameWithTemplateArguments) != NULL ||
                           scope->lookup_template_function_symbol(nameWithTemplateArguments,func_type,templateParameterList) != NULL ||
                           scope->lookup_template_member_function_symbol(nameWithTemplateArguments,func_type,templateParameterList) != NULL);

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_SYMBOL_TABLE_LOOKUP
            // if (scope->lookup_function_symbol(name) == NULL || scope->lookup_template_symbol(name) != NULL)
            // if (scope->lookup_function_symbol(nameWithTemplateArguments) == NULL || scope->lookup_template_symbol(nameWithTemplateArguments) != NULL)
            // if (scope->lookup_function_symbol(nameWithTemplateArguments) == NULL || scope->lookup_template_symbol(nameWithTemplateArguments,NULL,NULL) != NULL)
               if (scope->lookup_function_symbol(nameWithTemplateArguments,templateArgumentList) == NULL || scope->lookup_template_symbol(nameWithTemplateArguments,templateParameterList,NULL) != NULL)
                  {
                 // Make sure this is a template function declaration...
                    printf ("Need to make sure this is a template function declaration... \n");
                  }
#endif
             }

       // DQ (12/14/2011): Added test.
          ROSE_ASSERT(func->get_scope() != NULL);

          if (isSgFunctionDeclaration(func) == NULL)
             {
            // If this is a SgTemplateDeclaration, then we shuld be able to find the name in the associated scope.
#if 0
               printf ("In buildNondefiningFunctionDeclaration_T(): Looking up nameWithTemplateArguments = %s in scope = %p = %s \n",nameWithTemplateArguments.str(),scope,scope->class_name().c_str());
#endif
            // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
            // In this case these are unavailable from this point.
            // ROSE_ASSERT(scope->lookup_template_symbol(name) != NULL);
            // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments) != NULL);
            // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments,NULL,NULL) != NULL);
            // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments,templateParameterList,NULL) != NULL);
               ROSE_ASSERT(scope->lookup_template_function_symbol(nameWithTemplateArguments,func_type,templateParameterList) != NULL);
             }
#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): Setting the func = %p set_firstNondefiningDeclaration(func = %p) (to itself) \n",func,func);
          printf ("In buildNondefiningFunctionDeclaration_T(): Setting the func = %p set_definingDeclaration(func == NULL)      (to NULL) \n",func);
#endif
          func->set_firstNondefiningDeclaration(func);
          func->set_definingDeclaration(NULL);

       // DQ (5/8/2016): We need to test the first defining declaration that we used.
          ROSE_ASSERT(func->get_firstNondefiningDeclaration() == func);

          ROSE_ASSERT(func->get_definingDeclaration() == NULL);

#if 0
       // DQ (5/8/2016): We need to test the first defining declaration that we used.
          SgTemplateFunctionDeclaration* first_nondefiningTemplateFunctionDeclaration = isSgTemplateFunctionDeclaration(func->get_firstNondefiningDeclaration());
          ROSE_ASSERT(first_nondefiningTemplateFunctionDeclaration != NULL);

       // DQ (9/24/2015): Adding test of template parameter lists.
          if (templateParameterList->size() != first_nondefiningTemplateFunctionDeclaration->get_templateParameters().size())
             {
               printf ("Error: size mismatch: case first_nondefining_declaration == NULL: templateParameterList->size() = %zu \n",templateParameterList->size());
               printf ("Error: size mismatch: case first_nondefining_declaration == NULL: first_nondefiningTemplateFunctionDeclaration->get_templateParameters().size() = %zu \n",
                    first_nondefiningTemplateFunctionDeclaration->get_templateParameters().size());
             }
       // This may be OK at this point in the construction.
       // ROSE_ASSERT(templateParameterList->size() == first_nondefiningTemplateFunctionDeclaration->get_templateParameters().size());
#endif

       // DQ (12/14/2011): Error checking
          SgTemplateInstantiationMemberFunctionDecl* testMemberDecl = isSgTemplateInstantiationMemberFunctionDecl(func);
          if (testMemberDecl != NULL)
             {
               ROSE_ASSERT(testMemberDecl->get_scope() != NULL);
#if 0
               printf ("testMemberDecl->get_scope() = %s \n",testMemberDecl->get_scope()->class_name().c_str());
#endif
               ROSE_ASSERT(testMemberDecl->get_class_scope() != NULL);
#if 0
               printf ("testMemberDecl->get_class_scope() = %s \n",testMemberDecl->get_class_scope()->class_name().c_str());
#endif
               ROSE_ASSERT(testMemberDecl->get_associatedClassDeclaration() != NULL);
             }
#if 0
            else
             {
               SgTemplateInstantiationFunctionDecl* testNonMemberDecl = isSgTemplateInstantiationFunctionDecl(func);
               if (testNonMemberDecl != NULL)
                  {
                    ROSE_ASSERT(testNonMemberDecl->get_associatedClassDeclaration() != NULL);
                  }
             }
#endif
        }
       else
        {
          ROSE_ASSERT(func_symbol != NULL);

          ROSE_ASSERT(scope != NULL);

       // 2nd, or 3rd... prototype declaration
       // reuse function type, function symbol of previous declaration

       // std::cout<<"debug:SageBuilder.C: 267: "<<"found func_symbol!"<<std::endl;
       // delete (func_type-> get_argument_list ());
       // delete func_type; // bug 189
#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): func_symbol = %p = %s \n",func_symbol,func_symbol->class_name().c_str());
#endif
       // func_type = func_symbol->get_declaration()->get_type();
          SgNode* associatedSymbolBasis = func_symbol->get_symbol_basis();
          ROSE_ASSERT(associatedSymbolBasis != NULL);

          SgDeclarationStatement* associatedDeclaration = isSgDeclarationStatement(associatedSymbolBasis);
          ROSE_ASSERT(associatedDeclaration != NULL);
          SgFunctionDeclaration*         functionDeclaration         = isSgFunctionDeclaration(associatedDeclaration);
          SgTemplateFunctionDeclaration* templateFunctionDeclaration = isSgTemplateFunctionDeclaration(associatedDeclaration);
#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): associatedDeclaration = %p functionDeclaration = %p templateFunctionDeclaration = %p \n",
               associatedDeclaration,functionDeclaration,templateFunctionDeclaration);
#endif
          if (functionDeclaration != NULL)
             {
               func_type = functionDeclaration->get_type();
#if 0
            // DQ (5/8/2016): Error checking!
               if (templateFunctionDeclaration != NULL)
                  {
                    printf ("In buildNondefiningFunctionDeclaration_T(): templateFunctionDeclaration->get_templateParameters().size() = %zu \n",templateFunctionDeclaration->get_templateParameters().size());
                  }
#endif
             }
            else
             {
               if (templateFunctionDeclaration != NULL)
                  {
                 // DQ (5/8/2016): I think this code is never executed (because a templateFunctionDeclaration
                 // is derived from a SgFunctionDeclaration, in the newer design (a few years ago)).

                    printf ("This code should not be reachable! \n");
                    ROSE_ASSERT(false);

                    func_type = templateFunctionDeclaration->get_type();
#if 0
                    printf ("In buildNondefiningFunctionDeclaration_T(): templateFunctionDeclaration->get_templateParameters().size() = %zu \n",templateFunctionDeclaration->get_templateParameters().size());
#endif
                  }
                 else
                  {
                    printf ("Error: associatedDeclaration = %p = %s \n",associatedDeclaration,associatedDeclaration->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }
          ROSE_ASSERT(func_type != NULL);

       // func = new actualFunction(name,func_type,NULL);
          func = new actualFunction(nameWithTemplateArguments,func_type,NULL);
          ROSE_ASSERT(func != NULL);

#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): func->get_name() = %s func = %p = %s \n",func->get_name().str(),func,func->class_name().c_str());
#endif

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
       // DQ (5/1/2012): Make sure that we don't have IR nodes marked as translformations.
       // This is too early a point to test since the source position has not been set for func yet.
       // detectTransformations_local(func);
#endif
#if 0
          printf ("In buildNondefiningFunctionDeclaration_T() (part 2): constructor called to build func = %p = %s \n",func,func->class_name().c_str());
          if (isSgMemberFunctionDeclaration(func) != NULL)
             {
               printf ("In buildNondefiningFunctionDeclaration_T() (part 2): SgCtorInitializerList = %p \n",isSgMemberFunctionDeclaration(func)->get_CtorInitializerList());
             }
#endif

       // DQ (12/14/2011): Moved this up from below.
          func->set_scope(scope);

       // DQ (3/8/2012): Added assertion.
          ROSE_ASSERT(func->get_symbol_from_symbol_table() == NULL);

#if 1
       // DQ (12/15/2011): Added test.
          checkThatNoTemplateInstantiationIsDeclaredInTemplateDefinitionScope(func,scope);
#else
       // DQ (12/14/2011): We need the parent to be set so that we can call some of the test functions
       // (e.g assert that get_class_scope() for member functions).  So we set the parent to the scope
       // by default and see if this will work, else we could disable to assertion that the parent is
       // non-null in the get_class_scope() member function.
          if (isSgMemberFunctionDeclaration(func) != NULL)
             {
               printf ("WARNING: setting parent of function to match scope by default \n");
               func->set_parent(scope);

#error "DEAD CODE!"

               ROSE_ASSERT(scope != NULL);
               ROSE_ASSERT(isSgTemplateClassDefinition(scope) == NULL);
             }
            else
             {
               if (isSgTemplateMemberFunctionDeclaration(func) != NULL)
                  {
                    ROSE_ASSERT(isSgTemplateClassDefinition(scope) != NULL);
                  }
             }
#endif
       // we don't care if it is member function or function here for a pointer
       // SgFunctionDeclaration* prevDecl = NULL;
          SgDeclarationStatement* prevDecl = NULL;

       // This does not handle the case of a template function declaration.
       // prevDecl = func_symbol->get_declaration();
          if (functionDeclaration != NULL)
             {
#if 0
               printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): Set prevDecl = functionDeclaration \n");
#endif
               prevDecl = functionDeclaration;
             }
            else
             {
               ROSE_ASSERT(templateFunctionDeclaration != NULL);
#if 0
               printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): Set prevDecl = templateFunctionDeclaration \n");
#endif
               prevDecl = templateFunctionDeclaration;
             }

          ROSE_ASSERT(prevDecl != NULL);
#if 0
          printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): prevDecl = %p = %s \n",prevDecl,prevDecl->class_name().c_str());
#endif
#if 0
          if (prevDecl == prevDecl->get_definingDeclaration())
             {
            // The symbol points to a defining declaration and now that we have added a non-defining
            // declaration we should have the symbol point to the new non-defining declaration.
            // printf ("Switching declaration in functionSymbol to point to the non-defining declaration \n");

#error "DEAD CODE!"

            // DQ (11/23/2011): This change allows this to compile for where SgTemplateFunctionDeclarations are used.
            // func_symbol->set_declaration(func);
               func_symbol->set_declaration(isSgFunctionDeclaration(func));

            // DQ (11/23/2011): Added this test to support debugging the case where SgTemplateFunctionDeclarations is used.
               ROSE_ASSERT(func_symbol->get_declaration() != NULL);
             }
#else
          SgFunctionSymbol *function_symbol = isSgFunctionSymbol(func_symbol);
          if (prevDecl == prevDecl->get_definingDeclaration())
             {
            // The symbol points to a defining declaration and now that we have added a non-defining
            // declaration we should have the symbol point to the new non-defining declaration.
               printf ("WARNING: Switching declaration in functionSymbol to point to the non-defining declaration \n");
               function_symbol->set_declaration(isSgFunctionDeclaration(func));
               ROSE_ASSERT(function_symbol->get_declaration() != NULL);
             }
#endif

       // If this is the first non-defining declaration then set the associated data member.
          SgDeclarationStatement* nondefiningDeclaration = prevDecl->get_firstNondefiningDeclaration();
          if (nondefiningDeclaration == NULL)
             {
               nondefiningDeclaration = func;
             }

          ROSE_ASSERT(nondefiningDeclaration != NULL);
#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): Setting the func = %p set_firstNondefiningDeclaration(nondefiningDeclaration = %p)      (to nondefiningDeclaration) \n",func,nondefiningDeclaration);
          printf ("In buildNondefiningFunctionDeclaration_T(): Setting the func = %p set_definingDeclaration(prevDecl->get_definingDeclaration() = %p) (to prevDecl->get_definingDeclaration()) \n",func,prevDecl->get_definingDeclaration());
#endif
       // DQ (8/27/2014): Added assertions.
          ROSE_ASSERT(func != NULL);
          ROSE_ASSERT(prevDecl != NULL);

       // func->set_firstNondefiningDeclaration(prevDecl->get_firstNondefiningDeclaration());
          func->set_firstNondefiningDeclaration(nondefiningDeclaration);
          func->set_definingDeclaration(prevDecl->get_definingDeclaration());

#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): Setting new function (func = %p) to have firstNondefiningDeclaration = %p definingDeclaration = %p \n",func,func->get_firstNondefiningDeclaration(),func->get_definingDeclaration());
#endif
       // DQ (3/8/2012): Added assertion.
          ROSE_ASSERT(nondefiningDeclaration->get_symbol_from_symbol_table() != NULL);
          assert(func->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table() != NULL);

       // DQ (3/8/2012): If this is the redundant function prototype then we have to look
       // at the first defining declaration since only it will have an associated symbol.
       // ROSE_ASSERT(func->get_symbol_from_symbol_table() != NULL);
          if (func->get_symbol_from_symbol_table() == NULL)
             {
               ROSE_ASSERT(nondefiningDeclaration != NULL);
               ROSE_ASSERT(func->get_firstNondefiningDeclaration() == nondefiningDeclaration);
             }

       // DQ (12/14/2011): Added test.
          ROSE_ASSERT(scope != NULL);
          ROSE_ASSERT(func->get_scope() != NULL);
          ROSE_ASSERT(func->get_scope() == scope);

       // DQ (12/14/2011): Error checking
          SgTemplateInstantiationMemberFunctionDecl* testMemberDecl = isSgTemplateInstantiationMemberFunctionDecl(func);
          if (testMemberDecl != NULL)
             {
               ROSE_ASSERT(testMemberDecl->get_scope() != NULL);
               ROSE_ASSERT(testMemberDecl->get_associatedClassDeclaration() != NULL);
             }

       // DQ (12/18/2011): Testing to debug generation of wrong kind of declaration (symbol not found in correct scope or ...).
          if (isSgFunctionDeclaration(func) == NULL)
             {
            // If this is a SgTemplateDeclaration, then we shuld be able to find the name in the associated scope.
#if 0
               printf ("In buildNondefiningFunctionDeclaration_T(): Looking up name = %s in scope = %p = %s \n",name.str(),scope,scope->class_name().c_str());
#endif
            // DQ (8/12/2013): Added template parameter list.
            // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
            // In this case these are unavailable from this point.
            // DQ (12/18/2011): This fails because the first use of the function causes a non-defining function declaration
            // to be built and it is built as a template instantiation instead of a template declaration.  So the symbol for
            // the non-defining declaration is put into the correct scope, but as a SgMemberFunctionSymbol instead of as a
            // SgTemplateSymbol (if it were built as a SgTemplateMemberFunctionDeclaration).  So of course we can't find it
            // using lookup_template_symbol().
            // ROSE_ASSERT(scope->lookup_template_symbol(name) != NULL);
            // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments) != NULL);
            // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments,NULL,NULL) != NULL);
            // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments,templateParameterList,NULL) != NULL);
               ROSE_ASSERT(scope->lookup_template_function_symbol(nameWithTemplateArguments,func_type,templateParameterList) != NULL);
             }
        }

     ROSE_ASSERT(func != NULL);

     ROSE_ASSERT(func->get_file_info() == NULL);

  // DQ (3/8/2012): Added assertion.
     ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);
     assert(func_symbol != NULL);
     assert(func_symbol->get_symbol_basis() == func->get_firstNondefiningDeclaration());
     ROSE_ASSERT(func->get_symbol_from_symbol_table() != NULL || func->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table() != NULL);

  // DQ (2/24/2009): Delete the old parameter list build by the actualFunction (template argument) constructor.
     ROSE_ASSERT(func->get_parameterList() != NULL);
     delete func->get_parameterList();
     func->set_parameterList(NULL);

  // DQ (9/16/2012): Setup up the template arguments and the parents of the template arguments.
     if (buildTemplateInstantiation == true)
        {
          setTemplateArgumentsInDeclaration(func,templateArgumentsList);
        }

  // DQ (8/10/2013): Setup the template parameters if this is a template declaration.
     if (buildTemplateDeclaration == true)
        {
          setTemplateParametersInDeclaration(func,templateParameterList);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif

       // DQ (8/13/2013): Adding test of template parameter lists.
          SgTemplateFunctionDeclaration* templateFunctionDeclaration = isSgTemplateFunctionDeclaration(func);
#if 0
          if (templateFunctionDeclaration != NULL)
             {
               printf ("templateFunctionDeclaration->get_templateParameters().size() = %zu \n",templateFunctionDeclaration->get_templateParameters().size());
             }
#endif
          ROSE_ASSERT(templateFunctionDeclaration == NULL || (templateParameterList != NULL && templateParameterList->size() == templateFunctionDeclaration->get_templateParameters().size()));

          SgTemplateMemberFunctionDeclaration* templateMemberFunctionDeclaration = isSgTemplateMemberFunctionDeclaration(func);
          ROSE_ASSERT(templateMemberFunctionDeclaration == NULL || (templateParameterList != NULL && templateParameterList->size() == templateMemberFunctionDeclaration->get_templateParameters().size()));
        }

  // parameter list
  // DQ (11/23/2011): This change allows this to compile for where SgTemplateFunctionDeclarations are used.
  // setParameterList(func, paralist);
  // setParameterList(isSgFunctionDeclaration(func), paralist);
     setParameterList(func, paralist);

     SgInitializedNamePtrList argList = paralist->get_args();
     Rose_STL_Container<SgInitializedName*>::iterator argi;
     for (argi=argList.begin(); argi!=argList.end(); argi++)
        {
       // std::cout<<"patching argument's scope.... "<<std::endl;
          (*argi)->set_scope(scope);

       // DQ (2/23/2009): Also set the declptr (to NULL)
       // (*argi)->set_declptr(NULL);
        }

  // DQ (5/2/2012): Test this to make sure we have SgInitializedNames set properly.
     SageInterface::setSourcePosition(paralist);

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
  // Liao 11/21/2012: we should assert no transformation only when the current model is NOT transformation
     if (SourcePositionClassificationMode != e_sourcePositionTransformation)
        {
          detectTransformations_local(paralist);
        }
#endif

  // DQ (12/14/2011): Moved this closer to top of function.
  // TODO double check if there are exceptions
  // func->set_scope(scope);

  // DQ (12/14/2011): Added test.
     ROSE_ASSERT(scope != NULL);
     ROSE_ASSERT(func->get_scope() != NULL);
     ROSE_ASSERT(func->get_scope() == scope);

  // DQ (1/5/2009): This is not always true (should likely use SageBuilder::topScopeStack() instead)
     if (SageBuilder::topScopeStack() != NULL) // This comparison only makes sense when topScopeStack() returns non-NULL value
        {
       // since  stack scope is totally optional in SageBuilder.
          if (scope != SageBuilder::topScopeStack())
             {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("Warning: SageBuilder::buildNondefiningFunctionDeclaration_T(): scope parameter may not be the same as the topScopeStack() (e.g. for member functions) \n");
#endif
             }
        }

     func->set_parent(scope);

  // DQ (2/21/2009): We can't assert that this is always NULL or non-NULL.
  // ROSE_ASSERT(func->get_definingDeclaration() == NULL);

  // DQ (2/21/2009): Added assertion
     ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);

  // mark as a forward declartion
     func->setForward();

     ROSE_ASSERT(func->get_file_info() == NULL);

  // set File_Info as transformation generated or front end generated
     setSourcePositionAtRootAndAllChildren(func);

     ROSE_ASSERT(func->get_file_info() != NULL);

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
  // DQ (5/1/2012): Make sure that we don't have IR nodes marked as transformations.
     if (SourcePositionClassificationMode != e_sourcePositionTransformation)
        {
          detectTransformations_local(func);
        }
#endif

  // printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): generated function func = %p \n",func);

  // Liao 12/2/2010, special handling for Fortran functions and subroutines
     if ((SageInterface::is_Fortran_language() == true) && (getEnclosingFileNode(scope)->get_outputLanguage() == SgFile::e_Fortran_language))
        {
          SgProcedureHeaderStatement * f_func = isSgProcedureHeaderStatement(func);
          ROSE_ASSERT (f_func != NULL);
          if (return_type == buildVoidType())
               f_func->set_subprogram_kind(SgProcedureHeaderStatement::e_subroutine_subprogram_kind);
            else
               f_func->set_subprogram_kind(SgProcedureHeaderStatement::e_function_subprogram_kind);

       // hide it from the unparser since fortran prototype func declaration is internally used by ROSE AST
          f_func->get_startOfConstruct()->unsetOutputInCodeGeneration();
          f_func->get_endOfConstruct()->unsetOutputInCodeGeneration();
          ROSE_ASSERT(f_func->get_startOfConstruct()->isOutputInCodeGeneration() == false);
          ROSE_ASSERT(f_func->get_endOfConstruct()->isOutputInCodeGeneration() == false);
        }

  // DQ (12/11/2011): Added new test.
     ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);
     SgSymbol* symbol_from_first_nondefining_function = func->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table();
     ROSE_ASSERT(symbol_from_first_nondefining_function != NULL);

#if 0
     if (nameWithoutTemplateArguments == "getline")
        {
          printf ("In buildNondefiningFunctionDeclaration_T(): symbol_from_first_nondefining_function = %p \n",symbol_from_first_nondefining_function);
        }
#endif

  // DQ (12/11/2011): Note that this may be false when func is not the first nondefining declaration.
     if (func != func->get_firstNondefiningDeclaration())
        {
          SgSymbol* symbol_from_nondefining_function = func->get_symbol_from_symbol_table();
          ROSE_ASSERT(symbol_from_nondefining_function == NULL);
        }

  // DQ (12/18/2011): Testing to debug generation of wrong kind of declaration (symbol not found in correct scope or ...).
     if (isSgFunctionDeclaration(func) == NULL)
        {
       // If this is a SgTemplateDeclaration, then we shuld be able to find the name in the associated scope.
#if 0
          printf ("In buildNondefiningFunctionDeclaration_T(): Looking up nameWithTemplateArguments = %s in scope = %p = %s \n",nameWithTemplateArguments.str(),scope,scope->class_name().c_str());
#endif
       // DQ (8/12/2013): Make sure we use the template parameters and the template arguments that are available.
       // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
       // In this case these are unavailable from this point.
       // ROSE_ASSERT(scope->lookup_template_symbol(name) != NULL);
       // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments) != NULL);
       // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments,NULL,NULL) != NULL);
       // ROSE_ASSERT(scope->lookup_template_symbol(nameWithTemplateArguments,templateParameterList,NULL) != NULL);
          ROSE_ASSERT(scope->lookup_template_function_symbol(nameWithTemplateArguments,func_type,templateParameterList) != NULL);
        }

  // DQ (2/11/2012): If this is a template instantiation then we have to set the template name (seperate from the name of the function which can include template parameters)).
  // setTemplateNameInTemplateInstantiations(func,name);
     setTemplateNameInTemplateInstantiations(func,nameWithoutTemplateArguments);

#if 0
     printf ("Leaving buildNondefiningFunctionDeclaration_T(): function nameWithTemplateArguments = %s in scope = %p = %s func = %p func->get_firstNondefiningDeclaration() = %p \n",
          nameWithTemplateArguments.str(),scope,scope->class_name().c_str(),func,func->get_firstNondefiningDeclaration());
#endif

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
  // DQ (5/1/2012): Make sure that we don't have IR nodes marked as transformations.
     if (SourcePositionClassificationMode !=e_sourcePositionTransformation)
        {
          detectTransformations_local(func);
        }
#endif

  // DQ (12/11/2012): Force the two different ways that this can be set to match (we want consistancy).
     if (functionConstVolatileFlags & SgMemberFunctionType::e_restrict)
        {
          func->get_declarationModifier().get_typeModifier().setRestrict();
        }

  // DQ (8/19/2013): Added assertion that is tested and which fails for test_3 of the RoseExample_tests directory (in edgRose.C).
  // This fails for everything.... not sure why...
  // ROSE_ASSERT(func->get_symbol_from_symbol_table() != NULL);

#if 0
     printf ("In buildNondefiningFunctionDeclaration_T(): XXX_name = %s (calling unsetNodesMarkedAsModified()) \n", XXX_name.str());
#endif

  // DQ (4/16/2015): This is replaced with a better implementation.
  // Make sure the isModified boolean is clear for all newly-parsed nodes.
     unsetNodesMarkedAsModified(func);

#if 0
  // DQ (4/2/2018): Debugging case of two symbols for the same function in the same namespace (but different namespace definitions).
     if (nameWithoutTemplateArguments == "getline")
        {
          printf ("&&& Leaving buildNondefiningFunctionDeclaration_T(): getline: func: func = %p = %s unparseNameToString() = %s \n",func,func->class_name().c_str(),func->unparseNameToString().c_str());
          printf ("   --- scope = %p = %s \n",scope,scope->class_name().c_str());
          SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(scope);
          if (namespaceDefinition != NULL)
             {
               printf ("   --- namespaceDefinition: name = %s \n",namespaceDefinition->get_namespaceDeclaration()->get_name().str());
               printf ("   --- global namespace          = %p \n",namespaceDefinition->get_global_definition());
             }
            else
             {
               SgTemplateInstantiationDefn* templateInstantiationDefn = isSgTemplateInstantiationDefn(scope);
               if (templateInstantiationDefn != NULL)
                  {
                    SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(templateInstantiationDefn->get_declaration());
                    printf ("   --- templateInstantiationDecl: name = %s \n",templateInstantiationDecl->get_name().str());
                  }
                 else
                  {
                    SgTemplateClassDefinition* templateClassDefinition = isSgTemplateClassDefinition(scope);
                    if (templateClassDefinition != NULL)
                       {
                         SgTemplateClassDeclaration* templateClassDeclaration = isSgTemplateClassDeclaration(templateClassDefinition->get_declaration());
                         printf ("   --- templateClassDeclaration: name = %s \n",templateClassDeclaration->get_name().str());

                      // See where this is (because it happens twice in the same class).
                         templateClassDeclaration->get_file_info()->display("getline found in SgTemplateClassDeclaration: debug");
#if 0
                         printf ("Output the symbol tabel used for this template declaration/definition: \n");
                         templateClassDefinition->get_symbol_table()->print("getline found in SgTemplateClassDeclaration");
#endif
                       }
                  }
             }
          printf ("   --- symbol_from_first_nondefining_function = %p = %s \n",symbol_from_first_nondefining_function,symbol_from_first_nondefining_function->class_name().c_str());
        }
#endif

#if 0
     printf ("Leaving buildNondefiningFunctionDeclaration_T(): func: unparseNameToString() = %s \n",func->unparseNameToString().c_str());
#endif

     ROSE_ASSERT(paralist->get_parent() != NULL); 
     return func;
   }


//! Build a prototype for an existing function declaration (defining or nondefining )
SgFunctionDeclaration *
SageBuilder::buildNondefiningFunctionDeclaration (const SgFunctionDeclaration* funcdecl, SgScopeStatement* scope/*=NULL*/, SgExprListExp* decoratorList)
{
  ROSE_ASSERT(funcdecl!=NULL);
  SgName name=funcdecl->get_name();
  SgFunctionType * funcType = funcdecl->get_type();
  SgType* return_type = funcType->get_return_type();
  SgFunctionParameterList* paralist = deepCopy<SgFunctionParameterList>(funcdecl->get_parameterList());

 // make sure the function has consistent function type based on its return type and parameter list
 ROSE_ASSERT (funcType == buildFunctionType(funcdecl->get_type()->get_return_type(), buildFunctionParameterTypeList(funcdecl->get_parameterList())));
#if 0
  // DQ (2/19/2009): Fixed to handle extern "C" state in input "funcdecl"
  // return buildNondefiningFunctionDeclaration(name,return_type,paralist,scope);
  SgFunctionDeclaration* returnFunction = buildNondefiningFunctionDeclaration(name,return_type,paralist,scope,decoratorList);
#endif
  // buildNondefiningFunctionDeclaration() will check if a same function is created before by looking up function symbols.
  SgFunctionDeclaration* returnFunction  = buildNondefiningFunctionDeclaration (name, return_type, paralist, scope, decoratorList, false, NULL);

  returnFunction->set_linkage(funcdecl->get_linkage());
  if (funcdecl->get_declarationModifier().get_storageModifier().isExtern() == true)
  {
    returnFunction->get_declarationModifier().get_storageModifier().setExtern();
  }

  ROSE_ASSERT (returnFunction->get_linkage() == funcdecl->get_linkage());
  ROSE_ASSERT (returnFunction->get_declarationModifier().get_storageModifier().isExtern() ==
               funcdecl->get_declarationModifier().get_storageModifier().isExtern());

  ROSE_ASSERT(returnFunction->get_firstNondefiningDeclaration() != NULL);
  // Make sure that internal references are to the same file (else the symbol table information will not be consistent).
  if (scope != NULL)
  {
    // ROSE_ASSERT(returnFunction->get_parent() != NULL);
    ROSE_ASSERT(returnFunction->get_firstNondefiningDeclaration() != NULL);
  }

  return returnFunction;
}

SgFunctionDeclaration*
SageBuilder::buildNondefiningFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList)
   {
     SgFunctionDeclaration * result = NULL;
     if ((SageInterface::is_Fortran_language() == true) && (getEnclosingFileNode(scope)->get_outputLanguage() == SgFile::e_Fortran_language))
        {
       // result = buildNondefiningFunctionDeclaration_T <SgProcedureHeaderStatement> (name,return_type,paralist, /* isMemberFunction = */ false, scope, decoratorList,0);
          result = buildNondefiningFunctionDeclaration_T <SgProcedureHeaderStatement> (name,return_type,paralist, /* isMemberFunction = */ false, scope, decoratorList, false, NULL, NULL);
        }
       else
        {
#if 0
          printf ("In SageBuilder::buildNondefiningFunctionDeclaration(): buildTemplateInstantiation = %s \n",buildTemplateInstantiation ? "true" : "false");
#endif
       // DQ (11/27/2011): Added support to generate template declarations in the AST (this is part of a common API to make the build functions support more uniform).
          if (buildTemplateInstantiation == true)
             {
            // result = buildNondefiningFunctionDeclaration_T <SgTemplateInstantiationFunctionDecl> (name,return_type,paralist, /* isMemberFunction = */ false, scope, decoratorList,0);
               result = buildNondefiningFunctionDeclaration_T <SgTemplateInstantiationFunctionDecl> (name,return_type,paralist, /* isMemberFunction = */ false, scope, decoratorList, false, templateArgumentsList, NULL);
             }
            else
             {
            // result = buildNondefiningFunctionDeclaration_T <SgFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ false, scope, decoratorList,0);
               result = buildNondefiningFunctionDeclaration_T <SgFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ false, scope, decoratorList, false, NULL, NULL);
             }
        }

     return result;
   }


// DQ (8/28/2012): This preserves the original API with a simpler function (however for C++ at least, it is frequently not sufficent).
// We need to decide if the SageBuilder API should include these sorts of functions.
SgFunctionDeclaration*
SageBuilder::buildNondefiningFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* paralist, SgScopeStatement* scope, SgExprListExp* decoratorList)
   {
     return buildNondefiningFunctionDeclaration (name,return_type,paralist,scope,decoratorList,false,NULL);
   }


// DQ (8/28/2012): This preserves the original API with a simpler function (however for C++ at least, it is frequently not sufficent).
// We need to decide if the SageBuilder API should include these sorts of functions.
SgMemberFunctionDeclaration*
SageBuilder::buildNondefiningMemberFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* paralist, SgScopeStatement* scope)
   {
     unsigned int memberFunctionModifiers = 0;
     return buildNondefiningMemberFunctionDeclaration (name,return_type,paralist,scope,NULL,memberFunctionModifiers,false,NULL);
   }

// DQ (8/28/2012): This preserves the original API with a simpler function (however for C++ at least, it is frequently not sufficient).
// We need to decide if the SageBuilder API should include these sorts of functions.
SgMemberFunctionDeclaration*
SageBuilder::buildDefiningMemberFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* paralist, SgScopeStatement* scope)
   {
     unsigned int memberFunctionModifiers = 0;
  // each defining member function decl must have a non-defining counter part now. 11/27/2012, Liao
     SgMemberFunctionDeclaration* nondefining_decl = buildNondefiningMemberFunctionDeclaration (name, return_type, paralist, scope,NULL, memberFunctionModifiers, false, NULL);
     return buildDefiningMemberFunctionDeclaration (name,return_type,paralist,scope,NULL,false,memberFunctionModifiers,nondefining_decl,NULL);
   }


// SgTemplateFunctionDeclaration* SageBuilder::buildNondefiningTemplateFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope, SgExprListExp* decoratorList)
// SgTemplateFunctionDeclaration* SageBuilder::buildNondefiningTemplateFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, SgTemplateArgumentPtrList* templateArgumentsList)
SgTemplateFunctionDeclaration*
SageBuilder::buildNondefiningTemplateFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, SgTemplateParameterPtrList* templateParameterList)
   {
#if 0
  printf("In SageBuilder::buildNondefiningTemplateFunctionDeclaration(name = %s):\n", name.str());
#endif

  // DQ (8/15/2013): Note that we don't need template arguments because teplate functions can't support partial specialization.

  // DQ (11/25/2011): Adding support for template declarations in the AST.

  // DQ (8/7/2013): Added support for template function overloading using template parameters.
  // SgTemplateFunctionDeclaration* result = buildNondefiningFunctionDeclaration_T <SgTemplateFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ false, scope, decoratorList,0);
  // SgTemplateFunctionDeclaration* result = buildNondefiningFunctionDeclaration_T <SgTemplateFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ false, scope, decoratorList, false, NULL);
  // SgTemplateFunctionDeclaration* result = buildNondefiningFunctionDeclaration_T <SgTemplateFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ false, scope, decoratorList, false, templateArgumentsList);
     SgTemplateFunctionDeclaration* result = buildNondefiningFunctionDeclaration_T <SgTemplateFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ false, scope, decoratorList, false, NULL, templateParameterList);

#if 0
  // Optional debugging.
  // DQ (9/24/2015): Added more testing (for boost 1.54 and test2015_62.C).
     ROSE_ASSERT(result != NULL);
     ROSE_ASSERT(templateParameterList != NULL);
     ROSE_ASSERT(result->get_templateParameters().size() == templateParameterList->size());
     SgTemplateFunctionDeclaration* firstNondefining_result = isSgTemplateFunctionDeclaration(result->get_firstNondefiningDeclaration());
     ROSE_ASSERT(firstNondefining_result != NULL);
     if (firstNondefining_result->get_templateParameters().size() != templateParameterList->size())
        {
          printf ("name   = %s \n",name.str());
          printf ("result = %p \n",result);
          ROSE_ASSERT(scope != NULL);
          printf ("scope  = %p = %s \n",scope,scope->class_name().c_str());
          printf ("firstNondefining_result = %p \n",firstNondefining_result);
          printf ("templateParameterList->size()                            = %zu \n",templateParameterList->size());
          printf ("firstNondefining_result->get_templateParameters().size() = %zu \n",firstNondefining_result->get_templateParameters().size());
          firstNondefining_result->get_startOfConstruct()->display("Error: firstNondefining_result->get_templateParameters().size() == templateParameterList->size()");
        }
     ROSE_ASSERT(firstNondefining_result->get_templateParameters().size() == templateParameterList->size());
#endif

  // DQ (12/12/2011): Added test.
     ROSE_ASSERT(result != NULL);
     if (result->get_symbol_from_symbol_table() == NULL)
        {
#if 0
          printf ("In SageBuilder::buildNondefiningTemplateFunctionDeclaration(): result->get_symbol_from_symbol_table() == NULL \n");
#endif
          ROSE_ASSERT(result->get_firstNondefiningDeclaration() != NULL);
          ROSE_ASSERT(result != result->get_firstNondefiningDeclaration());
          ROSE_ASSERT(result->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table() != NULL);
        }
       else
        {
#if 0
       // DQ (2/12/2015): Added debugging code.
          printf ("In SageBuilder::buildNondefiningTemplateFunctionDeclaration(): result->get_symbol_from_symbol_table() != NULL \n");
          SgSymbol* symbol = result->get_symbol_from_symbol_table();
          printf ("symbol = %p = %s \n",symbol,symbol->class_name().c_str());
          SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(symbol);
          if (templateSymbol != NULL)
             {
               printf ("Found SgTemplateSymbol \n");
             }
#endif
        }

  // DQ (12/15/2011): We can't inforce this if it is not the first non-defining declaration (but we test for this above).
  // ROSE_ASSERT(result->get_symbol_from_symbol_table() != NULL);

  // DQ (2/12/2015): Added assertions earlier before calling buildDefiningFunctionDeclaration_T<>().
     ROSE_ASSERT(result->get_firstNondefiningDeclaration() != NULL);
#if 0
     if (result->get_firstNondefiningDeclaration() != result)
        {
          printf ("In SageBuilder::buildNondefiningTemplateFunctionDeclaration(): result->get_firstNondefiningDeclaration() = %p != result = %p: this is likely an additional prototype function \n",result->get_firstNondefiningDeclaration(),result);
          result->get_firstNondefiningDeclaration()->get_file_info()->display("In SageBuilder::buildNondefiningTemplateFunctionDeclaration(): result->get_firstNondefiningDeclaration() != result: debug");
        }
#endif
  // ROSE_ASSERT(result->get_firstNondefiningDeclaration() == result);

     return result;
   }

SgTemplateFunctionDeclaration*
SageBuilder::buildDefiningTemplateFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, SgTemplateFunctionDeclaration* first_nondefining_declaration)
   {
  // DQ (12/1/2011): Adding support for template declarations in the AST.

  // DQ (7/31/2013): Added assertions earlier before calling buildDefiningFunctionDeclaration_T<>().
     ROSE_ASSERT(first_nondefining_declaration != NULL);
     ROSE_ASSERT(first_nondefining_declaration->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(first_nondefining_declaration->get_firstNondefiningDeclaration() == first_nondefining_declaration);

  // template <class actualFunction> actualFunction * buildDefiningFunctionDeclaration_T (const SgName & name, SgType* return_type, SgFunctionParameterList * parlist, SgScopeStatement* scope=NULL, SgExprListExp* decoratorList = NULL);
  // SgTemplateFunctionDeclaration* result = buildDefiningFunctionDeclaration_T <SgTemplateFunctionDeclaration> (name,return_type,paralist,/* isMemberFunction = */ false, scope, decoratorList,functionConstVolatileFlags);
  // SgTemplateFunctionDeclaration* result = buildDefiningFunctionDeclaration_T <SgTemplateFunctionDeclaration> (name,return_type,paralist,/* isMemberFunction = */ false, scope, decoratorList, 0, first_nondefining_declaration);
     SgTemplateFunctionDeclaration* result = buildDefiningFunctionDeclaration_T <SgTemplateFunctionDeclaration> (name,return_type,paralist,/* isMemberFunction = */ false, scope, decoratorList, 0, first_nondefining_declaration, NULL);

     return result;
   }

SgTemplateMemberFunctionDeclaration*
SageBuilder::buildDefiningTemplateMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, SgTemplateMemberFunctionDeclaration* first_nondefining_declaration)
   {
  // DQ (12/1/2011): Adding support for template declarations in the AST.

     assert(first_nondefining_declaration->get_firstNondefiningDeclaration() == first_nondefining_declaration);

     SgTemplateMemberFunctionDeclaration* result = buildDefiningFunctionDeclaration_T <SgTemplateMemberFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ true, scope, decoratorList, functionConstVolatileFlags, first_nondefining_declaration, NULL);
     ROSE_ASSERT(result != NULL);

     ROSE_ASSERT(result->get_definition() != NULL);

#if 0
     printf ("This function is not yet finished being implemented in the build API! \n");
     ROSE_ASSERT(false);
#endif

     return result;
   }


#if 0
// DQ (3/20/2017): This function is not used (so let's see if we can remove it).
//! Build a prototype for an existing member function declaration (defining or nondefining )
SgMemberFunctionDeclaration *
SageBuilder::buildNondefiningMemberFunctionDeclaration (const SgMemberFunctionDeclaration* funcdecl, SgScopeStatement* scope/*=NULL*/, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags)
   {
     ROSE_ASSERT(funcdecl!=NULL);
     SgName name = funcdecl->get_name();

  // DQ (2/19/2009): Fixed to handle extern "C" state in input "funcdecl"
  // return buildNondefiningFunctionDeclaration(name,return_type,paralist,scope);
  // SgMemberFunctionDeclaration* returnFunction = buildNondefiningMemberFunctionDeclaration(name,return_type,paralist,scope,decoratorList);

#if 1
// DQ (7/26/2012): I am at least temporarily removing this function from the API.
// Later if we need it, we can update it to reflect that passing of the new
// SgTemplateArgumentPtrList function parameter (part of the new API design).

   SgMemberFunctionDeclaration* returnFunction = NULL;
   printf ("Error: buildNondefiningMemberFunctionDeclaration(): This function should not be used! \n");
   ROSE_ASSERT(false);
#else
     SgFunctionType * funcType = funcdecl->get_type();
     SgType* return_type = funcType->get_return_type();
     SgFunctionParameterList* paralist = deepCopy<SgFunctionParameterList>(funcdecl->get_parameterList());
     SgMemberFunctionDeclaration* returnFunction = buildNondefiningMemberFunctionDeclaration(name,return_type,paralist,scope,decoratorList,functionConstVolatileFlags);
#endif

     returnFunction->set_linkage(funcdecl->get_linkage());

     if (funcdecl->get_declarationModifier().get_storageModifier().isExtern() == true)
        {
          returnFunction->get_declarationModifier().get_storageModifier().setExtern();
        }

  // DQ (2/26/2009): Make this consistant with the non-member functions.
  // ROSE_ASSERT(returnFunction->get_definingDeclaration() == NULL);
     ROSE_ASSERT(returnFunction->get_firstNondefiningDeclaration() != NULL);

     return returnFunction;
   }
#endif

SgMemberFunctionDeclaration*
SageBuilder::buildNondefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope,
   SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList)
   {
  // This function builds either a SgMemberFunctionDeclaration (non-template; normal member function) or a SgTemplateInstantiationMemberFunctionDecl (template instantiation).

  // DQ (11/27/2011): Added support for instations of template member functions.
  // SgMemberFunctionDeclaration * result = buildNondefiningFunctionDeclaration_T <SgMemberFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ true,scope,decoratorList,functionConstVolatileFlags);
     SgMemberFunctionDeclaration * result = NULL;

#if 0
     printf ("In buildNondefiningMemberFunctionDeclaration(): buildTemplateInstantiation = %s \n",buildTemplateInstantiation ? "true" : "false");
#endif

     if (buildTemplateInstantiation == true)
        {
       // This is how we build an instantiation of a template (SgTemplateInstantiationMemberFunctionDecl).
          result = buildNondefiningFunctionDeclaration_T <SgTemplateInstantiationMemberFunctionDecl> (name,return_type,paralist, /* isMemberFunction = */ true,scope,decoratorList,functionConstVolatileFlags,templateArgumentsList,NULL);
        }
       else
        {
       // This is a non-template instatiation (normal member function).
          result = buildNondefiningFunctionDeclaration_T <SgMemberFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ true,scope,decoratorList,functionConstVolatileFlags,NULL,NULL);
        }
     ROSE_ASSERT(result != NULL);

  // set definingdecl for SgCtorInitializerList
     SgCtorInitializerList* ctor = result->get_CtorInitializerList();
     ROSE_ASSERT(ctor != NULL);

  // required in AstConsistencyTests.C:TestAstForProperlySetDefiningAndNondefiningDeclarations()
     ctor->set_definingDeclaration(ctor);
     ctor->set_firstNondefiningDeclaration(ctor);

  // DQ (1/4/2009): Error checking
     ROSE_ASSERT(result->get_associatedClassDeclaration() != NULL);

     if (result->get_associatedClassDeclaration() == NULL)
        {
          printf ("Warning, must set the SgMemberFunctionDeclaration::associatedClassDeclaration \n");

          ROSE_ASSERT(scope != NULL);
          SgClassDefinition* classDefinition = isSgClassDefinition(scope);
          ROSE_ASSERT(classDefinition != NULL);
          SgDeclarationStatement* associatedDeclaration = classDefinition->get_declaration();
          ROSE_ASSERT(associatedDeclaration != NULL);
          SgClassDeclaration* associatedClassDeclaration = isSgClassDeclaration(associatedDeclaration);

       // DQ (1/4/2009): This needs to be set, checked in AstConsistencyTests.C!
          result->set_associatedClassDeclaration(associatedClassDeclaration);
        }

#if 0
     printf ("In buildNondefiningMemberFunctionDeclaration(): result = %p result->get_firstNondefiningDeclaration() = %p \n",result,result->get_firstNondefiningDeclaration());
#endif

     return result;
   }


// DQ (8/12/2013): This function needs to handle the SgTemplateParameterPtrList (since it is generating a template).
// It need not take a SgTemplateArgumentPtrList because template functions (including template member functions) can not support partial specialization.
// SgTemplateMemberFunctionDeclaration* SageBuilder::buildNondefiningTemplateMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags)
SgTemplateMemberFunctionDeclaration*
SageBuilder::buildNondefiningTemplateMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, SgTemplateParameterPtrList* templateParameterList)
   {
#if 0
  printf("In SageBuilder::buildNondefiningTemplateMemberFunctionDeclaration(name = %s):\n", name.str());
#endif

  // This function only builds template member function declarations.

  // SgTemplateMemberFunctionDeclaration * result = buildNondefiningFunctionDeclaration_T <SgTemplateMemberFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ true,scope,decoratorList,functionConstVolatileFlags,NULL,NULL);
     SgTemplateMemberFunctionDeclaration * result = buildNondefiningFunctionDeclaration_T <SgTemplateMemberFunctionDeclaration> (name,return_type,paralist, /* isMemberFunction = */ true,scope,decoratorList,functionConstVolatileFlags,NULL,templateParameterList);

  // set definingdecl for SgCtorInitializerList
     ROSE_ASSERT(result != NULL);

#if 0
     printf(" In SageBuilder::buildNondefiningTemplateMemberFunctionDeclaration(...): result = %p (%s)\n", result, result->class_name().c_str());
     printf(" In SageBuilder::buildNondefiningTemplateMemberFunctionDeclaration(...): result->get_firstNondefiningDeclaration() = %p\n", result->get_firstNondefiningDeclaration());
#endif

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
  // DQ (5/1/2012): Make sure that we don't have IR nodes marked as translformations.
     if (SourcePositionClassificationMode != e_sourcePositionTransformation)
        {
          detectTransformations_local(result);
        }
#endif

#if 0
     printf ("After calling buildNondefiningFunctionDeclaration_T <SgTemplateMemberFunctionDeclaration>: result = %p = %s \n",result,result->class_name().c_str());
#endif

  // DQ (8/12/2013): Added template paremter list to call to get the function template symbol.
  // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
  // In this case these are unavailable from this point.
  // SgSymbol* associatedSymbol = scope->lookup_template_member_function_symbol(name,result->get_type());
  // SgSymbol* associatedSymbol = scope->lookup_template_member_function_symbol(name,result->get_type(),NULL);
     SgSymbol* associatedSymbol = scope->lookup_template_member_function_symbol(name,result->get_type(),templateParameterList);
     if (associatedSymbol == NULL)
        {
          printf ("ERROR: associatedSymbol == NULL \n");
          printf ("   --- result = %p = %s \n",result,result->class_name().c_str());
          printf ("   --- scope = %p = %s \n",scope,scope->class_name().c_str());
          printf ("   --- name = %s \n",name.str());
          printf ("   --- result->get_type() = %p = %s \n",result->get_type(),result->get_type()->class_name().c_str());
          printf ("   --- result->get_type()->get_mangled() = %s \n",result->get_type()->get_mangled().str());
       }
     ROSE_ASSERT(associatedSymbol != NULL);

#if 1
     SgCtorInitializerList * ctor = result->get_CtorInitializerList();
     ROSE_ASSERT(ctor != NULL);
  // required ty AstConsistencyTests.C:TestAstForProperlySetDefiningAndNondefiningDeclarations()
     ctor->set_definingDeclaration(ctor);
     ctor->set_firstNondefiningDeclaration(ctor);

#if 0
  // DQ (11/25/2011): I don't know if the SgTemplateMemberFunctionDeclaration needs this support!

  // DQ (1/4/2009): Error checking
     ROSE_ASSERT(result->get_associatedClassDeclaration() != NULL);

     if (result->get_associatedClassDeclaration() == NULL)
        {
          printf ("Warning, must set the SgMemberFunctionDeclaration::associatedClassDeclaration \n");

          ROSE_ASSERT(scope != NULL);
          SgClassDefinition* classDefinition = isSgClassDefinition(scope);
          ROSE_ASSERT(classDefinition != NULL);
          SgDeclarationStatement* associatedDeclaration = classDefinition->get_declaration();
          ROSE_ASSERT(associatedDeclaration != NULL);
          SgClassDeclaration* associatedClassDeclaration = isSgClassDeclaration(associatedDeclaration);

       // DQ (1/4/2009): This needs to be set, checked in AstConsistencyTests.C!
          result->set_associatedClassDeclaration(associatedClassDeclaration);
        }
#endif

#else
     printf ("Error: incomplete implementation of buildNondefiningTemplateMemberFunctionDeclaration() \n");
     ROSE_ASSERT(false);
#endif

  // DQ (12/11/2011): Added new test (also at the base of buildNondefiningFunctionDeclaration_T<>() function).
     ROSE_ASSERT(result->get_firstNondefiningDeclaration() != NULL);
     SgSymbol* symbol_from_first_nondefining_function = result->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table();
     ROSE_ASSERT(symbol_from_first_nondefining_function != NULL);

  // DQ (12/11/2011): Note that this may be false when func is not the first nondefining declaration.
     if (result != result->get_firstNondefiningDeclaration())
        {
          SgSymbol* symbol_from_nondefining_function = result->get_symbol_from_symbol_table();
          ROSE_ASSERT(symbol_from_nondefining_function == NULL);
        }

#if 0
     printf ("In buildNondefiningTemplateMemberFunctionDeclaration(): Looking up name = %s in scope = %p = %s \n",name.str(),scope,scope->class_name().c_str());
#endif

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_SYMBOL_TABLE_LOOKUP
  // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
  // In this case these are unavailable from this point.
  // ROSE_ASSERT(scope->lookup_template_symbol(name) != NULL);
  // if (scope->lookup_template_member_function_symbol(name,result->get_type()) == NULL)
  // if (scope->lookup_template_member_function_symbol(name,result->get_type(),NULL,NULL) == NULL)
     if (scope->lookup_template_member_function_symbol(name,result->get_type(),templateParameterList) == NULL)
        {
          printf ("Error: scope->lookup_template_member_function_symbol(name,result->get_type()) == NULL (investigate this) \n");
          printf ("--- function name = %s in scope = %p = %s result->get_type() = %p = %s \n",name.str(),scope,scope->class_name().c_str(),result->get_type(),result->get_type()->class_name().c_str());
          scope->get_symbol_table()->print("Error: scope->lookup_template_member_function_symbol(name,result->get_type()) == NULL (investigate this)");
        }
#endif
  // ROSE_ASSERT(scope->lookup_template_member_function_symbol(name,result->get_type()) != NULL);
  // ROSE_ASSERT(scope->lookup_template_member_function_symbol(name,result->get_type(),NULL) != NULL);
     ROSE_ASSERT(scope->lookup_template_member_function_symbol(name,result->get_type(),templateParameterList) != NULL);

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
  // DQ (5/1/2012): Make sure that we don't have IR nodes marked as transformations.
     if (SourcePositionClassificationMode != e_sourcePositionTransformation)
        {
          detectTransformations_local(result);
        }
#endif

#if 0
  // TV (04/12/2018): Add a scope for nonreal classes (and their member) on the first non-defining declaration of template member function
     if (result == result->get_firstNondefiningDeclaration()) {
       SgDeclarationScope * nonreal_decl_scope = new SgDeclarationScope();

       nonreal_decl_scope->set_parent(result);
       result->set_nonreal_decl_scope(nonreal_decl_scope);

       SageInterface::setSourcePosition(nonreal_decl_scope);
       nonreal_decl_scope->get_startOfConstruct()->setCompilerGenerated();
       nonreal_decl_scope->get_endOfConstruct()->setCompilerGenerated();
#if 1
       printf("In SageBuilder::buildNondefiningTemplateMemberFunctionDeclaration(name = %s): nrscope = %p (new)\n", name.str(), nonreal_decl_scope);
#endif
     }
#endif

     return result;
   }

#if 0
SgMemberFunctionDeclaration*
SageBuilder::buildNondefiningMemberFunctionDeclaration (const SgName & name, SgMemberFunctionType* func_type, SgFunctionParameterList* paralist, SgScopeStatement* scope)
   {
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

     ROSE_ASSERT(name.is_null() == false);
     ROSE_ASSERT(func_type != NULL);

     SgClassDefinition *struct_name = isSgClassDefinition(scope);
     ROSE_ASSERT(struct_name != NULL);


  // function declaration
     SgMemberFunctionDeclaration* func = NULL;

  // search before using the function type to create the function declaration
  // TODO only search current scope or all ancestor scope??
  // We don't have lookup_member_function_symbol  yet
  // SgFunctionSymbol *func_symbol = scope->lookup_function_symbol(name,func_type);
     SgFunctionSymbol *func_symbol = NULL;
     if (scope != NULL)
          func_symbol = scope->lookup_function_symbol(name,func_type);

  // printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): scope = %p func_symbol = %p \n",scope,func_symbol);
     if (func_symbol == NULL)
        {
       // first prototype declaration
          func = new SgMemberFunctionDeclaration (name,func_type,NULL);
          ROSE_ASSERT(func != NULL);

       // NOTE: we want to allow the input scope to be NULL (and even the SageBuilder::topScopeStack() == NULL)
       // so that function can be built bottom up style.  However this means that the symbol tables in the
       // scope of the returned function declaration will have to be setup separately.
          if (scope != NULL)
             {
            // function symbol table
               func_symbol = new SgMemberFunctionSymbol(func);

            // printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): scope = %p func_symbol = %p = %s = %s \n",scope,func_symbol,func_symbol->class_name().c_str(),SageInterface::get_name(func_symbol).c_str());
               ROSE_ASSERT(func_symbol != NULL);

               scope->insert_symbol(name, func_symbol);

            // ROSE_ASSERT(scope->lookup_function_symbol(name,func_type) != NULL);

            // DQ (2/26/2009): uncommented assertion.
               ROSE_ASSERT(scope->lookup_function_symbol(name) != NULL); // Did not pass for member function? Should we have used the mangled name?
             }

          func->set_firstNondefiningDeclaration(func);
          func->set_definingDeclaration(NULL);

          ROSE_ASSERT(func->get_definingDeclaration() == NULL);
        }
       else
        {
          ROSE_ASSERT(scope != NULL);

       // 2nd, or 3rd... prototype declaration
       // reuse function type, function symbol of previous declaration

       // std::cout<<"debug:SageBuilder.C: 267: "<<"found func_symbol!"<<std::endl;
       // delete (func_type-> get_argument_list ());
       // delete func_type; // bug 189

          func_type = isSgMemberFunctionType(func_symbol->get_declaration()->get_type());
          func = new SgMemberFunctionDeclaration(name,func_type,NULL);
          ROSE_ASSERT(func);

       // we don't care if it is member function or function here for a pointer
          SgFunctionDeclaration* prevDecl = NULL;
          prevDecl = func_symbol->get_declaration();
          ROSE_ASSERT(prevDecl != NULL);

       // printf ("In SageBuilder::buildNondefiningFunctionDeclaration_T(): prevDecl = %p \n",prevDecl);

          if (prevDecl == prevDecl->get_definingDeclaration())
             {
            // The symbol points to a defining declaration and now that we have added a non-defining
            // declaration we should have the symbol point to the new non-defining declaration.
            // printf ("Switching declaration in functionSymbol to point to the non-defining declaration \n");

               func_symbol->set_declaration(func);
             }

       // If this is the first non-defining declaration then set the associated data member.
          SgDeclarationStatement* nondefiningDeclaration = prevDecl->get_firstNondefiningDeclaration();
          if (nondefiningDeclaration == NULL)
             {
               nondefiningDeclaration = func;
             }

          ROSE_ASSERT(nondefiningDeclaration != NULL);

       // func->set_firstNondefiningDeclaration(prevDecl->get_firstNondefiningDeclaration());
          func->set_firstNondefiningDeclaration(nondefiningDeclaration);
          func->set_definingDeclaration(prevDecl->get_definingDeclaration());
        }

  // parameter list
     //SgFunctionParameterList* paralist = buildFunctionParameterList(func_type->get_argument_list());
     setParameterList(func, paralist);

     SgInitializedNamePtrList argList = paralist->get_args();
     Rose_STL_Container<SgInitializedName*>::iterator argi;
     for(argi=argList.begin(); argi!=argList.end(); argi++)
        {
       // std::cout<<"patching argument's scope.... "<<std::endl;
          (*argi)->set_scope(scope);

       // DQ (2/23/2009): Also set the declptr (to NULL)
       // (*argi)->set_declptr(NULL);
        }
  // TODO double check if there are exceptions
     func->set_scope(scope);

     // DQ (1/5/2009): This is not always true (should likely use SageBuilder::topScopeStack() instead)
     if (SageBuilder::topScopeStack()!= NULL) // This comparison only makes sense when topScopeStack() returns non-NULL value
        {
       // Since stack scope is totally optional in SageBuilder.
          if (scope != SageBuilder::topScopeStack())
             {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("Warning: SageBuilder::buildNondefiningFunctionDeclaration_T(): scope parameter may not be the same as the topScopeStack() (e.g. for member functions) \n");
#endif
             }
        }

     func->set_parent(scope);

  // DQ (2/21/2009): We can't assert that this is always NULL or non-NULL.
  // ROSE_ASSERT(func->get_definingDeclaration() == NULL);

  // DQ (2/21/2009): Added assertion
     ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);

  // mark as a forward declartion
     func->setForward();

  // set File_Info as transformation generated
     setSourcePositionAtRootAndAllChildren(func);

     return func;
   }
#endif

#if 0
// DQ (3/20/2017): This function is not used (so let's see if we can remove it).
// DQ (8/29/2012): This is re-enabled because the backstroke project is using it.
// DQ (7/26/2012): I would like to remove this from the API (at least for now while debugging the newer API required for template argument handling).
SgMemberFunctionDeclaration*
SageBuilder::buildDefiningMemberFunctionDeclaration (const SgName & name, SgMemberFunctionType* func_type, SgScopeStatement* scope, SgExprListExp* decoratorList, SgMemberFunctionDeclaration* first_nondefining_declaration)
   {
     SgType* return_type = func_type->get_return_type();
     SgFunctionParameterList* paralist = buildFunctionParameterList(func_type->get_argument_list());

     bool buildTemplateInstantiation         = false;
     unsigned int functionConstVolatileFlags = 0;
     SgTemplateArgumentPtrList* templateArgumentsList = NULL;

     return SageBuilder::buildDefiningMemberFunctionDeclaration(name, return_type, paralist, scope, decoratorList, buildTemplateInstantiation, functionConstVolatileFlags, first_nondefining_declaration, templateArgumentsList);
   }
#endif

#if 0
// DQ (7/26/2012): I would like to remove this from the API (at least for now while debugging the newer API required for template argument handling).

// DQ (5/12/2012): This interferes with the other function:
// SageBuilder::buildDefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, unsigned int functionConstVolatileFlags, SgMemberFunctionDeclaration* first_nondefining_declaration)
// Once the default arguments are removed.
// Comment this out for now.  this might be one too many API functions anyway.

SgMemberFunctionDeclaration*
SageBuilder::buildDefiningMemberFunctionDeclaration (const SgName & name, SgMemberFunctionType* func_type, SgFunctionParameterList* paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, SgMemberFunctionDeclaration* first_nondefining_declaration)
{
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);
  ROSE_ASSERT(name.is_null() == false);
  ROSE_ASSERT(func_type != NULL);

  SgMemberFunctionDeclaration * func;

 //  symbol table and non-defining
  SgMemberFunctionSymbol *func_symbol = isSgMemberFunctionSymbol(scope->lookup_function_symbol(name,func_type));
  if (func_symbol == NULL)
  {
    // new defining declaration
//    func = new SgFunctionDeclaration(name,func_type,NULL);
    func = new SgMemberFunctionDeclaration(name,func_type,NULL);
    ROSE_ASSERT(func);
    SgMemberFunctionSymbol *func_symbol = new SgMemberFunctionSymbol(func);
    scope->insert_symbol(name, func_symbol);
    func->set_firstNondefiningDeclaration(NULL);
  } else
  {
    // defining declaration after nondefining declaration
    //reuse function type, function symbol

//    delete func_type;// bug 189

    // Cong (10/25/2010): Make sure in this situation there is no defining declaration for this symbol.
    //ROSE_ASSERT(func_symbol->get_declaration()->get_definingDeclaration() == NULL);

    func_type = isSgMemberFunctionType(func_symbol->get_declaration()->get_type());
    //func = new SgFunctionDeclaration(name,func_type,NULL);
    func = new SgMemberFunctionDeclaration(name,func_type,NULL);
    ROSE_ASSERT(func);

    func->set_firstNondefiningDeclaration(func_symbol->get_declaration()->get_firstNondefiningDeclaration());

    // fix up defining declarations before current statement
    func_symbol->get_declaration()->set_definingDeclaration(func);
    //for the rare case that two or more prototype declaration exist
    // cannot do anything until append/prepend_statment() is invoked
  }

  // handle decorators
  if (decoratorList != NULL)
  {
      func->set_decoratorList(decoratorList);
      decoratorList->set_parent(func);
  }

  // definingDeclaration
  func->set_definingDeclaration(func);

  // function body and definition are created before setting argument list
  SgBasicBlock * func_body = new SgBasicBlock();
  ROSE_ASSERT(func_body);
  SgFunctionDefinition * func_def = new SgFunctionDefinition(func,func_body);
  ROSE_ASSERT(func_def);

  // DQ (11/28/2010): Added specification of case insensitivity (e.g. Fortran).
  if (symbol_table_case_insensitive_semantics == true)
     {
       func_def->setCaseInsensitive(true);
       func_body->setCaseInsensitive(true);
     }

  func_def->set_parent(func);
  func_def->set_body(func_body);
  func_body->set_parent(func_def);

   // parameter list,
   //TODO consider the difference between C++ and Fortran
  //SgFunctionParameterList* paralist = buildFunctionParameterList(func_type->get_argument_list());
  setParameterList(func,paralist);
         // fixup the scope and symbol of arguments,
  SgInitializedNamePtrList& argList = paralist->get_args();
  Rose_STL_Container<SgInitializedName*>::iterator argi;
  for(argi=argList.begin(); argi!=argList.end(); argi++)
  {
//    std::cout<<"patching defining function argument's scope and symbol.... "<<std::endl;
    (*argi)->set_scope(func_def);
    func_def->insert_symbol((*argi)->get_name(), new SgVariableSymbol(*argi) );
  }

  func->set_parent(scope);
  func->set_scope(scope);

  // set File_Info as transformation generated
  setSourcePositionAtRootAndAllChildren(func);
  return func;
}
#endif


// SgMemberFunctionDeclaration* SageBuilder::buildDefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, unsigned int functionConstVolatileFlags, SgMemberFunctionDeclaration* first_nondefining_declaration)
SgMemberFunctionDeclaration*
SageBuilder::buildDefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, unsigned int functionConstVolatileFlags, SgMemberFunctionDeclaration* first_nondefining_declaration, SgTemplateArgumentPtrList* templateArgumentsList)
   {

#if 0
     SgMemberFunctionDeclaration * result = buildDefiningFunctionDeclaration_T <SgMemberFunctionDeclaration> (name,return_type,paralist,/* isMemberFunction = */ true,scope,decoratorList);

  // DQ (12/1/2011): This API is not yet supported for member function template instantiations.
  // ROSE_ASSERT(buildTemplateInstantiation == false);
     if (buildTemplateInstantiation == true)
        {
       // DQ (12/18/2011): I can't think of what more might be required here, but there may be something missing.
          printf ("WARNING: In buildDefiningMemberFunctionDeclaration(): Case of buildTemplateInstantiation == true may be incomplete \n");

          ROSE_ASSERT(isSgTemplateInstantiationMemberFunctionDecl(result) != NULL);
        }
#else
  // DQ (12/18/2011): Need to build a SgTemplateInstantiationMemberFunctionDecl when buildTemplateInstantiation == true
     SgMemberFunctionDeclaration * result = NULL;
     if (buildTemplateInstantiation == true)
        {
       // SgTemplateArgumentPtrList emptyList;
       // nondefdecl = new SgTemplateInstantiationMemberFunctionDecl(name,return_type,paralist,/* isMemberFunction = */ true,scope,decoratorList);
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(first_nondefining_declaration);
          ROSE_ASSERT(templateInstantiationMemberFunctionDecl != NULL);

#if 1
       // DQ (1/26/2013): Added test failing in buildDefiningFunctionDeclaration_T().
             {
               ROSE_ASSERT(templateArgumentsList != NULL);
               string nameWithoutTemplateArguments = name;
               string nameWithTemplateArguments = appendTemplateArgumentsToName(nameWithoutTemplateArguments,*templateArgumentsList);
               SgMemberFunctionType* func_type = isSgMemberFunctionType(first_nondefining_declaration->get_type());
               ROSE_ASSERT(func_type != NULL);

            // DQ (8/7/2013): API change due to added support for template function overloading using template parameters.
            // SgSymbol* func_symbol = scope->get_symbol_table()->find_symbol_by_type_of_function<SgTemplateInstantiationMemberFunctionDecl>(nameWithTemplateArguments,func_type);
            // SgSymbol* func_symbol = scope->find_symbol_by_type_of_function<SgTemplateInstantiationMemberFunctionDecl>(nameWithTemplateArguments,func_type);
            // SgSymbol* func_symbol = scope->find_symbol_by_type_of_function<SgTemplateInstantiationMemberFunctionDecl>(nameWithTemplateArguments,func_type,NULL);
               SgSymbol* func_symbol = scope->find_symbol_by_type_of_function<SgTemplateInstantiationMemberFunctionDecl>(nameWithTemplateArguments,func_type,NULL,templateArgumentsList);
               if (func_symbol == NULL)
                  {
                    printf ("ERROR caught in SageBuilder::buildDefiningMemberFunctionDeclaration(): nameWithTemplateArguments = %s buildTemplateInstantiation = %s \n",nameWithTemplateArguments.c_str(),buildTemplateInstantiation ? "true:" : "false");
                    printf ("ERROR caught in SageBuilder::buildDefiningMemberFunctionDeclaration(): func_symbol == NULL for first_nondefining_declaration = %p = %s and func_type = %p = %s \n",
                         templateInstantiationMemberFunctionDecl,templateInstantiationMemberFunctionDecl->class_name().c_str(),func_type,func_type->class_name().c_str());
                  }
            // ROSE_ASSERT(func_symbol != NULL);
             }
#endif

       // result = buildDefiningFunctionDeclaration_T <SgTemplateInstantiationMemberFunctionDecl> (name,return_type,paralist,/* isMemberFunction = */ true,scope,decoratorList,functionConstVolatileFlags,templateInstantiationMemberFunctionDecl);
          result = buildDefiningFunctionDeclaration_T <SgTemplateInstantiationMemberFunctionDecl> (name, return_type, paralist, /* isMemberFunction = */ true, scope, decoratorList, functionConstVolatileFlags, templateInstantiationMemberFunctionDecl, templateArgumentsList);
#if 0
          printf ("In SageBuilder::buildDefiningMemberFunctionDeclaration(): isSgTemplateInstantiationMemberFunctionDecl(result)->get_templateName() = %s \n",isSgTemplateInstantiationMemberFunctionDecl(result)->get_templateName().str());
#endif
          ROSE_ASSERT(isSgTemplateInstantiationMemberFunctionDecl(result) != NULL);
          ROSE_ASSERT(isSgTemplateInstantiationMemberFunctionDecl(result)->get_templateName().is_null() == false);
        }
       else
        {
          ROSE_ASSERT(first_nondefining_declaration != NULL);

       // DQ (12/27/20134): Added these to permit testing earlier than in the buildDefiningFunctionDeclaration_T() function.
          ROSE_ASSERT(first_nondefining_declaration->get_firstNondefiningDeclaration() != NULL);
          ROSE_ASSERT(first_nondefining_declaration->get_firstNondefiningDeclaration() == first_nondefining_declaration);

          result = buildDefiningFunctionDeclaration_T <SgMemberFunctionDeclaration> (name,return_type,paralist,/* isMemberFunction = */ true,scope,decoratorList,functionConstVolatileFlags,first_nondefining_declaration, NULL);
        }
#endif

     ROSE_ASSERT(result != NULL);

  // set definingdecl for SgCtorInitializerList
     SgCtorInitializerList* ctor = result->get_CtorInitializerList();
     ROSE_ASSERT(ctor != NULL);

  // required ty AstConsistencyTests.C:TestAstForProperlySetDefiningAndNondefiningDeclarations()
     ctor->set_definingDeclaration(ctor);
     ctor->set_firstNondefiningDeclaration(ctor);

//     if (result->get_associatedClassDeclaration() == NULL && first_nondefining_declaration != NULL && first_nondefining_declaration->get_associatedClassDeclaration() != NULL)
//       result->set_associatedClassDeclaration(first_nondefining_declaration->get_associatedClassDeclaration());

  // DQ (1/4/2009): Error checking
     ROSE_ASSERT(result->get_associatedClassDeclaration() != NULL);
#if 0
     printf ("Looking up name = %s in scope = %p = %s \n",name.str(),scope,scope->class_name().c_str());
  // ROSE_ASSERT(scope->lookup_template_symbol(name) != NULL);
     ROSE_ASSERT(scope->lookup_template_symbol(name,templateParameterList) != NULL);
#endif

     return result;
   }


//----------------- defining function declaration------------
// a template builder for all kinds of defining SgFunctionDeclaration
// handle common chores for function type, symbol, paramter etc.

template <class actualFunction>
actualFunction*
SageBuilder::buildDefiningFunctionDeclaration_T(const SgName & XXX_name, SgType* return_type, SgFunctionParameterList* paralist, bool isMemberFunction, SgScopeStatement* scope, SgExprListExp* decoratorList, unsigned int functionConstVolatileFlags, actualFunction* first_nondefining_declaration, SgTemplateArgumentPtrList* templateArgumentsList)
   {
#if 0
     printf ("In buildDefiningFunctionDeclaration_T():\n");
     printf ("  --- XXX_name = %s \n", XXX_name.str());
     printf ("  --- return_type = %p (%s) \n", return_type, return_type ? return_type->class_name().c_str() : "");
     printf ("  --- first_nondefining_declaration = %p (%s) \n", first_nondefining_declaration, first_nondefining_declaration ? first_nondefining_declaration->class_name().c_str() : "");
#endif
  // Note that the semantics of this function now differs from that of the buildDefiningClassDeclaration().
  // We want to have the non-defining declaration already exist before calling this function.
  // We could still build a higher level function that built both together.  Or we could provide two versions
  // named differently (from this one) and depricate this function...which I like much better.

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
     printf ("WARNING: This function for building defining function declarations has different semantics from that of the function to build defining class declarations. \n");
#endif

     assert(first_nondefining_declaration != NULL);
     assert(first_nondefining_declaration->get_firstNondefiningDeclaration() != NULL);
     assert(first_nondefining_declaration->get_firstNondefiningDeclaration() == first_nondefining_declaration);

     if (scope == NULL)
        {
          scope = SageBuilder::topScopeStack();
        }

     ROSE_ASSERT(XXX_name.is_null() == false);
     ROSE_ASSERT(scope != NULL);
     ROSE_ASSERT(return_type != NULL);

     SgName nameWithoutTemplateArguments = XXX_name;
     SgName nameWithTemplateArguments    = nameWithoutTemplateArguments;

     bool buildTemplateInstantiation = ((VariantT)actualFunction::static_variant == V_SgTemplateInstantiationFunctionDecl || (VariantT)actualFunction::static_variant == V_SgTemplateInstantiationMemberFunctionDecl);

  // DQ (8/7/2013): Added support for template declarations.
     bool buildTemplateDeclaration   = ((VariantT)actualFunction::static_variant == V_SgTemplateFunctionDeclaration || (VariantT)actualFunction::static_variant == V_SgTemplateMemberFunctionDeclaration);

#if 0
     printf ("In buildDefiningFunctionDeclaration_T(): buildTemplateInstantiation = %s \n",buildTemplateInstantiation ? "true" : "false");
     printf ("In buildDefiningFunctionDeclaration_T(): buildTemplateDeclaration   = %s \n",buildTemplateDeclaration ? "true" : "false");
#endif

  // DQ (8/11/2013): Check that the template argument lists are consistant.  The templateArgumentsList can then be considered redundant if this works.
     if (buildTemplateInstantiation == true)
        {
          ROSE_ASSERT(templateArgumentsList != NULL);

          SgTemplateArgumentPtrList & templateArgumentsList_from_first_nondefining_declaration = (isMemberFunction == false) ?
               isSgTemplateInstantiationFunctionDecl(first_nondefining_declaration)->get_templateArguments() :
               isSgTemplateInstantiationMemberFunctionDecl(first_nondefining_declaration)->get_templateArguments();

          ROSE_ASSERT(templateArgumentsList != NULL);
#if 0
       // printf ("templateArgumentsList                                           = %p \n",templateArgumentsList);
          printf ("templateArgumentsList = %p templateArgumentsList->size() = %zu \n",templateArgumentsList,templateArgumentsList->size());
          printf ("templateArgumentsList_from_first_nondefining_declaration.size()      = %zu \n",templateArgumentsList_from_first_nondefining_declaration.size());
#endif
#if 1
          bool templateArgumentListsAreEquivalent = SageInterface::templateArgumentListEquivalence(*templateArgumentsList, templateArgumentsList_from_first_nondefining_declaration);
#if 0
       // DQ (3/21/2017): Removing debugging code (leaving assertion).
          if (templateArgumentListsAreEquivalent == false)
             {
               printf ("after test: nameWithoutTemplateArguments  = %s \n",nameWithoutTemplateArguments.str());
               printf ("after test: nameWithTemplateArguments     = %s \n",nameWithTemplateArguments.str());
               printf ("after test: templateArgumentsList         = %p \n",templateArgumentsList);
               printf ("after test: templateArgumentsList->size() = %zu \n",templateArgumentsList != NULL ? templateArgumentsList->size() : (size_t)0L);
               printf ("after test: templateArgumentsList_from_first_nondefining_declaration.size() = %zu \n",templateArgumentsList_from_first_nondefining_declaration.size());
               if (first_nondefining_declaration != NULL)
                  {
                    printf ("first_nondefining_declaration = %p = %s = %s \n",first_nondefining_declaration,
                         first_nondefining_declaration->class_name().c_str(),first_nondefining_declaration->get_name().str());
                    ROSE_ASSERT(first_nondefining_declaration->get_file_info() != NULL);
                    first_nondefining_declaration->get_file_info()->display("first_nondefining_declaration: debug");
                  }
             }
#endif
          ROSE_ASSERT(templateArgumentListsAreEquivalent == true);
#else
          ROSE_ASSERT(SageInterface::templateArgumentListEquivalence(*templateArgumentsList, templateArgumentsList_from_first_nondefining_declaration));
#endif
        }

     SgTemplateParameterPtrList* templateParameterList = NULL;
     if (buildTemplateDeclaration == true)
        {
       // DQ (8/11/2013): Since this is not passed in so we can access it but not assert its equivalence with a redundant input parameter.
          templateParameterList = (isMemberFunction == false) ?
               &(isSgTemplateFunctionDeclaration(first_nondefining_declaration)->get_templateParameters()) :
               &(isSgTemplateMemberFunctionDeclaration(first_nondefining_declaration)->get_templateParameters());

          ROSE_ASSERT(templateArgumentsList == NULL);
          ROSE_ASSERT(templateParameterList != NULL);
#if 0
          if (templateParameterList != NULL)
             {
                ROSE_ASSERT(templateArgumentsList == NULL);
             }
          ROSE_ASSERT(templateParameterList == NULL || templateParameterList->size() == templateArgumentsList->size());
#endif
        }

     if (buildTemplateInstantiation == true)
        {
          ROSE_ASSERT(templateArgumentsList != NULL);
          nameWithTemplateArguments = appendTemplateArgumentsToName(nameWithoutTemplateArguments,*templateArgumentsList);
#if 0
          printf ("Building a defining function: buildDefiningFunctionDeclaration_nfi() nameWithTemplateArguments = %s buildTemplateInstantiation = %s \n",nameWithTemplateArguments.str(),buildTemplateInstantiation ? "true:" : "false");
#endif

          if (nameWithTemplateArguments == "insert < __normal_iterator< SgInitializedName ** , __type  >  > ")
             {
               printf ("In buildDefiningFunctionDeclaration_T(): Found function nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
             }
        }

#if 0
     printf ("In buildDefiningFunctionDeclaration_T(): nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
#endif

     ROSE_ASSERT(nameWithoutTemplateArguments.is_null() == false);
     ROSE_ASSERT(nameWithTemplateArguments.is_null() == false);

  // DQ (12/3/2011): Added more checking.
     ROSE_ASSERT(paralist != NULL);

     if (SageInterface::is_Python_language() == false)
        {
          ROSE_ASSERT(scope->containsOnlyDeclarations());
        }

  // actualFunction* firstNondefiningFunctionDeclaration = NULL;
     actualFunction* defining_func = NULL;

  // build function type, manage function type symbol internally
  // SgFunctionType* func_type = buildFunctionType(return_type,paralist);
     SgFunctionType* func_type = NULL;

  // DQ (5/11/2012): Enforce this so that we can avoid building the function type (be reusing the function type of the first non-defining declaration).
  // This is a special problem for templates because the function parameters will evaluate different for different builds of the same template.
  // This is a problem for test2012_74.C (and a dozen other test codes that make use of STL).
     ROSE_ASSERT(first_nondefining_declaration != NULL);

#if 0
     printf ("In buildDefiningFunctionDeclaration_T(): isMemberFunction = %s \n",isMemberFunction ? "true" : "false");
#endif

  // DQ (7/27/2012): Note that the input name should not have template argument syntax.
  // I think this could still fail for a function with a name such as "X<Y>"  strange converstion operators.
  // DQ (7/27/2012): There are reasons why this can fail: e.g. for functions with names such as:
  // "operator std::auto_ptr_ref<_Tp1>" which is a user defined conversion operator to one class from another.
  // ROSE_ASSERT(SageInterface::hasTemplateSyntax(nameWithoutTemplateArguments) == false);

#if 1
  // DQ (5/12/2012): Use the newly added parameter to get the exact SgFunctionType used to build the symbol.
  // This should make the template handling more robust since we were sometimes using types that had different
  // levels of template instantiation between the non-definng and defining function declarations and this
  // caused symbols build to support the non-defining declaration to not be found when we searched for them
  // using the function type built for the defining declaration.  We want the function types for all defining
  // and non-defining declarations to be identical.  This define also means that we don't have to build a
  // SgFunctionType just to look up a symbol in the symbol table (which was always silly).  However, only
  // the defining function declaration can use the existing function type because it is required that a
  // non-defining declaration exist prior to the construction of the defining declaration (built by this
  // function).
     func_type = first_nondefining_declaration->get_type();
#else
     if (isMemberFunction == true)
        {
#if 0
          printf ("return_type = %p = %s \n",return_type,return_type->class_name().c_str());
#endif
          ROSE_ASSERT(isSgClassDefinition(scope) != NULL);
       // func_type = buildMemberFunctionType(return_type,paralist,isSgClassDefinition(scope),0);
          func_type = buildMemberFunctionType(return_type,paralist,isSgClassDefinition(scope),functionConstVolatileFlags);
        }
       else
        {
          func_type = buildFunctionType(return_type,paralist);
        }
#endif
     ROSE_ASSERT(func_type != NULL);

  // Make sure these are the same (this will fail until we generate the func_type directly from first_nondefining_declaration).
     ROSE_ASSERT(func_type == first_nondefining_declaration->get_type());

     SgDeclarationStatement* firstNondefiningFunctionDeclaration = NULL;

#if 0
     printf ("In buildDefiningFunctionDeclaration_T(): scope     = %p = %s \n",scope,scope->class_name().c_str());
     printf ("In buildDefiningFunctionDeclaration_T(): func_type = %p = %s \n",func_type,func_type->class_name().c_str());
     printf ("In buildDefiningFunctionDeclaration_T(): Looking for function in symbol table with nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
#endif

  // symbol table and non-defining
  // SgFunctionSymbol *func_symbol = scope->lookup_function_symbol(name,func_type);
  // SgSymbol* func_symbol = scope->lookup_function_symbol(name,func_type);

     ROSE_ASSERT(scope != NULL);
  // ROSE_ASSERT(scope->get_symbol_table() != NULL);

  // DQ (8/7/2013): API change due to added support for template function overloading using template parameters.
  // SgSymbol* func_symbol = scope->get_symbol_table()->find_symbol_by_type_of_function<actualFunction>(name,func_type);
  // SgSymbol* func_symbol = scope->get_symbol_table()->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type);
  // SgSymbol* func_symbol = scope->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type);
  // SgSymbol* func_symbol = scope->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type,NULL,templateArgumentsList);
     SgSymbol* func_symbol = scope->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type,templateParameterList,templateArgumentsList);

#if 0
     if (func_symbol == NULL)
        {
          printf ("In buildDefiningFunctionDeclaration_T(): could not find function symbol for name = %s \n",name.str());

       // Look for the template function
       // func_symbol = scope->lookup_template_symbol(name,func_type);
       // func_symbol = scope->lookup_template_symbol(name);

#error "DEAD CODE!"

       // DQ (3/10/2012): Fix this to call lookup_template_function_symbol() instead.
       // func_symbol = scope->lookup_template_symbol(name);
          func_symbol = scope->lookup_template_function_symbol(name,func_type);

          if (func_symbol == NULL)
             {
               printf ("In buildDefiningFunctionDeclaration_T(): could not find template symbol for name = %s \n",name.str());
             }
          ROSE_ASSERT(func_symbol != NULL);
        }
#else
#if 0
  // DQ (3/21/2017): Removing debugging code (leaving assertion).
  // DQ (5/10/2012): This is a failing assertion, I'm not sure that we can enforce this in general (see test2004_149.C).
  // DQ (3/13/2012): Note that this function building a defining declaration can enforce that the non-defining declaration has already build a proper symbol.
     if (func_symbol == NULL)
        {
          printf ("In buildDefiningFunctionDeclaration_T(): func_symbol == NULL We can't assume that the symbol already exists. \n");

          printf ("In buildDefiningFunctionDeclaration_T(): nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
          printf ("In buildDefiningFunctionDeclaration_T(): first_nondefining_declaration = %p = %s \n",
               first_nondefining_declaration,first_nondefining_declaration->class_name().c_str());

          printf ("In buildDefiningFunctionDeclaration_T(): scope                    = %p = %s \n",scope,scope->class_name().c_str());
          printf ("In buildDefiningFunctionDeclaration_T(): first_nondefining_declaration->get_scope() = %p = %s \n",
               first_nondefining_declaration->get_scope(),first_nondefining_declaration->get_scope()->class_name().c_str());

          printf ("In buildDefiningFunctionDeclaration_T(): func_type                = %p = %s \n",func_type,func_type->class_name().c_str());
          printf ("In buildDefiningFunctionDeclaration_T(): first_nondefining_declaration->get_type() = %p = %s \n",
               first_nondefining_declaration->get_type(),first_nondefining_declaration->get_type()->class_name().c_str());

          printf ("In buildDefiningFunctionDeclaration_T(): func_type->get_mangled() = %s \n",func_type->get_mangled().str());
          printf ("In buildDefiningFunctionDeclaration_T(): Looking for function in symbol table with name = %s \n",nameWithTemplateArguments.str());

#if 0
          scope->get_symbol_table()->print("In SageBuilder::buildDefiningFunctionDeclaration_T()");
#endif

       // DQ (1/29/2013): Retry using the name from the non-definging function declaration.
          SgFunctionDeclaration* nondefiningFunctionDeclaration = isSgFunctionDeclaration(first_nondefining_declaration);
          ROSE_ASSERT(nondefiningFunctionDeclaration != NULL);

          nameWithTemplateArguments = nondefiningFunctionDeclaration->get_name();

       // DQ (8/7/2013): API change due to added support for template function overloading using template parameters.
       // func_symbol = scope->get_symbol_table()->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type);
       // func_symbol = scope->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type);
       // func_symbol = scope->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type,templateArgumentsList);
       // func_symbol = scope->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type,NULL,templateArgumentsList);
          func_symbol = scope->find_symbol_by_type_of_function<actualFunction>(nameWithTemplateArguments,func_type,templateParameterList,templateArgumentsList);

          printf ("In buildDefiningFunctionDeclaration_T(): func_symbol = %p reset using lookup with reset nameWithTemplateArguments = %s \n",func_symbol,nameWithTemplateArguments.str());

          ROSE_ASSERT(func_symbol != NULL);

          printf ("In buildDefiningFunctionDeclaration_T(): func_symbol = %p = %s reset using lookup with reset nameWithTemplateArguments = %s \n",func_symbol,func_symbol->class_name().c_str(),nameWithTemplateArguments.str());
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

  // DQ (1/26/2013): This fails for ROSE compiling ROSE.
     ROSE_ASSERT(func_symbol != NULL);
#endif

#if 0
     printf ("In buildDefiningFunctionDeclaration_T(): func_symbol = %p \n",func_symbol);
#endif

#if 0
  // Not ready for member functions yet, need to generate SgMemberFucntionType instead of SgFunctionType.
     printf ("Not ready for member functions yet, need to generate SgMemberFucntionType instead of SgFunctionType. \n");
     ROSE_ASSERT(isMemberFunction == false);
#endif

     if (func_symbol == NULL)
        {
          printf ("Could not find an existing symbol for this function! \n");
       // scope->get_symbol_table()->print("Could not find an existing symbol for this function!");

       // DQ (12/2/2011): After discussion with Liao, we think this should be an error.
       // The defining declaration requires that the associated non-defining declaration should already exist.
       // If required, a higher level build function could build both of these and connect them as required.
          printf ("Error: building a defining declaration requires that the associated non-defining declaration already exists and it's symbol found the the same scope's symbol table! \n");
          ROSE_ASSERT(false);
#if 0
       // new defining declaration
       // func = new SgFunctionDeclaration(name,func_type,NULL);
          defining_func = new actualFunction(name,func_type,NULL);
          ROSE_ASSERT(defining_func != NULL);

          firstNondefiningFunctionDeclaration = defining_func;
          ROSE_ASSERT(firstNondefiningFunctionDeclaration != NULL);
#if 0
          SgFunctionSymbol *func_symbol = new SgFunctionSymbol(defining_func);
          scope->insert_symbol(name, func_symbol);
          defining_func->set_firstNondefiningDeclaration(NULL);
#else
          printf ("This code may still have to be debugged! \n");

          if (isSgFunctionDeclaration(defining_func) != NULL)
             {
               func_symbol = new SgFunctionSymbol(isSgFunctionDeclaration(defining_func));
               if (isMemberFunction == true)
                  {
                    ROSE_ASSERT(isSgMemberFunctionDeclaration(defining_func) != NULL);
                    func_symbol = new SgMemberFunctionSymbol(isSgMemberFunctionDeclaration(defining_func));
                  }
                 else
                  {
                    ROSE_ASSERT(isSgFunctionDeclaration(defining_func) != NULL);
                    func_symbol = new SgFunctionSymbol(isSgFunctionDeclaration(defining_func));
                  }

               ROSE_ASSERT(func_symbol != NULL);
               ROSE_ASSERT(func_symbol->get_symbol_basis() != NULL);
             }
            else
             {
            // How should we handled template functions in the symbol table???
            // DQ (11/24/2011): After some thought, I think that template declarations for function are more template declarations
            // than functions.  So all template function declarations will be handled as SgTemplateSymbols and not SgFunctionSymbols.
               SgTemplateDeclaration* templatedeclaration = isSgTemplateDeclaration(defining_func);
               ROSE_ASSERT(templatedeclaration != NULL);
               SgTemplateSymbol* template_symbol = new SgTemplateSymbol(templatedeclaration);
               ROSE_ASSERT(template_symbol != NULL);
               ROSE_ASSERT(template_symbol->get_symbol_basis() != NULL);

               func_symbol = template_symbol;
             }

          ROSE_ASSERT(func_symbol != NULL);
          scope->insert_symbol(name, func_symbol);
#if 0
          printf ("In buildDefiningFunctionDeclaration_T(): Setting the defining_func = %p set_firstNondefiningDeclaration(NULL) (to NULL) \n",defining_func);
       // printf ("In buildDefiningFunctionDeclaration_T(): Setting the func = %p set_definingDeclaration(prevDecl->get_definingDeclaration() = %p) (to prevDecl->get_definingDeclaration()) \n",func,prevDecl->get_definingDeclaration());
#endif
          defining_func->set_firstNondefiningDeclaration(NULL);

          printf ("In buildDefiningFunctionDeclaration_T(): func_symbol = %p = %s \n",func_symbol,func_symbol->class_name().c_str());
#endif
#endif
        }
       else
        {
       // We will now build a reference to the non-defining declaration found in the symbol.

       // defining declaration after nondefining declaration
       // reuse function type, function symbol

       // delete func_type;// bug 189

       // Cong (10/25/2010): Make sure in this situation there is no defining declaration for this symbol.
       // ROSE_ASSERT(func_symbol->get_declaration()->get_definingDeclaration() == NULL);
#if 0
          func_type = func_symbol->get_declaration()->get_type();
#else
          SgFunctionSymbol* temp_function_sym = isSgFunctionSymbol(func_symbol);
          SgTemplateSymbol* temp_template_sym = isSgTemplateSymbol(func_symbol);
          if (temp_function_sym != NULL)
             {
               func_type = temp_function_sym->get_declaration()->get_type();

            // firstNondefiningFunctionDeclaration = temp_function_sym->get_declaration()->get_firstNondefiningDeclaration();
            // firstNondefiningFunctionDeclaration = isSgFunctionDeclaration(temp_function_sym->get_declaration()->get_firstNondefiningDeclaration());
            // firstNondefiningFunctionDeclaration = dynamic_cast<SgFunctionDeclaration*>(temp_function_sym->get_declaration()->get_firstNondefiningDeclaration());
               firstNondefiningFunctionDeclaration = temp_function_sym->get_declaration()->get_firstNondefiningDeclaration();
               ROSE_ASSERT(firstNondefiningFunctionDeclaration != NULL);
             }
            else
             {
            // There is no type for a template function declaration.
            // func_type = temp_template_sym->get_declaration()->get_type();
               ROSE_ASSERT(temp_template_sym != NULL);
            // firstNondefiningFunctionDeclaration = temp_template_sym->get_declaration()->get_firstNondefiningDeclaration();
            // firstNondefiningFunctionDeclaration = isSgTemplateFunctionDeclaration(temp_template_sym->get_declaration()->get_firstNondefiningDeclaration());
            // firstNondefiningFunctionDeclaration = dynamic_cast<SgTemplateFunctionDeclaration*>(temp_template_sym->get_declaration()->get_firstNondefiningDeclaration());
               firstNondefiningFunctionDeclaration = temp_template_sym->get_declaration()->get_firstNondefiningDeclaration();
               ROSE_ASSERT(firstNondefiningFunctionDeclaration != NULL);
             }
#endif
#if 0
       // func = new SgFunctionDeclaration(name,func_type,NULL);
          defining_func = new actualFunction(name,func_type,NULL);
          ROSE_ASSERT(defining_func);

       // func->set_firstNondefiningDeclaration(func_symbol->get_declaration()->get_firstNondefiningDeclaration());
          ROSE_ASSERT(firstNondefiningFunctionDeclaration != NULL);
          defining_func->set_firstNondefiningDeclaration(firstNondefiningFunctionDeclaration);

       // fix up defining declarations before current statement
       // func_symbol->get_declaration()->set_definingDeclaration(func);
          firstNondefiningFunctionDeclaration->set_definingDeclaration(defining_func);

       // for the rare case that two or more prototype declaration exist
       // cannot do anything until append/prepend_statment() is invoked
#else
          ROSE_ASSERT(firstNondefiningFunctionDeclaration != NULL);
#endif
        }

  // defining_func = new actualFunction(name,func_type,NULL);
     defining_func = new actualFunction(nameWithTemplateArguments,func_type,NULL);

     ROSE_ASSERT(defining_func != NULL);

#if 0
     printf ("In buildDefiningFunctionDeclaration_T(): constructor called to build func = %p = %s \n",defining_func,defining_func->class_name().c_str());
     if (isSgMemberFunctionDeclaration(defining_func) != NULL)
        {
          printf ("In buildDefiningFunctionDeclaration_T(): SgCtorInitializerList = %p \n",isSgMemberFunctionDeclaration(defining_func)->get_CtorInitializerList());
        }
#endif

#if 0
     printf ("In buildDefiningFunctionDeclaration_T(): Setting the defining_func = %p set_firstNondefiningDeclaration(firstNondefiningFunctionDeclaration = %p) (to valid pointer) \n",defining_func,firstNondefiningFunctionDeclaration);
  // printf ("In buildDefiningFunctionDeclaration_T(): Setting the func = %p set_definingDeclaration(prevDecl->get_definingDeclaration() = %p) (to prevDecl->get_definingDeclaration()) \n",func,prevDecl->get_definingDeclaration());
#endif

  // func->set_firstNondefiningDeclaration(func_symbol->get_declaration()->get_firstNondefiningDeclaration());
     ROSE_ASSERT(firstNondefiningFunctionDeclaration != NULL);
     defining_func->set_firstNondefiningDeclaration(firstNondefiningFunctionDeclaration);

  // fix up defining declarations before current statement
     firstNondefiningFunctionDeclaration->set_definingDeclaration(defining_func);

  // Handle decorators (Python specific)
     if (decoratorList != NULL)
        {
          defining_func->set_decoratorList(decoratorList);
          decoratorList->set_parent(defining_func);
        }

  // definingDeclaration
     defining_func->set_definingDeclaration(defining_func);

  // function body and definition are created before setting argument list
     SgBasicBlock * func_body = new SgBasicBlock();
     ROSE_ASSERT(func_body != NULL);

#if 0
     SgFunctionDefinition* func_def = new SgFunctionDefinition(defining_func,func_body);
#else
     SgFunctionDefinition*          func_def                    = NULL;
     SgTemplateFunctionDeclaration* templateFunctionDeclaration = isSgTemplateFunctionDeclaration(defining_func);

  // Build either a definition for a template or non-template function definition.
  // DQ (2/11/2012): Swapped the order to test templateFunctionDeclaration, since functionDeclaration is always a valid pointer.
  // if (functionDeclaration != NULL)
     if (templateFunctionDeclaration == NULL)
        {
       // DQ (2/11/2012): If we can't assert this then I fear we may have the test in the wrong
       // order (for if test above should be on templateFunctionDeclaration instead).  The new
       // design for templates makes the SgFunctionDeclaration a base class of SgTemplateFunctionDeclaration.
       // Might not make a difference if it is OK to use SgFunctionDefinition interchangibly with
       // SgTemplateFunctionDefinition, but we would never want that.
          ROSE_ASSERT(templateFunctionDeclaration == NULL);

          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(defining_func);
          ROSE_ASSERT(functionDeclaration != NULL);
          func_def = new SgFunctionDefinition(functionDeclaration,func_body);
        }
       else
        {
          ROSE_ASSERT(templateFunctionDeclaration != NULL);
          func_def = new SgTemplateFunctionDefinition(templateFunctionDeclaration,func_body);
        }
#endif
     ROSE_ASSERT(func_def);

  // DQ (11/28/2010): Added specification of case insensitivity (e.g. Fortran).
     if (symbol_table_case_insensitive_semantics == true)
        {
          func_def->setCaseInsensitive(true);
          func_body->setCaseInsensitive(true);
        }

     func_def->set_parent(defining_func);
     func_def->set_body(func_body);
     func_body->set_parent(func_def);

  // parameter list,
  // TODO consider the difference between C++ and Fortran
     setParameterList(defining_func,paralist);
  // fixup the scope and symbol of arguments,
     SgInitializedNamePtrList& argList = paralist->get_args();
     Rose_STL_Container<SgInitializedName*>::iterator argi;
     for (argi = argList.begin(); argi!=argList.end(); argi++)
        {
        // std::cout<<"patching defining function argument's scope and symbol.... "<<std::endl;
          (*argi)->set_scope(func_def);

       // func_def->insert_symbol((*argi)->get_name(), new SgVariableSymbol(*argi) );
          SgVariableSymbol* variableSymbol = new SgVariableSymbol(*argi);
          ROSE_ASSERT(variableSymbol != NULL);
          func_def->insert_symbol((*argi)->get_name(), variableSymbol );

       // DQ (2/13/2016): Adding support for variable length array types in the function parameter list.
          SgArrayType* arrayType = isSgArrayType((*argi)->get_type());
          if (arrayType != NULL)
             {
            // Check if this is a VLA array type, if so look for the index expressions and check
            // if we need to add asociated symbols to the current function definition scope.
               SgExpression* indexExpression = arrayType->get_index();

            // DQ (2/15/2016): This fails for X10 support.
            // ROSE_ASSERT(indexExpression != NULL);
               if (indexExpression != NULL)
                  {
                 // DQ (2/14/2016): Handle the case of an expression tree with any number of variable references.
                 // Get the list of SgVarRef IR nodes and process each one as above.
                 // void collectVarRefs(SgLocatedNode* root, std::vector<SgVarRefExp* >& result);
                    vector<SgVarRefExp* > varRefList;
                    collectVarRefs(indexExpression,varRefList);
#if 0
                    printf ("For array variable: name = %s \n",(*argi)->get_name().str());
                    printf ("varRefList.size() = %zu \n",varRefList.size());
#endif
                    for (size_t i = 0; i < varRefList.size(); i++)
                       {
                      // Process each index subtree's SgVarRefExp.
#if 0
                         printf ("   --- index expression SgVarRefExp: name = %s \n",varRefList[i]->get_symbol()->get_name().str());
#endif
                         SgVariableSymbol* dimension_variableSymbol = varRefList[i]->get_symbol();
                         ROSE_ASSERT(dimension_variableSymbol != NULL);
#if 0
                         printf ("dimension_variableSymbol = %p \n",dimension_variableSymbol);
#endif
                         ROSE_ASSERT(dimension_variableSymbol != variableSymbol);

                      // The symbol from the referenced variable for the array dimension expression shuld already by in the function definition's symbol table.
                         SgSymbol* symbolFromLookup = func_def->lookup_symbol(dimension_variableSymbol->get_name());
                         if (symbolFromLookup != NULL)
                            {
                              SgVariableSymbol* variableSymbolFromLookup = isSgVariableSymbol(symbolFromLookup);
                              ROSE_ASSERT(variableSymbolFromLookup != NULL);

                           // varRefExp->set_symbol(symbolFromLookup);
                              varRefList[i]->set_symbol(variableSymbolFromLookup);
#if 0
                              printf ("Ignoring previously built dimension_variableSymbol: dimension_variableSymbol->get_name() = %s \n",dimension_variableSymbol->get_name().str());
#endif
                           // I think we have a problem if this is not true.
                              ROSE_ASSERT(dimension_variableSymbol != variableSymbol);
                            }
                           else
                            {
                           // This is not a reference to a variable from the current function's paramter lists, so we can ignore processing it within the VLA handling.
                            }
#if 0
                         printf ("Detected an array type in the function parameter list with nontrivial index expression tree \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }
                 else
                  {
                 // In X10 the array index can be more general (fixed to avoid failing X10 tests).
                  }
#if 0
               printf ("Detected an array type in the function parameter list \n");
               ROSE_ASSERT(false);
#endif
             }
        }

     defining_func->set_parent(scope);
     defining_func->set_scope(scope);

  // DQ (12/14/2011): Added test.
     ROSE_ASSERT(defining_func->get_scope() != NULL);

  // DQ (12/15/2011): Added test.
     checkThatNoTemplateInstantiationIsDeclaredInTemplateDefinitionScope(defining_func,scope);

  // set File_Info as transformation generated
     setSourcePositionAtRootAndAllChildren(defining_func);

  // DQ (2/11/2012): Enforce that the return type matches the specification to build a member function.
     if (isMemberFunction == true)
        {
          ROSE_ASSERT(isSgMemberFunctionDeclaration(defining_func) != NULL);
        }

  // DQ (2/11/2012): If this is a template instantiation then we have to set the template name (seperate from the name of the function which can include template parameters)).
  // setTemplateNameInTemplateInstantiations(defining_func,name);
     setTemplateNameInTemplateInstantiations(defining_func,nameWithoutTemplateArguments);

  // DQ (9/16/2012): Setup up the template arguments and the parents of the template arguments.
     if (buildTemplateInstantiation == true)
        {
          setTemplateArgumentsInDeclaration(defining_func,templateArgumentsList);
        }

  // DQ (8/13/2013): Added code to set the template parameters in the defining declaration (if it is a template declaration).
     if (buildTemplateDeclaration == true)
        {
          setTemplateParametersInDeclaration(defining_func,templateParameterList);

       // DQ (8/13/2013): Adding test of template parameter lists.
          SgTemplateFunctionDeclaration* templateFunctionDeclaration = isSgTemplateFunctionDeclaration(defining_func);
          ROSE_ASSERT(templateFunctionDeclaration == NULL || (templateParameterList != NULL && templateParameterList->size() == templateFunctionDeclaration->get_templateParameters().size()));
          SgTemplateMemberFunctionDeclaration* templateMemberFunctionDeclaration = isSgTemplateMemberFunctionDeclaration(defining_func);
          ROSE_ASSERT(templateMemberFunctionDeclaration == NULL || (templateParameterList != NULL && templateParameterList->size() == templateMemberFunctionDeclaration->get_templateParameters().size()));
        }

  // DQ (12/12/2012): Force the two different ways that this can be set to match (we want consistancy).
     if (functionConstVolatileFlags & SgMemberFunctionType::e_restrict)
        {
          defining_func->get_declarationModifier().get_typeModifier().setRestrict();
        }

#if 0
     printf ("In buildDefiningFunctionDeclaration_T(): XXX_name = %s (calling unsetNodesMarkedAsModified()) \n", XXX_name.str());
#endif

  // DQ (4/16/2015): This is replaced with a better implementation.
  // DQ (4/15/2015): We should reset the isModified flags as part of the transforamtion
  // because we have added statements explicitly marked as transformations.
  // checkIsModifiedFlag(defining_func);
     unsetNodesMarkedAsModified(defining_func);

#if 0
  // DQ (4/2/2018): Debugging case of two symbols for the same function in the same namespace (but different namespace definitions).
     if (nameWithoutTemplateArguments == "getline")
        {
          printf ("### Leaving buildDefiningFunctionDeclaration_T(): getline: func: defining_func = %p = %s unparseNameToString() = %s \n",
               defining_func,defining_func->class_name().c_str(),defining_func->unparseNameToString().c_str());
          printf ("   --- scope = %p = %s \n",scope,scope->class_name().c_str());
          SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(scope);
          if (namespaceDefinition != NULL)
             {
               printf ("   --- namespaceDefinition: name = %s \n",namespaceDefinition->get_namespaceDeclaration()->get_name().str());
               printf ("   --- global namespace          = %p \n",namespaceDefinition->get_global_definition());
             }
            else
             {
               SgTemplateInstantiationDefn* templateInstantiationDefn = isSgTemplateInstantiationDefn(scope);
               if (templateInstantiationDefn != NULL)
                  {
                    SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(templateInstantiationDefn->get_declaration());
                    printf ("   --- templateInstantiationDecl: name = %s \n",templateInstantiationDecl->get_name().str());
                  }
                 else
                  {
                    SgTemplateClassDefinition* templateClassDefinition = isSgTemplateClassDefinition(scope);
                    if (templateClassDefinition != NULL)
                       {
                         SgTemplateClassDeclaration* templateClassDeclaration = isSgTemplateClassDeclaration(templateClassDefinition->get_declaration());
                         printf ("   --- templateClassDeclaration: name = %s \n",templateClassDeclaration->get_name().str());

                      // See where this is (because it happens twice in the same class).
                         templateClassDeclaration->get_file_info()->display("getline found in SgTemplateClassDeclaration: debug");
#if 0
                         printf ("Output the symbol tabel used for this template declaration/definition: \n");
                         templateClassDefinition->get_symbol_table()->print("getline found in SgTemplateClassDeclaration");
#endif
                       }
                  }
             }
          printf ("   --- func_symbol = %p = %s \n",func_symbol,func_symbol->class_name().c_str());
        }
#endif

     return defining_func;
   }


void
SageBuilder::setTemplateNameInTemplateInstantiations( SgFunctionDeclaration* func, const SgName & name )
   {
  // DQ (2/11/2012): If this is a template instantiation then we have to set the template name (seperate from the name of the function which can include template parameters)).

#if 0
     printf ("In setTemplateNameInTemplateInstantiations(): name = %s func->get_name() = %s \n",name.str(),func->get_name().str());
#endif

     SgTemplateInstantiationFunctionDecl*       templateInstantiationFunctionDecl       = isSgTemplateInstantiationFunctionDecl(func);
     SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(func);
     bool isTemplateInstantition = (templateInstantiationFunctionDecl != NULL) || (templateInstantiationMemberFunctionDecl != NULL);
     if (isTemplateInstantition == true)
        {
       // If this is a template instantiation then we need to take care of a few more issues.

          SgName templateNameWithoutArguments = name;

#if 1
       // DQ (7/27/2012): New semantics is that we want to have the input name be without template arguments and
       // we will add the template arguments instead of trying to remove then (which was problematic for examples
       // such as "X<Y<Z>> operator X&()" and "X<Y<Z>> operator>()".
#if 0
          if (hasTemplateSyntax(templateNameWithoutArguments) == true)
             {
               printf ("WARNING: new semantics is that the input name has no template syntax. templateNameWithoutArguments = %s \n",templateNameWithoutArguments.str());
            // ROSE_ASSERT(false);
             }
#endif
#else
          XXX SageBuilder::appendTemplateArgumentsToName( const SgName & name, const SgTemplateArgumentPtrList & templateArgumentsList)

       // if (templateNameWithoutArguments.getString().find('<') != string::npos)
          if (hasTemplateSyntax(templateNameWithoutArguments) == true)
             {
               templateNameWithoutArguments = generateTemplateNameFromTemplateNameWithTemplateArguments(name);
             }
            else
             {
               printf ("WARNING: In setTemplateNameInTemplateInstantiations(): name = %s (does not have any template argument syntax) \n",name.str());

            // DQ (7/22/2012): Test exiting where we don't detect template syntax.
               printf ("Exiting as a test \n");
               ROSE_ASSERT(false);
             }
#endif
#if 0
          printf ("In setTemplateNameInTemplateInstantiations(): detected construction of template instantiation func->get_name() = %s \n",func->get_name().str());
          printf ("In setTemplateNameInTemplateInstantiations(): templateNameWithoutArguments            = %s \n",templateNameWithoutArguments.str());
          printf ("In setTemplateNameInTemplateInstantiations(): templateInstantiationFunctionDecl       = %p \n",templateInstantiationFunctionDecl);
          printf ("In setTemplateNameInTemplateInstantiations(): templateInstantiationMemberFunctionDecl = %p \n",templateInstantiationMemberFunctionDecl);
#endif

          bool isMemberFunction = (templateInstantiationMemberFunctionDecl != NULL);
          if (isMemberFunction == true)
             {
               ROSE_ASSERT(templateInstantiationMemberFunctionDecl != NULL);
               ROSE_ASSERT(templateInstantiationFunctionDecl == NULL);

               if (templateInstantiationMemberFunctionDecl->get_templateName().is_null() == true)
                  {
                 // Set the template name for the member function template instantiation.
                 // templateInstantiationMemberFunctionDecl->set_templateName(name);
                    templateInstantiationMemberFunctionDecl->set_templateName(templateNameWithoutArguments);

                 // DQ (5/31/2012): Find locations where this is set and include template syntax.
                 // ROSE_ASSERT(name.getString().find('<') == string::npos);
                 // ROSE_ASSERT(templateNameWithoutArguments.getString().find('<') == string::npos);
                 // ROSE_ASSERT(hasTemplateSyntax(templateNameWithoutArguments) == false);
                  }
#if 0
               printf ("templateInstantiationMemberFunctionDecl->get_templateName() = %s \n",templateInstantiationMemberFunctionDecl->get_templateName().str());
#endif
               ROSE_ASSERT(templateInstantiationMemberFunctionDecl->get_templateName().is_null() == false);
             }
            else
             {
               ROSE_ASSERT(templateInstantiationFunctionDecl != NULL);
               ROSE_ASSERT(templateInstantiationMemberFunctionDecl == NULL);

               if (templateInstantiationFunctionDecl->get_templateName().is_null() == true)
                  {
                 // Set the template name for the function template instantiation.
                 // templateInstantiationFunctionDecl->set_templateName(name);
                    templateInstantiationFunctionDecl->set_templateName(templateNameWithoutArguments);

                 // DQ (5/31/2012): Find locations where this is set and include template syntax.
                 // ROSE_ASSERT(name.getString().find('<') == string::npos);
                 // ROSE_ASSERT(templateNameWithoutArguments.getString().find('<') == string::npos);
                    ROSE_ASSERT(hasTemplateSyntax(templateNameWithoutArguments) == false);
                  }
#if 0
               printf ("templateInstantiationFunctionDecl->get_templateName() = %s \n",templateInstantiationFunctionDecl->get_templateName().str());
#endif
               ROSE_ASSERT(templateInstantiationFunctionDecl->get_templateName().is_null() == false);
             }
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
   }


// SgFunctionDeclaration* SageBuilder::buildDefiningFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, SgFunctionDeclaration* first_nondefining_declaration)
SgFunctionDeclaration*
SageBuilder::buildDefiningFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, SgFunctionDeclaration* first_nondefining_declaration, SgTemplateArgumentPtrList* templateArgumentsList)
   {
  // DQ (2/10/2012): This is not correct, we have to build template instantiations depending on the value of buildTemplateInstantiation.
  // SgFunctionDeclaration* func = buildDefiningFunctionDeclaration_T<SgFunctionDeclaration>(name,return_type,paralist,/* isMemberFunction = */ false,scope,decoratorList);

  // DQ (2/10/2012): Fixed to build either SgTemplateInstantiationFunctionDecl or SgFunctionDeclaration.
     SgFunctionDeclaration* func = NULL;
     if (buildTemplateInstantiation == true)
        {
          SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDecl = isSgTemplateInstantiationFunctionDecl(first_nondefining_declaration);

          ROSE_ASSERT(first_nondefining_declaration != NULL);
#if 0
          printf ("In buildDefiningFunctionDeclaration(): first_nondefining_declaration->get_declarationModifier().isFriend() = %s \n",first_nondefining_declaration->get_declarationModifier().isFriend() ? "true" : "false");
#endif
       // func = buildDefiningFunctionDeclaration_T<SgTemplateInstantiationFunctionDecl>(name,return_type,paralist,/* isMemberFunction = */ false,scope,decoratorList,0,templateInstantiationFunctionDecl);
       // func = buildDefiningFunctionDeclaration_T<SgTemplateInstantiationFunctionDecl>(name,return_type,paralist,/* isMemberFunction = */ false,scope,decoratorList,0,templateInstantiationFunctionDecl, templateArgumentsList);
          func = buildDefiningFunctionDeclaration_T<SgTemplateInstantiationFunctionDecl>(name,return_type,paralist,/* isMemberFunction = */ false,scope,decoratorList,0,templateInstantiationFunctionDecl, templateArgumentsList);

          ROSE_ASSERT(isSgTemplateInstantiationFunctionDecl(func) != NULL);
#if 0
          printf ("In SageBuilder::buildDefiningFunctionDeclaration(): isSgTemplateInstantiationFunctionDecl(func)->get_templateName() = %s \n",isSgTemplateInstantiationFunctionDecl(func)->get_templateName().str());
#endif
          ROSE_ASSERT(isSgTemplateInstantiationFunctionDecl(func) != NULL);
          ROSE_ASSERT(isSgTemplateInstantiationFunctionDecl(func)->get_templateName().is_null() == false);
        }
       else
        {
          ROSE_ASSERT(first_nondefining_declaration != NULL);

       // func = buildDefiningFunctionDeclaration_T<SgFunctionDeclaration>(name,return_type,paralist,/* isMemberFunction = */ false,scope,decoratorList,0,first_nondefining_declaration);
          func = buildDefiningFunctionDeclaration_T<SgFunctionDeclaration>(name,return_type,paralist,/* isMemberFunction = */ false,scope,decoratorList,0,first_nondefining_declaration, NULL);

          ROSE_ASSERT(isSgFunctionDeclaration(func) != NULL);
        }

     return func;
   }


// DQ (8/28/2012): This preserves the original API with a simpler function (however for C++ at least, it is frequently not sufficent).
// We need to decide if the SageBuilder API should include these sorts of functions.
SgFunctionDeclaration*
SageBuilder::buildDefiningFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* parameter_list, SgScopeStatement* scope)
   {
  // DQ (11/12/2012): Note that this function is not used in the AST construction in the EDG/ROSE interface.

  // DQ (8/23/2013): Added assertions.
     ROSE_ASSERT(return_type != NULL);
     ROSE_ASSERT(parameter_list != NULL);

  // DQ (8/23/2013): We need to provide the buildDefiningFunctionDeclaration() function with a pointer to the first non-defining declaration.
  // So we need to find it, and if it does not exist we need to build one so that we have a simple API for building defining declarations.
  // DQ (11/12/2012): Building a defining declaration from scratch now requires a non-defining declaration to exist.
  // SgFunctionDeclaration* nondefininfDeclaration = buildNondefiningFunctionDeclaration(name,return_type,parameter_list,scope,NULL);

     if (scope == NULL)
        {
          scope = SageBuilder::topScopeStack();
        }

     SgFunctionDeclaration* nondefiningDeclaration = NULL;

     SgFunctionType* func_type = buildFunctionType(return_type,parameter_list);
     SgFunctionSymbol* func_symbol = scope->find_symbol_by_type_of_function<SgFunctionDeclaration>(name,func_type,NULL,NULL);
     if (func_symbol != NULL)
        {
          nondefiningDeclaration = func_symbol->get_declaration();
        }
       else
        {
          nondefiningDeclaration = buildNondefiningFunctionDeclaration(name,return_type,parameter_list,scope,NULL);
#if 0
       // DQ (11/20/2013): We should not be appending the nondefiningDeclaration to the scope.  This was added a few months ago.
       // This was a mistake/misunderstanding with Laio about the semantics of the buildDefiningFunctionDeclaration()
       // function.  Building a function should not have a side-effect on the AST (though clearly it can build new
       // subtrees, the AST is not modified until the result of the buildDefiningFunctionDeclaration() is explicitly
       // added (it should also not add the nondefiningDeclaration).  Additionally this code was not consistant with
       // the associated fortran function to build defining function declaration.
          if (scope != NULL)
               appendStatement(nondefiningDeclaration, scope);
#endif
        }

  // DQ (8/23/2013): Added assertions.
     assert(nondefiningDeclaration != NULL);
     assert(nondefiningDeclaration->get_firstNondefiningDeclaration() != NULL);
     assert(nondefiningDeclaration->get_firstNondefiningDeclaration() == nondefiningDeclaration);

  // return buildDefiningFunctionDeclaration (name,return_type,parameter_list,scope,NULL,false,NULL,NULL);
     return buildDefiningFunctionDeclaration (name,return_type,parameter_list,scope,NULL,false,nondefiningDeclaration,NULL);
   }


// DQ (8/28/2012): This preserves the original API with a simpler function (however for C++ at least, it is frequently not sufficent).
// We need to decide if the SageBuilder API should include these sorts of functions.
SgProcedureHeaderStatement*
SageBuilder::buildProcedureHeaderStatement(const SgName& name, SgType* return_type, SgFunctionParameterList* parameter_list, SgProcedureHeaderStatement::subprogram_kind_enum kind, SgScopeStatement* scope)
   {
  // DQ (8/23/2013): Added assertions.
     ROSE_ASSERT(return_type != NULL);
     ROSE_ASSERT(parameter_list != NULL);

  // DQ (8/23/2013): We need to provide the buildDefiningFunctionDeclaration() function with a pointer to the first non-defining declaration.
  // So we need to find it, and if it does not exist we need to build one so that we have a simple API for building defining declarations.
  // DQ (11/12/2012): Building a defining declaration from scratch now requires a non-defining declaration to exist.
  // SgFunctionDeclaration* nondefininfDeclaration = buildNondefiningFunctionDeclaration(name,return_type,parameter_list,scope,NULL);

     if (scope == NULL)
        {
          scope = SageBuilder::topScopeStack();
        }

  // SgProcedureHeaderStatement* nondefiningDeclaration = NULL;
     SgFunctionDeclaration* nondefiningDeclaration = NULL;

     SgFunctionType* func_type = buildFunctionType(return_type,parameter_list);
     SgFunctionSymbol* func_symbol = scope->find_symbol_by_type_of_function<SgProcedureHeaderStatement>(name,func_type,NULL,NULL);
     if (func_symbol != NULL)
        {
          nondefiningDeclaration = func_symbol->get_declaration();
        }
       else
        {
          nondefiningDeclaration = buildNondefiningFunctionDeclaration(name,return_type,parameter_list,scope,NULL);
        }

  // DQ (8/23/2013): Added assertions.
     assert(nondefiningDeclaration != NULL);
     assert(nondefiningDeclaration->get_firstNondefiningDeclaration() != NULL);
     assert(nondefiningDeclaration->get_firstNondefiningDeclaration() == nondefiningDeclaration);

#if 0
  // Function prototype: buildNondefiningFunctionDeclaration_T (
  //      const SgName & name, SgType* return_type, SgFunctionParameterList * paralist, bool isMemberFunction, SgScopeStatement* scope, SgExprListExp* decoratorList,
  //      unsigned int functionConstVolatileFlags, SgTemplateArgumentPtrList* templateArgumentsList, SgTemplateParameterPtrList* templateParameterList);

  // DQ (8/21/2013): Fixed number of parameters in buildNondefiningFunctionDeclaration_T() function call.
  // SgProcedureHeaderStatement* non_def_decl = buildNondefiningFunctionDeclaration_T <SgProcedureHeaderStatement> (name,return_type,parameter_list, /* isMemberFunction = */ false, scope, NULL, false, NULL);
     SgProcedureHeaderStatement* non_def_decl = buildNondefiningFunctionDeclaration_T <SgProcedureHeaderStatement> (name,return_type,parameter_list, /* isMemberFunction = */ false, scope, NULL, 0, NULL, NULL);
#endif

  // return buildProcedureHeaderStatement(name.str(),return_type,parameter_list,kind,scope,NULL);
  // return buildProcedureHeaderStatement(name.str(),return_type,parameter_list,kind,scope, non_def_decl);
     return buildProcedureHeaderStatement(name.str(),return_type,parameter_list,kind,scope, isSgProcedureHeaderStatement(nondefiningDeclaration));
   }


//! Build a Fortran subroutine or procedure
SgProcedureHeaderStatement*
SageBuilder::buildProcedureHeaderStatement( const char* name, SgType* return_type, SgFunctionParameterList * paralist,
                                            SgProcedureHeaderStatement::subprogram_kind_enum kind, SgScopeStatement* scope/*=NULL*/,
                                            SgProcedureHeaderStatement* first_nondefining_declaration)
   {
  // DQ (7/14/2013): We would like to insist that a nondefining declaration has been built at this point.
     ROSE_ASSERT(first_nondefining_declaration != NULL);

  // We will want to call: SageBuilder::buildNondefiningFunctionDeclaration_T (const SgName & XXX_name, SgType* return_type, SgFunctionParameterList * paralist,
  //                                                                           bool isMemberFunction, SgScopeStatement* scope, SgExprListExp* decoratorList,
  //                                                                           unsigned int functionConstVolatileFlags, SgTemplateArgumentPtrList* templateArgumentsList)

     if (kind == SgProcedureHeaderStatement::e_subroutine_subprogram_kind)
        {
          ROSE_ASSERT(return_type == buildVoidType());
        }
       else
        {
          if (kind != SgProcedureHeaderStatement::e_function_subprogram_kind)
             {
               cerr << "unhandled subprogram kind for Fortran function unit:" << kind << endl;
               ROSE_ASSERT(false);
             }
        }

     SgName rose_name = name;
  // SgProcedureHeaderStatement* func = buildDefiningFunctionDeclaration_T<SgProcedureHeaderStatement> (rose_name,return_type,paralist,/* isMemberFunction = */ false,scope,NULL,0U,first_nondefining_declaration);
     SgProcedureHeaderStatement* func = buildDefiningFunctionDeclaration_T<SgProcedureHeaderStatement> (rose_name,return_type,paralist,/* isMemberFunction = */ false,scope,NULL,0U,first_nondefining_declaration, NULL);
     ROSE_ASSERT(func != NULL);

     func->set_subprogram_kind(kind) ;

     return func;
   }

#if 0
// DQ (7/26/2012): I would like to remove these from the API (if possible).
SgFunctionDeclaration*
SageBuilder::buildDefiningFunctionDeclaration(const std::string & name, SgType* return_type, SgFunctionParameterList * paralist,SgScopeStatement* scope, SgExprListExp* decoratorList, SgFunctionDeclaration* first_nondefining_declaration)
   {
     SgName sg_name(name);

     bool buildTemplateInstantiation = false;

  // DQ (5/11/2012): This is a compile time error (use if SgFunctionDeclaration* first_nondefining_declaration results in ambigous function type for g++).
  // return buildDefiningFunctionDeclaration(sg_name,return_type, paralist,scope,decoratorList,first_nondefining_declaration);
  // return buildDefiningFunctionDeclaration(sg_name,return_type, paralist,scope,decoratorList);
     return buildDefiningFunctionDeclaration(sg_name,return_type, paralist,scope,decoratorList,buildTemplateInstantiation,first_nondefining_declaration);
   }
#endif

#if 0
// DQ (7/26/2012): I would like to remove these from the API (if possible).
SgFunctionDeclaration *
SageBuilder::buildDefiningFunctionDeclaration(const char* name, SgType* return_type, SgFunctionParameterList * paralist,SgScopeStatement* scope, SgExprListExp* decoratorList, SgFunctionDeclaration* first_nondefining_declaration)
   {
     SgName sg_name(name);

     bool buildTemplateInstantiation = false;

  // DQ (5/11/2012): This is a compile time error (use if SgFunctionDeclaration* first_nondefining_declaration results in ambigous function type for g++).
  // return buildDefiningFunctionDeclaration(sg_name,return_type, paralist,scope,decoratorList,first_nondefining_declaration);
     return buildDefiningFunctionDeclaration(sg_name,return_type, paralist,scope,decoratorList,buildTemplateInstantiation,first_nondefining_declaration);
   }
#endif


//------------------build value expressions -------------------
//-------------------------------------------------------------
SgBoolValExp* SageBuilder::buildBoolValExp(int value /*=0*/)
{
  //TODO does valueString matter here?
  SgBoolValExp* boolValue= new SgBoolValExp(value);
  ROSE_ASSERT(boolValue);
  setOneSourcePositionForTransformation(boolValue);
  return boolValue;
}
SgBoolValExp* SageBuilder::buildBoolValExp(bool value /*=0*/)
{
  return buildBoolValExp(int(value));
}
SgBoolValExp* SageBuilder::buildBoolValExp_nfi(int value)
{
  SgBoolValExp* boolValue= new SgBoolValExp(value);
  ROSE_ASSERT(boolValue);
  setOneSourcePositionNull(boolValue);
  return boolValue;
}

SgNullptrValExp* SageBuilder::buildNullptrValExp()
   {
     SgNullptrValExp* nullptrValue = new SgNullptrValExp();
     ROSE_ASSERT(nullptrValue);
     setOneSourcePositionForTransformation(nullptrValue);
     return nullptrValue;
   }

SgNullptrValExp* SageBuilder::buildNullptrValExp_nfi()
   {
     SgNullptrValExp* nullptrValue = new SgNullptrValExp();
     ROSE_ASSERT(nullptrValue);
     setOneSourcePositionNull(nullptrValue);
     return nullptrValue;
   }

SgVoidVal* SageBuilder::buildVoidVal()
   {
     SgVoidVal* voidValue = new SgVoidVal();
     ROSE_ASSERT(voidValue);
     setOneSourcePositionForTransformation(voidValue);
     return voidValue;
   }

SgVoidVal* SageBuilder::buildVoidVal_nfi()
   {
     SgVoidVal* voidValue = new SgVoidVal();
     ROSE_ASSERT(voidValue);
     setOneSourcePositionNull(voidValue);
     return voidValue;
   }

SgCharVal* SageBuilder::buildCharVal(char value /*= 0*/)
{
  SgCharVal* result = new SgCharVal(value, "");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgCharVal* SageBuilder::buildCharVal_nfi(char value, const string& str)
{
  SgCharVal* result = new SgCharVal(value, str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgWcharVal* SageBuilder::buildWcharVal(wchar_t value /*= 0*/)
{
  SgWcharVal* result = new SgWcharVal(value, "");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgWcharVal* SageBuilder::buildWcharVal_nfi(wchar_t value, const string& str)
{
  SgWcharVal* result = new SgWcharVal(value, str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

// DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
SgChar16Val* SageBuilder::buildChar16Val(unsigned short value /*= 0*/)
{
  SgChar16Val* result = new SgChar16Val(value, "");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgChar16Val* SageBuilder::buildChar16Val_nfi(unsigned short value, const string& str)
{
  SgChar16Val* result = new SgChar16Val(value, str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

// DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
SgChar32Val* SageBuilder::buildChar32Val(unsigned int value /*= 0*/)
{
  SgChar32Val* result = new SgChar32Val(value, "");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgChar32Val* SageBuilder::buildChar32Val_nfi(unsigned int value, const string& str)
{
  SgChar32Val* result = new SgChar32Val(value, str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}


SgComplexVal* SageBuilder::buildComplexVal(SgValueExp* real_value, SgValueExp* imaginary_value)
{
  SgComplexVal* result = new SgComplexVal(real_value,imaginary_value,imaginary_value->get_type(),"");
  ROSE_ASSERT(result);

// DQ (12/31/2008): set and test the parents
  if (real_value != NULL)
       real_value->set_parent(result);

  if (imaginary_value != NULL)
       imaginary_value->set_parent(result);

  ROSE_ASSERT(real_value == NULL || real_value->get_parent() != NULL);
  ROSE_ASSERT(imaginary_value == NULL || imaginary_value->get_parent() != NULL);

  setOneSourcePositionForTransformation(result);
  return result;
}

SgComplexVal* SageBuilder::buildComplexVal_nfi(SgValueExp* real_value, SgValueExp* imaginary_value, const std::string& str)
{
  SgComplexVal* result = new SgComplexVal(real_value,imaginary_value,imaginary_value->get_type(),str);
  ROSE_ASSERT(result);

// DQ (12/31/2008): set and test the parents
  if (real_value != NULL)
       real_value->set_parent(result);

  if (imaginary_value != NULL)
       imaginary_value->set_parent(result);

  ROSE_ASSERT(real_value == NULL || real_value->get_parent() != NULL);
  ROSE_ASSERT(imaginary_value == NULL || imaginary_value->get_parent() != NULL);

  setOneSourcePositionNull(result);
  return result;
}

SgComplexVal* SageBuilder::buildImaginaryVal(long double imaginary_value /*= 0.0*/ )
{
  SgComplexVal* result = new SgComplexVal(NULL,buildLongDoubleVal(imaginary_value),SgTypeLongDouble::createType(),"");
  ROSE_ASSERT(result);

// DQ (12/31/2008): set and test the parents
  result->get_imaginary_value()->set_parent(result);
  ROSE_ASSERT(result->get_imaginary_value()->get_parent() != NULL);

  setOneSourcePositionForTransformation(result);
  return result;
}

SgComplexVal* SageBuilder::buildImaginaryVal(SgValueExp* imaginary_value)
{
  ROSE_ASSERT(imaginary_value != NULL);

  SgComplexVal* result = new SgComplexVal(NULL,imaginary_value,imaginary_value->get_type(),"");
  ROSE_ASSERT(result);

// DQ (12/31/2008): set and test the parents
  imaginary_value->set_parent(result);
  ROSE_ASSERT(imaginary_value->get_parent() != NULL);

  setOneSourcePositionForTransformation(result);
  return result;
}

SgComplexVal* SageBuilder::buildImaginaryVal_nfi(SgValueExp* imaginary_value, const std::string& str)
{
  ROSE_ASSERT(imaginary_value != NULL);

  SgComplexVal* result = new SgComplexVal(NULL,imaginary_value,imaginary_value->get_type(),str);
  imaginary_value->set_parent(result);
  ROSE_ASSERT(result);

// DQ (12/31/2008): set and test the parents
  ROSE_ASSERT(imaginary_value->get_parent() != NULL);

  setOneSourcePositionNull(result);
  return result;
}

SgDoubleVal* SageBuilder::buildDoubleVal(double t)
{
  SgDoubleVal* value = new SgDoubleVal(t,"");
  ROSE_ASSERT(value);
  setOneSourcePositionForTransformation(value);
  return value;
}

SgDoubleVal* SageBuilder::buildDoubleVal_nfi(double t, const string& str)
{
  SgDoubleVal* value = new SgDoubleVal(t,str);
  ROSE_ASSERT(value);
  setOneSourcePositionNull(value);
  return value;
}

SgFloatVal* SageBuilder::buildFloatVal(float value /*= 0.0*/)
{
  SgFloatVal* result = new SgFloatVal(value,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgFloatVal* SageBuilder::buildFloatVal_nfi(float value, const string& str)
{
  SgFloatVal* result = new SgFloatVal(value,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgIntVal* SageBuilder::buildIntVal(int value)
   {
     SgIntVal* intValue= new SgIntVal(value,"");
     ROSE_ASSERT(intValue);
     setOneSourcePositionForTransformation(intValue);

#if 0
     printf ("In buildIntVal(value = %d): intValue = %p \n",value,intValue);
#endif

     return intValue;
   }

SgIntVal* SageBuilder::buildIntValHex(int value)
   {
     SgIntVal* intValue= new SgIntVal(value, (value >= 0 ? StringUtility::intToHex((unsigned int)value) : "-" + StringUtility::intToHex((unsigned int)(-value))));
     ROSE_ASSERT(intValue);
     setOneSourcePositionForTransformation(intValue);

#if 0
     printf ("In buildIntValHex(value = %d): intValue = %p \n",value,intValue);
#endif

     return intValue;
   }

SgIntVal* SageBuilder::buildIntVal_nfi(int value, const string& str)
   {
     SgIntVal* intValue = new SgIntVal(value,str);
     ROSE_ASSERT(intValue);
     setOneSourcePositionNull(intValue);

#if 0
     printf ("In buildIntVal_nfi(value = %d,str = %s): intValue = %p \n",value,str.c_str(),intValue);
#endif

     return intValue;
   }

SgLongIntVal* SageBuilder::buildLongIntVal(long value)
{
  SgLongIntVal* intValue= new SgLongIntVal(value,"");
  ROSE_ASSERT(intValue);
  setOneSourcePositionForTransformation(intValue);
  return intValue;
}

SgLongIntVal* SageBuilder::buildLongIntVal_nfi(long value, const string& str)
{
  SgLongIntVal* intValue= new SgLongIntVal(value,str);
  ROSE_ASSERT(intValue);
  setOneSourcePositionNull(intValue);
  return intValue;
}

SgLongLongIntVal* SageBuilder::buildLongLongIntVal(long long value)
{
  SgLongLongIntVal* intValue= new SgLongLongIntVal(value,"");
  ROSE_ASSERT(intValue);
  setOneSourcePositionForTransformation(intValue);
  return intValue;
}

SgLongLongIntVal* SageBuilder::buildLongLongIntVal_nfi(long long value, const string& str)
{
  SgLongLongIntVal* intValue= new SgLongLongIntVal(value,str);
  ROSE_ASSERT(intValue);
  setOneSourcePositionNull(intValue);
  return intValue;
}

SgEnumVal* SageBuilder::buildEnumVal(int value, SgEnumDeclaration* decl, SgName name)
   {
     SgEnumVal* enumVal= new SgEnumVal(value,decl,name);
     ROSE_ASSERT(enumVal != NULL);

     setOneSourcePositionForTransformation(enumVal);
     return enumVal;
   }


SgEnumVal* SageBuilder::buildEnumVal_nfi(int value, SgEnumDeclaration* decl, SgName name)
   {
     SgEnumVal* enumVal= new SgEnumVal(value,decl,name);
     ROSE_ASSERT(enumVal != NULL);

     setOneSourcePositionNull(enumVal);

#if 0
  // DQ (6/10/2012): This is moved, but perhaps it really should be here!
     printf ("In buildEnumVal_nfi(): We need to add the enum value to the symbol table in the scope of the SgEnumDeclaration (name = %s) \n",name.str());

  // DQ (6/10/2012): We need to add the enum field value to the correct symbol table.
     SgScopeStatement* scope = decl->get_scope();
     ROSE_ASSERT(scope != NULL);

#error "DEAD CODE!"

     SgEnumFieldSymbol* enumFieldSymbol = scope->lookup_enum_field_symbol(name);
     ROSE_ASSERT(enumFieldSymbol == NULL);
     if (enumFieldSymbol == NULL)
        {
          SgEnumType* enumType = new SgEnumType(decl);
          ROSE_ASSERT(enumType != NULL);

          printf ("In buildEnumVal_nfi(): Building a SgInitializedName for enum field \n");

          SgInitializedName* initializedName = buildInitializedName(name,enumType);
          ROSE_ASSERT(initializedName != NULL);

          initializedName->set_declptr(decl);
          initializedName->set_scope(scope);

          enumFieldSymbol = new SgEnumFieldSymbol(initializedName);
        }

     ROSE_ASSERT(enumFieldSymbol != NULL);

     scope->insert_symbol(name,enumFieldSymbol);
#endif

     return enumVal;
   }

SgEnumVal* SageBuilder::buildEnumVal(SgEnumFieldSymbol * sym) {
  SgInitializedName * init_name = sym->get_declaration();
  assert(init_name != NULL);
  SgAssignInitializer * assign_init = isSgAssignInitializer(init_name->get_initptr());
  assert(assign_init != NULL);
  SgEnumVal * enum_val = isSgEnumVal(assign_init->get_operand_i());
  assert(enum_val != NULL);
  enum_val = isSgEnumVal(SageInterface::copyExpression(enum_val));
  return enum_val;
}

SgLongDoubleVal* SageBuilder::buildLongDoubleVal(long double value /*= 0.0*/)
{
  SgLongDoubleVal* result = new SgLongDoubleVal(value,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgLongDoubleVal* SageBuilder::buildLongDoubleVal_nfi(long double value, const string& str)
{
  SgLongDoubleVal* result = new SgLongDoubleVal(value,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgFloat80Val* SageBuilder::buildFloat80Val(long double value /*= 0.0*/)
{
  SgFloat80Val* result = new SgFloat80Val(value,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgFloat80Val* SageBuilder::buildFloat80Val_nfi(long double value, const string& str)
{
  SgFloat80Val* result = new SgFloat80Val(value,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgFloat128Val* SageBuilder::buildFloat128Val(long double value /*= 0.0*/)
{
  SgFloat128Val* result = new SgFloat128Val(value,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgFloat128Val* SageBuilder::buildFloat128Val_nfi(long double value, const string& str)
{
  SgFloat128Val* result = new SgFloat128Val(value,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgStringVal* SageBuilder::buildStringVal(std::string value /*=""*/)
{
  SgStringVal* result = new SgStringVal(value);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgStringVal* SageBuilder::buildStringVal_nfi(std::string value)
{
  SgStringVal* result = new SgStringVal(value);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgUnsignedCharVal* SageBuilder::buildUnsignedCharVal(unsigned char v)
{
  SgUnsignedCharVal* result = new SgUnsignedCharVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedCharVal* SageBuilder::buildUnsignedCharValHex(unsigned char v)
{
  SgUnsignedCharVal* result = new SgUnsignedCharVal(v,StringUtility::intToHex(v));
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedCharVal* SageBuilder::buildUnsignedCharVal_nfi(unsigned char v, const string& str)
{
  SgUnsignedCharVal* result = new SgUnsignedCharVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgShortVal* SageBuilder::buildShortVal(short v)
{
  SgShortVal* result = new SgShortVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgShortVal* SageBuilder::buildShortValHex(short v)
{
  SgShortVal* result = new SgShortVal(v, (v >= 0 ? StringUtility::intToHex((unsigned int)v) : "-" + StringUtility::intToHex((unsigned int)(-v))));
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgShortVal* SageBuilder::buildShortVal_nfi(short v, const string& str)
{
  SgShortVal* result = new SgShortVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgUnsignedShortVal* SageBuilder::buildUnsignedShortVal(unsigned short v)
{
  SgUnsignedShortVal* result = new SgUnsignedShortVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedShortVal* SageBuilder::buildUnsignedShortValHex(unsigned short v)
{
  SgUnsignedShortVal* result = new SgUnsignedShortVal(v,StringUtility::intToHex(v));
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedShortVal* SageBuilder::buildUnsignedShortVal_nfi(unsigned short v, const string& str)
{
  SgUnsignedShortVal* result = new SgUnsignedShortVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgUnsignedIntVal* SageBuilder::buildUnsignedIntVal(unsigned int v)
{
  SgUnsignedIntVal* result = new SgUnsignedIntVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedIntVal* SageBuilder::buildUnsignedIntValHex(unsigned int v)
{
  SgUnsignedIntVal* result = new SgUnsignedIntVal(v,StringUtility::intToHex(v) + "U");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedIntVal* SageBuilder::buildUnsignedIntVal_nfi(unsigned int v, const string& str)
{
  SgUnsignedIntVal* result = new SgUnsignedIntVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgUnsignedLongVal* SageBuilder::buildUnsignedLongVal(unsigned long v)
{
  SgUnsignedLongVal* result = new SgUnsignedLongVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedLongVal* SageBuilder::buildUnsignedLongValHex(unsigned long v)
{
  SgUnsignedLongVal* result = new SgUnsignedLongVal(v,StringUtility::intToHex(v) + "UL");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedLongVal* SageBuilder::buildUnsignedLongVal_nfi(unsigned long v, const string& str)
{
  SgUnsignedLongVal* result = new SgUnsignedLongVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgUnsignedLongLongIntVal* SageBuilder::buildUnsignedLongLongIntVal(unsigned long long v)
{
  SgUnsignedLongLongIntVal* result = new SgUnsignedLongLongIntVal(v,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedLongLongIntVal* SageBuilder::buildUnsignedLongLongIntValHex(unsigned long long v)
{
  SgUnsignedLongLongIntVal* result = new SgUnsignedLongLongIntVal(v,StringUtility::intToHex(v) + "ULL");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgUnsignedLongLongIntVal* SageBuilder::buildUnsignedLongLongIntVal_nfi(unsigned long long v, const string& str)
{
  SgUnsignedLongLongIntVal* result = new SgUnsignedLongLongIntVal(v,str);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgTemplateType* SageBuilder::buildTemplateType(SgName name/* ="" */)
{
  SgTemplateType* result = new SgTemplateType (name);
  ROSE_ASSERT (result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgTemplateParameter * SageBuilder::buildTemplateParameter (SgTemplateParameter::template_parameter_enum parameterType, SgType* t)
{
  ROSE_ASSERT (t);
  SgTemplateParameter* result = new SgTemplateParameter(parameterType, t);
  ROSE_ASSERT (result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgTemplateParameterVal* SageBuilder::buildTemplateParameterVal(int template_parameter_position)
{
  SgTemplateParameterVal* templateParameterValue = new SgTemplateParameterVal(template_parameter_position,"");
  ROSE_ASSERT(templateParameterValue);
  setOneSourcePositionForTransformation(templateParameterValue);

// DQ (7/25/2012): Assert this (it will be set later).
  ROSE_ASSERT(templateParameterValue->get_parent() == NULL);

  return templateParameterValue;
}

SgTemplateParameterVal* SageBuilder::buildTemplateParameterVal_nfi(int template_parameter_position, const string& str)
{
  SgTemplateParameterVal* templateParameterValue= new SgTemplateParameterVal(template_parameter_position,str);
  ROSE_ASSERT(templateParameterValue);
  setOneSourcePositionNull(templateParameterValue);

// DQ (7/25/2012): Assert this (it will be set later).
  ROSE_ASSERT(templateParameterValue->get_parent() == NULL);

  return templateParameterValue;
}

#define DEBUG_BUILD_NONREAL_DECL 0

SgNonrealDecl * SageBuilder::buildNonrealDecl(const SgName & name, SgDeclarationScope * scope, SgDeclarationScope * child_scope) {
  ROSE_ASSERT(scope != NULL);
#if DEBUG_BUILD_NONREAL_DECL
  printf("ENTER SageBuilder::buildNonrealDecl\n");
  printf("  --- name = %s\n", name.str());
  printf("  --- scope = %p (%s)\n", scope, scope->class_name().c_str());
#endif

  SgNonrealDecl * nrdecl = NULL;

  nrdecl = new SgNonrealDecl(name);
  SageInterface::setSourcePosition(nrdecl);
  nrdecl->set_firstNondefiningDeclaration(nrdecl);
#if DEBUG_BUILD_NONREAL_DECL
  printf("  --- nrdecl = %p (%s)\n", nrdecl, nrdecl->class_name().c_str());
#endif

  SgNonrealSymbol * symbol = new SgNonrealSymbol(nrdecl);
  scope->insert_symbol(name, symbol);
#if DEBUG_BUILD_NONREAL_DECL
  printf("  --- symbol = %p (%s)\n", symbol, symbol->class_name().c_str());
#endif

  SgNonrealType * type = new SgNonrealType();
  type->set_declaration(nrdecl);
  type->set_parent(scope);
  nrdecl->set_type(type);
  // FIXME (???) insert `type` in `scope`
#if DEBUG_BUILD_NONREAL_DECL
  printf("  --- type = %p (%s)\n", type, type->class_name().c_str());
#endif

  if (child_scope == NULL) {
    child_scope = new SgDeclarationScope();
#if DEBUG_BUILD_NONREAL_DECL
  printf("  --- child_scope = %p (new)\n", name.str(), child_scope);
#endif
    SageInterface::setSourcePosition(child_scope);
    child_scope->get_startOfConstruct()->setCompilerGenerated();
    child_scope->get_endOfConstruct()->setCompilerGenerated();
  } else {
#if DEBUG_BUILD_NONREAL_DECL
  printf("  --- child_scope = %p (provided)\n", name.str(), child_scope);
#endif

  }
  child_scope->set_parent(nrdecl);
  nrdecl->set_nonreal_decl_scope(child_scope);

#if DEBUG_BUILD_NONREAL_DECL
  printf("LEAVE SageBuilder::buildNonrealDecl\n");
#endif

  return nrdecl;
}

//! Build UPC THREADS (integer expression)
SgUpcThreads* SageBuilder::buildUpcThreads()
{
  SgUpcThreads* result = new SgUpcThreads(0,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build UPC THREADS (integer expression)
SgUpcThreads* SageBuilder::buildUpcThreads_nfi()
{
  SgUpcThreads* result = new SgUpcThreads(0,"");
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

//! Build UPC  MYTHREAD (integer expression)
SgUpcMythread* SageBuilder::buildUpcMythread()
{
  SgUpcMythread* result = new SgUpcMythread(0,"");
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build UPC  MYTHREAD (integer expression)
SgUpcMythread* SageBuilder::buildUpcMythread_nfi()
{
  SgUpcMythread* result = new SgUpcMythread(0,"");
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgThisExp* SageBuilder::buildThisExp(SgSymbol* sym)
{
  SgThisExp* result = new SgThisExp(isSgClassSymbol(sym), isSgNonrealSymbol(sym), 0);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgThisExp* SageBuilder::buildThisExp_nfi(SgSymbol* sym)
{
  SgThisExp* result = new SgThisExp(isSgClassSymbol(sym), isSgNonrealSymbol(sym), 0);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgSuperExp* SageBuilder::buildSuperExp(SgClassSymbol* sym)
{
  SgSuperExp* result = new SgSuperExp(sym, 0);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgSuperExp* SageBuilder::buildSuperExp_nfi(SgClassSymbol* sym)
{
  SgSuperExp* result = new SgSuperExp(sym, 0);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgClassExp* SageBuilder::buildClassExp(SgClassSymbol* sym)
{
  SgClassExp* result = new SgClassExp(sym, 0);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgClassExp* SageBuilder::buildClassExp_nfi(SgClassSymbol* sym)
{
  SgClassExp* result = new SgClassExp(sym, 0);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgTupleExp*
SageBuilder::buildTupleExp(SgExpression * elt1, SgExpression* elt2, SgExpression* elt3, SgExpression* elt4, SgExpression* elt5, SgExpression* elt6, SgExpression* elt7, SgExpression* elt8, SgExpression* elt9, SgExpression* elt10)
{
  SgTupleExp* tuple = new SgTupleExp();
  ROSE_ASSERT(tuple);
  if (elt1) appendExpression(tuple, elt1);
  if (elt2) appendExpression(tuple, elt2);
  if (elt3) appendExpression(tuple, elt3);
  if (elt4) appendExpression(tuple, elt4);
  if (elt5) appendExpression(tuple, elt5);
  if (elt6) appendExpression(tuple, elt6);
  if (elt7) appendExpression(tuple, elt7);
  if (elt8) appendExpression(tuple, elt8);
  if (elt9) appendExpression(tuple, elt9);
  if (elt10) appendExpression(tuple, elt10);

  setOneSourcePositionForTransformation(tuple);
  return tuple;
}

SgTupleExp*
SageBuilder::buildTupleExp(const std::vector<SgExpression*>& elts)
{
  SgTupleExp* expList = SageBuilder::buildTupleExp();
  appendExpressionList(expList, elts);
  return expList;
}

SgTupleExp*
SageBuilder::buildTupleExp_nfi()
{
  SgTupleExp* tuple = new SgTupleExp();
  ROSE_ASSERT(tuple);
  setOneSourcePositionNull(tuple);
  return tuple;
}

SgTupleExp*
SageBuilder::buildTupleExp_nfi(const std::vector<SgExpression*>& elts)
{
  SgTupleExp* tuple = SageBuilder::buildTupleExp_nfi();
  appendExpressionList(tuple, elts);
  return tuple;
}

SgListExp*
SageBuilder::buildListExp(SgExpression * elt1, SgExpression* elt2, SgExpression* elt3, SgExpression* elt4, SgExpression* elt5, SgExpression* elt6, SgExpression* elt7, SgExpression* elt8, SgExpression* elt9, SgExpression* elt10)
{
  SgListExp* tuple = new SgListExp();
  ROSE_ASSERT(tuple);
  if (elt1) appendExpression(tuple, elt1);
  if (elt2) appendExpression(tuple, elt2);
  if (elt3) appendExpression(tuple, elt3);
  if (elt4) appendExpression(tuple, elt4);
  if (elt5) appendExpression(tuple, elt5);
  if (elt6) appendExpression(tuple, elt6);
  if (elt7) appendExpression(tuple, elt7);
  if (elt8) appendExpression(tuple, elt8);
  if (elt9) appendExpression(tuple, elt9);
  if (elt10) appendExpression(tuple, elt10);

  setOneSourcePositionForTransformation(tuple);
  return tuple;
}

SgListExp*
SageBuilder::buildListExp(const std::vector<SgExpression*>& elts)
{
  SgListExp* expList = SageBuilder::buildListExp();
  appendExpressionList(expList, elts);
  return expList;
}

SgListExp*
SageBuilder::buildListExp_nfi()
{
  SgListExp* tuple = new SgListExp();
  ROSE_ASSERT(tuple);
  setOneSourcePositionNull(tuple);
  return tuple;
}

SgListExp*
SageBuilder::buildListExp_nfi(const std::vector<SgExpression*>& elts)
{
  SgListExp* tuple = SageBuilder::buildListExp_nfi();
  appendExpressionList(tuple, elts);
  return tuple;
}


#define BUILD_UNARY_DEF(suffix) \
  ROSE_DLL_API Sg##suffix* SageBuilder::build##suffix##_nfi(SgExpression* op) \
  { \
     return SageBuilder::buildUnaryExpression_nfi<Sg##suffix>(op); \
  } \
  ROSE_DLL_API Sg##suffix* SageBuilder::build##suffix(SgExpression* op) \
  { \
     return SageBuilder::buildUnaryExpression<Sg##suffix>(op); \
  }

BUILD_UNARY_DEF(AddressOfOp)
BUILD_UNARY_DEF(BitComplementOp)
BUILD_UNARY_DEF(MinusOp)
BUILD_UNARY_DEF(NotOp)
BUILD_UNARY_DEF(PointerDerefExp)
BUILD_UNARY_DEF(UnaryAddOp)
BUILD_UNARY_DEF(MinusMinusOp)
BUILD_UNARY_DEF(PlusPlusOp)
BUILD_UNARY_DEF(RealPartOp)
BUILD_UNARY_DEF(ImagPartOp)
BUILD_UNARY_DEF(ConjugateOp)
BUILD_UNARY_DEF(VarArgStartOneOperandOp)
BUILD_UNARY_DEF(VarArgEndOp)

BUILD_UNARY_DEF(MatrixTransposeOp) //SK(08/20/2015): Matlab matrix transpose operators

#undef BUILD_UNARY_DEF

SgCastExp * SageBuilder::buildCastExp(SgExpression *  operand_i,
                SgType * expression_type,
                SgCastExp::cast_type_enum cast_type)
{
  SgCastExp* result = new SgCastExp(operand_i, expression_type, cast_type);
  ROSE_ASSERT(result);
  if (operand_i) {operand_i->set_parent(result); markLhsValues(result);}
  setOneSourcePositionForTransformation(result);
  return result;
}

SgNewExp*
SageBuilder::buildNewExp ( SgType* specified_type,
                           SgExprListExp* placement_args,
                           SgConstructorInitializer* constructor_args,
                           SgExpression* builtin_args,
                        // FIXME: Change this from "short int" to "bool".
                           short int need_global_specifier,
                           SgFunctionDeclaration* newOperatorDeclaration)
   {
  // DQ (11/18/2012): Modified parameter names to make this function more clear.
     SgNewExp* result = new SgNewExp(specified_type, placement_args, constructor_args, builtin_args, need_global_specifier, newOperatorDeclaration);
     ROSE_ASSERT(result);

     setOneSourcePositionForTransformation(result);
     return result;
   }

SgDeleteExp* SageBuilder::buildDeleteExp(SgExpression* variable,
                                         short is_array,
                                         short need_global_specifier,
                                         SgFunctionDeclaration* deleteOperatorDeclaration)
{
  SgDeleteExp* result = new SgDeleteExp(variable, is_array,
          need_global_specifier, deleteOperatorDeclaration);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgTypeIdOp*
SageBuilder::buildTypeIdOp(SgExpression *operand_expr, SgType *operand_type)
   {
  // DQ (1/25/2013): Added support for typeId operators.
     SgTypeIdOp* result = new SgTypeIdOp(operand_expr,operand_type);
     ROSE_ASSERT(result != NULL);
     setOneSourcePositionForTransformation(result);
     return result;
   }

SgCastExp * SageBuilder::buildCastExp_nfi(SgExpression *  operand_i, SgType * expression_type, SgCastExp::cast_type_enum cast_type)
{
  SgCastExp* result = new SgCastExp(operand_i, expression_type, cast_type);
  ROSE_ASSERT(result);
  if (operand_i) {operand_i->set_parent(result); markLhsValues(result);}
  setOneSourcePositionNull(result);
  return result;
}

SgVarArgOp * SageBuilder::buildVarArgOp_nfi(SgExpression *  operand_i, SgType * expression_type) {
  SgVarArgOp* result = new SgVarArgOp(operand_i, expression_type);
  ROSE_ASSERT(result);
  if (operand_i) {operand_i->set_parent(result); markLhsValues(result);}
  setOneSourcePositionNull(result);
  return result;
}

SgMinusMinusOp *SageBuilder::buildMinusMinusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgMinusMinusOp* result = buildUnaryExpression<SgMinusMinusOp>(operand_i);
  ROSE_ASSERT(result);
  result->set_mode(a_mode);
  return result;
}

SgMinusMinusOp *SageBuilder::buildMinusMinusOp_nfi(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgMinusMinusOp* result = buildUnaryExpression_nfi<SgMinusMinusOp>(operand_i);
  ROSE_ASSERT(result);
  result->set_mode(a_mode);
  return result;
}

SgMinusOp *SageBuilder::buildMinusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgMinusOp* result = buildUnaryExpression<SgMinusOp>(operand_i);
  ROSE_ASSERT(result);
  result->set_mode(a_mode);
  return result;
}

SgMinusOp *SageBuilder::buildMinusOp_nfi(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgMinusOp* result = buildUnaryExpression_nfi<SgMinusOp>(operand_i);
  ROSE_ASSERT(result);
  result->set_mode(a_mode);
  return result;
}

SgPlusPlusOp *SageBuilder::buildPlusPlusOp(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgPlusPlusOp* result = buildUnaryExpression<SgPlusPlusOp>(operand_i);
  ROSE_ASSERT(result);
  result->set_mode(a_mode);
  return result;
}


SgPlusPlusOp *SageBuilder::buildPlusPlusOp_nfi(SgExpression* operand_i, SgUnaryOp::Sgop_mode  a_mode)
{
  SgPlusPlusOp* result = buildUnaryExpression_nfi<SgPlusPlusOp>(operand_i);
  ROSE_ASSERT(result);
  result->set_mode(a_mode);
  return result;
}

SgThrowOp *SageBuilder::buildThrowOp(SgExpression *operand_i, SgThrowOp::e_throw_kind throwKind)
   {
  // DQ (11/8/2011): operand_i is allowed to be NULL.

  // SgThrowOp* result = new SgThrowOp(operand_i, operand_i -> get_type(), throwKind);
     SgThrowOp* result = new SgThrowOp(operand_i, (operand_i != NULL) ? operand_i->get_type() : NULL, throwKind);

     if (operand_i != NULL)
        {
          markLhsValues(result);
        }

     setOneSourcePositionForTransformation(result);

     if (operand_i != NULL)
          operand_i -> set_parent(result);

     ROSE_ASSERT(result);

     return result;
   }



#define BUILD_BINARY_DEF(suffix) \
  ROSE_DLL_API Sg##suffix* SageBuilder::build##suffix##_nfi(SgExpression* lhs, SgExpression* rhs) \
  { \
     return buildBinaryExpression_nfi<Sg##suffix>(lhs, rhs); \
  } \
  ROSE_DLL_API Sg##suffix* SageBuilder::build##suffix(SgExpression* lhs, SgExpression* rhs) \
  { \
     return buildBinaryExpression<Sg##suffix>(lhs, rhs); \
  }

BUILD_BINARY_DEF(AddOp)
BUILD_BINARY_DEF(AndAssignOp)
BUILD_BINARY_DEF(AndOp)
BUILD_BINARY_DEF(ArrowExp)
BUILD_BINARY_DEF(ArrowStarOp)
BUILD_BINARY_DEF(AssignOp)
BUILD_BINARY_DEF(BitAndOp)
BUILD_BINARY_DEF(BitOrOp)
BUILD_BINARY_DEF(BitXorOp)

BUILD_BINARY_DEF(CommaOpExp)
BUILD_BINARY_DEF(ConcatenationOp)
BUILD_BINARY_DEF(DivAssignOp)
BUILD_BINARY_DEF(DivideOp)
BUILD_BINARY_DEF(DotExp)
BUILD_BINARY_DEF(DotStarOp)
BUILD_BINARY_DEF(EqualityOp)

BUILD_BINARY_DEF(ExponentiationOp)
BUILD_BINARY_DEF(ExponentiationAssignOp)
BUILD_BINARY_DEF(GreaterOrEqualOp)
BUILD_BINARY_DEF(GreaterThanOp)
BUILD_BINARY_DEF(IntegerDivideOp)
BUILD_BINARY_DEF(IntegerDivideAssignOp)
BUILD_BINARY_DEF(IorAssignOp)
BUILD_BINARY_DEF(IsOp)
BUILD_BINARY_DEF(IsNotOp)

BUILD_BINARY_DEF(LessOrEqualOp)
BUILD_BINARY_DEF(LessThanOp)
BUILD_BINARY_DEF(LshiftAssignOp)
BUILD_BINARY_DEF(LshiftOp)

BUILD_BINARY_DEF(MembershipOp)
BUILD_BINARY_DEF(MinusAssignOp)
BUILD_BINARY_DEF(ModAssignOp)
BUILD_BINARY_DEF(ModOp)
BUILD_BINARY_DEF(MultAssignOp)
BUILD_BINARY_DEF(MultiplyOp)

BUILD_BINARY_DEF(NotEqualOp)
BUILD_BINARY_DEF(NonMembershipOp)
BUILD_BINARY_DEF(OrOp)
BUILD_BINARY_DEF(PlusAssignOp)
BUILD_BINARY_DEF(PntrArrRefExp)
BUILD_BINARY_DEF(RshiftAssignOp)
BUILD_BINARY_DEF(JavaUnsignedRshiftAssignOp)

BUILD_BINARY_DEF(RshiftOp)
BUILD_BINARY_DEF(JavaUnsignedRshiftOp)
BUILD_BINARY_DEF(ScopeOp)
BUILD_BINARY_DEF(SubtractOp)
BUILD_BINARY_DEF(XorAssignOp)

BUILD_BINARY_DEF(VarArgCopyOp)
BUILD_BINARY_DEF(VarArgStartOp)

//SK(08/20/2015): Matlab operators
BUILD_BINARY_DEF(PowerOp);
BUILD_BINARY_DEF(ElementwisePowerOp);
BUILD_BINARY_DEF(ElementwiseMultiplyOp);
BUILD_BINARY_DEF(ElementwiseDivideOp);
BUILD_BINARY_DEF(LeftDivideOp);
BUILD_BINARY_DEF(ElementwiseLeftDivideOp);
BUILD_BINARY_DEF(ElementwiseAddOp);
BUILD_BINARY_DEF(ElementwiseSubtractOp);

#undef BUILD_BINARY_DEF



// Rasmussen ( 1/25/2018):
//           (10/30/2018): Fixed case when this function is called with NULL dim_info object.
SgArrayType* SageBuilder::buildArrayType(SgType* base_type, SgExprListExp* dim_info)
   {
     ROSE_ASSERT(base_type != NULL);

  // There must always be a dim_info object for this function.  If not, the
  // overloaded function must be used to handle it.
     if (dim_info == NULL)
        {
           SgExpression* index = NULL;
           return buildArrayType(base_type, index);
        }

     SgExpression* index = new SgNullExpression();
     ROSE_ASSERT(index);
     setSourcePosition(index);

     SgArrayType* array_type = new SgArrayType(base_type, index);
     ROSE_ASSERT(array_type);
     ROSE_ASSERT(array_type->get_dim_info() == NULL);

     index   ->set_parent(array_type);
     dim_info->set_parent(array_type);

     array_type->set_dim_info(dim_info);
     array_type->set_rank(dim_info->get_expressions().size());

     return array_type;
   }

SgArrayType* SageBuilder::buildArrayType(SgType* base_type/*=NULL*/, SgExpression* index/*=NULL*/)
   {
     SgArrayType* result = new SgArrayType(base_type,index);
     ROSE_ASSERT(result);

     if (index != NULL)
          index->set_parent(result); // important!

#if 0
     printf ("In SageBuilder::buildArrayType(): result = %p base_type = %p = %s index = %p = %s \n",result,base_type,base_type->class_name().c_str(),index,index->class_name().c_str());
#endif

     return result;
   }

SgConditionalExp* SageBuilder::buildConditionalExp(SgExpression* test, SgExpression* a, SgExpression* b)
{
  SgConditionalExp* result = new SgConditionalExp(test, a, b, NULL);
  if (test) test->set_parent(result);
  if (a) a->set_parent(result);
  if (b) b->set_parent(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgConditionalExp* SageBuilder::buildConditionalExp_nfi(SgExpression* test, SgExpression* a, SgExpression* b, SgType* t)
{
  SgConditionalExp* result = new SgConditionalExp(test, a, b, t);
  if (test) test->set_parent(result);
  if (a) {a->set_parent(result); markLhsValues(a);}
  if (b) {b->set_parent(result); markLhsValues(b);}
  setOneSourcePositionNull(result);
  return result;
}
SgVariantExpression * SageBuilder::buildVariantExpression()
{
  SgVariantExpression * result =  new SgVariantExpression();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgNullExpression* SageBuilder::buildNullExpression_nfi()
{
  SgNullExpression* ne = new SgNullExpression();
  ROSE_ASSERT(ne);
  setOneSourcePositionNull(ne);
  return ne;
}

SgNullExpression* SageBuilder::buildNullExpression() {
  SgNullExpression* e = buildNullExpression_nfi();
  setOneSourcePositionForTransformation(e);
  return e;
}

SgAssignInitializer * SageBuilder::buildAssignInitializer(SgExpression * operand_i /*= NULL*/, SgType * expression_type /* = NULL */)
   {
     SgAssignInitializer* result = new SgAssignInitializer(operand_i, expression_type);
     ROSE_ASSERT(result);
     if (operand_i!=NULL)
        {
          operand_i->set_parent(result);
       // set lvalue, it asserts operand!=NULL
          markLhsValues(result);
        }
     setOneSourcePositionForTransformation(result);
     return result;
   }

SgAssignInitializer * SageBuilder::buildAssignInitializer_nfi(SgExpression * operand_i /*= NULL*/, SgType * expression_type /* = UNLL */)
   {
     SgAssignInitializer* result = new SgAssignInitializer(operand_i, expression_type);
     ROSE_ASSERT(result);
     if (operand_i!=NULL)
        {
          operand_i->set_parent(result);
       // set lvalue, it asserts operand!=NULL
          markLhsValues(result);
        }

  // DQ (11/2/2012): Set the source positon using our standard approach.
  // result->set_startOfConstruct(NULL);
  // result->set_endOfConstruct(NULL);
  // result->set_operatorPosition(NULL);
     setSourcePosition(result);

     result->set_need_paren(false);

     return result;
   }

//! Build an aggregate initializer
SgAggregateInitializer * SageBuilder::buildAggregateInitializer(SgExprListExp * initializers/* = NULL*/, SgType *type/* = NULL */)
{
  SgAggregateInitializer* result = new SgAggregateInitializer(initializers, type);
  ROSE_ASSERT(result);
  if (initializers!=NULL)
  {
    initializers->set_parent(result);
  }
  result->set_need_explicit_braces(true);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build an aggregate initializer
SgAggregateInitializer * SageBuilder::buildAggregateInitializer_nfi(SgExprListExp * initializers/* = NULL*/, SgType *type/* = NULL */)
{
  SgAggregateInitializer* result = new SgAggregateInitializer(initializers, type);
  ROSE_ASSERT(result);
  if (initializers!=NULL)
  {
    initializers->set_parent(result);
  }
  result->set_need_explicit_braces(true);
  setOneSourcePositionNull(result);
  return result;
}

//! Build a compound initializer, for vector type initialization
SgCompoundInitializer * SageBuilder::buildCompoundInitializer(SgExprListExp * initializers/* = NULL*/, SgType *type/* = NULL */)
{
  SgCompoundInitializer* result = new SgCompoundInitializer(initializers, type);
  ROSE_ASSERT(result);
  if (initializers!=NULL)
  {
    initializers->set_parent(result);
  }
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build a compound initializer, for vector type initialization
SgCompoundInitializer * SageBuilder::buildCompoundInitializer_nfi(SgExprListExp * initializers/* = NULL*/, SgType *type/* = NULL */)
{
  SgCompoundInitializer* result = new SgCompoundInitializer(initializers, type);
  ROSE_ASSERT(result);
  if (initializers!=NULL)
  {
    initializers->set_parent(result);
  }
  setOneSourcePositionNull(result);
  return result;
}

// DQ (1/4/2009): Added support for SgConstructorInitializer
SgConstructorInitializer *
SageBuilder::buildConstructorInitializer(
   SgMemberFunctionDeclaration *declaration/* = NULL*/,
   SgExprListExp *args/* = NULL*/,
   SgType *expression_type/* = NULL*/,
   bool need_name /*= false*/,
   bool need_qualifier /*= false*/,
   bool need_parenthesis_after_name /*= false*/,
   bool associated_class_unknown /*= false*/)
   {
  // Prototype:
  // SgConstructorInitializer (SgMemberFunctionDeclaration *declaration, SgExprListExp *args, SgType *expression_type,
  //    bool need_name, bool need_qualifier, bool need_parenthesis_after_name, bool associated_class_unknown);

     //George Vulov (05/24/2011) Modified this assertion to allow for a NULL declaration (in case of implicit constructors)
     ROSE_ASSERT(declaration == NULL || declaration->get_associatedClassDeclaration() != NULL);

     SgConstructorInitializer* result = new SgConstructorInitializer( declaration, args, expression_type, need_name,
                                        need_qualifier, need_parenthesis_after_name, associated_class_unknown );
     ROSE_ASSERT(result != NULL);
     if (args != NULL)
        {
          args->set_parent(result);
          setOneSourcePositionForTransformation(args);
        }

     setOneSourcePositionForTransformation(result);

     return result;
   }

// DQ (1/4/2009): Added support for SgConstructorInitializer
SgConstructorInitializer *
SageBuilder::buildConstructorInitializer_nfi(
   SgMemberFunctionDeclaration *declaration/* = NULL*/,
   SgExprListExp *args/* = NULL*/,
   SgType *expression_type/* = NULL*/,
   bool need_name /*= false*/,
   bool need_qualifier /*= false*/,
   bool need_parenthesis_after_name /*= false*/,
   bool associated_class_unknown /*= false*/)
   {
  // Prototype:
  // SgConstructorInitializer (SgMemberFunctionDeclaration *declaration, SgExprListExp *args, SgType *expression_type, bool need_name, bool need_qualifier, bool need_parenthesis_after_name, bool associated_class_unknown);

  // DQ (11/7/2011): Added additional error checking.
  // ROSE_ASSERT(declaration != NULL);
  // DQ (1/4/2009): Error checking
  // ROSE_ASSERT(declaration->get_associatedClassDeclaration() != NULL);

  // DQ (11/7/2011): Fix symetric to the way George did it above.
     ROSE_ASSERT(declaration == NULL || declaration->get_associatedClassDeclaration() != NULL);

     SgConstructorInitializer* result = new SgConstructorInitializer( declaration, args, expression_type, need_name, need_qualifier, need_parenthesis_after_name, associated_class_unknown );
     ROSE_ASSERT(result != NULL);

     setOneSourcePositionNull(result);

     if (args != NULL)
        {
          args->set_parent(result);
        }

  // DQ (11/4/2012): This is required and appears to work fine now.
  // DQ (11/23/2011): Fixup the expression list (but this does not appear to work...)
     if (result->get_args()->get_startOfConstruct() == NULL)
       {
#if 0
         printf ("In buildConstructorInitializer_nfi(): Fixup the source position of result->get_args() \n");
#endif
         setOneSourcePositionNull(result->get_args());
       }

     return result;
   }

// DQ (11/15/2016):Adding support for braced initializer (required for template support).
//! Build an braced initializer
SgBracedInitializer*
SageBuilder::buildBracedInitializer(SgExprListExp * initializers, SgType * expression_type )
   {
     SgBracedInitializer* result = new SgBracedInitializer(initializers, expression_type);
     ROSE_ASSERT(result);
     if (initializers!=NULL)
        {
          initializers->set_parent(result);
        }
     setOneSourcePositionForTransformation(result);
     return result;
   }

SgBracedInitializer* SageBuilder::buildBracedInitializer_nfi(SgExprListExp * initializers, SgType * expression_type )
   {
     SgBracedInitializer* result = new SgBracedInitializer(initializers, expression_type);
     ROSE_ASSERT(result);
     if (initializers!=NULL)
        {
          initializers->set_parent(result);
        }
     setOneSourcePositionNull(result);
     return result;
   }


//! Build sizeof() expression with an expression parameter
SgSizeOfOp* SageBuilder::buildSizeOfOp(SgExpression* exp/*= NULL*/)
   {
  // SgType* exp_type = NULL;
  // if (exp) exp_type = exp->get_type();

     SgSizeOfOp* result = new SgSizeOfOp(exp,NULL, NULL);
  // SgSizeOfOp* result = new SgSizeOfOp(exp,NULL, exp_type);
     ROSE_ASSERT(result);
     if (exp)
        {
          exp->set_parent(result);
          markLhsValues(result);
        }
     setOneSourcePositionForTransformation(result);
     return result;
   }

//! Build sizeof() expression with an expression parameter
SgSizeOfOp* SageBuilder::buildSizeOfOp_nfi(SgExpression* exp/*= NULL*/)
   {
  // SgType* exp_type =NULL;
  // if (exp) exp_type = exp->get_type();

     SgSizeOfOp* result = new SgSizeOfOp(exp,NULL, NULL);
  // SgSizeOfOp* result = new SgSizeOfOp(exp,NULL, exp_type);
     ROSE_ASSERT(result);
     if (exp)
        {
          exp->set_parent(result);
          markLhsValues(result);
        }
     setOneSourcePositionNull(result);
     return result;
   }

//! Build sizeof() expression with a type parameter
SgSizeOfOp* SageBuilder::buildSizeOfOp(SgType* type /* = NULL*/)
   {
     SgSizeOfOp* result = new SgSizeOfOp((SgExpression*)NULL,type,NULL);
  // SgSizeOfOp* result = new SgSizeOfOp((SgExpression*)NULL,type,type);
     ROSE_ASSERT(result);
     setOneSourcePositionForTransformation(result);
     return result;
   }

//! Build sizeof() expression with a type parameter
SgSizeOfOp* SageBuilder::buildSizeOfOp_nfi(SgType* type /* = NULL*/)
   {
     SgSizeOfOp* result = new SgSizeOfOp((SgExpression*)NULL,type,NULL);
  // SgSizeOfOp* result = new SgSizeOfOp((SgExpression*)NULL,type,type);
     ROSE_ASSERT(result);
     setOneSourcePositionNull(result);
     return result;
   }

//! Build __alignof__() expression with an expression parameter
SgAlignOfOp* SageBuilder::buildAlignOfOp(SgExpression* exp/*= NULL*/)
   {
  // SgType* exp_type =NULL;
  // if (exp) exp_type = exp->get_type();

     SgAlignOfOp* result = new SgAlignOfOp(exp,NULL, NULL);
     ROSE_ASSERT(result);
     if (exp)
        {
          exp->set_parent(result);
          markLhsValues(result);
        }
     setOneSourcePositionForTransformation(result);
     return result;
   }

//! Build __alignof__() expression with an expression parameter
SgAlignOfOp* SageBuilder::buildAlignOfOp_nfi(SgExpression* exp/*= NULL*/)
   {
  // SgType* exp_type =NULL;
  // if (exp) exp_type = exp->get_type();

     SgAlignOfOp* result = new SgAlignOfOp(exp,NULL, NULL);
     ROSE_ASSERT(result);
     if (exp)
        {
          exp->set_parent(result);
          markLhsValues(result);
        }
     setOneSourcePositionNull(result);
     return result;
   }

//! Build noexcept operator expression with an expression parameter
SgNoexceptOp* SageBuilder::buildNoexceptOp(SgExpression* exp/*= NULL*/)
   {
  // SgType* exp_type =NULL;
  // if (exp) exp_type = exp->get_type();

     SgNoexceptOp* result = new SgNoexceptOp(exp);
     ROSE_ASSERT(result);
     if (exp)
        {
          exp->set_parent(result);
          markLhsValues(result);
        }

     setOneSourcePositionForTransformation(result);
     return result;
   }

//! Build noexcept operator expression with an expression parameter
SgNoexceptOp* SageBuilder::buildNoexceptOp_nfi(SgExpression* exp/*= NULL*/)
   {
  // SgType* exp_type =NULL;
  // if (exp) exp_type = exp->get_type();

     SgNoexceptOp* result = new SgNoexceptOp(exp);
     ROSE_ASSERT(result);
     if (exp)
        {
          exp->set_parent(result);
          markLhsValues(result);
        }

     setOneSourcePositionNull(result);
     return result;
   }

//! Build __alignof__() expression with a type parameter
SgAlignOfOp* SageBuilder::buildAlignOfOp(SgType* type /* = NULL*/)
   {
     SgAlignOfOp* result = new SgAlignOfOp((SgExpression*)NULL,type,NULL);
     ROSE_ASSERT(result);
     setOneSourcePositionForTransformation(result);
     return result;
   }

//! Build __alignof__() expression with a type parameter
SgAlignOfOp* SageBuilder::buildAlignOfOp_nfi(SgType* type /* = NULL*/)
   {
     SgAlignOfOp* result = new SgAlignOfOp((SgExpression*)NULL,type,NULL);
     ROSE_ASSERT(result);
     setOneSourcePositionNull(result);
     return result;
   }


SgExprListExp * SageBuilder::buildExprListExp(SgExpression * expr1, SgExpression* expr2, SgExpression* expr3, SgExpression* expr4, SgExpression* expr5, SgExpression* expr6, SgExpression* expr7, SgExpression* expr8, SgExpression* expr9, SgExpression* expr10)
{
  SgExprListExp* expList = new SgExprListExp();
  ROSE_ASSERT(expList);

//  printf ("In SageBuilder::buildExprListExp(SgExpression * expr1, SgExpression* expr2, ...): SgExprListExp* expList = %p \n",expList);

  setOneSourcePositionForTransformation(expList);
  if (expr1) appendExpression(expList, expr1);
  if (expr2) appendExpression(expList, expr2);
  if (expr3) appendExpression(expList, expr3);
  if (expr4) appendExpression(expList, expr4);
  if (expr5) appendExpression(expList, expr5);
  if (expr6) appendExpression(expList, expr6);
  if (expr7) appendExpression(expList, expr7);
  if (expr8) appendExpression(expList, expr8);
  if (expr9) appendExpression(expList, expr9);
  if (expr10) appendExpression(expList, expr10);
  return expList;
}

// CH (5/11/2010): Seems that this function is useful.
SgExprListExp * SageBuilder::buildExprListExp(const std::vector<SgExpression*>& exprs)
{
  SgExprListExp* expList = new SgExprListExp();
  ROSE_ASSERT(expList);

#if 0
  printf ("In SageBuilder::buildExprListExp(const std::vector<SgExpression*>& exprs): SgExprListExp* expList = %p \n",expList);
#endif

  setOneSourcePositionForTransformation(expList);
  for (size_t i = 0; i < exprs.size(); ++i) {
    appendExpression(expList, exprs[i]);
  }
  return expList;
}

SgExprListExp * SageBuilder::buildExprListExp_nfi()
   {
     SgExprListExp* expList = new SgExprListExp();
     ROSE_ASSERT(expList);

#if 0
     printf ("In SageBuilder::buildExprListExp_nfi(): SgExprListExp* expList = %p \n",expList);
#endif

     setOneSourcePositionNull(expList);
     return expList;
   }

SgExprListExp * SageBuilder::buildExprListExp_nfi(const std::vector<SgExpression*>& exprs)
   {
     SgExprListExp* expList = new SgExprListExp();
     ROSE_ASSERT(expList != NULL);

#if 0
     printf ("In SageBuilder::buildExprListExp_nfi(const std::vector<SgExpression*>& exprs): SgExprListExp* expList = %p expList->get_expressions().size() = %" PRIuPTR " \n",expList,expList->get_expressions().size());
#endif

     setOneSourcePositionNull(expList);
     for (size_t i = 0; i < exprs.size(); ++i)
        {
#if 0
          printf ("In SageBuilder::buildExprListExp_nfi(): exprs[i=%" PRIuPTR "] = %p = %s \n",i,exprs[i],exprs[i]->class_name().c_str());
#endif
          appendExpression(expList, exprs[i]);
        }

  // DQ (4/3/2012): Added test to make sure that the pointers are unique.
     testAstForUniqueNodes(expList);

     return expList;
   }

SgVarRefExp*
SageBuilder::buildVarRefExp(SgInitializedName* initname, SgScopeStatement* scope)
   {
     ROSE_ASSERT(initname);
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();
  // ROSE_ASSERT(scope != NULL); // we allow to build a dangling ref without symbol

     SgVarRefExp *varRef = NULL;
  // there is assertion for get_scope() != NULL in get_symbol_from_symbol_table()
     SgSymbol* symbol = NULL;
     if (initname->get_scope()!=NULL)
          symbol = initname->get_symbol_from_symbol_table ();

     if (symbol != NULL)
        {
          varRef = new SgVarRefExp(isSgVariableSymbol(symbol));
          setOneSourcePositionForTransformation(varRef);
          ROSE_ASSERT(varRef);
        }
       else
        {
#if 0
          printf ("In SageBuilder::buildVarRefExp(): we might be reusing an existing SgVarRefExp \n");
#endif
          varRef = buildVarRefExp(initname->get_name(), scope);
        }

#if 0
     printf ("In SageBuilder::buildVarRefExp(): Returning SgVarRefExp = %p \n",varRef);
#endif

     return varRef;
   }

SgVarRefExp *
SageBuilder::buildVarRefExp(const char* varName, SgScopeStatement* scope)
{
   SgName name(varName);
   return buildVarRefExp(name,scope);
}

SgVarRefExp *
SageBuilder::buildVarRefExp(const std::string& varName, SgScopeStatement* scope)
//SageBuilder::buildVarRefExp(std::string& varName, SgScopeStatement* scope=NULL)
{
  SgName name(varName);
  return buildVarRefExp(name,scope);
}

SgVarRefExp *
SageBuilder::buildVarRefExp(const SgName& name, SgScopeStatement* scope/*=NULL*/)
   {
#if 0
     printf ("In SageBuilder::buildVarRefExp(): scope = %p = %s = %s \n",scope,scope->class_name().c_str(),get_name(scope).c_str());
#endif

     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

  // ROSE_ASSERT(scope != NULL); // we allow to build a dangling ref without symbol
     SgSymbol*         symbol    = NULL;
     SgVariableSymbol* varSymbol = NULL;

     if (scope != NULL)
        {
       // DQ (12/30/2011): This is a bad idea for C++ since qualified names might indicate different scopes.
       // If the scope has been provided then is should be the correct scope.
#if 1
       // DQ (8/16/2013): Modified to use the new API supporting template parameters and template arguments, however
       // this should more likely be using lookupVariableSymbolInParentScopes() instead of lookupSymbolInParentScopes().
       // printf ("In SageBuilder::buildVarRefExp(): switch from lookupSymbolInParentScopes() and lookupVariableSymbolInParentScopes() \n");
       // symbol = lookupSymbolInParentScopes(name,scope);
          symbol = lookupVariableSymbolInParentScopes(name,scope);
#else
#error "DAED CODE!"
          symbol = scope->lookup_variable_symbol(name);
#endif
#if 0
          printf ("In SageBuilder::buildVarRefExp(): scope = %p = %s name = %s symbol = %p \n",scope,scope->class_name().c_str(),name.str(),symbol);
#endif
//        ROSE_ASSERT(symbol != NULL);
        }

     if (symbol != NULL)
        {
#if 0
          printf ("What type of symbol is this: symbol = %p = %s \n",symbol,symbol->class_name().c_str());
#endif
          varSymbol = isSgVariableSymbol(symbol);
        }
       else
        {
       // if not found: put fake init name and symbol here and
       // waiting for a postProcessing phase to clean it up
       // two features: no scope and unknown type for initializedName
          SgInitializedName * name1 = buildInitializedName(name,SgTypeUnknown::createType());
          name1->set_scope(scope);  // buildInitializedName() does not set scope for various reasons
          name1->set_parent(scope);
          varSymbol = new SgVariableSymbol(name1);
          varSymbol->set_parent(scope);

       // DQ (4/2/2012): Output a warning:
#if 0
          printf ("WARNING: In SageBuilder::buildVarRefExp(): symbol not found so we built a SgVariableSymbol = %p (but not put into symbol table) \n",varSymbol);
#endif
        }

     if (varSymbol == NULL)
        {
          printf ("Error: varSymbol == NULL for name = %s \n",name.str());
        }
     ROSE_ASSERT(varSymbol != NULL);

     SgVarRefExp *varRef = new SgVarRefExp(varSymbol);
     setOneSourcePositionForTransformation(varRef);
     ROSE_ASSERT(varRef != NULL);

     ROSE_ASSERT (isSgVariableSymbol(varRef->get_symbol())->get_declaration()!=NULL);
#if 0
     printf ("In SageBuilder::buildVarRefExp(const SgName& name, SgScopeStatement* scope = %p): varRef = %p \n",scope,varRef);
#endif

     return varRef;
   }

//! Build a variable reference from an existing variable declaration. The assumption is a SgVariableDeclartion only declares one variable in the ROSE AST.
SgVarRefExp *
SageBuilder::buildVarRefExp(SgVariableDeclaration* vardecl)
   {
     SgVariableSymbol* symbol = getFirstVarSym(vardecl);
     ROSE_ASSERT(symbol);

     return buildVarRefExp(symbol);
   }


SgVarRefExp *
SageBuilder::buildVarRefExp(SgVariableSymbol* sym)
   {
     SgVarRefExp *varRef = new SgVarRefExp(sym);
     ROSE_ASSERT(varRef);

     setOneSourcePositionForTransformation(varRef);

#if 0
     printf ("In SageBuilder::buildVarRefExp(SgVariableSymbol* sym): Returning SgVarRefExp = %p \n",varRef);
#endif

     return varRef;
   }

SgVarRefExp *
SageBuilder::buildVarRefExp_nfi(SgVariableSymbol* sym)
   {
     SgVarRefExp *varRef = new SgVarRefExp(sym);
     ROSE_ASSERT(varRef);

     setOneSourcePositionNull(varRef);

#if 0
     printf ("In SageBuilder::buildVarRefExp_nfi(SgVariableSymbol* sym): Returning SgVarRefExp = %p \n",varRef);
#endif

     return varRef;
   }

SgNonrealRefExp *
SageBuilder::buildNonrealRefExp_nfi(SgNonrealSymbol * sym)
   {
     SgNonrealRefExp * refexp = new SgNonrealRefExp(sym);
     ROSE_ASSERT(refexp != NULL);
     setOneSourcePositionNull(refexp);
     return refexp;
   }

//!Build a variable reference expression at scope to an opaque variable which has unknown information except for its name.  Used when referring to an internal variable defined in some headers of runtime libraries.(The headers are not yet inserted into the file during translation). Similar to buildOpaqueType();
/*! It will declare a hidden int varName  at the specified scope to cheat the AST consistence tests.
 */
SgVarRefExp*
SageBuilder::buildOpaqueVarRefExp(const std::string& name,SgScopeStatement* scope/* =NULL */)
   {
     SgVarRefExp *result = NULL;

     if (scope == NULL)
          scope = SageBuilder::topScopeStack();
     ROSE_ASSERT(scope != NULL);

  // DQ (8/16/2013): Modified to use the new API supporting template parameters and template arguments, however
  // this should more likely be using lookupVariableSymbolInParentScopes() instead of lookupSymbolInParentScopes().
  // SgSymbol * symbol = lookupSymbolInParentScopes(name,scope);
     SgSymbol * symbol = lookupVariableSymbolInParentScopes(name,scope);

     if (symbol)
        {
       // Can be the same opaque var ref built before
       // cerr<<"Error: trying to build an opaque var ref when the variable is actual explicit!"<<endl;
       // ROSE_ASSERT(false);
          ROSE_ASSERT(isSgVariableSymbol(symbol));
          result = buildVarRefExp(isSgVariableSymbol(symbol));

       // DQ (4/2/2012): Output a warning:
       //   printf ("WARNING: In SageBuilder::buildOpaqueVarRefExp(): proper symbol used to build SgVarRefExp = %p \n",result);
        }
       else
        {
         SgVariableDeclaration* fakeVar = buildVariableDeclaration(name, buildIntType(),NULL, scope);
         Sg_File_Info* file_info = fakeVar->get_file_info();

      // TGWE (7/16/2014): on the advice of DQ who doesn't like the function at all
         fakeVar->set_parent(scope);

         file_info->unsetOutputInCodeGeneration ();
         SgVariableSymbol* fakeSymbol = getFirstVarSym (fakeVar);
         result = buildVarRefExp(fakeSymbol);

       // DQ (4/2/2012): Output a warning:
       //   printf ("WARNING: In SageBuilder::buildOpaqueVarRefExp(): fake symbol generated to build SgVarRefExp = %p (but not put into symbol table) \n",result);
        }

     return result;
   } // buildOpaqueVarRefExp()


// DQ (9/4/2013): Added support for building compound literals (similar to a SgVarRefExp).
//! Build function for compound literals (uses a SgVariableSymbol and is similar to buildVarRefExp_nfi()).
SgCompoundLiteralExp*
SageBuilder::buildCompoundLiteralExp_nfi(SgVariableSymbol* varSymbol)
   {
     SgCompoundLiteralExp *compoundLiteral = new SgCompoundLiteralExp(varSymbol);
     ROSE_ASSERT(compoundLiteral != NULL);

     setOneSourcePositionNull(compoundLiteral);

#if 0
     printf ("In SageBuilder::buildCompoundLiteralExp_nfi(SgVariableSymbol* sym): Returning SgCompoundLiteralExp = %p \n",compoundLiteral);
#endif

     return compoundLiteral;
   }

// DQ (9/4/2013): Added support for building compound literals (similar to a SgVarRefExp).
//! Build function for compound literals (uses a SgVariableSymbol and is similar to buildVarRefExp()).
SgCompoundLiteralExp*
SageBuilder::buildCompoundLiteralExp(SgVariableSymbol* varSymbol)
   {
     SgCompoundLiteralExp *compoundLiteral = new SgCompoundLiteralExp(varSymbol);
     ROSE_ASSERT(compoundLiteral != NULL);

     setOneSourcePositionForTransformation(compoundLiteral);

#if 0
     printf ("In SageBuilder::buildCompoundLiteralExp(SgVariableSymbol* sym): Returning SgCompoundLiteralExp = %p \n",compoundLiteral);
#endif

     return compoundLiteral;
   }


//! Build a Fortran numeric label ref exp
SgLabelRefExp * SageBuilder::buildLabelRefExp(SgLabelSymbol * s)
{
   SgLabelRefExp * result= NULL;
   ROSE_ASSERT (s!= NULL);
   result = new SgLabelRefExp(s);
   ROSE_ASSERT (result != NULL);
   setOneSourcePositionForTransformation(result);
   return result;
}

SgFunctionParameterList*
SageBuilder::buildFunctionParameterList(SgFunctionParameterTypeList * paraTypeList)
{
  SgFunctionParameterList* paraList = buildFunctionParameterList();
  if (paraTypeList==NULL) return paraList;

  SgTypePtrList typeList = paraTypeList->get_arguments();
  SgTypePtrList::iterator i;
  for (i=typeList.begin();i!=typeList.end();i++)
  {
    SgInitializedName* arg = buildInitializedName(SgName(""),(*i));
    appendArg(paraList,arg);
  }

  return paraList;
}

SgFunctionParameterList*
SageBuilder::buildFunctionParameterList_nfi(SgFunctionParameterTypeList * paraTypeList)
{
  SgFunctionParameterList* paraList = buildFunctionParameterList();
  ROSE_ASSERT (paraList);
  SgTypePtrList typeList = paraTypeList->get_arguments();
  SgTypePtrList::iterator i;
  for (i=typeList.begin();i!=typeList.end();i++)
  {
    SgInitializedName* arg = buildInitializedName_nfi(SgName(""),(*i),NULL);
    appendArg(paraList,arg);
  }
  return paraList;
}

// lookup function symbol to create a reference to it
SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(const SgName& name,const SgType* funcType, SgScopeStatement* scope /*=NULL*/)
{
  ROSE_ASSERT(funcType); // function type cannot be NULL
  SgFunctionType* func_type = isSgFunctionType(const_cast<SgType*>(funcType));
  ROSE_ASSERT(func_type);

  bool isMemberFunc = isSgMemberFunctionType(func_type);

  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);
  SgFunctionSymbol* symbol = lookupFunctionSymbolInParentScopes(name,func_type,scope);
  if (symbol == NULL)
    // in rare cases when function calls are inserted before any prototypes exist
  {
    SgType* return_type = func_type->get_return_type();
    SgFunctionParameterTypeList * paraTypeList = func_type->get_argument_list();
    SgFunctionParameterList *parList = buildFunctionParameterList(paraTypeList);

    SgGlobal* globalscope = getGlobalScope(scope);

    ROSE_ASSERT (isMemberFunc == false);  // Liao, 11/21/2012. We assume only regular functions can go into this if-body so we can insert them into global scope by default
 // TODO: consider C++ template functions and Fortran functions
 // SgFunctionDeclaration * funcDecl= buildNondefiningFunctionDeclaration(name,return_type,parList,globalscope);
 // SgFunctionDeclaration * funcDecl = buildNondefiningFunctionDeclaration_T <SgFunctionDeclaration>(name,return_type,parList,false,globalscope,NULL, false, NULL, NULL);

 // TODO: consider C++ template functions
    SgFunctionDeclaration * funcDecl = NULL;
    if (SageInterface::is_Fortran_language ())
       {
      // DQ (8/21/2013): Fixed number of parameters in buildNondefiningFunctionDeclaration_T() function call.
      // funcDecl = buildNondefiningFunctionDeclaration_T <SgProcedureHeaderStatement>(name,return_type,parList,false,globalscope,NULL, false, NULL);
         funcDecl = buildNondefiningFunctionDeclaration_T <SgProcedureHeaderStatement>(name,return_type,parList,false,globalscope,NULL, false, NULL, NULL);
       }
      else
       {
      // DQ (8/21/2013): Fixed number of parameters in buildNondefiningFunctionDeclaration_T() function call.
      // funcDecl = buildNondefiningFunctionDeclaration_T <SgFunctionDeclaration>(name,return_type,parList,false,globalscope,NULL, false, NULL);
          funcDecl = buildNondefiningFunctionDeclaration_T <SgFunctionDeclaration>(name,return_type,parList,false,globalscope,NULL, false, NULL, NULL);
       }

    funcDecl->get_declarationModifier().get_storageModifier().setExtern();

    // This will conflict with prototype in a header
    // prepend_statement(globalscope,funcDecl);
    // Prepend a function prototype declaration in current scope, hide it from the unparser
    // prependStatement(funcDecl,scope);
    // Sg_File_Info* file_info = funcDecl->get_file_info();
    // file_info->unsetOutputInCodeGeneration ();

    symbol = lookupFunctionSymbolInParentScopes(name,func_type,scope);
    ROSE_ASSERT(symbol);
  }
  SgFunctionRefExp* func_ref = new SgFunctionRefExp(symbol,func_type);
  setOneSourcePositionForTransformation(func_ref);

  ROSE_ASSERT(func_ref);
  return func_ref;
}

SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(const char* name,const SgType* funcType, SgScopeStatement* scope /*=NULL*/)
{
  SgName name2(name);
  return buildFunctionRefExp(name2,funcType,scope);
}

// lookup function symbol to create a reference to it
SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(const SgFunctionDeclaration* func_decl)
{
  ROSE_ASSERT(func_decl != NULL);
  SgDeclarationStatement* nondef_func = func_decl->get_firstNondefiningDeclaration ();
  SgDeclarationStatement* def_func = func_decl->get_definingDeclaration ();
  SgSymbol* symbol = NULL;
  if (nondef_func != NULL)
  {
    ROSE_ASSERT(nondef_func!= NULL);
    symbol = nondef_func->get_symbol_from_symbol_table();
    ROSE_ASSERT( symbol != NULL);
  }
  // Liao 12/1/2010. It is possible that there is no prototype declarations at all
  else if (def_func != NULL)
  {
    symbol = def_func->get_symbol_from_symbol_table();
  }
  else
  {
    cerr<<"Fatal error: SageBuilder::buildFunctionRefExp():defining and nondefining declarations for a function cannot be both NULL"<<endl;
    ROSE_ASSERT (false);
  }
  ROSE_ASSERT( symbol != NULL);
  return buildFunctionRefExp( isSgFunctionSymbol (symbol));
}


// lookup function symbol to create a reference to it
SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(SgFunctionSymbol* sym)
{
  SgFunctionRefExp* func_ref = new SgFunctionRefExp(sym, NULL);
  setOneSourcePositionForTransformation(func_ref);
  ROSE_ASSERT(func_ref);
  return func_ref;
}

// lookup function symbol to create a reference to it
SgFunctionRefExp *
SageBuilder::buildFunctionRefExp_nfi(SgFunctionSymbol* sym)
{
  SgFunctionRefExp* func_ref = new SgFunctionRefExp(sym, NULL);
  setOneSourcePositionNull(func_ref);
  ROSE_ASSERT(func_ref);
  return func_ref;
}

// DQ (12/15/2011): Adding template declaration support to the AST.
SgTemplateFunctionRefExp *
SageBuilder::buildTemplateFunctionRefExp_nfi(SgTemplateFunctionSymbol* sym)
   {
  // DQ (2/23/2013): Added assertion.
     ROSE_ASSERT(sym != NULL);

     SgTemplateFunctionRefExp* func_ref = new SgTemplateFunctionRefExp(sym);
     ROSE_ASSERT(func_ref != NULL);

     setOneSourcePositionNull(func_ref);

  // DQ (2/23/2013): Added assertion.
     ROSE_ASSERT(func_ref->get_symbol() != NULL);

     return func_ref;
   }

// DQ (12/29/2011): Adding template declaration support to the AST.
SgTemplateMemberFunctionRefExp *
SageBuilder::buildTemplateMemberFunctionRefExp_nfi(SgTemplateMemberFunctionSymbol* sym, bool virtual_call, bool need_qualifier)
   {
     SgTemplateMemberFunctionRefExp* func_ref = new SgTemplateMemberFunctionRefExp(sym, virtual_call, need_qualifier);
     setOneSourcePositionNull(func_ref);
     ROSE_ASSERT(func_ref);
     return func_ref;
   }

// lookup member function symbol to create a reference to it
SgMemberFunctionRefExp *
SageBuilder::buildMemberFunctionRefExp_nfi(SgMemberFunctionSymbol* sym, bool virtual_call, bool need_qualifier)
{
  SgMemberFunctionRefExp* func_ref = new SgMemberFunctionRefExp(sym, virtual_call, NULL, need_qualifier);
  setOneSourcePositionNull(func_ref);
  ROSE_ASSERT(func_ref);
  return func_ref;
}

// lookup member function symbol to create a reference to it
SgMemberFunctionRefExp *
SageBuilder::buildMemberFunctionRefExp(SgMemberFunctionSymbol* sym, bool virtual_call, bool need_qualifier)
{
  SgMemberFunctionRefExp* func_ref = new SgMemberFunctionRefExp(sym, virtual_call, NULL, need_qualifier);
  setOneSourcePositionForTransformation(func_ref);
  ROSE_ASSERT(func_ref);
  return func_ref;
}

// lookup class symbol to create a reference to it
SgClassNameRefExp *
SageBuilder::buildClassNameRefExp_nfi(SgClassSymbol* sym)
{
  SgClassNameRefExp* class_ref = new SgClassNameRefExp(sym);
  setOneSourcePositionNull(class_ref);
  ROSE_ASSERT(class_ref);
  return class_ref;
}

SgClassNameRefExp *
SageBuilder::buildClassNameRefExp(SgClassSymbol* sym)
{
  SgClassNameRefExp* class_ref = new SgClassNameRefExp(sym);
  setOneSourcePositionForTransformation(class_ref);
  ROSE_ASSERT(class_ref);
  return class_ref;
}

//! Lookup a C style function symbol to create a function reference expression to it
SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(const SgName& name, SgScopeStatement* scope /*=NULL*/)
{
  if (scope == NULL)
     scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);
  SgFunctionSymbol* symbol = lookupFunctionSymbolInParentScopes(name,scope);

  if (symbol==NULL)
// in rare cases when function calls are inserted before any prototypes exist
  {
// assume int return type, and empty parameter list

#if 1
// DQ (7/26/2012): I am at least temporarily removing this function from the API.
// Later if we need it, we can update it to reflect that passing of the new
// SgTemplateArgumentPtrList function parameter (part of the new API design).

   SgFunctionDeclaration* funcDecl = NULL;
   printf ("Error: buildFunctionRefExp(): This function should not be used! \n");
   ROSE_ASSERT(false);
#else
    SgType* return_type = buildIntType();
    SgFunctionParameterList *parList = buildFunctionParameterList();

    SgGlobal* globalscope = getGlobalScope(scope);

    SgFunctionDeclaration * funcDecl = buildNondefiningFunctionDeclaration(name,return_type,parList,globalscope);
#endif

    funcDecl->get_declarationModifier().get_storageModifier().setExtern();


    symbol = lookupFunctionSymbolInParentScopes(name,scope);
    ROSE_ASSERT(symbol);
  }

  SgFunctionRefExp* func_ref = buildFunctionRefExp(symbol);
  setOneSourcePositionForTransformation(func_ref);

  ROSE_ASSERT(func_ref);
  return func_ref;
}

SgFunctionRefExp *
SageBuilder::buildFunctionRefExp(const char* name, SgScopeStatement* scope /*=NULL*/)
{
  SgName name2(name);
  return buildFunctionRefExp(name2,scope);
}


SgExprStatement*
SageBuilder::buildExprStatement(SgExpression*  exp)
{
  SgExprStatement* expStmt = new SgExprStatement(exp);
  ROSE_ASSERT(expStmt);
  if (exp) exp->set_parent(expStmt);
  setOneSourcePositionForTransformation(expStmt);
  return expStmt;
}

SgExprStatement*
SageBuilder::buildExprStatement_nfi(SgExpression*  exp)
{
  SgExprStatement* expStmt = new SgExprStatement(exp);
  ROSE_ASSERT(expStmt);
  if (exp) exp->set_parent(expStmt);
  setOneSourcePositionNull(expStmt);
  return expStmt;
}

// DQ (3/27/2015): Added support for SgStatementExpression.
SgStatementExpression*
SageBuilder::buildStatementExpression(SgStatement*  exp)
{
  SgStatementExpression* expStmt = new SgStatementExpression(exp);
  ROSE_ASSERT(expStmt);
  if (exp) exp->set_parent(expStmt);
  setOneSourcePositionForTransformation(expStmt);

  return expStmt;
}

// DQ (3/27/2015): Added support for SgStatementExpression.
SgStatementExpression*
SageBuilder::buildStatementExpression_nfi(SgStatement*  exp)
{
  SgStatementExpression* expStmt = new SgStatementExpression(exp);
  ROSE_ASSERT(expStmt);
  if (exp) exp->set_parent(expStmt);
  setOneSourcePositionNull(expStmt);

  return expStmt;
}

SgFunctionCallExp*
SageBuilder::buildFunctionCallExp(const SgName& name, SgType* return_type, SgExprListExp* parameters/*=NULL*/, SgScopeStatement* scope/*=NULL*/)
   {
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();
     ROSE_ASSERT(scope != NULL);

     if (parameters == NULL)
          parameters = buildExprListExp();

#if 0
     printf ("In SageBuilder::buildFunctionCallExp(): calling buildFunctionParameterTypeList() \n");
#endif

     SgFunctionParameterTypeList * typeList = buildFunctionParameterTypeList(parameters);
     SgFunctionType * func_type = buildFunctionType(return_type,typeList);
     SgFunctionRefExp* func_ref = buildFunctionRefExp(name,func_type,scope);
     SgFunctionCallExp * func_call_expr = new SgFunctionCallExp(func_ref,parameters,func_ref->get_type());
     parameters->set_parent(func_call_expr);
     setOneSourcePositionForTransformation(func_call_expr);
     ROSE_ASSERT(func_call_expr);

     return func_call_expr;
   }


SgFunctionCallExp*
SageBuilder::buildFunctionCallExp(SgFunctionSymbol* sym,
                                  SgExprListExp* parameters/*=NULL*/)
   {
     ROSE_ASSERT (sym);
     if (parameters == NULL)
          parameters = buildExprListExp();
     ROSE_ASSERT (parameters);

  // DQ (8/21/2011): We want to preserve the support for member functions to be built as SgMemberFunctionRefExp.
  // This is important for the Java support and the C++ support else we will be lowering all mmember function calls
  // to function calls which will be a proble for eht analysis of object oriented languages.
  // SgFunctionRefExp* func_ref = buildFunctionRefExp(sym);
  // SgFunctionCallExp * func_call_expr = new SgFunctionCallExp(func_ref,parameters,func_ref->get_type());
  // func_ref->set_parent(func_call_expr);
     SgFunctionCallExp * func_call_expr = NULL;
     SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(sym);
     if (memberFunctionSymbol != NULL)
        {
       // Note that we can't at this point be sure this is not a virtual function.
          bool virtual_call = false;

       // Name qualificaiton is handled separately from the setting of this variable (old API).
          bool need_qualifier = false;

          SgMemberFunctionRefExp* member_func_ref = buildMemberFunctionRefExp(memberFunctionSymbol,virtual_call,need_qualifier);
          func_call_expr = new SgFunctionCallExp(member_func_ref,parameters,member_func_ref->get_type());
          member_func_ref->set_parent(func_call_expr);
        }
       else
        {
          SgFunctionRefExp * func_ref = buildFunctionRefExp(sym);
          func_call_expr = new SgFunctionCallExp(func_ref,parameters,func_ref->get_type());
          func_ref->set_parent(func_call_expr);
        }


     parameters->set_parent(func_call_expr);

     setOneSourcePositionForTransformation(func_call_expr);

     ROSE_ASSERT(func_call_expr);
     return func_call_expr;
   }

SgFunctionCallExp*
SageBuilder::buildFunctionCallExp_nfi(SgExpression* f, SgExprListExp* parameters /*=NULL*/)
   {
     SgFunctionCallExp * func_call_expr = new SgFunctionCallExp(f,parameters,f->get_type());
     ROSE_ASSERT(func_call_expr != NULL);

     if (f) f->set_parent(func_call_expr);
     if (parameters) parameters->set_parent(func_call_expr);
     setOneSourcePositionNull(func_call_expr);

     return func_call_expr;
   }

SgFunctionCallExp*
SageBuilder::buildFunctionCallExp(SgExpression* f, SgExprListExp* parameters/*=NULL*/)
   {
     SgFunctionCallExp * func_call_expr = new SgFunctionCallExp(f,parameters,f->get_type());
     ROSE_ASSERT(func_call_expr != NULL);

     if (f) f->set_parent(func_call_expr);
     if (parameters) parameters->set_parent(func_call_expr);
     setOneSourcePositionForTransformation(func_call_expr);

     return func_call_expr;
   }

SgExprStatement*
SageBuilder::buildFunctionCallStmt(const SgName& name,
                      SgType* return_type,
                      SgExprListExp* parameters /*= NULL*/,
                      SgScopeStatement* scope /*=NULL*/)
{
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
  ROSE_ASSERT(scope != NULL);
  SgFunctionCallExp* func_call_expr = buildFunctionCallExp(name,return_type,parameters,scope);
  SgExprStatement * expStmt = buildExprStatement(func_call_expr);
  return expStmt;
}

//! Build a function call statement using function expression and argument list only, like (*funcPtr)(args);
SgExprStatement*
SageBuilder::buildFunctionCallStmt(SgExpression* function_exp, SgExprListExp* parameters/*=NULL*/)
{
  SgFunctionCallExp* func_call_expr = buildFunctionCallExp(function_exp, parameters);
  SgExprStatement * expStmt = buildExprStatement(func_call_expr);
  return expStmt;
}

SgTypeTraitBuiltinOperator*
SageBuilder::buildTypeTraitBuiltinOperator(SgName functionName, SgNodePtrList parameters)
   {
  // DQ (7/14/2013): This is supporting compiler extensions that are required to support type traits in C++.
  // These operators are used increasingly in newer versions of GNU and other compilers.  They are builtin
  // compiler extensions that typically take types as arguments.

     SgTypeTraitBuiltinOperator * builtin_func_call_expr = new SgTypeTraitBuiltinOperator(functionName);
     ROSE_ASSERT(builtin_func_call_expr != NULL);

  // Note that this is copy by value...on SgNodePtrList (because we have to support both SgExpression and SgType IR nodes as arguments).
  // In this way this is implemented differently from a SgCallExpression ro SgFunctionCallExp (which uses a SgExprListExp).
     builtin_func_call_expr->get_args() = parameters;

     return builtin_func_call_expr;
   }


//! Build a CUDA kernel call expression (kernel<<<config>>>(parameters))
SgCudaKernelCallExp * SageBuilder::buildCudaKernelCallExp_nfi(SgExpression * kernel, SgExprListExp* parameters, SgCudaKernelExecConfig * config)
   {
     ROSE_ASSERT(kernel);
     ROSE_ASSERT(parameters);
     ROSE_ASSERT(config);

  // DQ (1/19/2016): Adding template function ref support.
     SgFunctionRefExp * func_ref_exp = isSgFunctionRefExp(kernel);
     SgTemplateFunctionRefExp * template_func_ref_exp = isSgTemplateFunctionRefExp(kernel);
  // if (func_ref_exp == NULL)
     if (func_ref_exp == NULL && template_func_ref_exp == NULL)
        {
#if 0
          printf ("Error: SageBuilder::buildCudaKernelCallExp_nfi(): kernel = %p = %s \n",kernel,kernel->class_name().c_str());
#endif
          std::cerr << "SgCudaKernelCallExp accept only direct reference to a function. Got, " << typeid(*kernel).name()
                    << " with, " << kernel->unparseToString() << std::endl;

          // PP (7/1/19): experimental support for RAJA/CUDA Lulesh codes (producing SgNonrealRefExp) **1
          // was: ROSE_ASSERT(false);
        }

    else // was not here (**1)

  // DQ (1/19/2016): Adding template function ref support.
  // if (!(func_ref_exp->get_symbol_i()->get_declaration()->get_functionModifier().isCudaKernel()))
     if ( (func_ref_exp          != NULL && func_ref_exp->get_symbol_i()->get_declaration()->get_functionModifier().isCudaKernel() == false) &&
          (template_func_ref_exp != NULL && template_func_ref_exp->get_symbol_i()->get_declaration()->get_functionModifier().isCudaKernel() == false) )
        {
#if 0
          printf ("Error: SageBuilder::buildCudaKernelCallExp_nfi(): kernel = %p = %s \n",kernel,kernel->class_name().c_str());
          if (func_ref_exp != NULL)
               printf ("func_ref_exp->get_symbol_i()->get_declaration()->get_functionModifier().isCudaKernel() = %s \n",
                    func_ref_exp->get_symbol_i()->get_declaration()->get_functionModifier().isCudaKernel() ? "true" : "false");
          if (template_func_ref_exp != NULL)
               printf ("template_func_ref_exp->get_symbol_i()->get_declaration()->get_functionModifier().isCudaKernel() = %s \n",
                    template_func_ref_exp->get_symbol_i()->get_declaration()->get_functionModifier().isCudaKernel() ? "true" : "false");
#endif
          std::cerr << "To build a SgCudaKernelCallExp the callee needs to be a kernel (having \"__global__\" attribute)." << std::endl;
          ROSE_ASSERT(false);
        }

     SgCudaKernelCallExp * kernel_call_expr = new SgCudaKernelCallExp(kernel, parameters, kernel->get_type(), config);

     kernel->set_parent(kernel_call_expr);
     parameters->set_parent(kernel_call_expr);
     config->set_parent(kernel_call_expr);

     setOneSourcePositionNull(kernel_call_expr);

     ROSE_ASSERT(kernel_call_expr);

     return kernel_call_expr;
   }

//! Build a CUDA kernel execution configuration (<<<grid, blocks, shared, stream>>>)
SgCudaKernelExecConfig * SageBuilder::buildCudaKernelExecConfig_nfi(SgExpression *grid, SgExpression *blocks, SgExpression *shared, SgExpression *stream) {
  if (!grid || !blocks) {
     std::cerr << "SgCudaKernelExecConfig need fields 'grid' and 'blocks' to be set." << std::endl;
     ROSE_ASSERT(false);
  }

  // TODO-CUDA check types

  SgCudaKernelExecConfig * config = new SgCudaKernelExecConfig (grid, blocks, shared, stream);

  grid->set_parent(config);
  blocks->set_parent(config);
  if (shared)
    shared->set_parent(config);
  if (stream)
    stream->set_parent(config);

  setOneSourcePositionNull(config);

  ROSE_ASSERT(config);

  return config;
}

SgExprStatement*
SageBuilder::buildAssignStatement(SgExpression* lhs,SgExpression* rhs)
//SageBuilder::buildAssignStatement(SgExpression* lhs,SgExpression* rhs, SgScopeStatement* scope=NULL)
{
  ROSE_ASSERT(lhs != NULL);
  ROSE_ASSERT(rhs != NULL);

  //SgAssignOp* assignOp = new SgAssignOp(lhs,rhs,lhs->get_type());
// SgBinaryOp::get_type() assume p_expression_type is not set
  SgAssignOp* assignOp = new SgAssignOp(lhs,rhs,NULL);
  ROSE_ASSERT(assignOp);
  setOneSourcePositionForTransformation(assignOp);
  lhs->set_parent(assignOp);
  rhs->set_parent(assignOp);

  lhs->set_lvalue (true);
  SgExprStatement* exp = new SgExprStatement(assignOp);
  ROSE_ASSERT(exp);
   // some child nodes are transparently generated, using recursive setting is safer
  setSourcePositionAtRootAndAllChildren(exp);
  //setOneSourcePositionForTransformation(exp);
  assignOp->set_parent(exp);
  return exp;
}

// DQ (8/16/2011): This is an AST translate specific version (see note below).
// We would like to phase out the version above if possible (but we want to
// test this later).
SgExprStatement*
SageBuilder::buildAssignStatement_ast_translate(SgExpression* lhs,SgExpression* rhs)
{
  ROSE_ASSERT(lhs != NULL);
  ROSE_ASSERT(rhs != NULL);

  //SgAssignOp* assignOp = new SgAssignOp(lhs,rhs,lhs->get_type());
// SgBinaryOp::get_type() assume p_expression_type is not set
  SgAssignOp* assignOp = new SgAssignOp(lhs,rhs,NULL);
  ROSE_ASSERT(assignOp);
  setOneSourcePositionForTransformation(assignOp);
  lhs->set_parent(assignOp);
  rhs->set_parent(assignOp);

  lhs->set_lvalue (true);
  SgExprStatement* exp = new SgExprStatement(assignOp);
  ROSE_ASSERT(exp);

// DQ (8/16/2011): Modified to avoid recursive call to reset source position information
// (this version is required for the Java support where we have set source code position
// information on the lhs and rhs and we don't want it to be reset as a transformation.
// some child nodes are transparently generated, using recursive setting is safer
  setOneSourcePositionForTransformation(exp);
  assignOp->set_parent(exp);
  return exp;
}


SgLabelStatement * SageBuilder::buildLabelStatement(const SgName& name,  SgStatement * stmt/*=NULL*/, SgScopeStatement* scope /*=NULL*/)
{
  if (scope == NULL)
    scope = SageBuilder::topScopeStack();
 //  ROSE_ASSERT(scope != NULL); // We support bottom up building of label statements now

   // should including current scope when searching for the function definition
   // since users can only pass FunctionDefinition when the function body is not yet attached
  SgLabelStatement * labelstmt = new SgLabelStatement(name,stmt);
  ROSE_ASSERT(labelstmt);
  setOneSourcePositionForTransformation(labelstmt);

 if(stmt!=NULL)
   stmt->set_parent(labelstmt);
 #if 0  // moved to fixLabelStatement()
  SgFunctionDefinition * label_scope = getEnclosingFunctionDefinition(scope,true);
  ROSE_ASSERT (label_scope);
  labelstmt->set_scope(label_scope);
  SgLabelSymbol* lsymbol= new SgLabelSymbol(labelstmt);
  ROSE_ASSERT(lsymbol);
  // TODO should we prevent duplicated insertion ?
  label_scope->insert_symbol(lsymbol->get_name(), lsymbol);
 #endif

  // Liao 1/7/2010
  // SgLabelStatement is used for CONTINUE statement in Fortran
  // In this case , it has no inherent association with a Label symbol.
  // It is up to the SageInterface::setNumericalLabel(SgStatement*) to handle label symbol
  if (!SageInterface::is_Fortran_language() &&scope)
    fixLabelStatement(labelstmt,scope);
  // we don't want to set parent here yet
  // delay it until append_statement() or alike
  return labelstmt;
}

SgLabelStatement * SageBuilder::buildLabelStatement_nfi(const SgName& name,  SgStatement * stmt/*=NULL*/, SgScopeStatement* scope /*=NULL*/)
{
  SgLabelStatement * labelstmt = new SgLabelStatement(name,stmt);
  ROSE_ASSERT(labelstmt);
  setOneSourcePositionForTransformation(labelstmt);

 if(stmt!=NULL)
   stmt->set_parent(labelstmt);
  if (scope)
    fixLabelStatement(labelstmt,scope);
  // we don't want to set parent here yet
  // delay it until append_statement() or alike
  return labelstmt;
}

SgIfStmt * SageBuilder::buildIfStmt(SgStatement* conditional, SgStatement * true_body, SgStatement * false_body)
{
  ROSE_ASSERT(conditional);
  ROSE_ASSERT(true_body);
  // ROSE_ASSERT(false_body); -- this is not required anymore
  SgIfStmt *ifstmt = new SgIfStmt(conditional, true_body, false_body);
  ROSE_ASSERT(ifstmt);

 // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
  if (symbol_table_case_insensitive_semantics == true)
       ifstmt->setCaseInsensitive(true);

  setOneSourcePositionForTransformation(ifstmt);
  conditional->set_parent(ifstmt);
  true_body->set_parent(ifstmt);
  if (false_body != NULL) false_body->set_parent(ifstmt);

  if (SageInterface::is_Fortran_language() )
  {
    // Liao 1/20/2010
    // According to Fortran 77 standard Chapter 11.5 to 11.9,
    // this is a Fortran Block IF statement, if the true body is:
    // 1. A block of statement under SgBasicBlock
    // 2. DO, block if, or another logical if
    // Otherwise it is a logical if statement
    if (isSgBasicBlock(true_body)|| isSgFortranDo(true_body)|| isSgIfStmt(true_body))
    {
      ifstmt->set_use_then_keyword(true);
      ifstmt->set_has_end_statement(true);
    }
  }

  return ifstmt;
}

SgIfStmt * SageBuilder::buildIfStmt_nfi(SgStatement* conditional, SgStatement * true_body, SgStatement * false_body)
   {
     SgIfStmt *ifstmt = new SgIfStmt(conditional, true_body, false_body);
     ROSE_ASSERT(ifstmt);
#if 0
  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
          ifstmt->setCaseInsensitive(true);

     setOneSourcePositionNull(ifstmt);
     if (conditional) conditional->set_parent(ifstmt);
     if (true_body) true_body->set_parent(ifstmt);
     if (false_body) false_body->set_parent(ifstmt);
     return ifstmt;
#else
  // DQ (2/13/2012): This allows us to separate teh construction from the initialization (see note below).
     initializeIfStmt(ifstmt,conditional,true_body,false_body);
     return ifstmt;
#endif
   }

// Rasmussen (9/3/2018): Added build function for a Fortran do construct
SgFortranDo * SageBuilder::buildFortranDo(SgExpression* initialization, SgExpression* bound, SgExpression* increment, SgBasicBlock* loop_body)
  {
     ROSE_ASSERT(initialization);
     ROSE_ASSERT(bound);

     if (increment == NULL)
       {
          increment = buildNullExpression();
       }
     ROSE_ASSERT(increment);

     if (loop_body == NULL)
       {
          loop_body = buildBasicBlock();
       }
     ROSE_ASSERT(loop_body);

     SgFortranDo * result = new SgFortranDo(initialization, bound, increment, loop_body);
     ROSE_ASSERT(result);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
        result->setCaseInsensitive(true);

     setOneSourcePositionForTransformation(result);

     initialization->set_parent(result);
     bound->set_parent(result);
     increment->set_parent(result);
     loop_body->set_parent(result);

     return result;
   }

// charles4 10/14/2011:  Vanilla allocation. Use prepend_init_stmt and append_init_stmt to populate afterward.
SgForInitStatement * SageBuilder::buildForInitStatement()
   {
  // return new SgForInitStatement();
     SgForInitStatement* result = new SgForInitStatement();

  // DQ (11/3/2012): Added call to set file info to default settings.
     setSourcePosition(result);

     return result;
   }

// DQ (10/12/2012): Added specific API to handle simple (single) statement.
SgForInitStatement*
SageBuilder::buildForInitStatement( SgStatement* statement )
   {
     SgForInitStatement* forInit = new SgForInitStatement();
     ROSE_ASSERT(forInit != NULL);

     ROSE_ASSERT(statement != NULL);
     forInit->append_init_stmt(statement);

  // DQ (11/3/2012): Added call to set file info to default settings.
     setSourcePosition(forInit);

     return forInit;
   }

SgForInitStatement * SageBuilder::buildForInitStatement(const SgStatementPtrList & statements)
{
  SgForInitStatement * result = new SgForInitStatement();
  result->get_init_stmt() = statements;

  for (SgStatementPtrList::iterator it = result->get_init_stmt().begin(); it != result->get_init_stmt().end(); it++)
    (*it)->set_parent(result);

  setOneSourcePositionForTransformation(result);
  return result;
}

SgForInitStatement*
SageBuilder::buildForInitStatement_nfi(SgStatementPtrList & statements)
   {
     SgForInitStatement * result = new SgForInitStatement();

     result->get_init_stmt() = statements;

     for (SgStatementPtrList::iterator it = result->get_init_stmt().begin(); it != result->get_init_stmt().end(); it++)
        {
#if 0
          printf ("In buildForInitStatement_nfi(): set the parent for it = %p = %s \n",*it,(*it)->class_name().c_str());
#endif
          (*it)->set_parent(result);
        }

  // DQ (11/3/2012): Added call to set file info to default settings.
     setSourcePosition(result);

     return result;
   }

//! Based on the contribution from Pradeep Srinivasa@ LANL
//Liao, 8/27/2008
SgForStatement * SageBuilder::buildForStatement(SgStatement* initialize_stmt, SgStatement * test, SgExpression * increment, SgStatement * loop_body, SgStatement * else_body)
{
     SgForStatement * result = new SgForStatement(test,increment, loop_body);
     ROSE_ASSERT(result);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
          result->setCaseInsensitive(true);

     setOneSourcePositionForTransformation(result);
     if (test)
          test->set_parent(result);
     if (loop_body)
          loop_body->set_parent(result);
     if (increment)
          increment->set_parent(result);

  if (else_body)
    else_body->set_parent(result);
  result->set_else_body(else_body);

  // CH (5/13/2010): If the initialize_stmt is an object of SgForInitStatement, we can directly put it
  // into for statement. Or else, there will be two semicolons after unparsing.
     if (SgForInitStatement* for_init_stmt = isSgForInitStatement(initialize_stmt))
        {
#if 0
          printf ("Handled a proper SgForInitStatement as input! \n");
#endif
       // DQ (7/30/2011): We have to delete the the SgForInitStatement build within the SgForStatement::post_constructor_initialization()
       // to avoid causing errors in the AST consistancy checking later.
          if (result->get_for_init_stmt() != NULL)
             {
#if 0
               printf ("Deleting the old one build in SgForStatement::post_constructor_initialization() \n");
#endif
               delete result->get_for_init_stmt();
               result->set_for_init_stmt(NULL);
             }

          result->set_for_init_stmt(for_init_stmt);
          for_init_stmt->set_parent(result);
          return result;
        }

     SgForInitStatement* init_stmt = new SgForInitStatement();
     ROSE_ASSERT(init_stmt);
     setOneSourcePositionForTransformation(init_stmt);

  // DQ (7/30/2011): We have to delete the the SgForInitStatement build within the SgForStatement::post_constructor_initialization().
  // to avoid causeing errors in the AST consistancy checking later.
     if (result->get_for_init_stmt() != NULL)
        {
          delete result->get_for_init_stmt();
          result->set_for_init_stmt(NULL);
        }

     result->set_for_init_stmt(init_stmt);
     init_stmt->set_parent(result);

     if (initialize_stmt)
        {
          init_stmt->append_init_stmt(initialize_stmt);
       // Support for "for (int i=0; )", Liao, 3/11/2009
       // The symbols are inserted into the symbol table attached to SgForStatement
          if (isSgVariableDeclaration(initialize_stmt))
             {
               fixVariableDeclaration(isSgVariableDeclaration(initialize_stmt),result);
            // fix varRefExp to the index variable used in increment, conditional expressions
               fixVariableReferences(result);
             }
        }

     return result;
   }


//! Based on the contribution from Pradeep Srinivasa@ LANL
//Liao, 8/27/2008
SgForStatement*
SageBuilder::buildForStatement_nfi(SgStatement* initialize_stmt, SgStatement * test, SgExpression * increment, SgStatement * loop_body, SgStatement * else_body)
   {
     SgForStatement * result = new SgForStatement(test, increment, loop_body);
     ROSE_ASSERT(result);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
          result->setCaseInsensitive(true);

     setOneSourcePositionNull(result);
     if (test)      test->set_parent(result);
     if (loop_body) loop_body->set_parent(result);
     if (increment) increment->set_parent(result);
     if (else_body) else_body->set_parent(result);

     result->set_else_body(else_body);

     if (initialize_stmt != NULL)
        {
          SgForInitStatement* init_stmt = result->get_for_init_stmt();
          ROSE_ASSERT(init_stmt);
          setOneSourcePositionNull(init_stmt);
          init_stmt->append_init_stmt(initialize_stmt);
          initialize_stmt->set_parent(init_stmt);
        }

     return result;
   }


SgForStatement*
SageBuilder::buildForStatement_nfi(SgForInitStatement * init_stmt, SgStatement * test, SgExpression * increment, SgStatement * loop_body, SgStatement * else_body)
   {
     SgForStatement * result = new SgForStatement(init_stmt, test, increment, loop_body);
     ROSE_ASSERT(result != NULL);

#if 1
  // DQ (9/26/2012): Refactored this function to allow for where the SgForStatement had to be
  // constructed early to define the scope for types that can be defined in the SgForInitStatement.
     buildForStatement_nfi(result,init_stmt,test,increment,loop_body,else_body);
#else
  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
          result->setCaseInsensitive(true);

     setOneSourcePositionNull(result);
     if (test)           test->set_parent(result);
     if (loop_body) loop_body->set_parent(result);
     if (increment) increment->set_parent(result);
     if (init_stmt) init_stmt->set_parent(result);

     if (else_body) init_stmt->set_parent(result);
     result->set_else_body(else_body);
#endif

     return result;
   }


void
SageBuilder::buildForStatement_nfi(SgForStatement* result, SgForInitStatement * init_stmt, SgStatement * test, SgExpression * increment, SgStatement * loop_body, SgStatement * else_body)
   {
  // DQ (9/26/2012): Refactored this function to allow for where the SgForStatement had to be
  // constructed early to define the scope for types that can be defined in the SgForInitStatement.

     ROSE_ASSERT(result != NULL);

  // DQ  (9/26/2012): I think we can assert this (fails test2004_77.C).
  // ROSE_ASSERT(init_stmt != NULL);
  // ROSE_ASSERT(test      != NULL);
  // ROSE_ASSERT(increment != NULL);
  // ROSE_ASSERT(loop_body != NULL);

#if 0
  // DQ (9/26/2012): It might be that these should always be set.
#if 0
     if (result->get_for_init_stmt() == NULL)
          result->set_for_init_stmt(init_stmt);
     if (result->get_test() == NULL)
          result->set_test(test);
     if (result->get_increment() == NULL)
          result->set_increment(increment);
     if (result->get_loop_body() == NULL)
          result->set_loop_body(loop_body);
#else
  // This test might make it impossible for us to use this function in SgForStatement* SageBuilder::buildForStatement_nfi()
     if (result->get_for_init_stmt() != NULL)
        {
          if (init_stmt != result->get_for_init_stmt())
             {
               delete result->get_for_init_stmt();
               result->set_for_init_stmt(NULL);
             }
       // delete result->get_for_init_stmt();
       // result->set_for_init_stmt(NULL);
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
          printf ("Error: In SgForStatement, SgForInitStatement is already set result->get_for_init_stmt() = %p = %s \n",result->get_for_init_stmt(),result->get_for_init_stmt()->class_name().c_str());
#endif
       // ROSE_ASSERT(false);
        }
#endif
#endif

  // DQ (11/4/2012): I have added support for remove existing subtrees if they are different from what is provided as input.
     if (result->get_for_init_stmt() != NULL && init_stmt != result->get_for_init_stmt())
        {
          delete result->get_for_init_stmt();
          result->set_for_init_stmt(NULL);
        }

     if (result->get_test() != NULL && test != result->get_test())
        {
          delete result->get_test();
          result->set_test(NULL);
        }

     if (result->get_increment() != NULL && increment != result->get_increment())
        {
          delete result->get_increment();
          result->set_increment(NULL);
        }

     if (result->get_loop_body() != NULL && loop_body != result->get_loop_body())
        {
          delete result->get_loop_body();
          result->set_loop_body(NULL);
        }

     if (result->get_else_body() != NULL && else_body != result->get_else_body())
        {
          delete result->get_else_body();
          result->set_else_body(NULL);
        }

     result->set_for_init_stmt(init_stmt);
     result->set_test(test);
     result->set_increment(increment);
     result->set_loop_body(loop_body);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
          result->setCaseInsensitive(true);

     setOneSourcePositionNull(result);
     if (test)           test->set_parent(result);
     if (loop_body) loop_body->set_parent(result);
     if (increment) increment->set_parent(result);
     if (init_stmt) init_stmt->set_parent(result);

     if (else_body) init_stmt->set_parent(result);
     result->set_else_body(else_body);

     ROSE_ASSERT(result->get_for_init_stmt() != NULL);
     ROSE_ASSERT(result->get_test()          != NULL);
     ROSE_ASSERT(result->get_increment()     != NULL);
     ROSE_ASSERT(result->get_loop_body()     != NULL);
   }


// DQ (3/26/2018): Adding support for range based for statement.
// SgRangeBasedForStatement* SageBuilder::buildRangeBasedForStatement_nfi(SgVariableDeclaration* initializer, SgExpression* range, SgStatement* body)
SgRangeBasedForStatement*
SageBuilder::buildRangeBasedForStatement_nfi(
     SgVariableDeclaration* initializer,          SgVariableDeclaration* range,
     SgVariableDeclaration* begin_declaration,    SgVariableDeclaration* end_declaration,
     SgExpression*          not_equal_expression, SgExpression*          increment_expression,
     SgStatement*           body)
   {
  // DQ (6/26/2019): Commented these out so that we could build the SgRangeBasedForStatement before
  // building the children, since the scope of the chldren will be the SgRangeBasedForStatement and
  // it must exist before the children are constructed.
  // ROSE_ASSERT(initializer != NULL);
  // ROSE_ASSERT(range       != NULL);

  // DQ (6/26/2019): This was already commented out.
  // ROSE_ASSERT(body        != NULL);

     SgRangeBasedForStatement* result = new SgRangeBasedForStatement(initializer, range, begin_declaration, end_declaration, not_equal_expression, increment_expression, body);
     ROSE_ASSERT(result != NULL);

     setOneSourcePositionNull(result);

     if (initializer != NULL) initializer->set_parent(result);
     if (range       != NULL) range->set_parent(result);

     if (begin_declaration != NULL) begin_declaration->set_parent(result);
     if (end_declaration   != NULL) end_declaration->set_parent(result);

     if (not_equal_expression != NULL) not_equal_expression->set_parent(result);
     if (increment_expression != NULL) increment_expression->set_parent(result);

     if (body        != NULL) body->set_parent(result);

     return result;
   }


void
SageBuilder::buildDoWhileStatement_nfi(SgDoWhileStmt* result, SgStatement * body, SgStatement * condition)
   {
  // DQ (3/22/2014): This function has been built to support reusing an existing SgDoWhileStatement
  // that may have been built and pushed onto the stack as part of a top-down construction of the AST.
  // It is required in the EDG 4.8 useage because of a change from EDG 4.7 to 4.8 in how blocks are
  // handled (end-of-construct entries).

     ROSE_ASSERT(result    != NULL);
     ROSE_ASSERT(body      != NULL);
     ROSE_ASSERT(condition != NULL);

     ROSE_ASSERT(result->get_body()      == NULL);
     ROSE_ASSERT(result->get_condition() == NULL);

     result->set_body(body);
     result->set_condition(condition);

     body->set_parent(result);
     condition->set_parent(result);

     setOneSourcePositionNull(result);

     ROSE_ASSERT(result->get_body()      != NULL);
     ROSE_ASSERT(result->get_condition() != NULL);

     ROSE_ASSERT(result->get_body()->get_parent()      == result);
     ROSE_ASSERT(result->get_condition()->get_parent() == result);

#if 0
     printf ("Exiting at the base of SageBuilder::buildDoWhileStatement_nfi() \n");
     ROSE_ASSERT(false);
#endif
   }



//! Based on the contribution from Pradeep Srinivasa@ LANL
//Liao, 8/27/2008
SgUpcForAllStatement * SageBuilder::buildUpcForAllStatement_nfi(SgStatement* initialize_stmt, SgStatement * test, SgExpression * increment, SgExpression* affinity, SgStatement * loop_body)
{
  SgUpcForAllStatement * result = new SgUpcForAllStatement(test,increment, affinity, loop_body);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  if (test) test->set_parent(result);
  if (loop_body) loop_body->set_parent(result);
  if (increment) increment->set_parent(result);
  if (affinity) affinity->set_parent(result);

  if (initialize_stmt != NULL) {
    SgForInitStatement* init_stmt = result->get_for_init_stmt();
    ROSE_ASSERT(init_stmt);
    setOneSourcePositionNull(init_stmt);
    init_stmt->append_init_stmt(initialize_stmt);
    initialize_stmt->set_parent(init_stmt);
  }

  return result;
}


SgUpcForAllStatement * SageBuilder::buildUpcForAllStatement_nfi(SgForInitStatement * init_stmt, SgStatement * test, SgExpression * increment, SgExpression* affinity, SgStatement * loop_body)
{
  SgUpcForAllStatement * result = new SgUpcForAllStatement(init_stmt, test, increment, affinity, loop_body);
  ROSE_ASSERT(result);

 // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
  if (symbol_table_case_insensitive_semantics == true)
       result->setCaseInsensitive(true);

  setOneSourcePositionNull(result);
  if (test)           test->set_parent(result);
  if (loop_body) loop_body->set_parent(result);
  if (increment) increment->set_parent(result);
  if (affinity)   affinity->set_parent(result);
  if (init_stmt) init_stmt->set_parent(result);

  return result;
}

// DQ (3/3/2013): Added UPC specific build functions.
SgUpcNotifyStatement*
SageBuilder::buildUpcNotifyStatement_nfi(SgExpression* exp)
   {
     SgUpcNotifyStatement* result = new SgUpcNotifyStatement(exp);

     setOneSourcePositionNull(result);

     exp->set_parent(result);

     ROSE_ASSERT(exp->get_parent() != NULL);

     return result;
   }

SgUpcWaitStatement*
SageBuilder::buildUpcWaitStatement_nfi(SgExpression* exp)
   {
     SgUpcWaitStatement* result = new SgUpcWaitStatement(exp);

     setOneSourcePositionNull(result);

     exp->set_parent(result);

     ROSE_ASSERT(exp->get_parent() != NULL);

     return result;
   }

SgUpcBarrierStatement*
SageBuilder::buildUpcBarrierStatement_nfi(SgExpression* exp)
   {
     SgUpcBarrierStatement* result = new SgUpcBarrierStatement(exp);

     setOneSourcePositionNull(result);

     exp->set_parent(result);

     ROSE_ASSERT(exp->get_parent() != NULL);

     return result;
   }

SgUpcFenceStatement*
SageBuilder::buildUpcFenceStatement_nfi()
   {
     SgUpcFenceStatement* result = new SgUpcFenceStatement();

     setOneSourcePositionNull(result);

     return result;
   }




SgWhileStmt * SageBuilder::buildWhileStmt(SgStatement *  condition, SgStatement *body, SgStatement* else_body)
{
  ROSE_ASSERT(condition);
  ROSE_ASSERT(body);
  SgWhileStmt * result = new SgWhileStmt(condition,body);
  ROSE_ASSERT(result);

 // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
  if (symbol_table_case_insensitive_semantics == true)
       result->setCaseInsensitive(true);

  setOneSourcePositionForTransformation(result);
  condition->set_parent(result);
  body->set_parent(result);

// DQ (8/10/2011): This is added by Michael to support a Python specific feature.
  if (else_body != NULL) {
      result->set_else_body(else_body);
      else_body->set_parent(result);
  }

  return result;
}


SgWhileStmt*
SageBuilder::buildWhileStmt_nfi(SgStatement *  condition, SgStatement *body, SgStatement *else_body)
   {
     SgWhileStmt * result = new SgWhileStmt(condition,body);
     ROSE_ASSERT(result);

#if 0
  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
          result->setCaseInsensitive(true);

     setOneSourcePositionNull(result);
     if (condition) condition->set_parent(result);
     if (body) body->set_parent(result);

  // DQ (8/10/2011): This is added by Michael to support a Python specific feature.
     if (else_body != NULL)
        {
          result->set_else_body(else_body);
          else_body->set_parent(result);
        }
#else
  // DQ (2/15/2012): This function supports the case where in C++ the condition can include a variable declaration.
     initializeWhileStatement(result,condition,body,else_body);
#endif

     return result;
   }


SgWithStatement* SageBuilder::buildWithStatement(SgExpression* expr, SgStatement *body)
{
  ROSE_ASSERT(expr != NULL && body != NULL);
  SgWithStatement* result = new SgWithStatement(expr, body);
  expr->set_parent(result);
  body->set_parent(result);

  setOneSourcePositionForTransformation(result);
  return result;
}

SgWithStatement* SageBuilder::buildWithStatement_nfi(SgExpression* expr, SgStatement *body)
{
  ROSE_ASSERT(expr != NULL && body != NULL);
  SgWithStatement* result = new SgWithStatement(expr, body);
  expr->set_parent(result);
  body->set_parent(result);

  setOneSourcePositionNull(result);
  return result;
}

SgDoWhileStmt * SageBuilder::buildDoWhileStmt(SgStatement *  body, SgStatement *condition)
{
  ROSE_ASSERT(condition);
  ROSE_ASSERT(body);
  SgDoWhileStmt * result = new SgDoWhileStmt(body, condition);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  condition->set_parent(result);
  body->set_parent(result);
  return result;
}

SgDoWhileStmt * SageBuilder::buildDoWhileStmt_nfi(SgStatement *  body, SgStatement *condition)
{
  SgDoWhileStmt * result = new SgDoWhileStmt(body, condition);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  if (condition) condition->set_parent(result);
  if (body) body->set_parent(result);
  return result;
}

SgMatlabForStatement* SageBuilder::buildMatlabForStatement(SgExpression* loop_index, SgExpression* loop_range, SgBasicBlock* loop_body)
{
  SgMatlabForStatement* result = new SgMatlabForStatement(loop_index, loop_range, loop_body);
  SageInterface::setOneSourcePositionForTransformation(result);

  ROSE_ASSERT(result != NULL);

  loop_index->set_parent(result);
  loop_range->set_parent(result);
  loop_body->set_parent(result);
  return result;
}

SgBreakStmt * SageBuilder::buildBreakStmt()
{
  SgBreakStmt* result = new SgBreakStmt();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgBreakStmt * SageBuilder::buildBreakStmt_nfi()
{
  SgBreakStmt* result = new SgBreakStmt();
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgContinueStmt * SageBuilder::buildContinueStmt()
{
  SgContinueStmt* result = new SgContinueStmt();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgContinueStmt * SageBuilder::buildContinueStmt_nfi()
{
  SgContinueStmt* result = new SgContinueStmt();
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgPassStatement * SageBuilder::buildPassStatement()
{
  SgPassStatement* result = new SgPassStatement();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgPassStatement * SageBuilder::buildPassStatement_nfi()
{
  SgPassStatement* result = new SgPassStatement();
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

SgDeleteExp* SageBuilder::buildDeleteExp(SgExpression *target, bool is_array, bool need_global_specifier, SgFunctionDeclaration *deleteOperatorDeclaration)
{
    SgDeleteExp *result = new SgDeleteExp(target, is_array, need_global_specifier, deleteOperatorDeclaration);
    target->set_parent(result);
    setOneSourcePositionForTransformation(result);
    return result;
}

SgDeleteExp* SageBuilder::buildDeleteExp_nfi(SgExpression *target, bool is_array, bool need_global_specifier, SgFunctionDeclaration *deleteOperatorDeclaration)
{
    SgDeleteExp *result = new SgDeleteExp(target, is_array, need_global_specifier, deleteOperatorDeclaration);
    target->set_parent(result);
    setOneSourcePositionNull(result);
    return result;
}

SgAssertStmt* SageBuilder::buildAssertStmt(SgExpression* test)
{
  SgAssertStmt* result = new SgAssertStmt(test);
  ROSE_ASSERT(test != NULL);
  test->set_parent(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

// DQ (7/18/2011): Added support for SgJavaInstanceOfOp
//! This is part of Java specific operator support.
SgJavaInstanceOfOp* SageBuilder::buildJavaInstanceOfOp(SgExpression* exp, SgType* type)
   {
     SgType* exp_type = SgTypeBool::createType();

     SgJavaInstanceOfOp* result = new SgJavaInstanceOfOp(exp, type, exp_type);
     ROSE_ASSERT(result);
     if (exp != NULL)
        {
          exp->set_parent(result);
          markLhsValues(result);
        }

     setOneSourcePositionForTransformation(result);
     return result;
   }

SgAssertStmt* SageBuilder::buildAssertStmt(SgExpression* test, SgExpression* exceptionArgument)
{
  SgAssertStmt* result = new SgAssertStmt(test);
  ROSE_ASSERT(test != NULL);
  test->set_parent(result);
  if (exceptionArgument != NULL) {
      result -> set_exception_argument(exceptionArgument);
      exceptionArgument->set_parent(result);
  }
  setOneSourcePositionForTransformation(result);
  return result;
}

SgAssertStmt* SageBuilder::buildAssertStmt_nfi(SgExpression* test)
{
  SgAssertStmt* result = new SgAssertStmt(test);
  ROSE_ASSERT(test != NULL);
  test->set_parent(result);
  setOneSourcePositionNull(result);
  return result;
}

SgYieldExpression* SageBuilder::buildYieldExpression(SgExpression* value)
{
  ROSE_ASSERT(value != NULL);
  SgYieldExpression* result = new SgYieldExpression(value);
  value->set_parent(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgYieldExpression* SageBuilder::buildYieldExpression_nfi(SgExpression* value)
{
  ROSE_ASSERT(value != NULL);
  SgYieldExpression* result = new SgYieldExpression(value);
  value->set_parent(result);
  setOneSourcePositionNull(result);
  return result;
}

SgKeyDatumPair* SageBuilder::buildKeyDatumPair(SgExpression* key, SgExpression* datum)
{
    ROSE_ASSERT(key != NULL && datum != NULL);
    SgKeyDatumPair *result = new SgKeyDatumPair(key, datum);
    key->set_parent(result);
    datum->set_parent(result);
    setOneSourcePositionForTransformation(result);
    return result;
}

SgKeyDatumPair* SageBuilder::buildKeyDatumPair_nfi(SgExpression* key, SgExpression* datum)
{
    ROSE_ASSERT(key != NULL && datum != NULL);
    SgKeyDatumPair *result = new SgKeyDatumPair(key, datum);
    key->set_parent(result);
    datum->set_parent(result);
    setOneSourcePositionNull(result);
    return result;
}

SgDictionaryExp* SageBuilder::buildDictionaryExp(std::vector<SgKeyDatumPair*> pairs)
{
    SgDictionaryExp *result = new SgDictionaryExp();
    ROSE_ASSERT(result);
    for (size_t i = 0; i < pairs.size(); ++i)
        result->append_pair(pairs[i]);
    setOneSourcePositionForTransformation(result);
    return result;
}

SgDictionaryExp* SageBuilder::buildDictionaryExp_nfi(std::vector<SgKeyDatumPair*> pairs)
{
    SgDictionaryExp *result = new SgDictionaryExp();
    ROSE_ASSERT(result);
    for (size_t i = 0; i < pairs.size(); ++i)
        result->append_pair(pairs[i]);
    setOneSourcePositionNull(result);
    return result;
}

SgComprehension*
SageBuilder::buildComprehension(SgExpression *target, SgExpression *iter, SgExprListExp *ifs)
{
    ROSE_ASSERT(target != NULL);
    ROSE_ASSERT(iter != NULL);
    SgComprehension *result = new SgComprehension(target, iter, ifs);
    ROSE_ASSERT(result);

    target->set_parent(result);
    iter->set_parent(result);
    if (ifs != NULL) ifs->set_parent(result);

    setOneSourcePositionForTransformation(result);
    return result;
}

SgComprehension*
SageBuilder::buildComprehension_nfi(SgExpression *target, SgExpression *iter, SgExprListExp *ifs)
{
    ROSE_ASSERT(target != NULL);
    ROSE_ASSERT(iter != NULL);
    SgComprehension *result = new SgComprehension(target, iter, ifs);
    ROSE_ASSERT(result);
    target->set_parent(result);
    iter->set_parent(result);
    if (ifs != NULL) ifs->set_parent(result);
    setOneSourcePositionNull(result);
    return result;
}

SgListComprehension*
SageBuilder::buildListComprehension(SgExpression *elt, SgExprListExp *generators)
{
    ROSE_ASSERT(elt != NULL);
    ROSE_ASSERT(generators != NULL);
    SgListComprehension* result = new SgListComprehension(elt, generators);
    elt->set_parent(result);
    generators->set_parent(result);
    setOneSourcePositionNull(result);
    return result;
}

SgListComprehension*
SageBuilder::buildListComprehension_nfi(SgExpression *elt, SgExprListExp *generators)
{
    ROSE_ASSERT(elt != NULL);
    ROSE_ASSERT(generators != NULL);
    SgListComprehension* result = new SgListComprehension(elt, generators);
    elt->set_parent(result);
    generators->set_parent(result);
    setOneSourcePositionForTransformation(result);
    return result;
}

SgSetComprehension*
SageBuilder::buildSetComprehension(SgExpression *elt, SgExprListExp *generators)
{
    ROSE_ASSERT(elt != NULL);
    ROSE_ASSERT(generators != NULL);
    SgSetComprehension* result = new SgSetComprehension(elt, generators);
    elt->set_parent(result);
    generators->set_parent(result);
    setOneSourcePositionForTransformation(result);
    return result;
}

SgSetComprehension*
SageBuilder::buildSetComprehension_nfi(SgExpression *elt, SgExprListExp *generators)
{
    ROSE_ASSERT(elt != NULL);
    ROSE_ASSERT(generators != NULL);
    SgSetComprehension* result = new SgSetComprehension(elt, generators);
    elt->set_parent(result);
    generators->set_parent(result);
    setOneSourcePositionNull(result);
    return result;
}

SgDictionaryComprehension*
SageBuilder::buildDictionaryComprehension(SgKeyDatumPair *kd_pair, SgExprListExp *generators)
{
    ROSE_ASSERT(kd_pair != NULL);
    ROSE_ASSERT(generators != NULL);
    SgDictionaryComprehension* result = new SgDictionaryComprehension(kd_pair, generators);
    kd_pair->set_parent(result);
    generators->set_parent(result);
    setOneSourcePositionForTransformation(result);
    return result;
}

SgDictionaryComprehension*
SageBuilder::buildDictionaryComprehension_nfi(SgKeyDatumPair *kd_pair, SgExprListExp *generators)
{
    ROSE_ASSERT(kd_pair != NULL);
    ROSE_ASSERT(generators != NULL);
    SgDictionaryComprehension* result = new SgDictionaryComprehension(kd_pair, generators);
    kd_pair->set_parent(result);
    generators->set_parent(result);
    setOneSourcePositionNull(result);
    return result;
}

SgActualArgumentExpression*
SageBuilder::buildActualArgumentExpression(SgName arg_name, SgExpression* arg) {
    ROSE_ASSERT(arg != NULL);
    SgActualArgumentExpression* result = new SgActualArgumentExpression(arg_name, arg);
    arg->set_parent(result);
    setOneSourcePositionForTransformation(result);
    return result;
}

SgActualArgumentExpression*
SageBuilder::buildActualArgumentExpression_nfi(SgName arg_name, SgExpression* arg) {
    ROSE_ASSERT(arg != NULL);
    SgActualArgumentExpression* result = new SgActualArgumentExpression(arg_name, arg);
    arg->set_parent(result);
    setOneSourcePositionNull(result);
    return result;
}

SgPragmaDeclaration*
SageBuilder::buildPragmaDeclaration(const string& name, SgScopeStatement* scope)
   {
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

     SgPragma* pragma = new SgPragma(name);
     ROSE_ASSERT(pragma);

     setOneSourcePositionForTransformation(pragma);

     SgPragmaDeclaration* result = new SgPragmaDeclaration(pragma);
     ROSE_ASSERT(result);

     setOneSourcePositionForTransformation(result);

     result->set_definingDeclaration (result);
     result->set_firstNondefiningDeclaration(result);
     pragma->set_parent(result);

  // DQ (7/14/2012): Set the parent so that we can be consistent where possible (class declarations and
  // enum declaration can't have there parent set since they could be non-autonomous declarations).
     result->set_parent(topScopeStack());

    if (topScopeStack())
     ROSE_ASSERT(result->get_parent() != NULL);

     return result;
   }

//!Build SgPragma
SgPragma* SageBuilder::buildPragma(const std::string & name)
{
  SgPragma* result= new SgPragma(name);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}


SgEmptyDeclaration* SageBuilder::buildEmptyDeclaration()
   {
  // Build an empty declaration (useful for adding precission to comments and CPP handling under token-based unparsing).
     SgEmptyDeclaration* emptyDeclaration = new SgEmptyDeclaration();
     ROSE_ASSERT(emptyDeclaration != NULL);

     setOneSourcePositionForTransformation(emptyDeclaration);

     emptyDeclaration->set_definingDeclaration (emptyDeclaration);
     emptyDeclaration->set_firstNondefiningDeclaration(emptyDeclaration);

  // DQ (7/14/2012): Set the parent so that we can be consistent where possible (class declarations and
  // enum declaration can't have there parent set since they could be non-autonomous declarations).
     emptyDeclaration->set_parent(topScopeStack());

     if (topScopeStack() != NULL)
        {
          ROSE_ASSERT(emptyDeclaration->get_parent() != NULL);
        }

     return emptyDeclaration;
   }


SgBasicBlock * SageBuilder::buildBasicBlock(SgStatement * stmt1, SgStatement* stmt2, SgStatement* stmt3, SgStatement* stmt4, SgStatement* stmt5, SgStatement* stmt6, SgStatement* stmt7, SgStatement* stmt8, SgStatement* stmt9, SgStatement* stmt10)
{
  SgBasicBlock* result = new SgBasicBlock();
  ROSE_ASSERT(result);

// DQ (11/28/2010): Added specification of case insensitivity for Fortran.
  if (symbol_table_case_insensitive_semantics == true)
       result->setCaseInsensitive(true);

  setOneSourcePositionForTransformation(result);
  if (stmt1) SageInterface::appendStatement(stmt1, result);
  if (stmt2) SageInterface::appendStatement(stmt2, result);
  if (stmt3) SageInterface::appendStatement(stmt3, result);
  if (stmt4) SageInterface::appendStatement(stmt4, result);
  if (stmt5) SageInterface::appendStatement(stmt5, result);
  if (stmt6) SageInterface::appendStatement(stmt6, result);
  if (stmt7) SageInterface::appendStatement(stmt7, result);
  if (stmt8) SageInterface::appendStatement(stmt8, result);
  if (stmt9) SageInterface::appendStatement(stmt9, result);
  if (stmt10) SageInterface::appendStatement(stmt10, result);

  return result;
}

SgBasicBlock * SageBuilder::buildBasicBlock_nfi()
   {
     SgBasicBlock* result = new SgBasicBlock();
     ROSE_ASSERT(result);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
        {
          result->setCaseInsensitive(true);
        }

     setOneSourcePositionNull(result);

#if 0
     printf ("In buildBasicBlock_nfi(): returning result = %p \n",result);
#endif

     return result;
   }

SgBasicBlock* SageBuilder::buildBasicBlock_nfi(const vector<SgStatement*>& stmts)
   {
     SgBasicBlock* result = buildBasicBlock_nfi();
     appendStatementList(stmts, result);

#if 0
     printf ("In buildBasicBlock_nfi(const vector<SgStatement*>& stmts): returning result = %p \n",result);
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     return result;
   }

SgGotoStatement *
SageBuilder::buildGotoStatement(SgLabelStatement *  label)
{
  SgGotoStatement* result = new SgGotoStatement(label);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

SgGotoStatement *
SageBuilder::buildGotoStatement(SgLabelSymbol*  symbol)
{
  SgGotoStatement* result = NULL;
  ROSE_ASSERT (symbol != NULL);
  if (SageInterface::is_Fortran_language())
  {  // Fortran case
    result = buildGotoStatement((SgLabelStatement *)NULL);
    SgLabelRefExp* l_exp = buildLabelRefExp(symbol);
    l_exp->set_parent(result);
    result->set_label_expression(l_exp);
  }
  else  // C/C++ case
  {
    SgLabelStatement* l_stmt = isSgLabelStatement(symbol->get_declaration());
    ROSE_ASSERT (l_stmt != NULL);
    result = buildGotoStatement(l_stmt);
  }
  ROSE_ASSERT(result);
  return result;
}

SgGotoStatement *
SageBuilder::buildGotoStatement_nfi(SgLabelStatement *  label)
{
  SgGotoStatement* result = new SgGotoStatement(label);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

// DQ (11/22/2017): Added support for computed code goto as defined by GNU C/C++ extension.
SgGotoStatement *
SageBuilder::buildGotoStatement_nfi(SgExpression*  label_expression)
   {
     SgLabelStatement* label = NULL;
     SgGotoStatement* result = new SgGotoStatement(label);
     result->set_selector_expression(label_expression);
     ROSE_ASSERT(result);
     setOneSourcePositionNull(result);
     return result;
   }

//! Build a return statement
SgReturnStmt* SageBuilder::buildReturnStmt(SgExpression* expression /* = NULL */)
{
  // Liao 2/6/2013. We no longer allow NULL express pointer. Use SgNullExpression instead.
  // Rasmussen (4/27/18): The expression argument to the builder function is optional
  // (NULL is allowed).  What is not allowed is constructing an SgReturnStmt with a NULL
  // expression argument.
  if (expression == NULL)
  {
     expression = buildNullExpression();
  }
  SgReturnStmt * result = new SgReturnStmt(expression);
  ROSE_ASSERT(result);
  if (expression != NULL) expression->set_parent(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build a return statement
SgReturnStmt* SageBuilder::buildReturnStmt_nfi(SgExpression* expression /* = NULL */)
{
  SgReturnStmt * result = new SgReturnStmt(expression);
  ROSE_ASSERT(result);
  if (expression != NULL) expression->set_parent(result);
  setOneSourcePositionNull(result);
  return result;
}

SgCaseOptionStmt * SageBuilder::buildCaseOptionStmt( SgExpression * key,SgStatement *body)
{
  SgCaseOptionStmt* result = new SgCaseOptionStmt(key,body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  if (key) key->set_parent(result);
  if (body) body->set_parent(result);
  return result;
}

SgCaseOptionStmt * SageBuilder::buildCaseOptionStmt_nfi( SgExpression * key,SgStatement *body)
{
  SgCaseOptionStmt* result = new SgCaseOptionStmt(key,body);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  if (key) key->set_parent(result);
  if (body) body->set_parent(result);
  return result;
}

SgDefaultOptionStmt * SageBuilder::buildDefaultOptionStmt( SgStatement *body)
{
  SgDefaultOptionStmt* result = new SgDefaultOptionStmt(body);
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  if (body) body->set_parent(result);
  return result;
}

SgDefaultOptionStmt * SageBuilder::buildDefaultOptionStmt_nfi( SgStatement *body)
{
  SgDefaultOptionStmt* result = new SgDefaultOptionStmt(body);
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  if (body) body->set_parent(result);
  return result;
}

SgSwitchStatement* SageBuilder::buildSwitchStatement(SgStatement *item_selector,SgStatement *body)
{
  SgSwitchStatement* result = new SgSwitchStatement(item_selector,body);
  ROSE_ASSERT(result);

 // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
  if (symbol_table_case_insensitive_semantics == true)
       result->setCaseInsensitive(true);

  setOneSourcePositionForTransformation(result);
  if (item_selector) item_selector->set_parent(result);
  if (body) body->set_parent(result);
  return result;
}

SgSwitchStatement*
SageBuilder::buildSwitchStatement_nfi(SgStatement *item_selector,SgStatement *body)
   {
     SgSwitchStatement* result = new SgSwitchStatement(item_selector,body);
     ROSE_ASSERT(result);

#if 0
  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
          result->setCaseInsensitive(true);

     setOneSourcePositionNull(result);
     if (item_selector) item_selector->set_parent(result);
     if (body) body->set_parent(result);
#else
  // DQ (2/15/2012): Modified to handle C++ case where variable declarations are allowed in the condition.
     initializeSwitchStatement(result,item_selector,body);
#endif

     return result;
   }

//! Build a NULL statement
SgNullStatement* SageBuilder::buildNullStatement()
{
  SgNullStatement* result = NULL;
  result = new SgNullStatement();
  ROSE_ASSERT(result);
  setOneSourcePositionForTransformation(result);
  return result;
}

//! Build a NULL statement
SgNullStatement* SageBuilder::buildNullStatement_nfi()
{
  SgNullStatement* result = NULL;
  result = new SgNullStatement();
  ROSE_ASSERT(result);
  setOneSourcePositionNull(result);
  return result;
}

//! Build an exec stmt
SgExecStatement* SageBuilder::buildExecStatement(SgExpression* executable,
                                                 SgExpression* globals,
                                                 SgExpression* locals) {
    if (locals != NULL && globals == NULL)
        ROSE_ASSERT(!"buildExecStatement with non-NULL locals requires non-NULL globals");
    ROSE_ASSERT(executable != NULL);

    SgExecStatement* result = new SgExecStatement(executable, globals, locals);
    executable->set_parent(result);
    if (globals != NULL) globals->set_parent(result);
    if (locals != NULL) locals->set_parent(result);

    setOneSourcePositionForTransformation(result);
    return result;
}

//! Build an exec stmt
SgExecStatement* SageBuilder::buildExecStatement_nfi(SgExpression* executable,
                                                     SgExpression* globals,
                                                     SgExpression* locals) {
    if (locals != NULL && globals == NULL)
        ROSE_ASSERT(!"buildExecStatement with non-NULL locals requires non-NULL globals");
    ROSE_ASSERT(executable != NULL);

    SgExecStatement* result = new SgExecStatement(executable, globals, locals);
    executable->set_parent(result);
    if (globals != NULL) globals->set_parent(result);
    if (locals != NULL) locals->set_parent(result);

    setOneSourcePositionNull(result);
    return result;
}

// MH (6/10/2014): Added async support
SgAsyncStmt* SageBuilder::buildAsyncStmt(SgBasicBlock *body)
{
       ROSE_ASSERT(body != NULL);
       SgAsyncStmt *async_stmt = new SgAsyncStmt(body);
       ROSE_ASSERT(async_stmt);
       body->set_parent(async_stmt);
       setOneSourcePositionForTransformation(async_stmt);

       return async_stmt;
}

// MH (6/11/2014): Added finish support
SgFinishStmt* SageBuilder::buildFinishStmt(SgBasicBlock *body)
{
       ROSE_ASSERT(body != NULL);
       SgFinishStmt *finish_stmt = new SgFinishStmt(body);
       ROSE_ASSERT(finish_stmt);
       body->set_parent(finish_stmt);
       setOneSourcePositionForTransformation(finish_stmt);

       return finish_stmt;
}

// MH (6/11/2014): Added at support
SgAtStmt* SageBuilder::buildAtStmt(SgExpression *expression, SgBasicBlock *body)
{
       ROSE_ASSERT(expression);
       ROSE_ASSERT(body);
       SgAtStmt *at_stmt = new SgAtStmt(expression, body);
       SageInterface::setSourcePosition(at_stmt);
       expression->set_parent(at_stmt);
       body->set_parent(at_stmt);

       return at_stmt;
}

// MH (11/12/2014): Added atomic support
SgAtomicStmt* SageBuilder::buildAtomicStmt(SgBasicBlock *body)
{
       ROSE_ASSERT(body != NULL);
       SgAtomicStmt *atomic_stmt = new SgAtomicStmt(body);
       ROSE_ASSERT(atomic_stmt);
       body->set_parent(atomic_stmt);
       setOneSourcePositionForTransformation(atomic_stmt);

       return atomic_stmt;
}


SgWhenStmt* SageBuilder::buildWhenStmt(SgExpression *expression, SgBasicBlock *body)
{
       ROSE_ASSERT(expression);
       ROSE_ASSERT(body);
       SgWhenStmt *when_stmt = new SgWhenStmt(expression, body);
       SageInterface::setSourcePosition(when_stmt);
       expression->set_parent(when_stmt);
       body->set_parent(when_stmt);

       return when_stmt;
}

// MH (9/14/2014): Added atexpr support
SgAtExp* SageBuilder::buildAtExp(SgExpression *expression, SgBasicBlock *body)
{
       ROSE_ASSERT(expression);
       ROSE_ASSERT(body);
       SgAtExp *at_exp = new SgAtExp(expression, body);
       SageInterface::setSourcePosition(at_exp);
       expression->set_parent(at_exp);
       body->set_parent(at_exp);

       return at_exp;
}

// MH (11/7/2014): Added finish expression support
SgFinishExp* SageBuilder::buildFinishExp(SgExpression *expression, SgBasicBlock *body)
{
       ROSE_ASSERT(expression);
       ROSE_ASSERT(body);
       SgFinishExp *finish_exp = new SgFinishExp(expression, body);
       SageInterface::setSourcePosition(finish_exp);
       expression->set_parent(finish_exp);
       body->set_parent(finish_exp);

       return finish_exp;
}

SgHereExp* SageBuilder::buildHereExpression()
{
       SgHereExp *here = new SgHereExp(NULL);
       return here;
}

SgDotDotExp* SageBuilder::buildDotDotExp()
{
       SgDotDotExp *dotdot = new SgDotDotExp(NULL);
       return dotdot;
}


//! Build a try statement
SgTryStmt* SageBuilder::buildTryStmt(SgStatement* body,
                                     SgCatchOptionStmt* catch0,
                                     SgCatchOptionStmt* catch1,
                                     SgCatchOptionStmt* catch2,
                                     SgCatchOptionStmt* catch3,
                                     SgCatchOptionStmt* catch4
                                     )
   {
     ROSE_ASSERT(body != NULL);
     SgTryStmt* try_stmt = new SgTryStmt(body);
     body->set_parent(try_stmt);

  // DQ (11/3/2012): Added setting default source position info.
     setSourcePosition(try_stmt);

     if (try_stmt->get_catch_statement_seq_root() != NULL)
        {
          if (try_stmt->get_catch_statement_seq_root()->get_startOfConstruct() == NULL)
             {
               ROSE_ASSERT(try_stmt->get_catch_statement_seq_root()->get_endOfConstruct() == NULL);
               setSourcePosition(try_stmt->get_catch_statement_seq_root());
             }

          ROSE_ASSERT(try_stmt->get_catch_statement_seq_root()->get_startOfConstruct() != NULL);
          ROSE_ASSERT(try_stmt->get_catch_statement_seq_root()->get_endOfConstruct()   != NULL);
        }

     if (catch0 != NULL) try_stmt->append_catch_statement(catch0);
     if (catch1 != NULL) try_stmt->append_catch_statement(catch1);
     if (catch2 != NULL) try_stmt->append_catch_statement(catch2);
     if (catch3 != NULL) try_stmt->append_catch_statement(catch3);
     if (catch4 != NULL) try_stmt->append_catch_statement(catch4);

     return try_stmt;
   }


// charles4 09/16/2011
//! Build a try statement (used for Java)
SgTryStmt *SageBuilder::buildTryStmt(SgBasicBlock *try_body, SgBasicBlock *finally_body)
   {
    //
    // charles4 09/23/2011 - Note that when an SgTryStmt is allocated, its constructor
    // preallocates a SgCatchStementSeq for the field p_catch_statement_sequence_root.
    // So, although the method set_catch_statement_seq_root(catch_statement_sequence) is
    // available, it should not be used to set the catch_statement_sequence_root as that
    // would leave the one that was allocated by the constructor dangling!
    //
    ROSE_ASSERT(try_body != NULL);
    SgTryStmt* try_stmt = new SgTryStmt(try_body);
    try_body -> set_parent(try_stmt);

  // DQ (11/3/2012): Added setting default source position info.
     setSourcePosition(try_stmt);

    if (finally_body) {
        try_stmt -> set_finally_body(finally_body);
        finally_body -> set_parent(try_stmt);
   }

    return try_stmt;
}

// charles4 09/16/2011
// ! Build an initial sequence of Catch blocks containing 0 or 1 element.
SgCatchStatementSeq *SageBuilder::buildCatchStatementSeq(SgCatchOptionStmt *catch_option_stmt) {
    SgCatchStatementSeq *catch_statement_sequence = new SgCatchStatementSeq();

  // DQ (11/3/2012): Added setting default source position info.
     setSourcePosition(catch_statement_sequence);

    if (catch_option_stmt) {
        catch_statement_sequence -> append_catch_statement(catch_option_stmt);
        catch_option_stmt -> set_parent(catch_statement_sequence);
    }

    return catch_statement_sequence;
}

// charles4 09/21/2011 - Make condition and body arguments optional.
//! Build a catch statement
SgCatchOptionStmt* SageBuilder::buildCatchOptionStmt(SgVariableDeclaration* condition, SgStatement* body) {
    SgCatchOptionStmt* result = new SgCatchOptionStmt(condition, body, /* SgTryStmt*= */ NULL);
    if (condition) condition->set_parent(result);
    if (body) body->set_parent(result);
    setOneSourcePositionForTransformation(result);
    return result;
}

SgJavaSynchronizedStatement *SageBuilder::buildJavaSynchronizedStatement(SgExpression *expression, SgBasicBlock *body)
{
  ROSE_ASSERT(expression);
  ROSE_ASSERT(body);
  SgJavaSynchronizedStatement *sync_stmt = new SgJavaSynchronizedStatement(expression, body);
  SageInterface::setSourcePosition(sync_stmt);

  expression->set_parent(sync_stmt);
  body->set_parent(sync_stmt);

  return sync_stmt;
}

SgJavaThrowStatement *SageBuilder::buildJavaThrowStatement(SgThrowOp *op)
{
  ROSE_ASSERT(op);
  SgJavaThrowStatement *throw_stmt = new SgJavaThrowStatement(op);
  ROSE_ASSERT(throw_stmt);

  op->set_parent(throw_stmt);

  return throw_stmt;
}

// DQ (9/3/2011): Changed the API to conform to the Java grammar.
// SgJavaForEachStatement *SageBuilder::buildJavaForEachStatement(SgInitializedName *variable, SgExpression *collection, SgStatement *body)
SgJavaForEachStatement *SageBuilder::buildJavaForEachStatement(SgVariableDeclaration *variable, SgExpression *collection, SgStatement *body)
{
  SgJavaForEachStatement *foreach_stmt = new SgJavaForEachStatement(variable, collection, body);
  ROSE_ASSERT(foreach_stmt);
  if (variable) variable -> set_parent(foreach_stmt);
  if (collection) collection -> set_parent(foreach_stmt);
  if (body) body -> set_parent(foreach_stmt);

  return foreach_stmt;
}

SgJavaLabelStatement *SageBuilder::buildJavaLabelStatement(const SgName& name,  SgStatement *stmt /* = NULL */)
{
    SgJavaLabelStatement *label_stmt = new SgJavaLabelStatement(name, stmt);
    ROSE_ASSERT(label_stmt);
    setOneSourcePositionForTransformation(label_stmt);

    if (stmt != NULL)
        stmt -> set_parent(label_stmt);

    SgJavaLabelSymbol *lsymbol = label_stmt -> lookup_java_label_symbol(name);
    if (! lsymbol) // Should be an Assertion - always true!
    {
        lsymbol= new SgJavaLabelSymbol(label_stmt);
        ROSE_ASSERT(lsymbol);
        label_stmt -> insert_symbol(lsymbol -> get_name(), lsymbol);
    }

    return label_stmt;
}

SgPythonPrintStmt*
SageBuilder::buildPythonPrintStmt(SgExpression* dest, SgExprListExp* values) {
    SgPythonPrintStmt* result = new SgPythonPrintStmt(dest, values);
    if (dest) dest->set_parent(result);
    if (values) values->set_parent(result);
    setOneSourcePositionForTransformation(result);
    return result;
}

SgPythonPrintStmt*
SageBuilder::buildPythonPrintStmt_nfi(SgExpression* dest, SgExprListExp* values) {
    SgPythonPrintStmt* result = new SgPythonPrintStmt(dest, values);
    if (dest) dest->set_parent(result);
    if (values) values->set_parent(result);
    setOneSourcePositionNull(result);
    return result;
}

SgPythonGlobalStmt*
SageBuilder::buildPythonGlobalStmt(SgInitializedNamePtrList& names) {
    SgPythonGlobalStmt* result = new SgPythonGlobalStmt();
    foreach (SgInitializedName* name, names)
        result->append_name(name);
    setOneSourcePositionForTransformation(result);
    return result;
}

SgPythonGlobalStmt*
SageBuilder::buildPythonGlobalStmt_nfi(SgInitializedNamePtrList& names) {
    SgPythonGlobalStmt* result = new SgPythonGlobalStmt();
    foreach (SgInitializedName* name, names)
        result->append_name(name);
    setOneSourcePositionNull(result);
    return result;
}

// DQ (4/30/2010): Added support for building asm statements.
//! Build an asm statement
SgAsmStmt* SageBuilder::buildAsmStatement( std::string s )
{
  SgAsmStmt* result = NULL;
  result = new SgAsmStmt();
  ROSE_ASSERT(result);
  result->set_assemblyCode(s);
  setOneSourcePositionForTransformation(result);
  return result;
}

// DQ (4/30/2010): Added support for building asm statements.
//! Build an asm statement
SgAsmStmt* SageBuilder::buildAsmStatement_nfi( std::string s )
{
  SgAsmStmt* result = NULL;
  result = new SgAsmStmt();
  ROSE_ASSERT(result);
  result->set_assemblyCode(s);
  setOneSourcePositionNull(result);
  return result;
}

SgAsmStmt*
SageBuilder::buildMultibyteNopStatement( int n )
   {
// Multi-byte NOP instructions.
// Note: I can't seem to get the memonic versions to work properly
#define NOP_1_BYTE_STRING "nop"
#define NOP_2_BYTE_STRING ".byte 0x66,0x90"
#define NOP_3_BYTE_STRING "nopl (%eax)"
#define NOP_4_BYTE_STRING "nopl 0x01(%eax)"
#define NOP_5_BYTE_STRING ".byte 0x0f,0x1f,0x44,0x00,0x00"
#define NOP_6_BYTE_STRING ".byte 0x66,0x0f,0x1f,0x44,0x00,0x00"
#define NOP_7_BYTE_STRING ".byte 0x0f,0x1f,0x80,0x00,0x00,0x00,0x00"
#define NOP_8_BYTE_STRING ".byte 0x0f,0x1f,0x84,0x00,0x00,0x00,0x00,0x00"
#define NOP_9_BYTE_STRING ".byte 0x66,0x0f,0x1f,0x84,0x00,0x00,0x00,0x00,0x00"

     ROSE_ASSERT(n > 0);

     SgAsmStmt* nopStatement = NULL;

     switch (n)
        {
          case 1: nopStatement = buildAsmStatement(NOP_1_BYTE_STRING); break;
          case 2: nopStatement = buildAsmStatement(NOP_2_BYTE_STRING); break;
          case 3: nopStatement = buildAsmStatement(NOP_3_BYTE_STRING); break;
          case 4: nopStatement = buildAsmStatement(NOP_4_BYTE_STRING); break;
          case 5: nopStatement = buildAsmStatement(NOP_5_BYTE_STRING); break;
          case 6: nopStatement = buildAsmStatement(NOP_6_BYTE_STRING); break;
          case 7: nopStatement = buildAsmStatement(NOP_7_BYTE_STRING); break;
          case 8: nopStatement = buildAsmStatement(NOP_8_BYTE_STRING); break;
          case 9: nopStatement = buildAsmStatement(NOP_9_BYTE_STRING); break;

          default:
             {
               printf ("Only supporting values of multi-byte nop's up to 9 bytes long. \n");
               ROSE_ASSERT(false);
             }
        }

     return nopStatement;
   }

SgStaticAssertionDeclaration* SageBuilder::buildStaticAssertionDeclaration(SgExpression* condition, const SgName & string_literal)
   {
  // DQ (7/25/2014): Adding support for C11 static assertions.

     ROSE_ASSERT(condition != NULL);

     SgStaticAssertionDeclaration* result = new SgStaticAssertionDeclaration(condition,string_literal);
     ROSE_ASSERT(result != NULL);

  // DQ (7/25/2014): It is enforced that at least the firstNondefiningDeclaration be set.
     ROSE_ASSERT(result->get_firstNondefiningDeclaration() == NULL);
     result->set_firstNondefiningDeclaration(result);
     ROSE_ASSERT(result->get_firstNondefiningDeclaration() != NULL);

     setOneSourcePositionForTransformation(result);

     return result;
   }


// DQ (8/17/2014): Adding support for Microsoft MSVC specific attributes.
SgMicrosoftAttributeDeclaration* SageBuilder::buildMicrosoftAttributeDeclaration (const SgName & attribute_string)
   {
     SgMicrosoftAttributeDeclaration* result = new SgMicrosoftAttributeDeclaration(attribute_string);
     ROSE_ASSERT(result != NULL);

  // DQ (8/17/2014): It is enforced that at least the firstNondefiningDeclaration be set.
     ROSE_ASSERT(result->get_firstNondefiningDeclaration() == NULL);
     result->set_firstNondefiningDeclaration(result);
     ROSE_ASSERT(result->get_firstNondefiningDeclaration() != NULL);

     setOneSourcePositionForTransformation(result);

     return result;
   }

//! Build a statement from an arbitrary string, used for irregular statements with macros, platform-specified attributes etc.
// This does not work properly since the global scope expects declaration statement, not just SgNullStatement
#if 0
SgStatement* SageBuilder::buildStatementFromString(std::string str)
{
  SgStatement* result = NULL;

    return result;

} //buildStatementFromString()
#endif

SgPointerType* SageBuilder::buildPointerType(SgType * base_type /*= NULL*/)
   {
  // DQ (7/26/2010): This needs to call the SgPointerType::createType() function so that we can properly abstract the creation of types into the type table.
  // printf ("ERROR: This function needs to call the SgPointerType::createType() function so that we can properly abstract the creation of types into the type table. \n");
  // ROSE_ASSERT(false);

  // DQ (7/29/2010): This function needs to call the SgPointerType::createType() function to support the new type table.
  // SgPointerType* result = new SgPointerType(base_type);
     if (isSgReferenceType (base_type))
     {
       cerr<<"Error in SageBuilder::buildPointerType(): trying to build a pointer to a reference type! This is not allowed in C++."<<endl;
       ROSE_ASSERT (false);
     }

     SgPointerType* result = SgPointerType::createType(base_type);
     ROSE_ASSERT(result != NULL);

     return result;
   }

SgReferenceType* SageBuilder::buildReferenceType(SgType * base_type /*= NULL*/)
   {
  // DQ (7/26/2010): This needs to call the SgReferenceType::createType() function so that we can properly abstract the creation of types into the type table.
  // printf ("ERROR: This function needs to call the SgReferenceType::createType() function so that we can properly abstract the creation of types into the type table. \n");
  // ROSE_ASSERT(false);

  // DQ (7/29/2010): This function needs to call the SgPointerType::createType() function to support the new type table.
  // SgReferenceType* result= new SgReferenceType(base_type);
     SgReferenceType* result = SgReferenceType::createType(base_type);
     ROSE_ASSERT(result != NULL);

     return result;
   }

SgRvalueReferenceType* SageBuilder::buildRvalueReferenceType(SgType* base_type /*= NULL*/)
   {
     ROSE_ASSERT(base_type != NULL);
     SgRvalueReferenceType* result = SgRvalueReferenceType::createType(base_type);
     ROSE_ASSERT(result != NULL);

     return result;
   }

SgDeclType* SageBuilder::buildDeclType ( SgExpression *base_expression, SgType* base_type )
   {
     ROSE_ASSERT(base_expression != NULL);
#if 0
     printf ("In SageBuilder::buildDeclType(): base_expression = %p = %s \n",base_expression,base_expression->class_name().c_str());
#endif

  // SgDeclType* result = SgDeclType::createType(base_expression);
     SgDeclType* result = NULL;
     if (isSgFunctionParameterRefExp(base_expression) != NULL)
        {
       // result = SgDeclType::createType(base_type);
           result = new SgDeclType(base_expression);
           result->set_base_type(base_type);
        }
       else
        {
          result = SgDeclType::createType(base_expression);
        }

     ROSE_ASSERT(result != NULL);

  // DQ (8/12/2014): Set the parent in the expression.
     base_expression->set_parent(result);

     return result;
   }

//! Build a GNU typeof operator
SgTypeOfType* SageBuilder::buildTypeOfType(SgExpression *base_expression, SgType* base_type)
   {
  // ROSE_ASSERT(base_expression != NULL);

#define DEBUG_TYPEOF_TYPE 0

#if DEBUG_TYPEOF_TYPE
     printf ("In SageBuilder::buildTypeOfType(): base_expression = %p = %s \n",base_expression,base_expression != NULL ? base_expression->class_name().c_str() : "NULL");
     printf ("   ------------------------------- base_type       = %p = %s \n",base_type,base_type != NULL ? base_type->class_name().c_str() : "NULL");
#endif

     SgTypeOfType* result = NULL;
     if (isSgFunctionParameterRefExp(base_expression) != NULL)
        {
#if DEBUG_TYPEOF_TYPE
          printf ("In SageBuilder::buildTypeOfType(): isSgFunctionParameterRefExp(base_expression) != NULL: calling new SgTypeOfType(base_expression,NULL) \n");
#endif
          result = new SgTypeOfType(base_expression,NULL);

       // DQ (3/28/2015): Testing for corruption in return value.
          ROSE_ASSERT(result != NULL);
#if DEBUG_TYPEOF_TYPE
          printf ("In buildTypeOfType(): test 1: result = %p = %s \n",result,result->class_name().c_str());
#endif
          result->set_base_type(base_type);
        }
       else
        {
       // result = SgTypeOfType::createType(base_expression);
          if (base_expression != NULL)
             {
#if DEBUG_TYPEOF_TYPE
               printf ("In SageBuilder::buildTypeOfType(): isSgFunctionParameterRefExp(base_expression) == NULL: base_expression != NULL: calling SgTypeOfType::createType(base_expression,NULL) \n");
#endif
               result = SgTypeOfType::createType(base_expression,NULL);

            // DQ (3/28/2015): Testing for corruption in return value.
               ROSE_ASSERT(result != NULL);
#if DEBUG_TYPEOF_TYPE
               printf ("In buildTypeOfType(): test 2: result = %p = %s \n",result,result->class_name().c_str());
#endif
             }
            else
             {
            // result = SgTypeOfType::createType((SgType*)NULL);
               ROSE_ASSERT(base_type != NULL);

#if DEBUG_TYPEOF_TYPE
               printf ("In SageBuilder::buildTypeOfType(): isSgFunctionParameterRefExp(base_expression) == NULL: base_expression == NULL: calling SgTypeOfType::createType(base_type,NULL) \n");
#endif
               result = SgTypeOfType::createType(base_type,NULL);

            // DQ (3/28/2015): Testing for corruption in return value.
               ROSE_ASSERT(result != NULL);

#if DEBUG_TYPEOF_TYPE
               printf ("In buildTypeOfType(): test 3: result = %p = %s \n",result,result->class_name().c_str());
#endif
            // result->set_base_type(base_type);
               if (result->get_base_type() != base_type)
                  {
                    ROSE_ASSERT(result->get_base_type() != NULL);
#if DEBUG_TYPEOF_TYPE
                    printf ("result->get_base_type() = %p = %s \n",result->get_base_type(),result->get_base_type()->class_name().c_str());
#endif
                    ROSE_ASSERT(base_type != NULL);
#if DEBUG_TYPEOF_TYPE
                    printf ("base_type               = %p = %s \n",base_type,base_type->class_name().c_str());
#endif
                  }
            // ROSE_ASSERT(result->get_base_type() == base_type);
             }
        }

     ROSE_ASSERT(result != NULL);

     if (base_expression != NULL)
        {
          base_expression->set_parent(result);
        }

  // DQ (3/28/2015): Testing for corruption in return value.
     ROSE_ASSERT(result != NULL);

#if DEBUG_TYPEOF_TYPE
     printf ("In buildTypeOfType(): test 4: result = %p = %s \n",result,result->class_name().c_str());
#endif

     return result;
   }



#if 0
// Liao, 8/16/2010, This function is being phased out. Please don't call this!!
SgModifierType* SageBuilder::buildModifierType(SgType * base_type /*= NULL*/)
   {
  // DQ (7/30/2010): Note that this is called by the outline test: tests/nonsmoke/functional/roseTests/astOutliningTests/moreTest3.cpp
  // DQ (7/28/2010): Now we want to make calling this function an error, the functions buildConst() will return SgModifierType objects instead.
     printf ("Error: this function SageBuilder::buildModifierType() should not be called! (call the buildConst() function (or whatever other function is required) directly \n");
   ROSE_ASSERT(false);
   // Liao, 8/13/2010, This function is being phased out. Please don't call this!!

  // DQ (7/26/2010): This needs to call the SgModifierType::createType() function so that we can properly abstract the creation of types into the type table.
     SgModifierType* result = new SgModifierType(base_type);
  // SgModifierType* result = SgModifierType::createType(base_type);
     ROSE_ASSERT(result != NULL);

  // DQ (7/28/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }
#endif

SgTypeBool * SageBuilder::buildBoolType() {
  SgTypeBool * result =SgTypeBool::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeNullptr* SageBuilder::buildNullptrType()
   {
     SgTypeNullptr * result = SgTypeNullptr::createType();
     ROSE_ASSERT(result);
     return result;
   }

SgTypeChar * SageBuilder::buildCharType()
{
  SgTypeChar * result =SgTypeChar::createType();
  ROSE_ASSERT(result);
  return result;
}

#if 0 // did not work, build##itemType would be expanded correctly
#define BUILD_SGTYPE_DEF(item) \
  SgType##item * SageBuilder::build##itemType() { \
  SgType##item * result =SgType##item::createType(); \
  ROSE_ASSERT(result); \
  return result; \
  }

  BUILD_SGTYPE_DEF(Bool)
  BUILD_SGTYPE_DEF(Char)
  BUILD_SGTYPE_DEF(Double)
  BUILD_SGTYPE_DEF(Float)
  BUILD_SGTYPE_DEF(Int)
  BUILD_SGTYPE_DEF(Long)
  BUILD_SGTYPE_DEF(LongDouble)
  BUILD_SGTYPE_DEF(LongLong)
  BUILD_SGTYPE_DEF(Short)
  BUILD_SGTYPE_DEF(Void)

  BUILD_SGTYPE_DEF(Wchar)

// DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
  BUILD_SGTYPE_DEF(Char16)
  BUILD_SGTYPE_DEF(Char32)

  BUILD_SGTYPE_DEF(SignedChar)
  BUILD_SGTYPE_DEF(SignedInt)
  BUILD_SGTYPE_DEF(SignedLong)
  BUILD_SGTYPE_DEF(SignedShort)
  BUILD_SGTYPE_DEF(UnsignedChar)
  BUILD_SGTYPE_DEF(UnsignedInt)
  BUILD_SGTYPE_DEF(UnsignedLong)
  BUILD_SGTYPE_DEF(UnsignedLongLong)
  BUILD_SGTYPE_DEF(UnsignedShort)
#undef BUILD_SGTYPE_DEF
#endif
SgTypeLongLong * SageBuilder::buildLongLongType()
{
  SgTypeLongLong * result =SgTypeLongLong::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeLongDouble * SageBuilder::buildLongDoubleType()
{
  SgTypeLongDouble * result =SgTypeLongDouble::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeUnsignedLongLong * SageBuilder::buildUnsignedLongLongType()
{
  SgTypeUnsignedLongLong * result =SgTypeUnsignedLongLong::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeUnsignedLong * SageBuilder::buildUnsignedLongType()
{
  SgTypeUnsignedLong * result = SgTypeUnsignedLong::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeUnsignedInt * SageBuilder::buildUnsignedIntType()
{
  SgTypeUnsignedInt * result = SgTypeUnsignedInt::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeSignedShort * SageBuilder::buildSignedShortType()
{
  SgTypeSignedShort * result = SgTypeSignedShort::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeFloat80*  SageBuilder::buildFloat80Type() {
  SgTypeFloat80 * result = SgTypeFloat80::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeFloat128* SageBuilder::buildFloat128Type() {
  SgTypeFloat128 * result = SgTypeFloat128::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeSignedInt * SageBuilder::buildSignedIntType()
{
  SgTypeSignedInt * result = SgTypeSignedInt::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeUnsignedChar * SageBuilder::buildUnsignedCharType()
{
  SgTypeUnsignedChar * result = SgTypeUnsignedChar::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeSignedLong * SageBuilder::buildSignedLongType()
{
  SgTypeSignedLong * result = SgTypeSignedLong::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeSignedLongLong * SageBuilder::buildSignedLongLongType()
{
  SgTypeSignedLongLong * result = SgTypeSignedLongLong::createType();
  ROSE_ASSERT(result);
  return result;
}

#if 1
SgTypeSigned128bitInteger* SageBuilder::buildSigned128bitIntegerType()
{
  SgTypeSigned128bitInteger* result = SgTypeSigned128bitInteger::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeUnsigned128bitInteger* SageBuilder::buildUnsigned128bitIntegerType()
{
  SgTypeUnsigned128bitInteger* result = SgTypeUnsigned128bitInteger::createType();
  ROSE_ASSERT(result);
  return result;
}
#endif


SgTypeWchar * SageBuilder::buildWcharType()
{
  SgTypeWchar * result =SgTypeWchar::createType();
  ROSE_ASSERT(result);
  return result;
}

// DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
SgTypeChar16 * SageBuilder::buildChar16Type()
{
  SgTypeChar16 * result =SgTypeChar16::createType();
  ROSE_ASSERT(result);
  return result;
}

// DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
SgTypeChar32 * SageBuilder::buildChar32Type()
{
  SgTypeChar32 * result =SgTypeChar32::createType();
  ROSE_ASSERT(result);
  return result;
}


SgTypeSignedChar * SageBuilder::buildSignedCharType()
{
  SgTypeSignedChar * result =SgTypeSignedChar::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeVoid * SageBuilder::buildVoidType()
{
  SgTypeVoid * result =SgTypeVoid::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeUnknown * SageBuilder::buildUnknownType()
{
  SgTypeUnknown * result =SgTypeUnknown::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeShort * SageBuilder::buildShortType()
{
  SgTypeShort * result =SgTypeShort::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeUnsignedShort * SageBuilder::buildUnsignedShortType()
{
  SgTypeUnsignedShort * result = SgTypeUnsignedShort::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeLong * SageBuilder::buildLongType()
{
  SgTypeLong * result =SgTypeLong::createType();
  ROSE_ASSERT(result);
  return result;
}

SgTypeString * SageBuilder::buildStringType()
   {
  // DQ (8/17/2010): This function needs to use a different API to handle a literal
  // value for the string size (typical) or an expression for the string size (rare).
  // For now we will make it an error to call this function.

  // SgTypeString * result =SgTypeString::createType();
     SgTypeString * result = NULL;
     ROSE_ASSERT(result != NULL);
     return result;
   }

// SgTypeString * SageBuilder::buildStringType( SgExpression* stringLengthExpression, size_t stringLengthLiteral )
SgTypeString * SageBuilder::buildStringType( SgExpression* stringLengthExpression )
   {
  // DQ (8/21/2010): This is a new API for this function.  This type is specific to Fortran use,
  // in C/C++ a string is just an array of char. We could have a consistant handling between
  // C/C++ and Fortrna, but we have just corrected the implementation in Fortran to use this IR
  // node and we would have to add such support to C/C++.  The current implementation reflects
  // the grammar of the two languages.

  // This function needs to use a different API to handle a literal
  // value for the string size (typical) or an expression for the string size (rare).

  // SgTypeString* result = SgTypeString::createType(stringLengthExpression,stringLengthLiteral);
     SgTypeString* result = SgTypeString::createType(stringLengthExpression);
     ROSE_ASSERT(result != NULL);
     return result;
   }

SgTypeInt * SageBuilder::buildIntType()
{
  SgTypeInt * result =SgTypeInt::createType();
  ROSE_ASSERT(result);
  return result;
}
SgTypeDouble * SageBuilder::buildDoubleType()
{
  SgTypeDouble * result =SgTypeDouble::createType();
  ROSE_ASSERT(result);
  return result;
}
SgTypeFloat * SageBuilder::buildFloatType()
{
  SgTypeFloat * result =SgTypeFloat::createType();
  ROSE_ASSERT(result);
  return result;
}

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
//! Build a modifier type.
SgModifierType* SageBuilder::buildModifierType(SgType* base_type /* = NULL*/)
   {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

  // DQ (7/28/2010): New (similar) approach using type table support.
     SgModifierType* result = new SgModifierType(base_type);
     ROSE_ASSERT(result != NULL);

  // DQ (3/10/2018): Adding assertion.
     ROSE_ASSERT(result != base_type);

#if 0
     printf ("In SageBuilder::buildModifierType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

  // DQ (7/28/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     SgModifierType* result2 = SgModifierType::insertModifierTypeIntoTypeTable(result);

     if (result != result2)
        {
#if 1
       // DQ (10/27/2015): This is the cause of a bug in the test2015_97.C (boost template problem).
          printf ("WARNING: In SageBuilder::buildModifierType(): using previously build SgModifierType from global type table: result2 = %p = %s \n",result2,result2->class_name().c_str());
#endif
#if 0
       // DQ (9/3/2012): While debugging let's skip calling delete so that the slot in the memory pool will not be reused.
          printf ("(debugging) In SageBuilder::buildModifierType(): Skipping delete of SgModifierType = %p = %s \n",result,result->class_name().c_str());
#else
          delete result;
#endif
        }

  // DQ (3/10/2018): Adding assertion.
     ROSE_ASSERT(result2 != base_type);

     return result2;
   }

  //! Build a constant type.
SgModifierType* SageBuilder::buildConstType(SgType* base_type /*=NULL*/)
   {
#if 0
  // DQ (7/28/2010): Old approach before type table support.
     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);
     result->get_typeModifier().get_constVolatileModifier().setConst();
     return result;
#else
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

  // DQ (7/28/2010): New (similar) approach using type table support.
     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);
     result->get_typeModifier().get_constVolatileModifier().setConst();

#if 0
     printf ("In SageBuilder::buildConstType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

  // DQ (7/28/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     SgModifierType *result2 = SgModifierType::insertModifierTypeIntoTypeTable(result);

     if (result != result2)
        {
#if 0
          printf ("In SageBuilder::buildConstType(result = %p) using type from type table (result2 = %p) deleting result = %p (skipping delete) \n",result,result2,result);
#endif

#if 0
       // DQ (9/3/2012): While debugging let's skip calling delete so that the slot in the memory pool will not be reused.
          printf ("(debugging) In SageBuilder::buildConstType(): Skipping delete of SgModifierType = %p = %s \n",result,result->class_name().c_str());
#else
          delete result;
#endif
        }

  // DQ (3/10/2018): Adding assertion.
     ROSE_ASSERT(result2 != base_type);

     return result2;
#endif
 }

// DQ (8/27/2010): Added Fortran specific support for types based on kind expressions.
SgModifierType*
SageBuilder::buildFortranKindType(SgType* base_type, SgExpression* kindExpression )
   {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result != NULL);

     result->set_type_kind(kindExpression);

#if 0
     printf ("In SageBuilder::buildFortranKindType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

     SgModifierType *result2 = SgModifierType::insertModifierTypeIntoTypeTable(result);

     if (result != result2)
        {
#if 0
       // DQ (9/3/2012): While debugging let's skip calling delete so that the slot in the memory pool will not be reused.
          printf ("(debugging) In SageBuilder::buildFortranKindType(): Skipping delete of SgModifierType = %p = %s \n",result,result->class_name().c_str());
#else
          delete result;
#endif
        }

     return result2;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a volatile type.
SgModifierType* SageBuilder::buildVolatileType(SgType* base_type /*=NULL*/)
   {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_constVolatileModifier().setVolatile();

#if 0
     printf ("In SageBuilder::buildVolatileType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

  // DQ (7/29/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     SgModifierType * result2 = SgModifierType::insertModifierTypeIntoTypeTable(result);
     if (result != result2)
        {
#if 0
       // DQ (9/3/2012): While debugging let's skip calling delete so that the slot in the memory pool will not be reused.
          printf ("(debugging) In SageBuilder::buildVolatileType(): Skipping delete of SgModifierType = %p = %s \n",result,result->class_name().c_str());
#else
          delete result;
#endif
        }

     return result2;
   }

// DQ (1/19/2019): Adding support for const volatile type (both together as another value).
//! Build a const volatile type.
SgModifierType* SageBuilder::buildConstVolatileType(SgType* base_type /*=NULL*/)
   {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_constVolatileModifier().setConst();
     result->get_typeModifier().get_constVolatileModifier().setVolatile();

#if 1
     printf ("In SageBuilder::buildConstVolatileType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

  // DQ (7/29/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     SgModifierType * result2 = SgModifierType::insertModifierTypeIntoTypeTable(result);
     if (result != result2)
        {
#if 0
       // DQ (9/3/2012): While debugging let's skip calling delete so that the slot in the memory pool will not be reused.
          printf ("(debugging) In SageBuilder::buildConstVolatileType(): Skipping delete of SgModifierType = %p = %s \n",result,result->class_name().c_str());
#else
          delete result;
#endif
        }

     return result2;
   }

string
generate_type_list (SgType* type)
   {
  // This function generates a list of types for each level of the type structure.
     string returnString;

     unsigned char bit_array = (SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_RVALUE_REFERENCE_TYPE | SgType::STRIP_POINTER_TYPE | SgType::STRIP_ARRAY_TYPE | SgType::STRIP_TYPEDEF_TYPE);

     SgType* currentType = type;

     SgModifierType*        modType     = NULL;
     SgPointerType*         pointType   = NULL;
     SgReferenceType*       refType     = NULL;
     SgRvalueReferenceType* rRefType     = NULL;
     SgArrayType*           arrayType   = NULL;
     SgTypedefType*         typedefType = NULL;

     while (currentType != NULL)
        {
          returnString += currentType->class_name();
#if 0
          printf ("In generate_type_list(): returnString = %s \n",returnString.c_str());
#endif
          if ( (bit_array & SgType::STRIP_MODIFIER_TYPE) && (modType = isSgModifierType(currentType)) )
             {
               currentType = modType->get_base_type();
             }
          else if ( (bit_array & SgType::STRIP_REFERENCE_TYPE) &&  (refType = isSgReferenceType(currentType)) )
             {
               currentType = refType->get_base_type();
             }
          else if ( (bit_array & SgType::STRIP_RVALUE_REFERENCE_TYPE) &&  (rRefType = isSgRvalueReferenceType(currentType)) )
             {
               currentType = rRefType->get_base_type();
             }
          else if ( (bit_array & SgType::STRIP_POINTER_TYPE) && (pointType = isSgPointerType(currentType)) )
             {
               currentType = pointType->get_base_type();
             }
          else if ( (bit_array & SgType::STRIP_ARRAY_TYPE) && (arrayType = isSgArrayType(currentType)) )
             {
               currentType = arrayType->get_base_type();
             }
          else  if ( (bit_array & SgType::STRIP_TYPEDEF_TYPE) && (typedefType = isSgTypedefType(currentType)) )
             {
               currentType = typedefType->get_base_type();
             }
          else
             {
               break;
             }

          if (type != NULL)
               returnString += " , ";
        }

     return returnString;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a restrict type.
SgModifierType* SageBuilder::buildRestrictType(SgType* base_type)
   {
     ROSE_ASSERT(base_type != NULL);

  // DQ (1/30/2014): We need to include typedefs here as well (see test2014_77.c).
  // DQ (9/28/2012): Added that the base type could be an array (see test2012_03.c (C test code)).
  // if (!isSgPointerType(base_type) && !isSgReferenceType(base_type))
  // if (!isSgPointerType(base_type) && !isSgReferenceType(base_type) && !isSgArrayType(base_type))
  // if (!isSgPointerType(base_type) && !isSgReferenceType(base_type) && !isSgArrayType(base_type) && !isSgTypedefType(base_type))
     if (!isSgPointerType(base_type) && !isSgReferenceType(base_type) && !isSgArrayType(base_type) && !isSgTypedefType(base_type) && !isSgModifierType(base_type))
        {
          printf("ERROR: Base type of restrict type must be on a pointer or reference or array or typedef type: base_type = %p = %s \n",base_type,base_type->class_name().c_str());
          printf ("  --- generate_type_list() = %s \n",generate_type_list(base_type).c_str());
          ROSE_ASSERT(false);
        }

     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().setRestrict();

#if 0
     printf ("In SageBuilder::buildRestrictType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

  // DQ (7/29/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     SgModifierType * result2 = SgModifierType::insertModifierTypeIntoTypeTable(result);
     if (result != result2)
        {
#if 0
       // DQ (9/3/2012): While debugging let's skip calling delete so that the slot in the memory pool will not be reused.
          printf ("(debugging) In SageBuilder::buildRestrictType(): Skipping delete of SgModifierType = %p = %s \n",result,result->class_name().c_str());
#else
          delete result;
#endif
        }

     return result2;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC strict type.
SgModifierType* SageBuilder::buildUpcStrictType(SgType* base_type /*=NULL*/)
   {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_modifier(SgUPC_AccessModifier::e_upc_strict);

#if 0
     printf ("In SageBuilder::buildUpcStrictType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

  // DQ (7/29/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     SgModifierType *result2 = SgModifierType::insertModifierTypeIntoTypeTable(result);
     if (result != result2)
        {
#if 0
       // DQ (9/3/2012): While debugging let's skip calling delete so that the slot in the memory pool will not be reused.
          printf ("(debugging) In SageBuilder::buildUpcStrictType(): Skipping delete of SgModifierType = %p = %s \n",result,result->class_name().c_str());
#else
          delete result;
#endif
        }

     return result2;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC relaxed type.
SgModifierType* SageBuilder::buildUpcRelaxedType(SgType* base_type /*=NULL*/)
   {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_modifier(SgUPC_AccessModifier::e_upc_relaxed);

#if 0
     printf ("In SageBuilder::buildUpcRelaxedType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

  // DQ (7/29/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     SgModifierType * result2 = SgModifierType::insertModifierTypeIntoTypeTable(result);
     if (result != result2)
        {
#if 0
       // DQ (9/3/2012): While debugging let's skip calling delete so that the slot in the memory pool will not be reused.
          printf ("(debugging) In SageBuilder::buildUpcRelaxedType(): Skipping delete of SgModifierType = %p = %s \n",result,result->class_name().c_str());
#else
          delete result;
#endif
        }

     return result2;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC shared type.
SgModifierType* SageBuilder::buildUpcSharedType(SgType* base_type /*=NULL*/, long layout /*= -1*/)
   {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

     SgModifierType *result = new SgModifierType(base_type);
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_isShared(true);

  // DQ (7/29/2010): Modified to use new input parameter.
  // result->get_typeModifier().get_upcModifier().set_layout(-1); // No layout ("shared" without a block size)
     result->get_typeModifier().get_upcModifier().set_layout(layout); // No layout ("shared" without a block size)

#if 0
     printf ("In SageBuilder::buildUpcSharedType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

  // DQ (7/29/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     SgModifierType * result2 = SgModifierType::insertModifierTypeIntoTypeTable(result);
     if (result != result2)
        {
#if 0
       // DQ (9/3/2012): While debugging let's skip calling delete so that the slot in the memory pool will not be reused.
          printf ("(debugging) In SageBuilder::buildUpcSharedType(): Skipping delete of SgModifierType = %p = %s \n",result,result->class_name().c_str());
#else
          delete result;
#endif
        }

     return result2;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC shared[] type.
SgModifierType* SageBuilder::buildUpcBlockIndefiniteType(SgType* base_type /*=NULL*/)
   {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

     SgModifierType *result = isSgModifierType(buildUpcSharedType(base_type));
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_layout(0); // [] layout

#if 0
     printf ("In SageBuilder::buildUpcBlockIndefiniteType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

  // DQ (7/29/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC shared[*] type.
SgModifierType* SageBuilder::buildUpcBlockStarType(SgType* base_type /*=NULL*/)
   {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

     SgModifierType *result = isSgModifierType(buildUpcSharedType(base_type));
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_layout(-2); // [*] layout

#if 0
     printf ("In SageBuilder::buildUpcBlockStarType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

  // DQ (7/29/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }

// DQ (7/29/2010): Changed return type from SgType to SgModifierType
  //! Build a UPC shared[n] type.
SgModifierType* SageBuilder::buildUpcBlockNumberType(SgType* base_type, long block_factor)
   {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

     SgModifierType *result = isSgModifierType(buildUpcSharedType(base_type));
     ROSE_ASSERT(result!=NULL);

     result->get_typeModifier().get_upcModifier().set_layout(block_factor); // [block_factor] layout

#if 0
     printf ("In SageBuilder::buildUpcBlockNumberType(): Building a SgModifierType: result = %p base_type = %p = %s \n",result,base_type,base_type->class_name().c_str());
#endif

  // DQ (7/29/2010): Insert result type into type table and return it, or
  // replace the result type, if already available in the type table, with
  // the type from type table.
     result = SgModifierType::insertModifierTypeIntoTypeTable(result);

     return result;
   }



  //! Build a complex type.
SgTypeComplex* SageBuilder::buildComplexType(SgType* base_type /*=NULL*/)
 {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

   SgTypeComplex *result = new SgTypeComplex(base_type);
   ROSE_ASSERT(result!=NULL);
   return result;
 }

  //! Build an imaginary type.
SgTypeImaginary* SageBuilder::buildImaginaryType(SgType* base_type /*=NULL*/)
 {
  // DQ (9/3/2012): Added assertion.
     ROSE_ASSERT(base_type != NULL);

   SgTypeImaginary *result = new SgTypeImaginary(base_type);
   ROSE_ASSERT(result!=NULL);
   return result;
 }

//! Build a Matrix Type for Matlab
SgTypeMatrix* SageBuilder::buildMatrixType()
{
  SgTypeMatrix *result = new SgTypeMatrix();
  ROSE_ASSERT(result != NULL);
  return result;
}

//! Build a type that holds multiple types. Used to represent the return type of a matlab function when it returns multiple variables of different types
SgTypeTuple* SageBuilder::buildTupleType(SgType *t1, SgType *t2, SgType *t3, SgType *t4, SgType *t5, SgType *t6, SgType *t7, SgType *t8, SgType *t9, SgType *t10)
{
  SgTypeTuple *result = new SgTypeTuple();
  ROSE_ASSERT(result != NULL);

  if(t1) result->append_type(t1);
  if(t2) result->append_type(t2);
  if(t3) result->append_type(t3);
  if(t4) result->append_type(t4);
  if(t5) result->append_type(t5);
  if(t6) result->append_type(t6);
  if(t7) result->append_type(t7);
  if(t8) result->append_type(t8);
  if(t9) result->append_type(t9);
  if(t10) result->append_type(t10);

  SageInterface::setOneSourcePositionForTransformation(result);

  return result;
}

SgRangeExp* SageBuilder::buildRangeExp(SgExpression *start)
{
  SgRangeExp *result = new SgRangeExp();
  SageInterface::setOneSourcePositionForTransformation(result);
  ROSE_ASSERT(result != NULL);

  result->append(start);
  return result;
}

SgMatrixExp* SageBuilder::buildMatrixExp(SgExprListExp *firstRow)
{
  SgMatrixExp *result = new SgMatrixExp();
  SageInterface::setOneSourcePositionForTransformation(result);

  result->append_expression(firstRow);
  ROSE_ASSERT(result != NULL);

  return result;
}

SgMagicColonExp* SageBuilder::buildMagicColonExp()
{
  SgMagicColonExp *result = new SgMagicColonExp();
  SageInterface::setOneSourcePositionForTransformation(result);

  ROSE_ASSERT(result != NULL);

  return result;
}

//! Build a const/volatile type qualifier
SgConstVolatileModifier * SageBuilder::buildConstVolatileModifier (SgConstVolatileModifier::cv_modifier_enum mtype/*=SgConstVolatileModifier::e_unknown*/)
{
  SgConstVolatileModifier * result = NULL;
  result = new SgConstVolatileModifier();
  ROSE_ASSERT (result != NULL);
  result->set_modifier (mtype);

  return result;
}

//! Build lambda expression
SgLambdaRefExp*
SageBuilder::buildLambdaRefExp(SgType* return_type, SgFunctionParameterList* params, SgScopeStatement* scope)
   {
  // SgFunctionDeclaration* func_decl = buildDefiningFunctionDeclaration("__rose__lambda__",return_type,params,scope,NULL,NULL);
     SgFunctionDeclaration* func_decl = buildDefiningFunctionDeclaration("__rose__lambda__",return_type,params,scope,NULL,false,NULL,NULL);

     SgLambdaRefExp* result = new SgLambdaRefExp(func_decl);
     func_decl->set_parent(result);

     setOneSourcePositionForTransformation(result);

     return result;
   }

SgTypeExpression*
SageBuilder::buildTypeExpression(SgType *type)
   {
     SgTypeExpression *expr = new SgTypeExpression(type);
     SageInterface::setSourcePosition(expr);
     return expr;
   }

// DQ (8/11/2014): Added support for C++11 decltype used in new function return syntax.
SgFunctionParameterRefExp*
SageBuilder::buildFunctionParameterRefExp(int parameter_number, int parameter_level )
   {
     SgFunctionParameterRefExp *expr = new SgFunctionParameterRefExp(NULL,parameter_number,parameter_level);
     ROSE_ASSERT(expr != NULL);

     setSourcePosition(expr);
     return expr;
   }


// DQ (8/11/2014): Added support for C++11 decltype used in new function return syntax.
SgFunctionParameterRefExp*
SageBuilder::buildFunctionParameterRefExp_nfi(int parameter_number, int parameter_level )
   {
     SgFunctionParameterRefExp *expr = new SgFunctionParameterRefExp(NULL,parameter_number,parameter_level);
     ROSE_ASSERT(expr != NULL);

     setOneSourcePositionNull(expr);
     return expr;
   }

// DQ (9/3/2014): Adding support for C++11 Lambda expressions
SgLambdaExp*
SageBuilder::buildLambdaExp(SgLambdaCaptureList* lambda_capture_list, SgClassDeclaration* lambda_closure_class, SgFunctionDeclaration* lambda_function)
   {
     SgLambdaExp *expr = new SgLambdaExp(lambda_capture_list,lambda_closure_class,lambda_function);
     ROSE_ASSERT(expr != NULL);

  // Set the parents
     if (lambda_capture_list != NULL)
        {
          lambda_capture_list->set_parent(expr);
        }

     if (lambda_closure_class != NULL)
        {
          lambda_closure_class->set_parent(expr);
        }

     if (lambda_function != NULL)
        {
#if 1
          lambda_function->set_parent(expr);
#else
          if (lambda_closure_class != NULL)
             {
               lambda_function->set_parent(lambda_closure_class);
             }
            else
             {
               printf ("Warning: In SageBuilder::buildLambdaExp(): lambda_closure_class == NULL: lambda_function parent not set! \n");
             }
#endif
        }

     setSourcePosition(expr);
     return expr;
   }

SgLambdaExp*
SageBuilder::buildLambdaExp_nfi(SgLambdaCaptureList* lambda_capture_list, SgClassDeclaration* lambda_closure_class, SgFunctionDeclaration* lambda_function)
   {
     SgLambdaExp *expr = new SgLambdaExp(lambda_capture_list,lambda_closure_class,lambda_function);
     ROSE_ASSERT(expr != NULL);

  // Set the parents
     if (lambda_capture_list != NULL)
        {
          lambda_capture_list->set_parent(expr);
        }

     if (lambda_closure_class != NULL)
        {
          lambda_closure_class->set_parent(expr);
        }

     if (lambda_function != NULL)
        {
#if 1
          lambda_function->set_parent(expr);
#else
          if (lambda_closure_class != NULL)
             {
               lambda_function->set_parent(lambda_closure_class);
             }
            else
             {
               printf ("Warning: In SageBuilder::buildLambdaExp(): lambda_closure_class == NULL: lambda_function parent not set! \n");
             }
#endif
        }

     setOneSourcePositionNull(expr);
     return expr;
   }

#if 0
SgLambdaCapture*
SageBuilder::buildLambdaCapture(SgInitializedName* capture_variable, SgInitializedName* source_closure_variable, SgInitializedName* closure_variable)
   {
      SgLambdaCapture *lambdaCapture = new SgLambdaCapture(NULL,capture_variable,source_closure_variable,closure_variable);
     ROSE_ASSERT(lambdaCapture != NULL);

     setSourcePosition(lambdaCapture);
     return lambdaCapture;
   }

SgLambdaCapture*
SageBuilder::buildLambdaCapture_nfi(SgInitializedName* capture_variable, SgInitializedName* source_closure_variable, SgInitializedName* closure_variable)
   {
     SgLambdaCapture *lambdaCapture = new SgLambdaCapture(NULL,capture_variable,source_closure_variable,closure_variable);
     ROSE_ASSERT(lambdaCapture != NULL);

     setOneSourcePositionNull(lambdaCapture);
     return lambdaCapture;
   }
#else
SgLambdaCapture*
SageBuilder::buildLambdaCapture(SgExpression* capture_variable, SgExpression* source_closure_variable, SgExpression* closure_variable)
   {
      SgLambdaCapture *lambdaCapture = new SgLambdaCapture(NULL,capture_variable,source_closure_variable,closure_variable);
     ROSE_ASSERT(lambdaCapture != NULL);

     setSourcePosition(lambdaCapture);
     return lambdaCapture;
   }

SgLambdaCapture*
SageBuilder::buildLambdaCapture_nfi(SgExpression* capture_variable, SgExpression* source_closure_variable, SgExpression* closure_variable)
   {
     SgLambdaCapture *lambdaCapture = new SgLambdaCapture(NULL,capture_variable,source_closure_variable,closure_variable);
     ROSE_ASSERT(lambdaCapture != NULL);

     setOneSourcePositionNull(lambdaCapture);
     return lambdaCapture;
   }
#endif

SgLambdaCaptureList*
SageBuilder::buildLambdaCaptureList()
   {
     SgLambdaCaptureList *lambdaCaptureList = new SgLambdaCaptureList(NULL);
     ROSE_ASSERT(lambdaCaptureList != NULL);

     setSourcePosition(lambdaCaptureList);
     return lambdaCaptureList;
   }

SgLambdaCaptureList*
SageBuilder::buildLambdaCaptureList_nfi()
   {
     SgLambdaCaptureList *lambdaCaptureList = new SgLambdaCaptureList(NULL);
     ROSE_ASSERT(lambdaCaptureList != NULL);

     setOneSourcePositionNull(lambdaCaptureList);
     return lambdaCaptureList;
   }



SgNamespaceDefinitionStatement*
SageBuilder::buildNamespaceDefinition(SgNamespaceDeclarationStatement* d)
  {
    SgNamespaceDefinitionStatement* result = NULL;
    if (d!=NULL) // the constructor does not check for NULL d, causing segmentation fault
       {
         result = new SgNamespaceDefinitionStatement(d);
         result->set_parent(d); // set_declaration() == set_parent() in this case
       }
      else
       {
         result = new SgNamespaceDefinitionStatement(d);
       }

    ROSE_ASSERT(result);

    setOneSourcePositionForTransformation(result);
    return result;
  }



SgClassDefinition*
SageBuilder::buildClassDefinition(SgClassDeclaration *d/*= NULL*/, bool buildTemplateInstantiation )
   {
     SgClassDefinition* result = NULL;
     if (d != NULL) // the constructor does not check for NULL d, causing segmentation fault
        {
       // result->set_parent(d); // set_declaration() == set_parent() in this case
       // result = new SgClassDefinition(d);
          ROSE_ASSERT(buildTemplateInstantiation == false || isSgTemplateInstantiationDecl(d) != NULL);
          result = (buildTemplateInstantiation == true) ? new SgTemplateInstantiationDefn(isSgTemplateInstantiationDecl(d)) : new SgClassDefinition(d);
        }
       else
        {
       // result = new SgClassDefinition();
          result = (buildTemplateInstantiation == true) ? new SgTemplateInstantiationDefn() : new SgClassDefinition();
        }

     ROSE_ASSERT(result);

 // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
        {
          result->setCaseInsensitive(true);
        }

     setOneSourcePositionForTransformation(result);

     return result;
   }



SgClassDefinition*
SageBuilder::buildClassDefinition_nfi(SgClassDeclaration *d/*= NULL*/, bool buildTemplateInstantiation )
   {
     SgClassDefinition* result = NULL;
     if (d!=NULL) // the constructor does not check for NULL d, causing segmentation fault
        {
       // result->set_parent(d); // set_declaration() == set_parent() in this case
       // result = new SgClassDefinition(d);
          ROSE_ASSERT(buildTemplateInstantiation == false || isSgTemplateInstantiationDecl(d) != NULL);
          result = (buildTemplateInstantiation == true) ? new SgTemplateInstantiationDefn(isSgTemplateInstantiationDecl(d)) : new SgClassDefinition(d);
        }
       else
        {
       // result = new SgClassDefinition();
          result = (buildTemplateInstantiation == true) ? new SgTemplateInstantiationDefn() : new SgClassDefinition();
        }

     ROSE_ASSERT(result);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
          result->setCaseInsensitive(true);

     setOneSourcePositionNull(result);
     return result;
   }


SgClassDeclaration*
SageBuilder::buildNondefiningClassDeclaration_nfi(const SgName& XXX_name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList)
   {
     SgName nameWithoutTemplateArguments = XXX_name;

     SgName nameWithTemplateArguments = nameWithoutTemplateArguments;

  // SgClassDeclaration* nondefdecl = new SgClassDeclaration(name,kind,NULL,NULL);
     SgClassDeclaration* nondefdecl = NULL;

#define DEBUG_NONDEFINING_CLASS_DECLARATION 0

  // DQ (11/26/2011): Debugging EDG 3.3 use of templateArguments.
#if DEBUG_NONDEFINING_CLASS_DECLARATION
     printf ("Building a SgClassDeclaration: buildNondefiningClassDeclaration_nfi() nameWithoutTemplateArguments = %s buildTemplateInstantiation = %s \n",nameWithoutTemplateArguments.str(),buildTemplateInstantiation ? "true:" : "false");
     printf ("   --- scope = %p = %s \n",scope,(scope != NULL) ? scope->class_name().c_str() : "null");
#endif

  // DQ (7/27/2012): Note that the input name should not have template argument syntax.
  // I think this could still fail for a function with a name such as "X<Y>"  strange converstion operators.
     ROSE_ASSERT(SageInterface::hasTemplateSyntax(nameWithoutTemplateArguments) == false);

     if (buildTemplateInstantiation == true)
        {
          ROSE_ASSERT(templateArgumentsList != NULL);
          nameWithTemplateArguments = appendTemplateArgumentsToName(nameWithoutTemplateArguments,*templateArgumentsList);

#if DEBUG_NONDEFINING_CLASS_DECLARATION
          printf ("Building a SgClassDeclaration: buildNondefiningClassDeclaration_nfi() nameWithTemplateArguments = %s buildTemplateInstantiation = %s \n",nameWithTemplateArguments.str(),buildTemplateInstantiation ? "true:" : "false");
#endif

       // SgTemplateInstantiationDecl (SgName name, SgClassDeclaration::class_types class_type, SgClassType *type, SgClassDefinition *definition, SgTemplateDeclaration *templateDeclaration, SgTemplateArgumentPtrList templateArguments)
          SgTemplateArgumentPtrList emptyList;
       // nondefdecl = new SgTemplateInstantiationDecl(name,kind,NULL,NULL,NULL,emptyList);
          nondefdecl = new SgTemplateInstantiationDecl(nameWithTemplateArguments,kind,NULL,NULL,NULL,emptyList);
#if DEBUG_NONDEFINING_CLASS_DECLARATION
          printf ("In buildNondefiningClassDeclaration_nfi(): built new SgTemplateInstantiationDecl: nondefdecl = %p \n",nondefdecl);
#endif
          ROSE_ASSERT(nondefdecl->get_type() == NULL);
          ROSE_ASSERT(isSgTemplateInstantiationDecl(nondefdecl) != NULL);
#if DEBUG_NONDEFINING_CLASS_DECLARATION
          printf ("In buildNondefiningClassDeclaration_nfi(): nondefdecl->get_name() = %s nondefdecl->get_templateName() = %s \n",
               nondefdecl->get_name().str(),isSgTemplateInstantiationDecl(nondefdecl)->get_templateName().str());
#endif
       // DQ (6/6/2012): Added support for template arguments so that they can be a part of any generated type.
          ROSE_ASSERT(templateArgumentsList != NULL);

#if DEBUG_NONDEFINING_CLASS_DECLARATION
          printf ("nondefdecl->get_name() = %s \n",nondefdecl->get_name().str());
          printf ("nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
          printf ("Output templateArgumentsList: \n");
          for (size_t i = 0; i < templateArgumentsList->size(); i++)
             {
               printf ("   --- --- templateArgumentsList->[%zu] = %p \n",i,templateArgumentsList->operator[](i));
               printf ("   --- --- name = %s \n",unparseTemplateArgumentToString(templateArgumentsList->operator[](i)).str());
             }
#endif

       // DQ (3/9/2018): Added assertion.
          ROSE_ASSERT(nondefdecl->get_name() == nameWithTemplateArguments);

       // DQ (5/8/2013): This fails for explicit template instantation examples (e.g. template <> class RepeatedPtrField<string>::TypeHandler {};, in test2013_159.C)
       // ROSE_ASSERT(templateArgumentsList->size() > 0);
#if 0
       // DQ (9/16/2012): Call the newly refactored function after the firstNondefiningDeclaration is set.

       // Calling the assignment operator for the STL container class.
          isSgTemplateInstantiationDecl(nondefdecl)->get_templateArguments() = *templateArgumentsList;

#error "DEAD CODE!"

#if 1
       // DQ (9/13/2012): Refactored this code.
          setTemplateArgumentParents(nondefdecl);
#else
       // DQ (7/25/2012): Added this code here to reset the parents of the template arguments.
          for (size_t i = 0; i < templateArgumentsList->size(); i++)
             {
            // DQ (7/25/2012): This should be true because the template argument was set to the functions
            // scope so that the name with template arguments could be computed (with name qualification).
                ROSE_ASSERT((*templateArgumentsList)[i]->get_parent() != NULL);

#error "DEAD CODE!"

            // ROSE_ASSERT(isSgGlobal(templateArgumentsList[i]->get_parent()) == NULL);
            // ROSE_ASSERT(templateArgumentsList[i]->get_parent() == nondefining_templateInstantiation);

            // Be we want to reset it to be the function (now that it is available, because this is more precise).
            // All qualified names should compute to the same qualified name (if not then it is a bug in the name
            // qualification mechanism).
               (*templateArgumentsList)[i]->set_parent(nondefdecl);
             }
#endif
#endif
          ROSE_ASSERT(isSgTemplateInstantiationDecl(nondefdecl)->get_templateName().is_null() == true);
          isSgTemplateInstantiationDecl(nondefdecl)->set_templateName(nameWithoutTemplateArguments);
        }
       else
        {
       // nondefdecl = new SgClassDeclaration(name,kind,NULL,NULL);
          nondefdecl = new SgClassDeclaration(nameWithoutTemplateArguments,kind,NULL,NULL);
#if DEBUG_NONDEFINING_CLASS_DECLARATION
          printf ("In buildNondefiningClassDeclaration_nfi(): built new SgClassDeclaration: nondefdecl = %p \n",nondefdecl);
#endif
       // The default name for nameWithTemplateArguments is nameWithoutTemplateArguments so that we can use
       // nameWithTemplateArguments uniformally as the name of the function and it will work from non-template
       // instantiations.
          ROSE_ASSERT(nameWithoutTemplateArguments == nameWithTemplateArguments);
        }

     ROSE_ASSERT(nondefdecl != NULL);

  // DQ (6/9/2013): Added assertion to debug test2013_198.C.
     ROSE_ASSERT(nondefdecl->get_definition() == NULL);

  // DQ (3/22/2012): I think we can assert this! No, in fact we can assert that it is not built yet.
  // ROSE_ASSERT(nondefdecl->get_type() != NULL);
     ROSE_ASSERT(nondefdecl->get_type() == NULL);

#if 0
  // DQ (3/22/2012): I think this may be too early.
  // Liao, we ask for explicit creation of SgClassType to avoid duplicated type nodes
     if (nondefdecl->get_type() == NULL)
        {
          nondefdecl->set_type(SgClassType::createType(nondefdecl));
        }
#endif

#if 0
     printf ("SageBuilder::buildNondefiningClassDeclaration_nfi(): (and setting source position) nondefdecl = %p \n",nondefdecl);
#endif

  // The non-defining declaration asociated with a declaration does not have a
  // source position...unless it is the position of the defining declaration.
  // setOneSourcePositionNull(nondefdecl);
     setSourcePosition(nondefdecl);

  // This is find for now, but a little later in this function (if we can find a symbol)
  // we want to find the first non-defining declaration (using the symbol table) and use
  // that as a paramter to "nondefdecl->set_firstNondefiningDeclaration()".
     nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
     nondefdecl->set_definingDeclaration(NULL);
     nondefdecl->setForward();

  // This is the structural parent (the logical scope can be different than the parent).
  // TPS (09/18/2009) added a condition to be able to build this properly
     if (scope == NULL)
          nondefdecl->set_parent(topScopeStack());
       else
          nondefdecl->set_parent(scope);

  // This is the logical scope...
     nondefdecl->set_scope(scope);

     ROSE_ASSERT(nondefdecl->get_parent() != NULL);

     SgClassDeclaration* firstNondefdecl = NULL;
     if (scope != NULL)
        {
#if 0
          SgClassSymbol* mysymbol = new SgClassSymbol(nondefdecl);
          ROSE_ASSERT(mysymbol != NULL);

       // printf ("In SageBuilder::buildNondefiningClassDeclaration(): for nondefdecl = %p built SgClassSymbol = %p \n",nondefdecl,mysymbol);

#error "DEAD CODE"

          scope->insert_symbol(name, mysymbol);
#else
       // DQ (8/22/2012): Use the template arguments to further disambiguate names that would
       // not include name qualification on template arguments.
       // Reuse any previously defined symbols (to avoid redundant symbols in the symbol table)
       // and find the firstNondefiningDeclaration.
       // SgClassSymbol* mysymbol = scope->lookup_class_symbol(name);
       // SgClassSymbol* mysymbol = scope->lookup_nontemplate_class_symbol(name);
       // SgClassSymbol* mysymbol = scope->lookup_nontemplate_class_symbol(nameWithTemplateArguments);
          SgClassSymbol* mysymbol = scope->lookup_nontemplate_class_symbol(nameWithTemplateArguments,templateArgumentsList);

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
          printf ("In SageBuilder::buildNondefiningClassDeclaration(): mysymbol = %p = %s \n",mysymbol,(mysymbol != NULL) ? mysymbol->class_name().c_str() : "null");
#endif
          if (mysymbol != NULL)
             {
               firstNondefdecl = isSgClassDeclaration(mysymbol->get_declaration());
               ROSE_ASSERT(firstNondefdecl != NULL);

            // DQ (9/4/2012): Added assertion.
               ROSE_ASSERT(firstNondefdecl->get_type() != NULL);

            // DQ (3/22/2012): Now we can built the type and have it use the same nondefining declaration as from the symbol (required to match).
               ROSE_ASSERT(nondefdecl->get_type() == NULL);

               if (nondefdecl->get_type() == NULL)
                  {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                    printf ("In SageBuilder::buildNondefiningClassDeclaration(): Why are we creating a new type instead of reusing the type (firstNondefdecl->get_type() = %p) from the firstNondefdecl = %p \n",firstNondefdecl->get_type(),firstNondefdecl);
#endif
                 // Note: It would be better to just call: "nondefdecl->set_type(firstNondefdecl->get_type());"
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                    printf ("NOTE: Call nondefdecl->set_type(firstNondefdecl->get_type()); instead of nondefdecl->set_type(SgClassType::createType(firstNondefdecl)); \n");
#endif
                 // DQ (3/22/2012): Be careful to use the same declaration as from the symbol.
                 // nondefdecl->set_type(SgClassType::createType(nondefdecl));
                    nondefdecl->set_type(SgClassType::createType(firstNondefdecl));
                    ROSE_ASSERT(nondefdecl->get_type() != NULL);
#if 0
                    printf ("In SageBuilder::buildNondefiningClassDeclaration(): built class type: part 1: nondefdecl->get_type() = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
#endif

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                    printf ("In SageBuilder::buildNondefiningClassDeclaration(): nondefdecl->get_type() = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
#endif
                 // DQ (9/4/2012): Added assertion.
                    ROSE_ASSERT(nondefdecl->get_type() == firstNondefdecl->get_type());
                  }

#if (REUSE_CLASS_DECLARATION_FROM_SYMBOL == 0)
               ROSE_ASSERT(nondefdecl != NULL);
               ROSE_ASSERT(nondefdecl->get_parent() != NULL);

            // DQ (5/18/2014): Added test to match that in set_firstNondefiningDeclaration().
            // This is a problem for the Boost code after the fix to detec templates vs. template instantiation declarations.
               if (nondefdecl->variantT() != firstNondefdecl->variantT())
                  {
                    printf ("ERROR: In SgDeclarationStatement::set_firstNondefiningDeclaration(): nondefdecl = %p = %s IS NOT THE SAME AS firstNondefiningDeclaration = %p = %s \n",
                         nondefdecl,nondefdecl->class_name().c_str(),firstNondefdecl,firstNondefdecl->class_name().c_str());
                    ROSE_ASSERT(nondefdecl->get_file_info() != NULL);
                    nondefdecl->get_file_info()->display("ERROR: In SgDeclarationStatement::set_firstNondefiningDeclaration(): nondefdecl: debug");
                    ROSE_ASSERT(firstNondefdecl->get_file_info() != NULL);
                    firstNondefdecl->get_file_info()->display("ERROR: In SgDeclarationStatement::set_firstNondefiningDeclaration(): firstNondefdecl: debug");
                  }

            // DQ (5/18/2014): Added test to match that in set_firstNondefiningDeclaration().
               ROSE_ASSERT(nondefdecl->variantT() == firstNondefdecl->variantT());

               nondefdecl->set_firstNondefiningDeclaration(firstNondefdecl);

            // This might be NULL if the defining declaration has not been seen yet!
               nondefdecl->set_definingDeclaration(firstNondefdecl->get_definingDeclaration());

            // DQ (3/22/2012): New assertions.
               ROSE_ASSERT(firstNondefdecl != NULL);
               ROSE_ASSERT(firstNondefdecl->get_type() != NULL);

            // DQ (9/16/2012): This is a newly refactored function (call this after we know the firstNondefiningDeclaration is set correctly).
            // This is called in the other branch (mysymbol == NULL), but there is must be called before the symbol table is appended with
            // the new symbol for this declaration. So we have to call this in this brach and re can't refactor this be be called one before
            // both branches or once after both branches.
               if (buildTemplateInstantiation == true)
                  {
                    setTemplateArgumentsInDeclaration(nondefdecl,templateArgumentsList);
                  }

            // DQ (9/4/2012): We can now assert this because of how the type is constructed above.
               ROSE_ASSERT (nondefdecl->get_type() == firstNondefdecl->get_type());

            // Share the type!
               if (nondefdecl->get_type() != firstNondefdecl->get_type())
                  {
                 // Remove the type from the new SgClassDeclaration and set the reference to the type in the firstNondefiningDeclaration.
                    printf ("Deleting type in associated non-defining declaration (sharing type) nondefdecl->get_type() = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
                    printf ("Skipping delete of %p so we can maintain unique type pointers \n",nondefdecl->get_type());
                 // delete nondefdecl->get_type();
                    printf ("Setting the new type to be from firstNondefdecl = %p (sharing type) firstNondefdecl->get_type() = %p = %s \n",firstNondefdecl,firstNondefdecl->get_type(),firstNondefdecl->get_type()->class_name().c_str());
                    nondefdecl->set_type(firstNondefdecl->get_type());
#if 1
                 // DQ (12/13/2011): Is this executed!
                    printf ("Unclear if this code is executed \n");
                    ROSE_ASSERT(false);
#endif
                  }
#else
#error "DEAD CODE"

               ROSE_ASSERT(nondefdecl == NULL);
#endif
            // This function should return a new nondefining declaration each time (to support multile class prototypes!).
            // nondefdecl = firstNondefdecl;
             }
            else
             {
#if REUSE_CLASS_DECLARATION_FROM_SYMBOL
            // DQ (1/25/2009): We only want to build a new declaration if we can't reuse the existing declaration.

#error "DEAD CODE"

               nondefdecl = new SgClassDeclaration(name,kind,NULL,NULL);

#error "DEAD CODE"

               ROSE_ASSERT(nondefdecl != NULL);
               if (nondefdecl->get_type() == NULL)
                    nondefdel->set_type(SgClassType::createType(nondefdecl));

               printf ("SageBuilder::buildNondefiningClassDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

               setOneSourcePositionNull(nondefdecl);

#error "DEAD CODE"

               nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
               nondefdecl->set_definingDeclaration(NULL);
               nondefdecl->setForward();
#endif

            // DQ (6/9/2013): Added assertion to debug test2013_198.C.
               ROSE_ASSERT(nondefdecl->get_definition() == NULL);

               mysymbol = new SgClassSymbol(nondefdecl);
               firstNondefdecl = nondefdecl;

            // DQ (6/9/2013): Adding assertions to make sure that symbols only reference non-defining declarations.
               ROSE_ASSERT(nondefdecl->get_definition() == NULL);
               ROSE_ASSERT(mysymbol->get_declaration()->get_definition() == NULL);

            // DQ (9/16/2012): This is a newly refactored function (call this after the firstNondefiningDeclaration is set).
            // Note that since the symbol tables use the template arguments associated with the declaration it is best to
            // fixup the template arguments before the symbol table is fixup to have a symbol for this declaration. So we
            // fixup the template arguments here (just after we know that the firstNondefiningDeclaration is set correctly
            // and just before the symbol is inserted into the symbol table.
               if (buildTemplateInstantiation == true)
                  {
                    setTemplateArgumentsInDeclaration(nondefdecl,templateArgumentsList);
                  }
#if DEBUG_NONDEFINING_CLASS_DECLARATION
               printf ("BEFORE scope->insert_symbol(): scope = %p = %s nameWithTemplateArguments = %s mysymbol = %p = %s \n",
                    scope,scope->class_name().c_str(),nameWithTemplateArguments.str(),mysymbol,mysymbol->class_name().c_str());
#endif

            // scope->insert_symbol(name, mysymbol);
               scope->insert_symbol(nameWithTemplateArguments, mysymbol);

            // DQ (3/22/2012): Now we can built the type and have it use the same nondefining declaration as from the symbol (required to match).
               ROSE_ASSERT(nondefdecl->get_type() == NULL);
               if (nondefdecl->get_type() == NULL)
                  {
#if 0
                    printf ("In buildNondefiningClassDeclaration_nfi(): nondefdecl = %p = %s \n",nondefdecl,nondefdecl->class_name().c_str());
                    printf ("In buildNondefiningClassDeclaration_nfi(): nondefdecl->get_firstNondefiningDeclaration() = %p \n",nondefdecl->get_firstNondefiningDeclaration());
#endif
                    nondefdecl->set_type(SgClassType::createType(nondefdecl));
#if 0
                    printf ("In SageBuilder::buildNondefiningClassDeclaration(): built class type: part 2: nondefdecl->get_type() = %p = %s = %s \n",
                         nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str(),nondefdecl->get_type()->unparseToString().c_str());
#endif
                  }

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("NOTE: In buildNondefiningClassDeclaration_nfi(): 2nd time this is a performance issue (maybe) to call the lookup_nontemplate_class_symbol() again \n");
#endif
            // DQ (8/22/2012): Use the template arguments to further disambiguate names that would
            // not include name qualification on template arguments.
            // DQ (12/27/2011): Added new test.
            // ROSE_ASSERT(scope->lookup_nontemplate_class_symbol(name) != NULL);
            // TV (07/01/2013): this assertion fail when building basic class (buildTemplateInstantiation = false , templateArgumentsList = NULL)
            // ROSE_ASSERT(scope->lookup_nontemplate_class_symbol(nameWithTemplateArguments,templateArgumentsList) != NULL);

            // DQ (6/9/2013): Added test to make sure that symbols only reference non-defining declarations.
               SgClassSymbol* temp_classSymbol = nondefdecl->get_scope()->lookup_nontemplate_class_symbol(nameWithTemplateArguments,templateArgumentsList);
#if DEBUG_NONDEFINING_CLASS_DECLARATION
            // DQ (12/28/2018): When can this be NULL?
               printf ("In buildNondefiningClassDeclaration_nfi(): temp_classSymbol = %p \n",temp_classSymbol);
               printf ("In buildNondefiningClassDeclaration_nfi(): nondefdecl->get_scope() = %p = %s scope = %p \n",nondefdecl->get_scope(),nondefdecl->get_scope()->class_name().c_str(),scope);

               printf ("In buildNondefiningClassDeclaration_nfi(): nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
               if (templateArgumentsList != NULL)
                  {
                    printf ("   --- templateArgumentsList elements: \n");
                    for (size_t i = 0; i < templateArgumentsList->size(); i++)
                       {
                         printf ("   --- --- templateArgumentsList->[%zu] = %p \n",i,templateArgumentsList->operator[](i));
                         printf ("   --- --- templateArgumentsList->[%zu] = %s \n",i,templateArgumentsList->operator[](i)->class_name().c_str());
                         templateArgumentsList->operator[](i)->display("In SageBuilder::buildNondefiningClassDeclaration_nfi()");
                       }
                  }
#endif
            // DQ (12/28/2018): When can this be NULL?  When we call lookup_class_symbol() later it is NULL, so test it here.
               ROSE_ASSERT(nondefdecl->get_scope()->lookup_class_symbol(nameWithTemplateArguments,templateArgumentsList) != NULL);
               ROSE_ASSERT(nondefdecl->get_scope() == scope);

            // TV (07/01/2013): temp_classSymbol can be NULL, but lookup_class_symbol return a symbol
            // ROSE_ASSERT(temp_classSymbol->get_declaration()->get_definition() == NULL);
               ROSE_ASSERT(temp_classSymbol == NULL || temp_classSymbol->get_declaration()->get_definition() == NULL);
             }

          ROSE_ASSERT(mysymbol != NULL);
          ROSE_ASSERT(firstNondefdecl != NULL);
#endif
          nondefdecl->set_scope(scope);

       // DQ (1/25/2009): The scope is not the same as the parent, since the scope is logical, and the parent is structural (note that topScopeStack() is structural).
       // TPS (09/18/2009) added a condition to be able to build this properly
          if (scope==NULL)
            nondefdecl->set_parent(topScopeStack());
          else
            nondefdecl->set_parent(scope);
        }

  // The support for SgEnumDeclaration handles the type, but why not for SgClassDeclaration?
     ROSE_ASSERT(nondefdecl->get_type() != NULL);

     ROSE_ASSERT(nondefdecl->get_parent() != NULL);

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
     printf ("NOTE: In buildNondefiningClassDeclaration_nfi(): 3rd time this is a performance issue (maybe) to call the lookup_nontemplate_class_symbol() again \n");
#endif

  // DQ (8/22/2012): Use the template arguments to further disambiguate names that would not include name qualification on template arguments.
  // DQ (12/27/2011): Added new test.
  // ROSE_ASSERT(nondefdecl->get_scope()->lookup_nontemplate_class_symbol(name) != NULL);
  // TV (07/01/2013): this assertion fail when building basic class (buildTemplateInstantiation = false , templateArgumentsList = NULL)
  // ROSE_ASSERT(nondefdecl->get_scope()->lookup_nontemplate_class_symbol(nameWithTemplateArguments,templateArgumentsList) != NULL);

  // DQ (6/9/2013): Added test to make sure that symbols only reference non-defining declarations.
     SgClassSymbol* temp_classSymbol = nondefdecl->get_scope()->lookup_nontemplate_class_symbol(nameWithTemplateArguments,templateArgumentsList);
  // TV (07/01/2013): temp_classSymbol can be NULL, but lookup_class_symbol return a symbol
     ROSE_ASSERT(temp_classSymbol == NULL || temp_classSymbol->get_declaration()->get_definition() == NULL);

  // DQ (3/9/2018): Added assertion.
     ROSE_ASSERT(nondefdecl != NULL);
     ROSE_ASSERT(nondefdecl->get_name() == nameWithTemplateArguments);

  // DQ (3/9/2018): Test the consistancy of the template instantiation name.
     if (isSgTemplateInstantiationDecl(nondefdecl) != NULL)
        {
          SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(nondefdecl);
          SgName finalName = appendTemplateArgumentsToName(templateInstantiationDecl->get_templateName(),templateInstantiationDecl->get_templateArguments());
          ROSE_ASSERT(finalName == nameWithTemplateArguments);
          ROSE_ASSERT(finalName == nondefdecl->get_name());
        }

#if DEBUG_NONDEFINING_CLASS_DECLARATION
     printf ("Leaving buildNondefiningClassDeclaration_nfi(): nondefdecl = %p nondefdecl->unparseNameToString() = %s \n",nondefdecl,nondefdecl->unparseNameToString().c_str());
     printf (" --- nondefdecl = %p = %s \n",nondefdecl,nondefdecl->class_name().c_str());
#endif

#if DEBUG_NONDEFINING_CLASS_DECLARATION
  // DQ (1/27/2019): Test that symbol table to debug Cxx11_tests/test2019)33.C.
     printf ("Leaving buildNondefiningClassDeclaration_nfi(): Calling find_symbol_from_declaration() \n");
     SgSymbol* test_symbol = nondefdecl->get_scope()->find_symbol_from_declaration(nondefdecl);

  // DQ (1/27/2019): Test that symbol table to debug Cxx11_tests/test2019)33.C.
     printf ("Leaving buildNondefiningClassDeclaration_nfi(): Calling get_symbol_from_symbol_table() \n");
     ROSE_ASSERT(nondefdecl->get_symbol_from_symbol_table() != NULL);
#endif

     return nondefdecl;
   }

SgStmtDeclarationStatement*
SageBuilder::buildStmtDeclarationStatement_nfi(SgStatement* stmt) {
    ROSE_ASSERT(stmt != NULL);

    SgStmtDeclarationStatement* result = new SgStmtDeclarationStatement(stmt);
    stmt->set_parent(result);

    result->set_definingDeclaration(result);
    setOneSourcePositionNull(result);
    return result;
}

SgStmtDeclarationStatement*
SageBuilder::buildStmtDeclarationStatement(SgStatement* stmt) {
    ROSE_ASSERT(stmt != NULL);

    SgStmtDeclarationStatement* result = new SgStmtDeclarationStatement(stmt);
    stmt->set_parent(result);

    result->set_definingDeclaration(result);
    setOneSourcePositionForTransformation(result);
    return result;
}


// This should take a SgClassDeclaration::class_types kind parameter!
SgClassDeclaration * SageBuilder::buildStructDeclaration(const SgName& name, SgScopeStatement* scope /*=NULL*/)
   {
#if 0
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

  // TODO How about class type??
  // build defining declaration
     SgClassDefinition* classDef = buildClassDefinition();

     SgClassDeclaration* defdecl = new SgClassDeclaration (name,SgClassDeclaration::e_struct,NULL,classDef);
     ROSE_ASSERT(defdecl);
     setOneSourcePositionForTransformation(defdecl);
  // constructor is side-effect free
     classDef->set_declaration(defdecl);
     defdecl->set_definingDeclaration(defdecl);

  // build the nondefining declaration
     SgClassDeclaration* nondefdecl = new SgClassDeclaration (name,SgClassDeclaration::e_struct,NULL,NULL);
     ROSE_ASSERT(nondefdecl);

     setOneSourcePositionForTransformation(nondefdecl);
     nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
     nondefdecl->set_definingDeclaration(defdecl);
     defdecl->set_firstNondefiningDeclaration(nondefdecl);
     nondefdecl->setForward();

     if (scope !=NULL )  // put into fixStructDeclaration() or alike later on
        {
          fixStructDeclaration(nondefdecl,scope);
          fixStructDeclaration(defdecl,scope);
#if 0
          SgClassSymbol* mysymbol = new SgClassSymbol(nondefdecl);
          ROSE_ASSERT(mysymbol);
          scope->insert_symbol(name, mysymbol);
          defdecl->set_scope(scope);
          nondefdecl->set_scope(scope);
          defdecl->set_parent(scope);
          nondefdecl->set_parent(scope);
#endif
        }
#else
  // DQ (1/24/2009): Refactored to use the buildStructDeclaration_nfi function.
  // (if this work it needs to be done uniformally for the other nfi functions)
  // Also, "_nfi" is not a great name.
  // SgClassDeclaration* defdecl = buildClassDeclaration_nfi(name,SgClassDeclaration::e_struct,scope,NULL);
     bool buildTemplateInstantiation = false;
  // SgClassDeclaration* defdecl = buildClassDeclaration_nfi(name,SgClassDeclaration::e_struct,scope,NULL,buildTemplateInstantiation);
     SgClassDeclaration* defdecl = buildClassDeclaration_nfi(name,SgClassDeclaration::e_struct,scope,NULL,buildTemplateInstantiation,NULL);

     setOneSourcePositionForTransformation(defdecl);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != NULL);
     setOneSourcePositionForTransformation(defdecl->get_firstNondefiningDeclaration());
#endif

  // DQ (1/26/2009): I think this should be an error, but that appears it would
  // break the existing interface. Need to discuss this with Liao.
  // ROSE_ASSERT(defdecl->get_parent() != NULL);

     return defdecl;
   }

//! Build a Fortran derived type declaration.
SgDerivedTypeStatement * SageBuilder::buildDerivedTypeStatement(const SgName& name, SgScopeStatement* scope /*=NULL*/)
   {
     SgClassDeclaration::class_types kind = SgClassDeclaration::e_struct;
     SgDerivedTypeStatement* type_decl = buildClassDeclarationStatement_nfi <SgDerivedTypeStatement> (name, kind, scope);

     setOneSourcePositionForTransformation(type_decl);
     ROSE_ASSERT(type_decl->get_firstNondefiningDeclaration() != NULL);
     setOneSourcePositionForTransformation(type_decl->get_firstNondefiningDeclaration());

     return type_decl;
   }

//! Build a Jovial table declaration statement.  A Jovial table is essentially a C struct with an optional struct size.
SgJovialTableStatement * SageBuilder::buildJovialTableStatement(const SgName& name,
                                                                SgClassDeclaration::class_types kind,
                                                                SgScopeStatement* scope /*=NULL*/)
   {
     SgJovialTableStatement* table_decl = buildClassDeclarationStatement_nfi <SgJovialTableStatement> (name, kind, scope);

     setOneSourcePositionForTransformation(table_decl);
     ROSE_ASSERT(table_decl->get_firstNondefiningDeclaration() != NULL);
     setOneSourcePositionForTransformation(table_decl->get_firstNondefiningDeclaration());

     return table_decl;
   }

//! Build a Jovial table type with required class definition and defining and nondefining declarations.
SgJovialTableType * SageBuilder::buildJovialTableType (const SgName& name, SgType* base_type, SgExprListExp* dim_info, SgScopeStatement* scope)
   {
     SgClassDeclaration::class_types kind = SgClassDeclaration::e_jovial_table;
     SgJovialTableStatement* table_decl = buildClassDeclarationStatement_nfi <SgJovialTableStatement> (name, kind, scope);

     setOneSourcePositionForTransformation(table_decl);
     ROSE_ASSERT(table_decl->get_firstNondefiningDeclaration() != NULL);
     setOneSourcePositionForTransformation(table_decl->get_firstNondefiningDeclaration());

  // For a type declaration the parent of the nondefining declaration is the defining declaration
     SgClassDeclaration* nondef_decl = isSgClassDeclaration(table_decl->get_firstNondefiningDeclaration());
     ROSE_ASSERT(nondef_decl != NULL);
     nondef_decl->set_parent(table_decl);

     SgJovialTableType* table_type = new SgJovialTableType(nondef_decl);
     ROSE_ASSERT(table_type != NULL);

     table_type->set_base_type(base_type);
     table_type->set_dim_info(dim_info);
     table_type->set_rank(dim_info->get_expressions().size());

     nondef_decl->set_type(table_type);

     return table_type;
   }

//! Build a generic class declaration statement (SgClassDeclaration or subclass).
template <class DeclClass> DeclClass *
SageBuilder::buildClassDeclarationStatement_nfi(const SgName & name, SgClassDeclaration::class_types kind,
                                                SgScopeStatement* scope, SgClassDeclaration* nonDefiningDecl)
   {
  // DQ (3/15/2012): Added function to build C++ class (builds both the non-defining and defining declarations; in that order).
  // The implementation of this function could be simplified to directly call both:
  //    SgClassDeclaration* buildNondefiningClassDeclaration ( SgName name, SgScopeStatement* scope );
  // and
  //    SgClassDeclaration* buildDefiningClassDeclaration    ( SgName name, SgScopeStatement* scope );
  // This might refactor the implementation nicely.

     if (scope == NULL)
        {
          scope = SageBuilder::topScopeStack();
        }

  // Step 1. Build the nondefining declaration (but only if the input nonDefiningDecl pointer was NULL and it does not exist)
  // -----------------------------------------
  //

  // Get the nondefining declaration from the symbol if it has been built (if this works,
  // then we likely don't need the "SgClassDeclaration* nonDefiningDecl" parameter).

     SgClassDeclaration* nondefdecl = NULL;
     SgClassSymbol* mysymbol = NULL;

     if (scope != NULL)
        {
        // DQ (10/10/2015): look up the correct type of symbol.
           mysymbol = scope->lookup_class_symbol(name);

           if (mysymbol == NULL)
              {
              // Note: this is an input parameter (could/should go away?) [Rasmussen]
                 if (nonDefiningDecl != NULL)
                    {
                    // DQ (3/4/2018): I think this is the correct API to use (internal use only).
                       SgSymbol* temp_mysymbol = nonDefiningDecl->get_symbol_from_symbol_table();
                       ROSE_ASSERT(temp_mysymbol != NULL);

                       mysymbol = isSgClassSymbol(temp_mysymbol);
                       ROSE_ASSERT(mysymbol != NULL);

                    // check that the scopes are the same.
                       ROSE_ASSERT(scope == nonDefiningDecl->get_scope());
                    }
              }
        }

     if (mysymbol != NULL) // set links for existing nondefining declaration
        {
          nondefdecl = (mysymbol->get_declaration() == NULL)
                     ? NULL : dynamic_cast<DeclClass*>(mysymbol->get_declaration());
          ROSE_ASSERT(nondefdecl != NULL);

       // DQ (6/8/2013): This should not be true (see test2013_198.C).
       // Fundamentally the symbol should always only have a pointer to a non-defining
       // declaration, where by definition (get_definition() == NULL).
          ROSE_ASSERT(nondefdecl->get_definition() == NULL);

       // DQ (9/16/2012): This should be true by definition (verify).
          ROSE_ASSERT(nondefdecl == nondefdecl->get_firstNondefiningDeclaration());

          ROSE_ASSERT(nondefdecl->get_type()   != NULL);
          ROSE_ASSERT(nondefdecl->get_parent() != NULL);

       // DQ (9/7/2012): I think this might be the root of a problem in the haskell tests (ROSE compiling ROSE).
          if (nondefdecl->get_definingDeclaration() != NULL)
             {
               DeclClass* nondefining_classDeclaration = (nondefdecl == NULL) ? NULL : dynamic_cast<DeclClass*>(nondefdecl);
               ROSE_ASSERT(nondefining_classDeclaration != NULL);
               DeclClass* defining_classDeclaration = (nondefdecl->get_definingDeclaration() == NULL)
                                                    ? NULL : dynamic_cast<DeclClass*>(nondefdecl->get_definingDeclaration());
               ROSE_ASSERT(defining_classDeclaration != NULL);

               return defining_classDeclaration;
             }
        }
      else // build a nondefining declaration since it does not exist
        {
          ROSE_ASSERT(nondefdecl == NULL);

       // DeclClass is the template type parameter
          nondefdecl = new DeclClass(name, kind, NULL, NULL);
          ROSE_ASSERT(nondefdecl != NULL);

       // The first nondefining declaration has to be set before we generate the type.
          nondefdecl->set_firstNondefiningDeclaration(nondefdecl);

       // DQ (7/25/2017): This will be true, but it might not be what we want since it can be caught as an error in the code below.
          ROSE_ASSERT(nondefdecl->get_file_info() == NULL);

       // DQ (3/14/2012): For C++ we need the scope set so that types will have proper locations to resolve them
       // from being ambiguous or not properly defined.  Basically, we need a handle from which to generate something
       // that amounts to a kind of name qualification internally (maybe even exactly name qualification, but I would
       // have to think about that a bit more).
          ROSE_ASSERT(scope != NULL);

       // Set the parent before calling the SgClassType::createType() as the name mangling will require it.
       // This is true for Fortran SgDerivedTypeStatement at least.
          nondefdecl->set_parent(scope);
          nondefdecl->set_scope(scope);

          ROSE_ASSERT(nondefdecl->get_scope() != NULL);
          ROSE_ASSERT(nondefdecl->get_type() == NULL);

          if (nondefdecl->get_type() == NULL)
             {
               SgClassType* class_type = NULL;
               switch (kind)
                  {
                    case SgClassDeclaration::e_java_parameter:
                       class_type = SgJavaParameterType::createType(nondefdecl);
                       break;
                    case SgClassDeclaration::e_jovial_table:
                    case SgClassDeclaration::e_jovial_block:
                       class_type = SgJovialTableType::createType(nondefdecl);
                       break;
                    default:
                       class_type = SgClassType::createType(nondefdecl);
                       break;
                  }
               ROSE_ASSERT(class_type != NULL);

               nondefdecl->set_type(class_type);

               SgClassDeclaration* tmp_classDeclarationFromType = isSgClassDeclaration(class_type->get_declaration());
               ROSE_ASSERT(tmp_classDeclarationFromType != NULL);
             }

       // DQ (3/22/2012): Added assertions.
          ROSE_ASSERT(nondefdecl->get_type() != NULL);
          if (nondefdecl->get_type()->get_declaration() != nondefdecl)
             {
               printf ("ERROR: nondefdecl = %p = %s \n",nondefdecl,nondefdecl->class_name().c_str());
               printf ("ERROR: nondefdecl->get_type() = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
               printf ("ERROR: nondefdecl->get_type()->get_declaration() = %p = %s \n",nondefdecl->get_type()->get_declaration(),nondefdecl->get_type()->get_declaration()->class_name().c_str());

               SgClassDeclaration* classDeclarationFromType = isSgClassDeclaration(nondefdecl->get_type()->get_declaration());
               ROSE_ASSERT(classDeclarationFromType != NULL);

               printf ("nondefdecl->get_name() = %s \n",nondefdecl->get_name().str());
               printf ("nondefdecl->get_type()->get_name() = %s \n",nondefdecl->get_type()->get_name().str());
               printf ("nondefdecl->get_type()->get_declaration()->get_name() = %s \n",classDeclarationFromType->get_name().str());

               printf ("nondefdecl->get_mangled_name() = %s \n",nondefdecl->get_mangled_name().getString().c_str());
               printf ("nondefdecl->get_type()->get_mangled() = %s \n",nondefdecl->get_type()->get_mangled().getString().c_str());
               printf ("nondefdecl->get_type()->get_declaration()->get_mangled_name() = %s \n",classDeclarationFromType->get_mangled_name().getString().c_str());

            // DQ (12/27/2018): Added additional debugging support.
               printf ("nondefdecl->get_type()->get_declaration()->get_firstNondefiningDeclaration() = %s \n",classDeclarationFromType->get_firstNondefiningDeclaration() ? "true" : "false");
               printf ("nondefdecl->get_firstNondefiningDeclaration()                                = %s \n",nondefdecl->get_firstNondefiningDeclaration() ? "true" : "false");

            // DQ (12/27/2018): I think that if this is a base class declaration then it is OK for the type's declaration to not match.
            // ROSE_ASSERT(nondefdecl->get_parent() != NULL);
               if (nondefdecl->get_parent() != NULL)
                  {
                    printf ("nondefdecl->get_parent() = %p = %s \n",nondefdecl->get_parent(),nondefdecl->get_parent()->class_name().c_str());
                  }
             }
          ROSE_ASSERT(nondefdecl->get_type()->get_declaration() == nondefdecl);

       // The nondefining declaration will not appear in the source code, but is compiler
       // generated (so we have something about the class that we can reference; e.g in
       // types).  At the moment we make it a transformation, there might be another kind
       // of source position that would be more precise.  FIXME.
       // setOneSourcePositionNull(nondefdecl);
          setOneSourcePositionForTransformation(nondefdecl);
          ROSE_ASSERT (nondefdecl->get_startOfConstruct() != NULL);

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
       // DQ (5/2/2012): After EDG/ROSE translation, there should be no IR nodes marked as transformations.
          if (SourcePositionClassificationMode != e_sourcePositionTransformation)
             {
               detectTransformations(nondefdecl);
             }
#endif

          nondefdecl->setForward();

          if (scope != NULL)
             {
               mysymbol = new SgClassSymbol(nondefdecl);
               scope->insert_symbol(name, mysymbol);

               ROSE_ASSERT(nondefdecl->get_scope() == scope);
             }
        }

  // DQ (3/15/2012): I have moved construction of defining declaration to be AFTER the nondefining declaration!
  // This is a better organization ans also should make sure that the declaration in the SgClassType will
  // properly reference the firstNondefiningDeclaration (instead of the defining declaration).

  // Step 2. Build the defining declaration
  // --------------------------------------
  //
     SgClassDefinition* classDef = buildClassDefinition();

     DeclClass* defdecl = new DeclClass(name,kind,NULL,classDef);
     ROSE_ASSERT(defdecl != NULL);
     ROSE_ASSERT(defdecl->get_type() == NULL);

  // DQ (3/5/2012): Check that the SgClassDefinition is properly matching.
     ROSE_ASSERT(defdecl->get_definition() != NULL);
     ROSE_ASSERT(defdecl != NULL);

  // DQ (3/15/2012): Moved from original location above...
     nondefdecl->set_definingDeclaration(defdecl);

     ROSE_ASSERT(nondefdecl->get_definingDeclaration() == defdecl);
     ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != defdecl);

     setOneSourcePositionForTransformation(defdecl);
  // constructor is side-effect free
     classDef->set_declaration(defdecl);
     defdecl->set_definingDeclaration(defdecl);

     defdecl->set_firstNondefiningDeclaration(nondefdecl);

  // DQ (3/22/2012): I think we can assert this.
     ROSE_ASSERT(defdecl->get_type() == NULL);

  // Liao, 10/30/2009
  // The SgClassDeclaration constructor will automatically generate a SgClassType internally if NULL is passed for SgClassType
  // This is not desired when building a defining declaration and an inefficience in the constructor
  // Ideally, only the first nondefining class declaration should have a dedicated SgClassType and
  // the defining class declaration (and other nondefining declaration) just share that SgClassType.
     if (defdecl->get_type() != NULL)
        {
           // Removed several lines of dead code because of the assertion just above
        }
       else
        {
       // DQ (3/15/2012): Make sure that both the defining and non-defining declarations use the same type.
          ROSE_ASSERT (nondefdecl->get_type() != NULL);
          defdecl->set_type(nondefdecl->get_type());

          ROSE_ASSERT (nondefdecl->get_firstNondefiningDeclaration() != NULL);
        }

  // DQ (9/4/2012): Added assertion.
     ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());

  // patch up the SgClassType for the defining class declaration
     ROSE_ASSERT (nondefdecl->get_type() != NULL);
  // ROSE_ASSERT (nondefdecl->get_type()->get_declaration() == isSgDeclarationStatement(nondefdecl));
  // ROSE_ASSERT (defdecl->get_type()->get_declaration() == isSgDeclarationStatement(defdecl));
     ROSE_ASSERT (defdecl->get_type() != NULL);
     ROSE_ASSERT (defdecl->get_type()->get_declaration() != NULL);
     ROSE_ASSERT (defdecl->get_type()->get_declaration() != isSgDeclarationStatement(defdecl));
     ROSE_ASSERT (nondefdecl->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT (nondefdecl->get_firstNondefiningDeclaration() == nondefdecl);

  // DQ (9/4/2012): Added assertion.
     ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());

  // I don't think this is always a forward declaration (e.g. if it is not used in a prototype).
  // Checking the olded EDG/ROSE interface it appears that it is always marked forward (unless
  // used in a defining declaration).
     nondefdecl->setForward();

     if (scope != NULL)  // put into fixStructDeclaration() or alike later on
        {
       // DQ (9/4/2012): Added assertion.
          ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());

       // Note, this function sets the parent to be the scope if it is not already set.
          fixStructDeclaration(defdecl,scope);

       // DQ (9/4/2012): Added assertion.
          ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());

          fixStructDeclaration(nondefdecl,scope);

       // DQ (9/4/2012): Added assertion.
          ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());
        }

  // DQ (1/26/2009): I think we should assert this, but it breaks the interface as defined
  // by the test code in tests/nonsmoke/functional/roseTests/astInterfaceTests.
  // ROSE_ASSERT(defdecl->get_parent() != NULL);

  // ROSE_ASSERT(nonDefiningDecl->get_parent() != NULL);

  // DQ (2/27/2012): Tracking down where parents are not set correctly (class declaration in typedef is incorrectly set to SgGlobal).
     ROSE_ASSERT(defdecl->get_parent() == NULL);

  // DQ (2/29/2012):  We can't assert this (fails for test2012_09.C).
  // ROSE_ASSERT(nondefdecl->get_parent() == NULL);

     ROSE_ASSERT(defdecl->get_definingDeclaration() == defdecl);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != defdecl->get_definingDeclaration());

  // DQ (3/8/2018): Added for debugging.
     SgClassDeclaration* temp_firstNondefiningDeclaration = isSgClassDeclaration(defdecl->get_firstNondefiningDeclaration());
     SgClassDeclaration* temp_definingDeclaration         = isSgClassDeclaration(defdecl->get_definingDeclaration());
     ROSE_ASSERT(temp_firstNondefiningDeclaration != NULL);
     ROSE_ASSERT(temp_definingDeclaration != NULL);

  // DQ (3/8/2018): Added assertion.
     ROSE_ASSERT(temp_firstNondefiningDeclaration->get_name() == temp_definingDeclaration->get_name());
     ROSE_ASSERT(temp_firstNondefiningDeclaration->get_type() == temp_definingDeclaration->get_type());

#if 0
  // DQ (1/27/2019): Test that symbol table to debug Cxx11_tests/test2019)33.C.
     printf ("Leaving buildClassDeclaration_nfi(): Calling find_symbol_from_declaration() \n");
     SgSymbol* test_symbol = nondefdecl->get_scope()->find_symbol_from_declaration(nondefdecl);

  // DQ (1/27/2019): Test that symbol table to debug Cxx11_tests/test2019)33.C.
     printf ("Leaving buildClassDeclaration_nfi(): Calling get_symbol_from_symbol_table() \n");
     ROSE_ASSERT(nondefdecl->get_symbol_from_symbol_table() != NULL);
#endif

     return defdecl;
   }


SgNamespaceDeclarationStatement*
SageBuilder::buildNamespaceDeclaration(const SgName& name, SgScopeStatement* scope /*=NULL*/)
   {
     SgNamespaceDeclarationStatement* defdecl = buildNamespaceDeclaration_nfi(name,false,scope);

     setOneSourcePositionForTransformation(defdecl);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != NULL);
     setOneSourcePositionForTransformation(defdecl->get_firstNondefiningDeclaration());

     return defdecl;
   }

SgNamespaceDeclarationStatement*
SageBuilder::buildNamespaceDeclaration_nfi(const SgName& name, bool unnamednamespace, SgScopeStatement* scope)
   {
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

     ROSE_ASSERT(scope != NULL);

#if 0
     printf ("In SageBuilder::buildNamespaceDeclaration_nfi(): scope = %p = %s = %s \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
#endif

  // TODO How about class type??
  // build defining declaration
     SgNamespaceDefinitionStatement* namespaceDef = buildNamespaceDefinition();

     SgNamespaceDeclarationStatement* defdecl = new SgNamespaceDeclarationStatement(name,namespaceDef,unnamednamespace);
     ROSE_ASSERT(defdecl != NULL);
     namespaceDef->set_parent(defdecl);

#if 0
     printf ("#################### SageBuilder::buildNamespaceDeclaration_nfi(): defdecl = %p = %s namespaceDef = %p \n",defdecl,defdecl->get_name().str(),namespaceDef);
#endif

  // setOneSourcePositionForTransformation(defdecl);
     setOneSourcePositionNull(defdecl);

  // constructor is side-effect free
     namespaceDef->set_namespaceDeclaration(defdecl);

  // DQ (3/6/2012): For namespaces the definingDeclaration should be NULL.
  // defdecl->set_definingDeclaration(defdecl);
     ROSE_ASSERT(defdecl->get_definingDeclaration() == NULL);

  // Get the nondefining declaration from the symbol if it has been built (if this works,
  // then we likely don't need the "SgClassDeclaration* nonDefiningDecl" parameter).
     SgNamespaceDeclarationStatement* nondefdecl = NULL;

  // DQ (1/26/2009): It seems that (scope == NULL) can happen in the tests/nonsmoke/functional/roseTests/astInterfaceTests test codes.
  // ROSE_ASSERT(scope != NULL);
     SgNamespaceSymbol* mysymbol = NULL;
     if (scope != NULL)
        {
          mysymbol = scope->lookup_namespace_symbol(name);
        }
       else
        {
       // DQ (1/26/2009): I think this should be an error, but that appears it would
       // break the existing interface. Need to discuss this with Liao.
          printf ("Warning: In SageBuilder::buildNamespaceDeclaration_nfi(): scope == NULL \n");
        }

#if 0
     printf ("In SageBuilder::buildNamespaceDeclaration_nfi(): mysymbol = %p \n",mysymbol);
#endif

     if (mysymbol != NULL)
        {
       // nondefdecl = isSgNamespaceDeclarationStatement(mysymbol->get_declaration());
          SgNamespaceDeclarationStatement* namespaceDeclaration = mysymbol->get_declaration();
          ROSE_ASSERT(namespaceDeclaration != NULL);
          nondefdecl = isSgNamespaceDeclarationStatement(namespaceDeclaration);

          ROSE_ASSERT(nondefdecl != NULL);
          ROSE_ASSERT(nondefdecl->get_parent() != NULL);

       // DQ (5/16/2013): These should be non-defining declarations for the case of a C++ namespace (all instances are a non-defining declaration).
       // nondefdecl->set_definingDeclaration(defdecl);
       // ROSE_ASSERT(nondefdecl->get_definingDeclaration() == defdecl);
          ROSE_ASSERT(nondefdecl->get_definingDeclaration() == NULL);
          ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != defdecl);

       // DQ (5/16/2013): Set the global definition for the new namespace definition.
          ROSE_ASSERT(namespaceDeclaration->get_definition() != NULL);
          if (namespaceDeclaration->get_definition()->get_global_definition() == NULL)
             {
               printf ("ERROR: namespaceDeclaration->get_definition()->get_global_definition() == NULL: namespaceDeclaration = %p = %s namespaceDeclaration->get_definition() = %p \n",
                    namespaceDeclaration,namespaceDeclaration->get_name().str(),namespaceDeclaration->get_definition());
             }
          ROSE_ASSERT(namespaceDeclaration->get_definition()->get_global_definition() != NULL);
          namespaceDef->set_global_definition(namespaceDeclaration->get_definition()->get_global_definition());
          ROSE_ASSERT(namespaceDef->get_global_definition() != NULL);

       // DQ (5/19/2013): Make the global_definition point to itself.
          ROSE_ASSERT(namespaceDef->get_global_definition() == namespaceDef->get_global_definition()->get_global_definition());

          ROSE_ASSERT(defdecl->get_definition()->get_global_definition() != NULL);

          ROSE_ASSERT(nondefdecl->get_definition()->get_previousNamespaceDefinition() == NULL);
       // ROSE_ASSERT(nondefdecl->get_definition()->get_nextNamespaceDefinition() == NULL);

          SgNamespaceDefinitionStatement* i = namespaceDeclaration->get_definition();
          ROSE_ASSERT(i != NULL);
          while (i != NULL && i->get_nextNamespaceDefinition() != NULL)
             {
               i = i->get_nextNamespaceDefinition();
               ROSE_ASSERT(i->get_previousNamespaceDefinition() != NULL);
             }

          ROSE_ASSERT(i != NULL);
          i->set_nextNamespaceDefinition(namespaceDef);
          namespaceDef->set_previousNamespaceDefinition(i);
        }
       else
        {
       // DQ (5/16/2013): Note that since we don't build a SgNamespaceDefinition for the declaration we can't
       // build the global_definition.  This is a potential problem.

#if 1
          nondefdecl = defdecl;
          ROSE_ASSERT(nondefdecl != NULL);
          namespaceDef = nondefdecl->get_definition();
          ROSE_ASSERT(namespaceDef->get_namespaceDeclaration() != NULL);
#else
       // DQ (5/16/2013): We want to build an associated SgNamespaceDefinitionStatement so that we can
       // support a reference to a SgNamespaceDefinitionStatement as a global definition.
       // DQ (1/25/2009): We only want to build a new declaration if we can't reuse the existing declaration.
       // nondefdecl = new SgNamespaceDeclarationStatement(name,NULL, unnamednamespace);

#error "DEAD CODE!"

          SgNamespaceDefinitionStatement* namespaceDef = buildNamespaceDefinition();
          nondefdecl = new SgNamespaceDeclarationStatement(name,namespaceDef,unnamednamespace);
          ROSE_ASSERT(nondefdecl != NULL);
#if 0
          printf ("In SageBuilder::buildNamespaceDeclaration_nfi(): Built namespace definition for nondefdecl = %p = %s definition = %p \n",nondefdecl,nondefdecl->get_name().str(),namespaceDef);
#endif
       // DQ (5/16/2013): Added tests and setting of the associated declaration.
          ROSE_ASSERT(namespaceDef->get_namespaceDeclaration() == NULL);
          namespaceDef->set_namespaceDeclaration(nondefdecl);
          ROSE_ASSERT(namespaceDef->get_namespaceDeclaration() != NULL);
#endif

       // DQ (5/16/2013): Now add the global definition where we will accumulate all of the symbols for the logical namespace.
          SgNamespaceDefinitionStatement* global_definition_namespaceDef = buildNamespaceDefinition();
          namespaceDef->set_global_definition(global_definition_namespaceDef);
          ROSE_ASSERT(namespaceDef->get_global_definition() != NULL);

       // DQ (5/19/2013): Make the global_definition point to itself.
          global_definition_namespaceDef->set_global_definition(global_definition_namespaceDef);

          global_definition_namespaceDef->set_isUnionOfReentrantNamespaceDefinitions(true);

       // DQ (8/23/2013): Set the parent of the global_definition_namespaceDef.
          ROSE_ASSERT(global_definition_namespaceDef->get_parent() == NULL);
          global_definition_namespaceDef->set_parent(defdecl);
          ROSE_ASSERT(global_definition_namespaceDef->get_parent() != NULL);

       // DQ (5/16/2013): Added tests and setting of the associated declaration.
          ROSE_ASSERT(global_definition_namespaceDef->get_namespaceDeclaration() == NULL);
          global_definition_namespaceDef->set_namespaceDeclaration(nondefdecl);
          ROSE_ASSERT(global_definition_namespaceDef->get_namespaceDeclaration() != NULL);

       // DQ (5/16/2013): Set the associated declaration to be the nondefdecl.
          global_definition_namespaceDef->set_namespaceDeclaration(nondefdecl);
          ROSE_ASSERT(global_definition_namespaceDef->get_namespaceDeclaration() != NULL);

#if 0
          ROSE_ASSERT(defdecl->get_definition()->get_global_definition() == NULL);
       // defdecl->get_definition()->set_global_definition(namespaceDef->get_global_definition());
          defdecl->get_definition()->set_global_definition(global_definition_namespaceDef);
#else
#if 0
          printf ("In SageBuilder::buildNamespaceDeclaration_nfi(): defdecl->get_definition()->get_global_definition() = %p \n",defdecl->get_definition()->get_global_definition());
#endif
          if (defdecl->get_definition()->get_global_definition() == NULL)
             {
               defdecl->get_definition()->set_global_definition(global_definition_namespaceDef);
             }

       // DQ (5/19/2013): Make the global_definition point to itself.
          ROSE_ASSERT(global_definition_namespaceDef == global_definition_namespaceDef->get_global_definition());
#endif
          ROSE_ASSERT(defdecl->get_definition()->get_global_definition() != NULL);
          ROSE_ASSERT(defdecl->get_definition()->get_global_definition() == namespaceDef->get_global_definition());
#if 0
          printf ("In SageBuilder::buildNamespaceDeclaration_nfi(): Built namespace definition for nondefdecl = %p = %s get_global_definition() = %p \n",nondefdecl,nondefdecl->get_name().str(),namespaceDef->get_global_definition());
#endif
       // printf ("SageBuilder::buildNamespaceDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

       // The nondefining declaration will not appear in the source code, but is compiler
       // generated (so we have something about the class that we can reference; e.g in
       // types).  At the moment we make it a transformation, there might be another kind
       // of source position that would be more precise.  FIXME.
       // setOneSourcePositionNull(nondefdecl);
       // setOneSourcePositionForTransformation(nondefdecl);

          nondefdecl->set_firstNondefiningDeclaration(nondefdecl);

       // DQ (3/6/2012): For namespaces the definingDeclaration should be NULL.
       // nondefdecl->set_definingDeclaration(defdecl);
          ROSE_ASSERT(nondefdecl->get_definingDeclaration() == NULL);
          ROSE_ASSERT(defdecl->get_definingDeclaration() == NULL);

          nondefdecl->setForward();

       // nondefdecl->set_parent(topScopeStack());
          nondefdecl->set_parent(scope);
          ROSE_ASSERT(nondefdecl->get_parent());

          if (scope != NULL)
             {
               mysymbol = new SgNamespaceSymbol(name,nondefdecl); // tps: added name to constructor
               scope->insert_symbol(name, mysymbol);
             }
            else
             {
            // DQ (1/26/2009): I think this should be an error, but that appears it would
            // break the existing interface. Need to discuss this with Liao.
               printf ("Warning: no scope provided to support symbol table entry! \n");
             }

          ROSE_ASSERT(defdecl->get_definition() != NULL);
          ROSE_ASSERT(defdecl->get_definition()->get_global_definition() != NULL);

          ROSE_ASSERT(nondefdecl->get_definition()->get_previousNamespaceDefinition() == NULL);
          ROSE_ASSERT(nondefdecl->get_definition()->get_nextNamespaceDefinition() == NULL);
        }

     ROSE_ASSERT(nondefdecl != NULL);

  // printf ("SageBuilder::buildNamespaceDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

  // setOneSourcePositionForTransformation(nondefdecl);
  // setOneSourcePositionNull(nondefdecl);

  // nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
  // nondefdecl->set_definingDeclaration(defdecl);
     defdecl->set_firstNondefiningDeclaration(nondefdecl);

  // I don't think this is always a forward declaration (e.g. if it is not used in a prototype).
  // Checking the olded EDG/ROSE interface it appears that it is always marked forward (unless
  // used in a defining declaration).
     nondefdecl->setForward();

     if (scope != NULL)  // put into fixStructDeclaration() or alike later on
        {
          fixNamespaceDeclaration(nondefdecl,scope);
          fixNamespaceDeclaration(defdecl,scope);
#if 0
          SgClassSymbol* mysymbol = new SgClassSymbol(nondefdecl);
          ROSE_ASSERT(mysymbol);
          scope->insert_symbol(name, mysymbol);
#endif

          // tps namespace has no scope
          //defdecl->set_scope(scope);
          //nondefdecl->set_scope(scope);

       // defdecl->set_parent(scope);

       // DQ (1/25/2009): The scope is not the same as the parent, since the scope is logical, and the parent is structural (note that topScopeStack() is structural).
       // nondefdecl->set_parent(scope);
        //  nondefdecl->set_parent(topScopeStack());
        }

  //   defdecl->set_parent(topScopeStack());

  // DQ (1/26/2009): I think we should assert this, but it breaks the interface as defined
  // by the test code in tests/nonsmoke/functional/roseTests/astInterfaceTests.
  // ROSE_ASSERT(defdecl->get_parent() != NULL);

  // ROSE_ASSERT(nonDefiningDecl->get_parent() != NULL);

  // DQ (3/6/2012): For namespaces the definingDeclaration should be NULL.
  // ROSE_ASSERT(defdecl->get_definingDeclaration() == defdecl);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != defdecl->get_definingDeclaration());

  // DQ (3/6/2012): For namespaces the definingDeclaration should be NULL.
     ROSE_ASSERT(defdecl->get_definingDeclaration() == NULL);

  // DQ (5/16/2013): Added tests.
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(defdecl->get_definition() != NULL);
     ROSE_ASSERT(defdecl->get_definition()->get_global_definition() != NULL);

#if 0
  // DQ (5/19/2013): There should always be proper source file position infomation so this should not be required.
     if (defdecl->get_file_info()->isOutputInCodeGeneration() == true)
        {
          defdecl->get_file_info()->display("In buildNamespaceDeclaration_nfi(): namespaceDeclaration: debug");
        }
  // ROSE_ASSERT(defdecl->get_file_info()->isOutputInCodeGeneration() == false);
#endif

     return defdecl;
   }

// driscoll6 (7/20/11) : Support n-ary operators for python
SgNaryComparisonOp*
SageBuilder::buildNaryComparisonOp(SgExpression* lhs) {
    SgNaryComparisonOp* result = new SgNaryComparisonOp();

    result->get_operands().push_back(lhs);
    lhs->set_parent(result);

    setOneSourcePositionForTransformation(result);
    return result;
}

SgNaryComparisonOp*
SageBuilder::buildNaryComparisonOp_nfi(SgExpression* lhs) {
    SgNaryComparisonOp* result = new SgNaryComparisonOp();

    result->get_operands().push_back(lhs);
    lhs->set_parent(result);

    setOneSourcePositionNull(result);
    return result;
}

SgNaryBooleanOp*
SageBuilder::buildNaryBooleanOp(SgExpression* lhs) {
    SgNaryBooleanOp* result = new SgNaryBooleanOp();

    result->get_operands().push_back(lhs);
    lhs->set_parent(result);

    setOneSourcePositionForTransformation(result);
    return result;
}

SgNaryBooleanOp*
SageBuilder::buildNaryBooleanOp_nfi(SgExpression* lhs) {
    SgNaryBooleanOp* result = new SgNaryBooleanOp();

    result->get_operands().push_back(lhs);
    lhs->set_parent(result);

    setOneSourcePositionNull(result);
    return result;
}

SgStringConversion*
SageBuilder::buildStringConversion(SgExpression* exp) {
    ROSE_ASSERT(exp);
    SgStringConversion* result = new SgStringConversion(exp);
    exp->set_parent(result);

    setOneSourcePositionForTransformation(result);
    return result;
}


SgStringConversion*
SageBuilder::buildStringConversion_nfi(SgExpression* exp) {
    ROSE_ASSERT(exp);
    SgStringConversion* result = new SgStringConversion(exp);
    exp->set_parent(result);

    setOneSourcePositionNull(result);
    return result;
}

// DQ (11/7/2009): Added more uniform support for building class declarations.
SgClassDeclaration*
SageBuilder::buildNondefiningClassDeclaration ( SgName name, SgScopeStatement* scope )
   {
     SgClassDeclaration* defdecl    = NULL;
     SgClassDeclaration* nondefdecl = NULL;

  // DQ (7/27/2012): Note that the input name should not have template argument syntax.
  // I think this could still fail for a function with a name such as "X<Y>"  strange converstion operators.
     ROSE_ASSERT(SageInterface::hasTemplateSyntax(name) == false);

#if 1
     printf ("In buildNondefiningClassDeclaration(): name = %s scope = %p = %s \n",name.str(),scope,scope != NULL ? scope->class_name().c_str() : "NULL");

  // DQ (8/12/2013): If this function were to be called then we would have to
  // support a template argument list for the call to lookup_class_symbol().

  // DQ (6/9/2013): I want to know that I'm not debugging this function.
     ROSE_ASSERT(false);
#endif

  // DQ (1/26/2009): It seems that (scope == NULL) can happen in the tests/nonsmoke/functional/roseTests/astInterfaceTests test codes.
  // ROSE_ASSERT(scope != NULL);
     SgClassSymbol* mysymbol = NULL;
     if (scope != NULL)
        {
       // mysymbol = scope->lookup_class_symbol(name);
          mysymbol = scope->lookup_class_symbol(name,NULL);
        }
       else
        {
       // Liao 9/2/2009: This is not an error. We support bottomup AST construction and scope can be unkown.
       // DQ (1/26/2009): I think this should be an error, but that appears it would
       // break the existing interface. Need to discuss this with Liao.
       // printf ("Warning: In SageBuilder::buildClassDeclaration_nfi(): scope == NULL \n");
        }

#if 0
     printf ("In SageBuilder::buildClassDeclaration_nfi(): mysymbol = %p \n",mysymbol);
#endif

     if (mysymbol != NULL) // set links if nondefining declaration already exists.
        {
          nondefdecl = isSgClassDeclaration(mysymbol->get_declaration());

          ROSE_ASSERT(nondefdecl != NULL);
       // ROSE_ASSERT(nondefdecl->get_parent() != NULL);

          nondefdecl->set_definingDeclaration(defdecl);

          ROSE_ASSERT(nondefdecl->get_definingDeclaration() == defdecl);
          ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != defdecl);

       // DQ (10/30/2010): There should be a properly defined type at this point!
          ROSE_ASSERT(nondefdecl->get_type() != NULL);

       // DQ (7/31/2019): Check that this is true.
       // ROSE_ASSERT(nondefdecl->get_parent() != NULL);
        }
       else // build a nondefnining declaration if it does not exist
        {
       // DQ (1/25/2009): We only want to build a new declaration if we can't reuse the existing declaration.

          SgClassDeclaration::class_types kind = SgClassDeclaration::e_class;
          nondefdecl = new SgClassDeclaration(name,kind,NULL,NULL);
          ROSE_ASSERT(nondefdecl != NULL);
          if (nondefdecl->get_type() == NULL)
             {
               nondefdecl->set_type(SgClassType::createType(nondefdecl));
#if 0
               printf ("In SageBuilder::buildNondefiningClassDeclaration(): built class type: part 3: nondefdecl->get_type() = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
#endif
             }

 //         printf ("SageBuilder::buildClassDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

       // The nondefining declaration will not appear in the source code, but is compiler
       // generated (so we have something about the class that we can reference; e.g in
       // types).  At the moment we make it a transformation, there might be another kind
       // of source position that would be more precise.  FIXME.
       // setOneSourcePositionNull(nondefdecl);
          setOneSourcePositionForTransformation(nondefdecl);

          nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
          nondefdecl->set_definingDeclaration(defdecl);
          nondefdecl->setForward();
       // Liao, 9/2/2009. scope stack is optional, it can be empty
       // nondefdecl->set_parent(topScopeStack());
       // nondefdecl->set_parent(scope);

       // DQ (7/31/2019): Check that this is true.
       // ROSE_ASSERT(nondefdecl->get_parent() != NULL);

       // DQ (3/24/2011): This should be NULL before we set it (if the scope is known).
          ROSE_ASSERT(nondefdecl->get_scope() == NULL);
          if (scope != NULL)
             {
            // DQ (3/24/2011): Decided with Liao that we should set the scope where possible.  The AST consistancy test will make sure it is consistant with where it is inserted into the AST.
               nondefdecl->set_scope(scope);
               ROSE_ASSERT(nondefdecl->get_scope() != NULL);

               mysymbol = new SgClassSymbol(nondefdecl);
#if 0
               printf ("In buildNondefiningClassDeclaration(): Adding SgClassSymbol: mysymbol = %p from nondefdecl = %p = %s to scope = %p = %s \n",mysymbol,nondefdecl,nondefdecl->class_name().c_str(),scope,scope->class_name().c_str());
#endif
               scope->insert_symbol(name, mysymbol);
             }
            else
             {
            // Liao 9/2/2009: This is not an error. We support bottomup AST construction and scope can be unknown.
            // DQ (1/26/2009): I think this should be an error, but that appears it would
            // break the existing interface. Need to discuss this with Liao.
            // printf ("Warning: no scope provided to support symbol table entry! \n");
             }

       // DQ (10/30/2010): There should be a properly defined type at this point!
          ROSE_ASSERT(nondefdecl->get_type() != NULL);

       // DQ (3/24/2011): The scope should be set if the scope was available.
          ROSE_ASSERT(scope == NULL || (scope != NULL && nondefdecl->get_scope() != NULL));
        }

     ROSE_ASSERT(nondefdecl != NULL);

     return nondefdecl;
   }

// DQ (11/7/2009): Added more uniform support for building class declarations.
SgClassDeclaration*
SageBuilder::buildDefiningClassDeclaration ( SgName name, SgScopeStatement* scope )
   {
  // Note that the semantics of this function now differs from that of the buildDefiningFunctionDeclaration().
  // We want to have the non-defining declaration already exist before calling this function.
  // We could still build a higher level function that built both together.  Or we could provide two versions
  // named differently (from this one) and deprecate this function...which I like much better.
     printf ("WARNING: This function for building defining class declarations has different semantics from that of the function to build defining function declarations. \n");

#if 1
     printf ("In buildDefiningClassDeclaration(): name = %s scope = %p = %s \n",name.str(),scope,scope != NULL ? scope->class_name().c_str() : "NULL");

  // DQ (6/9/2013): I want to know that I'm not debugging this function.
     ROSE_ASSERT(false);
#endif

  // DQ (7/27/2012): Note that the input name should not have template argument syntax.
  // I think this could still fail for a function with a name such as "X<Y>"  strange converstion operators.
     ROSE_ASSERT(SageInterface::hasTemplateSyntax(name) == false);

     SgClassDeclaration* nondefiningClassDeclaration = buildNondefiningClassDeclaration(name,scope);
     ROSE_ASSERT(nondefiningClassDeclaration != NULL);

     SgClassDefinition* definingClassDefinition = buildClassDefinition();
     ROSE_ASSERT(definingClassDefinition != NULL);

  // DQ (10/30/2010): There should be a properly defined type at this point!
     SgClassType* classType = nondefiningClassDeclaration->get_type();
     ROSE_ASSERT(classType != NULL);

     SgClassDeclaration::class_types kind = SgClassDeclaration::e_class;

  // DQ (10/30/2010): We need to make sure that there is a type defined.
  // SgClassDeclaration* definingClassDeclaration = new SgClassDeclaration (name,kind,NULL,definingClassDefinition);
     SgClassDeclaration* definingClassDeclaration = new SgClassDeclaration (name,kind,classType,definingClassDefinition);
     ROSE_ASSERT(definingClassDeclaration != NULL);

  // printf ("SageBuilder::buildDefiningClassDeclaration(): definingClassDeclaration = %p \n",definingClassDeclaration);

     setOneSourcePositionForTransformation(definingClassDeclaration);

  // constructor is side-effect free
     definingClassDefinition->set_declaration(definingClassDeclaration);
     definingClassDeclaration->set_definingDeclaration(definingClassDeclaration);
     definingClassDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);

     nondefiningClassDeclaration->set_definingDeclaration(definingClassDeclaration);

  // some error checking
     ROSE_ASSERT(nondefiningClassDeclaration->get_definingDeclaration() != NULL);
     ROSE_ASSERT(nondefiningClassDeclaration->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(definingClassDeclaration->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(definingClassDeclaration->get_definition() != NULL);

     ROSE_ASSERT(definingClassDeclaration->get_scope() == NULL);
     if (scope != NULL)
        {
          definingClassDeclaration->set_scope(scope);
          ROSE_ASSERT(definingClassDeclaration->get_scope() != NULL);
          ROSE_ASSERT(nondefiningClassDeclaration->get_scope() != NULL);
        }

     ROSE_ASSERT(definingClassDeclaration->get_definition()->get_parent() != NULL);

  // DQ (10/30/2010): There should be a properly defined type at this point!
     ROSE_ASSERT(definingClassDeclaration->get_type() != NULL);

     return definingClassDeclaration;
   }

// DQ (11/7/2009): Added more uniform support for building class declarations.
SgClassDeclaration*
SageBuilder::buildClassDeclaration ( SgName name, SgScopeStatement* scope )
   {
     ROSE_ASSERT(scope != NULL);
     SgClassDeclaration* definingClassDeclaration = buildDefiningClassDeclaration(name,scope);
     ROSE_ASSERT(definingClassDeclaration != NULL);

     return definingClassDeclaration;
   }


// DQ (6/6/2012): Added support for template arguments (so that the type could be computing using the template arguments when building a template instantiation).
// DQ (1/24/2009): Built this "nfi" version but factored the code.
// SgClassDeclaration* SageBuilder::buildClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgClassDeclaration* nonDefiningDecl , bool buildTemplateInstantiation )
SgClassDeclaration*
SageBuilder::buildClassDeclaration_nfi(const SgName& XXX_name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgClassDeclaration* nonDefiningDecl , bool buildTemplateInstantiation, SgTemplateArgumentPtrList* templateArgumentsList )
   {
  // DQ (3/15/2012): Added function to build C++ class (builds both the non-defining and defining declarations; in that order).
  // The implementation of this function could be simplified to directly call both:
  //    SgClassDeclaration* buildNondefiningClassDeclaration ( SgName name, SgScopeStatement* scope );
  // and
  //    SgClassDeclaration* buildDefiningClassDeclaration    ( SgName name, SgScopeStatement* scope );
  // This might refactor the implementation nicely.

#define DEBUG_CLASS_DECLARATION 0

  // Note that the nonDefiningDecl pointer does not appear to be used.
#if 0
     printf ("WARNING: In SageBuilder::buildClassDeclaration_nfi(): the nonDefiningDecl pointer = %p (input parameter) does not appear to be used. \n",nonDefiningDecl);
#endif

#if 0
  // DQ (3/4/2018): Adding testing.
  // ROSE_ASSERT(nonDefiningDecl != NULL);
     if (nonDefiningDecl != NULL)
        {
          ROSE_ASSERT(nonDefiningDecl->get_type() != NULL);
          ROSE_ASSERT(nonDefiningDecl->get_type()->get_declaration() != NULL);
          printf ("nonDefiningDecl->get_type() = %p = %s \n",nonDefiningDecl->get_type(),nonDefiningDecl->get_type()->class_name().c_str());
          printf ("nonDefiningDecl->get_type()->get_declaration() = %p = %s \n",nonDefiningDecl->get_type()->get_declaration(),nonDefiningDecl->get_type()->get_declaration()->class_name().c_str());
#if 0
          printf ("In buildClassDeclaration_nfi(): nonDefiningDecl: unparseNameToString() = %s \n",nonDefiningDecl->unparseNameToString().c_str());
#endif

        }
#endif


  // DQ (10/10/2015): I think we can assert this! NO we can't (see test2015_87.C).
  // ROSE_ASSERT(nonDefiningDecl != NULL);

  // DQ (10/10/2015): OK, now we have a valid use on the input non-defining declaration.
     bool buildTemplateDeclaration = (isSgTemplateClassDeclaration(nonDefiningDecl) != NULL);

  // DQ (10/10/2015): If this is true, then we should have called a different function to build the associated SgTemplateClassDeclaration.
     if (buildTemplateDeclaration == true)
        {
       // Error checking.
          printf ("ERROR: If buildTemplateDeclaration == true, then we should have called a different function to build the associated SgTemplateClassDeclaration \n");
        }
     ROSE_ASSERT(buildTemplateDeclaration == false);

#if 0
     printf ("In SageBuilder::buildClassDeclaration_nfi(): XXX_name = %s \n",XXX_name.str());
     printf ("In SageBuilder::buildClassDeclaration_nfi(): the nonDefiningDecl pointer = %p = %s \n",nonDefiningDecl,nonDefiningDecl != NULL ? nonDefiningDecl->class_name().c_str() : "null");
     printf ("In SageBuilder::buildClassDeclaration_nfi(): buildTemplateDeclaration    = %s \n",buildTemplateDeclaration ? "true" : "false");
     printf ("   --- templateArgumentsList = %p \n",templateArgumentsList);
     if (templateArgumentsList != NULL)
        {
          printf ("   --- templateArgumentsList.size() = %zu \n",templateArgumentsList->size());
          for (size_t i = 0; i < templateArgumentsList->size(); i++)
             {
               printf ("   --- --- argument pointer: templateArgumentsList->[%zu] = %p \n",i,templateArgumentsList->operator[](i));
             }
        }
#endif

     if (scope == NULL)
        {
          scope = SageBuilder::topScopeStack();
#if 0
          printf ("In SageBuilder::buildClassDeclaration_nfi(): no scope was provided so using the SageBuilder::topScopeStack() = %p = %s \n",scope,scope->class_name().c_str());
#endif
        }
       else
        {
#if 0
          printf ("In SageBuilder::buildClassDeclaration_nfi(): scope was provided scope = %p = %s \n",scope,scope->class_name().c_str());
#endif
        }

#if 0
     printf ("Building a SgClassDeclaration: buildClassDeclaration_nfi() XXX_name = %s buildTemplateInstantiation = %s \n",XXX_name.str(),buildTemplateInstantiation ? "true" : "false");
#endif

  // Step 2 (now step 1). build the nondefining declaration,
  // but only if the input nonDefiningDecl pointer was NULL and it does not exist

  // Get the nondefining declaration from the symbol if it has been built (if this works,
  // then we likely don't need the "SgClassDeclaration* nonDefiningDecl" parameter).
     SgClassDeclaration* nondefdecl = NULL;

     SgName nameWithoutTemplateArguments = XXX_name;
     SgName nameWithTemplateArguments    = nameWithoutTemplateArguments;
     if (buildTemplateInstantiation == true)
        {
          ROSE_ASSERT(templateArgumentsList != NULL);
          nameWithTemplateArguments = appendTemplateArgumentsToName(nameWithoutTemplateArguments,*templateArgumentsList);
        }

#if DEBUG_CLASS_DECLARATION
      printf ("In SageBuilder::buildClassDeclaration_nfi():\n");
      printf ("  -- nameWithoutTemplateArguments = %s\n", nameWithoutTemplateArguments.str());
      printf ("  -- nameWithTemplateArguments    = %s\n", nameWithTemplateArguments.str());
#endif

  // DQ (7/27/2012): Note that the input name should not have template argument syntax.
  // I think this could still fail for a function with a name such as "X<Y>"  strange converstion operators.
  // This fails for test2005_35.C
  // ROSE_ASSERT(SageInterface::hasTemplateSyntax(nameWithoutTemplateArguments) == false);

  // DQ (1/26/2009): It seems that (scope == NULL) can happen in the tests/nonsmoke/functional/roseTests/astInterfaceTests test codes.
  // ROSE_ASSERT(scope != NULL);
     SgClassSymbol* mysymbol = NULL;
     if (scope != NULL)
        {
#if DEBUG_CLASS_DECLARATION
          printf ("Looking up the SgClassSymbol in scope = %p = %s nameWithTemplateArguments = %s \n",scope,scope->class_name().c_str(),nameWithTemplateArguments.str());
#endif

       // DQ (8/22/2012): We need to provide more information ofr the symbol table lookup to correctly resolve
       // (and disambiguate template instantations where the name qualification of the template arguments would
       // be significant).
       // mysymbol = scope->lookup_class_symbol(name);
       // mysymbol = scope->lookup_class_symbol(name);
       // mysymbol = scope->lookup_class_symbol(nameWithTemplateArguments);
#if 0
       // DQ (7/25/2017): Since this is overwritten below, for both branches, we don't need to call this here.
          printf ("This was a redundant call to lookup_class_symbol \n");
       // mysymbol = scope->lookup_class_symbol(nameWithTemplateArguments,templateArgumentsList);
#endif

       // DQ (10/10/2015): look up the correct type of symbol.
          if (buildTemplateDeclaration == true)
             {
#if DEBUG_CLASS_DECLARATION
               printf ("Note: In SageBuilder::buildClassDeclaration_nfi(): Need to look up a template symbol \n");
#endif
               ROSE_ASSERT(nonDefiningDecl != NULL);

               SgTemplateParameterPtrList templateParameterList;
               SgTemplateArgumentPtrList templateSpecializationArgumentList;

               ROSE_ASSERT(scope->lookup_template_class_symbol(nameWithTemplateArguments,&templateParameterList,&templateSpecializationArgumentList) != NULL);

               mysymbol = scope->lookup_template_class_symbol(nameWithTemplateArguments,&templateParameterList,&templateSpecializationArgumentList);

               ROSE_ASSERT(mysymbol != NULL);
#if 0
               printf ("ERROR: Need to look up a template symbol \n");
               ROSE_ASSERT(false);
#endif
             }
            else
             {
#if DEBUG_CLASS_DECLARATION
               printf ("In SageBuilder::buildClassDeclaration_nfi(): calling lookup_class_symbol(nameWithTemplateArguments = %s,templateArgumentsList->size() = %zu \n",
                       nameWithTemplateArguments.str(),(templateArgumentsList != NULL) ? templateArgumentsList->size() : 999);
               if (templateArgumentsList != NULL)
                  {
                    printf ("   --- templateArgumentsList elements: \n");
                    for (size_t i = 0; i < templateArgumentsList->size(); i++)
                       {
                         printf ("   --- --- templateArgumentsList->[%zu] = %p \n",i,templateArgumentsList->operator[](i));
                         printf ("   --- --- templateArgumentsList->[%zu] = %s \n",i,templateArgumentsList->operator[](i)->class_name().c_str());
                         templateArgumentsList->operator[](i)->display("In SageBuilder::buildClassDeclaration_nfi()");
                       }
                  }
#endif
               mysymbol = scope->lookup_class_symbol(nameWithTemplateArguments,templateArgumentsList);
#if DEBUG_CLASS_DECLARATION
               printf ("In SageBuilder::buildClassDeclaration_nfi(): mysymbol = %p \n",mysymbol);
#endif
            // DQ (3/4/2018): The only time I see this failing is when we should have used the nonDefiningDecl (see Cxx11_tests/test2015_08.C).
               if (mysymbol == NULL)
                  {
#if DEBUG_CLASS_DECLARATION
                    printf ("WARNING: scope->lookup_class_symbol(nameWithTemplateArguments = %s,templateArgumentsList->size() = %zu) == NULL \n",nameWithTemplateArguments.str(),templateArgumentsList->size());
#endif
                 // ROSE_ASSERT(nonDefiningDecl != NULL);

                 // DQ (12/28/2018): Could it be that we wanted to use the name without template arguments.
#if DEBUG_CLASS_DECLARATION
                    printf ("Checking lookup_class_symbol() using nameWithoutTemplateArguments = %s \n",nameWithoutTemplateArguments.str());
#endif
                    ROSE_ASSERT(scope->lookup_class_symbol(nameWithoutTemplateArguments,templateArgumentsList) == NULL);

#if DEBUG_CLASS_DECLARATION
                    printf ("nonDefiningDecl = %p \n",nonDefiningDecl);
#endif
                    if (nonDefiningDecl != NULL)
                       {
#if DEBUG_CLASS_DECLARATION
                         printf ("nonDefiningDecl = %p = %s \n",nonDefiningDecl,nonDefiningDecl->class_name().c_str());
#endif
                      // DQ (3/4/2018): I think this is the correct API to use (internal use only).
                         SgSymbol* temp_mysymbol = nonDefiningDecl->get_symbol_from_symbol_table();
                         ROSE_ASSERT(temp_mysymbol != NULL);

                         mysymbol = isSgClassSymbol(temp_mysymbol);
                         ROSE_ASSERT(mysymbol != NULL);

                      // DQ (3/4/2018): check that the scopes are the same.
                         ROSE_ASSERT(scope == nonDefiningDecl->get_scope());
                       }
                  }
             }

#if 0
       // DQ (11/21/2013): Added test based on debugging session with Philippe.
       // This test is not a test for a bug, since we require that symbols in base classes be aliased in the derived classes.
          if (mysymbol != NULL)
             {
               SgClassDeclaration* symbol_declaration = isSgClassDeclaration(mysymbol->get_declaration());
               ROSE_ASSERT(symbol_declaration != NULL);
               ROSE_ASSERT(symbol_declaration->get_scope() == scope);

               printf ("In SageBuilder::buildClassDeclaration_nfi(): Testing scope->get_symbol_table()->exists(mysymbol) == true (expensive) \n");

               ROSE_ASSERT(scope->get_symbol_table()->exists(mysymbol) == true);
             }
#endif
        }
       else
        {
       // Liao 9/2/2009: This is not an error. We support bottomup AST construction and scope can be unknow.
       // DQ (1/26/2009): I think this should be an error, but that appears it would
       // break the existing interface. Need to discuss this with Liao.
       // printf ("Warning: In SageBuilder::buildClassDeclaration_nfi(): scope == NULL \n");
        }

#if DEBUG_CLASS_DECLARATION
     printf ("In SageBuilder::buildClassDeclaration_nfi(): mysymbol = %p \n",mysymbol);
#endif

     if (mysymbol != NULL) // set links if nondefining declaration already exists.
        {
          nondefdecl = isSgClassDeclaration(mysymbol->get_declaration());

          ROSE_ASSERT(nondefdecl != NULL);

#if DEBUG_CLASS_DECLARATION
          printf ("In SageBuilder::buildClassDeclaration_nfi(): mysymbol->get_declaration(): nondefdecl = %p = %s nondefdecl->get_definition() = %p = %s \n",
               nondefdecl,nondefdecl->class_name().c_str(),nondefdecl->get_definition(),
               nondefdecl->get_definition() != NULL ? nondefdecl->get_definition()->class_name().c_str() : "NULL");
#endif
       // DQ (6/8/2013): This should not be true (see test2013_198.C).
       // Fundamentally the symbol should always only have a pointer to a non-defining
       // declaration, where by definition (get_definition() == NULL).
          ROSE_ASSERT(nondefdecl->get_definition() == NULL);

       // DQ (9/16/2012): This should be true by definition (verify).
          ROSE_ASSERT(nondefdecl == nondefdecl->get_firstNondefiningDeclaration());

       // DQ (9/16/2012): The declaration was build previously, but test it to make sure the template arguments were setup properly.
          testTemplateArgumentParents(nondefdecl);

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
       // DQ (5/2/2012): After EDG/ROSE translation, there should be no IR nodes marked as transformations.
          detectTransformations(nondefdecl);
#endif

       // DQ (3/22/2012): I think we can assert this.
          ROSE_ASSERT(nondefdecl->get_type() != NULL);

       // ROSE_ASSERT(nondefdecl->get_parent() != NULL);
          if (nondefdecl->get_parent() == NULL)
             {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("In SageBuilder::buildClassDeclaration_nfi(): Note that nondefdecl->get_parent() == NULL, this might be OK. \n");
#endif
             }

#if 0
       // DQ (12/22/2019): This is the code that causes the class declarations between defining 
       // class declarations across multiple translation units to be shared.

       // DQ (9/7/2012): I think this might be the root of a problem in the haskell tests (ROSE compiling ROSE).
          if (nondefdecl->get_definingDeclaration() != NULL)
             {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("ERROR: In SageBuilder::buildClassDeclaration_nfi(): Non defining declaration nondefdecl = %p = %s already has a defining declaration, so we would be build another nondefdecl->get_definingDeclaration() = %p = %s \n",
                    nondefdecl,nondefdecl->class_name().c_str(),nondefdecl->get_definingDeclaration(),nondefdecl->get_definingDeclaration()->class_name().c_str());
#endif
               SgClassDeclaration* nondefining_classDeclaration = isSgClassDeclaration(nondefdecl);
               ROSE_ASSERT(nondefining_classDeclaration != NULL);
               SgClassDeclaration* defining_classDeclaration = isSgClassDeclaration(nondefdecl->get_definingDeclaration());
               ROSE_ASSERT(defining_classDeclaration != NULL);

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("In SageBuilder::buildClassDeclaration_nfi(): nondefining_classDeclaration: scope = %p = %s name = %s \n",
                    nondefining_classDeclaration->get_scope(),nondefining_classDeclaration->get_scope()->class_name().c_str(),nondefining_classDeclaration->get_name().str());
               printf ("In SageBuilder::buildClassDeclaration_nfi(): defining_classDeclaration:    scope = %p = %s name = %s \n",
                    defining_classDeclaration->get_scope(),defining_classDeclaration->get_scope()->class_name().c_str(),defining_classDeclaration->get_name().str());
               defining_classDeclaration->get_file_info()->display("already has a defining declaration");
#endif
#if 0
               printf ("Error: In SageBuilder::buildClassDeclaration_nfi(): exiting as part of test \n");
               ROSE_ASSERT(false);
#endif
            // DQ (9/24/2012): This only appears to happen for large tests (e.g. ROSE compiling ROSE), alow it for the moment and look into this later.
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("WARNING: In SageBuilder::buildClassDeclaration_nfi(): but a defining declaration was found to have already been built (might be an error), so returning it defining_classDeclaration = %p \n",defining_classDeclaration);
#endif

#if 0
            // DQ (2/26/2019): Debugging support for multiple files on the command line.
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               return defining_classDeclaration;
             }
#endif

#if 0
          nondefdecl->set_definingDeclaration(defdecl);

          ROSE_ASSERT(nondefdecl->get_definingDeclaration() == defdecl);
          ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != defdecl);
#endif
        }
       else // build a nondefnining declaration if it does not exist
        {
#if DEBUG_CLASS_DECLARATION
          printf ("In SageBuilder::buildClassDeclaration_nfi(): building a nondefining declaration since it does not exist \n");
#endif
       // DQ (10/10/2015): This should be true.
          ROSE_ASSERT(nondefdecl == NULL);

       // DQ (1/25/2009): We only want to build a new declaration if we can't reuse the existing declaration.
       // DQ (1/1/2012): Fixed to force matching types or IR nodes for defining and non-defining declarations.
          if (buildTemplateInstantiation == true)
             {
            // This adds: SgTemplateDeclaration *templateDeclaration and SgTemplateArgumentPtrList templateArguments
#if DEBUG_CLASS_DECLARATION
               printf ("************************************************************************* \n");
               printf ("Building SgTemplateInstantiationDecl with empty SgTemplateArgumentPtrList \n");
               printf ("   --- using nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());
               printf ("************************************************************************* \n");
#endif
               SgTemplateArgumentPtrList emptyList;
            // nondefdecl = new SgTemplateInstantiationDecl (name,kind,NULL,NULL,NULL,emptyList);
               nondefdecl = new SgTemplateInstantiationDecl (nameWithTemplateArguments,kind,NULL,NULL,NULL,emptyList);
               ROSE_ASSERT(nondefdecl != NULL);
#if DEBUG_CLASS_DECLARATION
               printf ("In SageBuilder::buildClassDeclaration_nfi(): Build SgTemplateInstantiationDecl: nondefdecl = %p \n",nondefdecl);
#endif
            // DQ (2/27/2018): Added assertion now that we have implemented more consistant semantics
            // for template instantiations (types are not generated in the constructor calls).
               ROSE_ASSERT(nondefdecl->get_type() == NULL);
               ROSE_ASSERT(isSgTemplateInstantiationDecl(nondefdecl) != NULL);
#if 0
               printf ("In buildClassDeclaration_nfi(): nondefdecl->get_name() = %s nondefdecl->get_templateName() = %s \n",nondefdecl->get_name().str(),isSgTemplateInstantiationDecl(nondefdecl)->get_templateName().str());
#endif
#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
            // DQ (5/2/2012): After EDG/ROSE translation, there should be no IR nodes marked as transformations.
            // detectTransformations(nondefdecl);
#endif
            // DQ (6/6/2012): Set the first non-defining declaration to be itself.
            // nondefdecl->set_firstNondefiningDeclaration(nondefdecl);

            // DQ (1/1/2012): Added support for setting the template name (I think this should be fixed in the constructor).
            // It can't be fixed in the constructor since it has to be set after construction (or passed in explicitly).
               ROSE_ASSERT(isSgTemplateInstantiationDecl(nondefdecl)->get_templateName().is_null() == true);

            // DQ (6/6/2012): Added support for template arguments so that types would be computed with the template arguments.
               ROSE_ASSERT(templateArgumentsList != NULL);

#if 0
            // DQ (5/30/2014): Removing output spew.
            // DQ (5/17/2014): This must be allowed for some template instantiations (see test2014_77.C).
            // This occurs now under some revised rules for when to interpret a class or struct as a template
            // declaration or template instantiation declaration. This revisions is required for test2014_56.C
            // but has had a small cascading effect on other parts of ROSE (all fixed on 5/17/2014, if I can
            // finish this work today).
            // ROSE_ASSERT(templateArgumentsList->size() > 0);
               if (templateArgumentsList->size() == 0)
                  {
                    printf ("Warning: In SageBuilder::buildClassDeclaration_nfi(): templateArgumentsList->size() == 0 \n");
                  }
#endif
            // DQ (9/16/2012): Set the firstNondefiningDeclaration so that we can set the template parameters.
               nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
#if 1
            // DQ (9/16/2012): This is a newly refactored function (call this after the firstNondefiningDeclaration is set).
               setTemplateArgumentsInDeclaration(nondefdecl,templateArgumentsList);
#else
               isSgTemplateInstantiationDecl(nondefdecl)->get_templateArguments() = *templateArgumentsList;

#error "DEAD CODE!"

            // DQ (9/13/2012): Set the parents of the template arguments (if not already set, to the first non-defining declaration).
            // printf ("Calling setTemplateArgumentParents(): nondefdecl = %p = %s \n",nondefdecl,nondefdecl->class_name().c_str());
               setTemplateArgumentParents(nondefdecl);
            // printf ("DONE: Calling setTemplateArgumentParents(): nondefdecl = %p = %s \n",nondefdecl,nondefdecl->class_name().c_str());

               testTemplateArgumentParents(nondefdecl);
#endif
            // DQ (6/6/2012): Generate the name without the template arguments.
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("Warning: In buildClassDeclaration_nfi(): calling set_templateName(nameWithTemplateArguments = %s) for nondefining declaration \n",nameWithTemplateArguments.str());
#endif
            // isSgTemplateInstantiationDecl(nondefdecl)->set_templateName(name);
            // isSgTemplateInstantiationDecl(nondefdecl)->set_templateName("SETME_NONDEFINING_DECL<>");
            // isSgTemplateInstantiationDecl(nondefdecl)->set_templateName(name);
               isSgTemplateInstantiationDecl(nondefdecl)->set_templateName(nameWithoutTemplateArguments);

            // DQ (6/6/2012): I don't think we want this test any more (should apply only to the result of get_templateName()).
            // DQ (5/31/2012): Find locations where this is set and include template syntax.
            // ROSE_ASSERT(name.getString().find('<') == string::npos);
            // printf ("Commented out test for: name.getString().find('<') == string::npos (should apply only to the result of get_templateName() \n");

               ROSE_ASSERT(isSgTemplateInstantiationDecl(nondefdecl)->get_templateName().is_null() == false);

            // DQ (3/25/2017): Fixed Clang warning: warning: if statement has empty body [-Wempty-body]
            // DQ (3/22/2012): Make sure there is template syntax present.
            // if (isSgTemplateInstantiationDecl(nondefdecl)->get_templateName().getString().find('>') == string::npos)
            // if (hasTemplateSyntax(isSgTemplateInstantiationDecl(nondefdecl)->get_templateName()) == false);
               if (hasTemplateSyntax(isSgTemplateInstantiationDecl(nondefdecl)->get_templateName()) == false)
                  {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                    printf ("WARNING: No template syntax present in name of template class instantiation (nondefdecl) \n");
#endif
                  }
            // ROSE_ASSERT(isSgTemplateInstantiationDecl(nondefdecl)->get_templateName().getString().find('>') != string::npos);

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
            // DQ (5/2/2012): After EDG/ROSE translation, there should be no IR nodes marked as transformations.
            // detectTransformations(nondefdecl);
#endif
            // DQ (7/25/2017): This will be true, but it might not be what we want since it can be caught as an error in the code below.
               ROSE_ASSERT(nondefdecl->get_file_info() == NULL);
             }
            else
             {
            // We know that the name without template arguments should be used here (but they are the same).
#if DEBUG_CLASS_DECLARATION
               printf ("WARNING: In buildClassDeclaration_nfi(): Are we building a new SgClassDeclaration as a nondefining declaration when we should be using the nonDefiningDecl = %p \n",nonDefiningDecl);
               printf ("   --- nameWithoutTemplateArguments = %s \n",nameWithoutTemplateArguments.str());
#endif
            // nondefdecl = new SgClassDeclaration(name,kind,NULL,NULL);
               nondefdecl = new SgClassDeclaration(nameWithoutTemplateArguments,kind,NULL,NULL);

               ROSE_ASSERT(nondefdecl != NULL);
#if DEBUG_CLASS_DECLARATION
               printf ("In buildClassDeclaration_nfi(): (no file info set): nondefdecl = %p = %s \n",nondefdecl,nondefdecl->class_name().c_str());
#endif
            // DQ (10/9/2015): Added assertion. We can't assert this yet (see test2015_87.C).
            // ROSE_ASSERT(nondefdecl->get_type() != NULL);

               ROSE_ASSERT(nameWithoutTemplateArguments == nameWithTemplateArguments);

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
            // DQ (5/2/2012): After EDG/ROSE translation, there should be no IR nodes marked as transformations.
            // detectTransformations(nondefdecl);
#endif
            // DQ (9/16/2012): Set the firstNondefiningDeclaration because this is the one branch left were it
            // was not set (required in the true branch so that we could set the template parameters).
               nondefdecl->set_firstNondefiningDeclaration(nondefdecl);

               testTemplateArgumentParents(nondefdecl);

            // DQ (7/25/2017): This will be true, but it might not be what we want since it can be caught as an error in the code below.
               ROSE_ASSERT(nondefdecl->get_file_info() == NULL);
             }

          ROSE_ASSERT(nondefdecl != NULL);

       // DQ (6/6/2012): This has to be set before we generate the type.
       // nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
          ROSE_ASSERT(nondefdecl == nondefdecl->get_firstNondefiningDeclaration());

       // DQ (9/16/2012): This is a newly refactored function (call this after the firstNondefiningDeclaration is set).
       // setTemplateArgumentsInDeclaration(nondefdecl,templateArgumentsList);

       // DQ (3/14/2012): For C++ we need the scope set so that types will have proper locations to revolve them
       // from being ambiguous or not properly defined.  Basically, we need a handle from which to generate something
       // that amounts to a kind of name qualification internally (maybe even exactly name qualification, but I would
       // have to think about that a bit more).
          ROSE_ASSERT(scope != NULL);

#if DEBUG_CLASS_DECLARATION
          printf ("In SageBuilder::buildClassDeclaration_nfi(): Set the scope of the new non-defining declaration to %p = %s \n",scope,scope->class_name().c_str());
#endif
          nondefdecl->set_scope(scope);
          ROSE_ASSERT(nondefdecl->get_scope() != NULL);

       // DQ (8/2/2019): The was required becuase the parent pointers were not being set when reading a file from the SageBuilder::buildFil() API.
       // However the bug was that the astPostprocessing's call to resetParentPointersInMemoryPool() was not properly working to find the global 
       // scope in anyother case but when it was called usign a SgProject node.  This is not fixed to permit caloling using a SgSourceFile node
       // and it is now an error to call it using any other kind of IR node.
       // DQ (8/1/2019): Set the parent for the non defining declaration to be the same as the scope by default.
       // nondefdecl->set_parent(scope);
#if 0
          printf ("In buildClassDeclaration_nfi(): setting the parent of the non defining declaration to be the scope by default) \n");
#endif
       // DQ (7/31/2019): Check that the parent is set if this was used a the declaration referenced by a symbol.
       // ROSE_ASSERT (nondefdecl->get_parent() != NULL);

       // DQ (3/22/2012): I think we can assert this.
       // ROSE_ASSERT(nondefdecl->get_type() != NULL);
          ROSE_ASSERT(nondefdecl->get_type() == NULL);

          if (nondefdecl->get_type() == NULL)
             {
#if DEBUG_CLASS_DECLARATION
            // DQ (12/27/2018): If we have already built a type, then why did we need to build a nondefining declaration?
               printf ("Calling scope->get_type_table()->lookup_type(): nameWithTemplateArguments = %s \n",nameWithTemplateArguments.str());

               printf ("WE NEED THE MANGLED NAME FOR THIS TO BE RELEVANT! \n");

            // SgType* existingType = scope->get_type_table()->lookup_type(nameWithTemplateArguments);
            // ROSE_ASSERT(existingType == NULL);
#endif

#if DEBUG_CLASS_DECLARATION
               printf ("In SageBuilder::buildClassDeclaration_nfi(): kind == SgClassDeclaration::e_java_parameter = %s \n",(kind == SgClassDeclaration::e_java_parameter) ? "true" : "false");
#endif
               SgClassType *class_type = (kind == SgClassDeclaration::e_java_parameter
                                                ? (SgClassType *) SgJavaParameterType::createType(nondefdecl)
                                                : (SgClassType *) SgClassType::createType(nondefdecl));
#if DEBUG_CLASS_DECLARATION
               printf ("In SageBuilder::buildClassDeclaration_nfi(): nondefdecl->get_type() == NULL: building a new class_type = %p = %s \n",class_type,class_type->class_name().c_str());
#endif
               nondefdecl->set_type(class_type);
#if DEBUG_CLASS_DECLARATION
               printf ("In SageBuilder::buildNondefiningClassDeclaration(): built class type: part 4: nondefdecl->get_type() = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
#endif
               SgClassDeclaration* tmp_classDeclarationFromType = isSgClassDeclaration(class_type->get_declaration());
               ROSE_ASSERT(tmp_classDeclarationFromType != NULL);
#if DEBUG_CLASS_DECLARATION
               SgScopeStatement* scope = tmp_classDeclarationFromType->get_scope();
               printf ("tmp_classDeclarationFromType: scope = %p = %s \n",scope,scope->class_name().c_str());
               printf ("tmp_classDeclarationFromType = %p = %s \n",tmp_classDeclarationFromType,tmp_classDeclarationFromType->class_name().c_str());
               printf ("tmp_classDeclarationFromType name = %s \n",tmp_classDeclarationFromType->get_name().str());
               if (tmp_classDeclarationFromType->get_file_info() != NULL)
                  {
                    tmp_classDeclarationFromType->get_file_info()->display("tmp_classDeclarationFromType: debug");
                  }
#endif
             }

       // DQ (3/22/2012): Added assertions.
          ROSE_ASSERT(nondefdecl->get_type() != NULL);
          if (nondefdecl->get_type()->get_declaration() != nondefdecl)
             {
               printf ("ERROR: nondefdecl = %p = %s \n",nondefdecl,nondefdecl->class_name().c_str());
               printf ("ERROR: nondefdecl->get_type() = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
               printf ("ERROR: nondefdecl->get_type()->get_declaration() = %p = %s \n",nondefdecl->get_type()->get_declaration(),nondefdecl->get_type()->get_declaration()->class_name().c_str());

               SgClassDeclaration* classDeclarationFromType = isSgClassDeclaration(nondefdecl->get_type()->get_declaration());
               ROSE_ASSERT(classDeclarationFromType != NULL);

               printf ("nondefdecl->get_name() = %s \n",nondefdecl->get_name().str());
               printf ("nondefdecl->get_type()->get_name() = %s \n",nondefdecl->get_type()->get_name().str());
               printf ("nondefdecl->get_type()->get_declaration()->get_name() = %s \n",classDeclarationFromType->get_name().str());

               printf ("nondefdecl->get_mangled_name() = %s \n",nondefdecl->get_mangled_name().getString().c_str());
               printf ("nondefdecl->get_type()->get_mangled() = %s \n",nondefdecl->get_type()->get_mangled().getString().c_str());
               printf ("nondefdecl->get_type()->get_declaration()->get_mangled_name() = %s \n",classDeclarationFromType->get_mangled_name().getString().c_str());

            // DQ (12/27/2018): Added additional debugging support.
               printf ("nondefdecl->get_type()->get_declaration()->get_firstNondefiningDeclaration() = %s \n",classDeclarationFromType->get_firstNondefiningDeclaration() ? "true" : "false");
               printf ("nondefdecl->get_firstNondefiningDeclaration()                                = %s \n",nondefdecl->get_firstNondefiningDeclaration() ? "true" : "false");

            // DQ (12/27/2018): I think that if this is a base class declaration then it is OK for the type's declaration to not match.
            // ROSE_ASSERT(nondefdecl->get_parent() != NULL);
               if (nondefdecl->get_parent() != NULL)
                  {
                    printf ("nondefdecl->get_parent() = %p = %s \n",nondefdecl->get_parent(),nondefdecl->get_parent()->class_name().c_str());
                  }

            // DQ (12/27/2018): Activate this debugging support.
#if DEBUG_CLASS_DECLARATION
               nondefdecl->get_type()->get_declaration()->get_file_info()->display("nondefdecl->get_type()->get_declaration()");

            // DQ (7/24/2017): Added more debug information to support debugging test2014_187.C.
            // Note that this can be caught as an error if the class declaration was built in the code above when
            // the symbol was not found.  But if the nondefdecl->get_type()->get_declaration() == nondefdecl,
            // then this branch will not be taken (which is simply debugging information to assert that
            // nondefdecl->get_type()->get_declaration() == nondefdecl is true (below).
               if (nondefdecl->get_file_info() == NULL)
                  {
                    printf ("ERROR: In SageBuilder::buildClassDeclaration_nfi(): nondefdecl = %p = %s does not have its source position information setup \n",nondefdecl,nondefdecl->class_name().c_str());
                    printf ("   --- nondefdecl                                    = %s \n",nondefdecl->get_name().str());
                    printf ("   --- nondefdecl->get_firstNondefiningDeclaration() = %p \n",nondefdecl->get_firstNondefiningDeclaration());
                    printf ("   --- nondefdecl->get_definingDeclaration()         = %p \n",nondefdecl->get_definingDeclaration());
                    printf ("   --- nondefdecl->get_type()                        = %p \n",nondefdecl->get_type());
                    printf ("   --- nondefdecl->get_type()->get_declaration()     = %p \n",nondefdecl->get_type()->get_declaration());
                    printf ("The real error is: (nondefdecl->get_type()->get_declaration() != nondefdecl) \n");
                  }
                 else
                  {
                    ROSE_ASSERT(nondefdecl->get_file_info() != NULL);
                    nondefdecl->get_file_info()->display("nondefdecl");
                  }
#endif
             }
          ROSE_ASSERT(nondefdecl->get_type()->get_declaration() == nondefdecl);

#if 0
          printf ("In buildClassDeclaration_nfi(): after set_type(): nondefdecl = %p = %s nondefdecl->get_type() = %p = %s \n",nondefdecl,nondefdecl->class_name().c_str(),nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
#endif

       // printf ("SageBuilder::buildClassDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

       // The nondefining declaration will not appear in the source code, but is compiler
       // generated (so we have something about the class that we can reference; e.g in
       // types).  At the moment we make it a transformation, there might be another kind
       // of source position that would be more precise.  FIXME.
       // setOneSourcePositionNull(nondefdecl);
          setOneSourcePositionForTransformation(nondefdecl);
          ROSE_ASSERT (nondefdecl->get_startOfConstruct() != NULL);

#if BUILDER_MAKE_REDUNDANT_CALLS_TO_DETECT_TRANSFORAMTIONS
       // DQ (5/2/2012): After EDG/ROSE translation, there should be no IR nodes marked as transformations.
          if (SourcePositionClassificationMode != e_sourcePositionTransformation)
             {
               detectTransformations(nondefdecl);
             }
#endif
       // DQ (6/6/2012): This has to be set before we generate the type.
       // nondefdecl->set_firstNondefiningDeclaration(nondefdecl);

       // DQ (3/15/2012): This is now set below.
       // nondefdecl->set_definingDeclaration(defdecl);
          nondefdecl->setForward();

       // DQ (2/27/2012): I don't like that this is setting the parent to be a scope (not a bad default, but must be reset later if required).
       // Liao, 9/2/2009. scope stack is optional, it can be empty
       // nondefdecl->set_parent(topScopeStack());
#if 0
          printf ("WARNING: In buildClassDeclaration_nfi(): Skipping the setting of the parents (for both defining and nondefining declaration) to be the same as the scope \n");
#endif
       // nondefdecl->set_parent(scope);
       // defdecl->set_parent(scope);

          if (scope != NULL)
             {
               mysymbol = new SgClassSymbol(nondefdecl);
#if 0
               printf ("In buildClassDeclaration_nfi(): Insert the new SgClassSymbol = %p from nondefdecl = %p = %s into the scope = %p = %s \n",mysymbol,nondefdecl,nondefdecl->class_name().c_str(),scope,scope->class_name().c_str());
#endif
            // scope->insert_symbol(name, mysymbol);
               scope->insert_symbol(nameWithTemplateArguments, mysymbol);

            // DQ (11/21/2013): Added test based on debugging session with Philippe.
               ROSE_ASSERT(nondefdecl->get_scope() == scope);
             }
            else
             {
            // Liao 9/2/2009: This is not an error. We support bottomup AST construction and scope can be unkown.
            // DQ (1/26/2009): I think this should be an error, but that appears it would
            // break the existing interface. Need to discuss this with Liao.
               printf ("Warning: no scope provided to support symbol table entry! \n");
             }

       // DQ (7/31/2019): Check that the parent is set if this was used a the declaration referenced by a symbol.
       // ROSE_ASSERT (nondefdecl->get_parent() != NULL);
        }

  // printf ("SageBuilder::buildClassDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

#if 1
  // Refactored this code.
     testTemplateArgumentParents(nondefdecl);
#else
     if (buildTemplateInstantiation == true)
        {
       // DQ (7/25/2012): Added this code here to reset the parents of the template arguments.
          for (size_t i = 0; i < templateArgumentsList->size(); i++)
             {
            // DQ (7/25/2012): This should be true because the template argument was set to the functions
            // scope so that the name with template arguments could be computed (with name qualification).
               ROSE_ASSERT((*templateArgumentsList)[i]->get_parent() != NULL);

#error "DEAD CODE!"

            // ROSE_ASSERT(isSgGlobal(templateArgumentsList[i]->get_parent()) == NULL);
            // ROSE_ASSERT(templateArgumentsList[i]->get_parent() == nondefining_templateInstantiation);

            // Be we want to reset it to be the function (now that it is available, because this is more precise).
            // All qualified names should compute to the same qualified name (if not then it is a bug in the name
            // qualification mechanism).
               (*templateArgumentsList)[i]->set_parent(nondefdecl);
             }
        }
#endif

  // DQ (3/15/2012): I hhava moved construction of defining declaration to be AFTER the nondefining declaration!
  // This is a better organization ans also should make sure that the declaration in the SgClassType will
  // properly reference the firstNondefiningDeclaration (instead of the defining declaration).

  // step 1 (now step 2). Build defining declaration
  // SgClassDefinition* classDef = buildClassDefinition();
     SgClassDefinition* classDef = buildClassDefinition(NULL,buildTemplateInstantiation);

  // DQ (11/26/2011): Debugging EDG 3.3 use of templateArguments.
#if 0
     printf ("Building a SgClassDeclaration: buildClassDeclaration_nfi() buildTemplateInstantiation = %s \n",buildTemplateInstantiation ? "true:" : "false");
#endif

  // SgClassDeclaration* defdecl = new SgClassDeclaration (name,kind,NULL,classDef);
     SgClassDeclaration* defdecl = NULL;
     if (buildTemplateInstantiation == true)
        {
       // This adds: SgTemplateDeclaration *templateDeclaration and SgTemplateArgumentPtrList templateArguments
          SgTemplateArgumentPtrList emptyList;
       // defdecl = new SgTemplateInstantiationDecl (name,kind,NULL,classDef,NULL,emptyList);
          defdecl = new SgTemplateInstantiationDecl (nameWithTemplateArguments,kind,NULL,classDef,NULL,emptyList);

       // DQ (2/27/2018): Added assertion now that we have implemented more consistant semantics
       // for template instantiations (types are not generated in the constructor calls).
          ROSE_ASSERT(defdecl->get_type() == NULL);
          ROSE_ASSERT(isSgTemplateInstantiationDecl(defdecl) != NULL);
#if 0
          printf ("In buildClassDeclaration_nfi(): defdecl->get_name() = %s defdecl->get_templateName() = %s \n",defdecl->get_name().str(),isSgTemplateInstantiationDecl(defdecl)->get_templateName().str());
#endif
       // DQ (1/1/2012): Added support for setting the template name (I think this should be fixed in the constructor).
       // It can't be fixed in the constructor since it has to be set after construction (or passed in explicitly).
          ROSE_ASSERT(isSgTemplateInstantiationDecl(defdecl)->get_templateName().is_null() == true);

#if 0
          printf ("Warning: In buildClassDeclaration_nfi(): calling set_templateName(name = %s) for defining declaration \n",name.str());
#if 0
       // isSgTemplateInstantiationDecl(defdecl)->set_templateName(name);
       // isSgTemplateInstantiationDecl(defdecl)->set_templateName("SETME_DEFINING_DECL<>");
          isSgTemplateInstantiationDecl(defdecl)->set_templateName(name);

#error "DEAD CODE!"

       // DQ (5/31/2012): Find locations where this is set and include template syntax.
          ROSE_ASSERT(name.getString().find('<') == string::npos);
#else
       // DQ (6/1/2012): Make sure that the templateName is set and they it does not include the template syntax.
          SgName templateName = generateTemplateNameFromTemplateNameWithTemplateArguments(name);
          printf ("In buildClassDeclaration_nfi(): templateName = %s \n",templateName.str());
          isSgTemplateInstantiationDecl(defdecl)->set_templateName(templateName);

#error "DEAD CODE!"

       // DQ (5/31/2012): Find locations where this is set and include template syntax.
       // ROSE_ASSERT(templateName.getString().find('<') == string::npos);
          ROSE_ASSERT(hasTemplateSyntax(templateName) == false);

       // DQ (6/1/2012): Not clear if this is always true (for all template instantations).
       // ROSE_ASSERT(name.getString().find('<') != string::npos);
          ROSE_ASSERT(hasTemplateSyntax(name) == true);
#endif
#else
#if 0
          printf ("In buildClassDeclaration_nfi(): nameWithoutTemplateArguments = %s nameWithTemplateArguments = %s \n",nameWithoutTemplateArguments.str(),nameWithTemplateArguments.str());
#endif
          isSgTemplateInstantiationDecl(defdecl)->set_templateName(nameWithoutTemplateArguments);

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
       // DQ (5/8/2013): This fails for test2013_159.C, and it appears that we have been overly restrictive here.
          if (hasTemplateSyntax(nameWithTemplateArguments) == false)
             {
               printf ("WARNING: In buildClassDeclaration_nfi(): nameWithTemplateArguments = %s is not using template syntax \n",nameWithTemplateArguments.str());
             }
#endif
       // ROSE_ASSERT(hasTemplateSyntax(nameWithTemplateArguments) == true);

       // DQ (7/27/2012): This fails for test2005_35.C where conversion operators are seen.
       // ROSE_ASSERT(hasTemplateSyntax(nameWithoutTemplateArguments) == false);
#endif

          ROSE_ASSERT(isSgTemplateInstantiationDecl(defdecl)->get_templateName().is_null() == false);

       // DQ (3/22/2012): Make sure there is template syntax present.
          if (isSgTemplateInstantiationDecl(defdecl)->get_templateName().getString().find('>') == string::npos)
             {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("WARNING: No template syntax present in name of template class instantiation (defdecl) \n");
#endif
             }
       // ROSE_ASSERT(isSgTemplateInstantiationDecl(defdecl)->get_templateName().getString().find('>') != string::npos);
#if 0
          printf ("Should we have set the template instantiation name at this point? \n");
          ROSE_ASSERT(false);
#endif
       // DQ (3/5/2012): Check that the SgClassDefinition is properly matching.
          ROSE_ASSERT(defdecl->get_definition() != NULL);
          ROSE_ASSERT(isSgTemplateInstantiationDefn(defdecl->get_definition()) != NULL);
        }
       else
        {
#if 0
          printf ("Building a SgClassDeclaration, but we might require a SgTemplateClassDeclaration \n");
#endif
       // defdecl = new SgClassDeclaration (name,kind,NULL,classDef);
       // defdecl = new SgClassDeclaration (nameWithoutTemplateArguments,kind,NULL,classDef);

       // DQ (10/11/2015): Try to build a matching SgTemplateClassDeclaration.  The problem with this fix is that
       // I would prefer that the other function be called instead. We might still want to implementat that instead.
          if (buildTemplateDeclaration == true)
             {
               printf ("In buildClassDeclaration_nfi(): I think we also want template specialization arguments to be more general: using nameWithoutTemplateArguments = %s \n",nameWithoutTemplateArguments.str());

            //         = new SgTemplateClassDeclaration(nameWithTemplateSpecializationArguments,kind,classType,(SgClassDefinition*)NULL);
               defdecl = new SgTemplateClassDeclaration (nameWithoutTemplateArguments,kind,NULL,classDef);

            // DQ (2/27/2018): We should be able to enforce this, it should have always been true.
               ROSE_ASSERT(defdecl->get_type() == NULL);
#if 0
               printf ("Exiting afte test! \n");
               ROSE_ASSERT(false);
#endif
             }
            else
             {
               defdecl = new SgClassDeclaration (nameWithoutTemplateArguments,kind,NULL,classDef);

#if 0
            // DQ (12/22/2019): Debugging the case of shared class declarations between multiple files referencing the same defining declaration.
               printf ("In SageBuilder::buildClassDeclaration_nfi(): build a SgClassDeclaration: defdecl = %p \n",defdecl);
#endif

            // DQ (2/27/2018): We should be able to enforce this, it should have always been true.
               ROSE_ASSERT(defdecl->get_type() == NULL);
             }

       // DQ (3/5/2012): Check that the SgClassDefinition is properly matching.
          ROSE_ASSERT(defdecl->get_definition() != NULL);
          ROSE_ASSERT(isSgTemplateInstantiationDefn(defdecl->get_definition()) == NULL);
        }
     ROSE_ASSERT(defdecl != NULL);

#if 0
     printf ("In buildClassDeclaration_nfi(): nondefdecl = %p = %s \n",nondefdecl,nondefdecl->class_name().c_str());
#endif

  // DQ (3/15/2012): Moved from original location above...
     nondefdecl->set_definingDeclaration(defdecl);

     ROSE_ASSERT(nondefdecl->get_definingDeclaration() == defdecl);
     ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != defdecl);

  // printf ("SageBuilder::buildClassDeclaration_nfi(): defdecl = %p \n",defdecl);

     setOneSourcePositionForTransformation(defdecl);
  // constructor is side-effect free
     classDef->set_declaration(defdecl);
     defdecl->set_definingDeclaration(defdecl);

     testTemplateArgumentParents(nondefdecl);
     testTemplateArgumentParents(defdecl);

  // setOneSourcePositionForTransformation(nondefdecl);
  //
  // Liao 1/18/2011, I changed the semantics of setOneSourcePositionNull to set file_info to null regardless the existence of
  // file_info of the input node.
  // We do want to keep the file_info of nodefdecl if it is set already as compiler generated.
  //    setOneSourcePositionNull(nondefdecl);

  // nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
  // nondefdecl->set_definingDeclaration(defdecl);
     defdecl->set_firstNondefiningDeclaration(nondefdecl);

     if (buildTemplateInstantiation == true)
        {
       // DQ (9/16/2012): This is a newly refactored function (call this after the firstNondefiningDeclaration is set).
          setTemplateArgumentsInDeclaration(defdecl,templateArgumentsList);
        }

  // DQ (3/22/2012): I think we can assert this.
     ROSE_ASSERT(defdecl->get_type() == NULL);

  // Liao, 10/30/2009
  // The SgClassDeclaration constructor will automatically generate a SgClassType internally if NULL is passed for SgClassType
  // This is not desired when building a defining declaration and an inefficience in the constructor
  // Ideally, only the first nondefining class declaration should have a dedicated SgClassType and
  // the defining class declaration (and other nondefining declaration) just shared that SgClassType.
     if (defdecl->get_type() != NULL)
        {
       // if a defining class declaration's type is associated with a defining class.
       // This is a wrong SgClassType and has to be reset
          if (defdecl->get_type()->get_declaration() == isSgDeclarationStatement(defdecl))
             {
            // DQ (3/21/2012): Added this test.
               ROSE_ASSERT (nondefdecl->get_type() != NULL);

            // DQ (3/15/2012): Make this conditional upon the types not already being equal.
               if (nondefdecl->get_type() != defdecl->get_type())
                  {
#if 0
                    printf ("Deleting defdecl->get_type() = %p = %s (using type from nondefdecl = %p) \n",defdecl->get_type(),defdecl->get_type()->class_name().c_str(),nondefdecl);
                    printf ("Skipping delete of %p to maintain unique type pointers \n",defdecl->get_type());
#else
                    delete defdecl->get_type();
#endif
                 // DQ (3/15/2012): This will be reset below.
                    defdecl->set_type(NULL);
#if 0
                    printf ("In SageBuilder::buildClassDeclaration(): built class type: part 5: defdecl->get_type() = %p = %s \n",defdecl->get_type(),defdecl->get_type()->class_name().c_str());
#endif
#if 0
                 // DQ (12/13/2011): Is this executed...
                    printf ("Is this executed! \n");
                    ROSE_ASSERT(false);
#endif
                 // DQ (3/21/2012): set the types to be the same type.
                    ROSE_ASSERT (nondefdecl->get_type() != NULL);
                    defdecl->set_type(nondefdecl->get_type());
#if 0
                    printf ("In SageBuilder::buildClassDeclaration(): built class type: part 6: nondefdecl->get_type() = %p = %s \n",defdecl->get_type(),defdecl->get_type()->class_name().c_str());
#endif
                 // DQ (3/21/2012): Added these checks...
                    SgClassType* classType = nondefdecl->get_type();
                    ROSE_ASSERT(classType != NULL);
                    SgClassDeclaration* local_classDeclaration = isSgClassDeclaration(classType->get_declaration());
                    ROSE_ASSERT (local_classDeclaration != NULL);
                    printf ("In buildClassDeclaration_nfi(): classType = %p local_classDeclaration = %p \n",classType,local_classDeclaration);
                    ROSE_ASSERT (local_classDeclaration->get_firstNondefiningDeclaration() != NULL);
                    ROSE_ASSERT (local_classDeclaration->get_firstNondefiningDeclaration() == local_classDeclaration);
                  }
             }
        }
       else
        {
       // DQ (3/15/2012): Make sure that both the defining and non-defining declarations use the same type.
          ROSE_ASSERT (nondefdecl->get_type() != NULL);
          defdecl->set_type(nondefdecl->get_type());
#if 0
          printf ("In buildClassDeclaration_nfi(): defdecl = %p = %s defdecl->get_type() = %p = %s \n",defdecl,defdecl->class_name().c_str(),defdecl->get_type(),defdecl->get_type()->class_name().c_str());
#endif

          ROSE_ASSERT (nondefdecl->get_firstNondefiningDeclaration() != NULL);
#if 0
       // DQ (11/20/2017): Commented out output spew.
       // DQ (2/28/2015): This test is failing in the new application support for templates within the testRoseHeaders_01.C.
          if (nondefdecl->get_type()->get_declaration() != isSgDeclarationStatement(nondefdecl->get_firstNondefiningDeclaration()))
             {
               printf ("WARNING: In buildClassDeclaration_nfi(): inner test: commented out test for equality between the declaration asociated with the type and that associated with the firstNondefiningDeclaration \n");
               printf ("   --- nondefdecl->get_type()                        = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
               printf ("   --- nondefdecl->get_type()->get_declaration()     = %p \n",nondefdecl->get_type()->get_declaration());
               printf ("   --- nondefdecl->get_firstNondefiningDeclaration() = %p = %s \n",nondefdecl->get_firstNondefiningDeclaration(),nondefdecl->get_firstNondefiningDeclaration()->class_name().c_str());
             }
       // DQ (7/22/2017): Uncomment this test to better understand why this is a new issue (after two years).
       // ROSE_ASSERT (nondefdecl->get_type()->get_declaration() == isSgDeclarationStatement(nondefdecl->get_firstNondefiningDeclaration()));
#endif
#if 0
          ROSE_ASSERT (nondefdecl->get_type()->get_declaration() == isSgDeclarationStatement(nondefdecl->get_firstNondefiningDeclaration()));
#endif
        }

  // DQ (9/4/2012): Added assertion.
     ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());

  // patch up the SgClassType for the defining class declaration
     ROSE_ASSERT (nondefdecl->get_type() != NULL);
  // ROSE_ASSERT (nondefdecl->get_type()->get_declaration() == isSgDeclarationStatement(nondefdecl));
#if 0
     if (nondefdecl->get_type()->get_declaration() != isSgDeclarationStatement(nondefdecl))
        {
          printf ("nondefdecl                                    = %p = %s \n",nondefdecl,nondefdecl->class_name().c_str());
          printf ("nondefdecl->get_type()                        = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
          printf ("nondefdecl->get_type()->get_declaration()     = %p = %s \n",nondefdecl->get_type()->get_declaration(),nondefdecl->get_type()->get_declaration()->class_name().c_str());
          printf ("nondefdecl->get_firstNondefiningDeclaration() = %p = %s \n",nondefdecl->get_firstNondefiningDeclaration(),nondefdecl->get_firstNondefiningDeclaration()->class_name().c_str());
        }
#endif
  // ROSE_ASSERT (defdecl->get_type()->get_declaration() == isSgDeclarationStatement(defdecl));
     ROSE_ASSERT (defdecl->get_type() != NULL);
     ROSE_ASSERT (defdecl->get_type()->get_declaration() != NULL);
     ROSE_ASSERT (defdecl->get_type()->get_declaration() != isSgDeclarationStatement(defdecl));
     ROSE_ASSERT (nondefdecl->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT (nondefdecl->get_firstNondefiningDeclaration() == nondefdecl);
#if 0
     ROSE_ASSERT (nondefdecl->get_type()->get_declaration() == isSgDeclarationStatement(nondefdecl->get_firstNondefiningDeclaration()));
#else

#if 0
  // DQ (11/20/2017): Commented out output spew.
  // DQ (2/28/2015): This test is failing in the new application support for templates within the testRoseHeaders_01.C.
     if (nondefdecl->get_type()->get_declaration() != isSgDeclarationStatement(nondefdecl->get_firstNondefiningDeclaration()))
        {
          printf ("WARNING: In buildClassDeclaration_nfi(): outer test (test 1): commented out test for equality between the declaration asociated with the type and that associated with the firstNondefiningDeclaration \n");
          printf ("   --- nondefdecl->get_type()                        = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
          printf ("   --- nondefdecl->get_type()->get_declaration()     = %p \n",nondefdecl->get_type()->get_declaration());
          printf ("   --- nondefdecl->get_firstNondefiningDeclaration() = %p = %s \n",nondefdecl->get_firstNondefiningDeclaration(),nondefdecl->get_firstNondefiningDeclaration()->class_name().c_str());
        }
  // ROSE_ASSERT (nondefdecl->get_type()->get_declaration() == isSgDeclarationStatement(nondefdecl->get_firstNondefiningDeclaration()));
     if (nondefdecl->get_type()->get_declaration() != isSgDeclarationStatement(nondefdecl))
        {
          printf ("WARNING: In buildClassDeclaration_nfi(): outer test (test 2): commented out test for equality between the declaration asociated with the type and that associated with the firstNondefiningDeclaration \n");
          printf ("   --- nondefdecl->get_type()                        = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
          printf ("   --- nondefdecl->get_type()->get_declaration()     = %p \n",nondefdecl->get_type()->get_declaration());
          printf ("   --- nondefdecl->get_firstNondefiningDeclaration() = %p = %s \n",nondefdecl,nondefdecl->class_name().c_str());
        }
  // ROSE_ASSERT (nondefdecl->get_type()->get_declaration() == isSgDeclarationStatement(nondefdecl));
#endif

#endif

  // DQ (9/4/2012): Added assertion.
     ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());

  // This appears to be redundant...is it?
     defdecl->set_type(nondefdecl->get_type());

#if 0
     printf ("In buildClassDeclaration_nfi(): after calling set_type() again: defdecl = %p = %s defdecl->get_type() = %p = %s \n",
          defdecl,defdecl->class_name().c_str(),defdecl->get_type(),defdecl->get_type()->class_name().c_str());
#endif

  // DQ (9/4/2012): Added assertion.
     ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());

  // I don't think this is always a forward declaration (e.g. if it is not used in a prototype).
  // Checking the olded EDG/ROSE interface it appears that it is always marked forward (unless
  // used in a defining declaration).
     nondefdecl->setForward();

     if (scope != NULL)  // put into fixStructDeclaration() or alike later on
        {
       // DQ (9/4/2012): Added assertion.
          ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());

       // Note, this function sets the parent to be the scope if it is not already set.
          fixStructDeclaration(defdecl,scope);

       // DQ (9/4/2012): Added assertion.
          ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());

          fixStructDeclaration(nondefdecl,scope);

       // DQ (9/4/2012): Added assertion.
          ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());

#if 0
          SgClassSymbol* mysymbol = new SgClassSymbol(nondefdecl);
          ROSE_ASSERT(mysymbol);
          scope->insert_symbol(name, mysymbol);
          printf ("@@@@@@@@@@@@@@ In buildClassDeclaration_nfi(): setting scope of defining and non-defining declaration to scope = %s \n",scope->class_name().c_str());
          defdecl->set_scope(scope);
          nondefdecl->set_scope(scope);

       // defdecl->set_parent(scope);

       // Liao, 9/2/2009. merged into fixStructDeclaration
       // DQ (1/25/2009): The scope is not the same as the parent, since the scope is logical, and the parent is structural (note that topScopeStack() is structural).
          nondefdecl->set_parent(scope);
       // nondefdecl->set_parent(topScopeStack());
       // Liao, 9/2/2009. scope stack is optional, it can be empty
          defdecl->set_parent(scope);
       // defdecl->set_parent(topScopeStack());
#endif
        }

  // DQ (1/26/2009): I think we should assert this, but it breaks the interface as defined
  // by the test code in tests/nonsmoke/functional/roseTests/astInterfaceTests.
  // ROSE_ASSERT(defdecl->get_parent() != NULL);

  // ROSE_ASSERT(nonDefiningDecl->get_parent() != NULL);

  // DQ (2/27/2012): Tracking down where parents are not set correctly (class declaration in typedef is incorrectly set to SgGlobal).
     ROSE_ASSERT(defdecl->get_parent()    == NULL);

  // DQ (2/29/2012):  We can't assert this (fails for test2012_09.C).
  // ROSE_ASSERT(nondefdecl->get_parent() == NULL);

     ROSE_ASSERT(defdecl->get_definingDeclaration() == defdecl);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != defdecl->get_definingDeclaration());

     testTemplateArgumentParents(nondefdecl);
     testTemplateArgumentParents(defdecl);

  // DQ (3/8/2018): Added for debugging.
     SgClassDeclaration* temp_firstNondefiningDeclaration = isSgClassDeclaration(defdecl->get_firstNondefiningDeclaration());
     SgClassDeclaration* temp_definingDeclaration         = isSgClassDeclaration(defdecl->get_definingDeclaration());
     ROSE_ASSERT(temp_firstNondefiningDeclaration != NULL);
     ROSE_ASSERT(temp_definingDeclaration != NULL);

#if 0
     printf ("Leaving buildClassDeclaration_nfi(): defdecl = %p = %s = %s \n",defdecl,defdecl->class_name().c_str(),defdecl->get_name().str());
     printf ("   --- defdecl->get_firstNondefiningDeclaration() = %p \n",defdecl->get_firstNondefiningDeclaration());
     printf ("   --- defdecl->get_definingDeclaration()         = %p \n",defdecl->get_definingDeclaration());

     printf ("   --- defdecl->get_firstNondefiningDeclaration()->get_name() = %s \n",temp_firstNondefiningDeclaration->get_name().str());
     printf ("   --- defdecl->get_definingDeclaration()->get_name()         = %s \n",temp_definingDeclaration->get_name().str());

     printf ("   --- defdecl->get_firstNondefiningDeclaration()->get_type() = %p = %s \n",
          temp_firstNondefiningDeclaration->get_type(),temp_firstNondefiningDeclaration->get_type()->unparseToString().c_str());
     printf ("   --- defdecl->get_definingDeclaration()->get_type()         = %p = %s \n",
          temp_definingDeclaration->get_type(),temp_definingDeclaration->get_type()->unparseToString().c_str());

     printf ("   --- nameWithoutTemplateArguments               = %s \n",nameWithoutTemplateArguments.str());
     printf ("   --- nameWithTemplateArguments                  = %s \n",nameWithTemplateArguments.str());

#if 0
     printf ("Leaving buildClassDeclaration_nfi(): defdecl: unparseNameToString() = %s \n",defdecl->unparseNameToString().c_str());
#endif
#endif

  // DQ (3/8/2018): Added assertion.
     ROSE_ASSERT(temp_firstNondefiningDeclaration->get_name() == temp_definingDeclaration->get_name());
     ROSE_ASSERT(temp_firstNondefiningDeclaration->get_type() == temp_definingDeclaration->get_type());

  // DQ (3/7/2015): Only in EDG 4.7 does the defining declaration not have a valid templateDeclaration pointer (sometimes).
     SgTemplateInstantiationDecl* nondefiningDeclaration = isSgTemplateInstantiationDecl(defdecl->get_firstNondefiningDeclaration());
     SgTemplateInstantiationDecl* definingDeclaration    = isSgTemplateInstantiationDecl(defdecl->get_definingDeclaration());
     if (definingDeclaration != NULL && nondefiningDeclaration != NULL)
        {
          SgTemplateClassDeclaration* templateDeclaration = nondefiningDeclaration->get_templateDeclaration();
          if (templateDeclaration != NULL && definingDeclaration->get_templateDeclaration() == NULL)
             {
#if 0
               printf ("NOTE: buildClassDeclaration_nfi(): Setting the templateDeclaration for the defining declaration = %p using the value = %p from the nondefiningDeclaration = %p \n",
                    definingDeclaration,templateDeclaration,nondefiningDeclaration);
#endif
               definingDeclaration->set_templateDeclaration(templateDeclaration);

               ROSE_ASSERT(definingDeclaration->get_templateDeclaration() != NULL);
             }
       // ROSE_ASSERT(definingDeclaration->get_templateDeclaration() != NULL);
        }

  // DQ (3/7/2015): Only in EDG 4.7 does the defining declaration not have a valid templateDeclaration pointer (sometimes).
     if (definingDeclaration != NULL)
        {
          if (definingDeclaration->get_templateDeclaration() == NULL)
             {
#if 0
               printf ("NOTE: buildClassDeclaration_nfi(): definingDeclaration->get_templateDeclaration() == NULL \n");
#endif
             }
       // ROSE_ASSERT(definingDeclaration->get_templateDeclaration() != NULL);
        }

#if 0
     printf ("Leaving buildClassDeclaration_nfi(): defdecl = %p defdecl->unparseNameToString() = %s \n",defdecl,defdecl->unparseNameToString().c_str());
#endif

#if 0
  // DQ (1/27/2019): Test that symbol table to debug Cxx11_tests/test2019)33.C.
     printf ("Leaving buildClassDeclaration_nfi(): Calling find_symbol_from_declaration() \n");
     SgSymbol* test_symbol = nondefdecl->get_scope()->find_symbol_from_declaration(nondefdecl);

  // DQ (1/27/2019): Test that symbol table to debug Cxx11_tests/test2019)33.C.
     printf ("Leaving buildClassDeclaration_nfi(): Calling get_symbol_from_symbol_table() \n");
     ROSE_ASSERT(nondefdecl->get_symbol_from_symbol_table() != NULL);
#endif

     return defdecl;
   }


SgClassDeclaration* SageBuilder::buildStructDeclaration(const string& name, SgScopeStatement* scope/*=NULL*/)
   {
     SgName myname(name);
     return buildStructDeclaration(myname, scope);
   }

SgClassDeclaration* SageBuilder::buildStructDeclaration(const char* name, SgScopeStatement* scope/*=NULL*/)
   {
     SgName myname(name);
     return buildStructDeclaration(myname, scope);
   }


#if 0
// DQ (11/19/2011): Added more uniform support for building class declarations.
SgTemplateClassDeclaration*
SageBuilder::buildTemplateClassDeclaration ( SgName name, SgScopeStatement* scope )
   {
     ROSE_ASSERT(scope != NULL);
     SgTemplateClassDeclaration* definingClassDeclaration = buildDefiningTemplateClassDeclaration(name,scope);
     ROSE_ASSERT(definingClassDeclaration != NULL);

     return definingClassDeclaration;
   }
#endif


SgTemplateClassDefinition*
SageBuilder::buildTemplateClassDefinition(SgTemplateClassDeclaration *d /*= NULL*/ )
  {
    SgTemplateClassDefinition* result = NULL;
    if (d != NULL) // the constructor does not check for NULL d, causing segmentation fault
       {
         result = new SgTemplateClassDefinition(d);
      // result->set_parent(d); // set_declaration() == set_parent() in this case
       }
      else
       {
         result = new SgTemplateClassDefinition();
       }

    ROSE_ASSERT(result);

 // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
    if (symbol_table_case_insensitive_semantics == true)
         result->setCaseInsensitive(true);

    setOneSourcePositionForTransformation(result);
    return result;
  }


// SgTemplateClassDeclaration * SageBuilder::buildTemplateClassDeclaration_nfi(SgName & name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgTemplateClassDeclaration* nonDefiningDecl )
// SgTemplateClassDeclaration * SageBuilder::buildNondefiningTemplateClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope )
SgTemplateClassDeclaration*
SageBuilder::buildNondefiningTemplateClassDeclaration_nfi(const SgName& XXX_name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgTemplateParameterPtrList* templateParameterList, SgTemplateArgumentPtrList* templateSpecializationArgumentList )
   {
#if 0
  printf("In SageBuilder::buildNondefiningTemplateClassDeclaration_nfi(XXX_name = %p):\n", XXX_name.str());
#endif

     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

  // DQ (11/20/2011): This is for initial debugging only.
     ROSE_ASSERT(scope != NULL);

#if 0
     printf ("SageBuilder::buildNondefiningTemplateClassDeclaration_nfi(): XXX_name = %s scope = %p = %s \n",XXX_name.str(),scope,scope->class_name().c_str());
#endif

  // DQ (9/12/2012): We want to add the template arguments of any specialization to the template name and keep track of the name with and without template specialization arguments.
     SgName nameWithoutTemplateArguments            = XXX_name;
     SgName nameWithTemplateSpecializationArguments = appendTemplateArgumentsToName(nameWithoutTemplateArguments,*templateSpecializationArgumentList);

#if 0
     printf ("SageBuilder::buildNondefiningTemplateClassDeclaration_nfi(): nameWithTemplateSpecializationArguments = %s \n",nameWithTemplateSpecializationArguments.str());
#endif

  // SgTemplateClassDeclaration::class_types template_class_kind = SgTemplateClassDeclaration::e_class;

  // Step 2. build the nondefining declaration,
  // but only if the input nonDefiningDecl pointer was NULL and it does not exist

  // Get the nondefining declaration from the symbol if it has been built (if this works,
  // then we likely don't need the "SgClassDeclaration* nonDefiningDecl" parameter).
     SgTemplateClassDeclaration* nondefdecl = NULL;

  // DQ (1/26/2009): It seems that (scope == NULL) can happen in the tests/nonsmoke/functional/roseTests/astInterfaceTests test codes.
  // ROSE_ASSERT(scope != NULL);

  // DQ (12/21/2011): We want to use a newer design that derives the SgTemplateClassDeclaration from the SgClassDeclaration.
  // SgTemplateSymbol* mysymbol = NULL;
     SgClassSymbol* mysymbol = NULL;

     if (scope != NULL)
        {
       // DQ (9/12/2012): We want to include the template specialization into the name where it is required (this handling
       // is similar to normal template arguments for non-template declaration, but different than template parameters).
       // DQ (12/21/2011): We want to use a newer design that derives the SgTemplateClassDeclaration from the SgClassDeclaration.
       // mysymbol = scope->lookup_template_symbol(name);
       // mysymbol = scope->lookup_class_symbol(name);
       // mysymbol = scope->lookup_template_class_symbol(name);
       // mysymbol = scope->lookup_template_class_symbol(name,templateParameterList,templateSpecializationArgumentList);
          mysymbol = scope->lookup_template_class_symbol(nameWithTemplateSpecializationArguments,templateParameterList,templateSpecializationArgumentList);
        }
       else
        {
       // Liao 9/2/2009: This is not an error. We support bottom-up AST construction and scope can be unknown.
       // DQ (1/26/2009): I think this should be an error, but that appears it would
       // break the existing interface. Need to discuss this with Liao.
          printf ("Warning: In SageBuilder::buildNondefiningTemplateClassDeclaration_nfi(): scope == NULL \n");
        }
#if 0
     printf ("In SageBuilder::buildNondefiningTemplateClassDeclaration_nfi(): mysymbol = %p \n",mysymbol);
#endif

     if (mysymbol != NULL) // set links if nondefining declaration already exists.
        {
       // DQ (3/7/2012): Build a seperate non-defining declaration (reusing the existing one will cause the test for unique statements to fail).
       // printf ("WARNING: Even if the first non-defining SgTemplateClassDeclaration is found in the symbol table then likely we still might want to build a 2nd one. \n");
       // nondefdecl = isSgTemplateClassDeclaration(mysymbol->get_declaration());
          SgClassType* classType = isSgClassType(mysymbol->get_type());
          ROSE_ASSERT(classType != NULL);

       // DQ (9/12/2012): We want to include the template specialization into the name where it is required.
       // nondefdecl = new SgTemplateClassDeclaration(name,kind,classType,(SgClassDefinition*)NULL);
          nondefdecl = new SgTemplateClassDeclaration(nameWithTemplateSpecializationArguments,kind,classType,(SgClassDefinition*)NULL);

#if 0
       // DQ (3/4/2018): relax this requirement for SgTemplateInstantiationClassDeclaration.
       // DQ (2/27/2018): Enforce that this is not already set (should be set after the constructor to
       // simplify how derived classes (e.g. SgTemplateInstantiationClassDeclaration statements) work.
          if (nondefdecl->get_type() != NULL)
             {
               printf ("Note: SageBuilder::buildNondefiningTemplateClassDeclaration_nfi(): nondefdecl->get_type() != NULL: name = %s \n",nondefdecl->get_name().str());
             }
       // ROSE_ASSERT(nondefdecl->get_type() == NULL);
#endif

          ROSE_ASSERT(nondefdecl != NULL);

       // DQ (9/10/2012): Initialize the template parameter list.
          ROSE_ASSERT(templateParameterList != NULL);
          nondefdecl->get_templateParameters() = *templateParameterList;

       // DQ (9/16/2012): Moved this initialization of firstNondefiningDeclaration from farther down in this branch (and added assertion).
          nondefdecl->set_firstNondefiningDeclaration(mysymbol->get_declaration());
          ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != NULL);

      // TV (04/12/2018): Add a scope for nonreal classes (and their member) on the first non-defining declaration of template classes
          if (nondefdecl == nondefdecl->get_firstNondefiningDeclaration()) {
            SgDeclarationScope * nonreal_decl_scope = new SgDeclarationScope();

            nonreal_decl_scope->set_parent(nondefdecl);
            nondefdecl->set_nonreal_decl_scope(nonreal_decl_scope);

            SageInterface::setSourcePosition(nonreal_decl_scope);
            nonreal_decl_scope->get_startOfConstruct()->setCompilerGenerated();
            nonreal_decl_scope->get_endOfConstruct()->setCompilerGenerated();
#if 0
            printf("In SageBuilder::buildNondefiningTemplateClassDeclaration_nfi(XXX_name = %p): nrscope = %p (new)\n", XXX_name.str(), nonreal_decl_scope);
#endif
          }

#if 1
       // DQ (9/16/2012): This newly refactored function can only be called after firstNondefiningDeclaration is set.
       // This also sets the template argument parents to the firstNondefiningDeclaration.
          setTemplateSpecializationArgumentsInDeclaration(nondefdecl,templateSpecializationArgumentList);
#else
       // DQ (9/12/2012): Adding support for template specialization.
          ROSE_ASSERT(templateSpecializationArgumentList != NULL);
          nondefdecl->get_templateSpecializationArguments() = *templateSpecializationArgumentList;
#endif
       // DQ (9/10/2012): Test the just built template with its template parameters.
          if (nondefdecl->get_templateParameters().size() == 0)
             {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("WARNING: In buildNondefiningTemplateClassDeclaration_nfi(): (part 1) nondefdecl->get_templateParameters().size() == 0: OK for nested classes/structs in template declarations nondefdecl = %p \n",nondefdecl);
#endif
             }
       // ROSE_ASSERT(nondefdecl->get_templateParameters().size() > 0);

       // DQ (3/7/2012): We want this to be set later, so we can't test it here.
       // ROSE_ASSERT(nondefdecl->get_parent() != NULL);
#if 0
          nondefdecl->set_definingDeclaration(defdecl);

          ROSE_ASSERT(nondefdecl->get_definingDeclaration() == defdecl);
          ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != defdecl);
#endif
       // DQ (3/7/2012): But always refer to the first non-defining declaration so it will be unique (and set the scope).
          ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != NULL);
          ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != nondefdecl);
          nondefdecl->set_scope(scope);
          nondefdecl->setForward();

       // DQ (9/12/2012): Set the template name to be the name without template specialization arguments.
          nondefdecl->set_templateName(nameWithoutTemplateArguments);

          testTemplateArgumentParents(nondefdecl);
        }
       else // build a nondefnining declaration if it does not exist
        {
          nondefdecl = new SgTemplateClassDeclaration(nameWithTemplateSpecializationArguments,kind,(SgClassType*)NULL,(SgClassDefinition*)NULL);
          ROSE_ASSERT(nondefdecl != NULL);

       // DQ (9/10/2012): Initialize the template parameter list.
          ROSE_ASSERT(templateParameterList != NULL);
          nondefdecl->get_templateParameters() = *templateParameterList;

       // DQ (9/16/2012): Moved this initialization of firstNondefiningDeclaration from farther down in this branch.
          nondefdecl->set_firstNondefiningDeclaration(nondefdecl);

       // DQ (9/16/2012): This newly refactored function can only be called after firstNondefiningDeclaration is set.
       // This also sets the template argument parents to the firstNondefiningDeclaration.
          setTemplateSpecializationArgumentsInDeclaration(nondefdecl,templateSpecializationArgumentList);

       // DQ (9/10/2012): Test the just built template with its template parameters.
          if (nondefdecl->get_templateParameters().size() == 0)
             {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("WARNING: In buildNondefiningTemplateClassDeclaration_nfi(): (part 2) nondefdecl->get_templateParameters().size() == 0: OK for nested classes/structs in template declarations nondefdecl = %p \n",nondefdecl);
#endif
             }
       // ROSE_ASSERT(nondefdecl->get_templateParameters().size() > 0);

       // DQ (9/12/2012): Set the template name to be the name without template specialization arguments.
          nondefdecl->set_templateName(nameWithoutTemplateArguments);

#if 0
       // DQ (12/4/2011): Now we want to enable this so that the SgClassType will be available from a SgTemplateClassDeclaration.
          if (nondefdecl->get_type() == NULL)
             {
            // nondefdecl->set_type(SgClassType::createType(nondefdecl));
            // nondefdecl->set_type(NULL);
            // nondefdecl->set_type(SgTemplateType::createType(nondefdecl));
            // nondefdecl->set_type(SgTemplateType::createType());
               nondefdecl->set_type(SgClassType::createType(nondefdecl));
               ROSE_ASSERT(nondefdecl->get_type() != NULL);
             }
#endif
       // printf ("SageBuilder::buildClassDeclaration_nfi(): nondefdecl = %p \n",nondefdecl);

       // The nondefining declaration will not appear in the source code, but is compiler
       // generated (so we have something about the class that we can reference; e.g in
       // types).  At the moment we make it a transformation, there might be another kind
       // of source position that would be more precise.  FIXME.
       // setOneSourcePositionNull(nondefdecl);
          setOneSourcePositionForTransformation(nondefdecl);
          ROSE_ASSERT (nondefdecl->get_startOfConstruct() != NULL);

       // nondefdecl->set_definingDeclaration(defdecl);
          nondefdecl->setForward();

       // Liao, 9/2/2009. scope stack is optional, it can be empty
       // nondefdecl->set_parent(topScopeStack());
#if 0
          printf ("In buildNondefiningTemplateClassDeclaration_nfi(): Commented out setting the parent to the scope. \n");
#endif
       // printf ("Note that for C++, the parent may not be the same as the scope (dangerous code). \n");
       // nondefdecl->set_parent(scope);

          nondefdecl->set_scope(scope);

#if 1
       // DQ (12/4/2011): Set the scope first and then set the type (scope is required to compute the type (name mangling)).
       // DQ (12/4/2011): Now we want to enable this so that the SgClassType will be available from a SgTemplateClassDeclaration.
          if (nondefdecl->get_type() == NULL)
             {
            // nondefdecl->set_type(SgClassType::createType(nondefdecl));
            // nondefdecl->set_type(NULL);
            // nondefdecl->set_type(SgTemplateType::createType(nondefdecl));
            // nondefdecl->set_type(SgTemplateType::createType());
               nondefdecl->set_type(SgClassType::createType(nondefdecl));
               ROSE_ASSERT(nondefdecl->get_type() != NULL);
#if 0
               printf ("In SageBuilder::buildNondefiningTemplateClassDeclaration_nfi(): built class type: part 1: nondefdecl->get_type() = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
#endif
             }
#endif

       // Build a SgTemplateClassSymbol and put it into the specified scope.
          if (scope != NULL)
             {
#if 0
               printf ("Building a SgTemplateSymbol using nameWithTemplateSpecializationArguments = %s and nondefdecl = %p = %s \n",nameWithTemplateSpecializationArguments.str(),nondefdecl,nondefdecl->class_name().c_str());
#endif
            // DQ (12/21/2011): We want to use a newer design that derives the SgTemplateClassDeclaration from the SgClassDeclaration.
            // mysymbol = new SgTemplateSymbol(nondefdecl);
            // mysymbol = new SgClassSymbol(nondefdecl);
               mysymbol = new SgTemplateClassSymbol(nondefdecl);
               ROSE_ASSERT(mysymbol != NULL);

            // DQ (9/12/2012): We want to include the template specialization into the name where it is required.
            // DQ (3/6/2012): Added test for existing symbol (see test2012_18.C).
            // ROSE_ASSERT(scope->lookup_template_class_symbol(name) == NULL);
            // ROSE_ASSERT(scope->lookup_template_class_symbol(name,templateParameterList,templateSpecializationArgumentList) == NULL);
               ROSE_ASSERT(scope->lookup_template_class_symbol(nameWithTemplateSpecializationArguments,templateParameterList,templateSpecializationArgumentList) == NULL);

            // DQ (9/12/2012): We want to include the template specialization into the name where it is required.
            // scope->insert_symbol(name, mysymbol);
               scope->insert_symbol(nameWithTemplateSpecializationArguments, mysymbol);
#if 0
               printf ("SageBuilder::buildNondefiningTemplateClassDeclaration_nfi() (after building symbol): scope = %p = %s \n",scope,scope->class_name().c_str());
#endif
               ROSE_ASSERT(nondefdecl->get_scope() != NULL);

               ROSE_ASSERT(nondefdecl->get_symbol_from_symbol_table() != NULL);

            // DQ (9/12/2012): We want to include the template specialization into the name where it is required.
            // ROSE_ASSERT(scope->lookup_template_class_symbol(name) != NULL);
            // ROSE_ASSERT(scope->lookup_template_class_symbol(name,templateParameterList,templateSpecializationArgumentList) != NULL);
               ROSE_ASSERT(scope->lookup_template_class_symbol(nameWithTemplateSpecializationArguments,templateParameterList,templateSpecializationArgumentList) != NULL);
             }
            else
             {
            // Liao 9/2/2009: This is not an error. We support bottomup AST construction and scope can be unknown.
             }

          testTemplateArgumentParents(nondefdecl);

#if 1
       // DQ (7/16/2017): Added code to set the template parameters in the just build declaration (if it is a template declaration).
       // We want to set the parents of the template paremters to the frst nondefining template class declaration, and we want to reset
       // the scope of the declarations associated with any previously marked SgClassType objects associated with any template parameters.
       // printf ("SageBuilder::buildNondefiningTemplateClassDeclaration_nfi(): Calling setTemplateParametersInDeclaration(): nameWithTemplateSpecializationArguments = %s \n",nameWithTemplateSpecializationArguments.str());

          setTemplateParametersInDeclaration(nondefdecl,templateParameterList);

       // DQ (8/13/2013): Adding test of template parameter lists.
          SgTemplateClassDeclaration* templateClassDeclaration = isSgTemplateClassDeclaration(nondefdecl);
          ROSE_ASSERT(templateClassDeclaration == NULL || (templateParameterList != NULL && templateParameterList->size() == templateClassDeclaration->get_templateParameters().size()));
#endif
        }

  // defdecl->set_firstNondefiningDeclaration(nondefdecl);

  // DQ (11/3/2012): Setup the default source position information.
     setSourcePosition(nondefdecl);

#if 0
  // DQ (11/20/2011): SgTemplateClassDeclaration IR nodes don't have a SgType associated with them (template declarations don't have a type in C++, I think).

  // Liao, 10/30/2009
  // The SgClassDeclaration constructor will automatically generate a SgClassType internally if NULL is passed for SgClassType
  // This is not desired when building a defining declaration and an inefficience in the constructor
  // Ideally, only the first nondefining class declaration should have a dedicated SgClassType and
  // the defining class declaration (and other nondefining declaration) just shared that SgClassType.
     if (defdecl->get_type() != NULL)
        {
       // if a defining class declaration's type is associated with a defining class.
       // This is a wrong SgClassType and has to be reset
          if (defdecl->get_type()->get_declaration() == isSgDeclarationStatement(defdecl) )
             {
               delete defdecl->get_type();
             }
        }

  // patch up the SgClassType for the defining class declaration
     ROSE_ASSERT (nondefdecl->get_type() != NULL);
     ROSE_ASSERT (nondefdecl->get_type()->get_declaration() == isSgDeclarationStatement(nondefdecl));
     defdecl->set_type(nondefdecl->get_type());
#else
  // printf ("We might need to force the types used for defining and non-defining SgTemplateClassDeclaration to be the same! \n");
     ROSE_ASSERT(nondefdecl->get_type() != NULL);
#endif

  // I don't think this is always a forward declaration (e.g. if it is not used in a prototype).
  // Checking the olded EDG/ROSE interface it appears that it is always marked forward (unless
  // used in a defining declaration).
     nondefdecl->setForward();

     if (scope != NULL)  // put into fixStructDeclaration() or alike later on
        {
       // fixStructDeclaration(defdecl,scope);
       // fixStructDeclaration(nondefdecl,scope);

       // printf ("***** WARNING *****: Commented out call to fixStructDeclaration() \n");
       // ROSE_ASSERT(false);
        }

#if 0
     ROSE_ASSERT(defdecl->get_definingDeclaration() == defdecl);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != defdecl->get_definingDeclaration());

     ROSE_ASSERT(defdecl->get_scope() != NULL);
#endif

  // DQ (7/15/2012): We want to enforce this to not be set yet (might be part of non-autonomous declaration (e.g. nested in a typedef).
     ROSE_ASSERT(nondefdecl->get_parent() == NULL);

  // DQ (9/12/2012): Test that the templateName is set (name without template specialization parameters).
     ROSE_ASSERT(nondefdecl->get_templateName().is_null() == false);

     testTemplateArgumentParents(nondefdecl);

#if 0
  // DQ (1/27/2019): Test that symbol table to debug Cxx11_tests/test2019)33.C.
     printf ("Leaving buildNondefiningTemplateClassDeclaration_nfi(): Calling find_symbol_from_declaration() \n");
     SgClassDeclaration* tmp_classDeclaration = nondefdecl;
     SgSymbol* test_symbol = nondefdecl->get_scope()->find_symbol_from_declaration(tmp_classDeclaration);

  // DQ (1/27/2019): Test that symbol table to debug Cxx11_tests/test2019)33.C.
     printf ("Leaving buildNondefiningTemplateClassDeclaration_nfi(): Calling get_symbol_from_symbol_table() \n");
     ROSE_ASSERT(nondefdecl->get_symbol_from_symbol_table() != NULL);
#endif

     return nondefdecl;
   }

// SgTemplateClassDeclaration * SageBuilder::buildTemplateClassDeclaration_nfi(SgName & name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgTemplateClassDeclaration* nonDefiningDecl )
// SgTemplateClassDeclaration * SageBuilder::buildTemplateClassDeclaration_nfi(const SgName& name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgTemplateClassDeclaration* nonDefiningDecl )
SgTemplateClassDeclaration *
SageBuilder::buildTemplateClassDeclaration_nfi(const SgName& XXX_name, SgClassDeclaration::class_types kind, SgScopeStatement* scope, SgTemplateClassDeclaration* nonDefiningDecl,
                                               SgTemplateParameterPtrList* templateParameterList, SgTemplateArgumentPtrList* templateSpecializationArgumentList )
   {
  // DQ (12/26/2011): Notes that the input nonDefiningDecl is not used...this is a confusing point.
  // The specification of the scope appears to be enough.

     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

#if 0
     printf ("In buildTemplateClassDeclaration_nfi(): nonDefiningDecl = %p \n",nonDefiningDecl);
     if (nonDefiningDecl != NULL)
        {
          printf ("--- nonDefiningDecl->get_firstNondefiningDeclaration() = %p \n",nonDefiningDecl->get_firstNondefiningDeclaration());
          printf ("--- nonDefiningDecl->get_definingDeclaration()         = %p \n",nonDefiningDecl->get_definingDeclaration());
        }
#endif

  // DQ (11/20/2011): This is for initial debugging only.
     ROSE_ASSERT(scope != NULL);

  // DQ (9/12/2012): We want to add the template arguments of any specialization to the template name and keep track of the name with and without template specialization arguments.
     SgName nameWithoutTemplateArguments            = XXX_name;
     SgName nameWithTemplateSpecializationArguments = appendTemplateArgumentsToName(nameWithoutTemplateArguments,*templateSpecializationArgumentList);

  // step 1. Build defining declaration
  // Note that even the SgTemplateClassDeclaration uses a regular SgClassDefinition instead of the currently unused SgTemplateClassDefinition.
  // SgClassDefinition* classDef = buildClassDefinition();
  // SgTemplateClassDefinition* classDef = buildTemplateClassDefinition(name,);

  // DQ (11/29/2011): Added checks...
     if (nonDefiningDecl != NULL)
        {
          ROSE_ASSERT(nonDefiningDecl->get_firstNondefiningDeclaration() == nonDefiningDecl);
        }

     SgName templateString = "template string";
  // SgTemplateDeclaration::template_type_enum template_kind = SgTemplateDeclaration::e_template_class;
     SgTemplateParameterPtrList templateParameters;

  // SgTemplateDeclaration (SgName name, SgName string, SgTemplateDeclaration::template_type_enum template_kind, SgTemplateParameterPtrList templateParameters)
  // SgTemplateClassDeclaration* defdecl = new SgTemplateClassDeclaration (name,templateString,template_kind,templateParameters,kind,NULL,classDef);
  // SgTemplateClassDeclaration* defdecl = new SgTemplateClassDeclaration (name,templateString,template_kind,templateParameters);

  // SgTemplateClassDeclaration::class_types template_class_kind = SgTemplateClassDeclaration::e_class;
  // SgTemplateType*            classType = NULL;
  // SgTemplateClassDefinition* classDef  = NULL;
     SgTemplateClassDefinition* classDef = buildTemplateClassDefinition();

  // Constructure arguments: SgName, SgName, SgTemplateDeclaration::template_type_enum, SgTemplateParameterPtrList, SgTemplateClassDeclaration::class_types, SgClassType*, SgTemplateClassDefinition*
  // SgTemplateClassDeclaration* defdecl = new SgTemplateClassDeclaration (name,templateString,template_kind,templateParameters);
  // SgTemplateClassDeclaration* defdecl = new SgTemplateClassDeclaration (name,templateString,template_kind,templateParameters,template_class_kind,classType,classDef);
  // SgTemplateClassDeclaration* defdecl = new SgTemplateClassDeclaration (name,templateString,template_kind,templateParameters,template_class_kind,classDef);

#if 0
     printf ("In buildTemplateClassDeclaration_nfi(): calling new SgTemplateClassDeclaration() name = %s \n",nameWithTemplateSpecializationArguments.str());
#endif

  // DQ (9/12/2012): We want to include the template specialization into the name where it is required (this handling
  // is similar to normal template arguments for non-template declaration, but different than template parameters).
  // This copy of SgName is required to support passing it to the SgTemplateClassDeclaration constructor.
  // SgName localName = name;
  // SgTemplateClassDeclaration* defdecl = new SgTemplateClassDeclaration (name,template_class_kind,classDef);
  // SgTemplateClassDeclaration* defdecl = new SgTemplateClassDeclaration (name,kind,NULL,classDef);

  // DQ (1/13/2013): This is causing two defining declarations to be built for test2012_278.C (and the parent for the second defining
  // declaration is not being set, though the larger issue is that we have two defining declarations, however this might be acceptable
  // if this is a specialization).
  // SgTemplateClassDeclaration* defdecl = new SgTemplateClassDeclaration (nameWithTemplateSpecializationArguments,kind,NULL,classDef);
     SgTemplateClassDeclaration* defdecl = NULL;
     if (nonDefiningDecl != NULL)
        {
       // If we have a non-defining declaration specified, try to use any existing defining declaration withouth building a 2nd one
       // (which would be an error, unless maybe if this is a specialization).
          if (nonDefiningDecl->get_definingDeclaration() != NULL)
             {
            // This must be a valid SgTemplateClassDefinition.
               defdecl = isSgTemplateClassDeclaration(nonDefiningDecl->get_definingDeclaration());
               ROSE_ASSERT(defdecl != NULL);
#if 0
               printf ("In buildTemplateClassDeclaration_nfi(): Reusing the defining declaration previously build: defdecl = %p = %s \n",defdecl,defdecl->get_name().str());
#endif
             }
            else
             {
#if 0
               printf ("In buildTemplateClassDeclaration_nfi(): No defining declaration found, so we have to build one. \n");
#endif
             }
        }

     if (defdecl == NULL)
        {
#if 0
          printf ("Building a defining declaration \n");
#endif
          defdecl = new SgTemplateClassDeclaration (nameWithTemplateSpecializationArguments,kind,NULL,classDef);
        }

     ROSE_ASSERT(defdecl != NULL);

#if 0
     printf ("In buildTemplateClassDeclaration_nfi(): defdecl = %p = %s \n",defdecl,defdecl->class_name().c_str());
#endif

  // DQ (9/10/2012): Initialize the template parameter list.
     ROSE_ASSERT(templateParameterList != NULL);
     defdecl->get_templateParameters() = *templateParameterList;

  // DQ (9/12/2012): Adding support for template specialization.
     ROSE_ASSERT(templateSpecializationArgumentList != NULL);
     defdecl->get_templateSpecializationArguments() = *templateSpecializationArgumentList;

  // DQ (9/16/2012): We can't test this yet, since the firstNondefiningDeclaration has not be set.
  // testTemplateArgumentParents(defdecl);

  // DQ (9/10/2012): Test the just built template with its template parameters.
     if (defdecl->get_templateParameters().size() == 0)
        {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
          printf ("WARNING: In buildTemplateClassDeclaration_nfi(): defdecl->get_templateParameters().size() == 0: OK for nested classes/structs in template declarations defdecl = %p \n",defdecl);
#endif
        }
  // ROSE_ASSERT(defdecl->get_templateParameters().size() > 0);

  // DQ (9/12/2012): Set the template name to be the name without template specialization arguments.
     defdecl->set_templateName(nameWithoutTemplateArguments);

#if 0
     printf ("SageBuilder::buildTemplateClassDeclaration_nfi(): scope = %p = %s \n",scope,scope->class_name().c_str());
#endif

     defdecl->set_scope(scope);

  // DQ (7/15/2012): To support non-autonomous declarations (declarations nested in types) we don't want to set the parent here.  It will be set later.
  // DQ (11/20/2011): Can name qualification make this incorrect?
  // defdecl->set_parent(scope);

     ROSE_ASSERT(classDef != NULL);

  // printf ("SageBuilder::buildTemplateClassDeclaration_nfi(): defdecl = %p \n",defdecl);

     setOneSourcePositionForTransformation(defdecl);

  // constructor is side-effect free
     classDef->set_declaration(defdecl);
     defdecl->set_definingDeclaration(defdecl);

  // Step 2. build the nondefining declaration,
  // but only if the input nonDefiningDecl pointer was NULL and it does not exist

  // Get the nondefining declaration from the symbol if it has been built (if this works,
  // then we likely don't need the "SgClassDeclaration* nonDefiningDecl" parameter).
     SgTemplateClassDeclaration* nondefdecl = nonDefiningDecl;
     if (nondefdecl == NULL) {
       ROSE_ASSERT(scope != NULL);

       SgClassSymbol* mysymbol = scope->lookup_template_class_symbol(nameWithTemplateSpecializationArguments,templateParameterList,templateSpecializationArgumentList);
#if 0
       printf ("In buildTemplateClassDeclaration_nfi(): mysymbol = %p \n",mysymbol);
#endif

       if (mysymbol != NULL) {
         nondefdecl = isSgTemplateClassDeclaration(mysymbol->get_declaration());
         ROSE_ASSERT(nondefdecl != NULL);

         nondefdecl->set_definingDeclaration(defdecl);
         ROSE_ASSERT(nondefdecl->get_definingDeclaration() == defdecl);
         ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != defdecl);

      // DQ (9/12/2012): Test that the templateName is set (name without template specialization parameters).
         ROSE_ASSERT(nondefdecl->get_templateName().is_null() == false);

      // DQ (9/16/2012): Test this previously setup firstNondefiningDeclaration.
         ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() == nondefdecl);
         testTemplateArgumentParents(nondefdecl);
       } else {
#if 0
         printf("  start build non-defn decl for %p\n",defdecl);
#endif
      // DQ (1/25/2009): We only want to build a new declaration if we can't reuse the existing declaration.
         nondefdecl = new SgTemplateClassDeclaration(nameWithTemplateSpecializationArguments,kind,NULL,NULL);
         ROSE_ASSERT(nondefdecl != NULL);

         nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
         nondefdecl->set_definingDeclaration(defdecl);
#if 0
         printf("  nondefdecl = %p\n",nondefdecl);
#endif
#if 0
      // TV (04/12/2018): Add a scope for nonreal classes (and their member) on the first non-defining declaration of template classes
         SgDeclarationScope * nonreal_decl_scope = new SgDeclarationScope();

         nonreal_decl_scope->set_parent(nondefdecl);
         nondefdecl->set_nonreal_decl_scope(nonreal_decl_scope);

         SageInterface::setSourcePosition(nonreal_decl_scope);
         nonreal_decl_scope->get_startOfConstruct()->setCompilerGenerated();
         nonreal_decl_scope->get_endOfConstruct()->setCompilerGenerated();
#if 1
         printf("In buildTemplateClassDeclaration_nfi(): nrscope = %p\n", nonreal_decl_scope);
#endif
#endif
      // DQ (9/10/2012): Initialize the template parameter list.
         ROSE_ASSERT(templateParameterList != NULL);
         nondefdecl->get_templateParameters() = *templateParameterList;

      // DQ (9/16/2012): Newly refactored code.
         setTemplateSpecializationArgumentsInDeclaration(nondefdecl,templateSpecializationArgumentList);
         testTemplateArgumentParents(nondefdecl);

      // DQ (9/10/2012): Test the just built template with its template parameters.
      // ROSE_ASSERT(nondefdecl->get_templateParameters().size() > 0);
         if (nondefdecl->get_templateParameters().size() == 0)
            {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
              printf ("WARNING: In buildTemplateClassDeclaration_nfi(): nondefdecl->get_templateParameters().size() == 0: OK for nested classes/structs in template declarations nondefdecl = %p \n",nondefdecl);
#endif
            }
      // ROSE_ASSERT(nondefdecl->get_templateParameters().size() > 0);
#if 0
         printf("  next 1\n");
#endif

      // DQ (9/12/2012): Set the template name to be the name without template specialization arguments.
         nondefdecl->set_templateName(nameWithoutTemplateArguments);

      // DQ (12/26/2011): The non defining declaration should not have a valid pointer to the class definition.
         ROSE_ASSERT(nondefdecl->get_definition() == NULL);

      // The nondefining declaration will not appear in the source code, but is compiler
      // generated (so we have something about the class that we can reference; e.g in
      // types).  At the moment we make it a transformation, there might be another kind
      // of source position that would be more precise.  FIXME.
         setOneSourcePositionForTransformation(nondefdecl);
         ROSE_ASSERT (nondefdecl->get_startOfConstruct() != NULL);

         nondefdecl->setForward();

      // Liao, 9/2/2009. scope stack is optional, it can be empty
         nondefdecl->set_parent(scope);
         nondefdecl->set_scope(scope);
#if 0
         printf("  next 2\n");
#endif

         if (nondefdecl->get_type() == NULL)
            {
              nondefdecl->set_type(SgClassType::createType(nondefdecl));
#if 0
              printf ("In SageBuilder::buildTemplateClassDeclaration_nfi(): built class type: part 1: nondefdecl->get_type() = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
#endif
            }
#if 0
         printf("  next 3\n");
#endif

      // DQ (9/12/2012): Test that the templateName is set (name without template specialization parameters).
         ROSE_ASSERT(nondefdecl->get_templateName().is_null() == false);

      // DQ (7/16/2017): Added code to set the template parameters in the just build declaration (if it is a template declaration).
      // We want to set the parents of the template paremters to the frst nondefining template class declaration, and we want to reset
      // the scope of the declarations associated with any previously marked SgClassType objects associated with any template parameters.
      // printf ("SageBuilder::buildTemplateClassDeclaration_nfi(): Calling setTemplateParametersInDeclaration(): nameWithTemplateSpecializationArguments = %s \n",nameWithTemplateSpecializationArguments.str());

         setTemplateParametersInDeclaration(nondefdecl,templateParameterList);

      // DQ (8/13/2013): Adding test of template parameter lists.
         SgTemplateClassDeclaration* templateClassDeclaration = isSgTemplateClassDeclaration(nondefdecl);
         ROSE_ASSERT(templateClassDeclaration == NULL || (templateParameterList != NULL && templateParameterList->size() == templateClassDeclaration->get_templateParameters().size()));

         mysymbol = new SgTemplateClassSymbol(nondefdecl);
         scope->insert_symbol(nameWithTemplateSpecializationArguments, mysymbol);
#if 0
         printf("  end build non-defn decl\n");
#endif
       }
     } else {
       SgClassSymbol* mysymbol = scope->lookup_template_class_symbol(nameWithTemplateSpecializationArguments,templateParameterList,templateSpecializationArgumentList);
       if (mysymbol == NULL) {
         printf("WARNING: In buildTemplateClassDeclaration_nfi(): non-defining declaration was provided but cannot be located in the associated scope.\n");
       }
     }

#if 0
     printf ("In buildTemplateClassDeclaration_nfi(): Setting the firstNondefiningDeclaration to be nondefdecl = %p \n",nondefdecl);
#endif

     defdecl->set_firstNondefiningDeclaration(nondefdecl);

  // DQ (9/16/2012): Setup the template specialization arguments on the defining declaration (tested below at base of function).
     setTemplateSpecializationArgumentsInDeclaration(defdecl,templateSpecializationArgumentList);

#if 1
  // DQ (11/20/2011): SgTemplateClassDeclaration IR nodes don't have a SgType associated with them (template declarations don't have a type in C++, I think).

  // Liao, 10/30/2009
  // The SgClassDeclaration constructor will automatically generate a SgClassType internally if NULL is passed for SgClassType
  // This is not desired when building a defining declaration and an inefficience in the constructor
  // Ideally, only the first nondefining class declaration should have a dedicated SgClassType and
  // the defining class declaration (and other nondefining declaration) just shared that SgClassType.
     if (defdecl->get_type() != NULL)
        {
       // if a defining class declaration's type is associated with a defining class.
       // This is a wrong SgClassType and has to be reset
#if 0
       // if (defdecl->get_type()->get_declaration() == isSgDeclarationStatement(defdecl) )
          if (defdecl->get_type()->get_declaration() == isSgDeclarationStatement(defdecl) )
             {
               delete defdecl->get_type();
             }
#else
       // DQ (1/13/2013): I am not clear what this means... if (defdecl->get_type() != NULL) then it makes
       // no sense to assert that (defdecl->get_type() == NULL).  This is related to the reuse of the defining
       // declaration when it is available (instead of building a new one, which still might be required for a
       // template specialization (or template partial specialization)).
       // ROSE_ASSERT(defdecl->get_type() == NULL);
#endif
        }

  // patch up the SgClassType for the defining class declaration
     ROSE_ASSERT (nondefdecl->get_type() != NULL);
  // ROSE_ASSERT (nondefdecl->get_type()->get_declaration() == isSgDeclarationStatement(nondefdecl));

  // DQ (1/22/2013): This assertion is a problem for boost code represented by ROSE compiling ROSE (see testRoseHeaders_01.C)
     if (isSgClassType(nondefdecl->get_type())->get_declaration() != isSgDeclarationStatement(nondefdecl))
        {
#if 0
          printf ("In buildTemplateClassDeclaration_nfi(): detected isSgClassType(nondefdecl->get_type())->get_declaration() != isSgDeclarationStatement(nondefdecl) (problem with Boost code in ROSE compiling ROSE) \n");
#endif
        }
  // ROSE_ASSERT (isSgClassType(nondefdecl->get_type())->get_declaration() == isSgDeclarationStatement(nondefdecl));

     defdecl->set_type(nondefdecl->get_type());

#if 0
     printf ("In SageBuilder::buildTemplateClassDeclaration_nfi(): built class type: part 2: defdecl->get_type() = %p = %s \n",defdecl->get_type(),defdecl->get_type()->class_name().c_str());
#endif
#endif

  // I don't think this is always a forward declaration (e.g. if it is not used in a prototype).
  // Checking the olded EDG/ROSE interface it appears that it is always marked forward (unless
  // used in a defining declaration).
     nondefdecl->setForward();

#if 0
     if (scope != NULL)  // put into fixStructDeclaration() or alike later on
        {
       // fixStructDeclaration(defdecl,scope);
       // fixStructDeclaration(nondefdecl,scope);

          printf ("***** WARNING *****: Commented out call to fixStructDeclaration() \n");
       // ROSE_ASSERT(false);
        }
#endif

     ROSE_ASSERT(defdecl->get_definingDeclaration() == defdecl);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != defdecl->get_definingDeclaration());

     ROSE_ASSERT(defdecl->get_scope() != NULL);

  // DQ (12/4/2011): We need a concept for type for SgTemplateClassDeclaration so that we can construct SgMemberFunctionType nodes for template member functions.
  // We use a SgClassType which has been fixed to permit it to hold either a SgClassDeclaration or an SgTemplateClassDeclaration.
     ROSE_ASSERT(defdecl->get_type() != NULL);

  // DQ (12/26/2011): The non defining declaration should not have a valid pointer to the class definition.
     ROSE_ASSERT(nondefdecl->get_definition() == NULL);

  // DQ (7/15/2012): We want to inforce this.
  // ROSE_ASSERT(defdecl->get_parent() == NULL);
     if (defdecl->get_parent() != NULL)
        {
#if PRINT_DEVELOPER_WARNINGS
          printf ("WARNING: the parent will have been set if the defining declaration was found and reused! defdecl = %p = %s \n",defdecl,defdecl->class_name().c_str());
#endif
        }

  // DQ (9/12/2012): Test that the templateName is set (name without template specialization parameters).
     ROSE_ASSERT(defdecl->get_templateName().is_null() == false);

  // DQ (9/12/2012): Test that the templateName is set (name without template specialization parameters).
     ROSE_ASSERT(nondefdecl->get_templateName().is_null() == false);

     testTemplateArgumentParents(defdecl);
     testTemplateArgumentParents(nondefdecl);

#if 0
  // DQ (1/27/2019): Test that symbol table to debug Cxx11_tests/test2019)33.C.
     printf ("Leaving buildTemplateClassDeclaration_nfi(): Calling find_symbol_from_declaration() \n");
     SgClassDeclaration* tmp_classDeclaration = defdecl;
     SgSymbol* test_symbol = defdecl->get_scope()->find_symbol_from_declaration(tmp_classDeclaration);

  // DQ (1/27/2019): Test that symbol table to debug Cxx11_tests/test2019)33.C.
     printf ("Leaving buildTemplateClassDeclaration_nfi(): Calling get_symbol_from_symbol_table() \n");
     ROSE_ASSERT(defdecl->get_symbol_from_symbol_table() != NULL);
#endif

     return defdecl;
   }

SgEnumDeclaration * SageBuilder::buildEnumDeclaration(const SgName& name, SgScopeStatement* scope /*=NULL*/)
  {
 // DQ (1/11/2009): This function has semantics very different from the buildEnumDeclaration_nfi() function!

    if (scope == NULL)
         scope = SageBuilder::topScopeStack();

    SgEnumDeclaration* decl = buildEnumDeclaration_nfi(name, scope);
    setOneSourcePositionForTransformation(decl);
    setOneSourcePositionForTransformation(decl->get_firstNondefiningDeclaration());
    setOneSourcePositionForTransformation(decl->get_definingDeclaration());

  // DQ (7/15/2012): We want to inforce this.
     ROSE_ASSERT(decl->get_parent() == NULL);

    return decl;
  } //buildEnumDeclaration()


SgEnumDeclaration*
SageBuilder::buildNondefiningEnumDeclaration_nfi(const SgName& name, SgScopeStatement* scope)
   {
  // The support for SgEnumDeclaration is identical to that for SgClassDeclaration (excpet for the type handleing, why is that?).

  // DQ (7/27/2012): Note that the input name should not have template argument syntax.
  // I think this could still fail for a function with a name such as "X<Y>"  strange converstion operators.
     ROSE_ASSERT(SageInterface::hasTemplateSyntax(name) == false);

  // DQ (4/22/2013): We need this to be provided.
     ROSE_ASSERT(scope != NULL);

#if 0
     printf ("In buildNondefiningEnumDeclaration_nfi(): name = %s scope = %p = %s \n",name.str(),scope,scope->class_name().c_str());
#endif

  // DQ (5/8/2013): For testing with test2007_140.C, make sure this is not the SgBasicBlock (should be SgClassDefinition).
  // ROSE_ASSERT(isSgBasicBlock(scope) == NULL);

  // DQ (5/8/2013): I think if we searched for the type it might exist and this would allow the types to be shared.
     SgEnumType* enumType = NULL;
  // SgEnumDeclaration* nondefdecl = NULL;
     SgEnumDeclaration* first_nondefdecl = NULL;

     if (scope != NULL)
        {
          SgEnumSymbol* existing_symbol = scope->lookup_enum_symbol(name);
          if (existing_symbol != NULL)
             {
               enumType = isSgEnumType(existing_symbol->get_type());
               first_nondefdecl = existing_symbol->get_declaration();
               ROSE_ASSERT(first_nondefdecl != NULL);
             }
        }

#if 0
     printf ("In buildNondefiningEnumDeclaration_nfi(): name = %s building using enumType = %p first_nondefdecl = %p \n",name.str(),enumType,first_nondefdecl);
#endif

  // DQ (5/8/2013): We do want to build a new SgEnumDeclaration (to avoid sharing).
  // This forces each call to buildNondefiningEnumDeclaration_nfi() to build a unique declaration
  // required to avoid sharing declaration in examples such as test2007_29.C.
  // SgEnumDeclaration* nondefdecl = new SgEnumDeclaration(name, NULL);
     SgEnumDeclaration* nondefdecl = new SgEnumDeclaration(name, enumType);

     ROSE_ASSERT(nondefdecl);
     setOneSourcePositionNull(nondefdecl);

  // DQ (5/8/2013): Set the definig and first non-defining declarations.
  // nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
  // nondefdecl->set_definingDeclaration(NULL);
     if (first_nondefdecl != NULL)
        {
          nondefdecl->set_firstNondefiningDeclaration(first_nondefdecl);
          nondefdecl->set_definingDeclaration(first_nondefdecl->get_definingDeclaration());
        }
       else
        {
          nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
          nondefdecl->set_definingDeclaration(NULL);
        }

  // Any non-defining declaration is not always a forward declaration.
     nondefdecl->setForward();

  // SgEnumDeclaration* first_nondefdecl = NULL;

  // DQ (5/8/2013): Make sure that the enum type is available.
     SgType* type = nondefdecl->get_type();
     ROSE_ASSERT(type != NULL);

     if (scope != NULL)
        {
       // DQ (4/22/2013): check for an existing symbol (reuse it if it is found).
          SgEnumSymbol* mysymbol = NULL;
          SgEnumSymbol* existing_symbol = scope->lookup_enum_symbol(name);
       // ROSE_ASSERT(existing_symbol == NULL);

          if (existing_symbol != NULL)
             {
               mysymbol = existing_symbol;
#if 0
               nondefdecl = new SgEnumDeclaration(name, NULL);
               ROSE_ASSERT(nondefdecl != NULL);
#else
               first_nondefdecl = mysymbol->get_declaration();
#endif
             }
            else
             {
#if 0
               nondefdecl = new SgEnumDeclaration(name, NULL);
               ROSE_ASSERT(nondefdecl != NULL);
               setOneSourcePositionNull(nondefdecl);
               nondefdecl->set_firstNondefiningDeclaration(nondefdecl);
               nondefdecl->set_definingDeclaration(NULL);

            // Any non-defining declaration is not always a forward declaration.
               nondefdecl->setForward();
#endif
               first_nondefdecl = nondefdecl;

               mysymbol = new SgEnumSymbol(nondefdecl);
               ROSE_ASSERT(mysymbol != NULL);
               scope->insert_symbol(name, mysymbol);
             }

       // SgEnumSymbol* mysymbol = new SgEnumSymbol(nondefdecl);

          nondefdecl->set_scope(scope);

       // Can this be defined in C++ so that it is in a logical scope different from its structural scope?
          nondefdecl->set_parent(scope);
        }

     if (first_nondefdecl != nondefdecl)
        {
          nondefdecl->set_firstNondefiningDeclaration(first_nondefdecl);
       // nondefdecl->set_definingDeclaration(first_nondefdecl->get_definingDeclaration());

          if (first_nondefdecl->get_definingDeclaration() != NULL)
             {
               nondefdecl->set_definingDeclaration(first_nondefdecl->get_definingDeclaration());
             }
        }

  // DQ (1/25/2009): I want to check into this later, since it is not symetric with SageBuilder::buildNondefiningClassDeclaration()
#if 0
     printf ("In buildNondefiningEnumDeclaration_nfi(): Need to check if SgEnumDeclaration constructor builds the SgEnumType already nondefdecl->get_type() = %p \n",nondefdecl->get_type());
#endif
     ROSE_ASSERT(nondefdecl->get_type() != NULL);

  // DQ (4/22/2013): This type should already exist (likely built by call to get_type()).
  // SgEnumType* t = new SgEnumType(nondefdecl);
  // nondefdecl->set_type(t);

#if 0
     printf ("Leaving buildNondefiningEnumDeclaration_nfi(): name = %s nondefdecl = %p \n",name.str(),nondefdecl);
#endif

  // DQ (5/8/2013): Check that the symbol is present.
     ROSE_ASSERT(scope->lookup_enum_symbol(name) != NULL);

     return nondefdecl;
   }


SgEnumDeclaration*
SageBuilder::buildEnumDeclaration_nfi(const SgName& name, SgScopeStatement* scope)
   {
     ROSE_ASSERT(scope != NULL);

#if 0
     printf ("In buildEnumDeclaration_nfi(): name = %s scope = %p = %s \n",name.str(),scope,scope->class_name().c_str());
#endif

  // DQ (5/8/2013): I think if we searched for the type it might exist and this would allow the types to be shared.
     SgEnumType* enumType = NULL;

     if (scope != NULL)
        {
          SgEnumSymbol* existing_symbol = scope->lookup_enum_symbol(name);
          if (existing_symbol != NULL)
             {
               enumType = isSgEnumType(existing_symbol->get_type());
             }
        }

#if 0
     printf ("In buildEnumDeclaration_nfi(): name = %s building using enumType = %p \n",name.str(),enumType);
#endif

  // SgEnumDeclaration* defdecl = new SgEnumDeclaration (name,NULL);
     SgEnumDeclaration* defdecl = new SgEnumDeclaration (name,enumType);
     ROSE_ASSERT(defdecl);

#if 0
     printf ("In buildEnumDeclaration_nfi(): built defining declaration = %p name = %s scope = %p = %s \n",defdecl,name.str(),scope,scope->class_name().c_str());
#endif

  // DQ (5/8/2013): Make sure that the enum type is available.
     SgType* type = defdecl->get_type();
     ROSE_ASSERT(type != NULL);

     setOneSourcePositionNull(defdecl);
  // constructor is side-effect free
     defdecl->set_definingDeclaration(defdecl);

#if 0
     printf ("In buildEnumDeclaration_nfi(): name = %s \n",name.str());
#endif

#if 1
  // DQ (4/3/2017): Check for an existing non-defining declaration before building one (to avoid multiple versions). See test2017_13.C.
     ROSE_ASSERT(scope != NULL);
     SgEnumSymbol* enumSymbol = scope->lookup_enum_symbol(name);
  // ROSE_ASSERT(enumSymbol != NULL);
     SgEnumDeclaration* nondefdecl = NULL;
     if (enumSymbol != NULL)
        {
          ROSE_ASSERT(enumSymbol->get_declaration() != NULL);
          nondefdecl = enumSymbol->get_declaration();
          ROSE_ASSERT(nondefdecl != NULL);
        }
       else
        {
       // build the nondefining declaration
          nondefdecl = buildNondefiningEnumDeclaration_nfi(name, scope);
#if 0
          printf ("###### In buildEnumDeclaration_nfi(): built a non-defining declaration to support the symbol table: name = %s nondefdecl = %p \n",name.str(),nondefdecl);
#endif
        }
#else
  // build the nondefining declaration
     SgEnumDeclaration* nondefdecl = buildNondefiningEnumDeclaration_nfi(name, scope);
#endif

     nondefdecl->set_definingDeclaration(defdecl);
  // defdecl->set_firstNondefiningDeclaration(nondefdecl);
     defdecl->set_firstNondefiningDeclaration(nondefdecl->get_firstNondefiningDeclaration());

  // DQ (4/22/2013): We need to set the defining declaration on the first non-defining declaration.
     if (nondefdecl->get_firstNondefiningDeclaration() != NULL && nondefdecl->get_firstNondefiningDeclaration() != nondefdecl)
        {
          nondefdecl->get_firstNondefiningDeclaration()->set_definingDeclaration(defdecl);
        }

  // DQ (4/22/2013): Thing that should be true at this point.
     ROSE_ASSERT(nondefdecl->get_definingDeclaration() != NULL);
     ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration()->get_definingDeclaration() != NULL);
     ROSE_ASSERT(nondefdecl->get_firstNondefiningDeclaration()->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(defdecl->get_definingDeclaration() != NULL);
     ROSE_ASSERT(defdecl->get_firstNondefiningDeclaration() != NULL);

  // DQ (1/11/2009): The buildNondefiningEnumDeclaration function builds an entry in the symbol table, and so we don't want a second one!
#if 0
     SgEnumSymbol* mysymbol = new SgEnumSymbol(nondefdecl);
     ROSE_ASSERT(mysymbol);
  // scope->print_symboltable("buildEnumDeclaration_nfi(): before inserting new SgEnumSymbol");
     scope->insert_symbol(name, mysymbol);
#endif

     defdecl->set_scope(scope);
     nondefdecl->set_scope(scope);

#if 0
  // DQ (7/12/2012): We can't set the parent here because if this is a non-autonomous declaration then it must be set later (to the outer declaration where this declaration is nested).
     defdecl->set_parent(scope);
     nondefdecl->set_parent(scope);
#endif

  // DQ (7/12/2012): When this is an unnamed enum declaration, this it is NON-AUTONIMOUS
  // (and will have it's parent set in the associated variable or typedef declaration.
  // In the case of a class declaration this is always NULL (this should be similar).
     ROSE_ASSERT(defdecl->get_parent() == NULL);

#if 0
     printf ("In buildEnumDeclaration_nfi(): name = %s defdecl = %p \n",name.str(),defdecl);
#endif

  // DQ (5/8/2013): Check that the symbol is present.
     ROSE_ASSERT(scope->lookup_enum_symbol(name) != NULL);

     return defdecl;
   } //buildEnumDeclaration_nfi()


SgBaseClass*
SageBuilder::buildBaseClass ( SgClassDeclaration* classDeclaration, SgClassDefinition* classDefinition, bool isVirtual, bool isDirect )
   {
  // DQ (5/6/2013): Refactored the construction of the SgBaseClass support to the builder API.

  // Note: classDeclaration should be the first non-defining class declaration, not required to be the the declaration associated with the SgClassDefinition.
     ROSE_ASSERT(classDeclaration != NULL);
     ROSE_ASSERT(classDefinition  != NULL);

  // DQ (5/6/2013): This is not always true (see test2013_63.C).
  // ROSE_ASSERT(classDeclaration == classDeclaration->get_firstNondefiningDeclaration());

     ROSE_ASSERT(classDefinition->get_declaration() != NULL);

  // DQ (5/6/2013): This is not always true (see test2004_30.C).
  // ROSE_ASSERT(classDefinition->get_declaration() == classDeclaration->get_firstNondefiningDeclaration());

     SgBaseClass* baseclass = new SgBaseClass ( classDeclaration, isDirect );
     ROSE_ASSERT(baseclass != NULL);

     if (isVirtual == true)
        {
       // DQ (1/21/2019): get_baseClassModifier() uses ROSETTA generated access functions which return a pointer.
       // baseclass->get_baseClassModifier().setVirtual();
          ROSE_ASSERT(baseclass->get_baseClassModifier() != NULL);
          baseclass->get_baseClassModifier()->setVirtual();
        }

  // DQ (4/29/2004): add support to set access specifier
  // baseclass->get_baseClassModifier().get_accessModifier() = set_access_modifiers(bcdp->access);
  // baseclass->get_baseClassModifier().get_accessModifier() = buildAccessModifier(accessModifiers);

  // DQ (6/21/2005): Set the parent of the base class to the class definition
  // (these are not traversed in ROSE currently, so their parents are not set).
     baseclass->set_parent(classDefinition);

  // DQ (6/21/2005): Notice that this is copied by value (the base class list should be a list of pointers to SgBaseClass (later)
     classDefinition->append_inheritance(baseclass);

     return baseclass;
   }


SgNonrealBaseClass*
SageBuilder::buildNonrealBaseClass ( SgNonrealDecl* nrdecl, SgClassDefinition* classDefinition, bool isVirtual, bool isDirect )
   {
     ROSE_ASSERT(nrdecl != NULL);
     ROSE_ASSERT(classDefinition != NULL);

     SgNonrealBaseClass * baseclass = new SgNonrealBaseClass ( NULL , isDirect , nrdecl );
     ROSE_ASSERT(baseclass != NULL);

     if (isVirtual == true)
        {
          baseclass->get_baseClassModifier()->setVirtual();
        }

     baseclass->set_parent(classDefinition);

     classDefinition->append_inheritance(baseclass);

     return baseclass;
   }


#if 0
// This function would be more complex that I want to support at present since the mapping of
// edg modifier values to ROSE modifier values is offset and backwards (reversed in numerical order).
SgAccessModifier
SageBuilder::buildAccessModifier ( unsigned int access )
   {
     SgAccessModifier a;

     switch (access)
        {
          case as_public:
#if 0
               printf ("In SageBuilder::set_access_modifiers(): Mark as public \n");
#endif
               a.setPublic();
               break;

          case as_protected:
#if 0
               printf ("In SageBuilder::set_access_modifiers(): Mark as protected \n");
#endif
               a.setProtected();
               break;

          case as_private:
#if 0
               printf ("In SageBuilder::set_access_modifiers(): Mark as private \n");
#endif
               a.setPrivate();
               break;

          default:
               printf ("Error: default reached in SageBuilder::set_access_modifiers() \n");
               ROSE_ASSERT (false);
        }

     return a;
   }
#endif


void
SageBuilder::fixupSourcePositionFileSpecification(SgNode* subtreeRoot, const std::string& newFileName)
   {
  // DQ (11/8/2019): This function changes the filename designation in all of the Sg_File_Info objects 
  // associated with the designated AST subtree.

     ROSE_ASSERT(subtreeRoot != NULL);
     ROSE_ASSERT(newFileName != "");

#if 0
     printf ("In SageBuilder::fixupSourcePositionFileSpecification(): newFileName = %s \n",newFileName.c_str());
     printf ("In SageBuilder::fixupSourcePositionFileSpecification(): subtreeRoot = %p = %s \n",subtreeRoot,subtreeRoot->class_name().c_str());
#endif

     class Traversal : public AstSimpleProcessing
        {
          public:

               Traversal(const std::string& tmp_newFileName, int tmp_new_file_id, int tmp_originalFileId)
                  {
                    newFileName    = tmp_newFileName;
                    new_file_id    = tmp_new_file_id;
                    originalFileId = tmp_originalFileId;
#if 0
                    printf ("In SageBuilder::fixupSourcePositionFileSpecification(): newFileName = %s new_file_id = %d originalFileId = %d \n",newFileName.c_str(),new_file_id,originalFileId);
#endif
                  }

               void visit (SgNode* node)
                  {
#if 0
                    printf ("In visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif

                    SgLocatedNode* locatedNode = isSgLocatedNode(node);
                    if (locatedNode != NULL)
                       {
                      // if (locatedNode->get_startOfConstruct()->get_file_id() == originalFileId)
                         if (locatedNode->get_startOfConstruct()->get_physical_file_id() == originalFileId)
                            {
                              ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
                              ROSE_ASSERT(locatedNode->get_endOfConstruct()   != NULL);

                              if (locatedNode->get_startOfConstruct()->isShared() == true)
                                 {
#if 0
                                   printf ("Found SgLocatedNode marked as isShared() == true: locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
#endif
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                              locatedNode->get_startOfConstruct()->set_file_id(new_file_id);
                              locatedNode->get_endOfConstruct  ()->set_file_id(new_file_id);

                              locatedNode->get_startOfConstruct()->set_physical_file_id(new_file_id);
                              locatedNode->get_endOfConstruct  ()->set_physical_file_id(new_file_id);

#if 0
                              printf ("locatedNode->get_startOfConstruct()->get_filename() = %s locatedNode->get_startOfConstruct()->get_physical_filename() = %s \n",
                                   locatedNode->get_startOfConstruct()->get_filenameString().c_str(),locatedNode->get_startOfConstruct()->get_physical_filename().c_str());
                              printf ("locatedNode->get_startOfConstruct()->get_file_id() = %d locatedNode->get_startOfConstruct()->get_physical_file_id() = %d \n",
                                   locatedNode->get_startOfConstruct()->get_file_id(),locatedNode->get_startOfConstruct()->get_physical_file_id());
                              printf ("locatedNode->get_startOfConstruct()->isShared() = %s \n",locatedNode->get_startOfConstruct()->isShared() ? "true" : "false");
#endif
                            }
                           else
                            {
#if 0
                              printf ("NOT MATCHING: originalFileId = %d locatedNode->get_startOfConstruct()->get_file_id() = %d locatedNode->get_startOfConstruct()->get_physical_file_id() = %d \n",
                                      originalFileId,locatedNode->get_startOfConstruct()->get_file_id(),locatedNode->get_startOfConstruct()->get_physical_file_id());
                              printf (" ------------ originalFileId = %d locatedNode->get_endOfConstruct()->get_file_id() = %d locatedNode->get_endOfConstruct()->get_physical_file_id() = %d \n",
                                      originalFileId,locatedNode->get_endOfConstruct()->get_file_id(),locatedNode->get_endOfConstruct()->get_physical_file_id());
#endif
                            }
                       }
                      else
                       {
                         SgInitializedName* initializedName = isSgInitializedName(node);
                         if (initializedName != NULL)
                            {
                           // if (initializedName->get_startOfConstruct()->get_file_id() == originalFileId)
                              if (initializedName->get_startOfConstruct()->get_physical_file_id() == originalFileId)
                                 {
                                   ROSE_ASSERT(initializedName->get_startOfConstruct() != NULL);
                                   ROSE_ASSERT(initializedName->get_endOfConstruct() != NULL);

                                   initializedName->get_startOfConstruct()->set_file_id(new_file_id);
                                   initializedName->get_endOfConstruct  ()->set_file_id(new_file_id);

                                   initializedName->get_startOfConstruct()->set_physical_file_id(new_file_id);
                                   initializedName->get_endOfConstruct  ()->set_physical_file_id(new_file_id);
                                 }
                            }
                           else
                            {
                              SgSourceFile* sourceFile = isSgSourceFile(node);
                              if (sourceFile != NULL)
                                 {
                                   ROSE_ASSERT(sourceFile->get_startOfConstruct() != NULL);
#if 0
                                // A SgSourceFile has no endOfConstruct.
                                   if (sourceFile->get_endOfConstruct() == NULL)
                                      {
#if 0
                                        printf ("sourceFile->get_endOfConstruct() == NULL: fixup endOfConstruct \n");
#endif
                                        sourceFile->set_endOfConstruct(new Sg_File_Info());
                                        *(sourceFile->get_endOfConstruct()) = *(sourceFile->get_startOfConstruct());
                                      }
                                   ROSE_ASSERT(sourceFile->get_endOfConstruct() != NULL);
#endif
                                // Need to test the physical_file_id because we already set the regular file_id (as part of seeding the process).
                                // if (sourceFile->get_startOfConstruct()->get_file_id() == originalFileId)
                                   if (sourceFile->get_startOfConstruct()->get_physical_file_id() == originalFileId)
                                      {
                                        sourceFile->get_startOfConstruct()->set_file_id(new_file_id);
                                        sourceFile->get_startOfConstruct()->set_physical_file_id(new_file_id);
#if 0
                                        printf ("sourceFile->get_startOfConstruct()->get_file_id()          = %d \n",sourceFile->get_startOfConstruct()->get_file_id());
                                        printf ("sourceFile->get_startOfConstruct()->get_physical_file_id() = %d \n",sourceFile->get_startOfConstruct()->get_physical_file_id());
#endif
                                     // sourceFile->get_endOfConstruct  ()->set_file_id(new_file_id);
                                     // sourceFile->get_endOfConstruct  ()->set_physical_file_id(new_file_id);
                                      }
                                 }
                                else
                                 {
#if 0
                                   printf ("Unhandled: node = %p = %s \n",node,node->class_name().c_str());
#endif
                                 }
                            }
                       }

                    SgExpression* expression = isSgExpression(node);
                    if (expression != NULL)
                       {
                         if (expression->get_operatorPosition()->get_physical_file_id() == originalFileId)
                            {
                              expression->get_operatorPosition()->set_file_id(new_file_id);
                              expression->get_operatorPosition()->set_physical_file_id(new_file_id);
                            }
                       }
                  }

             // Data members.
               int new_file_id;
               int originalFileId;
               string newFileName;
        };


     SgFile* file       = isSgFile(subtreeRoot);
     int new_file_id    = -1;
     int originalFileId = -1;

     if (file != NULL)
        {
       // We need to set the filename in at least one Sg_File_Info object so that we can have 
       // the file_id be computed ans saved into the file_id to filename maps.

          originalFileId = file->get_startOfConstruct()->get_file_id();
#if 0
          printf ("originalFileId = %d \n",originalFileId);
#endif
          file->get_startOfConstruct()->set_filenameString(newFileName);
          new_file_id = Sg_File_Info::get_nametofileid_map()[newFileName];


#if 0
          file->get_endOfConstruct()->set_physical_file_id(new_file_id);

          file->get_startOfConstruct()->set_physical_file_id(new_file_id);
          file->get_endOfConstruct()->set_physical_file_id(new_file_id);

                                             // getFilenameFromID
          int new_file_id_2     = Sg_File_Info::getIDFromFilename(newFileName);
#if 0
          printf ("new_file_id = %d new_file_id_2 = %d \n",new_file_id,new_file_id_2);
#endif
          ROSE_ASSERT(new_file_id == new_file_id_2);

          string new_filename_2 = Sg_File_Info::getFilenameFromID(new_file_id);
#if 0
          printf ("newFileName = %s new_filename_2 = %s \n",newFileName.c_str(),new_filename_2.c_str());
#endif
          ROSE_ASSERT(newFileName == new_filename_2);
#endif

#if 0
          printf ("In SageBuilder::fixupSourcePositionFileSpecification(): file != NULL: newFileName = %s new_file_id = %d \n",newFileName.c_str(),new_file_id);
#endif
        }
       else
        {
          SgLocatedNode* subtreeLocatedNode = isSgLocatedNode(subtreeRoot);
          if (subtreeLocatedNode != NULL)
             {
#if 0
               printf ("subtreeLocatedNode->get_startOfConstruct()->get_file_id()          = %d \n",subtreeLocatedNode->get_startOfConstruct()->get_file_id());
               printf ("subtreeLocatedNode->get_startOfConstruct()->get_physical_file_id() = %d \n",subtreeLocatedNode->get_startOfConstruct()->get_physical_file_id());
#endif
               originalFileId = subtreeLocatedNode->get_startOfConstruct()->get_file_id();
               new_file_id = Sg_File_Info::getIDFromFilename(newFileName);
#if 0
               printf ("originalFileId = %d \n",originalFileId);
               printf ("new_file_id    = %d \n",new_file_id);
#endif
#if 0
               printf ("In SageBuilder::fixupSourcePositionFileSpecification(): subtreeLocatedNode = %s : originalFileId = %d newFileName = %s new_file_id = %d \n",
                    subtreeLocatedNode->class_name().c_str(),originalFileId,newFileName.c_str(),new_file_id);
#endif
             }
            else
             {
               printf ("Error: In SageBuilder::fixupSourcePositionFileSpecification(): subtree should be a SgFile or SgLocatedNode: subtreeRoot = %p = %s \n",subtreeRoot,subtreeRoot->class_name().c_str());
               ROSE_ASSERT(false);
             }

#if 0
          printf ("Error: In SageBuilder::fixupSourcePositionFileSpecification(): subtree should be a SgFile: subtreeRoot = %p = %s \n",subtreeRoot,subtreeRoot->class_name().c_str());
          ROSE_ASSERT(false);
#endif
        }

     ROSE_ASSERT(new_file_id >= 0);

  // Now buid the traveral object and call the traversal (preorder) on the function definition.
     Traversal traversal (newFileName,new_file_id,originalFileId);

  // traversal.traverse(subtreeRoot, preorder);
  // traversal.traverseInputFiles(subtreeRoot, preorder);
  // traversal.traverseWithinFile(subtreeRoot, preorder);
     traversal.traverse(subtreeRoot, preorder);

#if 0
     printf ("Exiting as a test in SageBuilder::fixupSourcePositionFileSpecification() \n");
     ROSE_ASSERT(false);
#endif
   }








void
SageBuilder::fixupSharingSourcePosition(SgNode* subtreeRoot, int new_file_id)
   {
  // DQ (11/8/2019): This function changes the filename designation in all of the Sg_File_Info objects 
  // associated with the designated AST subtree.

     ROSE_ASSERT(subtreeRoot != NULL);
     ROSE_ASSERT(new_file_id >= 0);

#if 0
     printf ("In SageBuilder::fixupSharingSourcePosition(): subtreeRoot = %p = %s \n",subtreeRoot,subtreeRoot->class_name().c_str());
     printf ("In SageBuilder::fixupSharingSourcePosition(): new_file_id = %d \n",new_file_id);
#endif

     class Traversal : public AstSimpleProcessing
        {
          public:

               Traversal(int tmp_new_file_id)
                  {
                    new_file_id    = tmp_new_file_id;
#if 0
                    printf ("In SageBuilder::fixupSharingSourcePosition(): new_file_id = %d \n",new_file_id);
#endif
                  }

               void visit (SgNode* node)
                  {
#if 0
                    printf ("In visit(): node = %p = %s new_file_id = %d \n",node,node->class_name().c_str(),new_file_id);
#endif

                    SgStatement* statement = isSgStatement(node);
                    if (statement != NULL)
                       {
                         Sg_File_Info* startOfConstruct = statement->get_startOfConstruct();
                         Sg_File_Info* endOfConstruct   = statement->get_endOfConstruct();
#if 0
                         printf ("new_file_id = %d startOfConstruct->get_physical_file_id() = %d \n",new_file_id,startOfConstruct->get_physical_file_id());
#endif
                      // Only mark the files from the associated file (not statements in header files, for example).
                         if (startOfConstruct->get_physical_file_id() == new_file_id)
                            {
                           // Mark this IR node as being shared
                              startOfConstruct->setShared();
                              endOfConstruct->setShared();

                           // Add this file_id to those file_id that will trigger this IR node to be unparsed.
#if 0
                              printf ("  --- adding entries for file_id and line number to support sharing: new_file_id = %d line = %d end line = %d \n",
                                   new_file_id,startOfConstruct->get_line(),endOfConstruct->get_line());
#endif
                              ROSE_ASSERT(startOfConstruct->get_fileIDsToUnparse().size() == startOfConstruct->get_fileLineNumbersToUnparse().size());
                              ROSE_ASSERT(endOfConstruct->get_fileIDsToUnparse().size()   == endOfConstruct->get_fileLineNumbersToUnparse().size());
                              ROSE_ASSERT(startOfConstruct->get_fileIDsToUnparse().size() == endOfConstruct->get_fileLineNumbersToUnparse().size());

                           // Add this existing_fi->get_file_id() to the list of file id's that will permit the assocated language construct to be unparsed.
                              startOfConstruct->get_fileIDsToUnparse().push_back(new_file_id);
                              startOfConstruct->get_fileLineNumbersToUnparse().push_back(startOfConstruct->get_line());

                              endOfConstruct->get_fileIDsToUnparse().push_back(new_file_id);
                              endOfConstruct->get_fileLineNumbersToUnparse().push_back(endOfConstruct->get_line());

                              ROSE_ASSERT(startOfConstruct->get_fileIDsToUnparse().size() == startOfConstruct->get_fileLineNumbersToUnparse().size());
                              ROSE_ASSERT(endOfConstruct->get_fileIDsToUnparse().size()   == endOfConstruct->get_fileLineNumbersToUnparse().size());
                              ROSE_ASSERT(startOfConstruct->get_fileIDsToUnparse().size() == endOfConstruct->get_fileLineNumbersToUnparse().size());
                            }
                       }
                      else
                       {
#if 0
                         printf ("Unhandled: node = %p = %s \n",node,node->class_name().c_str());
#endif
                       }
                  }

            // Data members.
               int new_file_id;
        };


     SgStatement* statement = isSgStatement(subtreeRoot);
     if (statement != NULL)
        {
#if 0
          printf ("statement->get_startOfConstruct()->get_file_id()          = %d \n",statement->get_startOfConstruct()->get_file_id());
          printf ("statement->get_startOfConstruct()->get_physical_file_id() = %d \n",statement->get_startOfConstruct()->get_physical_file_id());
#endif
#if 0
          printf ("new_file_id    = %d \n",new_file_id);
#endif
#if 0
          printf ("In SageBuilder::fixupSharingSourcePosition(): statement = %s : new_file_id = %d \n",statement->class_name().c_str(),new_file_id);
#endif
        }
       else
        {
          printf ("Error: In SageBuilder::fixupSharingSourcePosition(): subtree should be a SgFile or SgLocatedNode: subtreeRoot = %p = %s \n",subtreeRoot,subtreeRoot->class_name().c_str());
          ROSE_ASSERT(false);
        }

     ROSE_ASSERT(new_file_id >= 0);

  // Now buid the traveral object and call the traversal (preorder) on the function definition.
     Traversal traversal (new_file_id);

  // traversal.traverse(subtreeRoot, preorder);
  // traversal.traverseInputFiles(subtreeRoot, preorder);
  // traversal.traverseWithinFile(subtreeRoot, preorder);
     traversal.traverse(subtreeRoot, preorder);

#if 0
     printf ("Exiting as a test in SageBuilder::fixupSharingSourcePosition() \n");
     ROSE_ASSERT(false);
#endif
   }





  //! Build a SgFile node
SgFile*
SageBuilder::buildFile(const std::string& inputFileName, const std::string& outputFileName, SgProject* project/*=NULL*/)
   {
// Note that ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT defines a reduced set of ROSE to support front-end specific development.
// It is mostly used by quinlan to support laptop development where the smaller set of files permits one to do limited
// development work on a Mac (even with OSX's poor performance with large numbers of debug symbols).  This is an
// infrequently used option.
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT

#if 0
     printf ("In SageBuilder::buildFile(inputFileName = %s, outputFileName = %s, project = %p \n",inputFileName.c_str(),outputFileName.c_str(),project);
  // printf (" --- fullname = %s \n",fullname.c_str());
#endif

     ROSE_ASSERT(inputFileName.size() != 0); // empty file name is not allowed.

  // DQ (9/18/2019): I am unclear what the use of fullname is below.
  // string sourceFilename = inputFileName, fullname;
  // string sourceFilename_fullname = inputFileName, fullname;
     string sourceFilename          = inputFileName;

#if 0
  // printf ("sourceFilename_fullname = %s \n",sourceFilename_fullname.c_str());
     printf ("sourceFilename          = %s \n",sourceFilename.c_str());
#endif

  // DQ (9/18/2019): Test that the use of fullname has no effect.
  // ROSE_ASSERT(sourceFilename == sourceFilename_fullname);

     Rose_STL_Container<std::string> arglist;
     int nextErrorCode = 0;

     bool set_header_file_unparsing_optimization = false;

  // DQ (11/10/2019): Shared nodes between existing files that are copied need to be marked as shared.
     bool isCopyOfExistingFile_testForSharedNodes = false;
     SgFile* fileBeingCopied = NULL;

     if (project == NULL)
      // SgProject is created on the fly
      // Make up an arglist in order to reuse the code inside SgFile::setupSourceFilename()
        {
#if 0
          printf ("In SageBuilder::buildFile(): build the SgProject \n");
#endif
          project = new SgProject();
          ROSE_ASSERT(project);
          project->get_fileList().clear();

          arglist.push_back("cc");
          arglist.push_back("-c");
          project->set_originalCommandLineArgumentList (arglist);
        }
       else
        {
       // If project exists, then find the original source file if it exists and check the header file optimization setting for consistancy.

       // DQ (9/18/2019): Adding debugging support to header file optimization support.
          SgFilePtrList & files = project->get_fileList();
          for (SgFilePtrList::iterator i = files.begin(); i != files.end(); i++)
             {
               SgFile* file = *i;
#if 0
               printf ("file = %p = %s name = %s \n",file,file->class_name().c_str(), file->getFileName().c_str());

               printf ("file->get_header_file_unparsing_optimization() = %s \n",file->get_header_file_unparsing_optimization() ? "true" : "false");
               printf ("file->get_header_file_unparsing_optimization_source_file() = %s \n",file->get_header_file_unparsing_optimization_source_file() ? "true" : "false");
               printf ("file->get_header_file_unparsing_optimization_header_file() = %s \n",file->get_header_file_unparsing_optimization_header_file() ? "true" : "false");
#endif
               if (sourceFilename == file->getFileName())
                  {
#if 0
                    printf ("This is a copy of an existing file in the project: sourceFilename = %s \n",sourceFilename.c_str());
#endif
                 // DQ (11/10/2019): Shared nodes between existing files that are copied need to be marked as shared.
                    isCopyOfExistingFile_testForSharedNodes = true;
                    fileBeingCopied = file;

                 // We are building a second copy of an originally specified file (so we need to set the optimization setting similarly).
                    if (file->get_header_file_unparsing_optimization() == true)
                       {
                         set_header_file_unparsing_optimization = true;
                       }
                  }
             }

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     ifstream testfile(inputFileName.c_str());
     if (!testfile.is_open())
        {
       // create a temporary file if the file does not exist.
       // have to do this, otherwise StringUtility::getAbsolutePathFromRelativePath() complains
       // which is called by result->setupSourceFilename(arglist);
          testfile.close();
          ofstream outputfile(inputFileName.c_str(),ios::out);
       // DQ (2/6/2009): I think this comment is helpful to put into the file (helps explain why the file exists).
          outputfile<<"// Output file generated so that StringUtility::getAbsolutePathFromRelativePath() will see a vaild file ... unparsed file will have rose_ prefix "<<endl;
          outputfile.close();
        }
       else // file already exists , load and parse it
        {
       // should not reparse all files in case their ASTs have unsaved changes,
       // just parse the newly loaded file only.
       // use argv here, change non-existing input file later on
       // TODO add error code handling

       // DQ (2/6/2009): Avoid closing this file twice (so put this here, instead of below).
          testfile.close();
          // should remove the old one here, Liao, 5/1/2009
        }

  // DQ (2/6/2009): Avoid closing this file twice (moved to false branch above).
  // testfile.close();

  // DQ (2/6/2009): Need to add the inputFileName to the source file list in the project,
  // because this list will be used to subtract off the source files as required to build
  // the commandline for the backend compiler.
     project->get_sourceFileNameList().push_back(inputFileName);

     Rose_STL_Container<string> sourceFilenames = project->get_sourceFileNameList();
  // printf ("In SageBuilder::buildFile(): sourceFilenames.size() = %" PRIuPTR " sourceFilenames = %s \n",sourceFilenames.size(),StringUtility::listToString(sourceFilenames).c_str());

     arglist = project->get_originalCommandLineArgumentList();

  // DQ (2/6/2009): We will be compiling the source code generated in the
  // "rose_<inputFileName>" file, so we don't want this on the argument stack.
  // TV (09/19/2018): only add if not already present
     if (std::find(arglist.begin(), arglist.end(), sourceFilename) == arglist.end()) 
        {
          arglist.push_back(sourceFilename);
        }

  // DQ (2/6/2009): Modified.
  // There is output file name specified for rose translators
     if (outputFileName.empty() == false)
        {
          arglist.push_back("-rose:o");
       // arglist.push_back("-o");
          arglist.push_back(outputFileName);
        }

  // DQ (4/15/2010): Turn on verbose mode
  // arglist.push_back("-rose:verbose 2");

  // This handles the case where the original command line may have referenced multiple files.
     Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(arglist,/* binaryMode = */ false);
     CommandlineProcessing::removeAllFileNamesExcept(arglist,fileList,sourceFilename);

  // DQ (9/3/2008): Added support for SgSourceFile IR node
  // SgFile* result = new SgFile (arglist, nextErrorCode, 0, project);
  // AS(10/04/08) Because of refactoring we require the determineFileType function to be called
  // to construct the node.
  // SgSourceFile* result = new SgSourceFile (arglist, nextErrorCode, 0, project);
  // SgSourceFile* result = isSgSourceFile(determineFileType(arglist, nextErrorCode, project));
  // TH (2009-07-15): changed to more generig isSgFile, this also supports SgBinaryComposite
     SgFile* result = determineFileType(arglist, nextErrorCode, project);
     ROSE_ASSERT(result != NULL);

#if 0
     printf ("In SageBuilder::buildFile(): project = %p project->get_fileList_ptr()->get_listOfFiles().size() = %" PRIuPTR " \n",project,project->get_fileList_ptr()->get_listOfFiles().size());
#endif

#if 0
     printf ("Calling outputFileIds() \n");

     SageInterface::outputFileIds(result);

     printf ("DONE: Calling outputFileIds() \n");
#endif

#if 0
  // DQ (9/18/2019): Adding debugging support.
     printf ("In SageBuilder::buildFile(): file = %p = %s result->get_header_file_unparsing_optimization() = %s \n",
          result,result->class_name().c_str(),result->get_header_file_unparsing_optimization() ? "true" : "false");
     printf ("In SageBuilder::buildFile(): file = %p = %s result->get_header_file_unparsing_optimization_source_file() = %s \n",
          result,result->class_name().c_str(),result->get_header_file_unparsing_optimization_source_file() ? "true" : "false");
     printf ("In SageBuilder::buildFile(): file = %p = %s result->get_header_file_unparsing_optimization_header_file() = %s \n",
          result,result->class_name().c_str(),result->get_header_file_unparsing_optimization_header_file() ? "true" : "false");
#endif

  // DQ (9/18/2019): Adding debugging support.
     ROSE_ASSERT(result->get_header_file_unparsing_optimization() == false);
     ROSE_ASSERT(result->get_header_file_unparsing_optimization_source_file() == false);
     ROSE_ASSERT(result->get_header_file_unparsing_optimization_header_file() == false);

  // ROSE_ASSERT(result->get_header_file_unparsing_optimization() == true);

     if (set_header_file_unparsing_optimization == true)
        {
          result->set_header_file_unparsing_optimization(true);

       // DQ (9/18/2019): Also set the values for the source file and header files.
       // I think we only want to set the source file version to true and the header file version to false.
       // This is enforced in the attachPreprocessingInfo() function.
          result->set_header_file_unparsing_optimization_source_file(true);
       // result->set_header_file_unparsing_optimization_header_file(true);
          result->set_header_file_unparsing_optimization_header_file(false);

#if 0
          printf ("In SageBuilder::buildFile(): set_header_file_unparsing_optimization == true: file = %p = %s result->get_header_file_unparsing_optimization() = %s \n",
               result,result->class_name().c_str(),result->get_header_file_unparsing_optimization() ? "true" : "false");
          printf ("In SageBuilder::buildFile(): set_header_file_unparsing_optimization == true: file = %p = %s result->get_header_file_unparsing_optimization_source_file() = %s \n",
               result,result->class_name().c_str(),result->get_header_file_unparsing_optimization_source_file() ? "true" : "false");
          printf ("In SageBuilder::buildFile(): set_header_file_unparsing_optimization == true: file = %p = %s result->get_header_file_unparsing_optimization_header_file() = %s \n",
               result,result->class_name().c_str(),result->get_header_file_unparsing_optimization_header_file() ? "true" : "false");
#endif
        }

#if 0
  // DQ (3/4/2014): This fix is only for Java and for C will cause a second SgFile to be redundently added to the file list.
  // For now I will provide a temporary fix and check is this is for a Java project so that we can continue. But the longer
  // term fix would be to make the semantics for Java the same as that of C/C++ (or the other way around, whatever is the
  // cleaner semantics.
  // This just adds the new file to the list of files stored internally (note: this sets the parent of the newFile).
  // TOO1 (2/28/2014): This is definitely required for Java (ECJ frontend), though C passes without it (I think only
  //                   by luck :-).
  //                   The ECJ frontend uses the SgProject internally (via a global SgProject*). Therefore, the
  //                   SgProject must contain this newly created SgFile, otherwise ECJ won't be able to find it.
  // project->set_file ( *result );
     if (project->get_Java_only() == true)
        {
       // DQ (3/4/2014): For now we want to output a message and clean this up afterward (likely in the Java language support).
          printf ("WARNING: Java specific action to add new file to SgProject (using set_file()) (more uniform language handling symantics would avoid this problem) \n");
          project->set_file ( *result );
        }
#else
  // DQ (3/6/2014): The code below adresses the specific bug faced in the use of the outliner (so we use it directly).
  // This code was moved ahead of the call to "result->runFrontend(nextErrorCode);" because in the case of Java
  // the file must be set to be a part of the SgProject before calling the runFrontend() function.
  // project->set_file ( *result );

     result->set_parent(project);

#if 0
     printf ("In SageBuilder::buildFile(): Outliner::use_dlopen = %s \n",Outliner::use_dlopen ? "true" : "false");
#endif

  // DQ (3/5/2014): I need to check with Liao to understand this part of the code better.
  // I think that the default value for Outliner::use_dlopen is false, so that when the
  // Java support is used the true branch is taken.  However, if might be the we need
  // to support the outliner using the code below and so this would be a bug for the
  // outliner.
     if (!Outliner::use_dlopen)
        {
#if 0
          printf ("In SageBuilder::buildFile(): (after test for (!Outliner::use_dlopen) == true: project = %p project->get_fileList_ptr()->get_listOfFiles().size() = %" PRIuPTR " \n",project,project->get_fileList_ptr()->get_listOfFiles().size());
#endif
       // DQ (3/5/2014): If we added the file above, then don't add it here since it is redundant.
          project->set_file(*result);  // equal to push_back()
#if 0
          printf ("In SageBuilder::buildFile(): (after 2nd project->set_file()): project = %p project->get_fileList_ptr()->get_listOfFiles().size() = %" PRIuPTR " \n",project,project->get_fileList_ptr()->get_listOfFiles().size());
#endif
        }
       else
        {
#if 0
          printf ("In SageBuilder::buildFile(): (after test for (!Outliner::use_dlopen) == false: project = %p project->get_fileList_ptr()->get_listOfFiles().size() = %" PRIuPTR " \n",project,project->get_fileList_ptr()->get_listOfFiles().size());
#endif

       // Liao, 5/1/2009,
       // if the original command line is: gcc -c -o my.o my.c and we want to
       // add a new file(mynew.c), the command line for the new file would become "gcc -c -o my.o mynew.c "
       // which overwrites the object file my.o from my.c and causes linking error.
       // To avoid this problem, I insert the file at the beginning and let the right object file to be the last generated one
       //
       // TODO This is not an elegant fix and it causes some strange assertion failure in addAssociatedNodes(): default case node
       // So we only turn this on if Outliner:: use_dlopen is used for now
       // The semantics of adding a new source file can cause changes to linking phase (new object files etc.)
       // But ROSE has a long-time bug in handling combined compiling and linking command like "translator -o a.out a.c b.c"
       // It will generated two command line: "translator -o a.out a.c" and "translator -o a.out b.c", which are totally wrong.
       // This problem is very relevant to the bug.
          SgFilePtrList& flist = project->get_fileList();
          flist.insert(flist.begin(),result);
#if 0
          printf ("In SageBuilder::buildFile(): (after flist.insert(flist.begin(),result)): project = %p project->get_fileList_ptr()->get_listOfFiles().size() = %" PRIuPTR " \n",project,project->get_fileList_ptr()->get_listOfFiles().size());
#endif
        }
#endif

  // DQ (6/3/2019): The case of outlining to a seperate file will have transformations
  // that this checking will fail on because it is for the typical case of checking the
  // AST for transformations after construction of the AST from an typical input file.
     EDG_ROSE_Translation::suppress_detection_of_transformations = true;

#if 0
     printf ("In SageBuilder::buildFile(): EDG_ROSE_Translation::suppress_detection_of_transformations = %s \n",EDG_ROSE_Translation::suppress_detection_of_transformations ? "true" : "false");
#endif

#if 0
     printf ("In SageBuilder::buildFile(): (after project->set_file()): project = %p project->get_fileList_ptr()->get_listOfFiles().size() = %" PRIuPTR " \n",project,project->get_fileList_ptr()->get_listOfFiles().size());
#endif


  // DQ (6/5/2019): Record what is marked as isModified() and then reset these IR nodes to be isModified after the new file has been processed.
  // This is required because the modified IR nodes will be reset in this AST associated with the new file, and IR nodes that are common
  // across the two AST's will be reset (shared IR nodes, which are also not marked as shared).  The solution is to compute the list of IR nodes
  // which are marked as isModified, and then build the new file (which will reset them for the new file's AST (plus any shared nodes visited in
  // the traversal) and then afterward reset the set of isModified IR nodes to isModified.  By isolating the fix in this function we can eliminate
  // the complexity of it being seen from the outside (outside of this abstraction).  Note that the function:
  // SageInterface::collectModifiedLocatedNodes() has previously been implemented and used for debugging.
     std::set<SgLocatedNode*> modifiedNodeSet = collectModifiedLocatedNodes(project);

  // DQ (3/6/2014): For Java, this function can only be called AFTER the SgFile has been added to the file list in the SgProject.
  // For C/C++ it does not appear to matter if the call is made before the SgFile has been added to the file list in the SgProject.
  // DQ (6/14/2013): Since we seperated the construction of the SgFile IR nodes from the invocation of the frontend, we have to call the frontend explicitly.
     result->runFrontend(nextErrorCode);

#if 0
     printf ("In SageBuilder::buildFile(): (after result->runFrontend()): project = %p project->get_fileList_ptr()->get_listOfFiles().size() = %" PRIuPTR " \n",project,project->get_fileList_ptr()->get_listOfFiles().size());
#endif

#if 0
     printf ("After result->runFrontend(): calling outputFileIds() \n");

     SageInterface::outputFileIds(result);

     printf ("DONE: After result->runFrontend(): calling outputFileIds() \n");
#endif

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (SgFile only) \n");
     generateDOT ( *project );
  // generateAstGraph(project, 2000);
#endif

#if 0
     printf ("In SageBuilder::buildFile(): Generate the dot output for multiple files (ROSE AST) \n");
  // generateDOT ( *project );
     generateDOTforMultipleFile ( *project );
     printf ("DONE: Generate the dot output of the SAGE III AST \n");
#endif

#if 0
  // DQ (7/18/2019): Output a graph of the AST for debugging.
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

  // DQ (7/14/2019): I think we need to call the astPostProcessing at this point.
#if 0
     printf ("In SageBuilder::buildFile(): calling astPostProcessing() \n");
#endif

     AstPostProcessing(result);

#if 0
     printf ("In SageBuilder::buildFile(): DONE: calling astPostProcessing() \n");
#endif

#if 0
     result->display("SageBuilder::buildFile()");
#endif

     ROSE_ASSERT(project != NULL);
     project->set_frontendErrorCode(max(project->get_frontendErrorCode(), nextErrorCode));

  // Not sure why a warning shows up from astPostProcessing.C
  // SgNode::get_globalMangledNameMap().size() != 0 size = %" PRIuPTR " (clearing mangled name cache)
     if (result->get_globalMangledNameMap().size() != 0)
        {
          result->clearGlobalMangledNameMap();
        }

  // DQ (6/5/2019): Use the previously constructed set (above) to reset the IR nodes to be marked as isModified.
  // std::set<SgLocatedNode*> modifiedNodeSet = collectModifiedLocatedNodes(project);
  // void resetModifiedLocatedNodes(const std::set<SgLocatedNode*> & modifiedNodeSet);
     resetModifiedLocatedNodes(modifiedNodeSet);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // DQ (11/10/2019): Shared nodes between existing files that are copied need to be marked as shared.
     if (isCopyOfExistingFile_testForSharedNodes == true)
        {
       // Sharing of IR nodes happens in the AST when the same file is read twice.
       // Also in the case where two declarations in the global scope match in two different ASTs (typically in header files of different translation units).

#if 0
          printf ("Found isCopyOfExistingFile_testForSharedNodes == true \n");
          printf ("fileBeingCopied = %p = %s \n",fileBeingCopied,fileBeingCopied->getFileName().c_str());
#endif

          SgSourceFile* sourceFileBeingCopied = isSgSourceFile(fileBeingCopied);
          ROSE_ASSERT(sourceFileBeingCopied != NULL);

          SgSourceFile* sourceResult = isSgSourceFile(result);
          ROSE_ASSERT(sourceResult != NULL);

          SgGlobal* fileBeingCopied_globalScope = sourceFileBeingCopied->get_globalScope();
          SgGlobal* result_globalScope          = sourceResult->get_globalScope();
#if 0
          printf ("fileBeingCopied_globalScope = %p \n",fileBeingCopied_globalScope);
          printf ("result_globalScope          = %p \n",result_globalScope);
#endif
          ROSE_ASSERT(fileBeingCopied_globalScope != NULL);
          ROSE_ASSERT(result_globalScope != NULL);

          SgDeclarationStatementPtrList fileBeingCopied_declarationList = fileBeingCopied_globalScope->get_declarations();
          SgDeclarationStatementPtrList result_declarationList          = result_globalScope->get_declarations();

#if 1
       // DQ (11/22/2019): Use set intersection to compute the list to make be shared (this is a better implementation).
       // This implementation is insensitive to transforamtions in the original AST for the file.
          vector<SgDeclarationStatement*>::iterator it;
          SgDeclarationStatementPtrList v(fileBeingCopied_declarationList.size());

       // This is n log n in complexity, but likely OK.
          std::sort(fileBeingCopied_declarationList.begin(),fileBeingCopied_declarationList.end());
          std::sort(result_declarationList.begin(),result_declarationList.end());

       // printf ("v.size() = %zu \n",v.size());

          it = std::set_intersection(fileBeingCopied_declarationList.begin(),fileBeingCopied_declarationList.end(),result_declarationList.begin(),result_declarationList.end(),v.begin());

          v.resize(it-v.begin());

          int fileBeingCopied_file_id = fileBeingCopied->get_startOfConstruct()->get_physical_file_id();

       // printf ("v.size() = %zu \n",v.size());
          for (size_t i = 0; i < v.size(); i++)
             {
               SgDeclarationStatement* intersection_element = v[i];
            // printf ("intersection_element = %p = %s \n",intersection_element,intersection_element->class_name().c_str());
#if 0
               printf ("  --- SageBuilder::buildFile() is sharing this node: %p %s \n",intersection_element,intersection_element->class_name().c_str());
#endif
            // DQ (11/10/2019): Need to recursively mark this as shared so that the unparser will be able to test each line?

               fixupSharingSourcePosition(intersection_element,fileBeingCopied_file_id);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
#else

#error "DEAD CODE!"

       // This is the older implementation that is sensitive to transforamtions in the original AST from the file.
       // DQ (11/21/2019): Remove elements in the vector that are SgEmptyDeclarations which 
       // are associated with some transformations (include header, for example).
          std::vector<SgDeclarationStatementPtrList::iterator> removeList;
          SgDeclarationStatementPtrList::iterator i = fileBeingCopied_declarationList.begin();
          while (i != fileBeingCopied_declarationList.end())
             {
               SgEmptyDeclaration* emptyDeclaration = isSgEmptyDeclaration(*i);
               if (emptyDeclaration != NULL)
                  {
                    removeList.push_back(i);
                  }

               i++;
             }

#error "DEAD CODE!"

       // Need seperate list to avoid iterator invalidation.
       // for (SgDeclarationStatementPtrList::iterator i = removeList.begin(); i != removeList.end(); i++)
          for (std::vector<SgDeclarationStatementPtrList::iterator>::iterator i = removeList.begin(); i != removeList.end(); i++)
             {
               fileBeingCopied_declarationList.erase(*i);
             }

       // DQ (11/21/2019): These might be a different size if for example the file being 
       // copied is being copied after some transformations to the AST from the original file.
          if (fileBeingCopied_declarationList.size() != result_declarationList.size())
             {
               printf ("fileBeingCopied_declarationList.size() = %zu \n",fileBeingCopied_declarationList.size());
               printf ("result_declarationList.size() = %zu \n",result_declarationList.size());
             }
          ROSE_ASSERT(fileBeingCopied_declarationList.size() == result_declarationList.size());

#error "DEAD CODE!"

#if 0
          printf ("Statements from global scope (size = %zu): \n",fileBeingCopied_declarationList.size());
#endif
          for (size_t i = 0; i < fileBeingCopied_declarationList.size(); i++)
             {
               SgDeclarationStatement* fileBeingCopied_decl = fileBeingCopied_declarationList[i];
               SgDeclarationStatement* result_decl          = result_declarationList[i];
#if 0
               printf ("  #%zu global scope entry: fileBeingCopied: %p %s result %p %s \n",i,fileBeingCopied_decl,fileBeingCopied_decl->class_name().c_str(),result_decl,result_decl->class_name().c_str());
#endif
               if (fileBeingCopied_decl == result_decl)
                  {
#if 0
                    printf ("  --- SageBuilder::buildFile() is sharing this node: %p %s \n",fileBeingCopied_decl,fileBeingCopied_decl->class_name().c_str());
#endif
                 // DQ (11/10/2019): Need to recursively mark this as shared so that the unparser will be able to test each line?

#error "DEAD CODE!"

                    int fileBeingCopied_file_id = fileBeingCopied->get_startOfConstruct()->get_physical_file_id();
                    fixupSharingSourcePosition(fileBeingCopied_decl,fileBeingCopied_file_id);
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }

#error "DEAD CODE!"

#endif

#if 0
          printf ("exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
     reportModifiedStatements("Leaving SageBuilder::buildFile(): calling reportModifiedStatements()",project);
#endif

#if 0
     printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
     printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
     printf ("Leaving SageBuilder::buildFile(): (after result->runFrontend()): project = %p project->get_fileList_ptr()->get_listOfFiles().size() = %" PRIuPTR " \n",
          project,project->get_fileList_ptr()->get_listOfFiles().size());
     printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
     printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif

#if 0
  // DQ (11/8/2019): This is not working and breaks the current work at present.
  // DQ (11/8/2019): Support function to change the name in each of the IR node's source position info objects.
     fixupSourcePositionFileSpecification(result,outputFileName);
#endif

     return result;
#else

  // false branch of #ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT (at top of function.

     return NULL;
#endif
   }


//! Build a SgFile node
SgSourceFile*
SageBuilder::buildSourceFile(const std::string& outputFileName, SgProject* project)
   {
  // DQ (2/9/2013): Adding support to build a SgSourceFile with an empty global scope.
  // This function calls the buildFile(string,string,SgProject*) function and provides
  // a simple API where one wants to create a new SgSourceFile that will then have
  // statements added to it and then unparsed.

  // This function needs a way to specify the associated language for the generated file.
  // Currently this is taken from the input file (generated from a prefix on the output filename.

#if 0
     printf ("In SageBuilder::buildSourceFile(outputFileName = %s, project = %p \n",outputFileName.c_str(),project);
#endif

  // Call the supporting function to build a file.
     string inputFilePrefix = "temp_dummy_file_";

#if 0
     printf ("In SageBuilder::buildSourceFile(const std::string& outputFileName, SgProject* project): calling buildFile() \n");
#endif

     SgFile* file = buildFile(inputFilePrefix+outputFileName,outputFileName,project);
     ROSE_ASSERT(file != NULL);

#if 0
     printf ("DONE: In SageBuilder::buildSourceFile(): calling buildFile() \n");
#endif

     SgSourceFile* sourceFile = isSgSourceFile(file);
     ROSE_ASSERT(sourceFile != NULL);

     ROSE_ASSERT(sourceFile->get_globalScope() != NULL);

#if 0
     printf ("call the unparser on the just built file \n");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     return sourceFile;

   }

SgSourceFile* SageBuilder::buildSourceFile(const std::string& inputFileName,const std::string& outputFileName, SgProject* project)
   {
#if 0
     printf ("In SageBuilder::buildSourceFile(const std::string& inputFileName,const std::string& outputFileName, SgProject* project): calling buildFile() \n");
  // printf (" --- inputFileName  = %s outputFileName = %s \n",inputFileName.c_str(),outputFileName.c_str());
     printf (" --- inputFileName  = %s \n",inputFileName.c_str());
     printf (" --- outputFileName = %s \n",outputFileName.c_str());
#endif

     SgFile* file = buildFile(inputFileName, outputFileName,project);
     ROSE_ASSERT(file != NULL);

#if 0
     printf ("DONE: In SageBuilder::buildSourceFile(): calling buildFile() \n");
#endif

     SgSourceFile* sourceFile = isSgSourceFile(file);
     ROSE_ASSERT(sourceFile != NULL);

     ROSE_ASSERT(sourceFile->get_globalScope() != NULL);

#if 0
  // DQ (9/18/2019): Adding support for debugging the header file optimization.
     printf ("Debugging the unparsing header file optimization \n");

     printf ("sourceFile->get_header_file_unparsing_optimization()             = %s \n",sourceFile->get_header_file_unparsing_optimization() ? "true" : "false");
     printf ("sourceFile->get_header_file_unparsing_optimization_source_file() = %s \n",sourceFile->get_header_file_unparsing_optimization_source_file() ? "true" : "false");
     printf ("sourceFile->get_header_file_unparsing_optimization_header_file() = %s \n",sourceFile->get_header_file_unparsing_optimization_header_file() ? "true" : "false");
#endif

#if 0
  // ROSE_ASSERT(sourceFile->get_header_file_unparsing_optimization_header_file() == false);

  // DQ (9/18/2019): These are now set to true when the inputFileName matches a previously read file for which the optimizaton was turned on.
     ROSE_ASSERT(sourceFile->get_header_file_unparsing_optimization() == true);
     ROSE_ASSERT(sourceFile->get_header_file_unparsing_optimization_source_file() == true);
  // ROSE_ASSERT(sourceFile->get_header_file_unparsing_optimization_header_file() == true);
     ROSE_ASSERT(sourceFile->get_header_file_unparsing_optimization_header_file() == false);
#endif

  // DQ (9/18/2019): Adding support for the header file optimization.
  // Check is this file matches an existing file and if so avoid regathering the CPP directives and comments (if posible).
  // If the original file was specified as being optimized for unparsing header files, then make this one similarly.
     SgFilePtrList & fileList = project->get_fileList();

#if 0
     printf ("Looking for file = %s \n",inputFileName.c_str());
#endif

     for (SgFilePtrList::iterator i = fileList.begin(); i != fileList.end(); i++)
        {
          SgFile* temp_file = *i;
#if 0
          printf ("temp_file = %p = %s name = %s \n",temp_file,temp_file->class_name().c_str(),temp_file->getFileName().c_str());
#endif
          if (temp_file != file)
             {
               if (temp_file->getFileName() == file->getFileName())
                  {
                 // Then the temp_file is the original version of the file we are building for a second time 
                 // (usually as a part of the outlining to a seperate file).  and we need to mark at least the 
                 // unparsing headr file optimizations to be the same across thje two file.

                    temp_file->set_header_file_unparsing_optimization(sourceFile->get_header_file_unparsing_optimization());
                    temp_file->set_header_file_unparsing_optimization_source_file(sourceFile->get_header_file_unparsing_optimization_source_file());
                    temp_file->set_header_file_unparsing_optimization_header_file(sourceFile->get_header_file_unparsing_optimization_header_file());
#if 0
                    printf ("sourceFile = %p = %s \n",sourceFile,sourceFile->class_name().c_str());
                    printf ("sourceFile->get_header_file_unparsing_optimization()             = %s \n",sourceFile->get_header_file_unparsing_optimization() ? "true" : "false");
                    printf ("sourceFile->get_header_file_unparsing_optimization_source_file() = %s \n",sourceFile->get_header_file_unparsing_optimization_source_file() ? "true" : "false");
                    printf ("sourceFile->get_header_file_unparsing_optimization_header_file() = %s \n",sourceFile->get_header_file_unparsing_optimization_header_file() ? "true" : "false");

                    printf ("temp_file = %p = %s \n",temp_file,temp_file->class_name().c_str());
                    printf ("temp_file->get_header_file_unparsing_optimization()             = %s \n",temp_file->get_header_file_unparsing_optimization() ? "true" : "false");
                    printf ("temp_file->get_header_file_unparsing_optimization_source_file() = %s \n",temp_file->get_header_file_unparsing_optimization_source_file() ? "true" : "false");
                    printf ("temp_file->get_header_file_unparsing_optimization_header_file() = %s \n",temp_file->get_header_file_unparsing_optimization_header_file() ? "true" : "false");
#endif
                  }
                 else
                  {
                 // This is a different file.
                  }
             }
          else
            {
           // This is the same file, already added to the SgProject file list (as it should be).
            }
        }


#if 0
     printf ("sourceFile->get_file_info()->get_filename() = %s \n",sourceFile->get_file_info()->get_filename());
     int filename_id          = Sg_File_Info::get_nametofileid_map()[sourceFile->get_file_info()->get_filename()];
     int filename_physical_id = Sg_File_Info::get_nametofileid_map()[sourceFile->get_file_info()->get_filename()];
     printf ("Sg_File_Info::get_nametofileid_map()[sourceFile->get_file_info()->get_filename()] = %d \n",filename_id);
     printf ("Sg_File_Info::get_nametofileid_map()[sourceFile->get_file_info()->get_filename()] = %d \n",filename_physical_id);
     sourceFile->get_file_info()->set_physical_file_id(filename_physical_id);

     printf ("sourceFile->get_file_info()->get_physical_filename() = %s \n",sourceFile->get_file_info()->get_physical_filename().c_str());
     printf ("sourceFile->get_file_info()->get_physical_file_id()  = %d \n",sourceFile->get_file_info()->get_physical_file_id());
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 1
  // DQ (11/4/2019): I need to add this when I went back to testing tool_G.
  // It is required in the functions to attach CPP directives and comments.
     if (sourceFile->get_preprocessorDirectivesAndCommentsList() == NULL)
        {
#if 0
          printf ("Initialize NULL p_preprocessorDirectivesAndCommentsList to empty ROSEAttributesListContainer \n");
#endif
          ROSEAttributesListContainer* tmp_preprocessorDirectivesAndCommentsList = new ROSEAttributesListContainer();
          sourceFile->set_preprocessorDirectivesAndCommentsList(tmp_preprocessorDirectivesAndCommentsList);
        }
       else
        {
#if 0
          printf ("NOTE: p_preprocessorDirectivesAndCommentsList is already defined! \n");
          printf (" --- inputFileName  = %s \n",inputFileName.c_str());
          printf (" --- outputFileName = %s \n",outputFileName.c_str());
          printf (" --- sourceFile->get_preprocessorDirectivesAndCommentsList()->getList().size() = %zu \n",sourceFile->get_preprocessorDirectivesAndCommentsList()->getList().size());
#endif
        }
     ROSE_ASSERT (sourceFile->get_preprocessorDirectivesAndCommentsList() != NULL);

  // DQ (11/4/2019): This is a test that is use in attaching CPP directives and comments to the AST.
     ROSEAttributesListContainerPtr filePreprocInfo = sourceFile->get_preprocessorDirectivesAndCommentsList();
     ROSE_ASSERT(filePreprocInfo != NULL);
#endif

#if 0
     printf ("In SageBuilder::buildSourceFile(const std::string& inputFileName,const std::string& outputFileName, SgProject* project): calling attachPreprocessingInfo() \n");
#endif

  // Liao, 2019, 1/31: We often need the preprocessing info. (e.g. #include ..) attached to make the new file compilable. 
     attachPreprocessingInfo (sourceFile);

#if 0
     printf ("DONE: In SageBuilder::buildSourceFile(const std::string& inputFileName,const std::string& outputFileName, SgProject* project): calling attachPreprocessingInfo() \n");
#endif

#if 0
     printf ("call the unparser on the just built file \n");
#endif

#if 0
     printf ("In buildSourceFile(): AS A TEST: calling unparseFile(): filename = %s \n",sourceFile->getFileName().c_str());
     backend(project);
#endif

#if 1
  // DQ (11/8/2019): This is not working and breaks the current work at present.
  // DQ (11/8/2019): Support function to change the name in each of the IR node's source position info objects.
     fixupSourcePositionFileSpecification(sourceFile,outputFileName);
#endif

#if 0
     printf ("Leaving SageBuilder::buildSourceFile() sourceFile = %p globalScope = %p \n",sourceFile,sourceFile->get_globalScope());
     printf ("sourceFile->get_file_info()->get_file_id()          = %d \n",sourceFile->get_file_info()->get_file_id());
     printf ("sourceFile->get_file_info()->get_physical_file_id() = %d \n",sourceFile->get_file_info()->get_physical_file_id());
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     return sourceFile;
   }


PreprocessingInfo* SageBuilder::buildComment(SgLocatedNode* target, const std::string & content,PreprocessingInfo::RelativePositionType position/*=PreprocessingInfo::before*/,PreprocessingInfo::DirectiveType dtype/* = PreprocessingInfo::CpreprocessorUnknownDeclaration*/)
   {
     return SageInterface::attachComment(target,content, position, dtype);
   }

//! #define xxx yyy
PreprocessingInfo* SageBuilder::buildCpreprocessorDefineDeclaration(SgLocatedNode* target,const std::string & content,PreprocessingInfo::RelativePositionType position /* =PreprocessingInfo::before*/)
  {
    ROSE_ASSERT(target != NULL); //dangling #define xxx is not allowed in the ROSE AST
    // simple input verification
    std::string content2 = content;
    boost::algorithm::trim(content2);
    string prefix = "#define";
    string::size_type pos = content2.find(prefix, 0);
    ROSE_ASSERT (pos == 0);

    PreprocessingInfo* result = NULL;

    PreprocessingInfo::DirectiveType mytype = PreprocessingInfo::CpreprocessorDefineDeclaration;

 // DQ (7/19/2008): Modified interface to PreprocessingInfo
 // result = new PreprocessingInfo (mytype,content, "transformation-generated", 0, 0, 0, position, false, true);
    result = new PreprocessingInfo (mytype,content, "transformation-generated", 0, 0, 0, position);
    ROSE_ASSERT(result);
    target->addToAttachedPreprocessingInfo(result);
    return result;

  }


#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
//! Build an abstract handle from a SgNode
AbstractHandle::abstract_handle * SageBuilder::buildAbstractHandle(SgNode* n)
{
  // avoid duplicated creation
  static std::map<SgNode*, AbstractHandle::abstract_handle *> handleMap;

  ROSE_ASSERT(n != NULL);
  AbstractHandle::abstract_handle * ahandle =handleMap[n];
  if (ahandle==NULL)
  {
    AbstractHandle::abstract_node* anode = AbstractHandle::buildroseNode(n);
    ROSE_ASSERT(anode !=NULL );
    ahandle = new AbstractHandle::abstract_handle(anode);
    //TODO do we allow NULL handle to be returned?
    ROSE_ASSERT(ahandle != NULL);
  }
  return ahandle;
}
#endif

SgEquivalenceStatement*
SageBuilder::buildEquivalenceStatement(SgExpression* exp1,SgExpression* exp2)
{
  ROSE_ASSERT(exp1 != NULL);
  ROSE_ASSERT(exp2 != NULL);

  SgExprListExp* tuple = buildExprListExp(exp1,exp2);
  SgExprListExp* setList = buildExprListExp(tuple);
  SgEquivalenceStatement* equivalenceStatement = new SgEquivalenceStatement();
  ROSE_ASSERT(equivalenceStatement->get_equivalence_set_list() == NULL);
  equivalenceStatement->set_equivalence_set_list(setList);
  ROSE_ASSERT(equivalenceStatement->get_equivalence_set_list() != NULL);
  equivalenceStatement->set_firstNondefiningDeclaration(equivalenceStatement);
  setOneSourcePositionForTransformation(equivalenceStatement);
  return equivalenceStatement;
}

SgSymbol*
SageBuilder::findAssociatedSymbolInTargetAST(SgDeclarationStatement* snippet_declaration, SgScopeStatement* targetScope)
   {
  // Starting at the snippet_declaration, record the associated scope list to the global scope.
  // The do a reverse traversal on the list starting with the global scope of the target AST.
  // Lookup each declaration as we proceed deeper into the target AST to find the associated
  // symbol in the target AST (associated with the input declaration from the snippet AST).

     SgSymbol* returnSymbol = NULL;

     typedef Rose_STL_Container<SgScopeStatement*>  SgScopeStatementPtrList;
     SgScopeStatementPtrList snippet_scope_list;

  // Starting at the snippet_declaration, record the associated scope list to the global scope.
  // SgScopeStatement* snippet_scope = snippet_declaration->get_scope();
     SgScopeStatement* snippet_scope = snippet_declaration->get_scope();
#if 0
     printf ("First scope = %p = %s \n",snippet_scope,snippet_scope->class_name().c_str());
     SgClassDefinition* temp_classDefinition = isSgClassDefinition(snippet_scope);
     if (temp_classDefinition != NULL)
        {
          SgClassDeclaration* temp_classDeclaration = temp_classDefinition->get_declaration();
          SgName className = temp_classDeclaration->get_name();
#if 0
          printf ("Input snippet declaration's class name = %s \n",className.str());
#endif
        }
#endif
     snippet_scope_list.push_back(snippet_scope);
     while (snippet_scope != NULL && isSgGlobal(snippet_scope) == NULL)
        {
       // The scopes between the snippet declaration and the global scope should be named scopes,
       // else we will not be able to identify the associated scope in the target AST.
          ROSE_ASSERT(snippet_scope->isNamedScope() == true);

          snippet_scope = snippet_scope->get_scope();
#if 0
          printf ("snippet_scope = %p = %s \n",snippet_scope,snippet_scope->class_name().c_str());
#endif
          snippet_scope_list.push_back(snippet_scope);
        }

#if 0
     printf ("snippet_scope_list.size() = %" PRIuPTR " \n",snippet_scope_list.size());
#endif

     SgGlobal* global_scope_in_target_ast = TransformationSupport::getGlobalScope(targetScope);
     SgScopeStatementPtrList::reverse_iterator i = snippet_scope_list.rbegin();

     SgScopeStatement* target_AST_scope  = global_scope_in_target_ast;
     SgScopeStatement* snippet_AST_scope = *i;

     ROSE_ASSERT(isSgGlobal(snippet_AST_scope) != NULL);
  // Iterate past the global scope
     i++;

  // Traverse the snippet scopes in the reverse order from global scope to the associated scope in the target AST.
     while (i != snippet_scope_list.rend())
        {
       // This loop has to handle different types of names scopes (for C this only means structs, I think).
#if 0
          printf ("snippet_AST_scope list *i = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
       // printf ("target_AST_scope = %p = %s \n",target_AST_scope,target_AST_scope ->class_name().c_str());
       // printf ("snippet_AST_scope = %p = %s \n",snippet_AST_scope,snippet_AST_scope ->class_name().c_str());

          SgClassDefinition* classDefinition = isSgClassDefinition(*i);
          if (classDefinition != NULL)
             {
               SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
               SgName className = classDeclaration->get_name();
#if 0
               printf ("Found snippet class name = %s \n",className.str());
#endif
               SgClassSymbol* classSymbol = target_AST_scope->lookup_class_symbol(className);
               ROSE_ASSERT(classSymbol != NULL);
               ROSE_ASSERT(classSymbol->get_declaration() != NULL);
#if 0
               printf ("Associated symbol in taget AST: declaration = %p name = %s \n",classSymbol->get_declaration(),classSymbol->get_declaration()->get_name().str());
#endif
            // Set the return value as we go so that it will be properly set at the end of the reverse iteration over the scopes.
               returnSymbol = classSymbol;

            // Reset the target AST scope (as we traverse down the AST to the associated declaration in the target AST).
               target_AST_scope = classDefinition;
             }

       // Not clear if we can have this case for C.
          SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(*i);
          if (functionDefinition != NULL)
             {
               printf ("ERROR: Found an unusual case of SgFunctionDefinition in list of scopes holding a declaration for a type \n");
               ROSE_ASSERT(false);
             }

       // Increment the reverse iterator.
          i++;
        }

  // Handle the different cases using a switch (there are only a few cases).
     switch (snippet_declaration->variantT())
        {
          case V_SgClassDeclaration:
             {
               SgClassDeclaration* snippet_classDeclaration = isSgClassDeclaration(snippet_declaration);
               ROSE_ASSERT(snippet_classDeclaration != NULL);

               SgName snippet_className = snippet_classDeclaration->get_name();
#if 0
               printf ("snippet snippet declaration's class name = %s \n",snippet_className.str());
#endif
               SgClassSymbol* target_symbol = target_AST_scope->lookup_class_symbol(snippet_className);
               ROSE_ASSERT(target_symbol != NULL);
               returnSymbol = target_symbol;

               SgClassSymbol* classSymbolInTargetAST = isSgClassSymbol(returnSymbol);
               ROSE_ASSERT(classSymbolInTargetAST != NULL);
               SgClassDeclaration* target_classDeclaration = isSgClassDeclaration(classSymbolInTargetAST->get_declaration());
               ROSE_ASSERT(target_classDeclaration != NULL);
#if 0
               printf ("snippet: classDeclaration = %p = %s \n",snippet_classDeclaration,snippet_classDeclaration->get_name().str());
               printf ("target: classDeclaration  = %p = %s \n",target_classDeclaration,target_classDeclaration->get_name().str());
#endif
               ROSE_ASSERT(snippet_classDeclaration->get_name() == target_classDeclaration->get_name());
               break;
             }

          case V_SgTypedefDeclaration:
             {
               SgTypedefDeclaration* snippet_typedefDeclaration = isSgTypedefDeclaration(snippet_declaration);
               ROSE_ASSERT(snippet_typedefDeclaration != NULL);

               SgName snippet_typedefName = snippet_typedefDeclaration->get_name();
#if 0
               printf ("snippet snippet declaration's typedef name = %s \n",snippet_typedefName.str());
#endif
               SgTypedefSymbol* target_symbol = target_AST_scope->lookup_typedef_symbol(snippet_typedefName);
               ROSE_ASSERT(target_symbol != NULL);
               returnSymbol = target_symbol;

               SgTypedefSymbol* typedefSymbolInTargetAST = isSgTypedefSymbol(returnSymbol);
               ROSE_ASSERT(typedefSymbolInTargetAST != NULL);
               SgTypedefDeclaration* target_typedefDeclaration = isSgTypedefDeclaration(typedefSymbolInTargetAST->get_declaration());
               ROSE_ASSERT(target_typedefDeclaration != NULL);
#if 0
               printf ("snippet: typedefDeclaration = %p = %s \n",snippet_typedefDeclaration,snippet_typedefDeclaration->get_name().str());
               printf ("target: typedefDeclaration  = %p = %s \n",target_typedefDeclaration,target_typedefDeclaration->get_name().str());
#endif
               ROSE_ASSERT(snippet_typedefDeclaration->get_name() == target_typedefDeclaration->get_name());
               break;
             }

          case V_SgEnumDeclaration:
             {
               SgEnumDeclaration* snippet_enumDeclaration = isSgEnumDeclaration(snippet_declaration);
               ROSE_ASSERT(snippet_enumDeclaration != NULL);

               SgName snippet_enumName = snippet_enumDeclaration->get_name();
#if 0
               printf ("snippet snippet declaration's enum name = %s \n",snippet_enumName.str());
#endif
            // DQ (4/13/2014): check if this is an un-named enum beclaration.
               bool isUnNamed = snippet_enumDeclaration->get_isUnNamed();
               if (isUnNamed == false)
                  {
                 // SgEnumSymbol* target_symbol = target_AST_scope->lookup_enum_symbol(snippet_enumName);
                    SgEnumSymbol* target_symbol = lookupEnumSymbolInParentScopes(snippet_enumName,target_AST_scope);
                    if (target_symbol == NULL)
                       {
                      // Debug this case.
                         SgScopeStatement* scope = snippet_enumDeclaration->get_scope();
                         printf ("scope = %p = %s \n",scope,scope->class_name().c_str());
                         scope->get_file_info()->display("case V_SgEnumDeclaration: target_symbol == NULL: scope: debug");
                       }
                    ROSE_ASSERT(target_symbol != NULL);
                    returnSymbol = target_symbol;

                    SgEnumSymbol* enumSymbolInTargetAST = isSgEnumSymbol(returnSymbol);
                    ROSE_ASSERT(enumSymbolInTargetAST != NULL);
                    SgEnumDeclaration* target_enumDeclaration = isSgEnumDeclaration(enumSymbolInTargetAST->get_declaration());
                    ROSE_ASSERT(target_enumDeclaration != NULL);
#if 0
                    printf ("snippet: enumDeclaration = %p = %s \n",snippet_enumDeclaration,snippet_enumDeclaration->get_name().str());
                    printf ("target: enumDeclaration  = %p = %s \n",target_enumDeclaration,target_enumDeclaration->get_name().str());
#endif
                    ROSE_ASSERT(snippet_enumDeclaration->get_name() == target_enumDeclaration->get_name());
                  }
                 else
                  {
                 // DQ (4/13/2014): I think we all agreed these would not have to be handled.
                    printf ("Warning: can't handle unnamed enum declarations \n");
                    ROSE_ASSERT(returnSymbol == NULL);
                  }
               break;
             }

          default:
             {
               printf ("Error: default reached in switch: snippet_declaration = %p = %s \n",snippet_declaration,snippet_declaration->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

  // return the last found symbol.
     return returnSymbol;
   }

SgType*
SageBuilder::getTargetFileTypeSupport(SgType* snippet_type, SgScopeStatement* targetScope)
   {
  // This is the inner function to getTargetFileType()
     SgType* returnType = NULL;

     ROSE_ASSERT(snippet_type != NULL);
     ROSE_ASSERT(targetScope != NULL);

  // DQ (3/17/2014): Refactored code.
  // See if the type might be asociated with the snippet file.
  // DQ (7/25/2014): Remove warning from GNU 4.8 compiler.
  // SgType* type_copy     = snippet_type;

#if 0
     SgType* type_copy     = snippet_type;
     printf ("(before type_copy->getInternalTypes()): type_copy = %p = %s \n",type_copy,type_copy->class_name().c_str());
#endif

     SgNamedType* namedType = isSgNamedType(snippet_type);
     if (namedType != NULL)
        {
       // Find the associated declaration and it's corresponding declaration in the target AST.
          SgDeclarationStatement* snippet_declaration = namedType->get_declaration();
          ROSE_ASSERT(snippet_declaration != NULL);
#if 0
          printf ("Need to find the declaration in the target AST that is associated with the snippet_declaration in the snippet AST \n");
          printf ("   --- snippet_declaration = %p = %s = %s \n",snippet_declaration,snippet_declaration->class_name().c_str(),SageInterface::get_name(snippet_declaration).c_str());
#endif
       // There are only a few cases here!
          switch (namedType->variantT())
             {
               case V_SgClassType:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(snippet_declaration);
                    if (classDeclaration != NULL)
                       {
                         SgClassSymbol* classSymbolInTargetAST = lookupClassSymbolInParentScopes(classDeclaration->get_name(),targetScope);
                         if (classSymbolInTargetAST == NULL)
                            {
                           // For Java or C++ this could be a name qualified type and so we need a better mechanism
                           // to identify it thorugh it's parent scopes. Build a list of parent scope back to the
                           // global scope and then traverse the list backwards to identify each scope in the target
                           // AST's global scope until we each the associated declaration in the target AST.
#if 0
                              printf ("This is likely a name qualified scope (which can't be seen in a simple traversal of the parent scope (case of C++ or Java) \n");
                              printf ("   --- Looking for target AST match for class name = %s \n",classDeclaration->get_name().str());
#endif
                              SgSymbol* symbol = findAssociatedSymbolInTargetAST(classDeclaration,targetScope);
                              ROSE_ASSERT(symbol != NULL);

                              classSymbolInTargetAST = isSgClassSymbol(symbol);
                            }

                         ROSE_ASSERT(classSymbolInTargetAST != NULL);
                         SgClassDeclaration* target_classDeclaration = isSgClassDeclaration(classSymbolInTargetAST->get_declaration());
                         ROSE_ASSERT(target_classDeclaration != NULL);
#if 0
                         printf ("snippet: classDeclaration = %p = %s \n",classDeclaration,classDeclaration->get_name().str());
                         printf ("target: classDeclaration = %p = %s \n",target_classDeclaration,target_classDeclaration->get_name().str());
#endif
                         ROSE_ASSERT(classDeclaration->get_name() == target_classDeclaration->get_name());

                         returnType = classSymbolInTargetAST->get_type();
                       }
                    break;
                  }

               case V_SgTypedefType:
                  {
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(snippet_declaration);
                    if (typedefDeclaration != NULL)
                       {
                         SgTypedefSymbol* typedefSymbolInTargetAST = lookupTypedefSymbolInParentScopes(typedefDeclaration->get_name(),targetScope);

                      // Not clear if we have to handle a more general case here.
                      // ROSE_ASSERT(typedefSymbolInTargetAST != NULL);
                      // returnType = typedefSymbolInTargetAST->get_type();
                         if (typedefSymbolInTargetAST == NULL)
                            {
#if 0
                              printf ("Error: It is an error to not have a typedef type defined in the target AST (this is an old rule, we have to support more general rules now)! \n");
                              printf ("   --- The target AST must have a valid typedef type (and associated declaration) to support resetting the SgTypedefType: %p \n",typedefDeclaration->get_type());
#endif
                           // DQ (3/16/2014): Find the associated typedef declaration (from the target AST)
                           // for the input type associated with its declaration in the snippet AST.
                              SgSymbol* symbol = findAssociatedSymbolInTargetAST(typedefDeclaration,targetScope);
                              ROSE_ASSERT(symbol != NULL);

                              typedefSymbolInTargetAST = isSgTypedefSymbol(symbol);

                           // Note that test5d demonstrates this problem.
                           // ROSE_ASSERT(false);
                            }

                         ROSE_ASSERT(typedefSymbolInTargetAST != NULL);
                         SgTypedefDeclaration* target_typedefDeclaration = isSgTypedefDeclaration(typedefSymbolInTargetAST->get_declaration());
                         ROSE_ASSERT(target_typedefDeclaration != NULL);
#if 0
                         printf ("snippet: typedefDeclaration = %p = %s \n",typedefDeclaration,typedefDeclaration->get_name().str());
                         printf ("target:  typedefDeclaration = %p = %s \n",target_typedefDeclaration,target_typedefDeclaration->get_name().str());
#endif
                         ROSE_ASSERT(typedefDeclaration->get_name() == target_typedefDeclaration->get_name());

                         returnType = typedefSymbolInTargetAST->get_type();
                       }
                    break;
                  }

               case V_SgEnumType:
                  {
                    SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(snippet_declaration);
                    if (enumDeclaration != NULL)
                       {
                         ROSE_ASSERT(enumDeclaration->get_name().is_null() == false);
                         SgEnumSymbol* enumSymbolInTargetAST = lookupEnumSymbolInParentScopes(enumDeclaration->get_name(),targetScope);

                      // Not clear if we have to handle a more general case here.
                      // ROSE_ASSERT(enumSymbolInTargetAST != NULL);
                      // returnType = enumSymbolInTargetAST->get_type();
                         if (enumSymbolInTargetAST == NULL)
                            {
                              printf ("Error: It is an error to not have a enum type defined in the target AST! \n");
                              printf ("   --- The target AST must have a valid enum type (and associated declaration) to support resetting the SgEnumType: %p \n",enumDeclaration->get_type());

                           // We will allow this to pass for now, since it is a violation of the target AST, and not the snippet mechanism (I think).
                              returnType = snippet_type;

                           // Note that test5d demonstrates this problem.
                           // ROSE_ASSERT(false);
                            }
                           else
                            {
                              returnType = enumSymbolInTargetAST->get_type();
                            }
                       }

                    break;
                  }

               case V_SgJavaParameterizedType:
                  {
                 // DQ (3/10/2014): This type is a view of a generic class with dynamic type checking (e.g. T).
                 // This acts more like a class with reference to the template instead of the template instantiation.
                 // So reset the declaration.
#if 0
                    printf ("In getTargetFileTypeSupport(): case V_SgJavaParameterizedType: snippet_declaration = %p = %s \n",snippet_declaration,snippet_declaration->class_name().c_str());
#endif
#if 1
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(snippet_declaration);
                    if (classDeclaration != NULL)
                       {
#if 0
                         printf ("Looking for classDeclaration = %s \n",classDeclaration->get_name().str());
#endif
                         SgJavaParameterizedType* javaParameterizedType = isSgJavaParameterizedType(namedType);
                         ROSE_ASSERT(javaParameterizedType != NULL);
#if 0
                      // SgTemplateParameterPtrList* templateParameterList = javaParameterizedType->get_type_list();
                         SgTemplateParameterList* templateParameterListNode = javaParameterizedType->get_type_list();
                         ROSE_ASSERT(templateParameterListNode != NULL);
                         SgTemplateParameterPtrList* templateParameterList = &templateParameterListNode->get_args();
#else
                      // DQ (7/25/2014): Remove warning from GNU 4.8 compiler.
                      // SgTemplateParameterPtrList* templateParameterList = NULL;
#endif
                      // DQ (7/25/2014): Remove warning from GNU 4.8 compiler.
                      // SgTemplateArgumentPtrList*  templateSpecializationArgumentList = NULL;
#if 0
                         printf ("Calling lookupTemplateClassSymbolInParentScopes() name = %s \n",classDeclaration->get_name().str());
#endif
                      // SgTemplateClassSymbol* templateClassSymbolInTargetAST = lookupTemplateClassSymbolInParentScopes(classDeclaration->get_name(),templateParameterList,templateSpecializationArgumentList,targetScope);
                         SgClassSymbol* templateClassSymbolInTargetAST = lookupClassSymbolInParentScopes(classDeclaration->get_name(),targetScope);
#if 0
                         printf ("DONE: Calling lookupTemplateClassSymbolInParentScopes() \n");
#endif
#if 0
                         printf ("targetScope->get_symbol_table()->size() = %d \n",targetScope->get_symbol_table()->size());
                         if (templateClassSymbolInTargetAST == NULL)
                            {
                              targetScope->get_symbol_table()->print("ERROR: templateClassSymbolInTargetAST == NULL");
                            }
#endif
                      // DQ (3/30/2014): Add this approach.
                         if (templateClassSymbolInTargetAST == NULL)
                            {
#if 0
                              printf ("Calling findAssociatedSymbolInTargetAST \n");
#endif
                              SgSymbol* symbol = findAssociatedSymbolInTargetAST(classDeclaration,targetScope);
                              ROSE_ASSERT(symbol != NULL);

                              templateClassSymbolInTargetAST = isSgClassSymbol(symbol);

                              ROSE_ASSERT(templateClassSymbolInTargetAST != NULL);
                            }

                      // Not clear if we have to handle a more general case here.
                         ROSE_ASSERT(templateClassSymbolInTargetAST != NULL);

                         returnType = templateClassSymbolInTargetAST->get_type();
                       }
#else
                    SgJavaParameterizedType* javaParameterizedType = isSgJavaParameterizedType(namedType);
                    if (javaParameterizedType != NULL)
                       {
#error "DEAD CODE!"
                      // Not clear how to lookup this type in the target AST.
                         returnType = javaParameterizedType;

                         SgType* internal_type = javaParameterizedType->get_raw_type();
                         ROSE_ASSERT(internal_type != NULL);
                       }
#endif
#if 0
                    printf ("SgJavaParameterizedType not yet tested! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

               case V_SgJavaQualifiedType:
                  {
                 // DQ (3/10/2014): This type acts like a binary operator on types to define aggregate
                 // types to represent what in C++ would be name qualification. I need only set the
                 // declarations in each SgJavaQualifiedType to refer to a declaration in the target AST.
                 // So reset the declaration.

                 // This case is demonstrated by test code:
                 //    SS_JAVA_CWES/src/Error_Handling/CWE_248/CWE_248_0.java,Error_Handling.CWE_248.CWE_248_0.cwe_248_0
                 // printf ("***** SgJavaQualifiedType not yet tested! *** \n");

                    printf ("In getTargetFileTypeSupport(): case V_SgJavaQualifiedType: snippet_declaration = %p = %s \n",snippet_declaration,snippet_declaration->class_name().c_str());

                    SgJavaQualifiedType* javaQualifiedType = isSgJavaQualifiedType(namedType);
                    if (javaQualifiedType != NULL)
                       {
                      // Not clear how to lookup this type in the target AST.
                         returnType = javaQualifiedType;

                         SgType* internal_type_1 = javaQualifiedType->get_parent_type();
                         ROSE_ASSERT(internal_type_1 != NULL);
                         SgType* internal_type_2 = javaQualifiedType->get_type();
                         ROSE_ASSERT(internal_type_2 != NULL);
                       }

                    printf ("Case of SgJavaQualifiedType: not yet handled: commented out assertion! \n");
                 // ROSE_ASSERT(false);
                    break;
                  }

               case V_SgJavaWildcardType:
                  {
                 // DQ (3/10/2014): This type expressed constraints on an input type.
                 // if (?) then it is associated with the Java object type.
                 // It can be constraint with an upper bound or lower bound.
                 // if (?extends List) would be an upper bound for List.
                 // if (?super Integer) would be an lower bound for List.
                 // So reset the declaration.

                    printf ("In getTargetFileTypeSupport(): case V_SgJavaWildcardType: snippet_declaration = %p = %s \n",snippet_declaration,snippet_declaration->class_name().c_str());

                    SgJavaWildcardType* javaWildcardType = isSgJavaWildcardType(namedType);
                    if (javaWildcardType != NULL)
                       {
                      // Not clear how to lookup this type in the target AST.
                         returnType = javaWildcardType;
                       }

                    printf ("SgJavaWildcardType not yet tested! \n");
                    ROSE_ASSERT(false);
                    break;
                  }

               default:
                  {
                    printf ("Error: In getTargetFileTypeSupport(): default reached in switch: namedType = %p = %s \n",namedType,namedType->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }

          ROSE_ASSERT(returnType != NULL);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
       // Non-named types are shared, so we need not reset them.

       // If this was not a named type then return NULL (which is checked at the
       // calling point, so that the type will not be reset).
        }

     return returnType;
   }


SgType*
SageBuilder::getTargetFileType(SgType* snippet_type, SgScopeStatement* targetScope)
   {
     SgType* returnType = NULL;

     ROSE_ASSERT(snippet_type != NULL);
     ROSE_ASSERT(targetScope != NULL);

  // DQ (3/17/2014): Refactored code.
  // See if the type might be asociated with the snippet file.
     SgType* type_copy     = snippet_type;

#if 0
     printf ("(before type_copy->getInternalTypes()): type_copy = %p = %s \n",type_copy,type_copy->class_name().c_str());
#endif

  // We need to be able to reproduce the pointer types to class types, etc.
     Rose_STL_Container<SgType*> typeList = type_copy->getInternalTypes();

#if 0
     for (size_t i = 0; i < typeList.size(); i++)
        {
          printf ("Input type: typeList[i=%" PRIuPTR "] = %p = %s \n",i,typeList[i],typeList[i]->class_name().c_str());
        }
#endif

#if 1
  // This is the unwrapped version of the getTargetFileType() function.
     returnType = getTargetFileTypeSupport(snippet_type,targetScope);
#else
     SgNamedType* namedType = isSgNamedType(snippet_type);

#error "DEAD CODE!"

     if (namedType != NULL)
        {
       // Find the associated declaration and it's corresponding declaration in the target AST.
          SgDeclarationStatement* snippet_declaration = namedType->get_declaration();
          ROSE_ASSERT(snippet_declaration != NULL);
#if 0
          printf ("Need to find the declaration in the target AST that is associated with the snippet_declaration in the snippet AST \n");
          printf ("   --- snippet_declaration = %p = %s = %s \n",snippet_declaration,snippet_declaration->class_name().c_str(),SageInterface::get_name(snippet_declaration).c_str());
#endif
       // There are only a few cases here!
          switch (namedType->variantT())
             {
               case V_SgClassType:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(snippet_declaration);
                    if (classDeclaration != NULL)
                       {
                         SgClassSymbol* classSymbolInTargetAST = lookupClassSymbolInParentScopes(classDeclaration->get_name(),targetScope);
                         if (classSymbolInTargetAST == NULL)
                            {
                           // For Java or C++ this could be a name qualified type and so we need a better mechanism
                           // to identify it thorugh it's parent scopes. Build a list of parent scope back to the
                           // global scope and then traverse the list backwards to identify each scope in the target
                           // AST's global scope until we each the associated declaration in the target AST.
#if 0
                              printf ("This is likely a name qualified scope (which can't be seen in a simple traversal of the parent scope (case of C++ or Java) \n");
                              printf ("   --- Looking for target AST match for class name = %s \n",classDeclaration->get_name().str());
#endif
                              SgSymbol* symbol = findAssociatedSymbolInTargetAST(classDeclaration,targetScope);
                              ROSE_ASSERT(symbol != NULL);

                              classSymbolInTargetAST = isSgClassSymbol(symbol);
                            }

                         ROSE_ASSERT(classSymbolInTargetAST != NULL);
                         SgClassDeclaration* target_classDeclaration = isSgClassDeclaration(classSymbolInTargetAST->get_declaration());
                         ROSE_ASSERT(target_classDeclaration != NULL);
#if 0
                         printf ("snippet: classDeclaration = %p = %s \n",classDeclaration,classDeclaration->get_name().str());
                         printf ("target: classDeclaration = %p = %s \n",target_classDeclaration,target_classDeclaration->get_name().str());
#endif
                         ROSE_ASSERT(classDeclaration->get_name() == target_classDeclaration->get_name());

                         returnType = classSymbolInTargetAST->get_type();
                       }
                    break;
                  }

               case V_SgTypedefType:
                  {
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(snippet_declaration);
                    if (typedefDeclaration != NULL)
                       {
                         SgTypedefSymbol* typedefSymbolInTargetAST = lookupTypedefSymbolInParentScopes(typedefDeclaration->get_name(),targetScope);

                      // Not clear if we have to handle a more general case here.
                      // ROSE_ASSERT(typedefSymbolInTargetAST != NULL);
                      // returnType = typedefSymbolInTargetAST->get_type();
                         if (typedefSymbolInTargetAST == NULL)
                            {
#if 0
                              printf ("Error: It is an error to not have a typedef type defined in the target AST (this is an old rule, we have to support more general rules now)! \n");
                              printf ("   --- The target AST must have a valid typedef type (and associated declaration) to support resetting the SgTypedefType: %p \n",typedefDeclaration->get_type());
#endif
                           // DQ (3/16/2014): Find the associated typedef declaration (from the target AST)
                           // for the input type associated with its declaration in the snippet AST.
                              SgSymbol* symbol = findAssociatedSymbolInTargetAST(typedefDeclaration,targetScope);
                              ROSE_ASSERT(symbol != NULL);

                              typedefSymbolInTargetAST = isSgTypedefSymbol(symbol);

                           // Note that test5d demonstrates this problem.
                           // ROSE_ASSERT(false);
                            }

                         ROSE_ASSERT(typedefSymbolInTargetAST != NULL);
                         SgTypedefDeclaration* target_typedefDeclaration = isSgTypedefDeclaration(typedefSymbolInTargetAST->get_declaration());
                         ROSE_ASSERT(target_typedefDeclaration != NULL);
#if 0
                         printf ("snippet: typedefDeclaration = %p = %s \n",typedefDeclaration,typedefDeclaration->get_name().str());
                         printf ("target:  typedefDeclaration = %p = %s \n",target_typedefDeclaration,target_typedefDeclaration->get_name().str());
#endif
                         ROSE_ASSERT(typedefDeclaration->get_name() == target_typedefDeclaration->get_name());

                         returnType = typedefSymbolInTargetAST->get_type();
                       }
                    break;
                  }

               case V_SgEnumType:
                  {
                    SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(snippet_declaration);
                    if (enumDeclaration != NULL)
                       {
                         ROSE_ASSERT(enumDeclaration->get_name().is_null() == false);
                         SgEnumSymbol* enumSymbolInTargetAST = lookupEnumSymbolInParentScopes(enumDeclaration->get_name(),targetScope);

                      // Not clear if we have to handle a more general case here.
                      // ROSE_ASSERT(enumSymbolInTargetAST != NULL);
                      // returnType = enumSymbolInTargetAST->get_type();
                         if (enumSymbolInTargetAST == NULL)
                            {
                              printf ("Error: It is an error to not have a enum type defined in the target AST! \n");
                              printf ("   --- The target AST must have a valid enum type (and associated declaration) to support resetting the SgEnumType: %p \n",enumDeclaration->get_type());

                           // We will allow this to pass for now, since it is a violation of the target AST, and not the snippet mechanism (I think).
                              returnType = snippet_type;

                           // Note that test5d demonstrates this problem.
                           // ROSE_ASSERT(false);
                            }
                           else
                            {
                              returnType = enumSymbolInTargetAST->get_type();
                            }
                       }

                    break;
                  }

               case V_SgJavaParameterizedType:
                  {
                 // DQ (3/10/2014): This type is a view of a generic class with dynamic type checking (e.g. T).
                 // This acts more like a class with reference to the template instead of the template instantiation.
                 // So reset the declaration.

                    printf ("In getTargetFileType(): case V_SgJavaParameterizedType: snippet_declaration = %p = %s \n",snippet_declaration,snippet_declaration->class_name().c_str());
#if 1
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(snippet_declaration);
                    if (classDeclaration != NULL)
                       {
                         SgTemplateParameterPtrList* templateParameterList              = NULL;
                         SgTemplateArgumentPtrList*  templateSpecializationArgumentList = NULL;
                         SgTemplateClassSymbol* templateClassSymbolInTargetAST = lookupTemplateClassSymbolInParentScopes(classDeclaration->get_name(),templateParameterList,templateSpecializationArgumentList,targetScope);

                      // Not clear if we have to handle a more general case here.
                         ROSE_ASSERT(templateClassSymbolInTargetAST != NULL);

                         returnType = templateClassSymbolInTargetAST->get_type();
                       }
#else
                    SgJavaParameterizedType* javaParameterizedType = isSgJavaParameterizedType(namedType);
                    if (javaParameterizedType != NULL)
                       {
                      // Not clear how to lookup this type in the target AST.
                         returnType = javaParameterizedType;

                         SgType* internal_type = javaParameterizedType->get_raw_type();
                         ROSE_ASSERT(internal_type != NULL);
                       }
#endif
                    printf ("SgJavaParameterizedType not yet tested! \n");
                    ROSE_ASSERT(false);
                    break;
                  }

               case V_SgJavaQualifiedType:
                  {
                 // DQ (3/10/2014): This type acts like a binary operator on types to define aggregate
                 // types to represent what in C++ would be name qualification. I need only set the
                 // declarations in each SgJavaQualifiedType to refer to a declaration in the target AST.
                 // So reset the declaration.

                    printf ("In getTargetFileType(): case V_SgJavaQualifiedType: snippet_declaration = %p = %s \n",snippet_declaration,snippet_declaration->class_name().c_str());

                    SgJavaQualifiedType* javaQualifiedType = isSgJavaQualifiedType(namedType);
                    if (javaQualifiedType != NULL)
                       {
                      // Not clear how to lookup this type in the target AST.
                         returnType = javaQualifiedType;

                         SgType* internal_type_1 = javaQualifiedType->get_parent_type();
                         ROSE_ASSERT(internal_type_1 != NULL);
                         SgType* internal_type_2 = javaQualifiedType->get_type();
                         ROSE_ASSERT(internal_type_2 != NULL);
                       }

                    printf ("SgJavaQualifiedType not yet tested! \n");
                    ROSE_ASSERT(false);
                    break;
                  }

               case V_SgJavaWildcardType:
                  {
                 // DQ (3/10/2014): This type expressed constraints on an input type.
                 // if (?) then it is associated with the Java object type.
                 // It can be constraint with an upper bound or lower bound.
                 // if (?extends List) would be an upper bound for List.
                 // if (?super Integer) would be an lower bound for List.
                 // So reset the declaration.

                    printf ("In getTargetFileType(): case V_SgJavaWildcardType: snippet_declaration = %p = %s \n",snippet_declaration,snippet_declaration->class_name().c_str());

                    SgJavaWildcardType* javaWildcardType = isSgJavaWildcardType(namedType);
                    if (javaWildcardType != NULL)
                       {
                      // Not clear how to lookup this type in the target AST.
                         returnType = javaWildcardType;

                         SgType* internal_type_1 = javaWildcardType->get_bound_type();
                         // ROSE_ASSERT(internal_type_1 != NULL); // PC: 03/15/2014 - Dan, this cannot be asserted as the bound_type CAN BE NULL.
                       }

                    printf ("SgJavaWildcardType not yet tested! \n");
                    ROSE_ASSERT(false);
                    break;
                  }

               default:
                  {
                    printf ("Error: In getTargetFileType(): default reached in switch: namedType = %p = %s \n",namedType,namedType->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }

          ROSE_ASSERT(returnType != NULL);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
       // Non-named types are shared, so we need not reset them.

       // If this was not a named type then return NULL (which is checked at the
       // calling point, so that the type will not be reset).
        }
#endif

     SgType* new_type = returnType;

  // DQ (3/17/2014): Refactored code.
  // Now rebuild the type_copy as required to represent associated modifiers, typedef wrappers, pointers and references.
     if (new_type != NULL && typeList.size() > 1)
        {
          int size = (int)typeList.size();
          for (int i = size - 2; i >= 0; i--)
             {
#if 0
               printf ("Rebuild type: typeList[i=%d] = %p = %s \n",i,typeList[i],typeList[i]->class_name().c_str());
#endif
            // SgModifierType* SgModifierType::createType(SgType* base_type, unsigned int f, SgExpression* optional_fortran_type_kind )
               switch(typeList[i]->variantT())
                  {
                    case V_SgModifierType:
                       {
                         SgModifierType* modifierType = isSgModifierType(typeList[i]);
                         ROSE_ASSERT(modifierType != NULL);
                         if (modifierType->get_typeModifier().get_constVolatileModifier().isConst() == true)
                            {
                              ROSE_ASSERT(new_type != NULL);
#if 0
                              printf ("Building a SgModifierType: calling buildConstType(): new_type = %p = %s \n",new_type,new_type->class_name().c_str());
#endif
                              new_type = buildConstType(new_type);
                            }
                           else
                            {
                           // Flag any additional modifiers that we might require (make anything not supported an error).
                              printf ("Modifier kind not handled (not implemented) check what sort of modifier this is: \n");
                              modifierType->get_typeModifier().display("Modifier kind not handled");
                              ROSE_ASSERT(false);
                            }
                         break;
                       }

                    case V_SgTypedefType:
                       {
                         SgTypedefType* typedefType = isSgTypedefType(typeList[i]);
                         ROSE_ASSERT(typedefType != NULL);

                      // DQ (3/17/2014): Call the associated support function instead.
                      // SgType* SageBuilder::getTargetFileType(SgType* snippet_type, SgScopeStatement* targetScope)
                      // SgType* new_typedefType = getTargetFileType(typedefType,targetScope);
                         SgType* new_typedefType = getTargetFileTypeSupport(typedefType,targetScope);
                         ROSE_ASSERT(new_typedefType != NULL);
                         ROSE_ASSERT(isSgTypedefType(new_typedefType) != NULL);

                         new_type = new_typedefType;
#if 0
                         printf ("ERROSE: SgTypedefType kind not handled (not implemented) \n");
                         ROSE_ASSERT(false);
#endif
                         break;
                       }

                    case V_SgPointerType:
                       {
                         SgPointerType* pointerType = isSgPointerType(typeList[i]);
                         ROSE_ASSERT(pointerType != NULL);
#if 0
                         printf ("Building a SgPointerType: calling buildPointerType(): new_type = %p = %s \n",new_type,new_type->class_name().c_str());
#endif
                         ROSE_ASSERT(new_type != NULL);
                         new_type = buildPointerType(new_type);
#if 0
                         printf ("ERROSE: SgPointerType kind not handled (not implemented) \n");
                         ROSE_ASSERT(false);
#endif
                         break;
                       }

                    default:
                       {
                         printf ("Error: default reached in evaluation of typelist: typeList[i] = %p = %s \n",typeList[i],typeList[i]->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }
             }

          returnType = new_type;
        }

#if 0
     if (typeList.size() > 1)
        {
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
        }
#endif


     return returnType;
   }



void
SageBuilder::errorCheckingTargetAST (SgNode* node_copy, SgNode* node_original, SgFile* targetFile, bool failOnWarning )
   {
#if 0
     printf ("In errorCheckingTargetAST(): node_copy = %p = %s node_original = %p = %s \n",node_copy,node_copy->class_name().c_str(),node_original,node_original->class_name().c_str());
#endif

  // Handle what is the same about all statements before getting to the switch.
     SgStatement* statement_copy     = isSgStatement(node_copy);
     SgStatement* statement_original = isSgStatement(node_original);
     if (statement_copy != NULL)
        {
       // Check the scope if it is stored explicitly.
          if (statement_copy->hasExplicitScope() == true)
             {
            // Handle the scope for all statements.
               SgScopeStatement* scope_copy     = statement_copy->get_scope();
               SgScopeStatement* scope_original = statement_original->get_scope();
               ROSE_ASSERT(scope_copy     != NULL);
               ROSE_ASSERT(scope_original != NULL);

            // if (TransformationSupport::getFile(scope_original) != targetFile)
            // if (getEnclosingFileNode(scope_original) != targetFile)
               if (getEnclosingFileNode(scope_copy) != targetFile)
                  {
#if 0
                    printf ("Warning: SgStatement: scope = %p = %s \n",scope_original,scope_original->class_name().c_str());
#endif
                 // SgFile* snippetFile = TransformationSupport::getFile(scope_original);
                 // SgFile* snippetFile = getEnclosingFileNode(scope_original);
                    SgFile* snippetFile = getEnclosingFileNode(scope_copy);
                    ROSE_ASSERT(snippetFile != NULL);
                    ROSE_ASSERT(snippetFile->get_sourceFileNameWithPath().empty() == false);
#if 1
                    printf ("Warning: SgStatement: scope not in target file (snippetFile = %p = %s) \n",snippetFile,snippetFile->get_sourceFileNameWithPath().c_str());
                 // ROSE_ASSERT(false);
#endif
                    if (failOnWarning == true)
                       {
                         printf ("Exit on warning! \n");
                         ROSE_ASSERT(false);
                       }
                  }
#if 0
               SgNode* insertionPointScope = (insertionPointIsScope == true) ? insertionPoint : insertionPoint->get_parent();
               printf ("insertionPointIsScope = %s insertionPointScope = %p = %s \n",insertionPointIsScope ? "true" : "false",insertionPointScope,insertionPointScope->class_name().c_str());

            // SgScopeStatement* targetScope = isSgScopeStatement(insertionPointScope);
            // ROSE_ASSERT(targetScope != NULL);

            // SgSymbol* symbol = initializedName_copy->search_for_symbol_from_symbol_table();
            // SgSymbol* symbol = initializedName_copy->search_for_symbol_from_symbol_table();
            // SgVariableSymbol* variableSymbolInTargetAST = lookupVariableSymbolInParentScopes(variableSymbol->get_name(),targetScope);
            // ROSE_ASSERT(variableSymbolInTargetAST != NULL);

            // Unless we know that this is a declaration we can't set the scope here using the information about this being a definng declaration.
            // If this is a defining declaration then we want to set it's scope to targetScope, else we want to lookup
            // the symbol through the parent scope and set the scope using the symbol's first non-defining declaration.
            // statement_copy->set_scope(targetScope);

            // SgSymbol* symbol = statement_copy->search_for_symbol_from_symbol_table();
            // ROSE_ASSERT(symbol != NULL);
#endif
#if 0
               printf ("SgClassDeclaration: Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
#if 0
               if (TransformationSupport::getFile(scope) != targetFile)
                  {
                    printf ("Warning: SgStatement: scope = %p = %s \n",scope,scope->class_name().c_str());
                    SgFile* snippetFile = TransformationSupport::getFile(scope);
                    ROSE_ASSERT(snippetFile != NULL);
                    ROSE_ASSERT(snippetFile->get_sourceFileNameWithPath().empty() == false);

                    printf ("Warning: SgStatement: scope not in target file (snippetFile = %p = %s) \n",snippetFile,snippetFile->get_sourceFileNameWithPath().c_str());
                 // ROSE_ASSERT(false);
                  }
#endif
             }
        }

  // Handle what is the same about all declaration before getting to the switch.
     SgDeclarationStatement* declarationStatement_copy     = isSgDeclarationStatement(node_copy);
     SgDeclarationStatement* declarationStatement_original = isSgDeclarationStatement(node_original);
     if (declarationStatement_copy != NULL)
        {
       // Check the firstnondefiningDeclaration and definingDeclaration
          SgDeclarationStatement* firstNondefiningDeclaration_copy     = declarationStatement_copy->get_firstNondefiningDeclaration();
          SgDeclarationStatement* firstNondefiningDeclaration_original = declarationStatement_original->get_firstNondefiningDeclaration();

       // DQ (3/17/2014): Bugfix, we want to use the firstNondefiningDeclaration_copy instead of firstNondefiningDeclaration_original.
       // DQ (3/10/2014): We want to allow for NULL return values from getEnclosingFileNode() for Java classes that are in java.lang (for example).
       // SgFile* snippetFile = getEnclosingFileNode(firstNondefiningDeclaration_original);
          SgFile* snippetFile = getEnclosingFileNode(firstNondefiningDeclaration_copy);
          if (snippetFile != NULL && snippetFile != targetFile)
             {
            // I think we want to allow this because it is a common occurence in any merged AST.
            // However, if might be worth fixing for other reasons.  This needs to be discussed.
#if 0
               printf ("Note: SgDeclarationStatement: firstNondefiningDeclaration_original is not in target file (allowed for merged ASTs) \n");
#endif
            // ROSE_ASSERT(false);
               if (failOnWarning == true)
                  {
                    printf ("Exit on warning! \n");
                    ROSE_ASSERT(false);
                  }
             }
            else
             {
            // Warn about this if snippetFile == NULL.
               if (snippetFile == NULL)
                  {
                    printf ("Note: firstNondefiningDeclaration_original = %p getEnclosingFileNode() returned NULL \n",firstNondefiningDeclaration_original);

                    if (failOnWarning == true)
                       {
                         printf ("Exit on warning! \n");
                         ROSE_ASSERT(false);
                       }
                  }
             }

          SgDeclarationStatement* definingDeclaration_copy     = declarationStatement_copy->get_definingDeclaration();
          SgDeclarationStatement* definingDeclaration_original = declarationStatement_original->get_definingDeclaration();
          if (definingDeclaration_original != NULL)
             {
            // DQ (3/17/2014): Bugfix, we want to use the definingDeclaration_copy instead of definingDeclaration_original.
            // if (TransformationSupport::getFile(definingDeclaration_original) != targetFile)
            // if (getEnclosingFileNode(definingDeclaration_original) != targetFile)
            // SgFile* snippetFile = getEnclosingFileNode(definingDeclaration_original);
               SgFile* snippetFile = getEnclosingFileNode(definingDeclaration_copy);
               if (snippetFile != NULL && snippetFile != targetFile)
                  {
#if 1
                    printf ("Warning: SgDeclarationStatement: definingDeclaration is not in target file \n");
                 // ROSE_ASSERT(false);
#endif
                    if (failOnWarning == true)
                       {
                         printf ("Exit on warning! \n");
                         ROSE_ASSERT(false);
                       }

                    if (declarationStatement_original == definingDeclaration_original)
                       {
                      // This is a defining declaration, so we can set the scope (or can we?)
                      // I guess we could if the translation map were complete, but it is not complete yet.
                       }
                  }
                 else
                  {
                 // Warn about this if snippetFile == NULL.
                    if (snippetFile == NULL)
                       {
                         printf ("Note: definingDeclaration_original = %p getEnclosingFileNode() returned NULL \n",definingDeclaration_original);

                         if (failOnWarning == true)
                            {
                              printf ("Exit on warning! \n");
                              ROSE_ASSERT(false);
                            }
                       }
                  }
             }
        }

  // Handle what is the same about all expressions before getting to the switch.
     SgExpression* expression = isSgExpression(node_copy);
     if (expression != NULL)
        {
       // Check the scope if it is stored explicitly.

       // printf ("WARNING: Need to check if the type is explicitly stored in this expression! \n");

          if (expression->hasExplicitType() == true)
             {
            // Handle the type for all expressions.
               SgType* type = expression->get_type();
               ROSE_ASSERT(type != NULL);
             }
        }

#if 0
     printf ("Leaving errorCheckingTargetAST() \n");
#endif
   }


template <class T>
void
SageBuilder::resetDeclaration(T* classDeclaration_copy, T* classDeclaration_original, SgScopeStatement* targetScope)
   {
  // I'm not sure if this function is a good idea since we can't call set_scope() easily from any
  // SgDeclarationStatement and I don't want to make set_scope() a virtual function because it would
  // not make sense everywhere.

  // DQ (3/17/2014): This code is similar to the case for SgEnumDeclaration (later we can refactor this if this works well).
     T* classDeclaration_copy_defining        = dynamic_cast<T*>(classDeclaration_copy->get_definingDeclaration());
     T* classDeclaration_copy_nondefining     = dynamic_cast<T*>(classDeclaration_copy->get_firstNondefiningDeclaration());
     T* classDeclaration_original_defining    = dynamic_cast<T*>(classDeclaration_original->get_definingDeclaration());
     T* classDeclaration_original_nondefining = dynamic_cast<T*>(classDeclaration_original->get_firstNondefiningDeclaration());

  // Set the scope if it is still set to the scope of the snippet AST.
     if (classDeclaration_copy_defining != NULL && classDeclaration_copy_defining->get_scope() == classDeclaration_original_defining->get_scope())
        {
#if 0
          printf ("reset the scope of classDeclaration_copy_defining \n");
#endif
          classDeclaration_copy_defining->set_scope(targetScope);
        }

  // Set the scope if it is still set to the scope of the snippet AST.
     if (classDeclaration_copy_nondefining != NULL && classDeclaration_copy_nondefining->get_scope() == classDeclaration_original_nondefining->get_scope())
        {
#if 0
          printf ("reset the scope of classDeclaration_copy_nondefining \n");
#endif
          classDeclaration_copy_nondefining->set_scope(targetScope);
        }

  // Set the parent if it is still set to a node of the snippet AST.
     if (classDeclaration_copy_nondefining != NULL && classDeclaration_copy_nondefining->get_parent() == classDeclaration_original_nondefining->get_parent())
        {
#if 0
          printf ("reset the parent of classDeclaration_copy_nondefining \n");
#endif
          classDeclaration_copy_nondefining->set_parent(classDeclaration_copy->get_parent());
        }
   }


void
SageBuilder::fixupCopyOfNodeFromSeparateFileInNewTargetAst(SgStatement* insertionPoint, bool insertionPointIsScope,
                                                           SgNode* node_copy, SgNode* node_original)
   {
  // This function fixes up invidual IR nodes to be consistant in the context of the target AST
  // where the node is inserted and at the point specified by insertionPoint.  In this function,
  // node_copy is the copy that was made of node_original by the AST copy function.  The node_original
  // is assumed to be the node that is in the AST snippet (it is still connected in the snippet's
  // AST (from compilation of the snippet file).

  // This function hides the details of handling each different type of IR node.
  // It is assume that the node_copy is from an AST sub-tree generated by the AST copy mechanism,
  // and that the insertionPoint is a location in the target AST where the snippet AST has already
  // been inserted, this function makes each IR node internally consistant with the target AST.

  // BTW, the translationMap should only be required to support references to things that are name
  // qualified (which are C++ specific). These are a performance option to simplify tacking back
  // through scopes with code similarly complex as to what is supported in the name qualification
  // support.
#if 0
     printf ("In fixupCopyOfNodeFromSeperateFileInNewTargetAst: node_copy = %p = %s \n",node_copy,node_copy->class_name().c_str());
#endif

#if 0
     printf ("Disabled fixupCopyOfNodeFromSeperateFileInNewTargetAst() \n");
     return;
#endif

  // SgFile* targetFile = TransformationSupport::getFile(insertionPoint);
     SgFile* targetFile = getEnclosingFileNode(insertionPoint);

#if 0
     printf ("   --- targetFile            = %p = %s \n",targetFile,targetFile->get_sourceFileNameWithPath().c_str());
     printf ("   --- insertionPointIsScope = %s \n",insertionPointIsScope ? "true" : "false");
#endif

  // DQ (3/4/2014): As I recall there is a reason why we can't setup the scope here.

  // We also need to handle the symbol (move it from the body (SgBaicBlock) that was
  // a copy to the scope in the target AST where the SgInitializedName was inserted).
     SgNode* insertionPointScope = (insertionPointIsScope == true) ? insertionPoint : insertionPoint->get_parent();
#if 0
     printf ("insertionPointScope = %p = %s \n",insertionPointScope,insertionPointScope->class_name().c_str());
#endif
     SgScopeStatement* targetScope = isSgScopeStatement(insertionPointScope);
     ROSE_ASSERT(targetScope != NULL);

#if 1
  // Refactored code (error checking done after AST fixup).
#if 0
     errorCheckingTargetAST(node_copy,node_original,targetFile, false);
#endif
#else

#error "DEAD CODE!"

  // Handle what is the same about all statements before getting to the switch.
     SgStatement* statement_copy     = isSgStatement(node_copy);
     SgStatement* statement_original = isSgStatement(node_original);
     if (statement_copy != NULL)
        {
       // Check the scope if it is stored explicitly.
          if (statement_copy->hasExplicitScope() == true)
             {
            // Handle the scope for all statements.
               SgScopeStatement* scope_copy     = statement_copy->get_scope();
               SgScopeStatement* scope_original = statement_original->get_scope();
               ROSE_ASSERT(scope_copy     != NULL);
               ROSE_ASSERT(scope_original != NULL);

            // if (TransformationSupport::getFile(scope_original) != targetFile)
               if (getEnclosingFileNode(scope_original) != targetFile)
                  {
#if 0
                    printf ("Warning: SgStatement: scope = %p = %s \n",scope_original,scope_original->class_name().c_str());
#endif
                 // SgFile* snippetFile = TransformationSupport::getFile(scope_original);
                    SgFile* snippetFile = getEnclosingFileNode(scope_original);
                    ROSE_ASSERT(snippetFile != NULL);
                    ROSE_ASSERT(snippetFile->get_sourceFileNameWithPath().empty() == false);
#if 0
                    printf ("Warning: SgStatement: scope not in target file (snippetFile = %p = %s) \n",snippetFile,snippetFile->get_sourceFileNameWithPath().c_str());
                 // ROSE_ASSERT(false);
#endif
                  }
#if 0
               SgNode* insertionPointScope = (insertionPointIsScope == true) ? insertionPoint : insertionPoint->get_parent();
               printf ("insertionPointIsScope = %s insertionPointScope = %p = %s \n",insertionPointIsScope ? "true" : "false",insertionPointScope,insertionPointScope->class_name().c_str());

            // SgScopeStatement* targetScope = isSgScopeStatement(insertionPointScope);
            // ROSE_ASSERT(targetScope != NULL);

            // SgSymbol* symbol = initializedName_copy->search_for_symbol_from_symbol_table();
            // SgSymbol* symbol = initializedName_copy->search_for_symbol_from_symbol_table();
            // SgVariableSymbol* variableSymbolInTargetAST = lookupVariableSymbolInParentScopes(variableSymbol->get_name(),targetScope);
            // ROSE_ASSERT(variableSymbolInTargetAST != NULL);

            // Unless we know that this is a declaration we can't set the scope here using the information about this being a definng declaration.
            // If this is a defining declaration then we want to set it's scope to targetScope, else we want to lookup
            // the symbol through the parent scope and set the scope using the symbol's first non-defining declaration.
            // statement_copy->set_scope(targetScope);

            // SgSymbol* symbol = statement_copy->search_for_symbol_from_symbol_table();
            // ROSE_ASSERT(symbol != NULL);
#endif
#if 0
               printf ("SgClassDeclaration: Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
#if 0
               if (TransformationSupport::getFile(scope) != targetFile)
                  {
                    printf ("Warning: SgStatement: scope = %p = %s \n",scope,scope->class_name().c_str());
                    SgFile* snippetFile = TransformationSupport::getFile(scope);
                    ROSE_ASSERT(snippetFile != NULL);
                    ROSE_ASSERT(snippetFile->get_sourceFileNameWithPath().empty() == false);

                    printf ("Warning: SgStatement: scope not in target file (snippetFile = %p = %s) \n",snippetFile,snippetFile->get_sourceFileNameWithPath().c_str());
                 // ROSE_ASSERT(false);
                  }
#endif
             }
        }

#error "DEAD CODE!"

  // Handle what is the same about all declaration before getting to the switch.
     SgDeclarationStatement* declarationStatement_copy     = isSgDeclarationStatement(node_copy);
     SgDeclarationStatement* declarationStatement_original = isSgDeclarationStatement(node_original);
     if (declarationStatement_copy != NULL)
        {
       // Check the firstnondefiningDeclaration and definingDeclaration
          SgDeclarationStatement* firstNondefiningDeclaration_original = declarationStatement_original->get_firstNondefiningDeclaration();

       // DQ (3/10/2014): We want to allow for NULL return values from getEnclosingFileNode() for Java classes that are in java.lang (for example).
          SgFile* snippetFile = getEnclosingFileNode(firstNondefiningDeclaration_original);
          if (snippetFile != NULL && snippetFile != targetFile)
             {
            // I think we want to allow this because it is a common occurence in any merged AST.
            // However, if might be worth fixing for other reasons.  This needs to be discussed.
#if 0
               printf ("Note: SgDeclarationStatement: firstNondefiningDeclaration_original is not in target file (allowed for merged ASTs) \n");
#endif
            // ROSE_ASSERT(false);
             }
            else
             {
            // Warn about this if snippetFile == NULL.
               if (snippetFile == NULL)
                  {
                    printf ("Note: firstNondefiningDeclaration_original = %p getEnclosingFileNode() returned NULL \n",firstNondefiningDeclaration_original);
                  }
             }

#error "DEAD CODE!"

          SgDeclarationStatement* definingDeclaration_original = declarationStatement_original->get_definingDeclaration();
          if (definingDeclaration_original != NULL)
             {
            // if (TransformationSupport::getFile(definingDeclaration_original) != targetFile)
            // if (getEnclosingFileNode(definingDeclaration_original) != targetFile)
               SgFile* snippetFile = getEnclosingFileNode(definingDeclaration_original);
               if (snippetFile != NULL && snippetFile != targetFile)
                  {
#if 0
                    printf ("Warning: SgDeclarationStatement: definingDeclaration is not in target file \n");
                 // ROSE_ASSERT(false);
#endif
                    if (declarationStatement_original == definingDeclaration_original)
                       {
                      // This is a defining declaration, so we can set the scope (or can we?)
                      // I guess we could if the translation map were complete, but it is not complete yet.
                       }
                  }
                 else
                  {
                 // Warn about this if snippetFile == NULL.
                    if (snippetFile == NULL)
                       {
                         printf ("Note: definingDeclaration_original = %p getEnclosingFileNode() returned NULL \n",definingDeclaration_original);
                       }
                  }
             }
        }

#error "DEAD CODE!"

#endif

  // Handle what is the same about all expressions before getting to the switch.
     SgExpression* expression = isSgExpression(node_copy);
     if (expression != NULL)
        {
       // Check the scope if it is stored explicitly.

       // printf ("WARNING: Need to check if the type is explicitly stored in this expression! \n");

          if (expression->hasExplicitType() == true)
             {
            // Handle the type for all expressions.
               SgType* type = expression->get_type();
               ROSE_ASSERT(type != NULL);

            // DQ (3/17/2014): Avoid calling stripType with the newly refactored getTargetFileType() function.
            // SgType* new_type = getTargetFileType(type->stripType(),targetScope);
               SgType* new_type = getTargetFileType(type,targetScope);
               if (new_type != NULL)
                  {
                 // Reset the base type to be the one associated with the target file.
                    expression->set_explicitly_stored_type(new_type);
                  }
             }
        }

     switch (node_copy->variantT())
        {
          case V_SgInitializedName:
             {
               SgInitializedName* initializedName_copy     = isSgInitializedName(node_copy);
               SgInitializedName* initializedName_original = isSgInitializedName(node_original);

            // See if the scope might be associated with the snippet file.

            // Since we don't want the scope that is stored in the SgInitializedName we
            // have to get the associated statement and the scope of that statement.
            // SgScopeStatement* scope_copy     = initializedName_copy->get_scope();
               SgStatement* enclosingStatement_copy = TransformationSupport::getStatement(initializedName_copy);
#if 0
               printf ("enclosingStatement_copy = %p = %s \n",enclosingStatement_copy,enclosingStatement_copy->class_name().c_str());
#endif
               SgScopeStatement* scope_copy     = enclosingStatement_copy->get_scope();

               SgScopeStatement* scope_original = initializedName_original->get_scope();

               ROSE_ASSERT(scope_copy != NULL);
               ROSE_ASSERT(scope_original != NULL);

            // if (TransformationSupport::getFile(scope_original) != targetFile)
               if (getEnclosingFileNode(scope_original) != targetFile)
                  {
#if 0
                    ROSE_ASSERT(initializedName_copy != NULL);
                    printf ("initializedName_copy = %p = %s \n",initializedName_copy,initializedName_copy->get_name().str());
                    ROSE_ASSERT(initializedName_original != NULL);
                    printf ("initializedName_original = %p = %s \n",initializedName_original,initializedName_original->get_name().str());
                    SgType* initializedName_original_type = initializedName_original->get_type();
                    printf ("initializedName_original_type = %p = %s \n",initializedName_original_type,initializedName_original_type->class_name().c_str());
                    SgClassType* classType = isSgClassType(initializedName_original_type);
                 // ROSE_ASSERT(classType != NULL);
                    if (classType != NULL)
                       {
                         SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                         ROSE_ASSERT(classDeclaration != NULL);
                         printf ("classDeclaration = %p = %s \n",classDeclaration,classDeclaration->get_name().str());
                       }
#endif
#if 0
                    printf ("Warning: case V_SgInitializedName: scope_copy     = %p = %s \n",scope_copy,scope_copy->class_name().c_str());
                    printf ("Warning: case V_SgInitializedName: scope_original = %p = %s \n",scope_original,scope_original->class_name().c_str());

                    printf ("Warning: case V_SgInitializedName: initializedName_copy->get_parent()     = %p \n",initializedName_copy->get_parent());
                    printf ("Warning: case V_SgInitializedName: initializedName_original->get_parent() = %p \n",initializedName_original->get_parent());
#endif
                 // SgFile* snippetFile = TransformationSupport::getFile(scope_original);
                    SgFile* snippetFile = getEnclosingFileNode(scope_original);

                    ROSE_ASSERT(snippetFile != NULL);
                    ROSE_ASSERT(snippetFile->get_sourceFileNameWithPath().empty() == false);
#if 0
                    printf ("Warning: case V_SgInitializedName: scope not in target file (snippetFile = %p = %s) \n",snippetFile,snippetFile->get_sourceFileNameWithPath().c_str());
                 // ROSE_ASSERT(false);
#endif
                  }

            // See if the type might be asociated with the snippet file.
               SgType* type_copy     = initializedName_copy->get_type();
#if 0
               printf ("(before type_copy->getInternalTypes()): type_copy = %p = %s \n",type_copy,type_copy->class_name().c_str());
#endif

#if 0

#error "DEAD CODE!"

            // We need to be able to reproduce the pointer types to class types, etc.
               Rose_STL_Container<SgType*> typeList = type_copy->getInternalTypes();
#if 0
               for (size_t i = 0; i < typeList.size(); i++)
                  {
                    printf ("Input type: typeList[i=%" PRIuPTR "] = %p = %s \n",i,typeList[i],typeList[i]->class_name().c_str());
                  }
#endif
            // Note that the semantics of this function is that it can return a NULL pointer (e.g. for primative types).
               SgType* new_type = getTargetFileType(type_copy->stripType(),targetScope);

#error "DEAD CODE!"

            // Now rebuild the type_copy as required to represent associated modifiers, typedef wrappers, pointers and references.
               if (new_type != NULL && typeList.size() > 1)
                  {
                    int size = (int)typeList.size();
                    for (int i = size - 2; i >= 0; i--)
                       {
#if 0
                         printf ("Rebuild type: typeList[i=%d] = %p = %s \n",i,typeList[i],typeList[i]->class_name().c_str());
#endif
                      // SgModifierType* SgModifierType::createType(SgType* base_type, unsigned int f, SgExpression* optional_fortran_type_kind )
                         switch(typeList[i]->variantT())
                            {
                              case V_SgModifierType:
                                 {
                                   SgModifierType* modifierType = isSgModifierType(typeList[i]);
                                   ROSE_ASSERT(modifierType != NULL);
                                   if (modifierType->get_typeModifier().get_constVolatileModifier().isConst() == true)
                                      {
                                        ROSE_ASSERT(new_type != NULL);
#if 0
                                        printf ("Building a SgModifierType: calling buildConstType(): new_type = %p = %s \n",new_type,new_type->class_name().c_str());
#endif
                                        new_type = buildConstType(new_type);
                                      }
                                     else
                                      {
                                     // Flag any additional modifiers that we might require (make anything not supported an error).
                                        printf ("Modifier kind not handled (not implemented) check what sort of modifier this is: \n");
                                        modifierType->get_typeModifier().display("Modifier kind not handled");
                                        ROSE_ASSERT(false);
                                      }
                                   break;
                                 }

#error "DEAD CODE!"

                              case V_SgTypedefType:
                                 {
                                   SgTypedefType* typedefType = isSgTypedefType(typeList[i]);
                                   ROSE_ASSERT(typedefType != NULL);

                                // SgType* SageBuilder::getTargetFileType(SgType* snippet_type, SgScopeStatement* targetScope)
                                   SgType* new_typedefType = getTargetFileType(typedefType,targetScope);
                                   ROSE_ASSERT(new_typedefType != NULL);
                                   ROSE_ASSERT(isSgTypedefType(new_typedefType) != NULL);

                                   new_type = new_typedefType;
#if 0
                                   printf ("ERROSE: SgTypedefType kind not handled (not implemented) \n");
                                   ROSE_ASSERT(false);
#endif
                                   break;
                                 }

#error "DEAD CODE!"

                              case V_SgPointerType:
                                 {
                                   SgPointerType* pointerType = isSgPointerType(typeList[i]);
                                   ROSE_ASSERT(pointerType != NULL);
#if 0
                                   printf ("Building a SgPointerType: calling buildPointerType(): new_type = %p = %s \n",new_type,new_type->class_name().c_str());
#endif
                                   ROSE_ASSERT(new_type != NULL);
                                   new_type = buildPointerType(new_type);
#if 0
                                   printf ("ERROSE: SgPointerType kind not handled (not implemented) \n");
                                   ROSE_ASSERT(false);
#endif
                                   break;
                                 }

#error "DEAD CODE!"

                              default:
                                 {
                                   printf ("Error: default reached in evaluation of typelist: typeList[i] = %p = %s \n",typeList[i],typeList[i]->class_name().c_str());
                                   ROSE_ASSERT(false);
                                 }
                            }
                       }
                  }
#if 0
               if (typeList.size() > 1)
                  {
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
                  }
#endif

#error "DEAD CODE!"

#else
            // Refactored the above code to be a part of getTargetFileType() function.
            // Note that the semantics of this function is that it can return a NULL pointer (e.g. for primative types).
            // SgType* new_type = getTargetFileType(type_copy->stripType(),targetScope);
               SgType* new_type = getTargetFileType(type_copy,targetScope);
#endif
#if 0
               printf ("new_type = %p \n",new_type);
#endif
               if (new_type != NULL)
                  {
                 // Reset the base type to be the one associated with the target file.
#if 0
                    printf ("Reset type for initializedName_copy = %p from type = %p to type = %p \n",initializedName_copy,initializedName_copy->get_type(),new_type);
#endif
                    SgType* original_type = initializedName_copy->get_type();
                    SgNamedType* original_named_type = isSgNamedType(original_type);
                    SgNamedType* new_named_type = isSgNamedType(new_type);
                    if (original_named_type != NULL)
                       {
                         ROSE_ASSERT(new_named_type != NULL);
                         SgClassDeclaration* original_classDeclaration = isSgClassDeclaration(original_named_type->get_declaration());
                         SgClassDeclaration* new_classDeclaration      = isSgClassDeclaration(new_named_type->get_declaration());
                         if (original_classDeclaration != NULL)
                            {
                              ROSE_ASSERT(new_classDeclaration != NULL);
#if 0
                              printf ("original_classDeclaration = %p = %s \n",original_classDeclaration,original_classDeclaration->get_name().str());
                              printf ("new_classDeclaration      = %p = %s \n",new_classDeclaration,new_classDeclaration->get_name().str());
#endif
                           // Make sure that the type names are the same.
                              ROSE_ASSERT(new_classDeclaration->get_name() == original_classDeclaration->get_name());
                            }
                       }
#if 0
                    SgType* old_type = initializedName_copy->get_type();
                    printf ("Reset the type: initializedName_copy->set_type(new_type): old type = %p = %s new_type = %p = %s \n",old_type,old_type->class_name().c_str(),new_type,new_type->class_name().c_str());
#endif
                    initializedName_copy->set_type(new_type);
                  }
#if 0
               printf ("enclosingStatement_copy = %p = %s \n",enclosingStatement_copy,enclosingStatement_copy->class_name().c_str());
#endif
               SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(enclosingStatement_copy);
               if (functionParameterList != NULL)
                  {
                 // The use of SgInitializedName in function parametes is handled differently then in other
                 // locations in the AST (e.g. how the scope is set).
                 // This is a function parameter and the scope is set to the SgFunctionDefinition if
                 // this is for a defining function and the SgGlobal if it is a function prototype.
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionParameterList->get_parent());
                    ROSE_ASSERT(functionDeclaration != NULL);
                    SgFunctionDefinition* functionDefinition = functionDeclaration->get_definition();
                    if (functionDefinition != NULL)
                       {
                         ROSE_ASSERT(initializedName_copy->get_scope() == functionDefinition);
                      // initializedName_copy->set_scope(functionDefinition);
                       }
                      else
                       {
                         SgGlobal* globalScope = isSgGlobal(functionDeclaration->get_scope());
                         ROSE_ASSERT(globalScope != NULL);
                         if (initializedName_copy->get_scope() != globalScope)
                            {
#if 0
                              printf ("Reset scope for initializedName_copy = %p = %s \n",initializedName_copy,initializedName_copy->get_name().str());
#endif
                              initializedName_copy->set_scope(globalScope);
                            }
                         ROSE_ASSERT(initializedName_copy->get_scope() == globalScope);
                       }
                  }
                 else
                  {
#if 0
                    printf ("initializedName_copy->get_scope() = %p = %s \n",initializedName_copy->get_scope(),initializedName_copy->get_scope()->class_name().c_str());
#endif
                    SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(enclosingStatement_copy);
                    if (enumDeclaration != NULL)
                       {
                      // The case of enum declarations is special because the associated SgInitializedName IR nodes has a scope
                      // that is external to the SgEnumDeclaration (in the scope of the SgEnumDeclaration).  The typical case in C
                      // is that the enum declaration is in global scope and then the enum fields (represented by SgInitializedName
                      // objects) are have their associated symbol in the global scope.

                      // We have to use the name to search for the symbol instead of the pointer value of the initializedName_copy
                      // (since the original symbol was associated with initializedName_original).
                      // SgSymbol* symbol = initializedName_copy->search_for_symbol_from_symbol_table();
                         SgName name = initializedName_copy->get_name();
                         SgSymbol* symbol = initializedName_copy->get_scope()->lookup_enum_field_symbol(name);
                         ROSE_ASSERT(symbol != NULL);

                         SgEnumFieldSymbol* enumFieldSymbol = isSgEnumFieldSymbol(symbol);
                         ROSE_ASSERT(enumFieldSymbol != NULL);

                      // DQ (3/17/2014): Build a new sysmbol to for the initializedName_copy instead of reusing the existing symbol
                      // from the snippet AST.
                         SgEnumFieldSymbol* new_enumFieldSymbol = new SgEnumFieldSymbol(initializedName_copy);
                         ROSE_ASSERT(new_enumFieldSymbol != NULL);

                      // targetScope->insert_symbol(name,enumFieldSymbol);
                         targetScope->insert_symbol(name,new_enumFieldSymbol);

                      // DQ (3/6/2014): Set the scope of the SgInitializedName IR node.
                         initializedName_copy->set_scope(targetScope);
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                      else
                       {
#if 0
                         printf ("enclosingStatement_copy = %p = %s \n",enclosingStatement_copy,enclosingStatement_copy->class_name().c_str());
#endif
                         SgCatchOptionStmt* catchOptionStatement = isSgCatchOptionStmt(enclosingStatement_copy->get_parent());
                         if (catchOptionStatement != NULL)
                            {
                              SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(enclosingStatement_copy);
                              ROSE_ASSERT(variableDeclaration != NULL);

                           // SgSymbol* symbol = targetScope->lookup_variable_symbol(initializedName_copy->get_name());
                              SgStatement* enclosingStatement_original = TransformationSupport::getStatement(initializedName_original);
                              ROSE_ASSERT(enclosingStatement_original != NULL);
                              SgCatchOptionStmt* catchOptionStatement_original = isSgCatchOptionStmt(enclosingStatement_original->get_parent());

                           // SgSymbol* symbol = lookupVariableSymbolInParentScopes(initializedName_copy->get_name(),targetScope);
                              SgSymbol* symbol = lookupVariableSymbolInParentScopes(initializedName_copy->get_name(),catchOptionStatement_original);
                              if (symbol == NULL)
                                 {
                                   printf ("ERROR: (symbol == NULL): initializedName_copy->get_name() = %s \n",initializedName_copy->get_name().str());
                                // initializedName_original->get_file_info()->display("ERROR: (symbol == NULL): debug");
                                 }
                              ROSE_ASSERT(symbol != NULL);

                              initializedName_copy->set_scope(targetScope);

                              SgVariableSymbol* new_variableSymbol = new SgVariableSymbol(initializedName_copy);
                              ROSE_ASSERT(new_variableSymbol != NULL);

                           // DQ (3/19/2014): I am not certain this is the correct location to insert this symbol.
                              targetScope->insert_symbol(initializedName_copy->get_name(),new_variableSymbol);
                            }
                           else
                            {
                           // DQ (3/29/2014): Adding support for SgInitializedName IR nodes found in a SgJavaForEachStatement.
                              SgJavaForEachStatement* javaForEachStatement = isSgJavaForEachStatement(enclosingStatement_copy->get_parent());
                              if (javaForEachStatement != NULL)
                                 {
                                   SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(enclosingStatement_copy);
                                   ROSE_ASSERT(variableDeclaration != NULL);

                                   SgStatement* enclosingStatement_original = TransformationSupport::getStatement(initializedName_original);
                                   ROSE_ASSERT(enclosingStatement_original != NULL);
                                   SgJavaForEachStatement* javaForEachStatement_original = isSgJavaForEachStatement(enclosingStatement_original->get_parent());

                                   SgSymbol* symbol = lookupVariableSymbolInParentScopes(initializedName_copy->get_name(),javaForEachStatement_original);
                                   if (symbol == NULL)
                                      {
                                        printf ("ERROR: (symbol == NULL): initializedName_copy->get_name() = %s \n",initializedName_copy->get_name().str());
                                     // initializedName_original->get_file_info()->display("ERROR: (symbol == NULL): debug");
                                      }
                                   ROSE_ASSERT(symbol != NULL);

                                   initializedName_copy->set_scope(targetScope);

                                   SgVariableSymbol* new_variableSymbol = new SgVariableSymbol(initializedName_copy);
                                   ROSE_ASSERT(new_variableSymbol != NULL);

                                // DQ (3/29/2014): I am not certain this is the correct location to insert this symbol.
                                   targetScope->insert_symbol(initializedName_copy->get_name(),new_variableSymbol);
#if 0
                                   printf ("Need to handle case of SgJavaForEachStatement \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                                else
                                 {
                           // Case of non-SgFunctionParameterList and non-SgEnumDeclaration use of SgInitializedName in AST.
                              SgSymbol* symbol = initializedName_copy->search_for_symbol_from_symbol_table();
                              if (symbol == NULL)
                                 {
                                   printf ("ERROR: enclosingStatement_copy = %p = %s \n",enclosingStatement_copy,enclosingStatement_copy->class_name().c_str());
                                   ROSE_ASSERT(enclosingStatement_copy->get_parent() != NULL);
                                   printf ("ERROR: enclosingStatement_copy->get_parent() = %p = %s \n",enclosingStatement_copy->get_parent(),enclosingStatement_copy->get_parent()->class_name().c_str());
                                   printf ("ERROR: (symbol == NULL): initializedName_copy->get_name() = %s \n",initializedName_copy->get_name().str());
                                   initializedName_original->get_file_info()->display("ERROR: (symbol == NULL): debug");

                                // DQ (3/30/2014): Add this appraoch to find the symbol.
                                   SgScopeStatement* initializedName_copy_scope = isSgScopeStatement(initializedName_copy->get_scope());
                                   ROSE_ASSERT(initializedName_copy_scope != NULL);
                                   SgVariableSymbol* variableSymbol = initializedName_copy_scope->lookup_variable_symbol(initializedName_copy->get_name());
                                   ROSE_ASSERT(variableSymbol != NULL);

                                   symbol = variableSymbol;
                                 }
                              ROSE_ASSERT(symbol != NULL);

                              SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);
                              ROSE_ASSERT(variableSymbol != NULL);
#if 0
                              printf ("Insert symbol = %p for initializedName_copy = %p = %s into targetScope = %p = %s \n",variableSymbol,initializedName_copy,initializedName_copy->get_name().str(),targetScope,targetScope->class_name().c_str());
#endif
                           // DQ (3/17/2014): Build a new sysmbol to for the initializedName_copy instead of reusing the existing symbol
                           // from the snippet AST.
                              SgVariableSymbol* new_variableSymbol = new SgVariableSymbol(initializedName_copy);
                              ROSE_ASSERT(new_variableSymbol != NULL);

                           // targetScope->insert_symbol(initializedName_copy->get_name(),variableSymbol);
                              targetScope->insert_symbol(initializedName_copy->get_name(),new_variableSymbol);

                           // DQ (3/6/2014): Set the scope of the SgInitializedName IR node.
                              initializedName_copy->set_scope(targetScope);

                              SgName mangledName = variableSymbol->get_mangled_name();
#if 0
                              printf ("initializedName_copy: mangledName = %s \n",mangledName.str());
#endif
                           // DQ (3/2/2014): Make sure this is true (I think it should be, but I don't see that it was explicitly set).
                           // ROSE_ASSERT(initializedName_copy->get_scope() == targetScope);
                              if (initializedName_copy->get_scope() != targetScope)
                                 {
                                   printf ("WARNING: initializedName_copy->get_scope() != targetScope: initializedName_copy->get_scope() = %p = %s \n",initializedName_copy->get_scope(),initializedName_copy->get_scope()->class_name().c_str());

                                   printf ("I think this should be an error! \n");
                                   ROSE_ASSERT(false);
                                 }
                                 }
                            }
                       }
                  }

               break;
             }

          case V_SgVariableDeclaration:
             {
            // I think there is nothing to handle for this case (there is no type accessbile here
            // since they are in the SgInitializedName IR nodes).
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node_copy);
               ROSE_ASSERT(variableDeclaration != NULL);

               break;
             }

#define DEBUG_FUNCTION_DECLARATION 0

          case V_SgFunctionDeclaration:
             {
            // SgFunctionDeclaration is handled directly in the snippet support (insertRelatedThingsForC() function).

            // Note that function types are stored in global type tables so they need not be fixed up.

            // DQ (3/13/2014): As of today, this assumption is no longer true, we need to be able to insert
            // any function declaration in insertRelatedThingsForC() and use this function to fixup the AST.
            // The target AST should have a prototype (non-defining declaration) of the function defined
            // so that all internal types of the SgFunctionType are defined in the target AST.
               SgFunctionDeclaration* functionDeclaration_copy     = isSgFunctionDeclaration(node_copy);
               SgFunctionDeclaration* functionDeclaration_original = isSgFunctionDeclaration(node_original);
               SgFunctionType* functionType_copy     = functionDeclaration_copy->get_type();
               SgFunctionType* functionType_original = functionDeclaration_original->get_type();
               ROSE_ASSERT(functionType_copy != NULL);
               ROSE_ASSERT(functionType_original != NULL);
               ROSE_ASSERT(functionType_copy == functionType_original);
#if DEBUG_FUNCTION_DECLARATION
               printf ("case SgFunctionDeclaration: part 1: Calling functionDeclaration_copy->search_for_symbol_from_symbol_table() \n");
#endif
            // SgSymbol* symbol_copy = functionDeclaration_copy->search_for_symbol_from_symbol_table();
               SgSymbol* symbol_original = functionDeclaration_original->search_for_symbol_from_symbol_table();
               ROSE_ASSERT(symbol_original != NULL);
               SgFunctionSymbol* functionSymbol_original = isSgFunctionSymbol(symbol_original);
               ROSE_ASSERT(functionSymbol_original != NULL);

               SgFile* snippetFile = getEnclosingFileNode(functionSymbol_original);
               ROSE_ASSERT(snippetFile != NULL);
               if (snippetFile != targetFile)
                  {
#if DEBUG_FUNCTION_DECLARATION
                    printf ("Warning: case V_SgFunctionDeclaration: functionSymbol_original not in target file \n");
#endif
                 // DQ (3/13/2014): Handle the case of a member function seperately (I think this can't appear in Java, only in C++).
                 // ROSE_ASSERT(isSgMemberFunctionSymbol(symbol_copy) == NULL);
                    ROSE_ASSERT(isSgMemberFunctionSymbol(symbol_original) == NULL);

                 // printf ("case SgFunctionDeclaration: part 2: Calling functionDeclaration_copy->search_for_symbol_from_symbol_table() \n");
                 // SgFunctionSymbol* functionSymbol_copy = isSgFunctionSymbol(functionDeclaration_copy->search_for_symbol_from_symbol_table());
                 // ROSE_ASSERT(functionSymbol_copy != NULL);

                    SgName name = functionDeclaration_copy->get_name();
                    SgType* functionType = functionDeclaration_copy->get_type();
                    ROSE_ASSERT(functionType != NULL);
#if DEBUG_FUNCTION_DECLARATION
                    printf ("case V_SgFunctionDeclaration: name                  = %s \n",name.str());
                    printf ("case V_SgFunctionDeclaration: functionType          = %p \n",functionType);
                    printf ("case V_SgFunctionDeclaration: functionType_original = %p \n",functionType_original);
                    printf ("case V_SgFunctionDeclaration: functionType_copy     = %p \n",functionType_copy);
#endif
                    SgFunctionSymbol* functionSymbolInTargetAST = lookupFunctionSymbolInParentScopes(name,functionType,targetScope);

                    ROSE_ASSERT(targetScope != NULL);
                    functionDeclaration_copy->set_scope(targetScope);

                 // Set the scope of the non-defining declaration.
                    functionDeclaration_copy->get_firstNondefiningDeclaration()->set_scope(targetScope);

                    SgFunctionDeclaration* functionDeclaration_copy_firstNondefining = NULL;

                    if (functionSymbolInTargetAST == NULL)
                       {
#if DEBUG_FUNCTION_DECLARATION
                         printf ("functionSymbolInTargetAST not found in targetScope = %p = %s \n",targetScope,targetScope->class_name().c_str());
#endif
                      // If could be that the symbol is in the local scope of the snippet AST.
                         SgScopeStatement* otherPossibleScope = isSgScopeStatement(functionDeclaration_original->get_parent());
                         ROSE_ASSERT(otherPossibleScope != NULL);
#if DEBUG_FUNCTION_DECLARATION
                         printf ("case V_SgFunctionDeclaration: otherPossibleScope = %p = %s \n",otherPossibleScope,otherPossibleScope->class_name().c_str());
#endif
                      // We want to out serch the additional other scope and not it's parent scope.
                      // functionSymbolInTargetAST = lookupFunctionSymbolInParentScopes(name,functionType,otherPossibleScope);
                         functionSymbolInTargetAST = otherPossibleScope->lookup_function_symbol(name,functionType);

                         if (functionSymbolInTargetAST == NULL)
                            {
                              printf ("function symbol not found in otherPossibleScope = %p = %s \n",otherPossibleScope,otherPossibleScope->class_name().c_str());
                            }

                         ROSE_ASSERT(functionSymbolInTargetAST != NULL);
#if DEBUG_FUNCTION_DECLARATION
                         printf ("(building a new SgFunctionSymbol): functionSymbolInTargetAST->get_declaration() = %p \n",functionSymbolInTargetAST->get_declaration());
#endif
                      // DQ (3/15/2014): We need to insert a new symbol into the targetScope instead of reusing
                      // the existing symbol (because it points to the declaration in the snippet file).
                      // Insert the symbol into the targetScope.
                      // targetScope->insert_symbol(name,functionSymbolInTargetAST);
                         functionDeclaration_copy_firstNondefining     = isSgFunctionDeclaration(functionDeclaration_copy->get_firstNondefiningDeclaration());
                         SgFunctionSymbol* new_symbol = new SgFunctionSymbol(functionDeclaration_copy_firstNondefining);
                         ROSE_ASSERT(new_symbol != NULL);

                         targetScope->insert_symbol(name,new_symbol);

                         functionSymbolInTargetAST = new_symbol;

                      // DQ (3/26/2014): Added assertion.
                         ROSE_ASSERT(lookupFunctionSymbolInParentScopes(name,functionType,targetScope) != NULL);
                       }
                      else
                       {
                      // If we happend to find an associated symbol in the target scope then we nave to use it and
                      // set the first nondefining declaration pointer to the symbol's associate declaration.
                      // This is the case of the test3a test code (because the snippet functions declaration is
                      // in the target AST file (likely a mistake, but we should handle it properly).
#if DEBUG_FUNCTION_DECLARATION
                         printf ("(using existing symbol found in target scope): functionSymbolInTargetAST->get_declaration() = %p \n",functionSymbolInTargetAST->get_declaration());
#endif
                         functionDeclaration_copy_firstNondefining = isSgFunctionDeclaration(functionSymbolInTargetAST->get_declaration());
                       }

                    ROSE_ASSERT(functionSymbolInTargetAST != NULL);

                    ROSE_ASSERT(functionDeclaration_copy->get_firstNondefiningDeclaration() != NULL);
                    ROSE_ASSERT(functionDeclaration_copy_firstNondefining != NULL);

                 // SgFunctionDeclaration* functionDeclaration_copy_firstNondefining     = isSgFunctionDeclaration(functionDeclaration_copy->get_firstNondefiningDeclaration());
                    SgFunctionDeclaration* functionDeclaration_original_firstNondefining = isSgFunctionDeclaration(functionDeclaration_original->get_firstNondefiningDeclaration());
                    SgFunctionDeclaration* functionDeclaration_copy_defining     = isSgFunctionDeclaration(functionDeclaration_copy->get_definingDeclaration());
                    SgFunctionDeclaration* functionDeclaration_original_defining = isSgFunctionDeclaration(functionDeclaration_original->get_definingDeclaration());
                    if (functionDeclaration_copy_firstNondefining->get_symbol_from_symbol_table() == NULL)
                       {
                      // Check what might be wrong here!
                         ROSE_ASSERT(functionDeclaration_original_firstNondefining != NULL);
                         printf ("functionSymbolInTargetAST->get_declaration()  = %p \n",functionSymbolInTargetAST->get_declaration());

                         printf ("functionDeclaration_original = %p = %s \n",functionDeclaration_original,functionDeclaration_original->class_name().c_str());
                         printf ("functionDeclaration_copy     = %p = %s \n",functionDeclaration_copy,functionDeclaration_copy->class_name().c_str());
                         printf ("functionDeclaration_original_firstNondefining = %p \n",functionDeclaration_original_firstNondefining);
                         printf ("functionDeclaration_copy_firstNondefining     = %p \n",functionDeclaration_copy_firstNondefining);
                         printf ("functionDeclaration_original_defining         = %p \n",functionDeclaration_original_defining);
                         printf ("functionDeclaration_copy_defining             = %p \n",functionDeclaration_copy_defining);

                         printf ("functionDeclaration_original->get_scope()                  = %p = %s \n",functionDeclaration_original->get_scope(),functionDeclaration_original->get_scope()->class_name().c_str());
                         printf ("functionDeclaration_copy->get_scope()                      = %p = %s \n",functionDeclaration_copy->get_scope(),functionDeclaration_copy->get_scope()->class_name().c_str());
                         printf ("functionDeclaration_original_firstNondefining->get_scope() = %p = %s \n",functionDeclaration_original_firstNondefining->get_scope(),functionDeclaration_original_firstNondefining->get_scope()->class_name().c_str());
                         printf ("functionDeclaration_copy_firstNondefining->get_scope()     = %p = %s \n",functionDeclaration_copy_firstNondefining->get_scope(),functionDeclaration_copy_firstNondefining->get_scope()->class_name().c_str());
                         printf ("functionDeclaration_original_defining->get_scope()         = %p = %s \n",functionDeclaration_original_defining->get_scope(),functionDeclaration_original_defining->get_scope()->class_name().c_str());
                         printf ("functionDeclaration_copy_defining->get_scope()             = %p = %s \n",functionDeclaration_copy_defining->get_scope(),functionDeclaration_copy_defining->get_scope()->class_name().c_str());
                         printf ("functionSymbolInTargetAST = %p = %s \n",functionSymbolInTargetAST,functionSymbolInTargetAST->class_name().c_str());
                       }

                    ROSE_ASSERT(targetScope->lookup_function_symbol(name,functionType) != NULL);

                 // TV (10/22/2014): Might not be the case as we now have a project wide global scope
                 // ROSE_ASSERT(functionDeclaration_copy_firstNondefining->get_scope() == targetScope);

                    ROSE_ASSERT(functionDeclaration_copy_firstNondefining == functionDeclaration_copy_firstNondefining->get_firstNondefiningDeclaration());

                 // This is what is called internal to the get_symbol_from_symbol_table() function below.
                 // Use this function, SgFunctionDeclaration::get_symbol_from_symbol_table(), but not the template function: find_symbol_from_declaration().
                 // ROSE_ASSERT(targetScope->find_symbol_from_declaration(functionDeclaration_copy_firstNondefining) != NULL);

                    ROSE_ASSERT(functionDeclaration_copy_firstNondefining->get_symbol_from_symbol_table() != NULL);

#if 0
                    bool isDefiningDeclaration (functionDeclaration_original->get_declaration() != NULL);
                    if (isDefiningDeclaration == true)
                       {
                      // We may have to build a non-defining declaration.
                         SgFunctionDeclaration* nondefiningFunctionDeclaration_original = functionDeclaration_original->get_firstNondefiningDeclaration();
                         SgFile* nondefiningDeclarationFile = getEnclosingFileNode(functionSymbol_original);
                         ROSE_ASSERT(nondefiningDeclarationFile != NULL);
                         if (nondefiningDeclarationFile == targetFile)
                            {
                           // Use this nondefining declaration.
                            }
                           else
                            {
                           // Make a copy of the non-defining declaration for use in the symbol.
                            }

                       }
                      else
                       {
                      // Use this as non-defining declaration.
                       }

                    SgFile* snippetFile = getEnclosingFileNode(functionSymbol_original);
                    SgFunctionSymbol* new_function_symbol = new SgFunctionSymbol(isSgFunctionDeclaration(func));
                         ROSE_ASSERT(func_symbol->get_symbol_basis() != NULL);
                    targetScope->insert_symbol(name,symbol_original);

                    ROSE_ASSERT(false);
#endif
                  }

            // DQ (3/17/2014): Refactored code to support resetting the scopes in the SgDeclarationStatement IR nodes.
               resetDeclaration(functionDeclaration_copy,functionDeclaration_original,targetScope);
#if 0
               printf ("SageBuilder::fixupCopyOfNodeFromSeperateFileInNewTargetAst(): Need to be able to fixup the SgFunctionDeclaration \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case V_SgClassDeclaration:
             {
            // Need to handle the referenced types
               SgClassDeclaration* classDeclaration_copy     = isSgClassDeclaration(node_copy);
               SgClassDeclaration* classDeclaration_original = isSgClassDeclaration(node_original);
               SgClassType*        classType                 = classDeclaration_copy->get_type();
               ROSE_ASSERT(classType != NULL);
#if 0
               printf ("Need to handle named types from class declarations \n");
#endif
            // SgClassSymbol* classSymbol_copy = isSgClassSymbol(classDeclaration_copy->search_for_symbol_from_symbol_table());
            // ROSE_ASSERT(classSymbol_copy != NULL);

            // if (TransformationSupport::getFile(classSymbol_copy) != targetFile)
            // if (getEnclosingFileNode(classSymbol_copy) != targetFile)
            // if (getEnclosingFileNode(classDeclaration_copy) != targetFile)
                  {
                 // printf ("Warning: case V_SgClassDeclaration: classSymbol_copy not in target file \n");
#if 0
                    printf ("Warning: case V_SgClassDeclaration: assume getEnclosingFileNode(classDeclaration_copy) != targetFile \n");
#endif
                 // Find the symbol in the target scope.
                 // SgNode* insertionPointScope = (insertionPointIsScope == true) ? insertionPoint : insertionPoint->get_parent();
#if 0
                 // printf ("insertionPointIsScope = %s insertionPointScope = %p = %s \n",insertionPointIsScope ? "true" : "false",insertionPointScope,insertionPointScope->class_name().c_str());
#endif
                 // Find the nearest variable with the same name in an outer scope (starting at insertionPointScope).

                 // SgScopeStatement* targetScope = isSgScopeStatement(insertionPointScope);
                 // ROSE_ASSERT(targetScope != NULL);

                    SgName name = classDeclaration_copy->get_name();
#if 0
                 // If we randomize the names then we need to handle this case...
                    printf ("case V_SgClassDeclaration: targetScope = %p classSymbol_copy->get_name() = %s \n",targetScope,classSymbol_copy->get_name().str());
#endif
                 // SgClassSymbol* classSymbolInTargetAST = lookupClassSymbolInParentScopes(classSymbol_copy->get_name(),targetScope);
                    SgClassSymbol* classSymbolInTargetAST = lookupClassSymbolInParentScopes(name,targetScope);

                    if (classSymbolInTargetAST == NULL)
                       {
                      // If could be that the symbol is in the local scope of the snippet AST.
                         SgScopeStatement* otherPossibleScope = isSgScopeStatement(classDeclaration_original->get_parent());
                         ROSE_ASSERT(otherPossibleScope != NULL);
#if 0
                         printf ("case V_SgClassDeclaration: otherPossibleScope = %p \n",otherPossibleScope);
#endif
                      // classSymbolInTargetAST = lookupClassSymbolInParentScopes(classSymbol_copy->get_name(),otherPossibleScope);
                         classSymbolInTargetAST = lookupClassSymbolInParentScopes(name,otherPossibleScope);
                         ROSE_ASSERT(classSymbolInTargetAST != NULL);
#if 0
                      // I think this is the wrong code.
                         SgClassDeclaration* classDeclaration = classSymbolInTargetAST->get_declaration();
                         ROSE_ASSERT(classDeclaration != NULL);
                         SgScopeStatement* scope = classDeclaration->get_scope();
                         ROSE_ASSERT(scope != NULL);
                         classDeclaration_copy->set_scope(scope);
#else
                      // DQ (3/17/2014): The scope must be set to be the targetScope (at least for C, but maybe not C++).
                         classDeclaration_copy->set_scope(targetScope);
#endif
                      // DQ (3/17/2014): Build a new SgClassSymbol using the classDeclaration_copy.
                         SgClassSymbol* classSymbol = new SgClassSymbol(classDeclaration_copy);
                         ROSE_ASSERT(classSymbol != NULL);
                         classSymbolInTargetAST = classSymbol;

                      // Insert the symbol into the targetScope.
                      // targetScope->insert_symbol(classSymbol_copy->get_name(),classSymbolInTargetAST);
                         targetScope->insert_symbol(name,classSymbolInTargetAST);
                       }
                      else
                       {
                      // In this case the symbol is in a parent scope already (find the scope and set the scope of the classDeclaration_copy.
                         SgClassDeclaration* classDeclaration = classSymbolInTargetAST->get_declaration();
                         ROSE_ASSERT(classDeclaration != NULL);
                         SgScopeStatement* scope = classDeclaration->get_scope();
                         ROSE_ASSERT(scope != NULL);
                         classDeclaration_copy->set_scope(scope);
                       }

                    ROSE_ASSERT(classSymbolInTargetAST != NULL);
                  }

            // DQ (3/17/2014): Avoid calling strip type now that we have refactored the getTargetFileType() function.
            // DQ (3/10/2014): Added remaining type for this case.
            // SgType* new_type = getTargetFileType(classType->stripType(),targetScope);
               SgType* new_type = getTargetFileType(classType,targetScope);
               SgClassType* new_class_type = isSgClassType(new_type);
               if (new_class_type != NULL)
                  {
                 // Reset the base type to be the one associated with the target file.
                    classDeclaration_copy->set_type(new_class_type);
#if 0
                    printf ("case V_SgClassDeclaration: built class type: part 1: classDeclaration_copy->get_type() = %p = %s \n",
                         classDeclaration_copy->get_type(),classDeclaration_copy->get_type()->class_name().c_str());
#endif
                  }

               resetDeclaration(classDeclaration_copy,classDeclaration_original,targetScope);
#if 0
               printf ("SgClassDeclaration: Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case V_SgEnumDeclaration:
             {
            // Need to handle the referenced types
               SgEnumDeclaration* enumDeclaration_copy     = isSgEnumDeclaration(node_copy);
               SgEnumDeclaration* enumDeclaration_original = isSgEnumDeclaration(node_original);

            // SgEnumType* enumType = enumDeclaration_copy->get_type();
            // ROSE_ASSERT(enumType != NULL);

            // I don't think we have to test for this being a part of the snippet file.
                  {
                    SgName name = enumDeclaration_copy->get_name();
#if 0
                 // If we randomize the names then we need to handle this case...
                    printf ("case V_SgEnumDeclaration: targetScope = %p enumSymbol_copy->get_name() = %s \n",targetScope,name.str());
#endif
                    SgEnumSymbol* enumSymbolInTargetAST = lookupEnumSymbolInParentScopes(name,targetScope);

                    if (enumSymbolInTargetAST == NULL)
                       {
                      // If could be that the symbol is in the local scope of the snippet AST.
                         SgScopeStatement* otherPossibleScope = isSgScopeStatement(enumDeclaration_original->get_parent());
                         ROSE_ASSERT(otherPossibleScope != NULL);
#if 0
                         printf ("case V_SgEnumDeclaration: otherPossibleScope = %p \n",otherPossibleScope);
#endif
                      // I think we are not looking in the correct scope! Or else we need to also look in the target global scope.
#if 0
                         printf ("Since the symbol has not been inserted yet, what symbol are we looking for? \n");
#endif
                         enumSymbolInTargetAST = lookupEnumSymbolInParentScopes(name,otherPossibleScope);

                         ROSE_ASSERT(enumSymbolInTargetAST != NULL);
                         SgEnumDeclaration* enumDeclaration = enumSymbolInTargetAST->get_declaration();
                         ROSE_ASSERT(enumDeclaration != NULL);

                         ROSE_ASSERT(enumDeclaration != enumDeclaration_original);

                      // This is true, so we need to build a new sysmbol.
                         ROSE_ASSERT(enumSymbolInTargetAST->get_declaration() == enumDeclaration_original->get_firstNondefiningDeclaration());

                      // Build a new SgEnumSymbol using the enumDeclaration_copy.
                         SgEnumSymbol* enumSymbol = new SgEnumSymbol(enumDeclaration_copy);
                         ROSE_ASSERT(enumSymbol != NULL);
                         enumSymbolInTargetAST = enumSymbol;

                      // If this is false then we need to build a new SgEnumSymbol rather than reusing the existing one.
                         ROSE_ASSERT(enumSymbolInTargetAST->get_declaration() != enumDeclaration_original->get_firstNondefiningDeclaration());

                      // SgScopeStatement* scope = enumDeclaration->get_scope();
                         SgScopeStatement* scope = targetScope;
                         ROSE_ASSERT(scope != NULL);
                         enumDeclaration_copy->set_scope(scope);
#if 0
                         printf ("case V_SgEnumDeclaration: insert_symbol(): name = %s enumSymbolInTargetAST = %p \n",name.str(),enumSymbolInTargetAST);
#endif
                      // Insert the symbol into the targetScope.
                      // targetScope->insert_symbol(classSymbol_copy->get_name(),classSymbolInTargetAST);
                         targetScope->insert_symbol(name,enumSymbolInTargetAST);
                       }
                      else
                       {
#if 0
                         printf ("Found an existing enum declaration: name = %s enumSymbolInTargetAST = %p \n",name.str(),enumSymbolInTargetAST);
#endif
                      // In this case the symbol is in a parent scope already (find the scope and set the scope of the classDeclaration_copy.
                         SgEnumDeclaration* enumDeclaration = enumSymbolInTargetAST->get_declaration();
                         ROSE_ASSERT(enumDeclaration != NULL);
#if 0
                         SgScopeStatement* scope = enumDeclaration->get_scope();
                         ROSE_ASSERT(scope != NULL);
                         ROSE_ASSERT(scope == targetScope);
                      // enumDeclaration_copy->set_scope(scope);
#else
                      // TV (10/22/2014): Might not be the case as we now have a project wide global scope
                      // ROSE_ASSERT(enumDeclaration->get_scope() == targetScope);
#endif
                       }

                    ROSE_ASSERT(enumSymbolInTargetAST != NULL);
                  }
#if 0
               printf ("Exiting as a test 1! \n");
               ROSE_ASSERT(false);
#endif
               SgEnumType* enumType = enumDeclaration_copy->get_type();
               ROSE_ASSERT(enumType != NULL);
               SgType* new_type = getTargetFileType(enumType,targetScope);
#if 0
               printf ("Return type from getTargetFileType(): original enumType = %p new_type = %p \n",enumType,new_type);
#endif
               SgEnumType* new_enum_type = isSgEnumType(new_type);
               if (new_enum_type != NULL)
                  {
                 // Reset the base type to be the one associated with the target file.
#if 0
                    printf ("reset the type using the new enum type from the target AST \n");
#endif
                    enumDeclaration_copy->set_type(new_enum_type);
                  }
#if 0
               printf ("Exiting as a test 2! \n");
               ROSE_ASSERT(false);
#endif

               resetDeclaration(enumDeclaration_copy,enumDeclaration_original,targetScope);
               break;
             }

       // This is not a required declaration of C.
          case V_SgTemplateClassDeclaration:
             {
            // Need to handle the referenced types
               SgTemplateClassDeclaration* templateClassDeclaration = isSgTemplateClassDeclaration(node_copy);
               SgClassType* templateClassType = templateClassDeclaration->get_type();
               ROSE_ASSERT(templateClassType != NULL);

            // DQ (3/10/2014): Added support for enum types.
               SgType* new_type = getTargetFileType(templateClassType,targetScope);
               SgClassType* new_templateClass_type = isSgClassType(new_type);
               if (new_templateClass_type != NULL)
                  {
                 // Reset the base type to be the one associated with the target file.
                    templateClassDeclaration->set_type(new_templateClass_type);
#if 0
                    printf ("case V_SgTemplateClassDeclaration: built class type: part 1: templateClassDeclaration->get_type() = %p = %s \n",
                         templateClassDeclaration->get_type(),templateClassDeclaration->get_type()->class_name().c_str());
#endif
                  }

               break;
             }

          case V_SgTypedefDeclaration:
             {
            // Need to handle the referenced types (there are two for the case of a SgTypedefDeclaration).
               SgTypedefDeclaration* typedefDeclaration_copy     = isSgTypedefDeclaration(node_copy);
               SgTypedefDeclaration* typedefDeclaration_original = isSgTypedefDeclaration(node_original);

               SgType* base_type = typedefDeclaration_copy->get_base_type();
               ROSE_ASSERT(base_type != NULL);
               SgType* new_base_type = getTargetFileType(base_type,targetScope);
               if (new_base_type != NULL)
                  {
                 // Reset the base type to be the one associated with the target file.
                    typedefDeclaration_copy->set_base_type(new_base_type);
                  }

            // I don't think we have to test for this being a part of the snippet file.
                  {
                    SgName name = typedefDeclaration_copy->get_name();
#if 0
                 // If we randomize the names then we need to handle this case...
                    printf ("case V_SgTypedefDeclaration: targetScope = %p typedefSymbol_copy->get_name() = %s \n",targetScope,name.str());
#endif
                    SgTypedefSymbol* typedefSymbolInTargetAST = lookupTypedefSymbolInParentScopes(name,targetScope);

                    if (typedefSymbolInTargetAST == NULL)
                       {
                      // If could be that the symbol is in the local scope of the snippet AST.
                         SgScopeStatement* otherPossibleScope = isSgScopeStatement(typedefDeclaration_original->get_parent());
                         ROSE_ASSERT(otherPossibleScope != NULL);
#if 0
                         printf ("case V_SgTypedefDeclaration: otherPossibleScope = %p \n",otherPossibleScope);
#endif
                         typedefSymbolInTargetAST = lookupTypedefSymbolInParentScopes(name,otherPossibleScope);

                         ROSE_ASSERT(typedefSymbolInTargetAST != NULL);
                         SgTypedefDeclaration* typedefDeclaration = typedefSymbolInTargetAST->get_declaration();
                         ROSE_ASSERT(typedefDeclaration != NULL);
                      // SgScopeStatement* scope = typedefDeclaration->get_scope();
                         SgScopeStatement* scope = targetScope;
                         ROSE_ASSERT(scope != NULL);
                         typedefDeclaration_copy->set_scope(scope);

                      // DQ (3/17/2014): Build a new SgTypedefSymbol using the typedefDeclaration_copy.
                         SgTypedefSymbol* typedefSymbol = new SgTypedefSymbol(typedefDeclaration_copy);
                         ROSE_ASSERT(typedefSymbol != NULL);
                         typedefSymbolInTargetAST = typedefSymbol;
#if 0
                         printf ("case V_SgTypedefDeclaration: insert_symbol(): name = %s typedefSymbolInTargetAST = %p \n",name.str(),typedefSymbolInTargetAST);
#endif
                      // Insert the symbol into the targetScope.
                      // targetScope->insert_symbol(classSymbol_copy->get_name(),classSymbolInTargetAST);
                         targetScope->insert_symbol(name,typedefSymbolInTargetAST);
                       }
                      else
                       {
#if 0
                         printf ("Found an existing typedef declaration: name = %s typedefSymbolInTargetAST = %p \n",name.str(),typedefSymbolInTargetAST);
#endif
                      // In this case the symbol is in a parent scope already (find the scope and set the scope of the classDeclaration_copy.
                         SgTypedefDeclaration* typedefDeclaration = typedefSymbolInTargetAST->get_declaration();
                         ROSE_ASSERT(typedefDeclaration != NULL);
                         SgScopeStatement* scope = typedefDeclaration->get_scope();
                         ROSE_ASSERT(scope != NULL);
                         typedefDeclaration_copy->set_scope(scope);
                       }

                    ROSE_ASSERT(typedefSymbolInTargetAST != NULL);
                  }
#if 0
               printf ("Exiting as a test 1! \n");
               ROSE_ASSERT(false);
#endif
               SgTypedefType* typedefType = typedefDeclaration_copy->get_type();
               ROSE_ASSERT(typedefType != NULL);
               SgType* new_type = getTargetFileType(typedefType,targetScope);
               SgTypedefType* new_typedef_type = isSgTypedefType(new_type);
               if (new_typedef_type != NULL)
                  {
                 // Reset the base type to be the one associated with the target file.
#if 0
                    printf ("reset the type using the new typedef type from the target AST \n");
#endif
                    typedefDeclaration_copy->set_type(new_typedef_type);
#if 0
                    printf ("case V_SgTypedefDeclaration: built class type: part 1: typedefDeclaration_copy->get_type() = %p = %s \n",
                         typedefDeclaration_copy->get_type(),typedefDeclaration_copy->get_type()->class_name().c_str());
#endif
                  }

               resetDeclaration(typedefDeclaration_copy,typedefDeclaration_original,targetScope);
#if 0
               printf ("Exiting as a test 2! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case V_SgVarRefExp:
             {
            // Need to handle the referenced symbol.
            // but if we have handle this in the declaration for the variable (case V_SgInitializedName)
            // then we don't have to do anything here.  However, we have only handled this variable
            // declaration if the variable declaration was a part of the snippet.  If the variable
            // declaration is not a part of the original snippet (the copy of the snippet's AST that
            // we are inserting (not the snippet program where it would have to be defined for the
            // snippet to compile) then we have to find the associated variable sysmbol in the target
            // AST and reset the SgVarRefExp to use that symbol.

               SgVarRefExp* varRefExp_copy     = isSgVarRefExp(node_copy);
               SgVarRefExp* varRefExp_original = isSgVarRefExp(node_original);
               SgVariableSymbol* variableSymbol_copy = isSgVariableSymbol(varRefExp_copy->get_symbol());
               ROSE_ASSERT(variableSymbol_copy != NULL);
            // if (TransformationSupport::getFile(variableSymbol_copy) != targetFile)
               if (getEnclosingFileNode(variableSymbol_copy) != targetFile)
                  {
#if 0
                    printf ("Warning: case V_SgVarRefExp: variableSymbol not in target file: name = %s \n",variableSymbol_copy->get_name().str());
#endif
#if 0
                    printf ("insertionPoint = %p = %s \n",insertionPoint,insertionPoint->class_name().c_str());
#endif
                 // SgNode* insertionPointScope = (insertionPointIsScope == true) ? insertionPoint : insertionPoint->get_parent();
#if 0
                 // printf ("insertionPointIsScope = %s insertionPointScope = %p = %s \n",insertionPointIsScope ? "true" : "false",insertionPointScope,insertionPointScope->class_name().c_str());
#endif
                 // Find the nearest variable with the same name in an outer scope (starting at insertionPointScope).

                 // SgScopeStatement* targetScope = isSgScopeStatement(insertionPointScope);
                 // ROSE_ASSERT(targetScope != NULL);

                    SgVariableSymbol* variableSymbolInTargetAST = lookupVariableSymbolInParentScopes(variableSymbol_copy->get_name(),targetScope);

                    if (variableSymbolInTargetAST == NULL)
                       {
                      // This is a violation of the policy that the a variable with the same name will be found in the target AST.
                      // Note that if the variable could not be found then it should have been added as part of the snippet, or a
                      // previously added snippet.
#if 0
                         printf ("Error: The associated variable = %s should have been found in a parent scope of the target AST \n",variableSymbol_copy->get_name().str());
#endif
                      // We need to look into the scope of the block used to define the statments as seperate snippets (same issue as for functions).

                      // If could be that the symbol is in the local scope of the snippet AST.
                         SgStatement* enclosingStatement_original = TransformationSupport::getStatement(varRefExp_original);
                         ROSE_ASSERT(enclosingStatement_original != NULL);
#if 0
                         printf ("case V_SgVarRefExp: enclosingStatement_original = %p = %s \n",enclosingStatement_original,enclosingStatement_original->class_name().c_str());
#endif
                         SgScopeStatement* otherPossibleScope_original = isSgScopeStatement(enclosingStatement_original->get_parent());
                         ROSE_ASSERT(otherPossibleScope_original != NULL);
                      // SgFile* file = TransformationSupport::getFile(enclosingStatement_original);
                         SgFile* file = getEnclosingFileNode(enclosingStatement_original);
                         ROSE_ASSERT(file != NULL);
#if 0
                         printf ("enclosingStatement_original: associated file name = %s \n",file->get_sourceFileNameWithPath().c_str());
                      // printf ("   --- targetFile            = %p = %s \n",targetFile,targetFile->get_sourceFileNameWithPath().c_str());

                         printf ("case V_SgClassDeclaration: otherPossibleScope = %p = %s \n",otherPossibleScope_original,otherPossibleScope_original->class_name().c_str());
                         printf ("case V_SgClassDeclaration: variableSymbol_copy->get_name() = %s \n",variableSymbol_copy->get_name().str());
#endif
                         variableSymbolInTargetAST = lookupVariableSymbolInParentScopes(variableSymbol_copy->get_name(),otherPossibleScope_original);
                         if (variableSymbolInTargetAST == NULL)
                            {
#if 0
                              targetScope->get_symbol_table()->print("targetScope: symbol table");
                              otherPossibleScope_original->get_symbol_table()->print("otherPossibleScope_original: symbol table");
#endif

                           // Check for the case of a record reference (member of data structure).
                              SgExpression* parentExpression = isSgExpression(varRefExp_copy->get_parent());
                              SgBinaryOp*   parentBinaryOp   = isSgBinaryOp(parentExpression);
                              SgDotExp*     parentDotExp     = isSgDotExp(parentExpression);
                              SgArrowExp*   parentArrowExp   = isSgArrowExp(parentExpression);
                              if (parentDotExp != NULL || parentArrowExp != NULL)
                                 {
                                // This is a data member reference, so it's scope is the associated data structure.
                                   SgExpression* lhs = parentBinaryOp->get_lhs_operand();
                                   ROSE_ASSERT(lhs != NULL);
                                   ROSE_ASSERT(parentBinaryOp->get_rhs_operand() == varRefExp_copy);

                                   SgType* type = lhs->get_type();
                                   ROSE_ASSERT(type != NULL);
#if 0
                                   printf ("type = %p = %s \n",type,type->class_name().c_str());
#endif
                                   SgNamedType* namedType = isSgNamedType(type);
                                   ROSE_ASSERT(namedType != NULL);
                                   SgDeclarationStatement* declaration = namedType->get_declaration();
                                   ROSE_ASSERT(declaration != NULL);
                                   SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                                   ROSE_ASSERT(classDeclaration != NULL);
                                   SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(declaration->get_definingDeclaration());
                                   ROSE_ASSERT(definingClassDeclaration != NULL);
                                   SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                                   ROSE_ASSERT(classDefinition != NULL);
#if 0
                                   printf ("case V_SgClassDeclaration: classDefinition = %p = %s \n",classDefinition,classDefinition->class_name().c_str());
#endif
                                // I think we want the copy.
                                   otherPossibleScope_original = classDefinition;

                                   variableSymbolInTargetAST = lookupVariableSymbolInParentScopes(variableSymbol_copy->get_name(),otherPossibleScope_original);
                                 }

                            }
                         ROSE_ASSERT(variableSymbolInTargetAST != NULL);
                         SgInitializedName* initializedName = variableSymbolInTargetAST->get_declaration();
                         ROSE_ASSERT(initializedName != NULL);
                         SgScopeStatement* scope = initializedName->get_scope();
                         ROSE_ASSERT(scope != NULL);

                      // Is this the correct scope?
                         initializedName->set_scope(scope);

                      // Insert the symbol into the targetScope.
                         targetScope->insert_symbol(variableSymbol_copy->get_name(),variableSymbolInTargetAST);
                       }
                    ROSE_ASSERT(variableSymbolInTargetAST != NULL);

                 // Reset the symbol associated with this variable reference.
                    varRefExp_copy->set_symbol(variableSymbolInTargetAST);

                 // printf ("Exiting as a test! \n");
                 // ROSE_ASSERT(false);
                  }

               break;
             }

          case V_SgFunctionRefExp:
             {
            // Need to handle the referenced symbol
               SgFunctionRefExp* functionRefExp_copy     = isSgFunctionRefExp(node_copy);
            // SgFunctionRefExp* functionRefExp_original = isSgFunctionRefExp(node_original);
               SgFunctionSymbol* functionSymbol_copy = isSgFunctionSymbol(functionRefExp_copy->get_symbol());
               ROSE_ASSERT(functionSymbol_copy != NULL);
            // if (TransformationSupport::getFile(functionSymbol) != targetFile)
               if (getEnclosingFileNode(functionSymbol_copy) != targetFile)
                  {
#if 0
                    printf ("Warning: case V_SgFunctionRefExp: functionSymbol_copy not in target file (find function = %s) \n",functionSymbol_copy->get_name().str());
#endif
                 // SgNode* insertionPointScope = (insertionPointIsScope == true) ? insertionPoint : insertionPoint->get_parent();
#if 0
                    printf ("insertionPointIsScope = %s insertionPointScope = %p = %s \n",insertionPointIsScope ? "true" : "false",insertionPointScope,insertionPointScope->class_name().c_str());
#endif
                 // Find the nearest variable with the same name in an outer scope (starting at insertionPointScope).

                 // SgScopeStatement* targetScope = isSgScopeStatement(insertionPointScope);
                 // ROSE_ASSERT(targetScope != NULL);

                    SgName name = functionSymbol_copy->get_name();

                 // I think we need the function's mangled name to support this lookup.
                    SgFunctionSymbol* functionSymbolInTargetAST = lookupFunctionSymbolInParentScopes(name,targetScope);

                    if (functionSymbolInTargetAST == NULL)
                       {
                      // DQ (3/17/2014): Revised as of further discussion about how the snippet mechanism will copy required
                      // declaration from the snippet file to the target AST.
                      // This is a violation of the policy that the a variable with the same name will be found in the target AST.
                      // Note that if the variable could not be found then it should have been added as part of the snippet, or a
                      // previously added snippet.
                      // DQ (3/17/2014): After some revision of the specification for the snippet injection, this is still
                      // an error since this is the case where a declaration should have been visible from having already been
                      // inserted into the target AST and this visible from this injection point in the target AST.

                         fprintf (stderr, "Error: The associated function = \"%s\" should have been found in a parent scope"
                                  " of the target AST\n", name.str());

                         fprintf (stderr, "  targetScope = %p = %s \n",targetScope,targetScope->class_name().c_str());
                         SgGlobal* globalScope = TransformationSupport::getGlobalScope(targetScope);
                         ROSE_ASSERT(globalScope != NULL);
                         fprintf (stderr, "  globalScope = %p = %s \n",globalScope,globalScope->class_name().c_str());
#if 0
                         targetScope->get_file_info()->display("case V_SgFunctionRefExp: targetScope: debug");
                         node_original->get_file_info()->display("case V_SgFunctionRefExp: node_original: debug");
#endif
#if 0
                      // DQ (3/10/2014): This might be important for friend functions in C++ (but we can ignore it for now).
#error "DEAD CODE!"
                      // If could be that the symbol is in the local scope of the snippet AST.
                         SgStatement* enclosingStatement_original = TransformationSupport::getStatement(functionRefExp_original);
                         ROSE_ASSERT(enclosingStatement_original != NULL);
#if 0
                         printf ("case V_SgFunctionRefExp: enclosingStatement_original = %p = %s \n",enclosingStatement_original,enclosingStatement_original->class_name().c_str());
#endif
                         SgScopeStatement* otherPossibleScope_original = isSgScopeStatement(enclosingStatement_original->get_parent());
                         ROSE_ASSERT(otherPossibleScope_original != NULL);
                      // SgFile* file = TransformationSupport::getFile(enclosingStatement_original);
                         SgFile* file = getEnclosingFileNode(enclosingStatement_original);
                         ROSE_ASSERT(file != NULL);
#if 0
                         printf ("enclosingStatement_original: associated file name = %s \n",file->get_sourceFileNameWithPath().c_str());
                      // printf ("   --- targetFile            = %p = %s \n",targetFile,targetFile->get_sourceFileNameWithPath().c_str());

                         printf ("case V_SgClassDeclaration: otherPossibleScope = %p = %s \n",otherPossibleScope_original,otherPossibleScope_original->class_name().c_str());
                         printf ("case V_SgClassDeclaration: functionSymbol_copy->get_name() = %s \n",functionSymbol_copy->get_name().str());
#endif
                         functionSymbolInTargetAST = lookupFunctionSymbolInParentScopes(functionSymbol_copy->get_name(),otherPossibleScope_original);
#error "DEAD CODE!"
                         if (functionSymbolInTargetAST == NULL)
                            {
                           // Check for the case of a record reference (member function of class declaration).
                              SgExpression* parentExpression = isSgExpression(functionRefExp_copy->get_parent());
                              SgBinaryOp*   parentBinaryOp   = isSgBinaryOp(parentExpression);
                              SgDotExp*     parentDotExp     = isSgDotExp(parentExpression);
                              SgArrowExp*   parentArrowExp   = isSgArrowExp(parentExpression);
                              if (parentDotExp != NULL || parentArrowExp != NULL)
                                 {
                                // This is a data member reference, so it's scope is the associated data structure.
                                   SgExpression* lhs = parentBinaryOp->get_lhs_operand();
                                   ROSE_ASSERT(lhs != NULL);
                                   ROSE_ASSERT(parentBinaryOp->get_rhs_operand() == functionRefExp_copy);
#error "DEAD CODE!"
                                   SgType* type = lhs->get_type();
                                   ROSE_ASSERT(type != NULL);
#if 0
                                   printf ("type = %p = %s \n",type,type->class_name().c_str());
#endif
                                   SgNamedType* namedType = isSgNamedType(type);
                                   ROSE_ASSERT(namedType != NULL);
                                   SgDeclarationStatement* declaration = namedType->get_declaration();
                                   ROSE_ASSERT(declaration != NULL);
                                   SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                                   ROSE_ASSERT(classDeclaration != NULL);
                                   SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(declaration->get_definingDeclaration());
                                   ROSE_ASSERT(definingClassDeclaration != NULL);
                                   SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                                   ROSE_ASSERT(classDefinition != NULL);
#if 0
                                   printf ("case V_SgClassDeclaration: classDefinition = %p = %s \n",classDefinition,classDefinition->class_name().c_str());
#endif
                                // I think we want the copy.
                                   otherPossibleScope_original = classDefinition;

                                   functionSymbolInTargetAST = lookupFunctionSymbolInParentScopes(functionSymbol_copy->get_name(),otherPossibleScope_original);
                                 }
                            }

#error "DEAD CODE!"
                         ROSE_ASSERT(functionSymbolInTargetAST != NULL);
                         SgFunctionDeclaration* functionDeclaration = functionSymbolInTargetAST->get_declaration();
                         ROSE_ASSERT(functionDeclaration != NULL);
                         SgScopeStatement* scope = functionDeclaration->get_scope();
                         ROSE_ASSERT(scope != NULL);

                      // Is this the correct scope?
                         functionDeclaration->set_scope(scope);
#endif
                         ROSE_ASSERT(functionSymbolInTargetAST != NULL);

                      // Insert the symbol into the targetScope.
                         targetScope->insert_symbol(functionSymbol_copy->get_name(),functionSymbolInTargetAST);
                       }
                    ROSE_ASSERT(functionSymbolInTargetAST != NULL);

                 // Reset the symbol associated with this function reference.
                    functionRefExp_copy->set_symbol(functionSymbolInTargetAST);
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               break;
             }

#define DEBUG_MEMBER_FUNCTION_REF_EXP 0

          case V_SgMemberFunctionRefExp:
             {
            // Need to handle the referenced symbol
               SgMemberFunctionRefExp* memberFunctionRefExp_copy = isSgMemberFunctionRefExp(node_copy);
               SgMemberFunctionRefExp* memberFunctionRefExp_original = isSgMemberFunctionRefExp(node_original);
               SgMemberFunctionSymbol* memberFunctionSymbol_copy = isSgMemberFunctionSymbol(memberFunctionRefExp_copy->get_symbol());
               ROSE_ASSERT(memberFunctionSymbol_copy != NULL);
            // if (TransformationSupport::getFile(memberFunctionSymbol) != targetFile)
               if (getEnclosingFileNode(memberFunctionSymbol_copy) != targetFile)
                  {
                 // Not implemented (initial work is focused on C, then Java, then C++.
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                    printf ("Warning: case V_SgMemberFunctionRefExp: memberFunctionSymbol_copy not in target file (find member function = %s) \n",memberFunctionSymbol_copy->get_name().str());
#endif
                    SgMemberFunctionSymbol* memberFunctionSymbolInTargetAST = isSgMemberFunctionSymbol(lookupFunctionSymbolInParentScopes(memberFunctionSymbol_copy->get_name(),targetScope));

                    if (memberFunctionSymbolInTargetAST == NULL)
                       {
                      // This is a violation of the policy that the a variable with the same name will be found in the target AST.
                      // Note that if the variable could not be found then it should have been added as part of the snippet, or a
                      // previously added snippet.
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                         printf ("Error: The associated memberFunction_copy = %s should have been found in a parent scope of the target AST \n",memberFunctionSymbol_copy->get_name().str());
#endif
                      // DQ (3/10/2014): This is important for member functions in Java and C++.

                      // If could be that the symbol is in the local scope of the snippet AST.
                         SgStatement* enclosingStatement_original = TransformationSupport::getStatement(memberFunctionRefExp_original);
                         ROSE_ASSERT(enclosingStatement_original != NULL);
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                         printf ("case V_SgMemberFunctionRefExp: enclosingStatement_original = %p = %s \n",enclosingStatement_original,enclosingStatement_original->class_name().c_str());
#endif
                         SgScopeStatement* otherPossibleScope_original = isSgScopeStatement(enclosingStatement_original->get_parent());
                         ROSE_ASSERT(otherPossibleScope_original != NULL);
                      // SgFile* file = TransformationSupport::getFile(enclosingStatement_original);
                         SgFile* file = getEnclosingFileNode(enclosingStatement_original);
                         ROSE_ASSERT(file != NULL);
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                         printf ("enclosingStatement_original: associated file name = %s \n",file->get_sourceFileNameWithPath().c_str());
                      // printf ("   --- targetFile            = %p = %s \n",targetFile,targetFile->get_sourceFileNameWithPath().c_str());

                         printf ("case V_SgClassDeclaration: otherPossibleScope = %p = %s \n",otherPossibleScope_original,otherPossibleScope_original->class_name().c_str());
                         printf ("case V_SgClassDeclaration: memberFunctionSymbol_copy->get_name() = %s \n",memberFunctionSymbol_copy->get_name().str());
#endif
                         memberFunctionSymbolInTargetAST = isSgMemberFunctionSymbol(lookupFunctionSymbolInParentScopes(memberFunctionSymbol_copy->get_name(),otherPossibleScope_original));
                         if (memberFunctionSymbolInTargetAST == NULL)
                            {
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                              printf ("Backup and look for the associated class and then look for the member function in the class (assume non-friend function or Java member function) \n");
#endif
                           // Check for the case of a record reference (member function of class declaration).
                              SgExpression* parentExpression = isSgExpression(memberFunctionRefExp_copy->get_parent());

                              ROSE_ASSERT(parentExpression != NULL);
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                              printf ("parentExpression = %p = %s \n",parentExpression,parentExpression->class_name().c_str());
#endif
                              bool handle_as_java = false;
                              SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(parentExpression);
                              if (functionCallExp != NULL)
                                 {
                                // Note that this is a Java specific organization of the SgMemberFunctionRefExp and the SgFunctionCallExp.
                                // We might want to make this more uniform between C++ and Java later.
                                   handle_as_java = true;

                                   SgExpression* parentOfFunctionCallExpression = isSgExpression(functionCallExp->get_parent());

                                   ROSE_ASSERT(parentOfFunctionCallExpression != NULL);
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                                   printf ("parentOfFunctionCallExpression = %p = %s \n",parentOfFunctionCallExpression,parentOfFunctionCallExpression->class_name().c_str());
#endif
                                   parentExpression = parentOfFunctionCallExpression;
                                 }

                              SgBinaryOp*   parentBinaryOp   = isSgBinaryOp(parentExpression);
                              SgDotExp*     parentDotExp     = isSgDotExp(parentExpression);
                              SgArrowExp*   parentArrowExp   = isSgArrowExp(parentExpression);
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                              printf ("parentBinaryOp = %p \n",parentBinaryOp);
                              printf ("parentDotExp   = %p \n",parentDotExp);
                              printf ("parentArrowExp = %p \n",parentArrowExp);
#endif
                              if (parentDotExp != NULL || parentArrowExp != NULL)
                                 {
                                // This is a data member reference, so it's scope is the associated data structure.
                                   SgExpression* lhs = parentBinaryOp->get_lhs_operand();
                                   ROSE_ASSERT(lhs != NULL);

                                // This will be true for C++, but not Java (a little odd).
                                   if (handle_as_java == true)
                                      {
                                     // The rhs is the SgFunctionCallExp for Java.
                                        ROSE_ASSERT(parentBinaryOp->get_rhs_operand() == functionCallExp);
                                      }
                                     else
                                      {
                                     // This is what we expect to be true for C++.
                                        ROSE_ASSERT(parentBinaryOp->get_rhs_operand() == memberFunctionRefExp_copy);
                                      }
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                                   printf ("lhs = %p = %s \n",lhs,lhs->class_name().c_str());
#endif
                                   SgVarRefExp* varRefExp = isSgVarRefExp(lhs);

                                // DQ (3/15/2014): This can be a SgJavaTypeExpression (see testJava3a).
                                // ROSE_ASSERT(varRefExp != NULL);
                                   if (varRefExp != NULL)
                                      {
                                        SgVariableSymbol* variableSymbol = isSgVariableSymbol(varRefExp->get_symbol());
                                        ROSE_ASSERT(variableSymbol != NULL);
                                        SgInitializedName* initializedName = variableSymbol->get_declaration();
                                        ROSE_ASSERT(initializedName != NULL);

                                        SgType* initializedName_type = initializedName->get_type();
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                                        printf ("initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
                                        printf ("initializedName_type = %p \n",initializedName_type);
#endif
                                        SgClassType* classType = isSgClassType(initializedName_type);
                                        if (classType != NULL)
                                           {
                                             SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                                             ROSE_ASSERT(classDeclaration != NULL);
                                             SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
                                             ROSE_ASSERT(definingClassDeclaration != NULL);
                                             printf ("definingClassDeclaration->get_name() = %s \n",definingClassDeclaration->get_name().str());

                                             SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                                             ROSE_ASSERT(classDefinition != NULL);
                                             SgType* memberFunctionType = memberFunctionSymbol_copy->get_type();
                                             SgName memberFunctionName  = memberFunctionSymbol_copy->get_name();
                                             ROSE_ASSERT(memberFunctionType != NULL);
                                             SgFunctionSymbol *functionSymbol = classDefinition->lookup_function_symbol(memberFunctionName,memberFunctionType);
                                             if (functionSymbol == NULL)
                                                {
                                                  printf ("Symbol not found: output symbol table (size = %d): \n",classDefinition->get_symbol_table()->size());
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                                                  classDefinition->get_symbol_table()->print("Symbol not found: output symbol table: SgClassDefinition");
#endif
                                               // DQ (3/30/2014): If functionSymbol is not found then I think it is because the class was not availalbe
                                               // in the target where the snippet is being copied.  This is an error in the constrains for how the target
                                               // must be prepared for the snippet to be copied into it.
                                                  printf ("\n*************************************************************** \n");
                                                  printf ("ERROR: target has not be properly setup to receive the snippet. \n");
                                                  printf ("*************************************************************** \n");
                                                }
                                             ROSE_ASSERT(functionSymbol != NULL);
                                             SgMemberFunctionSymbol *memberFunctionSymbol = isSgMemberFunctionSymbol(functionSymbol);
                                             ROSE_ASSERT(memberFunctionSymbol != NULL);

                                             memberFunctionSymbolInTargetAST = memberFunctionSymbol;
#if 0
                                             printf ("Exiting as a test! \n");
                                             ROSE_ASSERT(false);
#endif
                                           }
                                      }

                                // DQ (3/30/2014): If this is a value expression then calling the member function uses a shared
                                // symbol from the global scope (or a type defined deep in the global scope, but common to the
                                // snippet AST and the target AST).
                                   SgValueExp* valueExp = isSgValueExp(lhs);
                                   if (valueExp != NULL)
                                      {
                                        memberFunctionSymbolInTargetAST = memberFunctionSymbol_copy;
                                        ROSE_ASSERT(memberFunctionSymbolInTargetAST != NULL);
                                      }

                                   if (memberFunctionSymbolInTargetAST == NULL)
                                      {
#if 1
                                        SgType* type = lhs->get_type();
                                        ROSE_ASSERT(type != NULL);
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                                        printf ("type = %p = %s \n",type,type->class_name().c_str());
#endif
                                        SgNamedType* namedType = isSgNamedType(type);
                                        ROSE_ASSERT(namedType != NULL);
                                        SgDeclarationStatement* declaration = namedType->get_declaration();
                                        ROSE_ASSERT(declaration != NULL);
                                        SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                                        ROSE_ASSERT(classDeclaration != NULL);
                                        SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(declaration->get_definingDeclaration());
                                        ROSE_ASSERT(definingClassDeclaration != NULL);
                                        SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                                        ROSE_ASSERT(classDefinition != NULL);
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                                        printf ("case V_SgClassDeclaration: classDefinition = %p = %s \n",classDefinition,classDefinition->class_name().c_str());
#endif
                                     // I think we want the copy.
                                        otherPossibleScope_original = classDefinition;
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                                        classDefinition->get_symbol_table()->print("Java classDefinition");
#endif
#if DEBUG_MEMBER_FUNCTION_REF_EXP
                                        SgClassDeclaration* associated_classDeclaration = classDefinition->get_declaration();
                                        SgFunctionSymbol* functionSymbol = lookupFunctionSymbolInParentScopes(memberFunctionSymbol_copy->get_name(),otherPossibleScope_original);
                                        printf ("associated_classDeclaration = %p name = %s \n",associated_classDeclaration,associated_classDeclaration->get_name().str());
                                        printf ("functionSymbol = %p \n",functionSymbol);
#endif
                                        memberFunctionSymbolInTargetAST = isSgMemberFunctionSymbol(lookupFunctionSymbolInParentScopes(memberFunctionSymbol_copy->get_name(),otherPossibleScope_original));
                                        if (memberFunctionSymbolInTargetAST == NULL)
                                           {
                                          // Output debugging info (16 of the CWE injection test codes fail here: see test_results.txt file for details).
                                             printf ("Error: (memberFunctionSymbolInTargetAST == NULL): memberFunctionSymbol_copy->get_name() = %s \n",memberFunctionSymbol_copy->get_name().str());
                                           }
#endif
                                        ROSE_ASSERT(memberFunctionSymbolInTargetAST != NULL);
                                      }
                                    ROSE_ASSERT(memberFunctionSymbolInTargetAST != NULL);
                                 }
                            }

                         ROSE_ASSERT(memberFunctionSymbolInTargetAST != NULL);
                         SgFunctionDeclaration* functionDeclaration = memberFunctionSymbolInTargetAST->get_declaration();
                         ROSE_ASSERT(functionDeclaration != NULL);
                         SgScopeStatement* scope = functionDeclaration->get_scope();
                         ROSE_ASSERT(scope != NULL);

                      // Is this the correct scope?
                         functionDeclaration->set_scope(scope);
                       }
                    ROSE_ASSERT(memberFunctionSymbolInTargetAST != NULL);

                 // Reset the symbol associated with this function reference.
                    memberFunctionRefExp_copy->set_symbol(memberFunctionSymbolInTargetAST);
                  }

               break;
             }

       // DQ (3/21/2014): I think we need this.
          case V_SgTryStmt:
             {
#if 0
               printf ("Exiting as a test! (SgTryStmt) \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // DQ (3/19/2014): Just found this case in a few of the CWE Java snippet tests.
          case V_SgCatchStatementSeq:
             {
            // DQ (3/19/2014): Note sure that we need to handle this specific case.

#if 0
               printf ("Exiting as a test! (SgCatchStatementSeq) \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // DQ (3/19/2014): Just found this case in a few of the CWE Java snippet tests.
          case V_SgCatchOptionStmt:
             {
            // DQ (3/19/2014): Note sure that we need to handle this specific case.
            // Decide if we need to implement this newly identified case tomorrow (note that this is a SgScopeStatement).
               SgCatchOptionStmt* catchOptionStatement_copy     = isSgCatchOptionStmt(node_copy);
               ROSE_ASSERT(catchOptionStatement_copy);

               printf ("Need to check the symbol table of the SgCatchOptionStmt (which is a SgScopeStatement) \n");

#if 0
               printf ("Exiting as a test! (SgCatchOptionStmt) \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // DQ (3/21/2014): I think we need this.
          case V_SgJavaPackageStatement:
             {
#if 1
               printf ("Exiting as a test! (SgJavaPackageStatement) \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case V_SgEnumVal:
             {
            // SgEnumVal expressions contain a reference to the associated SgEnumDeclaration, so this may have to be updated.
#if 0
               printf ("enum values contain a reference to the associated SgEnumDeclaration \n");
#endif
               SgEnumVal* enumVal_copy     = isSgEnumVal(node_copy);
               SgEnumVal* enumVal_original = isSgEnumVal(node_original);
#if 0
               printf ("   --- enumVal_original = %p = %d name = %s \n",enumVal_original,enumVal_original->get_value(),enumVal_original->get_name().str());
#endif
               SgEnumDeclaration* associatedEnumDeclaration_copy     = isSgEnumDeclaration(enumVal_copy->get_declaration());
               SgEnumDeclaration* associatedEnumDeclaration_original = isSgEnumDeclaration(enumVal_original->get_declaration());

            // DQ (4/13/2014): check if this is an un-named enum beclaration.
               bool isUnNamed = associatedEnumDeclaration_original->get_isUnNamed();
               if (isUnNamed == false)
                  {
                    if (associatedEnumDeclaration_copy == associatedEnumDeclaration_original)
                       {
#if 0
                         printf ("   --- The stored reference to the enum declaration in the SgEnumVal must be reset \n");
#endif
                      // SgSymbol* SageBuilder::findAssociatedSymbolInTargetAST(SgDeclarationStatement* snippet_declaration, SgScopeStatement* targetScope)
                         SgSymbol* symbol = findAssociatedSymbolInTargetAST(associatedEnumDeclaration_original,targetScope);
                         if (symbol == NULL)
                            {
                           // debug this case.
                              enumVal_original->get_file_info()->display("case V_SgEnumVal: symbol == NULL: debug");
                            }
                         ROSE_ASSERT(symbol != NULL);
                         SgEnumSymbol* enumSymbol = isSgEnumSymbol(symbol);
                         ROSE_ASSERT(enumSymbol != NULL);
                         SgEnumDeclaration* new_associatedEnumDeclaration_copy = enumSymbol->get_declaration();
                         ROSE_ASSERT(new_associatedEnumDeclaration_copy != NULL);

                      // If this is false then in means that we should have built a new SgEnumSymbol instead of reusing the existing one from the snippet.
                         ROSE_ASSERT(new_associatedEnumDeclaration_copy != associatedEnumDeclaration_original);// TV (10/22/2014): with project wide global scope this will always be false because 'symbol' is resolve to the first-non-defn-decl in original scope
                      // ROSE_ASSERT(new_associatedEnumDeclaration_copy != associatedEnumDeclaration_original->get_firstNondefiningDeclaration());
                         ROSE_ASSERT(new_associatedEnumDeclaration_copy != associatedEnumDeclaration_original->get_definingDeclaration());

                         enumVal_copy->set_declaration(new_associatedEnumDeclaration_copy);
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }
                 else
                  {
                 // DQ (4/13/2014): I think we all agreed these would not have to be handled, so issue a warning.
                 // It still is likely that I can allow this, but permit this intermediate fix.
                    printf ("Warning: can't handle enum values from unnamed enum declarations \n");
                    printf ("   --- enumVal_original = %p = %d name = %s \n",enumVal_original,enumVal_original->get_value(),enumVal_original->get_name().str());
                  }

               break;
             }

          default:
             {
            // Most IR nodes do not require specialized fixup (are not processed).
             }
        }

#if 1
  // DQ (3/17/2014): Cause failure on warnings about any constructs referencing the snippet AST.
#if 0
  // Assert fail on warnings.
     errorCheckingTargetAST(node_copy,node_original,targetFile, true);
#else
  // Cause only warnings.
     errorCheckingTargetAST(node_copy,node_original,targetFile, false);
#endif
#endif
   }


void
SageBuilder::fixupCopyOfAstFromSeparateFileInNewTargetAst(SgStatement *insertionPoint, bool insertionPointIsScope,
                                                          SgStatement *toInsert, SgStatement* original_before_copy)
   {
  // The semantics of the copy is that it will have been disconnected from the snippet AST in a few ways,
  // Namely the root of the copy of the snippet's AST will have been set with a NULL parent, and then
  // the parent would have been reset when the copy of the snippet was inserted into the target AST.
  // So a simple traversal of parents back to the SgFile will return the target AST's SgFile (confirmed below).

     ROSE_ASSERT(insertionPoint != NULL);
     ROSE_ASSERT(toInsert != NULL);
     ROSE_ASSERT(original_before_copy != NULL);

  // DQ (3/30/2014): Turn this on to support finding symbols in base classes (in Java).
  // Will be turned off at the base of this function (since we only only want to use it for the AST fixup, currently).
     SgSymbolTable::set_force_search_of_base_classes(true);

  // DQ (3/4/2014): Switch to using the SageInterface function.
  // SgFile* targetFile = TransformationSupport::getFile(insertionPoint);
     SgFile* targetFile = getEnclosingFileNode(insertionPoint);

  // For Java support this might be NULL, if the insertion point was in global scope.
     ROSE_ASSERT(targetFile != NULL);

  // SgFile* snippetFile_of_copy = TransformationSupport::getFile(toInsert);
     SgFile* snippetFile_of_copy = getEnclosingFileNode(toInsert);

  // At this point the parent pointers are set so that the same SgFile is found via a traversal back to the SgProject.
  // Confirm that the SgFile found by a traversal of parents in the copy of rthe snippet's AST will return that of the
  // SgFile for the target AST.  This also confirms that the copy of the snippet has already been inserted into the
  // target AST.
     ROSE_ASSERT(snippetFile_of_copy == targetFile);

  // SgFile* snippetFile_of_original = TransformationSupport::getFile(original_before_copy);
     SgFile* snippetFile_of_original = getEnclosingFileNode(original_before_copy);

     ROSE_ASSERT(snippetFile_of_original != targetFile);

#if 0
     printf ("   --- targetFile              = %p = %s \n",targetFile,targetFile->get_sourceFileNameWithPath().c_str());
     printf ("   --- snippetFile_of_copy     = %p = %s \n",snippetFile_of_copy,snippetFile_of_copy->get_sourceFileNameWithPath().c_str());
     printf ("   --- snippetFile_of_original = %p = %s \n",snippetFile_of_original,snippetFile_of_original->get_sourceFileNameWithPath().c_str());
#endif

  // Any node that has entries not referenced in the target file needs to be fixed up.
  // We can assume that any referenced variable or function that is referenced in the
  // snippet will exist in either the snippet or the target file.

  // DQ (3/4/2014): This is a test of the structural equality of the original snippet and it's copy.
  // If they are different then we can't support fixing up the AST.  Transformations on the snippet
  // should have been made after insertion into the AST.  The complexity of this test is a traversal
  // of the copy of the snippet to be inserted (typically very small compared to the target application).
     bool isStructurallyEquivalent = isStructurallyEquivalentAST(toInsert,original_before_copy);
     if (isStructurallyEquivalent == false)
        {
          printf ("WARNING: The copy of the snippet is a different size than the original snippet (don't do transformations on the copy before inserting into the target AST). \n");
          return;
        }
     ROSE_ASSERT(isStructurallyEquivalent == true);

#ifndef USE_CMAKEx
  // DQ (3/8/2014): Make this conditionally compiled based on when CMake is not used because the libraries are not configured yet.

  // This is AST container for the ROSE AST that will provide an iterator.
  // We want two iterators (one for the copy of the snippet and one for the
  // original snippet so that we can query the original snippet's AST
  // as we process each IR node of the AST for the copy of the snippet.
  // Only the copy of the snippet is inserted into the target AST.
     RoseAst ast_of_copy(toInsert);
     RoseAst ast_of_original(original_before_copy);

  // printf ("ast_of_copy.size() = %" PRIuPTR " \n",ast_of_copy.size());

  // Build the iterators so that we can increment thorugh both ASTs one IR node at a time.
     RoseAst::iterator i_copy     = ast_of_copy.begin();
     RoseAst::iterator i_original = ast_of_original.begin();

  // Iterate of the copy of the snippet's AST.
     while (i_copy != ast_of_copy.end())
        {
       // DQ (2/28/2014): This is a problem for some of the test codes (TEST   store/load heap string [test7a] and [test7a])
       // ROSE_ASSERT((*i_copy)->variantT() == (*i_original)->variantT());
          if ((*i_copy)->variantT() != (*i_original)->variantT())
             {
               printf ("ERROR: return from fixupCopyOfAstFromSeparateFileInNewTargetAst(): "
                       "(*i_copy)->variantT() != (*i_original)->variantT() \n");
#if 1
               printf ("Making this an error! \n");
               ROSE_ASSERT(false);
#endif
               return;
             }

       // Operate on individual IR nodes.
          fixupCopyOfNodeFromSeparateFileInNewTargetAst(insertionPoint, insertionPointIsScope, *i_copy, *i_original);

          i_copy++;

       // Verify that we have not reached the end of the ast for the original (both the
       // copy and the original are the same structurally, and thus the same size).
          ROSE_ASSERT(i_original != ast_of_original.end());
          i_original++;
        }

  // We have reached the end of both ASTs.
     ROSE_ASSERT(i_copy == ast_of_copy.end() && i_original == ast_of_original.end());

  // DQ (3/8/2014): ENDIF: Make this conditionally compiled based on when CMake is not used because the libraries are not configured yet.
#endif

#if 0
     if (functionDeclaration != NULL)
        {
          printf ("functionDeclaration = %s \n",functionDeclaration->get_name().str());
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

  // DQ (3/30/2014): Turn this off (since we only only want to use it for the AST fixup, currently).
     SgSymbolTable::set_force_search_of_base_classes(false);
   }

// Liao 9/18/2015
// The parser is implemented in
// src/frontend/SageIII/astFromString/AstFromString.h .C
SgStatement* SageBuilder::buildStatementFromString(const std::string& s, SgScopeStatement * scope)
{

  SgStatement* result = NULL;
  ROSE_ASSERT (scope != NULL);
  // set input and context for the parser
  AstFromString::c_char = s.c_str();
  assert (AstFromString::c_char== s.c_str());
  AstFromString::c_sgnode = scope;
  AstFromString::c_parsed_node = NULL;

  if (AstFromString::afs_match_statement())
  {
    result = isSgStatement(AstFromString::c_parsed_node); // grab the result
    assert (result != NULL);
  }
  else
  {
    cerr<<"Error. buildStatementFromString() cannot parse the input string:"<<s
        <<"\n\t within the given scope:"<<scope->class_name() <<endl;
    ROSE_ASSERT(false);
  }
  return result;
}


//
// pp (07/16/16)
// initial support for creating template instantiations
// from template declarations

namespace {
  // internal functions

  template <class SgAstNode>
  SgTemplateArgument* createTemplateArg_(SgAstNode& n)
  {
    static const bool explicitlySpecified = true;

    return new SgTemplateArgument(&n, explicitlySpecified);
  }

  SgTemplateArgument* createTemplateArg_(SgExpression& n)
  {
    SgTemplateArgument* res = createTemplateArg_<SgExpression>(n);

    n.set_parent(res);
    return res;
  }

  SgTemplateArgument* createTemplateArg(SgNode& n)
  {
    SgTemplateArgument* res = NULL;

    if (isSgType(&n))
      res = createTemplateArg_(*isSgType(&n));
    else if (isSgExpression(&n))
      res = createTemplateArg_(*isSgExpression(&n));
    else
    {
      ROSE_ASSERT(isSgTemplateDeclaration(&n));
      res = createTemplateArg_(*isSgTemplateDeclaration(&n));
    }

    ROSE_ASSERT(res);
    return res;
  }
#if 0
  SgName genTemplateName(SgName base, Rose_STL_Container<SgNode*>& targs)
  {
    Rose_STL_Container<SgNode*>::iterator aa = targs.begin();
    Rose_STL_Container<SgNode*>::iterator zz = targs.begin();
    std::string                           name(base.getString());

    name.append("<");
    for ( ; aa != zz; ++aa) name.append((*aa)->unparseToString());
    name.append(">");

    return SgName(name);
  }
#endif
  SgTemplateArgumentPtrList genTemplateArgumentList(Rose_STL_Container<SgNode*>& targs)
  {
    Rose_STL_Container<SgNode*>::iterator aa = targs.begin();
    Rose_STL_Container<SgNode*>::iterator zz = targs.begin();
    SgTemplateArgumentPtrList             lst;

    for ( ; aa != zz; ++aa)
    {
      lst.push_back(createTemplateArg(**aa));
    }

    return lst;
  }

  SgTemplateClassDeclaration* getCanonicalTemplateDecl(SgTemplateClassDeclaration* main_decl)
  {
    ROSE_ASSERT(main_decl);
    SgClassType*                ct    = main_decl->get_type();
    ROSE_ASSERT(ct);
    SgDeclarationStatement*     decl  = ct->get_declaration();
    SgTemplateClassDeclaration* tdecl = isSgTemplateClassDeclaration(decl);

    ROSE_ASSERT(tdecl);
    return tdecl;
  }

  SgTemplateInstantiationDecl* genTemplateInstantiationDecl(SgName tname, SgTemplateClassDeclaration* tclassdecl, SgTemplateArgumentPtrList targs)
  {
    ROSE_ASSERT(tclassdecl);

    SgTemplateInstantiationDecl* res = NULL;

    res = new SgTemplateInstantiationDecl( tname,
                                           SgClassDeclaration::e_class,
                                           NULL /* SgClassType* type -- to be set later */,
                                           NULL /* SgClassDefinition* def -- to be set later */,
                                           tclassdecl,
                                           targs
                                         );

    res->set_scope(tclassdecl->get_scope());
    res->set_templateName(tname); // \todo \pp create mangled name from tname
    res->set_firstNondefiningDeclaration(res);
    res->setForward(); // \pp set forward, since this is not a proper declaration
    return res;
  }

  SgClassType* genTemplateClass(SgTemplateInstantiationDecl* tdecl)
  {
    ROSE_ASSERT(tdecl);

    return SgClassType::createType(tdecl);
  }

  struct TemplateArgumentParentSetter
  {
    SgTemplateInstantiationDecl* parent;

    TemplateArgumentParentSetter(SgTemplateInstantiationDecl* p)
    : parent(p)
    {}

    void operator()(SgTemplateArgument* targ)
    {
      targ->set_parent(parent);
    }
  };
} /* anonymous namespace */


SgClassType*
SageBuilder::buildClassTemplateType(SgTemplateClassDeclaration* template_decl,
Rose_STL_Container<SgNode *>& template_args)
{
  ROSE_ASSERT(template_decl);

  // create a template instantiation decl
  SgName                       name  = template_decl->get_name();
  // SgName                       tname  = genTemplateName(, template_args);
  SgTemplateArgumentPtrList    targs = genTemplateArgumentList(template_args);
  SgTemplateClassDeclaration*  tdecl = getCanonicalTemplateDecl(template_decl);
  SgTemplateInstantiationDecl* tinst = genTemplateInstantiationDecl(name, tdecl, targs);
  ROSE_ASSERT(tinst);

  // create class type
  SgClassType*                 tclass = genTemplateClass(tinst);
  ROSE_ASSERT(tclass);

  // set remaining fields in the template instantiation decl
  tinst->set_type(tclass);
  tinst->set_definition(static_cast<SgTemplateInstantiationDefn*>(0)); /* \pp not sure what to set this to .. */

  // set parent of template arguments
  std::for_each(targs.begin(), targs.end(), TemplateArgumentParentSetter(tinst)); //

  return tclass;
}

//-----------------------------------------------------------------------------
#ifdef ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
//-----------------------------------------------------------------------------

/**
 *
 */
SgVarRefExp *SageBuilder::buildJavaArrayLengthVarRefExp() {
    ROSE_ASSERT(Rose::Frontend::Java::lengthSymbol);
    SgVarRefExp *var_ref = SageBuilder::buildVarRefExp(Rose::Frontend::Java::lengthSymbol);
    SageInterface::setSourcePosition(var_ref);
    return var_ref;
}

/**
 *
 */
SgScopeStatement *SageBuilder::buildScopeStatement(SgClassDefinition *parent_scope) {
    SgScopeStatement *scope = new SgScopeStatement();
    SageInterface::setSourcePosition(scope);
    if (parent_scope != NULL) {
        scope -> set_parent(parent_scope);
    }
    return scope;
}

/**
 *
 */
SgJavaTypeExpression *SageBuilder::buildJavaTypeExpression(SgType *type) {
    SgJavaTypeExpression *expr = new SgJavaTypeExpression(type);
    SageInterface::setSourcePosition(expr);
    return expr;
}

/**
 *
 */
SgJavaMarkerAnnotation *SageBuilder::buildJavaMarkerAnnotation(SgType *type) {
    SgJavaMarkerAnnotation *annotation = new SgJavaMarkerAnnotation(type);
    SageInterface::setSourcePosition(annotation);
    return annotation;
}

/**
 *
 */
SgJavaMemberValuePair *SageBuilder::buildJavaMemberValuePair(const SgName &name, SgExpression *value) {
    SgJavaMemberValuePair *pair = new SgJavaMemberValuePair();
    SageInterface::setSourcePosition(pair);
    pair -> set_name(name);
    pair -> set_value(value);
    value -> set_parent(pair);
    return pair;
}

/**
 *
 */
SgJavaSingleMemberAnnotation *SageBuilder::buildJavaSingleMemberAnnotation(SgType *type, SgExpression *value) {
    SgJavaSingleMemberAnnotation *annotation = new SgJavaSingleMemberAnnotation(type, value);
    SageInterface::setSourcePosition(annotation);
    return annotation;
}

/**
 *
 */
SgJavaNormalAnnotation *SageBuilder::buildJavaNormalAnnotation(SgType *type) {
    SgJavaNormalAnnotation *annotation = new SgJavaNormalAnnotation(type);
    SageInterface::setSourcePosition(annotation);
    return annotation;
}

/**
 *
 */
SgJavaNormalAnnotation *SageBuilder::buildJavaNormalAnnotation(SgType *type, list<SgJavaMemberValuePair *>& pair_list) {
    SgJavaNormalAnnotation *annotation = SageBuilder::buildJavaNormalAnnotation(type);
    for (std::list<SgJavaMemberValuePair *>::iterator i = pair_list.begin(); i != pair_list.end(); i++) {
        SgJavaMemberValuePair *member_value_pair = *i;
        member_value_pair -> set_parent(annotation);
        annotation -> append_value_pair(member_value_pair);
    }
    return annotation;
}


/**
 *
 */
SgInitializedName *SageBuilder::buildJavaFormalParameter(SgType *argument_type, const SgName &argument_name, bool is_var_args, bool is_final) {
    SgInitializedName *initialized_name = NULL;
    if (is_var_args) {
        initialized_name = SageBuilder::buildInitializedName(argument_name, SageBuilder::getUniqueJavaArrayType(argument_type, 1), NULL);
        initialized_name -> setAttribute("var_args", new AstRegExAttribute(""));
    }
    else {
        initialized_name = SageBuilder::buildInitializedName(argument_name, argument_type, NULL);
    }
    SageInterface::setSourcePosition(initialized_name);
    if (is_final) {
        initialized_name -> setAttribute("final", new AstRegExAttribute(""));
    }

    return initialized_name;
}

/**
 * The import_info represents the qualified name of a package, type or static field.
 */
SgJavaPackageStatement *SageBuilder::buildJavaPackageStatement(string package_name) {
    SgJavaPackageStatement *package_statement = new SgJavaPackageStatement(package_name);
    SageInterface::setSourcePosition(package_statement);
    package_statement -> set_firstNondefiningDeclaration(package_statement);
    package_statement -> set_definingDeclaration(package_statement);
    return package_statement;
}

/**
 * The import_info represents the qualified name of a package, type or static field.
 */
SgJavaImportStatement *SageBuilder::buildJavaImportStatement(string import_info, bool contains_wildcard) {
    SgJavaImportStatement *import_statement = new SgJavaImportStatement(import_info, contains_wildcard);
    SageInterface::setSourcePosition(import_statement);
    import_statement -> set_firstNondefiningDeclaration(import_statement);
    import_statement -> set_definingDeclaration(import_statement);
    return import_statement;
}

/**
 *  Build a class with the given name in the given scope and return its SgClassDefinition.
 */
SgClassDeclaration *SageBuilder::buildJavaDefiningClassDeclaration(SgScopeStatement *scope, string name, SgClassDeclaration::class_types kind) {
    ROSE_ASSERT(scope);
    SgName class_name = name;
    ROSE_ASSERT(scope -> lookup_class_symbol(class_name) == NULL);

    SgClassDeclaration* nonDefiningDecl              = NULL;
    bool buildTemplateInstantiation                  = false;
    SgTemplateArgumentPtrList* templateArgumentsList = NULL;
    SgClassDeclaration *class_declaration = SageBuilder::buildClassDeclaration_nfi(class_name, kind, scope, nonDefiningDecl, buildTemplateInstantiation, templateArgumentsList);
    ROSE_ASSERT(class_declaration);
    class_declaration -> set_parent(scope);
    class_declaration -> set_scope(scope);
    SageInterface::setSourcePosition(class_declaration);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    ROSE_ASSERT(class_definition);
    SageInterface::setSourcePosition(class_definition);

    class_definition -> setAttribute("extensions", new AstSgNodeListAttribute());
    class_definition -> setAttribute("extension_type_names", new AstRegExAttribute());

    SgScopeStatement *type_space = new SgScopeStatement();
    type_space -> set_parent(class_definition);
    SageInterface::setSourcePosition(type_space);
    class_declaration -> setAttribute("type_space", new AstSgNodeAttribute(type_space));

    return class_declaration;
}


/**
 * Create a source file in the directory_name for the given type_name and add it to the given project.
 * This function is useful in order to create a new type to be added to a pre-existing Rose AST.
 */
SgSourceFile *SageBuilder::buildJavaSourceFile(SgProject *project, string directory_name, SgClassDefinition *package_definition, string type_name) {
    string filename = directory_name + "/" + type_name + ".java";
    ROSE_ASSERT((*project)[filename] == NULL); // does not already exist!

    string command = string("touch ") + filename; // create the file
    int status = system(command.c_str());
    ROSE_ASSERT(status == 0);
    project -> get_sourceFileNameList().push_back(filename);
    Rose_STL_Container<std::string> arg_list = project -> get_originalCommandLineArgumentList();
    arg_list.push_back(filename);
    Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(arg_list, // binaryMode
                                                                                         false);
    CommandlineProcessing::removeAllFileNamesExcept(arg_list, fileList, filename);
    int error_code = 0;
    SgFile *file = determineFileType(arg_list, error_code, project);
    SgSourceFile *sourcefile = isSgSourceFile(file);
    ROSE_ASSERT(sourcefile);
    sourcefile -> set_parent(project);
    project -> get_fileList_ptr() -> get_listOfFiles().push_back(sourcefile);
    ROSE_ASSERT(sourcefile == isSgSourceFile((*project)[filename]));

    //
    // Create a package statement and add it to the source file
    //
    SgJavaPackageStatement *package_statement = SageBuilder::buildJavaPackageStatement(package_definition -> get_declaration() -> get_qualified_name().getString());
    package_statement -> set_parent(package_definition);
    sourcefile -> set_package(package_statement);

    //
    // Initialize an import-list for the sourcefile
    //
    SgJavaImportStatementList *import_statement_list = new SgJavaImportStatementList();
    import_statement_list -> set_parent(sourcefile);
    sourcefile -> set_import_list(import_statement_list);

    //
    // Initialize a class-declaration-list for the sourcefile
    //
    SgJavaClassDeclarationList *class_declaration_list = new SgJavaClassDeclarationList();
    class_declaration_list -> set_parent(package_definition);
    sourcefile -> set_class_list(class_declaration_list);

    return sourcefile;
}


/**
 *
 */
SgArrayType *SageBuilder::getUniqueJavaArrayType(SgType *base_type, int num_dimensions) {
    ROSE_ASSERT(num_dimensions > 0);
    if (num_dimensions > 1) {
        base_type = getUniqueJavaArrayType(base_type, num_dimensions - 1);
    }

    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) base_type -> getAttribute("array");
    if (attribute == NULL) {
        SgArrayType *array_type = SageBuilder::buildArrayType(base_type);
        array_type -> set_rank(num_dimensions);
        attribute = new AstSgNodeAttribute(array_type);
        base_type -> setAttribute("array", attribute);
    }

    return isSgArrayType(attribute -> getNode());
}


/**
 *
 */
SgJavaParameterizedType *SageBuilder::getUniqueJavaParameterizedType(SgNamedType *generic_type, SgTemplateParameterPtrList *new_args) {
    AstParameterizedTypeAttribute *attribute = (AstParameterizedTypeAttribute *) generic_type -> getAttribute("parameterized types");
    if (! attribute) {
        attribute = new AstParameterizedTypeAttribute(generic_type);
        generic_type -> setAttribute("parameterized types", attribute);
    }
    ROSE_ASSERT(attribute);

    return attribute -> findOrInsertParameterizedType(new_args);
}


/**
 *
 */
SgJavaQualifiedType *SageBuilder::getUniqueJavaQualifiedType(SgClassDeclaration *class_declaration, SgNamedType *parent_type, SgNamedType *type) {
    AstSgNodeListAttribute *attribute = (AstSgNodeListAttribute *) type -> getAttribute("qualified types");
    if (! attribute) {
        attribute = new AstSgNodeListAttribute();
        type -> setAttribute("qualified types", attribute);
    }
    ROSE_ASSERT(attribute);

    for (int i = 0; i < attribute -> size(); i++) {
        SgJavaQualifiedType *qualified_type = isSgJavaQualifiedType(attribute -> getNode(i));
        ROSE_ASSERT(qualified_type);
        if (qualified_type -> get_parent_type() == parent_type &&  qualified_type -> get_type() == type) {
            return qualified_type;
        }
    }

    SgJavaQualifiedType *qualified_type = new SgJavaQualifiedType(class_declaration);
    qualified_type -> set_parent_type(parent_type);
    qualified_type -> set_type(type);

    attribute -> addNode(qualified_type);

    return qualified_type;
}


/**
 * Generate the unbound wildcard if it does not yet exist and return it.  Once the unbound Wildcard
 * is generated, it is attached to the Object type so that it can be retrieved later.
 */
SgJavaWildcardType *SageBuilder::getUniqueJavaWildcardUnbound() {
  AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) Rose::Frontend::Java::ObjectClassType -> getAttribute("unbound");
    if (! attribute) {
        SgClassDeclaration *class_declaration = isSgClassDeclaration(Rose::Frontend::Java::ObjectClassType -> get_declaration());
        SgJavaWildcardType *wildcard = new SgJavaWildcardType(class_declaration -> get_definingDeclaration());
        attribute = new AstSgNodeAttribute(wildcard);
        Rose::Frontend::Java::ObjectClassType -> setAttribute("unbound", attribute);
    }

    return isSgJavaWildcardType(attribute -> getNode());
}


/**
 * If it does not exist yet, generate wildcard type that extends this type.  Return the wildcard in question.
 */
SgJavaWildcardType *SageBuilder::getUniqueJavaWildcardExtends(SgType *type) {
    ROSE_ASSERT(type);
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) type -> getAttribute("extends");
    if (! attribute) {
        SgArrayType *array_type = isSgArrayType(type);
        SgNamedType *named_type = isSgNamedType(type);
        ROSE_ASSERT(array_type || named_type);
        SgClassDeclaration *class_declaration = isSgClassDeclaration((array_type ? (SgNamedType *) Rose::Frontend::Java::ObjectClassType : named_type) -> get_declaration());
        SgJavaWildcardType *wildcard = new SgJavaWildcardType(class_declaration -> get_definingDeclaration(), type);

        wildcard -> set_has_extends(true);

        attribute = new AstSgNodeAttribute(wildcard);
        type -> setAttribute("extends", attribute);
    }

    return isSgJavaWildcardType(attribute -> getNode());
}


/**
 * If it does not exist yet, generate a super wildcard for this type.  Return the wildcard in question.
 */
SgJavaWildcardType *SageBuilder::getUniqueJavaWildcardSuper(SgType *type) {
    ROSE_ASSERT(type);
    AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) type -> getAttribute("super");
    if (! attribute) {
        SgArrayType *array_type = isSgArrayType(type);
        SgNamedType *named_type = isSgNamedType(type);
        ROSE_ASSERT(array_type || named_type);
        SgClassDeclaration *class_declaration = isSgClassDeclaration((array_type ? (SgNamedType *) Rose::Frontend::Java::ObjectClassType : named_type) -> get_declaration());
        SgJavaWildcardType *wildcard = new SgJavaWildcardType(class_declaration -> get_definingDeclaration(), type);

        wildcard -> set_has_super(true);

        attribute = new AstSgNodeAttribute(wildcard);
        type -> setAttribute("super", attribute);
    }

    return isSgJavaWildcardType(attribute -> getNode());
}
//-----------------------------------------------------------------------------
#endif // ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
//-----------------------------------------------------------------------------

