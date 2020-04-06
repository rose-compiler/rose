// tps (01/14/2010) : Switching from rose.h to sage3. Added buildMangledNameMap, buildReplacementMap, fixupTraversal, replaceExpressionWithStatement
//#include "rose.h"
#include "sage3basic.h"
#include "markLhsValues.h"
#include "fixupNames.h"

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "buildMangledNameMap.h"
   #include "buildReplacementMap.h"
   #include "fixupTraversal.h"
#endif

#include "sageInterface.h"

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
#include "replaceExpressionWithStatement.h"

#include "constantFolding.h"
#endif

// DQ (10/14/2006): Added supporting help functions. tps commented out since it caused no compilation errors
//#include "rewrite.h"

// Liao 1/24/2008 : need access to scope stack sometimes
#include "sageBuilder.h"

// PP 01/06/2012 : need swap operations for wrapFunction implementation
// PP 05/30/2012 : need ancestor function
#include "sageGeneric.h"

// PP 01/06/2012 : need convenience functors to interface STL
#include "sageFunctors.h"

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
// For reusing some code from Qing's loop optimizer
// Liao, 2/26/2009
#include "AstInterface_ROSE.h"
#include "LoopTransformInterface.h"

#include "DepInfoAnal.h" // for AnalyzeStmtRefs()
#include "ArrayAnnot.h"
#include "ArrayInterface.h"

#include "LoopUnroll.h"
#include "abstract_handle.h"
#include "roseAdapter.h"
#endif

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <sstream>
#include <iostream>
#include <algorithm> // for set operations
#include <numeric>   // for std::accumulate

#ifdef ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
#   include "jni.h"

namespace Rose {
    namespace Frontend {
        namespace Java {
            namespace Ecj {

                extern jclass currentJavaTraversalClass;
                extern JNIEnv *currentEnvironment;
                extern jmethodID mainMethod;
                extern jmethodID hasConflictsMethod;
                extern jmethodID getTempDirectoryMethod;
                extern jmethodID createTempFileMethod;
                extern jmethodID createTempNamedFileMethod;
                extern jmethodID createTempNamedDirectoryMethod;

            } // ::Rose::Frontend::Java::Ecj
        }// ::Rose::frontend::java
    }// ::Rose::frontend
}// ::Rose

using namespace Rose::Frontend::Java::Ecj;

#endif

#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "transformationSupport.h"
#endif

// We need this so that USE_CMAKE will be seen (set via configure).
#include "rose_config.h"

#ifndef USE_CMAKE
// DQ (3/8/2014): Make this conditionally compiled based on when CMake is not used because the libraries are not configured yet.
// DQ (3/4/2014): We need this feature to support the function: isStructurallyEquivalentAST().
#include "RoseAst.h"
#endif

//! C++ SageBuilder namespace specific state for storage of the source code position state (used to control how the source code positon is defined for IR nodes built within the SageBuilder interface).
extern SageBuilder::SourcePositionClassification SageBuilder::SourcePositionClassificationMode;


typedef std::set<SgLabelStatement*> SgLabelStatementPtrSet;

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace Rose;
using namespace SageBuilder;




void
SageInterface::DeclarationSets::addDeclaration(SgDeclarationStatement* decl)
   {
  // DQ (4/3/2014): This function either builds a new set or inserts declarations into an
  // existing set based on if a set defined by the key (firstNondefiningDeclaration) is present.
     ROSE_ASSERT(decl != NULL);

#if 0
     printf ("TOP of SageInterface::DeclarationSets::addDeclaration(): decl = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
#endif

     SgDeclarationStatement* firstNondefiningDeclaration = decl->get_firstNondefiningDeclaration();

     if (firstNondefiningDeclaration == NULL)
        {
       // It appears that some loop transformations (pass3.C) don't set the firstNondefiningDeclaration.
#if 0
          printf ("WARNING: SageInterface::DeclarationSets::addDeclaration(): firstNondefiningDeclaration == NULL: decl = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
#endif
          return;
        }
     ROSE_ASSERT(firstNondefiningDeclaration != NULL);

     if (decl == firstNondefiningDeclaration)
        {
#if 0
          if (isSgTypedefDeclaration(decl) != NULL)
             {
               printf ("TOP of SageInterface::DeclarationSets::addDeclaration(): decl = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
             }
#endif
          if (declarationMap.find(firstNondefiningDeclaration) == declarationMap.end())
             {
#if 0
               printf ("In SageInterface::DeclarationSets::addDeclaration(): Add a set for decl = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
#endif
#if 0
               if (isSgTypedefDeclaration(decl) != NULL)
                  {
                    printf ("In SageInterface::DeclarationSets::addDeclaration(): Add a set for decl = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
                  }
#endif
            // Add a new set.
               declarationMap[decl] = new set<SgDeclarationStatement*>();

               ROSE_ASSERT (declarationMap.find(firstNondefiningDeclaration) != declarationMap.end());
               ROSE_ASSERT(declarationMap[decl] != NULL);

            // Add a declaration to an existing set.
               declarationMap[firstNondefiningDeclaration]->insert(decl);
             }
            else
             {
               if (declarationMap[firstNondefiningDeclaration]->find(decl) == declarationMap[firstNondefiningDeclaration]->end())
                  {
#if 0
                    printf ("In SageInterface::DeclarationSets::addDeclaration(): Add the declaration to the existing set: decl = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
#endif
#if 0
                    if (isSgTypedefDeclaration(decl) != NULL)
                       {
                         printf ("In SageInterface::DeclarationSets::addDeclaration(): Add the declaration to the existing set: decl = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
                       }
#endif
                 // Add a declaration to an existing set.
                    declarationMap[firstNondefiningDeclaration]->insert(decl);
                  }
                 else
                  {
#if 1
                    printf ("WARNING: SageInterface::DeclarationSets::addDeclaration(): A set already exists for decl = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
#endif
                 // DQ (4/5/2014): The case of SgFunctionParameterList fails only for boost examples (e.g. test2014_240.C).
                 // Problem uses are associated with SgTemplateInstantiationFunctionDecl IR nodes.
                    bool ignore_error = (isSgFunctionParameterList(decl) != NULL);

                 // DQ (4/17/2014): This is required for the EDG version 4.8 and I don't know why.
                 // Currently the priority is to pass our existing tests.
                 // An idea is that this is sharing introduced as a result of the use of default parameters.
#if (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4) && (BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER > 6)
                    ignore_error = ignore_error || (isSgTemplateInstantiationDecl(decl) != NULL);
#else
                 // DQ (7/2/2014): I am seeing that this is required for a new application using GNU 4.4.7.
                 // It allows a boost issue specific to a revisited SgTypedefDeclaration pass, but I still
                 // don't understand the problem.  so this needs a better fix.
                 // ignore_error = ignore_error || (isSgTypedefDeclaration(decl) != NULL);
                    ignore_error = ignore_error || (isSgTypedefDeclaration(decl) != NULL) || (isSgTemplateInstantiationDecl(decl) != NULL);
#endif

                    if (ignore_error == true)
                       {
#if 0
#if (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4) && (BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER > 6)
                         printf ("Ignoring the error for a SgFunctionParameterList and SgTemplateInstantiationDecl \n");
#else
                         printf ("Ignoring the error for a SgFunctionParameterList \n");
#endif
#endif
                       }
                      else
                       {
                         printf ("declarationMap[firstNondefiningDeclaration]->size() = %zu \n",declarationMap[firstNondefiningDeclaration]->size());

                         printf ("decl                             = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
                         printf ("decl->get_parent()               = %p = %s = %s \n",decl->get_parent(),decl->get_parent()->class_name().c_str(),get_name(decl->get_parent()).c_str());
                         printf ("decl->get_parent()->get_parent() = %p = %s = %s \n",decl->get_parent()->get_parent(),decl->get_parent()->get_parent()->class_name().c_str(),get_name(decl->get_parent()->get_parent()).c_str());

                         SgNamespaceDefinitionStatement* namespaceDefinitionStatement = isSgNamespaceDefinitionStatement(decl->get_parent()->get_parent());
                         if (namespaceDefinitionStatement != NULL)
                            {
                              namespaceDefinitionStatement->get_file_info()->display("namespaceDefinitionStatement: debug");
                            }

                         if (isSgCtorInitializerList(decl) != NULL)
                            {
                              firstNondefiningDeclaration->get_parent()->get_file_info()->display("declarationMap.find(firstNondefiningDeclaration) != declarationMap.end(): firstNondefiningDeclaration->get_parent(): debug");
                              decl->get_parent()->get_file_info()->display("declarationMap.find(firstNondefiningDeclaration) != declarationMap.end(): decl->get_parent(): debug");
                            }

                         firstNondefiningDeclaration->get_file_info()->display("declarationMap.find(firstNondefiningDeclaration) != declarationMap.end(): firstNondefiningDeclaration: debug");
                         decl->get_file_info()->display("declarationMap.find(firstNondefiningDeclaration) != declarationMap.end(): decl: debug");
#if 1
                         printf ("Can not ignore this error \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }
             }
        }
       else
        {
       // Add the declaration (make sure there is a set that exists).
          if (declarationMap.find(firstNondefiningDeclaration) != declarationMap.end())
             {
            // Make sure it does not already exist in the set.
               ROSE_ASSERT (declarationMap[firstNondefiningDeclaration] != NULL);
#if 0
               printf ("In SageInterface::DeclarationSets::addDeclaration(): Add a declaration to an existing set for decl = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
#endif
            // Add a declaration to an existing set.
            // ROSE_ASSERT (declarationMap[firstNondefiningDeclaration]->find(decl) == declarationMap[firstNondefiningDeclaration]->end());
            // declarationMap[firstNondefiningDeclaration]->insert(decl);
               if (declarationMap[firstNondefiningDeclaration]->find(decl) == declarationMap[firstNondefiningDeclaration]->end())
                  {
                 // Add a declaration to an existing set.
                    declarationMap[firstNondefiningDeclaration]->insert(decl);
                  }
                 else
                  {
#if 0
                    printf ("This declaration is already in the set (skip adding it twice): decl = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
#endif
                  }
             }
            else
             {
            // In this case the defining declaration might be the only declaration to be traversed and
            // so a set has not been built yet.
#if 0
               printf ("In SageInterface::DeclarationSets::addDeclaration(): Adding set and declaration for the firstNondefiningDeclaration = %p = %s = %s \n",
                    firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str(),get_name(firstNondefiningDeclaration).c_str());
#endif

            // DQ (4/5/2014): Just build the set and don't insert the firstNondefiningDeclaration.
            // If we were to do so then it would be an error to use the insert it later.
            // Note recursive call.
            // addDeclaration(firstNondefiningDeclaration);
               declarationMap[firstNondefiningDeclaration] = new set<SgDeclarationStatement*>();

               ROSE_ASSERT (declarationMap.find(firstNondefiningDeclaration) != declarationMap.end());

            // DQ (4/5/2014): We have to insert this since it is different from the firstNondefiningDeclaration.
            // Add the declaration to the existing set.
               declarationMap[firstNondefiningDeclaration]->insert(decl);

            // DQ (4/5/2014): Added assertion.
               ROSE_ASSERT(declarationMap[firstNondefiningDeclaration]->find(decl) != declarationMap[firstNondefiningDeclaration]->end());
#if 0
               printf ("SageInterface::DeclarationSets::addDeclaration(): No set exists for the firstNondefiningDeclaration = %p = %s = %s \n",firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str(),get_name(firstNondefiningDeclaration).c_str());
               ROSE_ASSERT(false);
#endif
             }
        }

#if 0
     printf ("Leaving SageInterface::DeclarationSets::addDeclaration(): decl = %p = %s = %s \n",decl,decl->class_name().c_str(),get_name(decl).c_str());
#endif
   }

const std::set<SgDeclarationStatement*>* 
SageInterface::DeclarationSets::getDeclarations(SgDeclarationStatement* decl)
   {
  // DQ (4/3/2014): This function returns the associated set of declarations.
     SgDeclarationStatement* firstNondefiningDeclaration = decl->get_firstNondefiningDeclaration();

     ROSE_ASSERT(declarationMap.find(firstNondefiningDeclaration) != declarationMap.end());

     const set<SgDeclarationStatement*>* declarationSet = declarationMap[firstNondefiningDeclaration];

  // return this->declarationMap[firstNondefiningDeclaration];
     return declarationSet;
   }

std::map<SgDeclarationStatement*,std::set<SgDeclarationStatement*>* > &
SageInterface::DeclarationSets::getDeclarationMap()
   {
     return declarationMap;
   }

bool
SageInterface::DeclarationSets::isLocatedInDefiningScope(SgDeclarationStatement* decl)
   {
  // DQ (4/7/2014): This function assumes that the input is a friend declaration.

  // The existence of a declaration in a named scope (if a friend function) will cause
  // subsequent declarations to be qualified where name qualification is required.
  // A couple of issues:
  //    We likely need to keep track of the order of the declarations in the more
  //    complex cases because name qualification will be required after the declaration
  //    that appears in the named scope; but will not be required before the declaration in
  //    the named scope.

#define DEBUG_LOCATED_IN_DEFINING_SCOPE 0

     SgDeclarationStatement* firstNondefiningDeclaration = decl->get_firstNondefiningDeclaration();
     ROSE_ASSERT(firstNondefiningDeclaration != NULL);

     set<SgDeclarationStatement*>* declarationSet = declarationMap[firstNondefiningDeclaration];
     ROSE_ASSERT(declarationSet != NULL);

     set<SgDeclarationStatement*>::iterator i = declarationSet->begin();

     bool isDefinedInNamedScope = false;

#if DEBUG_LOCATED_IN_DEFINING_SCOPE
     printf ("In DeclarationSets::isLocatedInDefiningScope(): decl = %p = %s \n",decl,decl->class_name().c_str());
     printf ("   --- declarationSet->size()                        = %zu \n",declarationSet->size());
#endif

     SgDeclarationStatement* associatedDeclaration = NULL;

     while (isDefinedInNamedScope == false && i != declarationSet->end())
        {
          ROSE_ASSERT(*i != NULL);
#if DEBUG_LOCATED_IN_DEFINING_SCOPE
          printf ("   --- *i = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
       // We want to know the structural position, not the semantic scope.
          SgScopeStatement* semantic_scope   = (*i)->get_scope();
          SgScopeStatement* structural_scope = isSgScopeStatement((*i)->get_parent());

#if DEBUG_LOCATED_IN_DEFINING_SCOPE
          printf ("   --- semantic_scope = %p = %s \n",semantic_scope,semantic_scope->class_name().c_str());
          printf ("   --- structural_scope = %p = %s \n",structural_scope,structural_scope->class_name().c_str());
#endif
       // DQ (4/7/2014): If it is a member of a class then we don't consider the structural scope, else it makes a difference,
          SgScopeStatement* scope = isSgClassDefinition(semantic_scope) != NULL ? semantic_scope : structural_scope;
          ROSE_ASSERT(scope != NULL);

#if DEBUG_LOCATED_IN_DEFINING_SCOPE
          printf ("   --- scope = %p = %s \n",scope,scope->class_name().c_str());
          printf ("   --- scope->isNamedScope() = %s \n",scope->isNamedScope() ? "true" : "false");
#endif
       // if (scope->isNamedScope() == true)
          SgGlobal* globalScope = isSgGlobal(scope);

       // Friend functions declared in the class definition are not meaningful for determining name qualification.
       // if (globalScope != NULL || scope->isNamedScope() == true)
          if (globalScope != NULL || (scope->isNamedScope() == true && isSgClassDefinition(structural_scope) == NULL) )
             {
            // Check if the function is output in the unparing, else it would not be defined.
               bool willBeOutput = ((*i)->get_file_info()->isCompilerGenerated() == false ||
                                      ((*i)->get_file_info()->isCompilerGenerated() &&
                                       (*i)->get_file_info()->isOutputInCodeGeneration()) );
#if DEBUG_LOCATED_IN_DEFINING_SCOPE
               printf ("   --- before: willBeOutput = %s \n",willBeOutput ? "true" : "false");
#endif
            // Being output only count when it is output where it is located structurally.
               willBeOutput = willBeOutput && scope == structural_scope;

#if DEBUG_LOCATED_IN_DEFINING_SCOPE
               printf ("   --- after: willBeOutput = %s \n",willBeOutput ? "true" : "false");
#endif
               associatedDeclaration = *i;

            // isDefinedInNamedScope = true;
               isDefinedInNamedScope = willBeOutput;
             }

          i++;
        }

#if DEBUG_LOCATED_IN_DEFINING_SCOPE
     if (associatedDeclaration != NULL)
        {
          printf ("Leaving DeclarationSets::isLocatedInDefiningScope(): associatedDeclaration = %p = %s \n",associatedDeclaration,associatedDeclaration->class_name().c_str());
        }
       else
        {
          printf ("Leaving DeclarationSets::isLocatedInDefiningScope(): associatedDeclaration = %p \n",associatedDeclaration);
        }
#endif

     return isDefinedInNamedScope;
   }

SageInterface::DeclarationSets*
SageInterface::buildDeclarationSets(SgNode* n)
   {
     DeclarationSets* declarationSet = new DeclarationSets();

     class DeclarationSetTraversal : public AstSimpleProcessing
        {
          private:
               DeclarationSets* declarationSet;

          public:
               DeclarationSetTraversal(DeclarationSets* ds) : declarationSet(ds) {}
               void visit (SgNode* node)
                  {
                    SgDeclarationStatement* decl = isSgDeclarationStatement(node);
                    if (decl != NULL)
                       {
                         declarationSet->addDeclaration(decl);
                       }
                  }
        };

  // Now buid the traveral object and call the traversal (preorder) on the AST subtree.
     DeclarationSetTraversal traversal(declarationSet);
     traversal.traverse(n, preorder);

     return declarationSet;
   }


int SageInterface::gensym_counter = 0;
// DQ: 09/23/03
// We require a global function for getting the string associated
// with the definition of a variant (which is a global enum).
string getVariantName ( VariantT v )
   {
     ROSE_ASSERT( int(v) < int(V_SgNumVariants));

  // This code is far simpler (if the function is static)

  // DQ (4/8/2004): Modified code to use new global list of sage
  // class names (simpler this way)
  // return string(SgTreeTraversal<int,int>::get_variantName(v));
     extern const char* roseGlobalVariantNameList[];
     return string(roseGlobalVariantNameList[v]);
   }

bool
SageInterface::hasTemplateSyntax( const SgName & name )
   {
  // DQ (6/7/2012): This refactors the test for template instantation syntax in names used for classes, member functions, and functions
     bool usingTemplateSyntax = false;
     string nameString = name.getString();

  // DQ (6/7/2012): We need to avoid the identification of the "operator<()" as valid template syntax.
     usingTemplateSyntax = (nameString.find('<') != string::npos) && (nameString.find('>') != string::npos);

  // return (name.getString().find('<') == string::npos);
     return usingTemplateSyntax;
   }



void
SageInterface::whereAmI(SgNode* node)
   {
  // DQ (2/12/2012): Refactoring disagnostic support for detecting where we are when something fails.

  // This highest level node acceptable for us by this function is a SgGlobal (global scope).

     ROSE_ASSERT(node != NULL);
     printf ("Inside of SageInterface::whereAmI(node = %p = %s) \n",node,node->class_name().c_str());

  // Enforce that some IR nodes should not be acepted inputs.
     ROSE_ASSERT(isSgFile(node)     == NULL);
     ROSE_ASSERT(isSgFileList(node) == NULL);
     ROSE_ASSERT(isSgProject(node)  == NULL);

     SgNode* parent = node->get_parent();

  // Don't traverse past the SgFile level.
     while (parent != NULL && isSgFileList(parent) == NULL)
        {
          printf ("--- parent = %p = %s \n",parent,parent->class_name().c_str());

          ROSE_ASSERT(parent->get_file_info() != NULL);
          parent->get_file_info()->display("In SageInterface::whereAmI() diagnostics support");

          parent = parent->get_parent();
        }
   }

void
SageInterface::initializeIfStmt(SgIfStmt *ifstmt, SgStatement* conditional, SgStatement * true_body, SgStatement * false_body)
   {
  // DQ (2/13/2012): Added new function to support proper initialization of a SgIfStmt that has already been built.
  // This is important when we have to build the scope ahead of the test becasue the text contains a simple
  // declaration (which must be put into the SgIfStmt scope).

  // SgIfStmt *ifstmt = new SgIfStmt(conditional, true_body, false_body);
     ROSE_ASSERT(ifstmt);

     if (ifstmt->get_conditional() == NULL)
          ifstmt->set_conditional(conditional);

     if (ifstmt->get_true_body() == NULL)
          ifstmt->set_true_body(true_body);

     if (ifstmt->get_false_body() == NULL)
          ifstmt->set_false_body(false_body);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
         ifstmt->setCaseInsensitive(true);

     setOneSourcePositionNull(ifstmt);
     if (conditional) conditional->set_parent(ifstmt);
     if (true_body) true_body->set_parent(ifstmt);
     if (false_body) false_body->set_parent(ifstmt);
   }


void 
SageInterface::initializeSwitchStatement(SgSwitchStatement* switchStatement,SgStatement *item_selector,SgStatement *body)
   {
     ROSE_ASSERT(switchStatement != NULL);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
          switchStatement->setCaseInsensitive(true);

     if (switchStatement->get_item_selector() == NULL)
          switchStatement->set_item_selector(item_selector);

     if (switchStatement->get_body() == NULL)
          switchStatement->set_body(body);

     setOneSourcePositionForTransformation(switchStatement);
     if (item_selector != NULL)
          item_selector->set_parent(switchStatement);
     if (body != NULL)
          body->set_parent(switchStatement);
   }


void
SageInterface::initializeWhileStatement(SgWhileStmt* whileStatement, SgStatement *  condition, SgStatement *body, SgStatement *else_body)
   {
     ROSE_ASSERT(whileStatement);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     if (symbol_table_case_insensitive_semantics == true)
          whileStatement->setCaseInsensitive(true);

     if (whileStatement->get_condition() == NULL)
          whileStatement->set_condition(condition);
     if (whileStatement->get_body() == NULL)
          whileStatement->set_body(body);

  // Python support.
     if (whileStatement->get_else_body() == NULL)
          whileStatement->set_else_body(else_body);

     setOneSourcePositionNull(whileStatement);
     if (condition) condition->set_parent(whileStatement);
     if (body) body->set_parent(whileStatement);

  // DQ (8/10/2011): This is added by Michael to support a Python specific feature.
     if (else_body != NULL)
        {
          whileStatement->set_else_body(else_body);
          else_body->set_parent(whileStatement);
        }
   }



SgNamespaceDefinitionStatement*
SageInterface::enclosingNamespaceScope( SgDeclarationStatement* declaration )
   {
  // DQ (11/4/2007): This function is used in the markTemplateInstantiationsForOutput.C

  // This function returns the namespace associated with any declaration.
  // If the declaration is nested inside of other scopes this function
  // iterates through these scopes to get the the first namespace.
  // This is important for details of template handling, namely making sure
  // that the template specializations (generated by ROSE) are put into the
  // correct location (namespace) since stricted rules in gnu now enforce
  // these details.  If the declaration is not in a namespace as computed
  // in the chain of scopes then this function returns NULL.

     ROSE_ASSERT(declaration != NULL);
     SgScopeStatement* tempScope = declaration->get_scope();

  // Loop back to the first namespace or stop at global scope (stop on either a namespace or the global scope)
     while ( isSgNamespaceDefinitionStatement(tempScope) == NULL && isSgGlobal(tempScope) == NULL )
        {
          tempScope = tempScope->get_scope();
          ROSE_ASSERT(tempScope != NULL);
#if 0
          printf ("Iterating back through scopes: tempScope = %p = %s = %s \n",tempScope,tempScope->class_name().c_str(),SageInterface::get_name(tempScope).c_str());
#endif
        }

     SgNamespaceDefinitionStatement* namespaceScope = isSgNamespaceDefinitionStatement(tempScope);
     return namespaceScope;
   }


SgDeclarationStatement*
SageInterface::getNonInstantiatonDeclarationForClass ( SgTemplateInstantiationMemberFunctionDecl* memberFunctionInstantiation )
   {
  // DQ (11/4/2007): This function is used in the markTemplateInstantiationsForOutput.C
  // DQ (11/4/2007): Get the associated declaration that will be in the original source code,
  // if it is part of a template instantiation then get the original template declaration.

     SgDeclarationStatement* parentDeclaration = NULL;

     SgClassDefinition* classDefinition = memberFunctionInstantiation->get_class_scope();
     ROSE_ASSERT(classDefinition != NULL);

  // SgTemplateDeclaration* templateDeclaration = memberFunctionInstantiation->get_templateDeclaration();
     SgDeclarationStatement* templateDeclaration = memberFunctionInstantiation->get_templateDeclaration();
     ROSE_ASSERT(templateDeclaration != NULL);

  // If it is a template instatiation, then we have to find the temple declaration (not the template instantiation declaration), else we want the class declaration.
     SgTemplateInstantiationDefn* templateInstatiationClassDefinition = isSgTemplateInstantiationDefn(classDefinition);
     if (templateInstatiationClassDefinition != NULL)
        {
       // This is the case of a template member function in a templated class (see test2005_172.C).
          SgTemplateInstantiationDefn* parentTemplateInstantiationDefinition = isSgTemplateInstantiationDefn(templateDeclaration->get_scope());
          ROSE_ASSERT(parentTemplateInstantiationDefinition != NULL);
          SgTemplateInstantiationDecl* parentTemplateInstantiationDeclaration = isSgTemplateInstantiationDecl(parentTemplateInstantiationDefinition->get_declaration());
          ROSE_ASSERT(parentTemplateInstantiationDeclaration != NULL);

          parentDeclaration = parentTemplateInstantiationDeclaration->get_templateDeclaration();
          ROSE_ASSERT(parentDeclaration != NULL);
        }
       else
        {
       // This is the case of a template member function in a class definition (see test2005_168.C).
          parentDeclaration = classDefinition->get_declaration();
          ROSE_ASSERT(parentDeclaration != NULL);
        }

      return parentDeclaration;
   }

//! Liao, 11/9/2009
  //! A better version for SgVariableDeclaration::set_baseTypeDefininingDeclaration(), handling all side effects automatically
  //! Used to have a struct declaration embedded into a variable declaration
void
SageInterface::setBaseTypeDefiningDeclaration(SgVariableDeclaration* var_decl, SgDeclarationStatement *base_decl)
{
  ROSE_ASSERT (var_decl && base_decl);

  // try to remove it from the scope's declaration list
  // If the struct decl was previously inserted into its scope
  if (base_decl->get_parent() != NULL)
  {
    if (base_decl->get_scope() == base_decl->get_parent())
    {
      SgStatementPtrList stmt_list = base_decl->get_scope()->generateStatementList();
      if (find(stmt_list.begin(), stmt_list.end(), base_decl) != stmt_list.end())
        SageInterface::removeStatement(base_decl);
    }
  }
  base_decl->set_parent(var_decl);
  var_decl->set_baseTypeDefiningDeclaration(base_decl);

  //Set an internal mangled name for the anonymous declaration, if necessary
  //  resetNamesInAST(); // this will trigger mangleNameMap.size() ==0 assertion.
  // We call resetEmptyNames directly instead.
  ResetEmptyNames t1;
  t1.traverseMemoryPool();
}

// DQ (11/4/2007): This looks for a forward temple member function declaration of matching name exists in the specified scope.
bool
SageInterface::isPrototypeInScope ( SgScopeStatement* scope, SgFunctionDeclaration* functionDeclaration, SgDeclarationStatement* startingAtDeclaration )
   {
  // DQ (11/3/2007): Look for an existing previously inserted (or existing) template member function declaration and only add one if we can't find it.

     bool foundExistingPrototype = false;

     ROSE_ASSERT(scope != NULL);
     ROSE_ASSERT(functionDeclaration != NULL);
     ROSE_ASSERT(startingAtDeclaration != NULL);

  // These are the scopes for which get_declarationList() is properly defined.
     ROSE_ASSERT(scope->containsOnlyDeclarations() == true);

  // Note that this is only defined for certain scopes, but we only want it for THOSE scopes
     SgDeclarationStatementPtrList & declarationList = scope->getDeclarationList();

  // This is a simple way to restrict the evaluation (still linear, but jumps to initial position to start search).
     SgDeclarationStatementPtrList::iterator startingLocation = find(declarationList.begin(),declarationList.end(),startingAtDeclaration);

     if (startingLocation != declarationList.end())
        {
#if 0
          printf ("startingLocation = %p = %s = %s \n",*startingLocation,(*startingLocation)->class_name().c_str(),SageInterface::get_name(*startingLocation).c_str());
#endif
       // printf ("Found the startingLocation is the global scope, now check if we need to add a new declaration \n");
          SgDeclarationStatementPtrList::iterator i = startingLocation;
          SgScopeStatement* targetScope = functionDeclaration->get_scope();
          SgName targetName = functionDeclaration->get_name();

          while (i != declarationList.end())
             {
            // printf ("i = %p = %s = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());

            // Look for a prototype like what we want to insert, if we find it then we don't need to add a second one!
               SgTemplateInstantiationMemberFunctionDecl* templateMemberFunction = isSgTemplateInstantiationMemberFunctionDecl(*i);
               if (templateMemberFunction != NULL)
                  {
                 // Check the name and the scope
                 // if (memberFunctionInstantiation->get_scope() == templateMemberFunction->get_scope())
                    if (targetScope == templateMemberFunction->get_scope())
                       {
                      // printf ("Testing member function = %s against member function = %s \n",memberFunctionInstantiation->get_name().str(),templateMemberFunction->get_name().str());
                      // if (memberFunctionInstantiation->get_name() == templateMemberFunction->get_name())
                         if (targetName == templateMemberFunction->get_name())
                            {
                           // Don't count the defining declaration which may be at the end of the global scope.
                              if (templateMemberFunction->isForward() == true)
                                 {
                                   foundExistingPrototype = true;

                                // Exit the loop (optimization)
                                   break;
                                 }
                            }
                       }
                  }

               i++;
             }
        }

     return foundExistingPrototype;
   }

bool
SageInterface::isAncestor (SgNode* node1, SgNode* node2)
   {
     ROSE_ASSERT(node1 && node2);

     SgNode* curnode = node2;
     if (node1==node2)
        {
         return false;
        }

     do {
          curnode= curnode->get_parent();
        } while( (curnode!=NULL)&&(curnode!=node1));

     if (curnode==node1)
        {
          return true;
        }
       else
        {
          return false;
        }
   }

std::vector<SgNode*>
SageInterface::astIntersection ( SgNode* original, SgNode* copy, SgCopyHelp* help )
   {
     std::vector<SgNode*> intersectionSet;

  // Collect all the IR nodes for the original AST
     std::vector<SgNode*> AST_original = NodeQuery::querySubTree (original,V_SgNode);
     std::vector<SgNode*> AST_copy     = NodeQuery::querySubTree (copy,V_SgNode);

     int AST_original_size = AST_original.size();
     int AST_copy_size     = AST_copy.size();

     if (SgProject::get_verbose() > 0)
        {
          printf ("Original AST size         = %d \n",AST_original_size);
          printf ("Copy of original AST size = %d \n",AST_copy_size);
        }

     int differenceInSizes = AST_original_size - AST_copy_size;
     if (differenceInSizes == 0)
        {
          if (SgProject::get_verbose() > 0)
               printf ("Copied AST is the SAME size as the original (size = %d) \n",AST_original_size);
        }
       else
        {
          printf ("Warning: Copied AST and the original are DIFFERENT sizes (original size = %d copyied size = %d) \n",AST_original_size,AST_copy_size);
        }

  // Compute the intersection (reference in the copy that point to the origal AST).
  // std::set<SgNode*> AST_set_original = AST_original;
     std::set<SgNode*> AST_set_original;
     for (int i = 0; i < AST_original_size; i++)
        {
          AST_set_original.insert(AST_original[i]);
        }

     std::set<SgNode*> AST_set_copy;
     for (int i = 0; i < AST_copy_size; i++)
        {
          AST_set_copy.insert(AST_copy[i]);
        }

     int size = AST_original_size;
     std::vector<SgNode*> intersectionList(size);

  // Is there a better way to do this? Build scratch space and then use the different between begin() and end to build another vector.
     std::vector<SgNode*>::iterator end = set_intersection(AST_set_original.begin(),AST_set_original.end(),AST_set_copy.begin(),AST_set_copy.end(),intersectionList.begin());

  // Build a new vector from the relevant elements of the intersectionList scratch space.
     std::vector<SgNode*> meaningIntersectionList = std::vector<SgNode*>(intersectionList.begin(),end);
     std::vector<SgNode*> deleteList;
     for (int i = 0; i < (int)meaningIntersectionList.size(); i++)
        {
       // printf ("     (before removing types) meaningIntersectionList[%d] = %p = %s \n",i,meaningIntersectionList[i],(meaningIntersectionList[i] != NULL) ? meaningIntersectionList[i]->class_name().c_str() : "NULL");
          if (meaningIntersectionList[i] != NULL && isSgType(meaningIntersectionList[i]) != NULL)
             {
               deleteList.push_back(meaningIntersectionList[i]);
             }
        }

  // Remove the types since they are allowed to be shared...
     if (SgProject::get_verbose() > 0)
          printf ("Remove the types that are allowed to be shared: deleteList.size() = %ld \n",(long)deleteList.size());

     for (std::vector<SgNode*>::iterator i = deleteList.begin(); i != deleteList.end(); i++)
        {
          meaningIntersectionList.erase(find(meaningIntersectionList.begin(),meaningIntersectionList.end(),*i));
        }

     if (SgProject::get_verbose() > 0)
          printf ("After removing the types there are meaningIntersectionList.size() = %ld \n",(long)meaningIntersectionList.size());

     for (int i = 0; i < (int)meaningIntersectionList.size(); i++)
        {
       // printf ("     meaningIntersectionList[%d] = %p = %s \n",i,meaningIntersectionList[i],(meaningIntersectionList[i] != NULL) ? meaningIntersectionList[i]->class_name().c_str() : "NULL");
          printf ("     meaningIntersectionList[%d] = %p = %s = %s \n",i,meaningIntersectionList[i],meaningIntersectionList[i]->class_name().c_str(),get_name(meaningIntersectionList[i]).c_str());
        }

     int unmatchedIRnodes = 0;
     if (help != NULL)
        {
          std::vector<SgNode*> tmp_AST_original; // = AST_original;
          std::vector<SgNode*> tmp_AST_copy; //     = AST_copy;

          int AST_original_size = AST_original.size();
          for (int j = 0; j < AST_original_size; j++)
             {
               if (AST_original[j] != NULL && isSgType(AST_original[j]) == NULL)
                  {
                    tmp_AST_original.push_back(AST_original[j]);
                  }
             }

          int AST_copy_size = AST_copy.size();
          for (int j = 0; j < AST_copy_size; j++)
             {
               if (AST_copy[j] != NULL && isSgType(AST_copy[j]) == NULL)
                  {
                    tmp_AST_copy.push_back(AST_copy[j]);
                  }
             }

          std::vector<SgNode*> deleteList_original;
          std::vector<SgNode*> deleteList_copy;
          for (int j = 0; j < (int)tmp_AST_original.size(); j++)
             {
            // printf ("     (before removing types) meaningIntersectionList[%d] = %p = %s \n",i,meaningIntersectionList[i],(meaningIntersectionList[i] != NULL) ? meaningIntersectionList[i]->class_name().c_str() : "NULL");
               SgCopyHelp::copiedNodeMapTypeIterator i = help->get_copiedNodeMap().find(tmp_AST_original[j]);
            // printf ("SgCopyHelp::copiedNodeMapTypeIterator i != help->get_copiedNodeMap().end() = %s \n",i != help->get_copiedNodeMap().end() ? "true" : "false");
               if (i != help->get_copiedNodeMap().end())
                  {
                 // Matched up an original IR node with it's copy
                    SgNode* associated_node_copy = i->second;
                    ROSE_ASSERT(associated_node_copy != NULL);
                    deleteList_original.push_back(tmp_AST_original[j]);
                    deleteList_copy.push_back(associated_node_copy);
#if 0
                 // printf ("Original IR node = %p = %s copy = %p \n",tmp_AST_original[j],tmp_AST_original[j]->class_name().c_str(),associated_node_copy);
                    SgTemplateInstantiationMemberFunctionDecl* templateMemberFunction = isSgTemplateInstantiationMemberFunctionDecl(tmp_AST_original[j]);
                    SgTemplateInstantiationMemberFunctionDecl* templateMemberFunction_copy = isSgTemplateInstantiationMemberFunctionDecl(associated_node_copy);
                    if (templateMemberFunction != NULL)
                       {
                         printf ("In SageInterface::astIntersection(): Found a SgTemplateInstantiationMemberFunctionDecl = %p copy = %p \n",templateMemberFunction,associated_node_copy);
                      // templateMemberFunction->get_startOfConstruct()->display("original: debug");
                      // templateMemberFunction_copy->get_startOfConstruct()->display("copy: debug");
                       }
#endif
                  }
             }

          int deleteList_original_size = deleteList_original.size();
          for (int j = 0; j < deleteList_original_size; j++)
             {
            // tmp_AST_original.erase(find(tmp_AST_original.begin(),tmp_AST_original.end(),deleteList_original[j]));
               std::vector<SgNode*>::iterator k = find(tmp_AST_original.begin(),tmp_AST_original.end(),deleteList_original[j]);
               if (k != tmp_AST_original.end())
                  {
                    tmp_AST_original.erase(k);
                  }
             }

          if (SgProject::get_verbose() > 0)
               printf ("IR nodes different between the original AST and the copy of the AST = %zu \n",tmp_AST_original.size());

          for (int j = 0; j < (int)tmp_AST_original.size(); j++)
             {
               printf ("non matched IR node = %p = %s = %s \n",tmp_AST_original[j],tmp_AST_original[j]->class_name().c_str(),get_name(tmp_AST_original[j]).c_str());
            // tmp_AST_original[j]->get_startOfConstruct()->display("debug");
             }

          unmatchedIRnodes = (int)tmp_AST_original.size();
        }

  // DQ (11/2/2007): Make this an error now!
     if (differenceInSizes != 0)
        {
          SgProject* originalProject = isSgProject(original);
          if (originalProject != NULL)
             {
               printf ("In %s Copied AST and the original are DIFFERENT sizes (original size = %d copyied size = %d) IR nodes different = %d \n",
                    (*originalProject)[0]->get_sourceFileNameWithoutPath().c_str(),AST_original_size,AST_copy_size,unmatchedIRnodes);
             }

          if (unmatchedIRnodes > 0)
             {
               printf ("Make this an error under stricter testing \n");
               ROSE_ASSERT(false);
             }
        }

     return intersectionSet;
   }

// AJ (10/21/2004): Added support for changing the symbol name associated with an SgInitializedName
// by updating the symbol table
int
SageInterface::set_name ( SgInitializedName *initializedNameNode, SgName new_name )
   {
  // find the appropriate symbol table, delete the symbol
  // with the old name and add a symbol with the new name.
     ROSE_ASSERT(initializedNameNode != NULL);

  // SgNode * node = this;

#if 0
  // DQ (12/9/2004): This should likely call the get_scope function (which is more robust than traversing
  // parents, there is a reason why we are forced to include the scope explicitly on some IR nodes,
  // see test2004_133.C for details).
     while((node!=NULL) && ( isSgScopeStatement(node)==NULL))
          node = node->get_parent();

     ROSE_ASSERT(node!=NULL);

     SgScopeStatement * scope_stmt = isSgScopeStatement(node);
#else
     SgScopeStatement *scope_stmt = initializedNameNode->get_scope();
#endif

     ROSE_ASSERT(scope_stmt != NULL);
     ROSE_ASSERT(scope_stmt->get_symbol_table() != NULL);
     ROSE_ASSERT(scope_stmt->get_symbol_table()->get_table() != NULL);

     SgDeclarationStatement * parent_declaration = initializedNameNode->get_declaration();

     ROSE_ASSERT(parent_declaration != NULL);

  // Find the symbols associated with p_name
     std::pair<SgSymbolTable::hash_iterator,SgSymbolTable::hash_iterator> pair_it = scope_stmt->get_symbol_table()->get_table()->equal_range(initializedNameNode->get_name());

     SgSymbolTable::hash_iterator found_it = scope_stmt->get_symbol_table()->get_table()->end();

     for (SgSymbolTable::hash_iterator it = pair_it.first; it != pair_it.second; ++it)
        {
          switch(parent_declaration->variantT())
             {
               case V_SgVariableDeclaration:
                  {
                    if (isSgVariableSymbol((*it).second)!=NULL)
                         found_it = it;
                    break;
                  }
               case V_SgClassDeclaration:
                  {
                    if (isSgClassSymbol((*it).second)!=NULL)
                         found_it = it;
                    break;
                  }
               case V_SgFunctionDeclaration:
                  {
                    if (isSgFunctionSymbol((*it).second)!=NULL)
                         found_it = it;
                    break;
                  }
               default:
                  {
                  }
             };
        }

  // there is no Variable,Class or Function symbol associated with p_name
     if (found_it == scope_stmt->get_symbol_table()->get_table()->end())
        {
          printf ("Warning: There is no Variable,Class or Function symbol associated with p_name \n");
          return 0;
        }

     SgSymbol * associated_symbol = (*found_it).second;

  // erase the name from there
     scope_stmt->get_symbol_table()->get_table()->erase(found_it);

  // insert the new_name in the symbol table
// CH (4/9/2010): Use boost::unordered instead
//#ifdef _MSCx_VER
#if 0
  // DQ (11/28/2009): Unclear if this code is a problem (testing).

// CH (4/7/2010): It seems that the following code can be compiled under MSVC 9.0
//#pragma message ("WARNING: this code does not apprear to compile with MSVC.")
//       printf ("ERROR: this code does not apprear to compile with MSVC. \n");
//       ROSE_ASSERT(false);
     found_it = scope_stmt->get_symbol_table()->get_table()->insert(pair<SgName,SgSymbol*> ( new_name,associated_symbol));
#else
     found_it = scope_stmt->get_symbol_table()->get_table()->insert(pair<SgName,SgSymbol*> ( new_name,associated_symbol));
#endif
  // if insertion failed
     if (found_it == scope_stmt->get_symbol_table()->get_table()->end())
        {
          printf ("Warning: insertion of new symbol failed \n");
          return 0;
        }

  // Set the p_name to the new_name
     printf ("Reset initializedNameNode->get_name() = %s to new_name = %s \n",initializedNameNode->get_name().str(),new_name.str());
  // p_name = new_name;
     initializedNameNode->set_name(new_name);

  // Invalidate the p_iterator, p_no_name and p_name data members in the Symbol table

     return 1;
   }

string
SageInterface::get_name ( const SgC_PreprocessorDirectiveStatement* directive )
   {
     string name = "undefined_name";

     ROSE_ASSERT(directive != NULL);

     name = directive->class_name();

#if 1
  // I don't think we need this code now!
     switch (directive->variantT())
        {
       // Separate out these cases...
          case V_SgIncludeDirectiveStatement:
          case V_SgDefineDirectiveStatement:
          case V_SgUndefDirectiveStatement:
          case V_SgIfdefDirectiveStatement:
          case V_SgIfndefDirectiveStatement:
          case V_SgDeadIfDirectiveStatement:
          case V_SgIfDirectiveStatement:
          case V_SgElseDirectiveStatement:
          case V_SgElseifDirectiveStatement:
          case V_SgLineDirectiveStatement:
          case V_SgWarningDirectiveStatement:
          case V_SgErrorDirectiveStatement:
          case V_SgEmptyDirectiveStatement:
             {
               name = directive->class_name();
               break;
             }

       // case ClinkageDeclarationStatement:
          case V_SgClinkageStartStatement:
          case V_SgClinkageEndStatement:
             {
               name = directive->class_name();
               break;
             }

          case V_SgFortranIncludeLine:
             {
               name = directive->class_name();
               break;
             }

          default:
            // name = "default name (default case reached: not handled)";
               printf ("Warning: default case reached in SageInterface::get_name ( const SgC_PreprocessorDirectiveStatement* directive ), directive = %p = %s \n",
                    directive,directive->class_name().c_str());
               ROSE_ASSERT(false);

               name = "directive_default_name_case_reached_not_handled";
               break;
        }
#endif

     return name;
   }

string
SageInterface::get_name ( const SgDeclarationStatement* declaration )
   {
     string name = "undefined_name";

     ROSE_ASSERT(declaration != NULL);

  // DQ (11/23/2008): Handle the case of a Cpp directive...
     const SgC_PreprocessorDirectiveStatement* directive = isSgC_PreprocessorDirectiveStatement(declaration);
     if (directive != NULL)
        {
          return SageInterface::get_name (directive);
        }

     switch (declaration->variantT())
        {
       // DQ (12/28/2011): Added seperate support for new design of template IR nodes.
       // DQ (12/4/2011): Added support for template declarations in the AST.
          case V_SgTemplateMemberFunctionDeclaration:
               name = isSgTemplateMemberFunctionDeclaration(declaration)->get_name().str();
               break;

       // DQ (12/28/2011): Added seperate support for new design of template IR nodes.
          case V_SgTemplateFunctionDeclaration:
               name = isSgTemplateFunctionDeclaration(declaration)->get_name().str();
               break;

       // DQ (12/28/2011): Added seperate support for new design of template IR nodes.
       // DQ (6/11/2011): Added support for new template IR nodes.
          case V_SgTemplateClassDeclaration:
               name = isSgTemplateClassDeclaration(declaration)->get_name().str();
               break;

          case V_SgTemplateDeclaration:
               name = isSgTemplateDeclaration(declaration)->get_name().str();
               break;

       // DQ (2/10/2007): Use the get_templateName() instead of combining this with the case V_SgClassDeclaration
          case V_SgTemplateInstantiationDecl:
               name = isSgTemplateInstantiationDecl(declaration)->get_templateName().str();
               break;

          case V_SgClassDeclaration:
          case V_SgDerivedTypeStatement:
               name = isSgClassDeclaration(declaration)->get_name().str();
               break;

          case V_SgEnumDeclaration:
               name = isSgEnumDeclaration(declaration)->get_name().str();
               break;

          case V_SgTypedefDeclaration:
               name = isSgTypedefDeclaration(declaration)->get_name().str();
               break;

          case V_SgFunctionDeclaration:
          case V_SgProgramHeaderStatement:
          case V_SgProcedureHeaderStatement:
          case V_SgMemberFunctionDeclaration:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
               name = isSgFunctionDeclaration(declaration)->get_name().str();
               break;

          case V_SgNamespaceDeclarationStatement:
               name = isSgNamespaceDeclarationStatement(declaration)->get_name().str();
               break;

       // DQ (2/12/2006): Added support to get name of SgFunctionParameterList
          case V_SgFunctionParameterList:
             {
            // Parents should be set prior to calling these functions (if not we might have to implement that case)
               ROSE_ASSERT(declaration->get_parent() != NULL);
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration->get_parent());
               ROSE_ASSERT(functionDeclaration != NULL);
               name = get_name(functionDeclaration);
               name += "_parameter_list_";
               break;
             }

       // DQ (2/10/2012): Added support for template variable declarations (using base class support).
          case V_SgTemplateVariableDeclaration:

       // DQ (3/8/2006): Implemented case for variable declaration (forgot this case)
          case V_SgVariableDeclaration:
             {
            // DQ (2/11/2007): Modified to return names that can be used as variables (required
            // because we use this mechanism to generate names for unnamed structs and enums).
               name = "_variable_declaration_";
               const SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declaration);
               ROSE_ASSERT(variableDeclaration != NULL);
               SgInitializedNamePtrList::const_iterator i = variableDeclaration->get_variables().begin();

            // Make sure that we have at least one variable in the list
               ROSE_ASSERT(i != variableDeclaration->get_variables().end());
               do {
                 // name += string(" ") + string((*i)->get_name().str());
                    name += string((*i)->get_name().str());
                    i++;
                  }
               while (i != variableDeclaration->get_variables().end());
               break;
             }


       // DQ (3/8/2006): Implemented case for variable declaration (forgot this case)
          case V_SgVariableDefinition:
             {
            // DQ (2/11/2007): Modified to return names that can be used as variables (required
            // because we use this mechanism to generate names for unnamed structs and enums).
               name = "_variable_definition_";
               const SgVariableDefinition* variableDefinition = isSgVariableDefinition(declaration);
               ROSE_ASSERT(variableDefinition != NULL);

            // define this in terms of the associated SgInitializedName
               ROSE_ASSERT(variableDefinition->get_vardefn() != NULL);
               name += get_name(variableDefinition->get_vardefn());
               break;
             }

       // DQ (3/17/2006): Implemented case for pragma declaration (forgot this case)
          case V_SgPragmaDeclaration:
             {
               name = "_pragma_declaration_";
               const SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(declaration);
               ROSE_ASSERT(pragmaDeclaration != NULL);
               ROSE_ASSERT(pragmaDeclaration->get_pragma() != NULL);
               name += get_name(pragmaDeclaration->get_pragma());
               break;
             }

       // DQ (4/15/2007): Implemented case for using directive statement (forgot this case)
          case V_SgUsingDirectiveStatement:
             {
               name = "_using_directive_statement_";
               const SgUsingDirectiveStatement* usingDeclaration = isSgUsingDirectiveStatement(declaration);
               ROSE_ASSERT(usingDeclaration != NULL);
               ROSE_ASSERT(usingDeclaration->get_namespaceDeclaration() != NULL);
               name += get_name(usingDeclaration->get_namespaceDeclaration());
               break;
             }

       // DQ (6/20/2007): Added new case!
          case V_SgNamespaceAliasDeclarationStatement:
             {
               name = "_namespace_alias_directive_statement_";
               const SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(declaration);
               ROSE_ASSERT(namespaceAliasDeclaration != NULL);
               ROSE_ASSERT(namespaceAliasDeclaration->get_name().is_null() == false);
               name += namespaceAliasDeclaration->get_name();
               break;
             }

       // DQ (6/20/2007): Added new case!
          case V_SgUsingDeclarationStatement:
             {
               name = "_using_declaration_statement_";
               const SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(declaration);
               ROSE_ASSERT(usingDeclaration != NULL);
               if (usingDeclaration->get_declaration() != NULL)
                  {
                    name += get_name(usingDeclaration->get_declaration());
                  }
                 else
                  {
                    ROSE_ASSERT(usingDeclaration->get_initializedName() != NULL);
                    name += get_name(usingDeclaration->get_initializedName());
                  }
               break;
             }

       // DQ (6/20/2007): Added new case!
          case V_SgTemplateInstantiationDirectiveStatement:
             {
               name = "_template_instantiation_directive_statement_";
               ROSE_ASSERT(declaration != NULL);
               const SgTemplateInstantiationDirectiveStatement* templateInstantiationDirective = isSgTemplateInstantiationDirectiveStatement(declaration);
               ROSE_ASSERT(templateInstantiationDirective != NULL);
               ROSE_ASSERT(templateInstantiationDirective->get_declaration() != NULL);
               printf ("declaration->get_declaration() = %p = %s \n",templateInstantiationDirective->get_declaration(),templateInstantiationDirective->get_declaration()->class_name().c_str());
               name += get_name(templateInstantiationDirective->get_declaration());
               break;
             }

          case V_SgCtorInitializerList:
             {
               name = "_ctor_list_";
               const SgCtorInitializerList* ctorDeclaration = isSgCtorInitializerList(declaration);
               ROSE_ASSERT(ctorDeclaration != NULL);
               ROSE_ASSERT(ctorDeclaration->get_parent() != NULL);
               name += get_name(ctorDeclaration->get_parent());
               break;
             }

       // DQ (8/9/2007): Added case for SgAsmStmt
          case V_SgAsmStmt:
             {
               name = "_asm_stmt_";
               const SgAsmStmt* asmStatement = isSgAsmStmt(declaration);
               ROSE_ASSERT(asmStatement != NULL);
               ROSE_ASSERT(asmStatement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgAsmStmt*>(asmStatement));
               break;
             }

       // DQ (8/22/2007): Added case for SgImplicitStatement
          case V_SgImplicitStatement:
             {
               name = "_fortran_implicit_";
               const SgImplicitStatement* implicitStatement = isSgImplicitStatement(declaration);
               ROSE_ASSERT(implicitStatement != NULL);
               ROSE_ASSERT(implicitStatement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgImplicitStatement*>(implicitStatement));
               break;
             }

       // DQ (8/22/2007): Added case for SgNamelistStatement
          case V_SgNamelistStatement:
             {
               name = "_fortran_namelist_";
               const SgNamelistStatement* namelistStatement = isSgNamelistStatement(declaration);
               ROSE_ASSERT(namelistStatement != NULL);
               ROSE_ASSERT(namelistStatement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgNamelistStatement*>(namelistStatement));
               break;
             }

       // DQ (11/21/2007): Added case for SgEquivalenceStatement
          case V_SgEquivalenceStatement:
             {
               name = "_fortran_equivalence_";
               const SgEquivalenceStatement* equivalenceStatement = isSgEquivalenceStatement(declaration);
               ROSE_ASSERT(equivalenceStatement != NULL);
               ROSE_ASSERT(equivalenceStatement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgEquivalenceStatement*>(equivalenceStatement));
               break;
             }


       // DQ (11/21/2007): Added case for SgCommonBlock
          case V_SgCommonBlock:
             {
               name = "_fortran_common_block_";
               const SgCommonBlock* commonBlockStatement = isSgCommonBlock(declaration);
               ROSE_ASSERT(commonBlockStatement != NULL);
               ROSE_ASSERT(commonBlockStatement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgCommonBlock*>(commonBlockStatement));
               break;
             }

       // DQ (11/21/2007): Added case for SgImportStatement
          case V_SgImportStatement:
             {
               name = "_fortran_import_stmt_";
               const SgImportStatement* importStatement = isSgImportStatement(declaration);
               ROSE_ASSERT(importStatement != NULL);
               ROSE_ASSERT(importStatement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgImportStatement*>(importStatement));
               break;
             }

       // DQ (11/21/2007): Added case for SgFormatStatement
          case V_SgFormatStatement:
             {
               name = "_fortran_format_stmt_";
               const SgFormatStatement* formatStatement = isSgFormatStatement(declaration);
               ROSE_ASSERT(formatStatement != NULL);
               ROSE_ASSERT(formatStatement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgFormatStatement*>(formatStatement));
               break;
             }

       // DQ (12/27/2007): Added case for SgModuleStatement
          case V_SgModuleStatement:
             {
               name = "_fortran_module_stmt_";
               const SgModuleStatement* moduleStatement = isSgModuleStatement(declaration);
               ROSE_ASSERT(moduleStatement != NULL);
               ROSE_ASSERT(moduleStatement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgModuleStatement*>(moduleStatement));
               break;
             }

       // DQ (12/27/2007): Added case for SgUseStatement
          case V_SgUseStatement:
             {
               name = "_fortran_use_stmt_";
               const SgUseStatement* useStatement = isSgUseStatement(declaration);
               ROSE_ASSERT(useStatement != NULL);
               ROSE_ASSERT(useStatement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgUseStatement*>(useStatement));
               break;
             }

       // DQ (12/28/2007): Added case for SgContainsStatement
          case V_SgContainsStatement:
             {
               name = "_fortran_contains_stmt_";
               const SgContainsStatement* containsStatement = isSgContainsStatement(declaration);
               ROSE_ASSERT(containsStatement != NULL);
               ROSE_ASSERT(containsStatement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgContainsStatement*>(containsStatement));
               break;
             }

       // DQ (1/20/2008): Added case for SgEntryStatement
          case V_SgEntryStatement:
             {
               name = "_fortran_entry_stmt_";
               const SgEntryStatement* entryStatement = isSgEntryStatement(declaration);
               ROSE_ASSERT(entryStatement != NULL);
               ROSE_ASSERT(entryStatement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgEntryStatement*>(entryStatement));
               break;
             }

       // DQ (1/23/2008): Added case for SgAttributeSpecificationStatement
          case V_SgAttributeSpecificationStatement:
             {
               name = "_fortran_attribute_specification_stmt_";
               const SgAttributeSpecificationStatement* statement = isSgAttributeSpecificationStatement(declaration);
               ROSE_ASSERT(statement != NULL);
               ROSE_ASSERT(statement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgAttributeSpecificationStatement*>(statement));
               break;
             }

           case V_SgInterfaceStatement:
             {
               name = "_fortran_interface_stmt_";
               const SgInterfaceStatement* statement = isSgInterfaceStatement(declaration);
               ROSE_ASSERT(statement != NULL);
               ROSE_ASSERT(statement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgInterfaceStatement*>(statement));
               break;
             }

           case V_SgFortranIncludeLine:
             {
               name = "_fortran_include_line_stmt_";
               const SgFortranIncludeLine* statement = isSgFortranIncludeLine(declaration);
               ROSE_ASSERT(statement != NULL);
               ROSE_ASSERT(statement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgFortranIncludeLine*>(statement));
               break;
             }

        // DQ (4/16/2011): Added Java import statment support.
           case V_SgJavaImportStatement:
             {
               name = "_java_import_stmt_";
               const SgJavaImportStatement* statement = isSgJavaImportStatement(declaration);
               ROSE_ASSERT(statement != NULL);
               ROSE_ASSERT(statement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgJavaImportStatement*>(statement));
               break;
             }

           case V_SgJavaPackageDeclaration:
             {
               name = "_java_package_declaration_";
               const SgJavaPackageDeclaration* package_declaration = isSgJavaPackageDeclaration(declaration);
               ROSE_ASSERT(package_declaration != NULL);
               ROSE_ASSERT(package_declaration->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgJavaPackageDeclaration*>(package_declaration));
               break;
             }

           case V_SgJavaPackageStatement:
             {
               name = "_java_package_stmt_";
               const SgJavaPackageStatement* statement = isSgJavaPackageStatement(declaration);
               ROSE_ASSERT(statement != NULL);
               ROSE_ASSERT(statement->get_parent() != NULL);
               name += StringUtility::numberToString(const_cast<SgJavaPackageStatement*>(statement));
               break;
             }

       // Note that the case for SgVariableDeclaration is not implemented
          default:
            // name = "default name (default case reached: not handled)";
               printf ("Warning: default case reached in SageInterface::get_name ( const SgDeclarationStatement* declaration ), declaration = %p = %s \n",
                    declaration,declaration->class_name().c_str());
               ROSE_ASSERT(false);

               name = "default_name_case_reached_not_handled";
               break;
        }

     return name;
   }

string
SageInterface::get_name ( const SgScopeStatement* scope )
   {
     string name = "undefined_name";

     ROSE_ASSERT(scope != NULL);

     switch (scope->variantT())
        {
       // DQ (6/11/2011): Added support for new template IR nodes.
          case V_SgTemplateClassDefinition:
               name = get_name(isSgTemplateClassDefinition(scope)->get_declaration());
               break;

          case V_SgClassDefinition:
          case V_SgTemplateInstantiationDefn:
               name = get_name(isSgClassDefinition(scope)->get_declaration());
               break;

       // DQ (9/8/2012): Added missing case for SgTemplateFunctionDefinition.
          case V_SgTemplateFunctionDefinition:
          case V_SgFunctionDefinition:
               name = get_name(isSgFunctionDefinition(scope)->get_declaration());
               break;

          case V_SgNamespaceDefinitionStatement:
               name = get_name(isSgNamespaceDefinitionStatement(scope)->get_namespaceDeclaration());
               break;
          case V_SgJavaLabelStatement:
               name = (isSgJavaLabelStatement(scope)->get_label()).getString();
               break;

       // DQ (11/30/2007): Added more fortran support.
          case V_SgAssociateStatement:
          case V_SgJavaForEachStatement:

          case V_SgBasicBlock:
          case V_SgCatchOptionStmt:
          case V_SgDoWhileStmt:
          case V_SgForStatement:
          case V_SgGlobal:
          case V_SgIfStmt:
          case V_SgSwitchStatement:
          case V_SgWhileStmt:
          case V_SgFortranDo:
          case V_SgForAllStatement:
               name = StringUtility::numberToString(const_cast<SgScopeStatement*>(scope));
               break;

          default:
               printf ("Error: undefined case (SgScopeStatement) in SageInterface::get_name(): node = %s \n",scope->class_name().c_str());
               ROSE_ASSERT(false);
               break;
        }

     return name;
   }

string
SageInterface::get_name ( const SgStatement* stmt )
   {
     string name = "undefined_name";

     ROSE_ASSERT(stmt != NULL);

     const SgDeclarationStatement* declaration = isSgDeclarationStatement(stmt);
     if (declaration != NULL)
        {
          name = get_name(declaration);
        }
       else
        {
          const SgScopeStatement* scope = isSgScopeStatement(stmt);
          if (scope != NULL)
             {
               name = get_name(scope);
             }
            else
             {
#if 0
            // DQ (9/5/2005): I think this is good enough for the more general case
               name = stmt->class_name();
#else
            // DQ (10/25/2007): This is better since it names the SgLabelStatement case
               switch (stmt->variantT())
                  {
#if 0
                    case V_SgBreakStmt:
                    case V_SgCaseOptionStmt:
                    case V_SgCatchStatementSeq:
                    case V_SgClinkageStartStatement:
                    case V_SgContinueStmt:
                    case V_SgDefaultOptionStmt:
                    case V_SgExprStmt:
                    case V_SgForInitStmt:
                    case V_SgFunctionTypeTable:
                    case V_SgGotoStatement:
                    case V_SgReturnStmt:
                    case V_SgSpawnStmt:
                    case V_SgTryStmt:
                         name = stmt->class_name();
                         break;
#endif
                    case V_SgLabelStatement:
                       {
                         const SgLabelStatement* labelStatement = isSgLabelStatement(stmt);
                         name = labelStatement->get_label().str();
                         break;
                       }

                    default:
                       {
                      // printf ("Default reached in switch \n");
                      // name = "default name";
                         name = stmt->class_name();
                         break;
                       }
                  }
#endif
             }
        }

     return name;
   }

string
SageInterface::get_name ( const SgSupport* node )
   {
  // This function is useful for debugging
  // This is the most general case of a function to return a name for an IR node.
     ROSE_ASSERT(node != NULL);

     string name = "undefined_name";

     ROSE_ASSERT(node != NULL);

     switch (node->variantT())
        {
          case V_SgInitializedName:
             {
               const SgInitializedName* initializedName = isSgInitializedName(node);
               if (initializedName != NULL)
                  {
                    name = initializedName->get_name().str();
                  }
               break;
             }
#if 1
       // DQ (3/17/2006): Implemented case for pragma declaration (forgot this case)
          case V_SgPragma:
             {
               name = "_pragma_string_";
               const SgPragma* pragma = isSgPragma(node);
               ROSE_ASSERT(pragma != NULL);
               name += pragma->get_pragma();
               break;
             }
#endif

       // DQ (5/31/2007): Implemented case for SgProject
          case V_SgProject:
             {
               name = "_project_";
               break;
             }

       // DQ (5/31/2007): Implemented case for SgFile
       // case V_SgFile:
          case V_SgSourceFile:
          case V_SgBinaryComposite:
             {
               name = "_file_";

            // DQ (3/1/2009): Added support to include the file name.
               const SgFile* file = isSgFile(node);
               ROSE_ASSERT(file != NULL);

               name += file->getFileName();
               break;
             }

       // DQ (5/31/2007): Implemented case for SgSymbolTable
          case V_SgSymbolTable:
             {
               name = "_symbol_table_";
               break;
             }

       // DQ (5/31/2007): Implemented case for SgStorageModifier
          case V_SgStorageModifier:
             {
               name = "_storage_modifier_";
               break;
             }


       // DQ (5/31/2007): Implemented case for Sg_File_Info
          case V_Sg_File_Info:
             {
               name = "_file_info_";
               break;
             }

       // DQ (8/7/2007): Implemented case for SgTemplateArgument
          case V_SgTemplateArgument:
             {
               name = "_template_argument_";
               const SgTemplateArgument* templateArgument = isSgTemplateArgument(node);
               ROSE_ASSERT(templateArgument != NULL);
               switch(templateArgument->get_argumentType())
                  {
                    case SgTemplateArgument::type_argument:
                       {
                         SgType* t = templateArgument->get_type();
                         ROSE_ASSERT(t != NULL);
                         name += get_name(t);
                         break;
                       }

                    case SgTemplateArgument::nontype_argument:
                       {
                         SgExpression* t = templateArgument->get_expression();

                      // DQ (8/11/2013): Adding support for including an initializedName instead of an expression (for template parameters used as expressions).
                      // ROSE_ASSERT(t != NULL);
                      // name += get_name(t);
                         if (t != NULL)
                            {
                              ROSE_ASSERT(templateArgument->get_initializedName() == NULL);
                              name += get_name(t);
                            }
                           else
                            {
                              ROSE_ASSERT(t == NULL);
                              SgInitializedName* initializedName = templateArgument->get_initializedName();
                              ROSE_ASSERT(initializedName != NULL);

                              name += get_name(initializedName);
                            }
                         break;
                       }

                    case SgTemplateArgument::template_template_argument:
                       {
                      // SgTemplateDeclaration* t = templateArgument->get_templateDeclaration();
                         SgDeclarationStatement* t = templateArgument->get_templateDeclaration();
                         ROSE_ASSERT(t != NULL);
                         name += get_name(t);
                         break;
                       }

                    default:
                       {
                         printf ("Error: default case reached in switch on templateArgument->get_argumentType() \n");
                         ROSE_ASSERT(false);
                       }
                  }
               break;
             }


       // DQ (8/7/2007): Implemented case for SgTemplateArgument
          case V_SgTypeModifier:
             {
               const SgTypeModifier* typeModifier = isSgTypeModifier(node);
               ROSE_ASSERT(typeModifier != NULL);
               name = "_type_modifier_" + typeModifier->displayString();
               break;
             }

       // DQ (11/19/2007): Implemented case for SgNameGroup
          case V_SgNameGroup:
             {
               const SgNameGroup* nameGroup = isSgNameGroup(node);
               ROSE_ASSERT(nameGroup != NULL);
               name = "_name_group_" + nameGroup->get_group_name();
               break;
             }

       // DQ (11/20/2007): Implemented case for Fortran data statement support
          case V_SgDataStatementGroup:
             {
               const SgDataStatementGroup* dataGroup = isSgDataStatementGroup(node);
               ROSE_ASSERT(dataGroup != NULL);
               name = "_data_statement_group_";
               break;
             }

       // DQ (11/20/2007): Implemented case for Fortran data statement support
          case V_SgDataStatementObject:
             {
               const SgDataStatementObject* dataObject = isSgDataStatementObject(node);
               ROSE_ASSERT(dataObject != NULL);
               name = "_data_statement_object_";
               break;
             }

       // DQ (11/20/2007): Implemented case for Fortran data statement support
          case V_SgDataStatementValue:
             {
               const SgDataStatementValue* dataValue = isSgDataStatementValue(node);
               ROSE_ASSERT(dataValue != NULL);
               name = "_data_statement_value_";
               break;
             }

       // DQ (11/19/2007): Implemented case for SgCommonBlock
          case V_SgCommonBlockObject:
             {
               const SgCommonBlockObject* commonBlockObject = isSgCommonBlockObject(node);
               ROSE_ASSERT(commonBlockObject != NULL);
               name = "_common_block_object_" + commonBlockObject->get_block_name();
               break;
             }

       // DQ (12/23/2007): Added support for repeat_specification
          case V_SgFormatItem:
             {
               const SgFormatItem* formatItem = isSgFormatItem(node);
               ROSE_ASSERT(formatItem != NULL);
               name = "_format_item_";
               break;
             }

       // DQ (12/23/2007): Added support for repeat_specification
          case V_SgFormatItemList:
             {
               const SgFormatItemList* formatItemList = isSgFormatItemList(node);
               ROSE_ASSERT(formatItemList != NULL);
               name = "_format_item_list_";
               break;
             }

       // DQ (12/23/2007): Added support for repeat_specification
          case V_SgRenamePair:
             {
               const SgRenamePair* renamePair = isSgRenamePair(node);
               ROSE_ASSERT(renamePair != NULL);
               name = renamePair->get_local_name() + "__" + renamePair->get_use_name() + "_rename_pair_";
               break;
             }

       // DQ (12/2/2010): Implemented case for SgName
          case V_SgName:
             {
               const SgName* name_node = isSgName(node);
               ROSE_ASSERT(name_node != NULL);
               name = "_name_" + name_node->getString();
               break;
             }

       // DQ (8/8/2013): Implemented case for SgTemplateParameter
          case V_SgTemplateParameter:
             {
               const SgTemplateParameter* template_parameter_node = isSgTemplateParameter(node);
               ROSE_ASSERT(template_parameter_node != NULL);
               name = "_template_parameter_";

               switch(template_parameter_node->get_parameterType())
                  {
                    case SgTemplateParameter::type_parameter:
                       {
                         name += "type_parameter_";
                         break;
                       }

                    case SgTemplateParameter::nontype_parameter:
                       {
                         name += "nontype_parameter_";
#if 1
                         name += template_parameter_node->unparseToString();
#else
                         if (template_parameter_node->get_expression() != NULL)
                            {
                              name += template_parameter_node->get_expression()->unparseToString();
                            }
                           else
                            {
                              ROSE_ASSERT(template_parameter_node->get_initializedName() != NULL);

                           // DQ (8/8/2013): This does not handle the case of "template <void (foo::*M)()> void test() {}"
                           // since what is unparsed is: "_template_parameter_nontype_parameter_M"
                           // instead of a string to represent what is in "void (foo::*M)()"
                           // and differentiate it from: "int foo::*M" in: "template <void (foo::*M)()> void test() {}"
                              name += template_parameter_node->get_initializedName()->unparseToString();
                            }
#endif
                         break;
                       }

                    case SgTemplateParameter::template_parameter:
                       {
                         name += "template_parameter_";
                         break;
                       }

                    default:
                       {
                         printf ("Error: default reached \n");
                         ROSE_ASSERT(false);
                         break;
                       }
                  }
               break;
             }

          default:
             {
               printf ("Default reached in switch for SgSupport IR node = %s \n",node->class_name().c_str());
               name = "default name";
            // ROSE_ASSERT(false);
               break;
             }
        }

     return name;
   }


string
SageInterface::get_name ( const SgSymbol* symbol )
   {
  // This function is useful for debugging
  // This is the most general case of a function to return a name for an IR node.
     ROSE_ASSERT(symbol != NULL);

     string aliasSymbolPrefix = "";
     if (isSgAliasSymbol(symbol) != NULL)
        {
          aliasSymbolPrefix = "_ALIAS";
        }

  // printf ("In SageInterface::get_name(): symbol->get_symbol_basis() = %p = %s \n",symbol->get_symbol_basis(),symbol->get_symbol_basis()->class_name().c_str());
  // printf ("In SageInterface::get_name(): symbol->get_declaration() = %p = %s \n",symbol->get_declaration(),symbol->get_declaration()->class_name().c_str());

  // This is a call to the "get_name()" virtual function
     return symbol->get_name() + aliasSymbolPrefix + "_symbol_";
   }

string
SageInterface::get_name ( const SgType* type )
   {
  // This function is useful for debugging
  // This is the most general case of a function to return a name for an IR node.
     ROSE_ASSERT(type != NULL);

     string returnName;

     const SgNamedType* namedType = isSgNamedType(type);
     if (namedType != NULL)
        {
          returnName = "named_type_";
          returnName = namedType->get_name().getString();
        }
       else
        {
          switch(type->variantT())
             {
               case V_SgPointerType:
                  {
                    const SgPointerType* pointerType = isSgPointerType(type);
                    returnName = "pointer_to_";
                    returnName += get_name(pointerType->get_base_type());
                    break;
                  }

               case V_SgReferenceType:
                  {
                    const SgReferenceType* referenceType = isSgReferenceType(type);
                    returnName = "reference_to_";
                    returnName += get_name(referenceType->get_base_type());
                    break;
                  }

               case V_SgArrayType:
                  {
                    const SgArrayType* arrayType = isSgArrayType(type);
                    returnName = "array_of_";
                    returnName += get_name(arrayType->get_base_type());
                    break;
                  }

               case V_SgModifierType:
                  {
                    const SgModifierType* modifierType = isSgModifierType(type);
                    returnName = get_name(&(modifierType->get_typeModifier()));
                    returnName += get_name(modifierType->get_base_type());
                    break;
                  }

            // DQ (8/26/2012): Added case to support template declaration work.
               case V_SgTemplateType:
                  {
                    const SgTemplateType* templateType = isSgTemplateType(type);
                    returnName = "templateType_";
                    returnName += templateType->get_name();
                    break;
                  }

               default:
                  {
                    returnName = type->class_name();
                  }
             }
        }

     return returnName;
   }

string
SageInterface::get_name ( const SgExpression* expr )
   {
     string name = "undefined_name";

  // ROSE_ASSERT(expr != NULL);
     switch(expr->variantT())
        {
          case V_SgVarRefExp:
             {
               const SgVarRefExp* varRef = isSgVarRefExp(expr);
               name = "var_ref_of_";
               ROSE_ASSERT(varRef != NULL);
               ROSE_ASSERT(varRef->get_symbol() != NULL);
               name += varRef->get_symbol()->get_name();
               break;
             }

       // DQ (2/2/2011): Added case to support fortran use of label references in alternate return parameters.
          case V_SgLabelRefExp:
             {
               const SgLabelRefExp* labelRef = isSgLabelRefExp(expr);
               name = "label_ref_of_";
               ROSE_ASSERT(labelRef != NULL);
               ROSE_ASSERT(labelRef->get_symbol() != NULL);
               name += labelRef->get_symbol()->get_name();
               break;
             }

          case V_SgPntrArrRefExp:
             {
               const SgPntrArrRefExp* arrayRef = isSgPntrArrRefExp(expr);
               name = "array_ref_of_";
               name += get_name(arrayRef->get_lhs_operand());
               name += "_at_";
               name += get_name(arrayRef->get_rhs_operand());
               break;
             }

          case V_SgFunctionCallExp:
             {
               const SgFunctionCallExp* functionCall = isSgFunctionCallExp(expr);
               name = "function_call_";
               name += get_name(functionCall->get_function());
               break;
             }

          case V_SgFunctionRefExp:
             {
               const SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(expr);
               name = "function_ref_";
               name += functionRefExp->get_symbol()->get_name();
               break;
             }

       // DQ (4/19/2013): Added support for SgMemberFunctionRefExp.
          case V_SgMemberFunctionRefExp:
             {
               const SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(expr);
               name = "member_function_ref_";
               name += memberFunctionRefExp->get_symbol()->get_name();
               break;
             }

          case V_SgIntVal:
             {
               const SgIntVal* valueExp = isSgIntVal(expr);
               name = "integer_value_exp_";
               name += StringUtility::numberToString(valueExp->get_value());
               break;
             }

          case V_SgStringVal:
             {
               const SgStringVal* valueExp = isSgStringVal(expr);
               name = "string_value_exp_";
               name += valueExp->get_value();
               break;
             }

          case V_SgSubscriptExpression:
             {
               const SgSubscriptExpression* subscriptExpression = isSgSubscriptExpression(expr);
               name = "subscript_exp_";
            // name += StringUtility::numberToString(subscriptExpression->get_lowerBound());
               name += get_name(subscriptExpression->get_lowerBound());
               name += "_";
            // name += StringUtility::numberToString(subscriptExpression->get_upperBound());
               name += get_name(subscriptExpression->get_upperBound());
               name += "_";
            // name += StringUtility::numberToString(subscriptExpression->get_stride());
               name += get_name(subscriptExpression->get_stride());
               break;
             }

          case V_SgNullExpression:
             {
               name = "null_expression";
               break;
             }

       // DQ (1/17/2011): Added support for SgExprListExp (to support debugging).
          case V_SgExprListExp:
             {
               const SgExprListExp* exprListExp = isSgExprListExp(expr);
               name = "expr_list_exp_";
               for (size_t i = 0; i < exprListExp->get_expressions().size(); i++)
                  {
                    name += get_name(exprListExp->get_expressions()[i]);
                  }
               break;
             }

       // DQ (1/31/2011): Added to support Fortran debugging.
          case V_SgActualArgumentExpression:
             {
               const SgActualArgumentExpression* actualArgExp = isSgActualArgumentExpression(expr);
               name = "actual_arg_exp_name_";
               name += actualArgExp->get_argument_name();
               name = "_exp_";
               name += get_name(actualArgExp->get_expression());
               break;
             }

       // DQ (7/25/2012): Added support for new template IR nodes.
          case V_SgTemplateParameterVal:
             {
               const SgTemplateParameterVal* valueExp = isSgTemplateParameterVal(expr);
               name = "template_parameter_value_expression_number_";
            // name += valueExp->get_value();
            // name += get_name(valueExp);
               name += StringUtility::numberToString(valueExp->get_template_parameter_position());
               break;
             }

       // DQ (4/19/2013): Added support for SgDotExp.
          case V_SgDotExp:
             {
               const SgDotExp* dotExp = isSgDotExp(expr);
               ROSE_ASSERT(dotExp != NULL);

               name = "_dot_exp_lhs_";
               name += get_name(dotExp->get_lhs_operand());
               name += "_dot_exp_rhs_";
               name += get_name(dotExp->get_rhs_operand());
            // name += StringUtility::numberToString(valueExp->get_template_parameter_position());
               break;
             }

          default:
             {
            // Nothing to do for other IR nodes
#if 0
            // DQ (7/25/2012): Make this an error.
               printf ("Note: default reached in get_name() expr = %p = %s \n",expr,expr->class_name().c_str());
               ROSE_ASSERT(false);
#endif
            // DQ (4/8/2010): define something specific to this function to make debugging more clear.
            // printf ("Note: default reached in get_name() expr = %p = %s \n",expr,expr->class_name().c_str());
              // name = "undefined_expression_name";
               name = expr->class_name() + "_undef_name";
               break;
             }
        }

     return name;
   }

string
SageInterface::get_name ( const SgLocatedNodeSupport* node )
   {
  // This function is useful for debugging
  // This is the most general case of a function to return a name for an IR node.
     ROSE_ASSERT(node != NULL);

     string returnName;

     switch(node->variantT())
        {
          case V_SgRenamePair:
             {
               const SgRenamePair* n = isSgRenamePair(node);
               returnName = "rename_pair_";
               returnName += n->get_local_name().str();
               returnName += "_from_";
               returnName += n->get_use_name().str();
               break;
             }
          case V_SgInitializedName:
          {
            const SgInitializedName * n = isSgInitializedName (node);
            ROSE_ASSERT (n != NULL);
            returnName = "initialized_name_";
            returnName += n->get_name().str();

            break;
          }
#if 0
          case V_SgInterfaceBody:
             {
               const SgInterfaceBody* n = isSgInterfaceBody(node);
               returnName = "interface_body";
               break;
             }
#endif
          default:
             {
               returnName = node->class_name();
             }
        }

     return returnName;
   }

string
SageInterface::get_name ( const SgNode* node )
   {
  // This function is useful for debugging
  // This is the most general case of a function to return a name for an IR node.
  // Later this function will handle expressions, etc.

     string name = "undefined_name";

     ROSE_ASSERT(node != NULL);

     const SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
          const SgStatement* statement = isSgStatement(node);
          if (statement != NULL)
             {
               name = get_name(statement);
             }
            else
             {
               const SgExpression* expression = isSgExpression(node);
               if (expression != NULL)
                  {
                    name = get_name(expression);
                  }
                 else
                  {
                    const SgLocatedNodeSupport* locatedNodeSupport = isSgLocatedNodeSupport(node);
                    if (locatedNodeSupport != NULL)
                       {
                         name = get_name(locatedNodeSupport);
                       }
                      else
                       {
                         const SgToken* token = isSgToken(node);
                         if (token != NULL)
                            {
                              name = get_name(token);
                            }
                           else
                            {
                              printf ("Unknown SgLocatedNode = %p = %s \n",node,node->class_name().c_str());
                              ROSE_ASSERT(false);
                            }
                       }
                  }
             }
        }
       else
        {
          const SgSupport* supportNode = isSgSupport(node);
          if (supportNode != NULL)
             {
               name = get_name(supportNode);
             }
            else
             {
            // DQ (9/21/2005): I think this is good enough for the more general case (for now)
               const SgSymbol* symbol = isSgSymbol(node);
               if (symbol != NULL)
                  {
                    name = get_name(symbol);
                  }
                 else
                  {
                 // DQ (8/7/2007): Added support to get names of types (usefult for SgNamedType objects).
                    const SgType* type = isSgType(node);
                    if (type != NULL)
                       {
                         name = get_name(type);
                       }
                      else
                       {
                         name = node->class_name();
                       }
                  }
             }
        }

     return name;
   }


string
SageInterface::get_name ( const SgToken* token )
   {
  // This function is useful for debugging
     string name = "undefined_name";

     ROSE_ASSERT(token != NULL);
     name = token->get_lexeme_string();

  // Handle special cases
     if (name == " ")
        name = "<space>";
     else if (name == "\n")
        name = "<eol>";
     else if (name == "\t")
        name = "<tab>";

     return name;
   }


SgMemberFunctionDeclaration*
SageInterface::getDefaultConstructor( SgClassDeclaration* classDeclaration )
   {
     SgMemberFunctionDeclaration* defaultConstructor = NULL;

     ROSE_ASSERT(classDeclaration != NULL);
     SgDeclarationStatement* definingDeclaration = classDeclaration->get_definingDeclaration();
     ROSE_ASSERT(definingDeclaration != NULL);
     SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(definingDeclaration);

  // Note that not all class declarations have to have a defining declaration (e.g. "~int()"
  // would be associated with a class declaration but no definition would exist).
     if (definingClassDeclaration != NULL)
        {
          SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
          ROSE_ASSERT(classDefinition != NULL);

          SgDeclarationStatementPtrList::iterator i = classDefinition->get_members().begin();
          while ( i != classDefinition->get_members().end() )
             {
            // Check the parent pointer to make sure it is properly set
               ROSE_ASSERT( (*i)->get_parent() != NULL);
               ROSE_ASSERT( (*i)->get_parent() == classDefinition);

               SgMemberFunctionDeclaration* memberFunction = isSgMemberFunctionDeclaration(*i);
               if (memberFunction != NULL)
                  {
                 // printf ("memberFunction = %p = %s \n",memberFunction,memberFunction->get_name().str());
                    if ( memberFunction->get_specialFunctionModifier().isConstructor() == true )
                       defaultConstructor = memberFunction;
                  }

            // iterate through the class members
               i++;
             }
        }

  // This should be true for the specific case that we are currently debugging!
  // ROSE_ASSERT(defaultConstructor != NULL);

     return defaultConstructor;
   }

SgMemberFunctionDeclaration*
SageInterface::getDefaultDestructor( SgClassDeclaration* classDeclaration )
   {
     SgMemberFunctionDeclaration* defaultDestructor = NULL;

     ROSE_ASSERT(classDeclaration != NULL);
     SgDeclarationStatement* definingDeclaration = classDeclaration->get_definingDeclaration();
     if (definingDeclaration != NULL)
        {
          ROSE_ASSERT(definingDeclaration != NULL);
          SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(definingDeclaration);

       // Note that not all class declarations have to have a defining declaration (e.g. "~int()"
       // would be associated with a class declaration but no definition would exist).
          if (definingClassDeclaration != NULL)
             {
               SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
               ROSE_ASSERT(classDefinition != NULL);

               SgDeclarationStatementPtrList::iterator i = classDefinition->get_members().begin();
               while ( i != classDefinition->get_members().end() )
                  {
                 // Check the parent pointer to make sure it is properly set
                    ROSE_ASSERT( (*i)->get_parent() != NULL);

                 // DQ (11/1/2005): Note that a template instantiation can have a parent which is the
                 // variable which forced it's instantiation.  Since it does not really exist in the
                 // source code explicitly (it is compiler generated) this is as reasonable as anything else.
                 // if ( (*i)->get_parent() != classDefinition && isSgVariableDeclaration((*i)->get_parent()) == NULL)
                    if ( (*i)->get_parent() != classDefinition )
                       {
                         printf ("Error: (*i)->get_parent() = %p = %s \n",(*i)->get_parent(),(*i)->get_parent()->class_name().c_str());
                         printf ("(*i) = %p = %s = %s \n",*i,(*i)->class_name().c_str(),(*i)->unparseToString().c_str());
                         (*i)->get_file_info()->display("Called from SageInterface::getDefaultDestructor: debug");
                       }
                    ROSE_ASSERT( (*i)->get_parent() == classDefinition);
                 // ROSE_ASSERT( (*i)->get_parent() == classDefinition || isSgVariableDeclaration((*i)->get_parent()) != NULL);

                    SgMemberFunctionDeclaration* memberFunction = isSgMemberFunctionDeclaration(*i);
                    if (memberFunction != NULL)
                       {
                      // printf ("memberFunction = %p = %s \n",memberFunction,memberFunction->get_name().str());
                         if ( memberFunction->get_specialFunctionModifier().isDestructor() == true )
                            defaultDestructor = memberFunction;
                       }

                 // iterate through the class members
                    i++;
                  }
             }
        }

  // This should be true for the specific case that we are currently debugging!
  // ROSE_ASSERT(defaultDestructor != NULL);

     return defaultDestructor;
   }


void
SageInterface::outputGlobalFunctionTypeSymbolTable()
   {
  // DQ (6/27/2005): This function outputs the global table of function type symbols
  // it is built during the EDG/Sage translation phase, and it built initially with
  // the EDG names of all instantiated templates.  At a later phase (incomplete at
  // the moment) the AST fixup rebuilds the table to use the modified template names
  // (that is mangled names built from the modified template names used in Sage III).

  // DQ (1/31/2006): Modified to build all types in the memory pools
  // extern SgFunctionTypeTable Sgfunc_type_table;
  // Sgfunc_type_table.print_functypetable();
     ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
     SgNode::get_globalFunctionTypeTable()->print_functypetable();
   }

void
SageInterface::outputLocalSymbolTables ( SgNode* node )
   {
  // This simplifies how the traversal is called!
     OutputLocalSymbolTables astTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astTraversal.traverse(node,preorder);
   }

void
SageInterface::OutputLocalSymbolTables::visit ( SgNode* node )
   {
  // DQ (6/27/2005): Output the local symbol table from each scope.
  // printf ("node = %s \n",node->sage_class_name());

     SgScopeStatement* scope = isSgScopeStatement(node);
     if (scope != NULL)
        {
       // SgSymbolTable* symbolTable = scope->get_symbol_table();
       // ROSE_ASSERT(symbolTable != NULL);

          printf ("Symbol Table from %p = %s at: \n",scope,scope->sage_class_name());
          scope->get_file_info()->display("Symbol Table Location (Called from SageInterface::OutputLocalSymbolTables::visit())");
       // symbolTable->print("Called from SageInterface::OutputLocalSymbolTables::visit()");
          scope->print_symboltable("Called from SageInterface::OutputLocalSymbolTables::visit()");
        }
   }

#if 0
// DQ (8/28/2005): This is already a member function of the SgFunctionDeclaration
// (so that it can handle template functions and member functions)

// DQ (8/27/2005):
bool
SageInterface::isTemplateMemberFunction( SgTemplateInstantiationMemberFunctionDecl* memberFunctionDeclaration )
   {
  // This function determines if the member function is associated with a template
  // or just a template class (where it is a normal non-template member function).

     bool result = false;

     result = memberFunctionDeclaration->isTemplateFunction();

     return result;
   }
#endif

// DQ (8/27/2005):
bool
SageInterface::templateDefinitionIsInClass( SgTemplateInstantiationMemberFunctionDecl* memberFunctionDeclaration )
   {
  // This function determines if the template declaration associated withthis member function instantiation is
  // defined in the class or outside of the class.

     bool result = false;

#if 0
  // Check if this is a template or non-template member function
     if (isTemplateMemberFunction(memberFunctionDeclaration) == true)
        {
          SgTemplateDeclaration* templateDeclaration = memberFunctionDeclaration->get_templateDeclaration();
          printf ("templateDeclaration = %p parent of templateDeclaration = %p \n",templateDeclaration,templateDeclaration->get_parent());

       // SgTemplateInstantiationDecl* classTemplateInstantiation = memberFunctionDeclaration->get_class_scope()->get_declaration();
          SgClassDeclaration* classDeclaration = memberFunctionDeclaration->get_class_scope()->get_declaration();
          ROSE_ASSERT(classDeclaration != NULL);
          SgTemplateInstantiationDecl* classTemplateInstantiation = isSgTemplateInstantiationDecl(classDeclaration);

          if (classTemplateInstantiation != NULL)
             {
               SgTemplateDeclaration* classTemplateDeclaration = classTemplateInstantiation->get_templateDeclaration();
               if (classTemplateDeclaration != NULL && classTemplateDeclaration != templateDeclaration)
                  {
                    result = true;
                  }
             }
        }
#endif

  // Alternative approach
  // SgTemplateDeclaration* templateDeclaration = memberFunctionDeclaration->get_templateDeclaration();
     SgDeclarationStatement* templateDeclaration = memberFunctionDeclaration->get_templateDeclaration();
     printf ("In templateDefinitionIsInClass(): templateDeclaration = %p parent of templateDeclaration = %p = %s \n",templateDeclaration,
          templateDeclaration->get_parent(),templateDeclaration->get_parent()->class_name().c_str());

     SgScopeStatement* parentScope = isSgScopeStatement(templateDeclaration->get_parent());
     if (isSgClassDefinition(parentScope) != NULL)
        {
          result = true;
        }

     return result;
   }

SgDeclarationStatement*
generateUniqueDeclaration ( SgDeclarationStatement* declaration )
   {
  // DQ (10/11/2007): This is no longer used.
     printf ("Error: This is no longer used. \n");
     ROSE_ASSERT(false);

  // Get the defining or first non-defining declaration so that we can use it as a key to know
  // when we have found the same function. So we don't count a redundant forward declaration
  // found outside of the class as matching the first non-defining declaration or the defining
  // declaration in the class by mistake. All declarations share the same firstNondefining
  // declaration and defining declaration so either could be a key declaration, but there are
  // times when either one of then (but not both) can be NULL (function defined withouth forward
  // declaration or not defined at all).
     SgDeclarationStatement* firstNondefiningDeclaration = declaration->get_firstNondefiningDeclaration();
     SgDeclarationStatement* definingDeclaration         = declaration->get_definingDeclaration();
     SgDeclarationStatement* keyDeclaration = NULL;
     if (firstNondefiningDeclaration != NULL)
          keyDeclaration = firstNondefiningDeclaration;
       else
          keyDeclaration = definingDeclaration;
     ROSE_ASSERT(keyDeclaration != NULL);

     return keyDeclaration;
   }
//! Extract a SgPragmaDeclaration's leading keyword . For example "#pragma omp parallel" has a keyword of "omp".
std::string SageInterface::extractPragmaKeyword(const SgPragmaDeclaration *pragmaDeclaration)
{
  string pragmaString = pragmaDeclaration->get_pragma()->get_pragma();
  istringstream istr(pragmaString);
  std::string key;
  istr >> key;
  return key;
}

//! Check if a node is SgOmp*Statement
bool SageInterface::isOmpStatement(SgNode* n)
{
  ROSE_ASSERT (n != NULL);
  bool result = false;
  if (isSgOmpBarrierStatement(n)||isSgOmpBodyStatement(n)|| isSgOmpFlushStatement(n)|| isSgOmpTaskwaitStatement(n) )
    result = true;

  return result;

}
// DQ (8/28/2005):
bool
SageInterface::isOverloaded ( SgFunctionDeclaration* functionDeclaration )
   {
  // This function looks for any other function that might overload the input function.
  // for member functions we only look in the scope where the member function is defined.
  // for non-member functions we look only in the scope where the function is declared.

  // Note that there are scenarios where this approach of searching only these locations
  // might not catch an overloaded function.
  //     * member functions:
  //         - overloaded functions might be declared in base classes
  //     * non-member functions:
  //         - function declarations might be friend functions in classes
  //         - functions might be declared in differen namespace definitions
  //              (SgNamespaceDefinitionStatement), since a namespace in re-entrant
  //              and can have many namespace declarations and definitions.

  // bool result = false;
     int counter = 0;

     SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(functionDeclaration);
     if (memberFunctionDeclaration != NULL)
        {
       // This is a member function declaration

       // DQ (10/11/2007): Fix this to NOT use the generateUniqueDeclaration() function.
       // Generate a key to use for comparision (avoids false positives)
       // SgMemberFunctionDeclaration* keyDeclaration = isSgMemberFunctionDeclaration(generateUniqueDeclaration(functionDeclaration));
       // ROSE_ASSERT(keyDeclaration != NULL);

       // Get the class definition
          SgClassDefinition* classDefinition =
               isSgClassDefinition(memberFunctionDeclaration->get_scope());
          ROSE_ASSERT(classDefinition != NULL);

       // Get the class declaration associated with the class definition
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(classDefinition->get_declaration());
          ROSE_ASSERT(classDeclaration != NULL);

       // Get the list of member declarations in the class
          SgDeclarationStatementPtrList & memberList = classDefinition->get_members();
#if 0
       // DQ (10/11/2007): Fix this function better by checking for more than one member function declaration in the class definition.
          printf ("   memberList.size() = %zu \n",memberList.size());
#endif
          for (SgDeclarationStatementPtrList::iterator i = memberList.begin(); i != memberList.end(); i++)
             {
#if 0
               printf ("   counter = %d declaration = %p = %s \n",counter,*i,(*i)->class_name().c_str());
#endif
               SgMemberFunctionDeclaration* tempMemberFunction = isSgMemberFunctionDeclaration(*i);
               SgTemplateDeclaration* tempTemplateDeclaration  = isSgTemplateDeclaration(*i);

            // Member functions could be overloaded
               if (tempMemberFunction != NULL)
                  {
#if 0
                 // DQ (10/11/2007): This is a problem for where s member function prototype from outside the class is checked to be an ovverloaded function.
                 // Check using the key declaration
                    if ( keyDeclaration == generateUniqueDeclaration(tempMemberFunction) )
                       {
#if 0
                         printf ("Skipping the case of keyDeclaration == generateUniqueDeclaration(tempMemberFunction) = %p \n",keyDeclaration);
#endif
                         continue;
                       }
#endif
                    ROSE_ASSERT(tempMemberFunction->get_name() != "");
#if 0
                    printf ("      tempMemberFunction = (name) %s = (qualified) %s \n",
                              tempMemberFunction->get_name().str(),
                              tempMemberFunction->get_qualified_name().str());
#endif
                    if (tempMemberFunction->get_name() == memberFunctionDeclaration->get_name())
                       {
#if 0
                         printf ("      Found a matching overloaded member function! \n");
#endif
                      // result = true;
                         counter++;
                       }
                  }
                 else
                  {
                 // Or templates could be overloaded, but nothing else.
                    if (tempTemplateDeclaration != NULL)
                       {
                      // If this is a template declaration, it could be a template
                      // declaration for an overloaded member function of the same name.
#if 0
                      // printf ("keyDeclaration->get_name() = %s \n",keyDeclaration->get_name().str());
                         printf ("tempTemplateDeclaration->get_name() = %s \n",tempTemplateDeclaration->get_name().str());
#endif
                      // if (keyDeclaration->get_name() == tempTemplateDeclaration->get_name())
                         if (memberFunctionDeclaration->get_name() == tempTemplateDeclaration->get_name())
                            {
#if 0
                              printf ("      Found a matching overloaded member function! \n");
#endif
                           // result = true;
                              counter++;
                            }
                       }
                      else
                       {
                       // DQ (10/12/2007): Could friend functions defined in a class be overloaded?  Need to check this!
                          if (functionDeclaration->variantT() == V_SgFunctionDeclaration)
                             {
                               printf ("In SageInterface::isOverloaded(): could friend functions be overloaded in a class? \n");
                             }
                       }
                  }

            // If we have detected two, so we have an overloaded function.
               if (counter > 1)
                    break;
             }
        }
       else
        {
       // This is a non-member function declaration
          printf ("In SageInterface::isOverloaded(): case of non-member function not yet implemented! \n");
          ROSE_ASSERT(false);
        }

  // DQ (10/11/2007): Fixup to use the counter and consider more than 1 function with the same name an overloaded member function.
  // This might be a problem for friend functions, so test this separately.
  // return result;
     return (counter > 1);
   }




SgTemplateInstantiationMemberFunctionDecl*
SageInterface::buildForwardFunctionDeclaration ( SgTemplateInstantiationMemberFunctionDecl* memberFunctionInstantiation )
   {
  // DQ (9/6/2005): This function builds a copy of the input function for the
  // construction of a forward declaration of the function. Required in the
  // instantiated functions definition is to be output as a specialization by ROSE.
  // Since the shallow copy mechanism will cause IR nodes to be shared, we have
  // to fix it up with deep copies of the parameter list and the CtorInitializerList.

     ROSE_ASSERT(memberFunctionInstantiation != NULL);

#if 0
    printf ("buildForwardFunctionDeclaration: Member function = %p = %s = definition = %p \n",
          memberFunctionInstantiation,
          memberFunctionInstantiation->get_name().str(),
          memberFunctionInstantiation->get_definition());
    memberFunctionInstantiation->get_file_info()->display("memberFunctionInstantiation: debug");
#endif

  // This is a better implementation using a derived class from SgCopyHelp to control the
  // copying process (skipping the copy of any function definition).  This is a variable
  // declaration with an explicitly declared class type.
     class NondefiningFunctionDeclarationCopyType : public SgCopyHelp
        {
       // DQ (9/26/2005): This class demonstrates the use of the copy mechanism
       // within Sage III (originally designed and implemented by Qing Yi).
       // One problem with it is that there is no context information permitted.

          public:
               virtual SgNode *copyAst(const SgNode *n)
                  {
                 // If still NULL after switch then make the copy
                    SgNode* returnValue = NULL;

                    switch(n->variantT())
                       {
                      // DQ (10/21/2007): Now that s bug is fixed in the SgDeclarationStatement::fixupCopy()
                      // member function, I think we might not need this case.

                      // Don't copy the associated non-defining declaration when building a function prototype!
                         case V_SgFunctionDeclaration:
                         case V_SgMemberFunctionDeclaration:
                         case V_SgTemplateInstantiationFunctionDecl:
                         case V_SgTemplateInstantiationMemberFunctionDecl:
                            {
                           // printf ("Skip copying an associated non-defining declaration if it is present \n");
                              const SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
                              ROSE_ASSERT(functionDeclaration != NULL);
                              if (functionDeclaration == functionDeclaration->get_definingDeclaration())
                                 {
                                // This is the defining declaration (make a shallow copy)
                                // printf ("In NondefiningFunctionDeclarationCopyType::copyAst(): This is the DEFINING declaration! \n");
                                // return const_cast<SgNode *>(n);
                                   returnValue = const_cast<SgNode *>(n);
                                 }
                                else
                                 {
                                // This is the non-defining declaration where we want to make a deep copy.
                                // printf ("In NondefiningFunctionDeclarationCopyType::copyAst(): This is the NONDEFINING declaration! \n");
                                 }

                              break;
                            }

                      // Don't copy the function definitions (roughly the body of the function)
                         case V_SgFunctionDefinition:
                            {
                              printf ("Skip copying the function definition if it is present \n");
                           // return const_cast<SgNode *>(n);
                              returnValue = const_cast<SgNode *>(n);
                              break;
                            }

                         default:
                            {
                           // Nothing to do here
                              break;
                            }
                       }

                 // return n->copy(*this);

                 // If not set at this point then make the copy!
                    if (returnValue == NULL)
                         returnValue = n->copy(*this);

                    ROSE_ASSERT(returnValue != NULL);
                    return returnValue;
                  }
        } nondefiningFunctionDeclarationCopy;

  // DQ (10/20/2007): The more accurate copy mechanism now builds us a defining declaration to go with the non-defining declaration!
  // This is because we have to remove the pointers from non-defining declaration to the definition (which should be pointed to ONLY by the defining declaration!
  // delete copyOfMemberFunction->get_definingDeclaration();
     if (memberFunctionInstantiation->get_definition() != NULL)
        {
          printf ("\n\nNEED TO REMOVE POINTERS IN THE NON-DEFINING DECLARATION TO THE SgClassDefinition objects. \n");
          ROSE_ASSERT(false);

       // If we see the assertion above fail then we might want to use this code:
          ROSE_ASSERT( memberFunctionInstantiation != memberFunctionInstantiation->get_definingDeclaration() );
          memberFunctionInstantiation->set_definition(NULL);
        }
     SgNode* copyOfMemberFunctionNode = memberFunctionInstantiation->copy(nondefiningFunctionDeclarationCopy);
     SgTemplateInstantiationMemberFunctionDecl* copyOfMemberFunction = static_cast<SgTemplateInstantiationMemberFunctionDecl*>(copyOfMemberFunctionNode);

  // printf ("\n\nHOW DO WE KNOW WHEN TO NOT COPY THE DEFINING DECLARATION SO THAT WE CAN JUST BUILD A FUNCTION PROTOTYPE! \n");
  // ROSE_ASSERT(false);

#if 0
     printf ("copyOfMemberFunction->get_firstNondefiningDeclaration() = %p \n",copyOfMemberFunction->get_firstNondefiningDeclaration());
#endif
  // DQ (10/11/2007): The copy function sets the firstNondefiningDeclaration to itself if in the original declaration it had the
  // firstNondefiningDeclaration set to itself, this is incorrect in the case where we only make a copy of function declaration,
  // and causes the symbol table tests for get_declaration_associated_with_symbol() to fail because it uses the
  // firstNondefiningDeclaration which is not the one associated with the symbol in the parent scope. So reset the
  // firstNondefiningDeclaration in the copy of the member function.
     copyOfMemberFunction->set_firstNondefiningDeclaration(memberFunctionInstantiation->get_firstNondefiningDeclaration());

#if 0
     printf ("memberFunctionInstantiation->get_firstNondefiningDeclaration() = %p \n",memberFunctionInstantiation->get_firstNondefiningDeclaration());

     printf ("copyOfMemberFunction->isForward()        = %s \n",copyOfMemberFunction->isForward() ? "true" : "false");
     printf ("memberFunctionInstantiation->isForward() = %s \n",memberFunctionInstantiation->isForward() ? "true" : "false");
#endif
#if 0
     printf ("memberFunctionInstantiation->isSpecialization()        = %s \n",memberFunctionInstantiation->isSpecialization() ? "true" : "false");
     printf ("copyOfMemberFunctionNode = %p = %s = %s memberFunctionInstantiation->isSpecialization() = %s \n",
          copyOfMemberFunction,copyOfMemberFunction->class_name().c_str(),SageInterface::get_name(copyOfMemberFunction).c_str(),copyOfMemberFunction->isSpecialization() ? "true" : "false");
     copyOfMemberFunction->get_file_info()->display("copyOfMemberFunction: debug");
#endif

  // Make sure that we have the same number of arguments on the copy that we build
     ROSE_ASSERT(memberFunctionInstantiation->get_args().size() == copyOfMemberFunction->get_args().size());

  // DQ (11/5/2007): Additional tests
     ROSE_ASSERT(memberFunctionInstantiation->get_startOfConstruct()->isOutputInCodeGeneration() == copyOfMemberFunction->get_startOfConstruct()->isOutputInCodeGeneration());
     ROSE_ASSERT(memberFunctionInstantiation->get_startOfConstruct()->isCompilerGenerated() == copyOfMemberFunction->get_startOfConstruct()->isCompilerGenerated());

     ROSE_ASSERT(copyOfMemberFunction != NULL);
     return copyOfMemberFunction;
   }


void
supportForBaseTypeDefiningDeclaration ( SgSymbolTable* symbolTable, SgDeclarationStatement* declarationForType )
   {
  // DQ (10/14/2007): Supporting function for

     ROSE_ASSERT(declarationForType != NULL);

  // DQ (11/7/2007): Added more cases...
     switch(declarationForType->variantT())
        {
       // DQ (12/26/2012): Added support for templates.
          case V_SgTemplateInstantiationDecl:
          case V_SgTemplateClassDeclaration:

          case V_SgClassDeclaration:
             {
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationForType);
            // SgSymbol* symbol = new SgClassSymbol(classDeclaration);
               SgSymbol* symbol = NULL;
               if (isSgTemplateClassDeclaration(declarationForType) != NULL)
                  {
                    symbol = new SgTemplateClassSymbol(classDeclaration);
                  }
                 else
                  {
                    symbol = new SgClassSymbol(classDeclaration);
                  }

               ROSE_ASSERT(symbol != NULL);
               SgName name = classDeclaration->get_name();
               symbolTable->insert(name,symbol);
               break;
             }

          case V_SgEnumDeclaration:
             {
               SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(declarationForType);
               SgSymbol* symbol = new SgEnumSymbol(enumDeclaration);
               ROSE_ASSERT(symbol != NULL);
               SgName name = enumDeclaration->get_name();
               symbolTable->insert(name,symbol);
               break;
             }

          case V_SgFunctionDeclaration:
             {
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declarationForType);
               SgSymbol* symbol = new SgFunctionSymbol(functionDeclaration);
               ROSE_ASSERT(symbol != NULL);
               SgName name = functionDeclaration->get_name();
               symbolTable->insert(name,symbol);
               break;
             }

          case V_SgMemberFunctionDeclaration:
             {
               SgMemberFunctionDeclaration* functionDeclaration = isSgMemberFunctionDeclaration(declarationForType);
               SgSymbol* symbol = new SgMemberFunctionSymbol(functionDeclaration);
               ROSE_ASSERT(symbol != NULL);

            // printf ("In supportForBaseTypeDefiningDeclaration(): symbol = %p = %s = %s \n",symbol,symbol->class_name().c_str(),SageInterface::get_name(symbol).c_str());

               SgName name = functionDeclaration->get_name();
               symbolTable->insert(name,symbol);
               break;
             }

          default:
             {
               printf ("Default reached in evaluation of typedef inner definition = %p = %s and building a symbol for it for the symbol table \n",declarationForType,declarationForType->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }
   }


void
supportForVariableLists ( SgScopeStatement* scope, SgSymbolTable* symbolTable, SgInitializedNamePtrList & variableList )
   {
  // DQ (11/1/2007): Added supporting function to refactor code.
     SgInitializedNamePtrList::iterator i = variableList.begin();
     while (i != variableList.end())
        {
          SgInitializedName* variable = *i;
          ROSE_ASSERT(variable != NULL);
          if (variable->get_scope() == scope)
             {
            // printf ("Scopes match, OK! \n");
             }
            else
             {
               if (SgProject::get_verbose() > 0)
                  {
                    printf ("WARNING: Scopes do NOT match! variable = %p = %s (could this be a static variable, or has the symbol table been setup before the scopes have been set?) \n",variable,variable->get_name().str());
                  }
             }

          SgSymbol* symbol = new SgVariableSymbol(variable);
          ROSE_ASSERT(symbol != NULL);
          SgName name = variable->get_name();
          symbolTable->insert(name,symbol);

          i++;
        }
   }

// DQ (3/2/2014): Added a new interface function (used in the snippet insertion support).
void
SageInterface::supportForInitializedNameLists ( SgScopeStatement* scope, SgInitializedNamePtrList & variableList )
   {
     SgSymbolTable* symbolTable = scope->get_symbol_table();
     ROSE_ASSERT(symbolTable != NULL);

     supportForVariableLists(scope,symbolTable,variableList);
   }


void
supportForVariableDeclarations ( SgScopeStatement* scope, SgSymbolTable* symbolTable, SgVariableDeclaration* variableDeclaration )
   {
  // DQ (10/24/2007): Added supporting function to refactor code.
     SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
#if 1
     supportForVariableLists(scope,symbolTable,variableList);
#else
     SgInitializedNamePtrList::iterator i = variableList.begin();
     while ( i != variableList.end() )
        {
          SgInitializedName* variable = *i;
          ROSE_ASSERT(variable != NULL);

       // DQ (10/20/2007): static data members declared outside the class scope don't generate symbols.
       // if (variable->get_prev_decl_item() != NULL)
          if (variable->get_scope() == scope)
             {
               SgSymbol* symbol = new SgVariableSymbol(variable);
               ROSE_ASSERT(symbol != NULL);

            // printf ("In SageInterface::rebuildSymbolTable() variable = %p building a new SgVariableSymbol = %p \n",variable,symbol);

               SgName name = variable->get_name();
               symbolTable->insert(name,symbol);
             }
            else
             {
            // I think there is nothing to do in this case
            // printf ("In SageInterface::rebuildSymbolTable() This variable has a scope inconsistant with the symbol table: variable->get_scope() = %p scope = %p \n",variable->get_scope(),scope);
             }

          i++;
        }
#endif

  // DQ (10/13/2007): Need to look into variable declarations to see if there are defining declaration
  // that also force symbols to be built in the current scope!
  // ROSE_ASSERT(derivedDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == false);
     if (variableDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == true)
        {
       // Build a SgClassDeclaration, SgEnumDeclaration associated symbol and add it to the symbol table.
          ROSE_ASSERT(symbolTable != NULL);
          ROSE_ASSERT(variableDeclaration->get_baseTypeDefiningDeclaration() != NULL);
          supportForBaseTypeDefiningDeclaration ( symbolTable, variableDeclaration->get_baseTypeDefiningDeclaration() );
        }
   }

void
supportForLabelStatements ( SgScopeStatement* scope, SgSymbolTable* symbolTable )
   {
  // Update the symbol table in SgFunctionDefinition with all the labels in the function.

     std::vector<SgNode*> labelList = NodeQuery::querySubTree (scope,V_SgLabelStatement);

     int numberOfLabels = labelList.size();
     for (int i=0; i < numberOfLabels; i++)
        {
          SgLabelStatement* labelStatement = isSgLabelStatement(labelList[i]);

          ROSE_ASSERT(labelStatement->get_scope() == scope);

          SgSymbol* symbol = new SgLabelSymbol(labelStatement);
          ROSE_ASSERT(symbol != NULL);

       // printf ("In SageInterface::rebuildSymbolTable() labelStatement = %p building a new SgLabelSymbol = %p \n",labelStatement,symbol);

          SgName name = labelStatement->get_name();
          symbolTable->insert(name,symbol);
        }
   }


void
SageInterface::rebuildSymbolTable ( SgScopeStatement* scope )
   {
  // This function is called from the implementation of the copy member functions.

     ROSE_ASSERT(scope != NULL);
#if 0
     printf ("In SageInterface::rebuildSymbolTable(): Symbol Table from %p = %s \n",scope,scope->class_name().c_str());
#endif
#if 0
     printf ("Exiting as a test \n");
     ROSE_ASSERT(false);
#endif

     SgSymbolTable* symbolTable = scope->get_symbol_table();

     if (symbolTable != NULL)
        {
       // This must be an empty symbol table!
          if (symbolTable->size() != 0)
             {
               printf ("symbolTable->size() = %d \n",symbolTable->size());
             }
          ROSE_ASSERT(symbolTable->size() == 0);
#if 0
          printf ("Symbol Table from %p = %s of size = %zu \n",scope,scope->class_name().c_str(),symbolTable->size());
          symbolTable->print("Called from SageInterface::rebuildSymbolTable()");
#endif
        }
       else
        {
#if 0
          printf ("In SageInterface::rebuildSymbolTable(): No symbol table found \n");
#endif
          ROSE_ASSERT(symbolTable == NULL);

       // DQ (10/8/2007): Build a new symbol table if there was not already one built.
          symbolTable = new SgSymbolTable();

          ROSE_ASSERT(symbolTable != NULL);
          ROSE_ASSERT(symbolTable->get_table() != NULL);

       // DQ (2/16/2006): Set this parent directly (now tested)
          symbolTable->set_parent(scope);
          ROSE_ASSERT(symbolTable->get_parent() != NULL);

          scope->set_symbol_table(symbolTable);
        }

  // DQ (10/8/2007): If there is already a symbol table then don't over write it. This fixes copies generated with more than one symbol table.
     ROSE_ASSERT(scope->get_symbol_table() != NULL);
     ROSE_ASSERT(scope->get_symbol_table()->get_table() != NULL);

  // This implements SgScopeStatement specific details (e.g function declarations have parameters and their declaration causes variable
  // symbols to be placed into the SgFunctionDefinition scope (but only for defining declarations).
     switch(scope->variantT())
        {
          case V_SgForStatement:
             {
            // These scopes have their symbols split between the attached symbol table and the symbol tables in the SgBasicBlock data member(s).
            // printf ("Symbol tables could contain symbols outside of the inclosed body scope = %p = %s \n",scope,scope->class_name().c_str());

               SgForStatement* forStatement = isSgForStatement(scope);

               SgStatementPtrList::iterator i = forStatement->get_init_stmt().begin();

            // printf ("i != forStatement->get_init_stmt().end() = %s \n",i != forStatement->get_init_stmt().end() ? "true" : "false");

               while (i != forStatement->get_init_stmt().end())
                  {
                 // printf ("For statement initialization list: *i = %p = %s \n",*i,(*i)->class_name().c_str());
                    SgVariableDeclaration* variableDeclarationInitializer = isSgVariableDeclaration(*i);
                    if (variableDeclarationInitializer != NULL)
                       {
                      // There is a variable declaration in the conditional, it needs to be added to the symbol table.
                      // printf ("There is a variable declaration in the for statement initializer, it needs to be added to the symbol table scope = %p = %s \n",scope,scope->class_name().c_str());
                      // ROSE_ASSERT(false);
                         supportForVariableDeclarations ( scope, symbolTable, variableDeclarationInitializer );
                       }

                    i++;
                  }

               SgVariableDeclaration* variableDeclarationCondition = isSgVariableDeclaration(forStatement->get_test());
               if (variableDeclarationCondition != NULL)
                  {
                 // There is a variable declaration in the conditional, it needs to be added to the symbol table.
                 // printf ("There is a variable declaration in the for statement test, it needs to be added to the symbol table scope = %p = %s \n",scope,scope->class_name().c_str());
                 // ROSE_ASSERT(false);
                    supportForVariableDeclarations ( scope, symbolTable, variableDeclarationCondition );
                  }

               return;
               break;
             }


       // DQ (12/23/2012): Added support for templates.
          case V_SgTemplateFunctionDefinition:

          case V_SgFunctionDefinition:
             {
            // These scopes have their symbols split between the attached symbol table and the symbol tables in the SgBasicBlock data member(s).
            // printf ("Symbol tables could contain symbols outside of the enclosed body scope = %p = %s \n",scope,scope->class_name().c_str());

            // DQ (10/8/2007): If this is a SgFunctionDefinition, then include the parameters in the SgFunctionDeclaration.
               SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(scope);
               if (functionDefinition != NULL)
                  {
                    SgFunctionDeclaration* functionDeclaration = functionDefinition->get_declaration();
                 // printf ("In SageInterface::rebuildSymbolTable(): functionDefinition = %p functionDeclaration = %p \n",functionDefinition,functionDeclaration);

                 // DQ (10/8/2007): It turns out that this is always NULL, because the parent of the functionDeclaration has not yet been set in the copy mechanism!
                    if (functionDeclaration != NULL)
                       {
                      // DQ (3/28/2014): After a call with Philippe, this Java specific issues is fixed and we don't seem to see this problem any more.
                         if (functionDeclaration->isForward() == true)
                            {
                              printf ("ERROR: functionDeclaration = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
                              printf ("   --- functionDeclaration (get_name())   = %s \n",get_name(functionDeclaration).c_str());
                              printf ("   --- functionDeclaration (mangled name) = %s \n",functionDeclaration->get_mangled_name().str());
                              SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(functionDeclaration);
                              if (memberFunctionDeclaration != NULL)
                                 {
                                   printf ("memberFunctionDeclaration != NULL \n");
                                 }
                            }
                         ROSE_ASSERT(functionDeclaration->isForward() == false);
                         SgInitializedNamePtrList & argumentList = functionDeclaration->get_args();
                         supportForVariableLists(scope,symbolTable,argumentList);
                       }
                      else
                       {
                      // This happens in the copy function because the function definition is copied from the SgFunctionDeclaration
                      // and only after the copy is made is the parent of the definition set to be the function declaration.  Thus
                      // the get_declaration() member function returns NULL.
                      // printf ("There is no function declaration associated with this function definition! \n");
                      // ROSE_ASSERT(functionDeclaration->isForward() == true);
                       }
                  }

            // DQ (10/25/2007): Label symbols are now places into the SgFunctionDefinition (they have to be collected from the function).
               supportForLabelStatements(scope,symbolTable);

               return;
               break;
             }

          case V_SgIfStmt:
             {
            // These scopes have their sysmbols split between the attached symbol table and the symbol tables in the SgBasicBlock data member(s).
            // printf ("Symbol tables could contain symbols outside of the enclosed body scope = %p = %s \n",scope,scope->class_name().c_str());

               SgIfStmt* ifStatement = isSgIfStmt(scope);
               SgVariableDeclaration* variableDeclarationCondition = isSgVariableDeclaration(ifStatement->get_conditional());
               if (variableDeclarationCondition != NULL)
                  {
                 // There is a variable declaration in the conditional, it needs to be added to the symbol table.
                 // printf ("There is a variable declaration in the conditional, it needs to be added to the symbol table \n");
                 // ROSE_ASSERT(false);
                    supportForVariableDeclarations ( scope, symbolTable, variableDeclarationCondition );
                  }
               return;
               break;
             }

          case V_SgSwitchStatement:
             {
            // These scopes have their sysmbols split between the attached symbol table and the symbol tables in the SgBasicBlock data member(s).
            // printf ("Symbol tables could contain symbols outside of the enclosed body scope = %p = %s \n",scope,scope->class_name().c_str());

               SgSwitchStatement* switchStatement = isSgSwitchStatement(scope);
               SgVariableDeclaration* variableDeclarationSelector = isSgVariableDeclaration(switchStatement->get_item_selector());
               if (variableDeclarationSelector != NULL)
                  {
                 // There is a variable declaration in the conditional, it needs to be added to the symbol table.
                 // printf ("There is a variable declaration in the item selector of the switch statement, it needs to be added to the symbol table \n");

                    supportForVariableDeclarations ( scope, symbolTable, variableDeclarationSelector );
                  }
               return;
               break;
             }

          case V_SgWhileStmt:
             {
            // These scopes have their sysmbols split between the attached symbol table and the symbol tables in the SgBasicBlock data member(s).
            // commented out like for others, otherwise show up each time a While is being copied. Liao, 1/31/2008
            // printf ("Symbol tables could contain symbols outside of the enclosed body scope = %p = %s \n",scope,scope->class_name().c_str());

               SgWhileStmt* whileStatement = isSgWhileStmt(scope);
               SgVariableDeclaration* variableDeclarationCondition = isSgVariableDeclaration(whileStatement->get_condition());
               if (variableDeclarationCondition != NULL)
                  {
                 // There is a variable declaration in the conditional, it needs to be added to the symbol table.
                 // printf ("There is a variable declaration in the while statement condition, it needs to be added to the symbol table \n");
                 // ROSE_ASSERT(false);

                    supportForVariableDeclarations ( scope, symbolTable, variableDeclarationCondition );
                  }
               return;
               break;
             }

          case V_SgCatchOptionStmt:
          case V_SgDoWhileStmt:
             {
            // These scopes contain a SgBasicBlock as a data member and the scope is held there.
            // printf ("Symbol tables can must be computed by the enclosed body scope = %p = %s \n",scope,scope->class_name().c_str());
               return;
               break;
             }

       // DQ (12/24/2012): Added support for templates.
          case V_SgTemplateClassDefinition:

          case V_SgBasicBlock:
          case V_SgClassDefinition:
          case V_SgTemplateInstantiationDefn:
          case V_SgGlobal:
          case V_SgNamespaceDefinitionStatement:
          case V_SgFortranDo: // Liao 12/19/2008, My understanding is that Fortran do loop header does not introduce new symbols like  a C/C++ for loop does
             {
            // printf ("Used the list of statements/declarations that are held deirectly by this scope \n");
               break;
             }

       // DQ (3/29/2014): Added support for SgJavaForEachStatement.
          case V_SgJavaForEachStatement:
             {
               SgJavaForEachStatement* javaForEachStatement = isSgJavaForEachStatement(scope);
               SgVariableDeclaration* variableDeclarationCondition = isSgVariableDeclaration(javaForEachStatement->get_element());
               if (variableDeclarationCondition != NULL)
                  {
                 // There is a variable declaration in the conditional, it needs to be added to the symbol table.
                 // printf ("There is a variable declaration in the while statement condition, it needs to be added to the symbol table \n");
                 // ROSE_ASSERT(false);

                    supportForVariableDeclarations ( scope, symbolTable, variableDeclarationCondition );
                  }
               return;
               break;
             }

          default:
             {
               printf ("Default reached in SageInterface::rebuildSymbolTable() scope = %p = %s \n",scope,scope->class_name().c_str());
               ROSE_ASSERT(false);
               break;
             }
        }

#if 0
     printf ("In SageInterface::rebuildSymbolTable(): fixup declarations in Symbol Table from %p = %s \n",scope,scope->class_name().c_str());
#endif

  // Generate a copy of the statement list (this is simpler than handling the cases of a
  // declaration list and a statement list separately for the scopes that contain one or the other.
     SgStatementPtrList statementList = scope->generateStatementList();

  // Loop through the statements and for each declaration build a symbol and add it to the symbol table
     for (SgStatementPtrList::iterator i = statementList.begin(); i != statementList.end(); i++)
        {
       // At some point we should move this mechanism in to a factory patterns for SgSymbol

       // printf ("Iterating through the declaration in this scope ... %p = %s = %s \n",*i,(*i)->class_name().c_str(),get_name(*i).c_str());

          SgDeclarationStatement* declaration = isSgDeclarationStatement(*i);
          if (declaration != NULL)
             {
            // DQ (11/7/2007): Where there can be multiple declaration (e.g. function declarations with prototypes) only use one of them.
               bool useThisDeclaration = (declaration->get_firstNondefiningDeclaration() == declaration) ||
                                         ( (declaration->get_firstNondefiningDeclaration() == NULL) && (declaration->get_definingDeclaration() == declaration) );

               list<SgSymbol*> symbolList;
               switch(declaration->variantT())
                  {
                    case V_SgTemplateInstantiationMemberFunctionDecl:
                       {
                         SgTemplateInstantiationMemberFunctionDecl* derivedDeclaration = isSgTemplateInstantiationMemberFunctionDecl(declaration);
                      // DQ (11/6/2007): Don't build a symbol for the defining declaration defined in another scope and put the resulting symbol into the wrong scope
                         if (scope == derivedDeclaration->get_scope())
                            {
                              SgSymbol* symbol = new SgMemberFunctionSymbol(derivedDeclaration);
                              ROSE_ASSERT(symbol != NULL);

                           // printf ("In rebuildSymbolTable: symbol = %p = %s = %s \n",symbol,symbol->class_name().c_str(),SageInterface::get_name(symbol).c_str());

                           // printf ("SgTemplateInstantiationMemberFunctionDecl: scope = %p derivedDeclaration = %p = %s inserting a symbol = %p \n",scope,derivedDeclaration,get_name(derivedDeclaration).c_str(),symbol);

                              SgName name = derivedDeclaration->get_name();
                              symbolTable->insert(name,symbol);
                            }
                           else
                            {
                           // printf ("SgTemplateInstantiationMemberFunctionDecl: scope = %p derivedDeclaration = %p = %s didn't match the scope \n",scope,derivedDeclaration,get_name(derivedDeclaration).c_str());

                           // These IR nodes might only exist as a template declaration and thus not be structureally present in their scope.
                           // So we would never traverse them in the correct scope and so never build sysmbols for them and add the symbols
                           // to the correct symbol table.  This is a fundamental problem.  So we have to try to add these sorts of symbols
                           // to the scope were they belong.
                              SgScopeStatement* derivedDeclarationScope = derivedDeclaration->get_scope();
                              ROSE_ASSERT(derivedDeclarationScope != NULL);

                           // If this is a copy then it would be nice to make sure that the scope has been properly set.
                           // Check this by looking for the associated template declaration in the scope.
                           // SgTemplateDeclaration* templateDeclaration = derivedDeclaration->get_templateDeclaration();
                           // SgDeclarationStatement* templateDeclaration = derivedDeclaration->get_templateDeclaration();
                              SgTemplateMemberFunctionDeclaration* templateDeclaration = derivedDeclaration->get_templateDeclaration();
                              ROSE_ASSERT(templateDeclaration != NULL);
                           // SgTemplateSymbol* templateSymbol = derivedDeclarationScope->lookup_template_symbol(templateDeclaration->get_name());

                           // DQ (8/13/2013): Fixed the interface to avoid use of lookup_template_symbol() (removed).
                           // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
                           // In this case these are unavailable from this point.
                           // SgTemplateSymbol* templateSymbol = derivedDeclarationScope->lookup_template_symbol(templateDeclaration->get_template_name());
                           // SgTemplateSymbol* templateSymbol = derivedDeclarationScope->lookup_template_symbol(templateDeclaration->get_template_name(),NULL,NULL);
                              SgType* functionType                               = templateDeclaration->get_type();
                              SgTemplateParameterPtrList & templateParameterList = templateDeclaration->get_templateParameters();
                           // SgTemplateMemberFunctionSymbol* templateSymbol     = derivedDeclarationScope->lookup_template_symbol(templateDeclaration->get_template_name(),NULL,NULL);
                              SgTemplateMemberFunctionSymbol* templateSymbol     = derivedDeclarationScope->lookup_template_member_function_symbol(templateDeclaration->get_template_name(),functionType,&templateParameterList);
                              if (templateSymbol != NULL)
                                 {
                                // The symbol is not present, so we have to build one and add it.
#if 0
                                   printf ("Building a symbol for derivedDeclaration = %p = %s to an alternative symbol table in derivedDeclarationScope = %p \n",
                                        derivedDeclaration,get_name(derivedDeclaration).c_str(),derivedDeclarationScope);
#endif
                                   SgSymbol* symbol = new SgMemberFunctionSymbol(derivedDeclaration);
                                   ROSE_ASSERT(symbol != NULL);
                                   SgName name = derivedDeclaration->get_name();
                                   derivedDeclarationScope->insert_symbol(name,symbol);
                                 }
                                else
                                 {
                                // printf ("The symbol was already present in the derivedDeclarationScope = %p \n",derivedDeclarationScope);
                                 }
                            }
                         break;
                       }

                 // DQ (12/26/2012): Added support for templates.
                    case V_SgTemplateMemberFunctionDeclaration:

                    case V_SgMemberFunctionDeclaration:
                       {
                         SgMemberFunctionDeclaration* derivedDeclaration = isSgMemberFunctionDeclaration(declaration);

                      // DQ (11/6/2007): Don't build a symbol for the defining declaration defined in another scope and put the resulting symbol into the wrong scope
                         if (scope == derivedDeclaration->get_scope())
                            {
                           // SgSymbol* symbol = new SgMemberFunctionSymbol(derivedDeclaration);
                              SgSymbol* symbol = NULL;
                              if (isSgTemplateFunctionDeclaration(declaration) != NULL)
                                   symbol = new SgTemplateMemberFunctionSymbol(derivedDeclaration);
                                else
                                   symbol = new SgMemberFunctionSymbol(derivedDeclaration);

                              ROSE_ASSERT(symbol != NULL);

                           // printf ("In rebuildSymbolTable: symbol = %p = %s = %s \n",symbol,symbol->class_name().c_str(),SageInterface::get_name(symbol).c_str());

                              SgName name = derivedDeclaration->get_name();
                              symbolTable->insert(name,symbol);
                            }
                           else
                            {
                           // This happens when a defining declaration is located outside of the class where it is associated.
                           // printf ("SgMemberFunctionDeclaration: scope = %p derivedDeclaration = %p = %s didn't match the scope \n",scope,derivedDeclaration,get_name(derivedDeclaration).c_str());
                            }
                         break;
                       }

                 // DQ (2/26/2009): These have to be reformatted from where someone removed the formatting previously.
                    case V_SgTemplateInstantiationFunctionDecl:
                       {
                         SgTemplateInstantiationFunctionDecl* derivedDeclaration = isSgTemplateInstantiationFunctionDecl(declaration);
                      // DQ (10/21/2007): If this is a friend function in a class then we have to skip insertion of the symbol into this scope (this symbol table)
                      // if (derivedDeclaration->get_declarationModifier().isFriend() == false)
                         if (scope == derivedDeclaration->get_scope())
                            {
                              SgSymbol* symbol = new SgFunctionSymbol(derivedDeclaration);
                              ROSE_ASSERT(symbol != NULL);
                              SgName name = derivedDeclaration->get_name();
                              symbolTable->insert(name,symbol);
                            }
                           else
                            {
                              if (derivedDeclaration->get_declarationModifier().isFriend() == false)
                                 {
#if PRINT_DEVELOPER_WARNINGS
                                   printf ("Shouldn't this be a friend declaration = %p = %s \n",derivedDeclaration,derivedDeclaration->class_name().c_str());
#endif
                                 }
                            }

                         break;
                       }

                 // DQ (12/24/2012): Added support for templates.
                    case V_SgTemplateFunctionDeclaration:

                    case V_SgFunctionDeclaration:
                       {
                         SgFunctionDeclaration* derivedDeclaration = isSgFunctionDeclaration(declaration);

                      // DQ (10/20/2007): If this is a friend function in a class then we have to skip insertion of the symbol into this scope (this symbol table)
                      // if (derivedDeclaration->get_declarationModifier().isFriend() == false)

                         if (useThisDeclaration == true)
                            {
                              if (scope == derivedDeclaration->get_scope())
                                 {
                                // DQ (12/24/2012): Added support for templates.
                                // SgSymbol* symbol = new SgFunctionSymbol(derivedDeclaration);
                                   SgSymbol* symbol = NULL;
                                   if (isSgTemplateFunctionDeclaration(declaration) != NULL)
                                        symbol = new SgTemplateFunctionSymbol(derivedDeclaration);
                                     else
                                        symbol = new SgFunctionSymbol(derivedDeclaration);

                                   ROSE_ASSERT(symbol != NULL);
                                   SgName name = derivedDeclaration->get_name();
                                   symbolTable->insert(name,symbol);
                                 }
                                else
                                 {
                                   if (derivedDeclaration->get_declarationModifier().isFriend() == false)
                                      {
#if PRINT_DEVELOPER_WARNINGS
                                        printf ("Shouldn't this be a friend declaration = %p = %s \n",derivedDeclaration,derivedDeclaration->class_name().c_str());
#endif
                                      }
                                 }
                            }
                         break;
                       }

                 // DQ (12/28/2012): Adding support for templates.
                    case V_SgTemplateVariableDeclaration:

                    case V_SgVariableDeclaration:
                       {
                         SgVariableDeclaration* derivedDeclaration = isSgVariableDeclaration(declaration);
                         SgInitializedNamePtrList & variableList = derivedDeclaration->get_variables();
                         SgInitializedNamePtrList::iterator i = variableList.begin();
                         while ( i != variableList.end() )
                            {
                              SgInitializedName* variable = *i;
                              ROSE_ASSERT(variable != NULL);

                           // DQ (10/20/2007): static data members declared outside the class scope don't generate symbols.
                           // if (variable->get_prev_decl_item() != NULL)
                              if (variable->get_scope() == scope)
                                 {
                                   SgSymbol* symbol = new SgVariableSymbol(variable);
                                   ROSE_ASSERT(symbol != NULL);

                                // printf ("In SageInterface::rebuildSymbolTable() variable = %p building a new SgVariableSymbol = %p \n",variable,symbol);

                                   SgName name = variable->get_name();
                                   symbolTable->insert(name,symbol);
                                 }
                                else
                                 {
                                // I think there is nothing to do in this case
                                // printf ("In SageInterface::rebuildSymbolTable() This variable has a scope inconsistant with the symbol table: variable->get_scope() = %p scope = %p \n",variable->get_scope(),scope);
                                 }

                              i++;
                            }

                      // DQ (10/13/2007): Need to look into variable declarations to see if there are defining declaration
                      // that also force symbols to be built in the current scope!
                      // ROSE_ASSERT(derivedDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == false);
                         if (derivedDeclaration->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == true)
                            {
                           // Build a SgClassDeclaration, SgEnumDeclaration associated symbol and add it to the symbol table.
                              ROSE_ASSERT(symbolTable != NULL);
                              ROSE_ASSERT(derivedDeclaration->get_baseTypeDefiningDeclaration() != NULL);
                              supportForBaseTypeDefiningDeclaration ( symbolTable, derivedDeclaration->get_baseTypeDefiningDeclaration() );
                            }

                      // ROSE_ASSERT(symbolList.empty() == false);
                         break;
                       }

                    case V_SgTemplateInstantiationDecl:
                       {
                         SgTemplateInstantiationDecl* derivedDeclaration = isSgTemplateInstantiationDecl(declaration);
#if 1
                      // printf ("case SgTemplateInstantiationDecl: derivedDeclaration name = %s derivedDeclaration->get_declarationModifier().isFriend() = %s \n",
                      //      derivedDeclaration->get_name().str(),derivedDeclaration->get_declarationModifier().isFriend() ? "true" : "false");

                      // if (derivedDeclaration->get_declarationModifier().isFriend() == false)
                         if (scope == derivedDeclaration->get_scope())
                            {
                              SgSymbol* symbol = new SgClassSymbol(derivedDeclaration);
                           // printf ("Inserting SgClassSymbol = %p into scope = %p = %s \n",symbol,scope,scope->class_name().c_str());
                              ROSE_ASSERT(symbol != NULL);
                              SgName name = derivedDeclaration->get_name();
#if 0
                           // DQ (10/21/2007): The scopes should match
                              if (scope != derivedDeclaration->get_scope())
                                 {
                                   printf ("Error: scopes don't match for derivedDeclaration = %p = %s \n",derivedDeclaration,derivedDeclaration->class_name().c_str());
                                 }
                              ROSE_ASSERT(scope == derivedDeclaration->get_scope());
#endif
                              symbolTable->insert(name,symbol);
                            }
                           else
                            {
                           // printf ("SgTemplateInstantiationDecl: scope = %p derivedDeclaration = %p = %s didn't match the scope \n",scope,derivedDeclaration,get_name(derivedDeclaration).c_str());

                              if (derivedDeclaration->get_declarationModifier().isFriend() == false)
                                 {
#if PRINT_DEVELOPER_WARNINGS
                                   printf ("Shouldn't this be a friend declaration = %p = %s \n",derivedDeclaration,derivedDeclaration->class_name().c_str());
#endif
                                 }
                            }
#else
                         SgSymbol* symbol = new SgClassSymbol(derivedDeclaration);
                         ROSE_ASSERT(symbol != NULL);
                         SgName name = derivedDeclaration->get_name();
                         symbolTable->insert(name,symbol);
#endif
                         break;
                       }


                 // DQ (12/24/2012): Added support for templates.
                    case V_SgTemplateClassDeclaration:

                    case V_SgClassDeclaration:
                       {
                         SgClassDeclaration* derivedDeclaration = isSgClassDeclaration(declaration);
#if 1
                      // printf ("case SgClassDeclaration: derivedDeclaration name = %s derivedDeclaration->get_declarationModifier().isFriend() = %s \n",
                      //      derivedDeclaration->get_name().str(),derivedDeclaration->get_declarationModifier().isFriend() ? "true" : "false");
                      // if (derivedDeclaration->get_declarationModifier().isFriend() == false)
                         if (scope == derivedDeclaration->get_scope())
                            {
                           // SgSymbol* symbol = new SgClassSymbol(derivedDeclaration);
                               SgSymbol* symbol = NULL;
                                   if (isSgTemplateClassDeclaration(declaration) != NULL)
                                        symbol = new SgTemplateClassSymbol(derivedDeclaration);
                                     else
                                        symbol = new SgClassSymbol(derivedDeclaration);

                              ROSE_ASSERT(symbol != NULL);
                              SgName name = derivedDeclaration->get_name();
                              symbolTable->insert(name,symbol);
                            }
                           else
                            {
                              if (derivedDeclaration->get_declarationModifier().isFriend() == false)
                                 {
#if PRINT_DEVELOPER_WARNINGS
                                   printf ("Shouldn't this be a friend declaration = %p = %s \n",derivedDeclaration,derivedDeclaration->class_name().c_str());
#endif
                                 }
                            }
#else
                         SgSymbol* symbol = new SgClassSymbol(derivedDeclaration);
                         ROSE_ASSERT(symbol != NULL);
                         SgName name = derivedDeclaration->get_name();
                         symbolTable->insert(name,symbol);
#endif
                         break;
                       }

                    case V_SgEnumDeclaration:
                       {
                         SgEnumDeclaration* derivedDeclaration = isSgEnumDeclaration(declaration);
                         ROSE_ASSERT(derivedDeclaration != NULL);
                         SgSymbol* symbol = new SgEnumSymbol(derivedDeclaration);
                         ROSE_ASSERT(symbol != NULL);
                         SgName name = derivedDeclaration->get_name();
                         symbolTable->insert(name,symbol);

                      // DQ (10/18/2007): Fixed construction of symbol tabel to include enum fields.
                         SgInitializedNamePtrList & enumFieldList = derivedDeclaration->get_enumerators();
                         SgInitializedNamePtrList::iterator i     = enumFieldList.begin();

                      // Iterate over enum fields and add each one to the symbol table.
                         while (i != enumFieldList.end())
                            {
                              SgSymbol* enum_field_symbol = new SgEnumFieldSymbol(*i);
                              ROSE_ASSERT(enum_field_symbol != NULL);
                              SgName enum_field_name = (*i)->get_name();
                              symbolTable->insert(enum_field_name,enum_field_symbol);

                              i++;
                            }

                         break;
                       }

                    case V_SgTypedefDeclaration:
                       {
                         SgTypedefDeclaration* derivedDeclaration = isSgTypedefDeclaration(declaration);
                         SgSymbol* symbol = new SgTypedefSymbol(derivedDeclaration);
                         ROSE_ASSERT(symbol != NULL);
                         SgName name = derivedDeclaration->get_name();
                         symbolTable->insert(name,symbol);
#if 0
                         printf ("In SageInterface::rebuildSymbolTable(): case of SgTypedefDeclaration \n");
#endif
                      // DQ (10/13/2007): Need to look into typedefs to see if there are defining declaration
                      // that also force symbols to be built in the current scope!
                      // ROSE_ASSERT(derivedDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() == false);
                         if (derivedDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() == true)
                            {
                           // Build a SgClassDeclaration, SgEnumDeclaration associated symbol and add it to the symbol table.
                              ROSE_ASSERT(symbolTable != NULL);
                              ROSE_ASSERT(derivedDeclaration->get_baseTypeDefiningDeclaration() != NULL);
#if 0
                              printf ("In SageInterface::rebuildSymbolTable(): case of SgTypedefDeclaration: typedefBaseTypeContainsDefiningDeclaration == true calling supportForBaseTypeDefiningDeclaration() \n");
#endif
                              supportForBaseTypeDefiningDeclaration ( symbolTable, derivedDeclaration->get_baseTypeDefiningDeclaration() );
                            }
                           else
                            {
                           // DQ (11/7/2007): If the typedef has a definition (e.g. function pointer) then build a symbol.
                              SgDeclarationStatement* declaration = derivedDeclaration->get_declaration();
                              if (declaration != NULL)
                                 {
#if 0
                                   printf ("In SageInterface::rebuildSymbolTable(): case of SgTypedefDeclaration: typedefBaseTypeContainsDefiningDeclaration == false calling supportForBaseTypeDefiningDeclaration() \n");
#endif
                                // DQ (12/27/2012): Debugging the support for copytest_2007_40_cpp.C (we don't want to build this symbol since it is associated with an outer scope).
                                // supportForBaseTypeDefiningDeclaration ( symbolTable, derivedDeclaration->get_declaration() );

                                   printf ("In SageInterface::rebuildSymbolTable(): case of SgTypedefDeclaration: typedefBaseTypeContainsDefiningDeclaration == false: skipping call to supportForBaseTypeDefiningDeclaration() \n");
                                 }
                            }
#if 0
                         printf ("In SageInterface::rebuildSymbolTable(): Leaving case of SgTypedefDeclaration \n");
#endif
                         break;
                       }

                    case V_SgTemplateDeclaration:
                       {
                         SgTemplateDeclaration* derivedDeclaration = isSgTemplateDeclaration(declaration);
#if 1
                      // DQ (10/21/2007): If this is a friend function in a class then we have to skip insertion of the symbol into this scope (this symbol table)
#if 0
                         printf ("case V_SgTemplateDeclaration: derivedDeclaration               = %p \n",derivedDeclaration);
                         printf ("case V_SgTemplateDeclaration: derivedDeclaration->get_declarationModifier().isFriend() = %s \n",derivedDeclaration->get_declarationModifier().isFriend() ? "true" : "false");
                         printf ("case V_SgTemplateDeclaration: derivedDeclaration->get_name()   = %s \n",derivedDeclaration->get_name().str());
                         printf ("case V_SgTemplateDeclaration: derivedDeclaration->get_string() = %s \n",derivedDeclaration->get_string().str());
#endif
                      // if (derivedDeclaration->get_declarationModifier().isFriend() == false)
                         if (scope == derivedDeclaration->get_scope())
                            {
                           // printf ("Building symbol for SgTemplateDeclaration: derivedDeclaration = %p for symbol table in scope = %p \n",derivedDeclaration,scope);
                              SgSymbol* symbol = new SgTemplateSymbol(derivedDeclaration);
                              ROSE_ASSERT(symbol != NULL);
                              SgName name = derivedDeclaration->get_name();
                              symbolTable->insert(name,symbol);
                            }
                           else
                            {
                              if (derivedDeclaration->get_declarationModifier().isFriend() == false)
                                 {
#if PRINT_DEVELOPER_WARNINGS
                                   printf ("Shouldn't this be a friend declaration = %p = %s \n",derivedDeclaration,derivedDeclaration->class_name().c_str());
#endif
                                 }
                            }
#else
                         SgTemplateDeclaration* derivedDeclaration = isSgTemplateDeclaration(declaration);
                         SgSymbol* symbol = new SgTemplateSymbol(derivedDeclaration);
                         ROSE_ASSERT(symbol != NULL);
                         SgName name = derivedDeclaration->get_name();
                         symbolTable->insert(name,symbol);
#endif
                         break;
                       }

                 // Does this cause a symbol to be built?  Seems that it should,
                 // unless we always reference the non-aliased symbol (reuse it)!
                    case V_SgNamespaceAliasDeclarationStatement:
                       {
                         SgNamespaceAliasDeclarationStatement* aliasDeclaration = isSgNamespaceAliasDeclarationStatement(declaration);
                         ROSE_ASSERT(aliasDeclaration != NULL);
                         ROSE_ASSERT(aliasDeclaration->get_namespaceDeclaration() != NULL);

                         SgNamespaceDeclarationStatement* derivedDeclaration = isSgNamespaceDeclarationStatement(aliasDeclaration->get_namespaceDeclaration());
                         ROSE_ASSERT(derivedDeclaration != NULL);

                      // The constructor for the SgNamespaceSymbol is disturbingly different from the rest of the constructors.
                         SgSymbol* symbol = new SgNamespaceSymbol(derivedDeclaration->get_name(),derivedDeclaration);
                         ROSE_ASSERT(symbol != NULL);
                         SgName name = derivedDeclaration->get_name();
                         symbolTable->insert(name,symbol);
                      // symbolList.push_back(symbol);
                      // ROSE_ASSERT(symbolList.empty() == false);
                         break;
                       }

                 // Does this cause a symbol to be built?  Seems that it should,
                 // unless we always reference the non-aliased symbol (reuse it)!
                    case V_SgNamespaceDeclarationStatement:
                       {
                         SgNamespaceDeclarationStatement* derivedDeclaration = isSgNamespaceDeclarationStatement(declaration);
                         ROSE_ASSERT(derivedDeclaration != NULL);

                      // The constructor for the SgNamespaceSymbol is disturbingly different from the rest of the constructors.
                         SgSymbol* symbol = new SgNamespaceSymbol(derivedDeclaration->get_name(),derivedDeclaration);
                         ROSE_ASSERT(symbol != NULL);
                         SgName name = derivedDeclaration->get_name();
                         symbolTable->insert(name,symbol);
                      // symbolList.push_back(symbol);
                      // ROSE_ASSERT(symbolList.empty() == false);
                         break;
                       }

                    case V_SgUsingDirectiveStatement:
                    case V_SgPragmaDeclaration:
                    case V_SgTemplateInstantiationDirectiveStatement:
                    case V_SgUsingDeclarationStatement:
                      {
                        // DQ (10/22/2005): Not sure if we have to worry about this declaration's appearance in the symbol table!
#if 0
                        printf ("This declaration is ignored in rebuilding symbol table %p = %s = %s \n",*i,(*i)->class_name().c_str(),get_name(*i).c_str());
#endif
                        break;
                      }

                    case V_SgAsmStmt:
                      {
                        // DQ (8/13/2006): This is not really a declaration (I think).  This will be fixed later.
#if 0
                        printf ("An ASM statement (SgAsmStmt) declaration is not really a declaration %p = %s = %s \n",*i,(*i)->class_name().c_str(),get_name(*i).c_str());
#endif
                        break;
                      }

                 // Cases where declations are not used or referenced and so symbols are not required!
                    case V_SgVariableDefinition:
                    case V_SgFunctionParameterList:
                    case V_SgCtorInitializerList:
                 // These are not referenced so they don't need a symbol!
                      {
                        printf ("Special cases not handled %p = %s = %s \n",*i,(*i)->class_name().c_str(),get_name(*i).c_str());
                        ROSE_ASSERT(false);
                        break;
                      }

                    default:
                      {
                        printf ("Error: Default reached in rebuildSymbolTable declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                        ROSE_ASSERT(false);
                      }
                  }

               ROSE_ASSERT(symbolTable != NULL);
               ROSE_ASSERT(symbolTable->get_table() != NULL);
             }
       // printf ("DONE: Iterating through the declaration in this scope ... %p = %s = %s \n",*i,(*i)->class_name().c_str(),get_name(*i).c_str());

        }

     ROSE_ASSERT(symbolTable != NULL);
     ROSE_ASSERT(symbolTable->get_table() != NULL);

#if 0
     printf ("Leaving SageInterface::rebuildSymbolTable(): fixup declarations in Symbol Table from %p = %s \n",scope,scope->class_name().c_str());
#endif

#if 0
     printf ("Symbol Table from %p = %s at: \n",scope,scope->class_name().c_str());
     scope->get_file_info()->display("Symbol Table Location");
     symbolTable->print("Called from SageInterface::rebuildSymbolTable()");
#endif
   }


// #ifndef USE_ROSE

void
SageInterface::fixupReferencesToSymbols( const SgScopeStatement* this_scope,  SgScopeStatement* copy_scope, SgCopyHelp & help )
   {
  // This function is called by the SageInterface::rebuildSymbolTable().
  // It resets references to old symbols to the new symbols (just built).
  // All pairs of old/new symbols are also saved in the object:
  //    SgCopyHelp::copiedNodeMapType copiedNodeMap

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
     ROSE_ASSERT(this_scope != NULL);
     ROSE_ASSERT(copy_scope != NULL);

#if 0
     printf ("In fixupReferencesToSymbols(this_scope = %p = %s = %s, copy_scope = %p = %s = %s) \n",
          this_scope,this_scope->class_name().c_str(),get_name(this_scope).c_str(),
          copy_scope,copy_scope->class_name().c_str(),get_name(copy_scope).c_str());
#endif

     SgSymbolTable* this_symbolTable = this_scope->get_symbol_table();
     SgSymbolTable* copy_symbolTable = copy_scope->get_symbol_table();
#if 0
     printf ("Before fixup: this scope = %p = %s this_symbolTable->get_table()->size() = %zu \n",this_scope,this_scope->class_name().c_str(),this_symbolTable->get_table()->size());
     printf ("Before fixup: copy scope = %p = %s copy_symbolTable->get_table()->size() = %zu \n",copy_scope,copy_scope->class_name().c_str(),copy_symbolTable->get_table()->size());
#endif

  // DQ (3/4/2009): For now just output a warning, but this might be a more serious problem.
  // Since the symbol table size of the copy is larger than that of the original it might
  // be that a symbol is enterted twice by the copy mechanism.  If so I want to fix this.
     if (this_symbolTable->get_table()->size() != copy_symbolTable->get_table()->size())
        {
          if (SgProject::get_verbose() > 0)
              {
               printf ("Before fixup: this scope = %p = %s this_symbolTable->get_table()->size() = %zu \n",this_scope,this_scope->class_name().c_str(),this_symbolTable->get_table()->size());
               printf ("Before fixup: copy scope = %p = %s copy_symbolTable->get_table()->size() = %zu \n",copy_scope,copy_scope->class_name().c_str(),copy_symbolTable->get_table()->size());
               printf ("Warning the symbols tables in these different scopes are different sizes \n");
             }
        }
  // ROSE_ASSERT(this_symbolTable->get_table()->size() <= copy_symbolTable->get_table()->size());

     SgSymbolTable::hash_iterator i = this_symbolTable->get_table()->begin();

  // This is used to fixup the AST by resetting references to IR nodes (leveraged from AST merge).
     int replacementHashTableSize = 1001;
// CH (4/9/2010): Use boost::unordered instead
//#ifdef _MSCx_VER
#if 0
//#pragma message ("WARNING: in MSCV, hash_map constructor taking integer is not availalbe in MSVC.")
     p
     int counter = 0;
     while (i != this_symbolTable->get_table()->end())
        {
          ROSE_ASSERT ( isSgSymbol( (*i).second ) != NULL );

          SgName name     = (*i).first;
       // SgSymbol* symbol = isSgSymbol((*i).second);
          SgSymbol* symbol = (*i).second;
          ROSE_ASSERT ( symbol != NULL );

          SgSymbol* associated_symbol = NULL;
#if 0
          printf ("Symbol number: %d (pair.first (SgName) = %s) pair.second (SgSymbol) = %p sage_class_name() = %s \n",counter,i->first.str(),i->second,i->second->class_name().c_str());
#endif
       // Look for the associated symbol in the copy_scope's symbol table.
          SgSymbolTable::hash_iterator associated_symbol_iterator = copy_symbolTable->get_table()->find(name);

       // Note that this is a multi-map and for C++ a number of symbols can have the same name
       // (though not the same type of symbol) so we have to iterator over the symbols of the
       // same name so that we can identify the associated symbol.
          while (associated_symbol_iterator != copy_symbolTable->get_table()->end() && associated_symbol_iterator->first == name)
             {
               if ( associated_symbol_iterator->second->variantT() == symbol->variantT() )
                  {
                    associated_symbol = associated_symbol_iterator->second;
                  }

               associated_symbol_iterator++;
             }

          if (associated_symbol != NULL)
             {
               ROSE_ASSERT ( associated_symbol != NULL );

            // Check to make sure that this is correct
               ROSE_ASSERT(copy_scope->symbol_exists(associated_symbol) == true);

            // Add the SgGlobal referenece to the replacementMap
            // replacementMap.insert(pair<SgNode*,SgNode*>(originalFileGlobalScope,scope));
            // DQ (23/1/2009): Find the reference to symbol and replace it with associated_symbol.
               replacementMap.insert(pair<SgNode*,SgNode*>(symbol,associated_symbol));

            // DQ (3/1/2009): This is backwards
            // replacementMap.insert(pair<SgNode*,SgNode*>(associated_symbol,symbol));

            // DQ (3/2/2009): accumulate the symbol pair into the SgCopyHelp object (to support the outliner).
            // Actually this should also improve the robustness of the outliner.
               help.get_copiedNodeMap().insert(pair<const SgNode*,SgNode*>(symbol,associated_symbol));
             }
            else
             {
            // DQ (3/4/2009): This case was broken out because copytest2007_14.C fails here.
               if (SgProject::get_verbose() > 0)
                  {
                    printf ("Warning: Symbol number: %d (pair.first (SgName) = %s) pair.second (SgSymbol) = %p sage_class_name() = %s \n",counter,i->first.str(),i->second,i->second->class_name().c_str());
                    printf ("Warning: associated_symbol == NULL, need to investigate this (ignoring for now) \n");
                  }
             }

          counter++;

          i++;
        }

     set<SgNode*>  intermediateDeleteSet;

#if 0
     printf ("\n\n************************************************************\n");
     printf ("fixupReferencesToSymbols(this_scope = %p copy_scope = %p = %s = %s): calling fixupSubtreeTraversal() \n",this_scope,copy_scope,copy_scope->class_name().c_str(),get_name(copy_scope).c_str());
#endif

     fixupSubtreeTraversal(copy_scope,replacementMap,intermediateDeleteSet);

#if 0
     printf ("fixupReferencesToSymbols(): calling fixupSubtreeTraversal(): DONE \n");
     printf ("************************************************************\n\n");

     printf ("\n\n After replacementMapTraversal(): intermediateDeleteSet: \n");
     displaySet(intermediateDeleteSet,"After fixupTraversal");

     printf ("After fixup: this_symbolTable->get_table()->size() = %zu \n",this_symbolTable->get_table()->size());
     printf ("After fixup: copy_symbolTable->get_table()->size() = %zu \n",copy_symbolTable->get_table()->size());
#endif

  // DQ (3/1/2009): find a case where this code is tested.
  // ROSE_ASSERT(this_symbolTable->get_table()->size() == 0);
  // ROSE_ASSERT(isSgClassDefinition(copy_scope) == NULL);
#endif

#if 0
     printf ("Exiting as a test in fixupReferencesToSymbols() \n");
     ROSE_ASSERT(false);
#endif
   }

// #endif

#ifndef USE_ROSE

std::vector<SgFile*>
SageInterface::generateFileList()
   {
  // This function uses a memory pool traversal specific to the SgFile IR nodes
     class FileTraversal : public ROSE_VisitTraversal
        {
          public:
               vector<SgFile*> fileList;
               void visit ( SgNode* node)
                  {
                    SgFile* file = isSgFile(node);
                    ROSE_ASSERT(file != NULL);
                    if (file != NULL)
                       {
                         fileList.push_back(file);
                       }
                  };

              virtual ~FileTraversal() {}
        };

     FileTraversal fileTraversal;

  // traverse just the SgFile nodes (both the SgSourceFile and SgBinaryComposite IR nodes)!
  // SgFile::visitRepresentativeNode(fileTraversal);
     SgSourceFile::visitRepresentativeNode(fileTraversal);
     SgBinaryComposite::visitRepresentativeNode(fileTraversal);

  // This would alternatively traverse all IR nodes in thememory pool!
  // fileTraversal.traverseMemoryPool();

  // DQ (10/11/2014): This is allowed to be empty (required for new aterm support).
  // ROSE_ASSERT(fileTraversal.fileList.empty() == false);

     return fileTraversal.fileList;
   }

#endif

// #ifndef USE_ROSE

// This function uses a memory pool traversal specific to the SgProject IR nodes
SgProject*
SageInterface::getProject()
{
#if 0
  class ProjectTraversal : public ROSE_VisitTraversal
  {
    public:
      SgProject * project;
      void visit ( SgNode* node)
      {
        project = isSgProject(node);
        ROSE_ASSERT(project!= NULL);
      };
      virtual ~ProjectTraversal() {}
  };

  ProjectTraversal projectTraversal;
  SgProject::visitRepresentativeNode(projectTraversal);
  return projectTraversal.project;
#endif
  std::vector<SgProject* > resultlist = getSgNodeListFromMemoryPool<SgProject>();
  if (resultlist.empty())
      return NULL;
  ROSE_ASSERT(resultlist.size()==1);
  return resultlist[0];
}

SgFunctionDeclaration* SageInterface::getDeclarationOfNamedFunction(SgExpression* func) {
  if (isSgFunctionRefExp(func)) {
    return isSgFunctionRefExp(func)->get_symbol()->get_declaration();
  } else if (isSgDotExp(func) || isSgArrowExp(func)) {
    SgExpression* func2 = isSgBinaryOp(func)->get_rhs_operand();
    ROSE_ASSERT (isSgMemberFunctionRefExp(func2));
    return isSgMemberFunctionRefExp(func2)->get_symbol()->get_declaration();
  } else return 0;
}

SgExpression* SageInterface::forallMaskExpression(SgForAllStatement* stmt) {
  SgExprListExp* el = stmt->get_forall_header();
  const SgExpressionPtrList& ls = el->get_expressions();
  if (ls.empty()) return 0;
  if (isSgAssignOp(ls.back())) return 0;
  return ls.back();
}

//Find all SgPntrArrRefExp under astNode, add the referenced dim_info SgVarRefExp (if any) into NodeList_t
void SageInterface::addVarRefExpFromArrayDimInfo(SgNode * astNode, Rose_STL_Container<SgNode *>& NodeList_t)
{
  ROSE_ASSERT (astNode != NULL);
  Rose_STL_Container<SgNode*> arr_exp_list = NodeQuery::querySubTree(astNode,V_SgPntrArrRefExp);
  for (Rose_STL_Container<SgNode*>::iterator iter_0 = arr_exp_list.begin(); iter_0 !=arr_exp_list.end(); iter_0 ++)
  {
    SgPntrArrRefExp * arr_exp = isSgPntrArrRefExp(*iter_0);
    ROSE_ASSERT (arr_exp != NULL);
    //printf("Debug: Found SgPntrArrRefExp :%p\n", arr_exp);
    Rose_STL_Container<SgNode*> refList = NodeQuery::querySubTree(arr_exp->get_lhs_operand(),V_SgVarRefExp);
    for (Rose_STL_Container<SgNode*>::iterator iter = refList.begin(); iter !=refList.end(); iter ++)
    {
      SgVarRefExp* cur_ref = isSgVarRefExp(*iter);
      ROSE_ASSERT (cur_ref != NULL);
      SgVariableSymbol * sym = cur_ref->get_symbol();
      ROSE_ASSERT (sym != NULL);
      SgInitializedName * i_name = sym->get_declaration();
      ROSE_ASSERT (i_name != NULL);
      SgArrayType * a_type = isSgArrayType(i_name->get_typeptr());
      if (a_type && a_type->get_dim_info())
      {
        Rose_STL_Container<SgNode*> dim_ref_list = NodeQuery::querySubTree(a_type->get_dim_info(),V_SgVarRefExp);
        for (Rose_STL_Container<SgNode*>::iterator iter2 = dim_ref_list.begin(); iter2 != dim_ref_list.end(); iter2++)
        {
          SgVarRefExp* dim_ref = isSgVarRefExp(*iter2);
          //printf("Debug: Found indirect SgVarRefExp as part of array dimension declaration:%s\n", dim_ref->get_symbol()->get_name().str());
          NodeList_t.push_back(dim_ref);
        }
      }
    }
  } // end for
}


bool
SageInterface::is_C_language()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_C_only() == true)
               returnValue = true;
        }

     return returnValue;
   }
bool
SageInterface::is_OpenMP_language()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_openmp() == true)
               returnValue = true;
        }

     return returnValue;
   }

bool
SageInterface::is_UPC_language()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_UPC_only() == true)
               returnValue = true;
        }

     return returnValue;
   }

//FMZ
bool
SageInterface::is_CAF_language()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_CoArrayFortran_only()==true)
               returnValue = true;
        }

     return returnValue;
   }


// true if any of upc_threads is set to >0 via command line: -rose:upc_threads n
bool
SageInterface::is_UPC_dynamic_threads()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_upc_threads() > 0)
               returnValue = true;
        }

     return returnValue;
   }



bool
SageInterface::is_C99_language()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_C99_only() == true)
               returnValue = true;
        }

     return returnValue;
   }

bool
SageInterface::is_Cxx_language()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
       // DQ (8/19/2007): Make sure this is not a Fortran code!
       // if (fileList[i]->get_C99_only() == false && fileList[i]->get_C_only() == false)
       // if (fileList[i]->get_Fortran_only() == false && fileList[i]->get_C99_only() == false && fileList[i]->get_C_only() == false && fileList[i]->get_binary_only() == false)
          if (fileList[i]->get_Cxx_only() == true)
             {
            // ROSE_ASSERT(fileList[i]->get_Cxx_only() == true);
               ROSE_ASSERT(fileList[i]->get_Fortran_only() == false && fileList[i]->get_C99_only() == false && fileList[i]->get_C_only() == false && fileList[i]->get_binary_only() == false);

               returnValue = true;
             }
        }

     return returnValue;
   }

bool
SageInterface::is_Java_language()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_Java_only() == true)
               returnValue = true;
        }

     return returnValue;
   }


bool
SageInterface::is_Fortran_language()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_Fortran_only() == true)
               returnValue = true;
        }

     return returnValue;
   }


bool
SageInterface::is_binary_executable()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_binary_only() == true)
               returnValue = true;
        }

     return returnValue;
   }

bool
SageInterface::is_PHP_language()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_PHP_only() == true)
               returnValue = true;
        }

     return returnValue;
   }

bool
SageInterface::is_Python_language()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_Python_only() == true)
               returnValue = true;
        }

     return returnValue;
   }

bool
SageInterface::is_Cuda_language()
   {
     bool returnValue = false;

     vector<SgFile*> fileList = generateFileList();

     int size = (int)fileList.size();
     for (int i = 0; i < size; i++)
        {
          if (fileList[i]->get_Cuda_only() == true)
               returnValue = true;
        }

     return returnValue;
   }

bool
SageInterface::is_X10_language()
{
  bool returnValue = false;

  vector<SgFile*> fileList = generateFileList();

  int size = (int)fileList.size();
  for (int i = 0; i < size; i++)
    {
      if (fileList[i]->get_X10_only() == true)
           returnValue = true;
    }

  return returnValue;
}

bool SageInterface::is_mixed_C_and_Cxx_language()
   {
     return is_C_language() && is_Cxx_language();
   }

bool SageInterface::is_mixed_Fortran_and_C_language()
   {
     return is_Fortran_language() && is_C_language();
   }

bool SageInterface::is_mixed_Fortran_and_Cxx_language()
   {
     return is_Fortran_language() && is_Cxx_language();
   }

bool SageInterface::is_mixed_Fortran_and_C_and_Cxx_language()
   {
     return is_Fortran_language() && is_C_language() && is_Cxx_language();
   }

// #endif

// DQ (10/5/2006): Added support for faster (non-quadratic) computation of unique
// labels for scopes in a function (as required for name mangling).
void
SageInterface::clearScopeNumbers( SgFunctionDefinition* functionDefinition )
   {
     ROSE_ASSERT(functionDefinition != NULL);
     std::map<SgNode*,int> & scopeMap = functionDefinition->get_scope_number_list();

  // Clear the cache of stored (scope,integer) pairs
     scopeMap.erase(scopeMap.begin(),scopeMap.end());

     ROSE_ASSERT(scopeMap.empty() == true);
     ROSE_ASSERT(functionDefinition->get_scope_number_list().empty() == true);
   }

#ifndef USE_ROSE

// DQ (10/5/2006): Added support for faster (non-quadratic) computation of unique
// labels for scopes in a function (as required for name mangling).
void
SageInterface::resetScopeNumbers( SgFunctionDefinition* functionDefinition )
   {
     ROSE_ASSERT(functionDefinition != NULL);
  // std::map<SgNode*,int> & scopeMap = functionDefinition->get_scope_number_list();
  // ROSE_ASSERT(scopeMap.empty() == true);
     ROSE_ASSERT(functionDefinition->get_scope_number_list().empty() == true);

  // Preorder traversal to uniquely label the scopes (SgScopeStatements)
     class ScopeNumberingTraversal : public AstSimpleProcessing
        {
          public:
               ScopeNumberingTraversal() : count (0), storedFunctionDefinition(NULL) {}
               void visit (SgNode* node)
                  {
                    SgScopeStatement* scope = isSgScopeStatement (node);
                    if (scope != NULL)
                       {
                      // Set the function definition
                         SgFunctionDefinition* testFunctionDefinition = isSgFunctionDefinition(scope);
                         if (testFunctionDefinition != NULL && storedFunctionDefinition == NULL)
                            {
                              ROSE_ASSERT(storedFunctionDefinition == NULL);
                              storedFunctionDefinition = testFunctionDefinition;
                            }

                      // This should now be set (since the root of each traversal is a SgFunctionDefinition).
                         ROSE_ASSERT(storedFunctionDefinition != NULL);

                         count++;

                         std::map<SgNode*,int> & scopeMap = storedFunctionDefinition->get_scope_number_list();
                         scopeMap.insert(pair<SgNode*,int>(scope,count));
#if 0
                         string functionName = storedFunctionDefinition->get_declaration()->get_name().str();
                         printf ("In function = %s insert scope = %p = %s with count = %d into local map (size = %d) \n",
                              functionName.c_str(),scope,scope->class_name().c_str(),count,scopeMap.size());
#endif
                       }
                  }

          private:
               int count; // running total of scopes found in the input function
               SgFunctionDefinition* storedFunctionDefinition;
        };

    // Now buid the traveral object and call the traversal (preorder) on the function definition.
       ScopeNumberingTraversal traversal;
       traversal.traverse(functionDefinition, preorder);
   }

#endif

#ifndef USE_ROSE

#if 0
// DQ (6/26/2007): These are removed and the support is added to SgNode to support a single mangled name cache.
// DQ (10/5/2006): Added support for faster (non-quadratic) computation of unique
// labels for scopes in a function (as required for name mangling).
void
SageInterface::clearMangledNameCache( SgGlobal* globalScope )
   {
     ROSE_ASSERT(globalScope != NULL);
     std::map<SgNode*,std::string> & mangledNameCache = globalScope->get_mangledNameCache();

  // Clear the cache of stored (scope,integer) pairs
     mangledNameCache.erase(mangledNameCache.begin(),mangledNameCache.end());

     ROSE_ASSERT(mangledNameCache.empty() == true);
     ROSE_ASSERT(globalScope->get_mangledNameCache().empty() == true);
   }

// DQ (10/5/2006): Added support for faster (non-quadratic) computation of unique
// labels for scopes in a function (as required for name mangling).
void
SageInterface::resetMangledNameCache( SgGlobal* globalScope )
   {
     ROSE_ASSERT(globalScope != NULL);
     ROSE_ASSERT(globalScope->get_mangledNameCache().empty() == true);

  // Preorder traversal to uniquely label the scopes (SgScopeStatements)
     class MangledNameTraversal : public AstSimpleProcessing
        {
          public:
               MangledNameTraversal() : storedGlobalScope(NULL) {}
               void visit (SgNode* node)
                  {
                    SgFunctionDeclaration* mangleableNode = isSgFunctionDeclaration(node);
                    if ( (mangleableNode != NULL) || (isSgGlobal(node) != NULL) )
                       {
                      // Set the global scope
                         SgGlobal* testGlobalScope = isSgGlobal(mangleableNode);
                         if (testGlobalScope != NULL && storedGlobalScope == NULL)
                            {
                              ROSE_ASSERT(storedGlobalScope == NULL);
                              storedGlobalScope = testGlobalScope;
                            }

                      // This should now be set (since the root of each traversal is a SgFunctionDefinition).
                         ROSE_ASSERT(storedGlobalScope != NULL);

                         string mangledName = mangleableNode->get_mangled_name();
                      // printf ("mangledName = %s \n",mangledName.c_str());

                         std::map<SgNode*,std::string> & mangledNameCache = storedGlobalScope->get_mangledNameCache();
                         mangledNameCache.insert(pair<SgNode*,std::string>(mangleableNode,mangledName));
#if 0
                         string nodeName = get_name(mangleableNode);
                         printf ("At node = %p = %s = %s in local map (size = %d) \n",
                              mangleableNode,mangleableNode->class_name().c_str(),nodeName.c_str(),mangledNameCache.size());
#endif
                       }
                  }

          private:
               SgGlobal* storedGlobalScope;
        };

    // Now buid the traveral object and call the traversal (preorder) on the function definition.
       MangledNameTraversal traversal;
       traversal.traverse(globalScope, preorder);
   }
#endif


string
SageInterface::getMangledNameFromCache( SgNode* astNode )
   {
  // The TransformationSupport is not defined yet (I forget the
  // details but I recall that there is a reason why this is this way).
  // SgGlobal* globalScope = TransformationSupport::getGlobalScope(astNode);
#if 0
     SgGlobal* globalScope = isSgGlobal(astNode);

     if (globalScope == NULL && isSgFile(astNode) != NULL)
        {
          globalScope = isSgFile(astNode)->get_globalScope();
          ROSE_ASSERT(globalScope != NULL);
        }

     if (globalScope == NULL && isSgProject(astNode) != NULL)
        {
       // Check to make sure that the SgFile can be uniquely determined
          ROSE_ASSERT( isSgProject(astNode)->get_fileList()->size() == 1 );
          globalScope = isSgProject(astNode)->get_fileList()->operator[](0)->get_globalScope();
          ROSE_ASSERT(globalScope != NULL);
        }

     SgNode* temp = astNode;
     while (temp->get_parent() != NULL && globalScope == NULL)
        {
          temp = temp->get_parent();
          globalScope = isSgGlobal(temp);
        }
     ROSE_ASSERT(globalScope != NULL);
#endif

  // std::map<SgNode*,std::string> & mangledNameCache = globalScope->get_mangledNameCache();
     std::map<SgNode*,std::string> & mangledNameCache = SgNode::get_globalMangledNameMap();

  // Build an iterator
     std::map<SgNode*,std::string>::iterator i = mangledNameCache.find(astNode);

     string mangledName;
     if (i != mangledNameCache.end())
        {
       // get the precomputed mangled name!
       // printf ("Mangled name IS found in cache (node = %p = %s) \n",astNode,astNode->class_name().c_str());
          mangledName = i->second;
        }
       else
        {
       // mangled name not found in cache!
       // printf ("Mangled name NOT found in cache (node = %p = %s) \n",astNode,astNode->class_name().c_str());
        }

     return mangledName;
   }

std::string
SageInterface::addMangledNameToCache( SgNode* astNode, const std::string & oldMangledName)
   {
#if 0
     SgGlobal* globalScope = isSgGlobal(astNode);

     if (globalScope == NULL && isSgFile(astNode) != NULL)
        {
          globalScope = isSgFile(astNode)->get_globalScope();
          ROSE_ASSERT(globalScope != NULL);
        }

     if (globalScope == NULL && isSgProject(astNode) != NULL)
        {
       // Check to make sure that the SgFile can be uniquely determined
          ROSE_ASSERT( isSgProject(astNode)->get_fileList()->size() == 1 );
          globalScope = isSgProject(astNode)->get_fileList()->operator[](0)->get_globalScope();
          ROSE_ASSERT(globalScope != NULL);
        }

     SgNode* temp = astNode;
     while (temp->get_parent() != NULL && globalScope == NULL)
        {
          temp = temp->get_parent();
          globalScope = isSgGlobal(temp);
        }
     ROSE_ASSERT(globalScope != NULL);
#endif

  // std::map<SgNode*,std::string> & mangledNameCache = globalScope->get_mangledNameCache();
  // std::map<std::string, int> & shortMangledNameCache = globalScope->get_shortMangledNameCache();
     std::map<SgNode*,std::string> & mangledNameCache   = SgNode::get_globalMangledNameMap();

     std::string mangledName;

#define USE_SHORT_MANGLED_NAMES 1
#if USE_SHORT_MANGLED_NAMES
     std::map<std::string, int> & shortMangledNameCache = SgNode::get_shortMangledNameCache();

  // This bound was 40 previously!
     if (oldMangledName.size() > 40) {
       std::map<std::string, int>::const_iterator shortMNIter = shortMangledNameCache.find(oldMangledName);
       int idNumber = (int)shortMangledNameCache.size();
       if (shortMNIter != shortMangledNameCache.end())
          {
            idNumber = shortMNIter->second;
          }
         else
          {
            shortMangledNameCache.insert(std::pair<std::string, int>(oldMangledName, idNumber));
          }

       std::ostringstream mn;
       mn << 'L' << idNumber << 'R';
       mangledName = mn.str();
     } else {
       mangledName = oldMangledName;
     }
#else
  // DQ (7/24/2012): Note that using this option can cause some test codes using operators that have
  // difficult names (conversion operators to user-defined types) to fail.  See test2004_141.C for example.
  // The conversion operator "operator T&() const;" will fail because the character "&" will remain in
  // mangled name.  The substring coding changes the strings for the mangled names and this effectively
  // removes the special characters, but there could be cases where they might remain.

  // DQ (3/27/2012): Use this as a mechanism to limit the I/O but still output a warning infrequently.
     static unsigned long counter = 0;

  // DQ (3/27/2012): Use this as a mechanism to limit the I/O but still output a warning infrequently.
  // This supports debugging the new EDG 4.x interface...
     if (counter++ % 500 == 0)
        {
          printf ("WARNING: In SageInterface::addMangledNameToCache(): Using longer forms of mangled names (can cause some function names with embedded special characters to fail; test2004_141.C) \n");
        }
     mangledName = oldMangledName;
#endif

  // DQ (6/26/2007): Output information useful for understanding Jeremiah's shortended name merge caching.
  // std::cerr << "Changed MN " << oldMangledName << " to " << mangledName << std::endl;

#if 0
     printf ("Updating mangled name cache for node = %p = %s with mangledName = %s \n",astNode,astNode->class_name().c_str(),mangledName.c_str());
#endif

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
     SgStatement* statement = isSgStatement(astNode);
     if (statement != NULL && statement->hasExplicitScope() == true)
        {
          if (statement->get_scope() == NULL)
             {
               printf ("Warning: SageInterface::addMangledNameToCache(): In it might be premature to add this IR node and name to the mangledNameCache: statement = %p = %s oldMangledName = %s \n",
                    statement,statement->class_name().c_str(),oldMangledName.c_str());
             }
        }
#endif

     mangledNameCache.insert(pair<SgNode*,string>(astNode,mangledName));

  // printf ("In SageInterface::addMangledNameToCache(): returning mangledName = %s \n",mangledName.c_str());

     return mangledName;
   }


// #endif

#ifndef USE_ROSE

bool
SageInterface::declarationPreceedsDefinition ( SgDeclarationStatement* nonDefiningDeclaration, SgDeclarationStatement* definingDeclaration )
   {
  // This function is used in the unparser, but might be more generally useful.  Since it is
  // related to general AST tests, I have put it here.  It might be alternatively put in the
  // src/backend/unparser or related utility directory.

  // Preorder traversal to test the order of declaration of non-defining vs. defining class
  // declarations in the preorder traversal of the AST.
     class DeclarationOrderTraversal : public AstSimpleProcessing
        {
          public:
               DeclarationOrderTraversal( SgDeclarationStatement* nonDefiningDeclaration, SgDeclarationStatement* definingDeclaration )
                  : storedNondefiningDeclaration(nonDefiningDeclaration),
                    storedDefiningDeclaration(definingDeclaration)
                  {
                    storedDeclarationFound                            = false;
                    nonDefiningDeclarationPreceedsDefiningDeclaration = false;
                  }

               void visit (SgNode* node)
                  {
                    ROSE_ASSERT(storedNondefiningDeclaration != NULL);
                    ROSE_ASSERT(storedDefiningDeclaration != NULL);
                    ROSE_ASSERT(storedNondefiningDeclaration != storedDefiningDeclaration);
                 // ROSE_ASSERT(storedNondefiningDeclaration->get_definingDeclaration() == storedDefiningDeclaration);

                 // Even though we only care about the SgClassDeclaration IR nodes we have to traverse them
                 // in the AST in the order defined by the traversal (order of apprearance in the AST).  We
                 // also can't just search the declarations of a single scope (since the nondefining declaration
                 // can appear in a different scope than the defining declaration).
                    SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
                    if ( declaration != NULL )
                       {
                      // if (classDeclaration == storedNondefiningDeclaration && neitherStoredDeclarationFound == true)
                         if (storedDeclarationFound == false)
                            {
                              if (declaration == storedDefiningDeclaration)
                                 {
                                   storedDeclarationFound = true;
                                   nonDefiningDeclarationPreceedsDefiningDeclaration = false;
                                 }
                              if (declaration == storedNondefiningDeclaration)
                                 {
                                   storedDeclarationFound = true;
                                   nonDefiningDeclarationPreceedsDefiningDeclaration = true;
                                 }
                            }
                       }
                  }

          public:
               bool storedDeclarationFound;
               bool nonDefiningDeclarationPreceedsDefiningDeclaration;

          private:
               SgDeclarationStatement* storedNondefiningDeclaration;
               SgDeclarationStatement* storedDefiningDeclaration;
        };

     ROSE_ASSERT(nonDefiningDeclaration != NULL);
#if 0
     printf ("In SageInterface::declarationPreceedsDefinition(): \n");
     printf ("     nondefiningDeclaration                            = %p = %s \n",nonDefiningDeclaration,nonDefiningDeclaration->class_name().c_str());
     nonDefiningDeclaration->get_file_info()->display("nonDefiningDeclaration");
     printf ("     nondefiningDeclaration->get_definingDeclaration() = %p \n",nonDefiningDeclaration->get_definingDeclaration());
     printf ("     definingDeclaration                               = %p = %s \n",definingDeclaration,definingDeclaration->class_name().c_str());
     definingDeclaration->get_file_info()->display("definingDeclaration");
     printf ("************************************************************* \n");
#endif

#if 0
  // The nonDefiningDeclaration can be a different type of declaration than the definingDeclaration
     if (nonDefiningDeclaration->get_definingDeclaration() != definingDeclaration)
        {
          printf ("In SageInterface::declarationPreceedsDefinition() (warning): \n");
          printf ("     nondefiningDeclaration                            = %p \n",nonDefiningDeclaration);
          printf ("     nondefiningDeclaration->get_definingDeclaration() = %p \n",nonDefiningDeclaration->get_definingDeclaration());
          printf ("     definingDeclaration                               = %p \n",definingDeclaration);
        }
#endif

  // define trival case of classNonDefiningDeclaration == classDefiningDeclaration to be false.
     bool returnResult = false;
     if (nonDefiningDeclaration != definingDeclaration)
        {
       // Get the global scope from a traversal back (up) through the AST.
          SgGlobal* globalScope = TransformationSupport::getGlobalScope(definingDeclaration);
          ROSE_ASSERT(globalScope != NULL);

       // Now buid the traveral object and call the traversal (preorder) on the function definition.
          DeclarationOrderTraversal traversal (nonDefiningDeclaration,definingDeclaration);

#if 0
          traversal.traverse(globalScope, preorder);
#else
       // DQ (5/29/2007): To avoid order n^2 complexity in unparsing we turn off the name qualification for now.
          printf ("Skipping traversal within SageInterface::declarationPreceedsDefinition() \n");
          traversal.storedDeclarationFound = true;
          traversal.nonDefiningDeclarationPreceedsDefiningDeclaration = false;
#endif

       // I hope that we have found the input nondefining or defining declaration, if not let it be an error
       // for now.  There may however be good reasons why we might miss them (hidden island problem, or
       // nondefining declarations that are not in the traversal).

          if (traversal.storedDeclarationFound == false)
             {
#if 0
               printf ("In SageInterface::declarationPreceedsDefinition(): warning, nonDefiningDeclaration not found in the AST \n");
               nonDefiningDeclaration->get_file_info()->display("nonDefiningDeclaration");
               definingDeclaration->get_file_info()->display("definingDeclaration");
               printf ("---------------------------------------------------- \n\n");
#endif
            // Set this error case to return true so that we will not assume incorrectly
            // that the classNonDefiningDeclaration apears after the classDefiningDeclaration.
               returnResult = true;
             }
            else
             {
               returnResult = traversal.nonDefiningDeclarationPreceedsDefiningDeclaration;
             }

       // ROSE_ASSERT(traversal.storedDeclarationFound == true);
        }
#if 0
     printf ("returnResult = %s \n",returnResult ? "true" : "false");
     printf ("************************************************************* \n\n");
#endif
     return returnResult;
   }


bool
SageInterface::functionCallExpressionPreceedsDeclarationWhichAssociatesScope ( SgFunctionCallExp* functionCall )
   {
  // This function is used in the unparser, but might be more generally useful.  Since it is
  // related to general AST tests, I have put it here.  It might be alternatively put in the
  // src/backend/unparser or related utility directory.

  // Preorder traversal to test the order of declaration of non-defining vs. defining class
  // declarations in the preorder traversal of the AST.
     class DeclarationOrderTraversal : public AstSimpleProcessing
        {
          public:
               DeclarationOrderTraversal( SgFunctionCallExp* functionCall )
                  : storedFunctionCall(functionCall)
                  {
                    ROSE_ASSERT(functionCall != NULL);
                    ROSE_ASSERT(functionCall->get_function() != NULL);
                    SgExpression* functionExpression = functionCall->get_function();

                    switch (functionExpression->variantT())
                       {
                      // these are the acceptable cases
                         case V_SgDotExp:
                         case V_SgDotStarOp:
                         case V_SgArrowExp:
                         case V_SgArrowStarOp:
                         case V_SgPointerDerefExp:
                            {
                           // These are the acceptable cases, but not handled yet.
                              printf ("These are the acceptable cases, but not handled yet... \n");
                              ROSE_ASSERT(false);
                              break;
                            }

                         case V_SgFunctionRefExp:
                            {
                              SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionExpression);
                              ROSE_ASSERT(functionRefExp != NULL);
                              SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
                              ROSE_ASSERT(functionSymbol != NULL);

                           // Make sure that the function has a valid declaration
                              ROSE_ASSERT(functionSymbol->get_declaration() != NULL);
                              storedFunctionDeclaration = functionSymbol->get_declaration();
                              break;
                            }

                         case V_SgMemberFunctionRefExp:
                            {
                              SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(functionExpression);
                              ROSE_ASSERT(memberFunctionRefExp != NULL);
                              SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();
                              ROSE_ASSERT(memberFunctionSymbol != NULL);

                              storedFunctionDeclaration = memberFunctionSymbol->get_declaration();

                              printf ("V_SgMemberFunctionRefExp case not handled yet... \n");
                              ROSE_ASSERT(false);
                            }

                         default:
                            {
                              printf ("default reached in SageInterface::functionCallExpressionPreceedsDeclarationWhichAssociatesScope() functionExpression = %s \n",
                                   functionExpression->class_name().c_str());
                              ROSE_ASSERT(false);
                            }
                       }
                 // ROSE_ASSERT(functionCall->get_declaration() != NULL);
                 // ROSE_ASSERT(functionCall->get_function()->get_firstNondefiningDeclaration() != NULL);
                 // storedFunctionDeclaration = functionCall->get_declaration()->get_firstNondefiningDeclaration();
                    ROSE_ASSERT(storedFunctionDeclaration != NULL);

                    storedFunctionCallFound                                       = false;
                    functionCallExpressionPreceedsDeclarationWhichAssociatesScope = false;
                  }

               void visit (SgNode* node)
                  {
                    ROSE_ASSERT(storedFunctionCall != NULL);
                    ROSE_ASSERT(storedFunctionDeclaration != NULL);
                 // ROSE_ASSERT(storedFunctionCall != storedFunctionDeclaration);

                    if (storedFunctionCallFound == false)
                       {
                         SgFunctionCallExp* functionCall = isSgFunctionCallExp(node);
                         if ( functionCall != NULL )
                            {
                              if (functionCall == storedFunctionCall)
                                 {
                                   storedFunctionCallFound = true;

                                // A declaration for the function in a scope where the function could be defined
                                // (and a scope associated with it) has not been found so the function call
                                // preceeds such a declaration (if it even exists).
                                   functionCallExpressionPreceedsDeclarationWhichAssociatesScope = true;
                                 }
                            }

                         SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
                         ROSE_ASSERT(storedFunctionDeclaration != NULL);

                      // Note that all firstNondefiningDeclaration are set to the same declaration (for all associated declarations).
                      // Need to check the result of get_firstNondefiningDeclaration() since this will be the same for all declarations
                      // of the same function and thus can be used to match that we have an associated declaration for the same function.
                      // if ( declaration != NULL && declaration->get_firstNondefiningDeclaration() == storedFunctionDeclaration)
                         if ( declaration != NULL && declaration->get_firstNondefiningDeclaration() == storedFunctionDeclaration->get_firstNondefiningDeclaration())
                            {
                           // Test if this is a declaration is a scope where the existance of the forward
                           // declaration will define the scope fo the function declaration.
                           // if (declaration->get_scope()->supportsDefiningFunctionDeclaration() == true)
                              SgScopeStatement* parentScopeOfDeclaration = isSgScopeStatement(declaration->get_parent());
                              if (parentScopeOfDeclaration != NULL && parentScopeOfDeclaration->supportsDefiningFunctionDeclaration() == true)
                                 {
                                // We are done so we can skip further testing
                                   storedFunctionCallFound = true;

                                // We have found a declaration which will associated the scope of a function declaration
                                // (so all function calls after this point can be qualified (and might have to be).
                                   functionCallExpressionPreceedsDeclarationWhichAssociatesScope = false;
#if 0
                                   printf ("Found a declaration which preceeds the function \n");
                                   declaration->get_file_info()->display("Found a declaration which preceeds the function: declaration");
                                   storedFunctionCall->get_file_info()->display("Found a declaration which preceeds the function: storedFunctionCall");
                                   storedFunctionDeclaration->get_file_info()->display("Found a declaration which preceeds the function: storedFunctionDeclaration");
#endif
                                 }
                                else
                                 {
                                // Error checking!
                                   if (parentScopeOfDeclaration == NULL)
                                      {
                                     // This might be a function declaration (non-defining) used in a type or buried deeply in some sort of declaration!
                                        printf ("Strange case of parentScopeOfDeclaration == NULL in SageInterface::functionCallExpressionPreceedsDeclarationWhichAssociatesScope() \n");
                                        printf ("declaration->get_parent() = %s \n",declaration->get_parent()->class_name().c_str());
                                        declaration->get_file_info()->display("case of parentScopeOfDeclaration == NULL");
                                        ROSE_ASSERT(false);
                                      }
                                 }
                            }
                       }
                  }

          public:
               bool storedFunctionCallFound;
               bool functionCallExpressionPreceedsDeclarationWhichAssociatesScope;

          private:
               SgFunctionCallExp* storedFunctionCall;
               SgDeclarationStatement* storedFunctionDeclaration;
        };

     ROSE_ASSERT(functionCall != NULL);
#if 0
     printf ("In SageInterface::functionCallExpressionPreceedsDeclarationWhichAssociatesScope(): \n");
     printf ("     storedFunctionCall                           = %p = %s \n",functionCall,functionCall->class_name().c_str());
     functionCall->get_file_info()->display("storedFunctionCall");
     printf ("     storedFunctionCall->get_function() = %p = %s \n",functionCall->get_function(),functionCall->get_function()->class_name().c_str());
     printf ("************************************************************* \n");
#endif

  // define trival case of classNonDefiningDeclaration == classDefiningDeclaration to be false.
     bool returnResult = false;

  // Get the global scope from a traversal back (up) through the AST.
     SgGlobal* globalScope = TransformationSupport::getGlobalScope(functionCall);
     ROSE_ASSERT(globalScope != NULL);

  // Now buid the traveral object and call the traversal (preorder) on the function definition.
     DeclarationOrderTraversal traversal (functionCall);

#if 0
     traversal.traverse(globalScope, preorder);
#else
  // DQ (5/29/2007): To avoid order n^2 complexity in unparsing we turn off the name qualification for now.
     printf ("Skipping traversal within SageInterface::functionCallExpressionPreceedsDeclarationWhichAssociatesScope() \n");
     traversal.storedFunctionCallFound = true;
     traversal.functionCallExpressionPreceedsDeclarationWhichAssociatesScope = false;
#endif

  // I hope that we have found the input nondefining or defining declaration, if not let it be an error
  // for now.  There may however be good reasons why we might miss them (hidden island problem, or
  // nondefining declarations that are not in the traversal).

     if (traversal.storedFunctionCallFound == false)
        {
#if 0
          printf ("In SageInterface::functionCallExpressionPreceedsDeclarationWhichAssociatesScope(): warning, storedFunctionCall not found in the AST \n");
          functionCall->get_file_info()->display("functionCall");
          printf ("---------------------------------------------------- \n\n");
#endif
       // Set this error case to return true so that we will not assume incorrectly
       // that the function call is used after a declaration that associated the scope
       // with the function.  This still causes the global name qualifier to be omitted.
          returnResult = true;
        }
       else
        {
          returnResult = traversal.functionCallExpressionPreceedsDeclarationWhichAssociatesScope;
        }

  // I hope that this is nearly always be true!
     ROSE_ASSERT(traversal.storedFunctionCallFound == true);
#if 0
     printf ("returnResult = %s \n",returnResult ? "true" : "false");
     printf ("************************************************************* \n\n");
#endif
     return returnResult;
   }

#endif

// #ifndef USE_ROSE

string
SageInterface::generateProjectName( const SgProject* project, bool supressSuffix )
   {
  // This function generates a string to use as a unique project name for
  // a collection of files.  The last file will include it's suffix so that
  // we generate proper names that communicate the source language.
  // Also also allows the single file case to be consistant with the previous
  // version of names generated for "DOT" files in the tutorial.

  // DQ (9/6/2008): Introduced optional parameter to supresse the suffix in the
  // generation of the project name so that we can support more complex name
  // construction as required for the generation of names for the whole AST
  // graphs which append an additional suffix to avoid filename collision.

     ROSE_ASSERT(project != NULL);
     string projectName;

  // DQ (9/2/2008): Removed the redundant function getFileNames().
  // Rose_STL_Container<string> fileList = project->get_sourceFileNameList();
     Rose_STL_Container<string> fileList = project->getAbsolutePathFileNames();

     Rose_STL_Container<string>::iterator i = fileList.begin();

  // Handle the case of an empty list (e.g. for link lines).
     if ( fileList.empty() == true )
        {
          return "empty_file_list";
        }

     do {
          string filename = *i;

       // printf ("In SageInterface::generateProjectName(): absolute filename = %s \n",filename.c_str());

       // string filenameWithoutSuffix       = StringUtility::stripFileSuffixFromFileName(filename);

          if (i != fileList.begin())
               projectName += "--";

          i++;

          string filenameWithoutSuffix;
          if ( i != fileList.end() || supressSuffix == true )
               filenameWithoutSuffix = StringUtility::stripFileSuffixFromFileName(filename);
            else
               filenameWithoutSuffix = filename;

          string filenameWithoutPathOrSuffix = StringUtility::stripPathFromFileName(filenameWithoutSuffix);

       // printf ("filenameWithoutSuffix       = %s \n",filenameWithoutSuffix.c_str());
       // printf ("filenameWithoutPathOrSuffix = %s \n",filenameWithoutPathOrSuffix.c_str());

          filename = filenameWithoutPathOrSuffix;

          unsigned long int n = 0;
          while (n < filename.size())
             {
               if (filename[n] == '/')
                    filename[n] = '_';
               n++;
             }

       // printf ("In SageInterface:generateProjectName(): modified absolute filename = %s \n",filename.c_str());

          projectName += filename;

       // printf ("In SageInterface:generateProjectName(): evolving projectName = %s \n",projectName.c_str());
        }
     while (i != fileList.end());

  // printf ("In SageInterface:generateProjectName(): projectName = %s \n",projectName.c_str());

     return projectName;
   }

// #endif

// #ifndef USE_ROSE

SgFunctionSymbol*
SageInterface::lookupFunctionSymbolInParentScopes(const SgName & functionName, SgScopeStatement* currentScope )
   {
  // DQ (11/24/2007): This function can return NULL.  It returns NULL when the function symbol is not found.
  // This can happen when a function is referenced before it it defined (no prototype mechanism in Fortran is required).

  // enable default search from top of StackScope, Liao, 1/24/2008
     SgFunctionSymbol* functionSymbol = NULL;
     if (currentScope == NULL)
          currentScope = SageBuilder::topScopeStack();
     ROSE_ASSERT(currentScope != NULL);

     SgScopeStatement* tempScope = currentScope;
     while ((functionSymbol == NULL) && (tempScope != NULL))
        {
          functionSymbol = tempScope->lookup_function_symbol(functionName);
#if 0
          printf ("In lookupFunctionSymbolInParentScopes(): Searching scope = %p = %s functionName = %s functionSymbol = %p \n",tempScope,tempScope->class_name().c_str(),functionName.str(),functionSymbol);
#endif
          if (tempScope->get_parent()!=NULL) // avoid calling get_scope when parent is not set in middle of translation
               tempScope = isSgGlobal(tempScope) ? NULL : tempScope->get_scope();
            else
               tempScope = NULL;
        }
     return functionSymbol;
   }


void
SageInterface::addTextForUnparser ( SgNode* astNode, string s, AstUnparseAttribute::RelativePositionType inputlocation )
   {
    // printf ("addText(): using new attribute interface (s = %s) \n",s.c_str());

     if (isSgType(astNode) != NULL)
        {
          printf ("Error: the mechanism to add text to be unparsed at IR nodes is not intended to operate on SgType IR nodes (since they are shared) \n");
          ROSE_ASSERT(false);
        }

     if (astNode->attributeExists(AstUnparseAttribute::markerName) == true)
        {
          AstUnparseAttribute* code = dynamic_cast<AstUnparseAttribute*>(astNode->getAttribute(AstUnparseAttribute::markerName));
          ROSE_ASSERT(code != NULL);

       // DQ (2/23/2009): commented added.
       // Since there is at least one other string (there is an existing attribute) the relative order of the strings is significant.
          code->addString(s,inputlocation);
        }
       else
        {
       // DQ (2/23/2009): commented added.
       // Note that this will be the only string in the attribute, so inputlocation is not significant (and e_before is the default used).
          AstUnparseAttribute* code = new AstUnparseAttribute(s,AstUnparseAttribute::e_before);
          ROSE_ASSERT(code != NULL);

          astNode->addNewAttribute(AstUnparseAttribute::markerName,code);
        }
   }


#if 0
// DQ (7/20/2011): Resolving conflict, this was added in previous work in dq-cxx-rc branch.
// DQ (7/17/2011): Added function from cxx branch that I need here for the Java support.
SgClassSymbol *
SageInterface::lookupClassSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope)
   {
  // DQ (5/7/2011): I think this is the better implementation that lookupVariableSymbolInParentScopes() should have.
     SgClassSymbol* symbol = NULL;
     if (cscope == NULL)
          cscope = SageBuilder::topScopeStack();
     ROSE_ASSERT(cscope != NULL);

     while ((cscope != NULL) && (symbol == NULL))
        {
       // I think this will resolve SgAliasSymbols to be a SgClassSymbol where the alias is of a SgClassSymbol.
          symbol = cscope->lookup_class_symbol(name,NULL);

          if (cscope->get_parent() != NULL) // avoid calling get_scope when parent is not set
               cscope = isSgGlobal(cscope) ? NULL : cscope->get_scope();
            else
               cscope = NULL;
        }

     return symbol;
   }
#endif



SgType* SageInterface::lookupNamedTypeInParentScopes(const std::string& type_name, SgScopeStatement* scope/*=NULL*/)
   {
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();

  // DQ (8/16/2013): Added NULL pointers to at least handle the API change.
     SgSymbol* symbol = lookupSymbolInParentScopes (type_name,scope,NULL,NULL);

     if (symbol == NULL)
          return NULL;
       else 
          return symbol->get_type();
   }

SgFunctionSymbol *SageInterface::lookupFunctionSymbolInParentScopes (const SgName &functionName,
                                                        const SgType* t,
                                                        SgScopeStatement *currentScope)
                                                        //SgScopeStatement *currentScope=NULL)
{
    SgFunctionSymbol* functionSymbol = NULL;
    if (currentScope == NULL)
        currentScope = SageBuilder::topScopeStack();
    ROSE_ASSERT(currentScope != NULL);
    SgScopeStatement* tempScope = currentScope;
    while (functionSymbol == NULL && tempScope != NULL)
    {
        functionSymbol = tempScope->lookup_function_symbol(functionName,t);
        if (tempScope->get_parent()!=NULL) // avoid calling get_scope when parent is not set
            tempScope = isSgGlobal(tempScope) ? NULL : tempScope->get_scope();
        else tempScope = NULL;
    }
    return functionSymbol;
}

// Liao, 1/22/2008
// SgScopeStatement* SgStatement::get_scope
// SgScopeStatement* SgStatement::get_scope() assumes all parent pointers are set, which is
// not always true during translation.
// SgSymbol *SageInterface:: lookupSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope)
SgSymbol*
SageInterface::lookupSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope, SgTemplateParameterPtrList* templateParameterList, SgTemplateArgumentPtrList* templateArgumentList)
   {
     SgSymbol* symbol = NULL;
     if (cscope == NULL)
          cscope = SageBuilder::topScopeStack();

     ROSE_ASSERT(cscope != NULL);

#if 0
     printf ("In SageInterface:: lookupSymbolInParentScopes(): cscope = %p = %s (templateParameterList = %p templateArgumentList = %p) \n",cscope,cscope->class_name().c_str(),templateParameterList,templateArgumentList);
#endif

     while ((cscope != NULL) && (symbol == NULL))
        {
#if 0
       // DQ (5/21/2013): Restricting direct access to the symbol table to support namespace symbol table support.
          if (cscope->get_symbol_table() == NULL)
             {
               printf ("Error: cscope->get_symbol_table() == NULL for cscope = %p = %s \n",cscope,cscope->class_name().c_str());
               cscope->get_startOfConstruct()->display("cscope->p_symbol_table == NULL: debug");
#if 0
               ROSE_ASSERT(cscope->get_parent() != NULL);
               SgNode* parent = cscope->get_parent();
               while (parent != NULL)
                  {
                    printf ("Error: cscope->get_symbol_table() == NULL for parent = %p = %s \n",parent,parent->class_name().c_str());
                    parent->get_startOfConstruct()->display("parent == NULL: debug");
                    parent = parent->get_parent();
                  }
#endif
             }
          ROSE_ASSERT(cscope->get_symbol_table() != NULL);
#endif

#if 0
          printf("   --- In SageInterface:: lookupSymbolInParentScopes(): name = %s cscope = %p = %s \n",name.str(),cscope,cscope->class_name().c_str());
#endif

       // DQ (8/16/2013): Changed API to support template parameters and template arguments.
       // symbol = cscope->lookup_symbol(name);
          symbol = cscope->lookup_symbol(name,templateParameterList,templateArgumentList);

#if 0
       // debug
          printf("   --- In SageInterface:: lookupSymbolInParentScopes(): symbol = %p \n",symbol);
          cscope->print_symboltable("In SageInterface:: lookupSymbolInParentScopes(): debug");
#endif
          if (cscope->get_parent() != NULL) // avoid calling get_scope when parent is not set
               cscope = isSgGlobal(cscope) ? NULL : cscope->get_scope();
            else
               cscope = NULL;

#if 0
          printf ("   --- In SageInterface:: lookupSymbolInParentScopes(): symbol = %p \n",symbol);
#endif
        }

     if (symbol == NULL)
        {
#if 0
          printf ("Warning: In SageInterface:: lookupSymbolInParentScopes(): could not locate the specified name %s in any outer symbol table (templateParameterList = %p templateArgumentList = %p) \n",name.str(),templateParameterList,templateArgumentList);
#endif
       // ROSE_ASSERT(false);
        }

     return symbol;
   }

#if 0
// DQ (7/13/2011): This was part of a merge conflict with the above modified function.
// It appeas they are the same so this one is commented out.
SgSymbol *SageInterface:: lookupSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope)
{
    SgSymbol* symbol = NULL;
    if (cscope == NULL)
        cscope = SageBuilder::topScopeStack();
    ROSE_ASSERT(cscope);

    while ((cscope!=NULL)&&(symbol==NULL))
    {
        symbol = cscope->lookup_symbol(name);
        //debug
        // cscope->print_symboltable("debug sageInterface.C L3749...");
        if (cscope->get_parent()!=NULL) // avoid calling get_scope when parent is not set
            cscope = isSgGlobal(cscope) ? NULL : cscope->get_scope();
        else
            cscope = NULL;
    }

    if (symbol==NULL)
    {
        //    printf ("Warning: could not locate the specified name %s in any outer symbol table \n"e,
        //  name.str());
        //  ROSE_ASSERT(false);
    }
    return symbol;
}
#endif

SgVariableSymbol *
SageInterface::lookupVariableSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope)
   {
  // DQ (1/24/2011): This function is inconsistant with an implementation that would correctly handle SgAliasSymbols.
  // Also this function might get a SgClassSymbol instead of a SgVariableSymbol when both names are used.
  // This function needs to be fixed to handle the multi-map semantics of the symbol tables.

#if 0
  // DQ (5/7/2011): I think this implementation is not correct (does not resolve past hidden types) and so should
  // be fixed to be consistant with the implementation of SageInterface::lookupClassSymbolInParentScopes().
  // Since I don't know where this function is used, I don't want to change it just yet.
     printf ("WARNING: SageInterface::lookupVariableSymbolInParentScopes() should be implemented similar to SageInterface::lookupClassSymbolInParentScopes() \n");

     SgVariableSymbol* result = NULL;
     SgSymbol* symbol=lookupSymbolInParentScopes(name,cscope);
     if (symbol != NULL)
        {
          if (isSgAliasSymbol(symbol) != NULL)
             {
               printf ("Error: This SageInterface::lookupVariableSymbolInParentScopes() function does not handle SgAliasSymbols \n");
               ROSE_ASSERT(false);
             }
          result = isSgVariableSymbol(symbol);
        }
     return result;
#else
  // I think this is the better implementation.
     SgVariableSymbol* symbol = NULL;
     if (cscope == NULL)
          cscope = SageBuilder::topScopeStack();
     ROSE_ASSERT(cscope != NULL);

     while ((cscope != NULL) && (symbol == NULL))
        {
       // I think this will resolve SgAliasSymbols to be a SgClassSymbol where the alias is of a SgClassSymbol.
          symbol = cscope->lookup_variable_symbol(name);

          if (cscope->get_parent() != NULL) // avoid calling get_scope when parent is not set
               cscope = isSgGlobal(cscope) ? NULL : cscope->get_scope();
            else
               cscope = NULL;
        }

     return symbol;
#endif
   }

// SgClassSymbol* SageInterface::lookupClassSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope)
SgClassSymbol*
SageInterface::lookupClassSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope, SgTemplateArgumentPtrList* templateArgumentList)
   {
  // DQ (5/7/2011): I think this is the better implementation that lookupVariableSymbolInParentScopes() should have.
     SgClassSymbol* symbol = NULL;
     if (cscope == NULL)
          cscope = SageBuilder::topScopeStack();
     ROSE_ASSERT(cscope != NULL);

     while ((cscope != NULL) && (symbol == NULL))
        {
       // I think this will resolve SgAliasSymbols to be a SgClassSymbol where the alias is of a SgClassSymbol.
       // symbol = cscope->lookup_class_symbol(name,NULL);
          symbol = cscope->lookup_class_symbol(name,templateArgumentList);

          if (cscope->get_parent() != NULL) // avoid calling get_scope when parent is not set
               cscope = isSgGlobal(cscope) ? NULL : cscope->get_scope();
            else
               cscope = NULL;
        }

     return symbol;
   }

SgTypedefSymbol *
SageInterface::lookupTypedefSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope)
   {
  // DQ (5/7/2011): This is similar to lookupClassSymbolInParentScopes().
     SgTypedefSymbol* symbol = NULL;
     if (cscope == NULL)
          cscope = SageBuilder::topScopeStack();
     ROSE_ASSERT(cscope != NULL);

     while ((cscope != NULL) && (symbol == NULL))
        {
       // I think this will resolve SgAliasSymbols to be a SgClassSymbol where the alias is of a SgClassSymbol.
          symbol = cscope->lookup_typedef_symbol(name);

          if (cscope->get_parent() != NULL) // avoid calling get_scope when parent is not set
               cscope = isSgGlobal(cscope) ? NULL : cscope->get_scope();
            else
               cscope = NULL;
        }

     return symbol;
   }

#if 0
// DQ (8/13/2013): This function does not make since any more, now that we have make the symbol
// table handling more precise and we have to provide template parameters for any template lookup.
// We also have to know if we want to lookup template classes, template functions, or template 
// member functions (since each have specific requirements).
SgTemplateSymbol*
SageInterface::lookupTemplateSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope)
   {
  // DQ (5/7/2011): This is similar to lookupClassSymbolInParentScopes().
     SgTemplateSymbol* symbol = NULL;
     if (cscope == NULL)
          cscope = SageBuilder::topScopeStack();
     ROSE_ASSERT(cscope != NULL);

     while ((cscope != NULL) && (symbol == NULL))
        {
       // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
       // In this case these are unavailable from this point.
       // I think this will resolve SgAliasSymbols to be a SgClassSymbol where the alias is of a SgClassSymbol.
       // symbol = cscope->lookup_template_symbol(name);
          symbol = cscope->lookup_template_symbol(name,NULL,NULL);
#if 0
          printf ("In lookupTemplateSymbolInParentScopes(): Searching scope = %p = %s name = %s symbol = %p \n",cscope,cscope->class_name().c_str(),name.str(),symbol);
#endif
          if (cscope->get_parent() != NULL) // avoid calling get_scope when parent is not set
               cscope = isSgGlobal(cscope) ? NULL : cscope->get_scope();
            else
               cscope = NULL;
        }

     return symbol;
   }
#endif


SgTemplateClassSymbol*
SageInterface::lookupTemplateClassSymbolInParentScopes (const SgName &  name, SgTemplateParameterPtrList* templateParameterList, SgTemplateArgumentPtrList* templateArgumentList, SgScopeStatement *cscope)
   {
  // DQ (5/7/2011): This is similar to lookupClassSymbolInParentScopes().
     SgTemplateClassSymbol* symbol = NULL;
     if (cscope == NULL)
          cscope = SageBuilder::topScopeStack();
     ROSE_ASSERT(cscope != NULL);

     while ((cscope != NULL) && (symbol == NULL))
        {
       // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
       // In this case these are unavailable from this point.
       // I think this will resolve SgAliasSymbols to be a SgClassSymbol where the alias is of a SgClassSymbol.
       // symbol = cscope->lookup_template_symbol(name);
          symbol = cscope->lookup_template_class_symbol(name,templateParameterList,templateArgumentList);
#if 0
          printf ("In lookupTemplateSymbolInParentScopes(): Searching scope = %p = %s name = %s symbol = %p \n",cscope,cscope->class_name().c_str(),name.str(),symbol);
#endif
          if (cscope->get_parent() != NULL) // avoid calling get_scope when parent is not set
               cscope = isSgGlobal(cscope) ? NULL : cscope->get_scope();
            else
               cscope = NULL;
        }

     return symbol;
   }


SgEnumSymbol *
SageInterface::lookupEnumSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope)
   {
  // DQ (5/7/2011): This is similar to lookupClassSymbolInParentScopes().
  // A templated solution might make for a better implementation.
     SgEnumSymbol* symbol = NULL;
     if (cscope == NULL)
          cscope = SageBuilder::topScopeStack();
     ROSE_ASSERT(cscope != NULL);

     while ((cscope != NULL) && (symbol == NULL))
        {
       // I think this will resolve SgAliasSymbols to be a SgClassSymbol where the alias is of a SgClassSymbol.
          symbol = cscope->lookup_enum_symbol(name);

          if (cscope->get_parent() != NULL) // avoid calling get_scope when parent is not set
               cscope = isSgGlobal(cscope) ? NULL : cscope->get_scope();
            else
               cscope = NULL;
        }

     return symbol;
   }

SgNamespaceSymbol *
SageInterface::lookupNamespaceSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope)
   {
  // DQ (5/7/2011): This is similar to lookupClassSymbolInParentScopes().
     SgNamespaceSymbol* symbol = NULL;
     if (cscope == NULL)
          cscope = SageBuilder::topScopeStack();
     ROSE_ASSERT(cscope != NULL);

     while ((cscope != NULL) && (symbol == NULL))
        {
       // I think this will resolve SgAliasSymbols to be a SgNamespaceSymbol where the alias is of a SgNamespaceSymbol.
          symbol = cscope->lookup_namespace_symbol(name);

          if (cscope->get_parent() != NULL) // avoid calling get_scope when parent is not set
               cscope = isSgGlobal(cscope) ? NULL : cscope->get_scope();
            else
               cscope = NULL;
        }

     return symbol;
   }

#if 0
// DQ (5/2/2012): This is redundant with the more general function using the same name.
void
SageInterface::setSourcePosition( SgLocatedNode* locatedNode )
   {
  // DQ (5/1/2012): Older depricated function.
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
     printf ("+++++ Depricated function setSourcePosition() (use setSourcePositionToDefault() instead) \n");
#endif
     setSourcePositionToDefault(locatedNode);
   }
#endif

#if 0
// DQ (5/3/2012): This version does not handle SgPragma and so is not used (a templaed version is implemented below).
void
SageInterface::setSourcePositionToDefault( SgLocatedNode* locatedNode )
   {
  // DQ (1/24/2009): It might be that this function is only called from the Fortran support.

  // This function sets the source position to be marked as not
  // available (since we often don't have token information)
  // These nodes WILL be unparsed in the conde generation phase.

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

  // We have to support this being called where the Sg_File_Info have previously been set.
     if (locatedNode->get_endOfConstruct() == NULL && locatedNode->get_startOfConstruct() == NULL)
        {
       // Check the endOfConstruct first since it is most likely NULL (helpful in debugging)
          ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);
          ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);

          Sg_File_Info* start_fileInfo = Sg_File_Info::generateDefaultFileInfo();
          Sg_File_Info* end_fileInfo   = Sg_File_Info::generateDefaultFileInfo();

       // DQ (5/2/2012): I think we don't want to do this.
          printf ("In SageInterface::setSourcePositionToDefault(): Calling setSourcePositionUnavailableInFrontend() \n");
          start_fileInfo->setSourcePositionUnavailableInFrontend();
          end_fileInfo->setSourcePositionUnavailableInFrontend();

       // DQ (5/2/2012): I think we don't want to do this.
          printf ("In SageInterface::setSourcePositionToDefault(): Calling setOutputInCodeGeneration() \n");

       // This is required for the unparser to output the code from the AST.
          start_fileInfo->setOutputInCodeGeneration();
          end_fileInfo->setOutputInCodeGeneration();

          locatedNode->set_startOfConstruct(start_fileInfo);
          locatedNode->set_endOfConstruct  (end_fileInfo);

          locatedNode->get_startOfConstruct()->set_parent(locatedNode);
          locatedNode->get_endOfConstruct  ()->set_parent(locatedNode);
        }
       else
        {
       // If both the starting  and ending Sg_File_Info pointers are not NULL then both must be valid.
       // We don't want to support partially completed source code position information.

          if (locatedNode->get_startOfConstruct() == NULL)
             {
               printf ("ERROR: startOfConstruct not set for locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
             }
          if (locatedNode->get_endOfConstruct() == NULL)
             {
               printf ("ERROR: endOfConstruct not set for locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
             }

          ROSE_ASSERT(locatedNode->get_endOfConstruct()   != NULL);
          ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
          ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL && locatedNode->get_startOfConstruct() != NULL);
        }
   }
#endif

template<class T>
void
SageInterface::setSourcePositionToDefault( T* node )
   {
  // This is a templated function because SgPragma is not yet derived from SgLocatedNode.

  // DQ (2/17/2013): This function is called a lot, so it might be a performance issue.
  // All IR nodes built by the Build Interface are assighed source position information
  // using this function and then reset afterward as we use information within EDG to 
  // reset the source position information.  Ideally, the EDG/ROSE connection would
  // use NULL pointers as the behavior for the front-end mode.  We can move to that 
  // later to maek the source position handling more efficient.

  // DQ (1/24/2009): It might be that this function is only called from the Fortran support.

  // This function sets the source position to be marked as not
  // available (since we often don't have token information)
  // These nodes WILL be unparsed in the conde generation phase.

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(node != NULL);

  // We have to support this being called where the Sg_File_Info have previously been set.
     if (node->get_endOfConstruct() == NULL && node->get_startOfConstruct() == NULL)
        {
#if 0
          printf ("Both startOfConstruct and endOfConstruct are NOT yet initialized with pointers to Sg_File_Info objects (node = %p = %s) \n",node,node->class_name().c_str());
#endif
       // Check the endOfConstruct first since it is most likely NULL (helpful in debugging)
          ROSE_ASSERT(node->get_endOfConstruct()   == NULL);
          ROSE_ASSERT(node->get_startOfConstruct() == NULL);

          Sg_File_Info* start_fileInfo = Sg_File_Info::generateDefaultFileInfo();
          Sg_File_Info* end_fileInfo   = Sg_File_Info::generateDefaultFileInfo();

       // DQ (5/2/2012): I think we don't want to do this.
#if 0
          printf ("In SageInterface::setSourcePositionToDefault(): Calling setSourcePositionUnavailableInFrontend() \n");
#endif
          start_fileInfo->setSourcePositionUnavailableInFrontend();
          end_fileInfo->setSourcePositionUnavailableInFrontend();

       // DQ (5/2/2012): I think we don't want to do this.
#if 0
          printf ("In SageInterface::setSourcePositionToDefault(): Calling setOutputInCodeGeneration() \n");
#endif
       // This is required for the unparser to output the code from the AST.
          start_fileInfo->setOutputInCodeGeneration();
          end_fileInfo->setOutputInCodeGeneration();

          node->set_startOfConstruct(start_fileInfo);
          node->set_endOfConstruct  (end_fileInfo);

          node->get_startOfConstruct()->set_parent(node);
          node->get_endOfConstruct  ()->set_parent(node);
        }
       else
        {
       // If both the starting  and ending Sg_File_Info pointers are not NULL then both must be valid.
       // We don't want to support partially completed source code position information.
#if 0
          printf ("Both startOfConstruct and endOfConstruct are ALREADY initialized with pointers to Sg_File_Info objects (node = %p = %s) \n",node,node->class_name().c_str());
#endif
          if (node->get_startOfConstruct() == NULL)
             {
               printf ("ERROR: startOfConstruct not set for locatedNode = %p = %s \n",node,node->class_name().c_str());
             }
          if (node->get_endOfConstruct() == NULL)
             {
               printf ("ERROR: endOfConstruct not set for locatedNode = %p = %s \n",node,node->class_name().c_str());
             }

          ROSE_ASSERT(node->get_startOfConstruct() != NULL);
          ROSE_ASSERT(node->get_endOfConstruct()   != NULL);
          ROSE_ASSERT(node->get_endOfConstruct() != NULL && node->get_startOfConstruct() != NULL);
        }

  // DQ (11/2/2012): This is an important fix to support the new EDG 4.x branch.
  // Note that because the unparser will use the function isFromAnotherFile() in the unparsing
  // of expressions, specifically: SgAggregateInitializer, SgCompoundInitializer, and anything 
  // in their expression lists (which could be any expression).   The isFromAnotherFile() will
  // use the get_file_info() function on the SgExpression IR nodes and the data from that 
  // Sg_File_Info object to determine if that expression subtree should be unparsed.  This 
  // expression level granularity of unparsing capability is extremely useful in handling
  // now #includes and other CPP directives are woven back into the AST.  But since the
  // get_file_info() function is used, and it returns the value of get_operatorPosition(),
  // it is critically important to have correct data in the SgExpression::p_operatorPosition
  // Sg_File_Info object (it counts more that the startOfConstruct and endOfConstruct
  // Sg_File_Info objects in controlling what expressions are unparsed.  So we have to set these
  // up for all expressions (since any SgExpression could appear in the list contained in 
  // a SgAggregateInitializer or SgCompoundInitializer.

  // DQ (11/2/2012): Set the operator source position information to default values.
  // This will trigger it to be reset to valid source position information in the front-end.
     SgExpression* expression = isSgExpression(node);
     if (expression != NULL)
        {
#if 0
          SgBinaryOp* binaryOp = isSgBinaryOp(expression);
          if (binaryOp != NULL)
             {
               if (binaryOp->get_operatorPosition() == NULL)
                  {
                    Sg_File_Info* operator_fileInfo = Sg_File_Info::generateDefaultFileInfo();
                    operator_fileInfo->setSourcePositionUnavailableInFrontend();
                    operator_fileInfo->setOutputInCodeGeneration();
                    binaryOp->set_operatorPosition(operator_fileInfo);

                 // This is equivalent to: "operator_fileInfo->set_parent(binaryOp);"
                    binaryOp->get_operatorPosition()->set_parent(binaryOp);
                  }
             }
#else
       // Setup all of the SgExpression operatorPosition pointers to default objects.
          if (expression->get_operatorPosition() == NULL)
             {
               Sg_File_Info* operator_fileInfo = Sg_File_Info::generateDefaultFileInfo();
               operator_fileInfo->setSourcePositionUnavailableInFrontend();
               operator_fileInfo->setOutputInCodeGeneration();
               expression->set_operatorPosition(operator_fileInfo);

            // This is equivalent to: "operator_fileInfo->set_parent(binaryOp);"
            // expression->get_operatorPosition()->set_parent(expression);
               operator_fileInfo->set_parent(expression);
               ROSE_ASSERT(expression->get_operatorPosition()->get_parent() == expression);
             }
#endif
        }
   }


void
SageInterface::setOneSourcePositionForTransformation(SgNode *node)
   {
  // DQ (5/1/2012): Older depricated function.
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
     printf ("+++++ Depricated function (use setSourcePositionAsTransformation() instead) (no using internal source position mode) \n");
#endif

  // setSourcePositionAsTransformation(node);
     setSourcePosition(node);
   }


void
SageInterface::setSourcePositionAsTransformation(SgNode *node)
   {
  // DQ (5/1/2012): Newer function to support specification of IR nodes being a part of a transformation.

  // DQ (1/24/2009): I think this should be renamed to be "setSourcePositionAsTransformation(SgNode *node)"
  // The logic should be make more independent of if (locatedNode->get_startOfConstruct() == NULL)
  // Since that make understanding where the function is applied too complex.
  // If (locatedNode->get_startOfConstruct() != NULL), then we should just make that Sg_File_Info as
  // a transforamtion directly, this function misses that step.

     ROSE_ASSERT(node != NULL);

  // DQ (10/12/2012): Commented out since we are past AST regression tests passing and now trying to get this work checked in.
//     printf ("In SageInterface::setSourcePositionAsTransformation() for node = %p = %s (make this an error while debugging AST construction) \n",node,node->class_name().c_str());
  // ROSE_ASSERT(false);

     SgLocatedNode*     locatedNode = isSgLocatedNode(node);
     SgExpression*      expression  = isSgExpression(node);
  // SgInitializedName* initName    = isSgInitializedName(node);
     SgPragma*          pragma      = isSgPragma(node); // missed this one!! Liao, 1/30/2008
     SgGlobal*          global      = isSgGlobal(node); // SgGlobal should have NULL endOfConstruct()

#if 0
     SgVariableDefinition * v_d = isSgVariableDefinition(node);
     if (v_d )
       printf ("Debug, Found a variable definition: %p\n", v_d);
#endif

  // if ((locatedNode) && (locatedNode->get_endOfConstruct() == NULL))
  // if ( (locatedNode != NULL) && (locatedNode->get_startOfConstruct() == NULL) )
     if (locatedNode != NULL)
        {
          locatedNode->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
          locatedNode->get_startOfConstruct()->set_parent(locatedNode);

          if (global==NULL)
             {
               locatedNode->set_endOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
               locatedNode->get_endOfConstruct  ()->set_parent(locatedNode);
             }

       // Only SgExpression IR nodes have a 3rd source position data structure.
          if (expression!=NULL)
             {
               expression->set_operatorPosition(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
               expression->get_operatorPosition()->set_parent(expression);
             }
        }
       else // special non-located node with file info
        {
//        if ( (initName != NULL) && (initName->get_startOfConstruct() == NULL) )
//         {
//           locatedNode->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
//           locatedNode->get_startOfConstruct()->set_parent(locatedNode);
//
//           locatedNode->set_endOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
//           locatedNode->get_endOfConstruct  ()->set_parent(locatedNode);
//
//         }
//         else

          if ( (pragma != NULL) && (pragma->get_startOfConstruct() == NULL) )
             {
               pragma->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
               pragma->get_startOfConstruct()->set_parent(pragma);
             }
        }
   }


void
SageInterface::setOneSourcePositionNull(SgNode *node)
   {
  // DQ (5/1/2012): Older depricated function (use setSourcePositionPointersToNull() instead).
#if 0
     printf ("+++++ Depricated name setOneSourcePositionNull() (use setSourcePositionPointersToNull() instead) (no using internal source position mode) \n");
#endif

     setSourcePosition(node);
   }


// DQ (5/1/2012): Newly renamed function (previous name preserved for backward compatability).
void
SageInterface::setSourcePositionPointersToNull(SgNode *node)
   {
  // DQ (1/24/2009): I think this should be renamed to be "setSourcePositionToNULL(SgNode *node)"
  // However, if this is doen then the logic should be that it asserts that: (locatedNode->get_startOfConstruct() == NULL)
  // so that we know when we are leaking memory.  Similarly, we should assert that:
  // (locatedNode->get_endOfConstruct() == NULL).
  // But then this function just asserts that everything is already NULL and is less about setting them to NULL.
  // If (locatedNode->get_startOfConstruct() != NULL), should we delete the existing Sg_File_Info object?
  // This function misses that step.

     ROSE_ASSERT(node != NULL);

     SgLocatedNode *    locatedNode = isSgLocatedNode(node);
     SgExpression*      expression  = isSgExpression(node);
     SgPragma*          pragma      = isSgPragma(node); // missed this one!! Liao, 1/30/2008
     SgGlobal*          global      = isSgGlobal(node); // SgGlobal should have NULL endOfConstruct()

  // DQ (1/24/2009): If the point is to set the source position to NULL pointers,
  // why do we only handle the case when (get_startOfConstruct() == NULL)
  // (i.e. when the start source postion is already NULL).

  // if ((locatedNode) && (locatedNode->get_endOfConstruct() == NULL))
  // if ( (locatedNode != NULL) && (locatedNode->get_startOfConstruct() == NULL) )
     if (locatedNode != NULL)
        {
          if (locatedNode->get_startOfConstruct() != NULL)
               printf ("WARNING: In SageInterface::setSourcePositionPointersToNull(): Memory leak of startOfConstruct Sg_File_Info object (setting Sg_File_Info pointers to NULL) \n");

          locatedNode->set_startOfConstruct(NULL);

       // Note that SgGlobal should have NULL endOfConstruct()
          if (global == NULL)
             {
               if (locatedNode->get_endOfConstruct() != NULL)
                    printf ("WARNING: In SageInterface::setSourcePositionPointersToNull(): Memory leak of endOfConstruct Sg_File_Info object (setting Sg_File_Info pointers to NULL) \n");

               locatedNode->set_endOfConstruct(NULL);
             }

       // Only SgExpression IR nodes have a 3rd source position data structure.
          if (expression != NULL)
             {
               if (expression->get_operatorPosition() != NULL)
                    printf ("WARNING: In SageInterface::setSourcePositionPointersToNull(): Memory leak of operatorPosition Sg_File_Info object (setting Sg_File_Info pointers to NULL) \n");

               expression->set_operatorPosition(NULL);
             }
        }
       else
        {
       // if ( (pragma != NULL) && (pragma->get_startOfConstruct() == NULL) )
          if ( (pragma != NULL) && (pragma->get_startOfConstruct() != NULL) )
             {
               printf ("WARNING: In SageInterface::setSourcePositionPointersToNull(): Memory leak of Sg_File_Info object (setting Sg_File_Info pointers to NULL) \n");

               pragma->set_startOfConstruct(NULL);
             }
        }
   }


// DQ (1/24/2009): Could we change the name to be "setSourcePositionAtRootAndAllChildrenAsTransformation(SgNode *root)"
void
SageInterface::setSourcePositionForTransformation(SgNode *root)
   {
#if 1
#if 0
     printf ("+++++ Depricated name setSourcePositionForTransformation() (use setSourcePositionAtRootAndAllChildrenAsTransformation() instead) \n");
#endif

  // This is the semantically correct function to call.
  // setSourcePositionAtRootAndAllChildrenAsTransformation(root);

  // DQ (5/2/2012): This is a test to replace the support we have to mark every thing as a transformation with the new mechanism using source position modes.
  // setSourcePosition(root);
  // Liao 11/21/2012. This function should only be called when the mode is transformation
  // Liao 8/2/2013. It can actually be called inside frontend by OmpSupport::lower_omp().
     //ROSE_ASSERT(SageBuilder::SourcePositionClassificationMode == SageBuilder::e_sourcePositionTransformation);
     setSourcePositionAtRootAndAllChildren(root);
#else
     Rose_STL_Container <SgNode*> nodeList = NodeQuery::querySubTree(root,V_SgNode);
     for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i!=nodeList.end(); i++ )
        {
          setOneSourcePositionForTransformation(*i);
        }
#endif
   }


#if 0
// DQ (5/1/2012): New function with improved name (still preserving the previous interface).
void
SageInterface::setSourcePositionAtRootAndAllChildrenAsTransformation(SgNode *root)
   {
     Rose_STL_Container <SgNode*> nodeList= NodeQuery::querySubTree(root,V_SgNode);
     for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i!=nodeList.end(); i++ )
        {
          setOneSourcePositionForTransformation(*i);
        }
   }
#endif

#if 0
void
SageInterface::setSourcePositionAtRootAndAllChildrenAsDefault(SgNode *root)
   {
     Rose_STL_Container <SgNode*> nodeList= NodeQuery::querySubTree(root,V_SgNode);
     for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i!=nodeList.end(); i++ )
        {
          setSourcePositionAsDefault(*i);
        }
   }
#endif

void
SageInterface::setSourcePositionAtRootAndAllChildren(SgNode *root)
   {
  // DQ (2/17/2013): This is a relatively expensive operation so we might look into this.

     Rose_STL_Container <SgNode*> nodeList = NodeQuery::querySubTree(root,V_SgNode);

#if 0
     printf ("In setSourcePositionAtRootAndAllChildren(): nodeList.size() = %zu \n",nodeList.size());
#endif

     for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
        {
#if 0
          printf ("In setSourcePositionAtRootAndAllChildren(): *i = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
          setSourcePosition(*i);
        }
   }

// DQ (5/1/2012): This function queries the SageBuilder::SourcePositionClassification mode (stored in the SageBuilder 
// interface) and used the specified mode to initialize the source position data (Sg_File_Info objects).  This 
// function is the only function that should be called directly (though in a namespace we can't define permissions).
void
SageInterface::setSourcePosition(SgNode* node)
   {
  // Check the mode and build the correct type of source code position.
     SourcePositionClassification scp = getSourcePositionClassificationMode();

  // DQ (2/17/2013): Note that the SourcePositionClassification will be e_sourcePositionFrontendConstruction
  // during construction of the AST from the EDG frontend.

#if 0
     printf ("In SageInterface::setSourcePosition(): SourcePositionClassification scp = %s \n",display(scp).c_str());
#endif

     switch(scp)
        {
          case e_sourcePositionError: // Error value for enum.
             {
               printf ("Error: error value e_sourcePositionError in SageInterface::setSourcePosition() \n");
               ROSE_ASSERT(false);
               break;
             }

          case e_sourcePositionDefault: // Default source position.
             {
#if 0
               printf ("e_sourcePositionDefault in SageInterface::setSourcePosition() \n");
#endif
               SgLocatedNode* locatedNode = isSgLocatedNode(node);
               if (locatedNode != NULL)
                  {
                    setSourcePositionToDefault(locatedNode);
                  }
                 else
                  {
                 // This is not supported (not clear if it need be).
                    printf ("Error: can't call setSourcePosition() in mode e_sourcePositionDefault with non SgLocatedNode (node = %p = %s) \n",node,node->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
               break;
             }

          case e_sourcePositionTransformation:       // Classify as a transformation.
             {
#if 0
               printf ("e_sourcePositionTransformation in SageInterface::setSourcePosition() \n");
#endif
            // setSourcePositionAtRootAndAllChildrenAsTransformation(node);
               setSourcePositionAsTransformation(node);
               break;
             }

          case e_sourcePositionCompilerGenerated:    // Classify as compiler generated code (e.g. template instantiation).
             {
               printf ("e_sourcePositionCompilerGenerated in SageInterface::setSourcePosition() \n");

               printf ("Sorry, not implemented \n");
               ROSE_ASSERT(false);

               break;
             }

          case e_sourcePositionNullPointers:         // Set pointers to Sg_File_Info objects to NULL.
             {
            // DQ (2/17/2013): We want to move to this mode as the one used for EDG/ROSE connection so that we can 
            // avoid building and rebuilding source position information.
#if 0
               printf ("e_sourcePositionNullPointers in SageInterface::setSourcePosition() \n");
#endif
               setSourcePositionPointersToNull(node);
               break;
             }

          case e_sourcePositionFrontendConstruction: // Specify as source position to be filled in as part of AST construction in the front-end.
             {
            // DQ (2/17/2013): The setSourcePositionToDefault() function is called a lot, so it might be a performance issue.
            // All IR nodes built by the Build Interface are assighed source position information
            // using this function and then reset afterward as we use information within EDG to 
            // reset the source position information.  Ideally, the EDG/ROSE connection would
            // use NULL pointers as the behavior for the front-end mode.  We can move to that 
            // later to make the source position handling more efficient.

            // This function builds an empty Sg_File_Info entry (valid object but filled with default values; must be reset in front-end processing).
#if 0
               printf ("e_sourcePositionFrontendConstruction in SageInterface::setSourcePosition() \n");
#endif
               SgLocatedNode* locatedNode = isSgLocatedNode(node);
               if (locatedNode != NULL)
                  {
                 // setSourcePositionAtRootAndAllChildrenAsDefault(locatedNode);
                    setSourcePositionToDefault(locatedNode);
                  }
                 else
                  {
                 // This is not supported (not clear if it need be).
                    SgPragma* pragma = isSgPragma(node);
                    if (pragma != NULL)
                       {
                         setSourcePositionToDefault(pragma);
                       }
                      else
                       {
                      // printf ("Error: can't call setSourcePosition() in mode e_sourcePositionFrontendConstruction with non SgLocatedNode (node = %p = %s) \n",node,node->class_name().c_str());
                      // ROSE_ASSERT(false);
                         SgType* type = isSgType(node);
                         if (type != NULL)
                            {
                           // Ignore this case, OK.
                            }
                           else
                            {
                              SgFunctionParameterTypeList* functionParameterTypeList = isSgFunctionParameterTypeList(node);
                              if (functionParameterTypeList != NULL)
                                 {
                                // Ignore this case, OK.
                                 }
                                else
                                 {
                                   printf ("Error: can't call setSourcePosition() in mode e_sourcePositionFrontendConstruction with non SgLocatedNode (node = %p = %s) \n",node,node->class_name().c_str());
                                   ROSE_ASSERT(false);
                                 }
                            }
                       }
                  }

               break;
             }


          case e_sourcePosition_last:
             {
               printf ("Error: error value e_sourcePositionError in SageInterface::setSourcePosition() \n");
               ROSE_ASSERT(false);
               break;
             }

          default:
             {
               printf ("Error: default reached in SageInterface::setSourcePosition() \n");
               break;
             }
        }

#if 0
     if (node->get_file_info() != NULL)
        {
          node->get_file_info()->display("Leaving SageInterface::setSourcePosition()");
        }
#endif
   }


void
SageInterface::setSourcePositionForTransformation_memoryPool()
   {
  // DQ (1/24/2009): This seems like a very dangerous function to have, is it required!
 
  // DQ (5/1/2012): Make it an error to call this function.
     printf ("ERROR: In setSourcePositionForTransformation_memoryPool(): This seems like a very dangerous function to have, is it required? \n");
     ROSE_ASSERT(false);

     VariantVector vv(V_SgNode);
     Rose_STL_Container<SgNode*> nodeList = NodeQuery::queryMemoryPool(vv);
     for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++ )
        {
          setOneSourcePositionForTransformation(*i);
        }
   }


SgGlobal * SageInterface::getFirstGlobalScope(SgProject *project)
   {
  // Liao, 1/9/2008, get the first global scope from current project
  // mostly used to prepare for AST construction under the global scope
     ROSE_ASSERT(project != NULL);

  // DQ (1/24/2009): Added a check to make sure STL list is non-empty.
     ROSE_ASSERT(project->get_fileList().empty() == false);

  // SgGlobal* global = project->get_file(0).get_root();
     SgSourceFile* file = isSgSourceFile(project->get_fileList()[0]);
     SgGlobal* global = file->get_globalScope();

#if 0
     SgFilePtrListPtr fileList = project->get_fileList();
     SgFilePtrList::const_iterator i= fileList->begin();

     SgGlobal* global = (*i)->get_globalScope();
#endif
     ROSE_ASSERT(global != NULL);

     return global;
   }

// Liao, 1/10/2008, get the last stmt from the scope
// two cases
//      SgScopeStatement::getDeclarationList
//      SgScopeStatement::getStatementList()
SgStatement* SageInterface::getLastStatement(SgScopeStatement *scope)
   {
     ROSE_ASSERT(scope);
     SgStatement* stmt = NULL;

     if (scope->containsOnlyDeclarations())
        {
       // DQ (2/17/2013): Fixed declList to be a reference instead of a copy.
       // SgDeclarationStatementPtrList declList = scope->getDeclarationList();
          SgDeclarationStatementPtrList & declList = scope->getDeclarationList();

       // DQ (2/17/2013): Fixed evaluation of empty list use SgDeclarationStatementPtrList::empty() member function for faster performance.
       // if (declList.size()>0)
          if (declList.empty() == false)
             {
               stmt = isSgStatement(declList.back());
             }
        }
       else
        {
       // DQ (2/17/2013): Fixed stmtList to be a reference instead of a copy.
          SgStatementPtrList & stmtList = scope->getStatementList();

       // DQ (2/17/2013): Fixed evaluation of empty list use SgDeclarationStatementPtrList::empty() member function for faster performance.
       // if (stmtList.size()>0)
          if (stmtList.empty() == false)
             {
               stmt = stmtList.back();
             }
        }

     return stmt;
   }


SgStatement* SageInterface::getFirstStatement(SgScopeStatement *scope, bool includingCompilerGenerated/*=false*/)
   {
  // DQ (2/17/2013): This function appears to be a performance problem (so a pass was made to 
  // use lower complexity operations that are equivalent in funcionality for this context).

     ROSE_ASSERT(scope);
     SgStatement* stmt = NULL;

     if (scope->containsOnlyDeclarations())
        {
       // DQ (2/17/2013): Fixed declList to be a reference instead of a copy.
       // DQ Note: Do we really need to make a copy of the list just to return a pointer to the first entry?
       // SgDeclarationStatementPtrList declList = scope->getDeclarationList();
          SgDeclarationStatementPtrList & declList = scope->getDeclarationList();
          if (includingCompilerGenerated)
             {
            // DQ (2/17/2013): Fixed evaluation of empty list use SgDeclarationStatementPtrList::empty() member function for faster performance.
            // DQ Note: (declList.empty() == false) is a much faster test  O(1) than (declList.size() > 0), which is O(n).
            // if (declList.size()>0)
               if (declList.empty() == false)
                  {
                    stmt = isSgStatement(declList.front());
                  }
             }
            else
             {
            // skip compiler-generated declarations
               SgDeclarationStatementPtrList::iterator i=declList.begin();
               while (i != declList.end())
                  {
                 // isCompilerGenerated(),isOutputInCodeGeneration(),etc. are not good enough,
                 // some content from headers included are not marked as compiler-generated
                 //
                 // cout<<(*i)->unparseToString()<<endl;
                 // ((*i)->get_file_info())->display("debug.......");
                    Sg_File_Info * fileInfo = (*i)->get_file_info();
                 // include transformation-generated  statements, but not the hidden ones
                 // Note: isOutputInCodeGeneration is not default to true for original statements from user code
                    if ((fileInfo->isSameFile(scope->get_file_info())) || (fileInfo->isTransformation() && fileInfo->isOutputInCodeGeneration()))
                       {
                         stmt = *i;
                         break;
                       }
                      else
                       {
                         i++;
                         continue;
                       }
                  }
             }
        }
       else
        {
       // DQ Note: Do we really need to make a copy of the list just to return a pointer to the first entry?
          SgStatementPtrList & stmtList = scope->getStatementList();
          if (includingCompilerGenerated)
             {
            // DQ (2/17/2013): Fixed evaluation of empty list use SgStatementPtrList::empty() member function for faster performance.
            // DQ Note: (stmtList.empty() == false) is a much faster test  O(1) than (stmtList.size() > 0), which is O(n).
            // if (stmtList.size()>0)
               if (stmtList.empty() == false)
                  {
                    stmt = stmtList.front();
                  }
             }
            else
             {
            // skip compiler-generated declarations
               SgStatementPtrList::iterator i = stmtList.begin();
               while (i!=stmtList.end())
                  {
                 // isCompilerGenerated(),isOutputInCodeGeneration(),etc. are not good enough,
                 // some content from headers included are not marked as compiler-generated
                 //
                 // cout<<(*i)->unparseToString()<<endl;
                 // ((*i)->get_file_info())->display("debug.......");
                    Sg_File_Info * fileInfo = (*i)->get_file_info();
                 // include transformation-generated  statements, but not the hidden ones
                 // Note: isOutputInCodeGeneration is not default to true for original statements from user code
                    if ( (fileInfo->isSameFile(scope->get_file_info())) || (fileInfo->isTransformation()&& fileInfo->isOutputInCodeGeneration()))
                       {
                         stmt=*i;
                         break;
                       }
                      else
                       {
                         i++;
                         continue;
                       }
                  }
             }
        }

     return stmt;
   }


  SgFunctionDeclaration* SageInterface::findFirstDefiningFunctionDecl(SgScopeStatement* scope)
  {
    ROSE_ASSERT(scope);
    SgFunctionDeclaration* result = NULL;
    if (scope->containsOnlyDeclarations())
    {
      SgDeclarationStatementPtrList declList = scope->getDeclarationList();
      SgDeclarationStatementPtrList::iterator i=declList.begin();
      while (i!=declList.end())
      {
        Sg_File_Info * fileInfo = (*i)->get_file_info();

       if ((fileInfo->isSameFile(scope->get_file_info()))||
          (fileInfo->isTransformation()&& fileInfo->isOutputInCodeGeneration())
        )
        {
          SgFunctionDeclaration* func = isSgFunctionDeclaration(*i);
          if (func)
          {
            if (func->get_definingDeclaration ()==func)
            {
            //cout<<"debug, first defining func decl is:"<<(*i)->unparseToString()<<endl;
            result=func;
            break;
            }
          }
        }
        i++;
      }//end while
    } else
    {
      SgStatementPtrList stmtList = scope->getStatementList();
      SgStatementPtrList::iterator i=stmtList.begin();
      while (i!=stmtList.end())
      {
        Sg_File_Info * fileInfo = (*i)->get_file_info();
        if ( (fileInfo->isSameFile(scope->get_file_info()))||
             (fileInfo->isTransformation()&& fileInfo->isOutputInCodeGeneration())
            )
         {
          SgFunctionDeclaration* func = isSgFunctionDeclaration(*i);
          if (func)
          {
            if (func->get_definingDeclaration ()==func)
            {
            //cout<<"debug, first defining func decl is:"<<(*i)->unparseToString()<<endl;
            result=func;
            break;
            }// if defining
          } // if func
         }// if fileInof
        i++;
      }//while
    } // end if
    return result;
  }


bool SageInterface::isMain(const SgNode* n)
{
 bool result = false;
 // Liao 1/5/2010, handle Fortran main entry: SgProgramHeaderStatement
 if (SageInterface::is_Fortran_language())
 {
   if (isSgProgramHeaderStatement(n))
     result = true;
 }
 else
 {
   if (isSgFunctionDeclaration(n) &&
       (SageInterface::is_Java_language() || isSgGlobal(isSgStatement(n)->get_scope())) &&
       isSgFunctionDeclaration(n)->get_name() == "main")
   result = true;
 }

   return result;
}

// Originally from ompTranslator.C
// DQ (1/6/2007): The correct qualified name for "main" is "::main", at least in C++.
// however for C is should be "main".  Our name qualification is not language specific,
// however, for C is makes no sense to as for the qualified name, so the name we
// want to search for could be language specific.  The test code test2007_07.C
// demonstrates that the function "main" can exist in both classes (as member functions)
// and in namespaces (as more meaningfully qualified names).  Because of this C++
// would have to qualify the global main function as "::main", I think.

// Revised by Jeremiah,
// Added check to see if the scope is global: Liao
SgFunctionDeclaration* SageInterface::findMain(SgNode* n) {
  if (!n) return 0;
  if (isMain(n))
  {
    return isSgFunctionDeclaration(n);
  }
  vector<SgNode*> children = n->get_traversalSuccessorContainer();
  for (vector<SgNode*>::const_iterator i = children.begin();
  i != children.end(); ++i) {

    SgFunctionDeclaration* mainDecl = findMain(*i);
    if (mainDecl)
      if (mainDecl->get_definingDeclaration() == mainDecl) // skip non-defining main() declaration, Liao 8/27/2010
      return mainDecl;
  }
  return 0;
}

//! iterate through the statement within a scope, find the last declaration statement (if any) after which
//  another declaration statement can be inserted.
// This is useful to find a safe place to insert a declaration statement with special requirements about where it can be inserted.
// e.g. a variable declaration statement should not be inserted before IMPLICIT none in Fortran
// If it returns NULL, a declaration statement should be able to be prepended to the scope
SgStatement* SageInterface::findLastDeclarationStatement(SgScopeStatement * scope)
{
  SgStatement* rt = NULL;
  ROSE_ASSERT (scope != NULL);

  SgStatementPtrList stmt_list = scope->generateStatementList ();

  for (size_t i = 0; i<stmt_list.size(); i++)
  {
    SgStatement* cur_stmt = stmt_list[i];
    if (isSgDeclarationStatement(cur_stmt))
      rt = cur_stmt;
    //if (isSgImplicitStatement(cur_stmt)) || isSgFortranIncludeLine(cur_stmt) || isSgDeclarationStatement
  }

  return rt;
}

SgNode * SageInterface::deepCopyNode (const SgNode* n)
{
  SgTreeCopy g_treeCopy; // should use a copy object each time of usage!
  return n ? n->copy (g_treeCopy) : 0;
}

// by Jeremiah
// Return bool for C++ code, and int for C code
SgType* SageInterface::getBoolType(SgNode* n) {
  bool isC = TransformationSupport::getSourceFile(n)->get_outputLanguage() == SgFile::e_C_output_language;
  if (isC) {
    return SgTypeInt::createType();
  } else {
    return SgTypeBool::createType();
  }
}

#if 1
// Change continue statements in a given block of code to gotos to a label
void SageInterface::changeContinuesToGotos(SgStatement* stmt, SgLabelStatement* label)
   {
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
     std::vector<SgContinueStmt*> continues = SageInterface::findContinueStmts(stmt);
     for (std::vector<SgContinueStmt*>::iterator i = continues.begin(); i != continues.end(); ++i)
        {
          SgGotoStatement* gotoStatement = SageBuilder::buildGotoStatement(label);
       // printf ("Building gotoStatement #1 = %p \n",gotoStatement);
#ifndef _MSC_VER
          LowLevelRewrite::replace(*i, make_unit_list( gotoStatement ) );
#else
          ROSE_ASSERT(false);
#endif
        }
#else
          printf ("Not supported in mode: ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT (LowLevelRewrite::replace() is unavailable)");
          ROSE_ASSERT(false);
#endif
   }

// Add a step statement to the end of a loop body
// Add a new label to the end of the loop, with the step statement after
// it; then change all continue statements in the old loop body into
// jumps to the label
//
// For example:
// while (a < 5) {if (a < -3) continue;} (adding "a++" to end) becomes
// while (a < 5) {if (a < -3) goto label; label: a++;}
void SageInterface::addStepToLoopBody(SgScopeStatement* loopStmt, SgStatement* step) {
  using namespace SageBuilder;
  SgScopeStatement* proc = SageInterface::getEnclosingProcedure(loopStmt);
  SgStatement* old_body = SageInterface::getLoopBody(loopStmt);
  SgBasicBlock* new_body = buildBasicBlock();
// printf ("Building IR node #13: new SgBasicBlock = %p \n",new_body);
  SgName labelname = "rose_label__";
  labelname << ++gensym_counter;
  SgLabelStatement* labelstmt = buildLabelStatement(labelname,
buildBasicBlock(), proc);
  changeContinuesToGotos(old_body, labelstmt);
  appendStatement(old_body, new_body);
  appendStatement(labelstmt, new_body);
  appendStatement(step, new_body);
  SageInterface::setLoopBody(loopStmt, new_body);
}


void SageInterface::moveForStatementIncrementIntoBody(SgForStatement* f) {
  if (isSgNullExpression(f->get_increment())) return;
  SgExprStatement* incrStmt = SageBuilder::buildExprStatement(f->get_increment());
  f->get_increment()->set_parent(incrStmt);
  SageInterface::addStepToLoopBody(f, incrStmt);
  SgNullExpression* ne = SageBuilder::buildNullExpression();
  f->set_increment(ne);
  ne->set_parent(f);
}

void SageInterface::convertForToWhile(SgForStatement* f) {
  moveForStatementIncrementIntoBody(f);
  SgBasicBlock* bb = SageBuilder::buildBasicBlock();
  SgForInitStatement* inits = f->get_for_init_stmt();
  SgStatementPtrList& bbStmts = bb->get_statements();
  SgStatementPtrList& initStmts = inits->get_init_stmt();
  bbStmts = initStmts;
  for (size_t i = 0; i < bbStmts.size(); ++i) {
    bbStmts[i]->set_parent(bb);
  }
  bool testIsNull =
    isSgExprStatement(f->get_test()) &&
    isSgNullExpression(isSgExprStatement(f->get_test())->get_expression());
  SgStatement* test =
    testIsNull ?
    SageBuilder::buildExprStatement(
        SageBuilder::buildBoolValExp(true)) :
    f->get_test();
  SgWhileStmt* ws = SageBuilder::buildWhileStmt(
      test,
      f->get_loop_body());
  appendStatement(ws, bb);
  isSgStatement(f->get_parent())->replace_statement(f, bb);
}

void SageInterface::convertAllForsToWhiles(SgNode* top) {
  Rose_STL_Container<SgNode*> fors = NodeQuery::querySubTree(top,
V_SgForStatement);
  for (size_t i = 0; i < fors.size(); ++i) {
    convertForToWhile(isSgForStatement(fors[i]));
  }
}

  vector<SgGotoStatement*> SageInterface::findGotoStmts(SgStatement* scope, SgLabelStatement* l) {
 // DQ (9/25/2007): Moved from std::list to std::vector uniformally in ROSE.
 // But we still need the copy since the return type is IR node specific.
    Rose_STL_Container<SgNode*> allGotos = NodeQuery::querySubTree(scope, V_SgGotoStatement);

    vector<SgGotoStatement*> result;
    for (Rose_STL_Container<SgNode*>::const_iterator i = allGotos.begin(); i != allGotos.end(); ++i) {
      if (isSgGotoStatement(*i)->get_label() == l) {
        result.push_back(isSgGotoStatement(*i));
      }
    }
    return result;
  }

#if 0 // Liao 5/21/2010. This is a bad function in terms of performance
      // vectors are created/destroyed multiple times
  vector<SgReturnStmt*> SageInterface::findReturnStmts(SgStatement* scope) {
 // DQ (9/25/2007): Moved from std::list to std::vector uniformally in ROSE.
 // But we still need the copy since the return type is IR node specific.
    Rose_STL_Container<SgNode*> returns = NodeQuery::querySubTree(scope, V_SgReturnStmt);

    vector<SgReturnStmt*> result;
    for (Rose_STL_Container<SgNode*>::const_iterator i = returns.begin(); i != returns.end(); ++i) {
      result.push_back(isSgReturnStmt(*i));
    }
    return result;
  }

#endif
static  void getSwitchCasesHelper(SgStatement* top, vector<SgStatement*>& result) {
    ROSE_ASSERT (top);
    if (isSgSwitchStatement(top)) return; // Don't descend into nested switches
    if (isSgCaseOptionStmt(top) || isSgDefaultOptionStmt(top)) {
      result.push_back(top);
    }
    vector<SgNode*> children = top->get_traversalSuccessorContainer();
    for (unsigned int i = 0; i < children.size(); ++i) {
      if (isSgStatement(children[i])) {
        getSwitchCasesHelper(isSgStatement(children[i]), result);
      }
    }
  }

  vector<SgStatement*> SageInterface::getSwitchCases(SgSwitchStatement* sw) {
    vector<SgStatement*> result;
    getSwitchCasesHelper(sw->get_body(), result);
    return result;
  }


#endif

AbstractHandle::abstract_handle* SageInterface::buildAbstractHandle(SgNode* n)
{
  return AbstractHandle::buildAbstractHandle(n);
  
}
//! Get the enclosing scope from a node n 
SgScopeStatement* SageInterface::getEnclosingScope(SgNode* n, const bool includingSelf/* =false*/)
{
  SgScopeStatement* result = getScope (n);
  if (result == n)
  {
    if (!includingSelf)
      result = getScope(n->get_parent());
  }
  return result;
}

// from transformationSupport.C
SgScopeStatement*
SageInterface::getScope( const SgNode* astNode )
   {

   // Case 1: directly call get_scope() for some types of nodes  
    if (const SgSymbol* symbol = isSgSymbol(astNode))
        return symbol->get_scope();
    else if (const SgInitializedName* initName = isSgInitializedName(astNode))
        return initName->get_scope();
    else if (const SgTemplateArgument* tempArg = isSgTemplateArgument(astNode))
        return tempArg->get_scope();
    else if (const SgQualifiedName* qualifiedName = isSgQualifiedName(astNode))
        return qualifiedName->get_scope();

    // DQ (6/9/2007): This function traverses through the parents to the first scope (used for name qualification support of template arguments)
    const SgNode* parentNode = astNode;
    while (!isSgScopeStatement(parentNode))
    {
        //George Vulov (11/29/2010)
        //Function parameter lists are siblings of SgFunctionDefinition, so just going up to parents
        //produces SgGlobal.
        if (isSgFunctionParameterList(parentNode) || isSgCtorInitializerList(parentNode))
        {
            const SgFunctionDeclaration* funcDeclaration = isSgFunctionDeclaration(parentNode->get_parent());
            ROSE_ASSERT(funcDeclaration != NULL);
            funcDeclaration = isSgFunctionDeclaration(funcDeclaration->get_definingDeclaration());
            if (funcDeclaration != NULL)
            {
                return funcDeclaration->get_definition();
            }
        }
  
        parentNode = parentNode->get_parent();
        if (parentNode == NULL)
        {
            break;
        }
    }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgScopeStatement(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
          printf ("Error: could not trace back to SgScopeStatement node \n");
          ROSE_ABORT();
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgStatement from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }

  // Make sure we have a SgStatement node
     const SgScopeStatement* scopeStatement = isSgScopeStatement(parentNode);
     //SgScopeStatement* scopeStatement = isSgScopeStatement(parentNode);
     ROSE_ASSERT (scopeStatement != NULL);

     // ensure the search is inclusive
     if (isSgScopeStatement(astNode))
       if (isSgScopeStatement(parentNode))
       {
          ROSE_ASSERT (astNode == parentNode);
       }

   // return scopeStatement;
       return const_cast<SgScopeStatement*>(scopeStatement);
   }


// from outliner, ASTtools
// ========================================================================

/*!
 *  \brief Return an existing variable symbol for the given
 *  initialized name.
 *
 *  This routine checks various scopes in trying to find a suitable
 *  variable symbol for the given initialized name.
 */
static
const SgVariableSymbol *
getVarSymFromName_const (const SgInitializedName* name)
   {
     SgVariableSymbol* v_sym = NULL;

     if (name != NULL)
        {
#if 0
          printf ("In getVarSymFromName(): name->get_name() = %s \n",name->get_name().str());
#endif
          SgScopeStatement* scope = name->get_scope();
          ROSE_ASSERT (scope != NULL);
#if 0
          printf ("In getVarSymFromName(): name->get_name() = %s scope = %p = %s \n",name->get_name().str(),scope,scope->class_name().c_str());
#endif
          v_sym = scope->lookup_var_symbol (name->get_name());

          if (!v_sym) // E.g., might be part of an 'extern' declaration.
            {
           // Try the declaration's scope.
              SgDeclarationStatement* decl = name->get_declaration ();
              ROSE_ASSERT (decl);

              SgScopeStatement* decl_scope = decl->get_scope ();
              if (decl_scope != NULL)
                   v_sym = decl_scope->lookup_var_symbol (name->get_name());

              if (!v_sym)
                   cerr << "\t\t*** WARNING: Can't seem to find a symbol for '"
                        << name->get_name ().str ()
                        << "' ***" << endl;
             }
        }

     return v_sym;
   }

#if 0
/*!
 *  \brief Return an existing variable symbol for the given
 *  initialized name.
 *
 *  This routine checks various scopes in trying to find a suitable
 *  variable symbol for the given initialized name.
 */
static
SgVariableSymbol *
getVarSymFromName (SgInitializedName* name)
{
  const SgVariableSymbol* v_sym = getVarSymFromName_const (name);
  return const_cast<SgVariableSymbol *> (v_sym);
}
#endif
/*!
 *  \brief Returns the SgVariableSymbol associated with an SgVarRefExp
 *  or SgInitializedName, or 0 if none.
 */
static
const SgVariableSymbol *
getVarSym_const (const SgNode* n)
{
  const SgVariableSymbol* v_sym = 0;
  switch (n->variantT ())
    {
    case V_SgVarRefExp:
      v_sym = isSgVarRefExp (n)->get_symbol ();
      break;
    case V_SgInitializedName:
      v_sym = getVarSymFromName_const (isSgInitializedName (n));
      break;
    default:
      break;
    }
  return v_sym;
}

#if 0
/*!
 *  \brief Returns the SgVariableSymbol associated with an SgVarRefExp
 *  or SgInitializedName, or 0 if none.
 */
static
SgVariableSymbol *
getVarSym (SgNode* n)
{
  const SgVariableSymbol* v_sym = getVarSym_const (n);
  return const_cast<SgVariableSymbol *> (v_sym);
}
#endif

static
const SgVariableSymbol *
getFirstVarSym_const (const SgVariableDeclaration* decl)
{
  if (!decl) return 0;
  const SgInitializedNamePtrList& names = decl->get_variables ();
  if (names.begin () != names.end ())
    {
      const SgInitializedName* name = *(names.begin ());
      return getVarSym_const (name);
    }
  else
    return 0;
}

SgVariableSymbol *
SageInterface::getFirstVarSym (SgVariableDeclaration* decl)
{
  const SgVariableSymbol* sym = getFirstVarSym_const (decl);
  return const_cast<SgVariableSymbol *> (sym);
}


SgInitializedName*
SageInterface::getFirstInitializedName (SgVariableDeclaration* decl)
{
  ROSE_ASSERT(decl);
  SgInitializedNamePtrList& names = decl->get_variables ();
  if (names.begin () != names.end ())
    return *(names.begin ());
  else
    return NULL;
}


static void findBreakStmtsHelper(SgStatement* code, const std::string& fortranLabel, bool inOutermostBody, vector<SgBreakStmt*>& breakStmts) {
  if (isSgWhileStmt(code) || isSgDoWhileStmt(code) || isSgForStatement(code) || isSgSwitchStatement(code)) {
    if (fortranLabel == "") {
      // Outermost loop only
      return;
    } else {
      // Set this for query on children
      inOutermostBody = false;
    }
  }
  if (isSgBreakStmt(code)) {
    SgBreakStmt* bs = isSgBreakStmt(code);
    bool breakMatchesThisConstruct = false;
    if (bs->get_do_string_label() == "") {
      // Break matches closest construct
      breakMatchesThisConstruct = inOutermostBody;
    } else {
      breakMatchesThisConstruct = (fortranLabel == bs->get_do_string_label());
    }
    if (breakMatchesThisConstruct) {
      breakStmts.push_back(bs);
    }
    return;
  }
  vector<SgNode*> children = code->get_traversalSuccessorContainer();
  for (unsigned int i = 0; i < children.size(); ++i) {
    if (isSgStatement(children[i])) {
      findBreakStmtsHelper(isSgStatement(children[i]), fortranLabel, inOutermostBody, breakStmts);
    }
  }
}

vector<SgBreakStmt*> SageInterface::findBreakStmts(SgStatement* code, const std::string& fortranLabel) {
  // Run this on the body of a loop or switch, because it stops at any
  // construct which defines a new scope for break statements
  vector<SgBreakStmt*> result;
  findBreakStmtsHelper(code, fortranLabel, true, result);
  return result;
}


static void findContinueStmtsHelper(SgStatement* code, const std::string& fortranLabel, bool inOutermostBody, vector<SgContinueStmt*>& continueStmts) {
  if (isSgWhileStmt(code) || isSgDoWhileStmt(code) || isSgForStatement(code)) {
    if (fortranLabel == "") {
      // Outermost loop only
      return;
    } else {
      // Set this for query on children
      inOutermostBody = false;
    }
  }
  if (isSgContinueStmt(code)) {
    SgContinueStmt* cs = isSgContinueStmt(code);
    bool continueMatchesThisConstruct = false;
    if (cs->get_do_string_label() == "") {
      // Continue matches closest construct
      continueMatchesThisConstruct = inOutermostBody;
    } else {
      continueMatchesThisConstruct = (fortranLabel == cs->get_do_string_label());
    }
    if (continueMatchesThisConstruct) {
      continueStmts.push_back(cs);
    }
    return;
  }
  vector<SgNode*> children = code->get_traversalSuccessorContainer();
  for (unsigned int i = 0; i < children.size(); ++i) {
    if (isSgStatement(children[i])) {
      findContinueStmtsHelper(isSgStatement(children[i]), fortranLabel, inOutermostBody, continueStmts);
    }
  }
}

vector<SgContinueStmt*> SageInterface::findContinueStmts(SgStatement* code, const std::string& fortranLabel) {
  // Run this on the body of a loop, because it stops at any construct which
  // defines a new scope for continue statements
  vector<SgContinueStmt*> result;
  findContinueStmtsHelper(code, fortranLabel, true, result);
  return result;
}


// Get the initializer containing an expression if it is within an
// initializer.
//  from replaceExpressionWithStatement.C
SgInitializer* SageInterface::getInitializerOfExpression(SgExpression* n) {
  assert(n);
#if 0
  std::cout << "Starting getInitializerOfExpression on 0x" << std::hex << (int)n << ", which has type " << n->sage_class_name() << std::endl;
#endif
  while (!isSgInitializer(n)) {
    n = isSgExpression(n->get_parent());
#if 0
    std::cout << "Continuing getInitializerOfExpression on 0x" << std::hex << (int)n;
    if (n) std::cout << ", which has type " << n->sage_class_name();
    std::cout << std::endl;
#endif
    assert(n);
  }
  return isSgInitializer(n);
}

#ifndef USE_ROSE
// Get all symbols used in a given expression
vector<SgVariableSymbol*> SageInterface::getSymbolsUsedInExpression(SgExpression* expr) {
 class GetSymbolsUsedInExpressionVisitor: public AstSimpleProcessing {
  public:
  std::vector<SgVariableSymbol*> symbols;

  virtual void visit(SgNode* n) {
    if (isSgVarRefExp(n))
      symbols.push_back(isSgVarRefExp(n)->get_symbol());
  }
 };

  GetSymbolsUsedInExpressionVisitor vis;
  vis.traverse(expr, preorder);
  return vis.symbols;
}
#endif

SgSourceFile* SageInterface::getEnclosingSourceFile(SgNode* n,bool includingSelf)
{
    SgSourceFile* temp = getEnclosingNode<SgSourceFile>(n,includingSelf);
  if (temp)
    return temp;
  else
    return NULL;
}


SgFunctionDeclaration* SageInterface::findFunctionDeclaration(SgNode* root, std::string name, SgScopeStatement* scope, bool isDefining)
{
  return findDeclarationStatement<SgFunctionDeclaration> (root, name, scope, isDefining);

}


SgFunctionDefinition* SageInterface::getEnclosingProcedure(SgNode* n, bool includingSelf)
{
  return getEnclosingFunctionDefinition(n,includingSelf);
}

SgFunctionDefinition* SageInterface::getEnclosingFunctionDefinition(SgNode* n,bool includingSelf)
{
    SgFunctionDefinition* temp = getEnclosingNode<SgFunctionDefinition>(n,includingSelf);
  if (temp)
    return temp;
  else
    return NULL;
}


SgFunctionDeclaration *
SageInterface::getEnclosingFunctionDeclaration (SgNode * astNode,bool includingSelf)
{
  SgNode* temp = getEnclosingNode<SgFunctionDeclaration>(astNode,includingSelf);
  if (temp)
    return isSgFunctionDeclaration(temp);
  else
    return NULL;
#if 0
  SgNode *astnode = astNode;
  ROSE_ASSERT (astNode != NULL);
  do
    {
      astnode = astnode->get_parent ();
    }
  while ((astnode != NULL) &&
         (isSgFunctionDeclaration (astnode) == NULL) &&
         (isSgMemberFunctionDeclaration (astnode) == NULL));
  if (astnode==NULL) return NULL;
  else return isSgFunctionDeclaration(astnode);
#endif
}

// #endif

// #ifndef USE_ROSE

SgGlobal*
SageInterface::getGlobalScope( const SgNode* astNode )
   {
  // should including itself in this case
     SgNode* temp = getEnclosingNode<SgGlobal>(astNode,true);
     if (temp)
          return isSgGlobal(temp);
       else
          return NULL;
  }

SgClassDefinition*
SageInterface::getEnclosingClassDefinition(SgNode* astNode, const bool includingSelf/* =false*/)
  {
    SgNode* temp = getEnclosingNode<SgClassDefinition>(astNode,includingSelf);
    if (temp)
      return isSgClassDefinition(temp);
    else
      return NULL;
 }


SgFile * SageInterface::getEnclosingFileNode(SgNode* astNode)
   {
  // DQ (3/4/2014): This new version of this function supports both C/C++ and also Java.
  // If the SgJavaPackageDeclaration is noticed then the previous parent is a 
  // SgClassDefinition and the previous previous parent is a SgClassDeclaration whose
  // name can be used to match the filename in the SgProject's list of files.
  // A better implementation usign an attribute (not in place until tomorrow) and
  // from the attribute the pointer to the associated file is directly available.
  // The later implementation is as fast as possible.

     ROSE_ASSERT (astNode != NULL);

  // Make sure this is not a project node (since the SgFile exists below
  // the project and could not be found by a traversal of the parent list)
     ROSE_ASSERT (isSgProject(astNode) == NULL);

     SgNode* previous_parent = NULL;
     SgNode* previous_previous_parent = NULL;

     SgNode* parent = astNode;
  // while ( (parent != NULL) && (isSgFile(parent) == NULL) )
     while ( (parent != NULL) && (isSgFile(parent) == NULL) && isSgJavaPackageDeclaration(parent) == NULL)
        {
#if 0
          printf ("In getEnclosingFileNode(): parent = %p = %s \n",parent,parent->class_name().c_str());
#endif
          previous_previous_parent = previous_parent;
          previous_parent = parent;

          parent = parent->get_parent();
        }

     if (previous_previous_parent != NULL && previous_parent != NULL && isSgJavaPackageDeclaration(parent) != NULL)
        {
       // This is for a Java program and is contained within a SgJavaPackageDeclaration
#if 0
          printf ("parent                   = %p = %s \n",parent,parent->class_name().c_str());
          printf ("previous_parent          = %p = %s \n",previous_parent,previous_parent->class_name().c_str());
          printf ("previous_previous_parent = %p = %s \n",previous_previous_parent,previous_previous_parent->class_name().c_str());
#endif
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(previous_previous_parent);
          if (classDeclaration != NULL)
             {
#if 0
               printf ("Class name = %p = %s = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
#endif
            // Find the associated Java class file.
#if 0
            // DQ (3/4/2014): This is the code we want to use until we get Philippe's branch in place with the attribute.
               SgProject* project = TransformationSupport::getProject(parent);
               ROSE_ASSERT(project != NULL);
               SgFileList* fileList = project->get_fileList_ptr();
               ROSE_ASSERT(fileList != NULL);
               SgFilePtrList & vectorFile = fileList->get_listOfFiles();
#if 0
               printf ("Output list of files: \n");
#endif
               SgFilePtrList::iterator i = vectorFile.begin();
               while (i != vectorFile.end())
                  {
                    SgFile* file = *i;
                    ROSE_ASSERT(file != NULL);
#if 0
                    printf ("   --- filename = %s \n",file->getFileName().c_str());
#endif
                    string filename            = file->getFileName();
                    string filenameWithoutPath = file->get_sourceFileNameWithoutPath();
                    string classname           = classDeclaration->get_name();
                    string matchingfilename    = classname + ".java";
#if 0
                    printf ("   ---   --- filename            = %s \n",filename.c_str());
                    printf ("   ---   --- filenameWithoutPath = %s \n",filenameWithoutPath.c_str());
                    printf ("   ---   --- classname           = %s \n",classname.c_str());
                    printf ("   ---   --- matchingfilename    = %s \n",matchingfilename.c_str());
#endif
                    if (filenameWithoutPath == matchingfilename)
                       {
#if 0
                         printf ("   return file = %p \n",file);
#endif
                         return file;
                       }

                    i++;
                  }
#else
            // DQ (3/4/2014): This is the code we want to use when the attribute is in place (philippe's branch).
               AstSgNodeAttribute *attribute = (AstSgNodeAttribute *) classDeclaration->getAttribute("sourcefile");

               // "This simpler and more efficent code requires the latest work in Java support (3/6/2014)"

               if (attribute) 
                  {
                 // true for all user-specified classes and false for all classes fom libraries
                    SgSourceFile *sourcefile = isSgSourceFile(attribute->getNode());
                    ROSE_ASSERT(sourcefile != NULL);
                    return sourcefile;
                  }
#endif
             }
        }
       else
        {
       // previous_parent was uninitialized to a non-null value or astNode is a SgJavaPackageDeclaration or SgFile.
          if (previous_parent == NULL && isSgJavaPackageDeclaration(parent) != NULL)
             {
            // The input was a SgJavaPackageDeclaration (so there is no associated SgFile).
               ROSE_ASSERT(isSgJavaPackageDeclaration(astNode) != NULL);
               return NULL;
             }
            else
             {
               if (previous_previous_parent == NULL && isSgJavaPackageDeclaration(parent) != NULL)
                  {
                 // The input was a SgClassDefinition (so there is no associated SgFile).
                    ROSE_ASSERT(isSgClassDefinition(astNode) != NULL);
                    return NULL;
                  }
                 else
                  {
                 // This could be a C/C++ file (handled below).
                  }
             }
        }

  // This is where we handle the C/C++ files.
  // if (!parent)
     if (parent == NULL)
        {
          return NULL;
        }
       else
        {
          return isSgFile(parent);
        }
   }

SgStatement* SageInterface::getEnclosingStatement(SgNode* n) {
  while (n && !isSgStatement(n)) n = n->get_parent();
  return isSgStatement(n);
}

// DQ (/20/2010): Control debugging output for SageInterface::removeStatement() function.
#define REMOVE_STATEMENT_DEBUG 0

//! Remove a statement: TODO consider side effects for symbol tables
void SageInterface::removeStatement(SgStatement* targetStmt, bool autoRelocatePreprocessingInfo /*= true*/)
   {
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
  // This function removes the input statement.
  // If there are comments and/or CPP directives then those comments and/or CPP directives will
  // be moved to a new SgStatement.  The new SgStatement is selected using the findSurroundingStatementFromSameFile()
  // function and if there is not statement found then the SgGlobal IR node will be selected.
  // this work is tested by the tests/nonsmoke/functional/roseTests/astInterfaceTests/removeStatementCommentRelocation.C
  // translator and a number of input codes that represent a range of contexts which exercise different
  // cases in the code below.

#ifndef _MSC_VER
  // This function only supports the removal of a whole statement (not an expression within a statement)
     ROSE_ASSERT (targetStmt != NULL);

     SgStatement * parentStatement = isSgStatement(targetStmt->get_parent());

  // Can't assert this since SgFile is the parent of SgGlobal, and SgFile is not a statement.
  // Even so SgGlobal can't be removed from SgFile, but isRemovableStatement() takes a SgStatement.
  // ROSE_ASSERT (parentStatement != NULL);

     bool isRemovable = (parentStatement != NULL) ? LowLevelRewrite::isRemovableStatement(targetStmt) : false;

#if REMOVE_STATEMENT_DEBUG
     printf ("In parentStatement = %s remove targetStatement = %s (isRemovable = %s) \n",parentStatement->class_name().c_str(),targetStmt->class_name().c_str(),isRemovable ? "true" : "false");
#endif

     if (isRemovable == true)
        {
       // DQ (9/19/2010): Disable this new (not completely working feature) so that I can checkin the latest UPC/UPC++ work.
#if 1
       // DQ (9/16/2010): Added support to move comments and CPP directives marked to
       // appear before the statment to be attached to the inserted statement (and marked
       // to appear before that statement).
          AttachedPreprocessingInfoType* comments = targetStmt->getAttachedPreprocessingInfo();

       // DQ (9/17/2010): Trying to eliminate failing case in OpenMP projects/OpenMP_Translator/tests/npb2.3-omp-c/LU/lu.c
       // I think that special rules apply to inserting a SgBasicBlock so disable comment reloation when inserting a SgBasicBlock.
       // if (comments != NULL && isRemovable == true && isSgBasicBlock(targetStmt) == NULL )
       // Liao 10/28/2010. Sometimes we want remove the statement with all its preprocessing information
         if (autoRelocatePreprocessingInfo)
         {
          if (comments != NULL && isSgBasicBlock(targetStmt) == NULL )
             {
               vector<int> captureList;
#if REMOVE_STATEMENT_DEBUG
               printf ("Found attached comments (removing %p = %s): comments->size() = %zu \n",targetStmt,targetStmt->class_name().c_str(),comments->size());
#endif

             // Liao 10/28/2010. relinking AST statements may be achieved by remove it and attach it to somewhere else.
             // In this case, preprocessing information sometimes should go with the statements and not be relocated to the original places.
            // Dan: Since this statement will be removed we have to relocate all the associated comments and CPP directives.
               int commentIndex = 0;
               AttachedPreprocessingInfoType::iterator i;
               for (i = comments->begin(); i != comments->end(); i++)
                  {
                    ROSE_ASSERT ( (*i) != NULL );
#if REMOVE_STATEMENT_DEBUG
                    printf ("          Attached Comment (relativePosition=%s): %s\n",
                         ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                         (*i)->getString().c_str());
                    printf ("Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
                    (*i)->get_file_info()->display("comment/directive location debug");
#endif
                    captureList.push_back(commentIndex);
                    commentIndex++;
                  }

#if REMOVE_STATEMENT_DEBUG
               printf ("captureList.size() = %zu \n",captureList.size());
#endif

               if (captureList.empty() == false)
                  {
                 // Remove these comments and/or CPP directives and put them into the previous statement (marked to be output after the statement).
                 // Find the surrounding statement by first looking up in the sequence of statements in this block, then down, we need another
                 // statement from the same file.
                    bool surroundingStatementPreceedsTargetStatement = false;
                    SgStatement* surroundingStatement = findSurroundingStatementFromSameFile(targetStmt,surroundingStatementPreceedsTargetStatement);

                 // ROSE_ASSERT(surroundingStatement != NULL);
                    if (surroundingStatement != NULL)
                       {
                      // If we have identified a valid surrounding statemen, then move the comments and CPP directives to that statement.
#if REMOVE_STATEMENT_DEBUG
                         printf ("In removeStatement(): surroundingStatementPreceedsTargetStatement = %s \n",surroundingStatementPreceedsTargetStatement ? "true" : "false");
#endif
                         moveCommentsToNewStatement(targetStmt,captureList,surroundingStatement,surroundingStatementPreceedsTargetStatement);
                       }
                  }
             } // end if (comments)
        }// end if (autoRelocatePreprocessingInfo)
#endif  // end #if 1

          parentStatement->remove_statement(targetStmt);
        }
#else
     printf ("Error: This is not supported within Microsoft Windows (I forget why). \n");
     ROSE_ASSERT(false);
#endif

#endif
   }

//! Relocate comments and CPP directives from one statement to another.
void
SageInterface::moveCommentsToNewStatement(SgStatement* sourceStatement, const vector<int> & indexList, SgStatement* targetStatement , bool surroundingStatementPreceedsTargetStatement)
   {
     AttachedPreprocessingInfoType* comments = sourceStatement->getAttachedPreprocessingInfo();

#if REMOVE_STATEMENT_DEBUG
     printf ("In moveCommentsToNewStatement(): surroundingStatementPreceedsTargetStatement = %s \n",surroundingStatementPreceedsTargetStatement ? "true" : "false");
#endif

  // Now add the entries from the captureList to the surroundingStatement and remove them from the targetStmt.
  // printf ("This is a valid surrounding statement = %s for insertBefore = %s \n",surroundingStatement->class_name().c_str(),insertBefore ? "true" : "false");
     vector<int>::const_iterator j = indexList.begin();
     while (j != indexList.end())
        {
       // Add the captured comments to the new statement. Likely we need to make sure that the order is preserved.
          ROSE_ASSERT(targetStatement->get_file_info() != NULL);
#if REMOVE_STATEMENT_DEBUG
          printf ("Attaching comments to targetStatement = %p = %s on file = %s line %d \n",
               targetStatement,targetStatement->class_name().c_str(),
               targetStatement->get_file_info()->get_filenameString().c_str(),
               targetStatement->get_file_info()->get_line());

          printf ("(*comments)[*j]->getRelativePosition() = %s \n",PreprocessingInfo::relativePositionName((*comments)[*j]->getRelativePosition()).c_str());
#endif

          if (surroundingStatementPreceedsTargetStatement == true)
             {
               if ((*comments)[*j]->getRelativePosition() == PreprocessingInfo::before)
                  {
                    (*comments)[*j]->setRelativePosition(PreprocessingInfo::after);
                    ROSE_ASSERT((*comments)[*j]->getRelativePosition() == PreprocessingInfo::after);
                  }
                 else
                  {
                 // If is is not before, I hope it can only be after.
                    ROSE_ASSERT((*comments)[*j]->getRelativePosition() == PreprocessingInfo::after);
                  }
             }
            else
             {
               if ((*comments)[*j]->getRelativePosition() == PreprocessingInfo::before)
                  {
                 // Leave the comments marked as being before the associated statement.
                  }
                 else
                  {
                 // If is is not before, I hope it can only be after.
                    ROSE_ASSERT((*comments)[*j]->getRelativePosition() == PreprocessingInfo::after);
                    (*comments)[*j]->setRelativePosition(PreprocessingInfo::before);
                    ROSE_ASSERT((*comments)[*j]->getRelativePosition() == PreprocessingInfo::before);
                  }

            // printf (" This case (surroundingStatementPreceedsTargetStatement == false) is not handled yet. \n");
            // ROSE_ASSERT(false);
             }

          targetStatement->addToAttachedPreprocessingInfo((*comments)[*j]);

       // Remove them from the targetStmt. (set them to NULL and then remove them in a separate step).
#if REMOVE_STATEMENT_DEBUG
          printf ("Marking entry from comments list as NULL on sourceStatement = %p = %s \n",sourceStatement,sourceStatement->class_name().c_str());
#endif
          (*comments)[*j] = NULL;

          j++;
        }

  // Now remove each NULL entries in the comments vector.
  // Because of iterator invalidation we must reset the iterators after each call to erase (I think).
     for (size_t n = 0; n < indexList.size(); n++)
        {
#if REMOVE_STATEMENT_DEBUG
          printf ("Erase entry from comments list on comments->size() %zu \n",comments->size());
#endif
          bool modifiedList = false;
          AttachedPreprocessingInfoType::iterator k = comments->begin();
          while (k != comments->end() && modifiedList == false)
             {
            // Only modify the list once per iteration over the captureList
            // if ((*comments)[*k] == NULL)
                if (*k == NULL)
                {
                    k = comments->erase(k);
                    modifiedList = true;
                    continue;
                }
                else
                {
                    k++;
                }
             }
        }
   }


//! Remove a statement: TODO consider side effects for symbol tables
SgStatement*
SageInterface::findSurroundingStatementFromSameFile(SgStatement* targetStmt, bool & surroundingStatementPreceedsTargetStatement)
   {
  // Note that if the return value is SgGlobal (global scope), then surroundingStatementPreceedsTargetStatement is false, but meaningless.
  // This function can not return a NULL pointer.

     ROSE_ASSERT(targetStmt != NULL);

     SgStatement* surroundingStatement = targetStmt;
     int surroundingStatement_fileId   = Sg_File_Info::BAD_FILE_ID; // No file id can have this value.

#if REMOVE_STATEMENT_DEBUG
     printf ("TOP of findSurroundingStatementFromSameFile(): surroundingStatementPreceedsTargetStatement = %s \n",surroundingStatementPreceedsTargetStatement ? "true" : "false");
#endif

  // Only handle relocation for statements that exist in the file (at least for now while debugging).
     if (targetStmt->get_file_info()->get_file_id() >= 0)
        {
          surroundingStatementPreceedsTargetStatement = true;

#if REMOVE_STATEMENT_DEBUG
          printf ("   targetStmt->get_file_info()->get_file_id()           = %d \n",targetStmt->get_file_info()->get_file_id());
#endif
          bool returningNullSurroundingStatement = false;
       // while (surroundingStatement->get_file_info()->get_file_id() != targetStmt->get_file_info()->get_file_id())
          while ((returningNullSurroundingStatement == false) && (surroundingStatement != NULL) && surroundingStatement_fileId != targetStmt->get_file_info()->get_file_id())
             {
            // Start by going up in the source sequence.
            // This is a declaration from the wrong file so go to the next statement.
            // surroundingStatement = (insertBefore == true) ? getNextStatement(surroundingStatement) : getPreviousStatement(surroundingStatement);
            // surroundingStatement = (insertBefore == true) ? getPreviousStatement(surroundingStatement) : getNextStatement(surroundingStatement);
               surroundingStatement = getPreviousStatement(surroundingStatement);
               if (surroundingStatement == NULL)
                  {
                    surroundingStatement_fileId = Sg_File_Info::BAD_FILE_ID;
#if REMOVE_STATEMENT_DEBUG
                    printf ("   surroundingStatement_fileId set to Sg_File_Info::BAD_FILE_ID \n");
#endif
                  }
                 else
                  {
                    surroundingStatement_fileId = surroundingStatement->get_file_info()->get_file_id();
#if REMOVE_STATEMENT_DEBUG
                    printf ("   surroundingStatement = %p = %s surroundingStatement->get_file_info()->get_file_id() = %d \n",
                         surroundingStatement,surroundingStatement->class_name().c_str(),surroundingStatement->get_file_info()->get_file_id());
#endif
                  }

#if REMOVE_STATEMENT_DEBUG
               if (surroundingStatement != NULL)
                  {
                    printf ("Looping toward the top of the file for a statement to attach comments and CPP directives to: surroundingStatement = %p = %s file = %s file id = %d line = %d \n",
                         surroundingStatement,surroundingStatement->class_name().c_str(),
                         surroundingStatement->get_file_info()->get_filenameString().c_str(),
                         surroundingStatement->get_file_info()->get_file_id(),
                         surroundingStatement->get_file_info()->get_line());
                  }
                 else
                  {
                    printf ("surroundingStatement == NULL \n");
                  }
#endif
            // As a last resort restart and go down in the statement sequence.
            // if (surroundingStatement == NULL)
               if (surroundingStatement == NULL || isSgGlobal(surroundingStatement) != NULL)
                  {
                 // This is triggered by rose_inputloopUnrolling.C
#if REMOVE_STATEMENT_DEBUG
                    printf ("We just ran off the start (top) of the file... targetStmt = %p = %s \n",targetStmt,targetStmt->class_name().c_str());
#endif
#if 0
                    ROSE_ASSERT(false);
#endif
                 // A statement in the same file could not be identified, so this is false.
                    surroundingStatementPreceedsTargetStatement = false;

                 // Restart by going the other direction (down in the source sequence)
                    surroundingStatement = targetStmt;
                    SgStatement* previousStatement = surroundingStatement;
                 // surroundingStatement = getNextStatement(surroundingStatement);
                    surroundingStatement_fileId = Sg_File_Info::BAD_FILE_ID;
                 // while ( (surroundingStatement != NULL) && (surroundingStatement->get_file_info()->get_file_id() != targetStmt->get_file_info()->get_file_id()) )
                    while ( (surroundingStatement != NULL) && (surroundingStatement_fileId != targetStmt->get_file_info()->get_file_id()) )
                       {
                         previousStatement = surroundingStatement;
                         surroundingStatement = getNextStatement(surroundingStatement);

                         if (surroundingStatement == NULL)
                            {
                              surroundingStatement_fileId = Sg_File_Info::BAD_FILE_ID;
#if REMOVE_STATEMENT_DEBUG
                              printf ("We just ran off the end (bottom) of the file... \n");
#endif
#if 0
                              ROSE_ASSERT(false);
#endif
                              returningNullSurroundingStatement = true;
                            }
                           else
                            {
                              surroundingStatement_fileId = surroundingStatement->get_file_info()->get_file_id();
#if REMOVE_STATEMENT_DEBUG
                              printf ("Looping toward the bottom of the file for a statement to attach comments and CPP directives to: surroundingStatement = %p = %s file = %s file id = %d line = %d \n",
                                   surroundingStatement,surroundingStatement->class_name().c_str(),
                                   surroundingStatement->get_file_info()->get_filenameString().c_str(),
                                   surroundingStatement->get_file_info()->get_file_id(),
                                   surroundingStatement->get_file_info()->get_line());
#endif
                            }
                       }

                    if (surroundingStatement == NULL)
                       {
#if REMOVE_STATEMENT_DEBUG
                         printf ("Resetting the surroundingStatement to the previousStatement = %p = %s \n",previousStatement,previousStatement->class_name().c_str());
#endif
                         surroundingStatement = previousStatement;

                      // Check if this is the input statement we are removing (since the we have to attach comments to the global scope IR node.
                         if (surroundingStatement == targetStmt)
                            {
                           // This can happen if there was only a single statement in a file and it was removed.
                           // All associated comments would have to be relocated to the SgGlobal IR node.
#if REMOVE_STATEMENT_DEBUG
                              printf ("Setting the surroundingStatement to be global scope \n");
#endif
                              surroundingStatement = TransformationSupport::getGlobalScope(targetStmt);
                            }
                       }
                  }
             }

          ROSE_ASSERT(surroundingStatement != NULL);
        }
       else
        {
          printf ("This is a special statement (not associated with the original source code, comment relocation is not supported for these statements) targetStmt file id = %d \n",targetStmt->get_file_info()->get_file_id());
          surroundingStatement = NULL;
        }

#if REMOVE_STATEMENT_DEBUG
     printf ("BOTTOM of findSurroundingStatementFromSameFile(): surroundingStatementPreceedsTargetStatement = %s surroundingStatement = %p \n",surroundingStatementPreceedsTargetStatement ? "true" : "false",surroundingStatement);
     if (surroundingStatement != NULL)
        {
          printf ("surroundingStatement = %p = %s \n",surroundingStatement,surroundingStatement->class_name().c_str());
        }
#endif

  // ROSE_ASSERT(surroundingStatement != NULL);

     return surroundingStatement;
   }


#ifndef USE_ROSE
//! Deep delete a sub AST tree. It uses postorder traversal to delete each child node.
void SageInterface::deepDelete(SgNode* root)
{
#if 0
   struct Visitor: public AstSimpleProcessing {
    virtual void visit(SgNode* n) {
        delete (n);
     }
    };
  Visitor().traverse(root, postorder);
#else
  deleteAST(root);
#endif
}
#endif

//! Replace a statement with another
void SageInterface::replaceStatement(SgStatement* oldStmt, SgStatement* newStmt, bool movePreprocessinInfo/* = false*/)
{
  ROSE_ASSERT(oldStmt);
  ROSE_ASSERT(newStmt);
  if (oldStmt == newStmt) return;
  SgStatement * p = isSgStatement(oldStmt->get_parent());
  ROSE_ASSERT(p);
#if 0
  // TODO  handle replace the body of a C/Fortran function definition with a single statement?
  // Liao 2/1/2010, in some case, we want to replace the entire body (SgBasicBlock) for some parent nodes.
  // the built-in replace_statement() (insert_child() underneath) may not defined for them.
  if (SgFortranDo * f_do = isSgFortranDo (p))
  {
    ROSE_ASSERT (f_do->get_body() == oldStmt);
    if (!isSgBasicBlock(newStmt))
     newStmt = buildBasicBlock (newStmt);
    f_do->set_body(isSgBasicBlock(newStmt));
    newStmt->set_parent(f_do);
  }
  else
#endif
    p->replace_statement(oldStmt,newStmt);

// Some translators have their own handling for this (e.g. the outliner)
  if (movePreprocessinInfo)
    moveUpPreprocessingInfo(newStmt, oldStmt);
}

//! Replace an anchor node with a specified pattern subtree with optional SgVariantExpression.
// All SgVariantExpression in the pattern will be replaced with copies of the anchor node.
SgNode* SageInterface::replaceWithPattern (SgNode * anchor, SgNode* new_pattern)
{
  SgExpression * anchor_exp = isSgExpression(anchor);
  SgExpression * pattern_exp = isSgExpression(new_pattern);
  ROSE_ASSERT (anchor_exp != NULL);
  ROSE_ASSERT (pattern_exp != NULL);

   // we replace all SgExpression within the pattern with copies of anchor
   Rose_STL_Container<SgNode*> opaque_exp_list = NodeQuery::querySubTree(pattern_exp,V_SgExpression);
   for (size_t i = 0; i<opaque_exp_list.size(); i++)
   {
     SgExpression* opaque_exp = isSgExpression(opaque_exp_list[i]);
     ROSE_ASSERT (opaque_exp != NULL);
     if (opaque_exp->variantT() == V_SgVariantExpression)
     {
       SgExpression * anchor_exp_copy = deepCopy(anchor_exp);
       replaceExpression(opaque_exp, anchor_exp_copy);
     }
   }

   // finally we replace anchor_exp with the pattern_exp
   replaceExpression(anchor_exp, pattern_exp, false);
  return new_pattern;
}
/** Generate a name that is unique in the current scope and any parent and children scopes.
 * @param baseName the word to be included in the variable names. */
string SageInterface::generateUniqueVariableName(SgScopeStatement* scope, std::string baseName)
{
    //This implementation tends to generate numbers that are unnecessarily high.
    static int counter = 0;

    string name;
    bool collision = false;
    do
    {
        name = "__" + baseName + boost::lexical_cast<string > (counter++) + "__";

     // DQ (8/16/2013): Modified to reflect new API.
     // Look up the name in the parent scopes
     // SgSymbol* nameSymbol = SageInterface::lookupSymbolInParentScopes(SgName(name), scope);
        SgSymbol* nameSymbol = SageInterface::lookupSymbolInParentScopes(SgName(name), scope,NULL,NULL);
        collision = (nameSymbol != NULL);

        //Look up the name in the children scopes
        Rose_STL_Container<SgNode*> childScopes = NodeQuery::querySubTree(scope, V_SgScopeStatement);

        BOOST_FOREACH(SgNode* childScope, childScopes)
        {
            SgScopeStatement* childScopeStatement = isSgScopeStatement(childScope);

         // DQ (8/16/2013): Modified to reflect new API.
         // nameSymbol = childScopeStatement->lookup_symbol(SgName(name));
            nameSymbol = childScopeStatement->lookup_symbol(SgName(name),NULL,NULL);

            collision = collision || (nameSymbol != NULL);
        }
    } while (collision);

    return name;
}


std::pair<SgVariableDeclaration*, SgExpression*> SageInterface::createTempVariableForExpression
(SgExpression* expression, SgScopeStatement* scope, bool initializeInDeclaration, SgAssignOp** reEvaluate)
{
    SgType* expressionType = expression->get_type();
    SgType* variableType = expressionType;

    //If the expression has a reference type, we need to use a pointer type for the temporary variable.
    //Else, re-assigning the variable is not possible
    bool isReferenceType = SageInterface::isReferenceType(expressionType);
    if (isReferenceType)
    {
        SgType* expressionBaseType = expressionType->stripType(SgType::STRIP_TYPEDEF_TYPE | SgType::STRIP_REFERENCE_TYPE);
        variableType = SageBuilder::buildPointerType(expressionBaseType);
    }

    // If the expression is a dereferenced pointer, use a reference to hold it.
    if (isSgPointerDerefExp(expression))
        variableType = SageBuilder::buildReferenceType(variableType);

    //Generate a unique variable name
    string name = generateUniqueVariableName(scope);

    //Initialize the temporary variable to an evaluation of the expression
    SgExpression* tempVarInitExpression = SageInterface::copyExpression(expression);
    ROSE_ASSERT(tempVarInitExpression != NULL);
    if (isReferenceType)
    {
        //FIXME: the next line is hiding a bug in ROSE. Remove this line and talk to Dan about the resulting assert
        tempVarInitExpression->set_lvalue(false);

        tempVarInitExpression = SageBuilder::buildAddressOfOp(tempVarInitExpression);
    }

    //Optionally initialize the variable in its declaration
    SgAssignInitializer* initializer = NULL;
    if (initializeInDeclaration)
    {
        SgExpression* initExpressionCopy = SageInterface::copyExpression(tempVarInitExpression);
        initializer = SageBuilder::buildAssignInitializer(initExpressionCopy);
    }

    SgVariableDeclaration* tempVarDeclaration = SageBuilder::buildVariableDeclaration(name, variableType, initializer, scope);
    ROSE_ASSERT(tempVarDeclaration != NULL);

    //Now create the assignment op for reevaluating the expression
    if (reEvaluate != NULL)
    {
        SgVarRefExp* tempVarReference = SageBuilder::buildVarRefExp(tempVarDeclaration);
        *reEvaluate = SageBuilder::buildAssignOp(tempVarReference, tempVarInitExpression);
    }

    //Build the variable reference expression that can be used in place of the original expression
    SgExpression* varRefExpression = SageBuilder::buildVarRefExp(tempVarDeclaration);
    if (isReferenceType)
    {
        //The temp variable is a pointer type, so dereference it before using it
        varRefExpression = SageBuilder::buildPointerDerefExp(varRefExpression);
    }

    return std::make_pair(tempVarDeclaration, varRefExpression);
}

// This function creates a temporary variable for a given expression in the given scope
// This is different from SageInterface::createTempVariableForExpression in that it does not
// try to be smart to create pointers to reference types and so on. The tempt is initialized to expression.
// The caller is responsible for setting the parent of SgVariableDeclaration since buildVariableDeclaration
// may not set_parent() when the scope stack is empty. See programTransformation/extractFunctionArgumentsNormalization/ExtractFunctionArguments.C for sample usage.

std::pair<SgVariableDeclaration*, SgExpression*> SageInterface::createTempVariableAndReferenceForExpression
(SgExpression* expression, SgScopeStatement* scope)
{
    SgType* expressionType = expression->get_type();
    SgType* variableType = expressionType;

    //Generate a unique variable name
    string name = generateUniqueVariableName(scope);
    
    //initialize the variable in its declaration
    SgAssignInitializer* initializer = NULL;
    SgExpression* initExpressionCopy = SageInterface::copyExpression(expression);
    initializer = SageBuilder::buildAssignInitializer(initExpressionCopy);
    
    SgVariableDeclaration* tempVarDeclaration = SageBuilder::buildVariableDeclaration(name, variableType, initializer, scope);
    ROSE_ASSERT(tempVarDeclaration != NULL);
    
    //Build the variable reference expression that can be used in place of the original expression
    SgExpression* varRefExpression = SageBuilder::buildVarRefExp(tempVarDeclaration);    
    return std::make_pair(tempVarDeclaration, varRefExpression);
}


// This code is based on OpenMP translator's ASTtools::replaceVarRefExp() and astInling's replaceExpressionWithExpression()
// Motivation: It involves the parent node to replace a VarRefExp with a new node
// Used to replace shared variables with the dereference expression of their addresses
// e.g. to replace shared1 with (*__pp_shared1)

void SageInterface::replaceExpression(SgExpression* oldExp, SgExpression* newExp, bool keepOldExp/*=false*/)
{
  ROSE_ASSERT(oldExp);
  ROSE_ASSERT(newExp);
  if (oldExp==newExp) return;

  if (isSgVarRefExp(newExp))
    newExp->set_need_paren(true); // enclosing new expression with () to be safe

  SgNode* parent = oldExp->get_parent();
  ROSE_ASSERT(parent!=NULL);
  newExp->set_parent(parent);

  // set lvalue when necessary
  if (oldExp->get_lvalue() == true) newExp->set_lvalue(true);

  if (isSgExprStatement(parent)) {
    isSgExprStatement(parent)->set_expression(newExp);
    }
  else if (isSgForStatement(parent)) {
    ROSE_ASSERT (isSgForStatement(parent)->get_increment() == oldExp);
    isSgForStatement(parent)->set_increment(newExp);
    // TODO: any other cases here??
  }
  else if (isSgReturnStmt(parent))
    isSgReturnStmt(parent)->set_expression(newExp);
  else  if (isSgBinaryOp(parent)!=NULL){
    if (oldExp==isSgBinaryOp(parent)->get_lhs_operand())
        {
          isSgBinaryOp(parent)->set_lhs_operand(newExp);
         }
     else if (oldExp==isSgBinaryOp(parent)->get_rhs_operand())
         {isSgBinaryOp(parent)->set_rhs_operand(newExp); }
     else
       ROSE_ASSERT(false);
   } else //unary parent
  if (isSgUnaryOp(parent)!=NULL){
      if (oldExp==isSgUnaryOp(parent)->get_operand_i())
           isSgUnaryOp(parent)->set_operand_i(newExp);
      else
        ROSE_ASSERT(false);
  }  else//SgConditionalExp
  if (isSgConditionalExp(parent)!=NULL){
     SgConditionalExp *expparent= isSgConditionalExp(parent);//get explicity type parent
     if (oldExp==expparent->get_conditional_exp()) expparent->set_conditional_exp(newExp);
     else if (oldExp==expparent->get_true_exp()) expparent->set_true_exp(newExp);
     else if (oldExp==expparent->get_false_exp()) expparent->set_false_exp(newExp);
     else
       ROSE_ASSERT(false);
  } else if (isSgExprListExp(parent)!=NULL)
  {
    SgExpressionPtrList & explist = isSgExprListExp(parent)->get_expressions();
    for (Rose_STL_Container<SgExpression*>::iterator i=explist.begin();i!=explist.end();i++)
      if (isSgExpression(*i)==oldExp) {
        isSgExprListExp(parent)->replace_expression(oldExp,newExp);
       // break; //replace the first occurrence only??
      }
  }
  else if (isSgValueExp(parent))
  {
      // For compiler generated code, this could happen.
      // We can just ignore this function call since it will not appear in the final AST.
      return;
  }
  else if (isSgExpression(parent)) {
    int worked = isSgExpression(parent)->replace_expression(oldExp, newExp);
    // ROSE_DEPRECATED_FUNCTION
    ROSE_ASSERT (worked);
  }
  else if (isSgInitializedName(parent))
  {
          SgInitializedName* initializedNameParent = isSgInitializedName(parent);
          if (oldExp == initializedNameParent->get_initializer())
          {
                  //We can only replace an initializer expression with another initializer expression
                  ROSE_ASSERT(isSgInitializer(newExp));
                  initializedNameParent->set_initializer(isSgInitializer(newExp));
          }
          else
          {
                  //What other expressions can be children of an SgInitializedname?
                  ROSE_ASSERT(false);
          }
  }
  else if (isSgFortranDo(parent))
  {
    SgFortranDo* fortranDo = isSgFortranDo(parent);
    if(oldExp == fortranDo->get_initialization())
    {
      fortranDo->set_initialization(newExp);
    }
    else if(oldExp == fortranDo->get_bound())
    {
      fortranDo->set_bound(newExp);
    }
    else if(oldExp == fortranDo->get_increment())
    {
      fortranDo->set_increment(newExp);
    }
    else
    {
      ROSE_ASSERT(false);
    }
  }
 else{
  cerr<<"SageInterface::replaceExpression(). Unhandled parent expression type of SageIII enum value: " <<parent->class_name()<<endl;
  ROSE_ASSERT(false);
  }

  if (!keepOldExp)
  {
    deepDelete(oldExp); // avoid dangling node in memory pool
  }
  else
  {
      oldExp->set_parent(NULL);
  }

} //replaceExpression()

 SgStatement* SageInterface::getNextStatement(SgStatement * currentStmt)
{
// reuse the implementation in ROSE namespace from src/roseSupport/utility_functions.C
  return Rose::getNextStatement(currentStmt);
}

  SgStatement* SageInterface::getPreviousStatement(SgStatement * currentStmt)
{
  return Rose::getPreviousStatement(currentStmt);
}

bool SageInterface::isEqualToIntConst(SgExpression* e, int value) {
     return isSgIntVal(e) && isSgIntVal(e)->get_value() == value;
  }

 bool SageInterface::isSameFunction(SgFunctionDeclaration* func1, SgFunctionDeclaration* func2)
 {
   ROSE_ASSERT(func1&& func2);
   bool result = false;
   if (func1 == func2)
     result = true;
   else
    {
      if (is_C_language()||is_C99_language()||is_PHP_language()||is_Cuda_language()||is_Python_language())
      {
        if (func1->get_name() == func2->get_name())
          result = true;
      }
      else if (is_Cxx_language() || is_Java_language())
      {
         if (func1->get_qualified_name().getString() +
            func1->get_mangled_name().getString() ==
            func2->get_qualified_name().getString() +
            func2->get_mangled_name().getString()
            )
         result = true;
      }
      else if (is_Fortran_language())
      {
        if (func1->get_name() == func2->get_name())
          result = true;
      }
      else
      {
        cout<<"Error: SageInterface::isSameFunction(): unhandled language"<<endl;
        ROSE_ASSERT(false);
      }

    } // not identical
  return result;
 } // isSameFunction()

//! Check if a statement is the last statement within its closed scope
 bool SageInterface::isLastStatement(SgStatement* stmt)
{
  bool result =false;
  ROSE_ASSERT(stmt != NULL);
  SgScopeStatement* p_scope = stmt->get_scope();
  ROSE_ASSERT(p_scope != NULL);
#if 0
  if (p_scope->containsOnlyDeclarations())
  {
    SgDeclarationStatementPtrList stmtlist= p_scope->getDeclarationList ()
      if (stmtlist[stmtlist.size()-1] == stmt)
        result = true;
  }
  else
  {
    SgStatementPtrList  stmtlist= p_scope->getStatementList ();
    if (stmtlist[stmtlist.size()-1] == stmt)
      result = true;
  }
#endif
  SgStatementPtrList  stmtlist= p_scope->generateStatementList ();
  if (stmtlist[stmtlist.size()-1] == stmt)
    result = true;

  return result;
}

#ifndef USE_ROSE
//-----------------------------------------------
// Remove original expression trees from expressions, so you can change
// the value and have it unparsed correctly.
void SageInterface::removeAllOriginalExpressionTrees(SgNode* top) {
  struct Visitor: public AstSimpleProcessing {
    virtual void visit(SgNode* n) {
      if (isSgValueExp(n)) {
        isSgValueExp(n)->set_originalExpressionTree(NULL);
      } else if (isSgCastExp(n)) {
        isSgCastExp(n)->set_originalExpressionTree(NULL);
      }
    }
  };
  Visitor().traverse(top, preorder);
}
#endif

SgSwitchStatement* SageInterface::findEnclosingSwitch(SgStatement* s) {
  while (s && !isSgSwitchStatement(s)) {
    s = isSgStatement(s->get_parent());
  }
  ROSE_ASSERT (s);
  return isSgSwitchStatement(s);
}

SgScopeStatement* SageInterface::findEnclosingLoop(SgStatement* s, const std::string& label, bool stopOnSwitches) {
  /* label can represent a fortran label or a java label provided as a label in a continue/break statement */
  for (; s; s = isSgStatement(s->get_parent())) {
    SgScopeStatement* sc = isSgScopeStatement(s);
    // Need to check for empty label as for java we must detect the
        // innermost labeled statement and skip everything in between
    switch (s->variantT()) {
      case V_SgDoWhileStmt: {
          if (label.empty()) {
                  return sc;
          }
          break;
      }
      case V_SgForStatement: {
          if (label.empty()) {
                  return sc;
          }
          break;
      }
      case V_SgFortranDo:
      case V_SgFortranNonblockedDo: {
        if (label.empty() ||
                        label == isSgFortranDo(sc)->get_string_label()) {
          return sc;
        }
        break;
      }
      case V_SgWhileStmt: {
        if (label.empty() ||
                        label == isSgWhileStmt(sc)->get_string_label()) {
          return sc;
        }
        break;
      }
      case V_SgSwitchStatement: {
        if (stopOnSwitches) return sc;
        break;
      }
      case V_SgJavaForEachStatement: {
          if (label.empty()) {
                  return sc;
          }
          break;
      }
      case V_SgJavaLabelStatement: {
          if (label.empty() ||
                          label == isSgJavaLabelStatement(sc)->get_label().getString()) {
            return sc;
          }
          break;
      }
      default: continue;
    }
  }
  return NULL;
}

#ifndef USE_ROSE
void SageInterface::removeJumpsToNextStatement(SgNode* top)
{
 class RemoveJumpsToNextStatementVisitor: public AstSimpleProcessing {
    public:
    virtual void visit(SgNode* n) {
      if (isSgBasicBlock(n)) {
        SgBasicBlock* bb = isSgBasicBlock(n);
        bool changes = true;
        while (changes) {
          changes = false;
          for (SgStatementPtrList::iterator i = bb->get_statements().begin();
               i != bb->get_statements().end(); ++i) {
            if (isSgGotoStatement(*i)) {
              SgGotoStatement* gs = isSgGotoStatement(*i);
              SgStatementPtrList::iterator inext = i;
              ++inext;
             if (inext == bb->get_statements().end())
                continue;
             if (!isSgLabelStatement(*inext))
                continue;
              SgLabelStatement* ls = isSgLabelStatement(*inext);
              if (gs->get_label() == ls) {
                changes = true;
                bb->get_statements().erase(i);
                break;
              }
            }
          }
        }
      }
    }
  };

  RemoveJumpsToNextStatementVisitor().traverse(top, postorder);

}
#endif

// special purpose remove for AST transformation/optimization from astInliner, don't use it otherwise.
void SageInterface::myRemoveStatement(SgStatement* stmt) {
  // assert (LowLevelRewrite::isRemovableStatement(*i));
  SgStatement* parent = isSgStatement(stmt->get_parent());
  ROSE_ASSERT (parent);
  SgBasicBlock* bb = isSgBasicBlock(parent);
  SgForInitStatement* fis = isSgForInitStatement(parent);
  if (bb || fis) {
    ROSE_ASSERT (bb || fis);
    SgStatementPtrList& siblings =
      (bb ? bb->get_statements() : fis->get_init_stmt());
    SgStatementPtrList::iterator j =
      std::find(siblings.begin(), siblings.end(), stmt);
    ROSE_ASSERT (j != siblings.end());
    siblings.erase(j);
    // LowLevelRewrite::remove(*i);
  } else {
    parent->replace_statement(stmt, new SgNullStatement(TRANS_FILE));
  }
}


#ifndef USE_ROSE
// Remove all unused labels in a section of code.
void SageInterface::removeUnusedLabels(SgNode* top) {

class FindUsedAndAllLabelsVisitor: public AstSimpleProcessing {
  SgLabelStatementPtrSet& used;
  SgLabelStatementPtrSet& all;

  public:
  FindUsedAndAllLabelsVisitor(SgLabelStatementPtrSet& used,
                              SgLabelStatementPtrSet& all):
    used(used), all(all) {}

  virtual void visit(SgNode* n) {
    if (isSgGotoStatement(n)) {
      used.insert(isSgGotoStatement(n)->get_label());
    }
    if (isSgLabelStatement(n)) {
      all.insert(isSgLabelStatement(n));
    }
  }
};

  SgLabelStatementPtrSet used;
  SgLabelStatementPtrSet unused;
  FindUsedAndAllLabelsVisitor(used, unused).traverse(top, preorder);
  for (SgLabelStatementPtrSet::iterator i = used.begin();
       i != used.end(); ++i) {
    assert (unused.find(*i) != unused.end());
    // std::cout << "Keeping used label " << (*i)->get_label().str() << std::endl;
    unused.erase(*i);
  }
  for (SgLabelStatementPtrSet::iterator i = unused.begin();
       i != unused.end(); ++i) {
    // std::cout << "Removing unused label " << (*i)->get_label().str() << std::endl;
    myRemoveStatement(*i);
  }
}
#endif

  SgStatement* SageInterface::getLoopBody(SgScopeStatement* loopStmt) {
    if (isSgWhileStmt(loopStmt)) return isSgWhileStmt(loopStmt)->get_body();
    if (isSgForStatement(loopStmt)) return isSgForStatement(loopStmt)->get_loop_body();
    if (isSgDoWhileStmt(loopStmt)) return isSgDoWhileStmt(loopStmt)->get_body();

        ROSE_ASSERT (!"Bad loop kind");
    return NULL;
  }

  void SageInterface::setLoopBody(SgScopeStatement* loopStmt, SgStatement* body) {
    if (isSgWhileStmt(loopStmt)) {
      isSgWhileStmt(loopStmt)->set_body(body);
    } else if (isSgForStatement(loopStmt)) {
      isSgForStatement(loopStmt)->set_loop_body(body);
    } else if (isSgDoWhileStmt(loopStmt)) {
      isSgDoWhileStmt(loopStmt)->set_body(body);
    } else {
      ROSE_ASSERT (!"Bad loop kind");
    }
    body->set_parent(loopStmt);
  }

  SgStatement* SageInterface::getLoopCondition(SgScopeStatement* loopStmt) {
    if (isSgWhileStmt(loopStmt)) return isSgWhileStmt(loopStmt)->get_condition();
    if (isSgForStatement(loopStmt)) return isSgForStatement(loopStmt)->get_test();
    if (isSgDoWhileStmt(loopStmt)) return isSgDoWhileStmt(loopStmt)->get_condition();

        ROSE_ASSERT (!"Bad loop kind");
    return NULL;
  }

  void SageInterface::setLoopCondition(SgScopeStatement* loopStmt, SgStatement* cond) {
    if (isSgWhileStmt(loopStmt)) {
      isSgWhileStmt(loopStmt)->set_condition(cond);
    } else if (isSgForStatement(loopStmt)) {
      isSgForStatement(loopStmt)->set_test(cond);
    } else if (isSgDoWhileStmt(loopStmt)) {
      isSgDoWhileStmt(loopStmt)->set_condition(cond);
    } else {
      ROSE_ASSERT (!"Bad loop kind");
    }
    cond->set_parent(loopStmt);
  }

//! A helper function to strip off possible type casting operations for an expression
// usually useful when compare two expressions to see if they actually refer to the same variable
static SgExpression* SkipCasting (SgExpression* exp)
{
  SgCastExp* cast_exp = isSgCastExp(exp);
   if (cast_exp != NULL)
   {
      SgExpression* operand = cast_exp->get_operand();
      assert(operand != 0);
      return SkipCasting(operand);
   }
  else
    return exp;
}

//! Promote the single variable declaration statement outside of the for loop header's init statement, e.g. for (int i=0;) becomes int i_x; for (i_x=0;..) and rewrite the loop with the new index variable
bool SageInterface::normalizeForLoopInitDeclaration(SgForStatement* loop) {
  ROSE_ASSERT(loop!=NULL);

  SgStatementPtrList &init = loop ->get_init_stmt();
  if (init.size() !=1) // We only handle one statement case
    return false;
  else
  {
    SgStatement* init1 = init.front();
    SgVariableDeclaration* decl = isSgVariableDeclaration(init1);
    if (decl)
    {
      SgVariableSymbol* osymbol = getFirstVarSym(decl);
      SgInitializedName* ivarname = decl->get_variables().front();
      SgExpression* lbast = NULL; // the lower bound, initial state
      ROSE_ASSERT(ivarname != NULL);
      SgInitializer * initor = ivarname->get_initializer();
      if (isSgAssignInitializer(initor))
      {
        lbast = isSgAssignInitializer(initor)->get_operand();
      } else
      { //SgConstructorInitializer etc.
        // other complex declaration statements, such as Decomposition::Iterator ditr(&decomp) should be skipped
        // they cause a loop to be non-canonical.
        return false;
      }

      // add a new statement like int i; and insert it to the enclosing function
      // There are multiple choices about where to insert this statement:
      //  global scope: max name pollution,
      //  right before the loop: mess up perfectly nested loops
      //  So we prepend the statement to the enclosing function's body
      SgFunctionDefinition* funcDef =  getEnclosingFunctionDefinition(loop);
      ROSE_ASSERT(funcDef!=NULL);
      SgBasicBlock* funcBody = funcDef->get_body();
      ROSE_ASSERT(funcBody!=NULL);
      //TODO a better name
      std::ostringstream os;
      os<<ivarname->get_name().getString();
      os<<"_nom_";
      os<<++gensym_counter;
      SgVariableDeclaration* ndecl = buildVariableDeclaration(os.str(),ivarname->get_type(), NULL, funcBody);
      prependStatement(ndecl, funcBody);
      SgVariableSymbol* nsymbol = getFirstVarSym(ndecl);

      // replace variable ref to the new symbol
      Rose_STL_Container<SgNode*> varRefs = NodeQuery::querySubTree(loop,V_SgVarRefExp);
      for (Rose_STL_Container<SgNode *>::iterator i = varRefs.begin(); i != varRefs.end(); i++)
      {
        SgVarRefExp *vRef = isSgVarRefExp((*i));
        if (vRef->get_symbol()==osymbol)
          vRef->set_symbol(nsymbol);
      }
      // replace for (int i=0;) with for (i=0;)
      SgExprStatement* ninit = buildAssignStatement(buildVarRefExp(nsymbol),deepCopy(lbast));
      removeStatement(decl); //any side effect to the symbol? put after symbol replacement anyway
      init.push_back(ninit);
      ROSE_ASSERT (loop->get_for_init_stmt () != NULL);
      // ninit->set_parent(loop);
       ninit->set_parent(loop->get_for_init_stmt ());
    }
  }
  return true;
}
//! Normalize a for loop, part of migrating Qing's loop handling into SageInterface
// Her loop translation does not pass AST consistency tests so we rewrite some of them here
// NormalizeCPP.C  NormalizeLoopTraverse::ProcessLoop()
bool SageInterface::forLoopNormalization(SgForStatement* loop, bool foldConstant /*= true*/)
{
  ROSE_ASSERT(loop != NULL);
  // Normalize initialization statement of the for loop
  // -------------------------------------
  // for (int i=0;... ) becomes int i; for (i=0;..)
  // Only roughly check here, isCanonicalForLoop() should be called to have a stricter check
  if (!normalizeForLoopInitDeclaration(loop))
    return false;

  // Normalized the test expressions
  // -------------------------------------
  SgExpression* test = loop->get_test_expr();
  SgExpression* testlhs=NULL, * testrhs=NULL;
  if (isSgBinaryOp(test))
  {
    testlhs = isSgBinaryOp(test)->get_lhs_operand();
    testrhs = isSgBinaryOp(test)->get_rhs_operand();
    ROSE_ASSERT(testlhs && testrhs);
  }
  else
    return false;
  // keep the variable since test will be removed later on
  SgVarRefExp* testlhs_var = isSgVarRefExp(SkipCasting(testlhs));
  if (testlhs_var == NULL )
    return false;
  SgVariableSymbol * var_symbol = testlhs_var->get_symbol();
  if (var_symbol==NULL)
    return false;

  switch (test->variantT()) {
    case V_SgLessThanOp:  // i<x is normalized to i<= (x-1)
      replaceExpression(test, buildLessOrEqualOp(deepCopy(testlhs),
            buildSubtractOp(deepCopy(testrhs), buildIntVal(1))));
      // deepDelete(test);// replaceExpression() does this already by default.
      break;
    case V_SgGreaterThanOp: // i>x is normalized to i>= (x+1)
      replaceExpression( test, buildGreaterOrEqualOp(deepCopy(testlhs),
            buildAddOp(deepCopy(testrhs), buildIntVal(1))));
      break;
    case V_SgLessOrEqualOp:
    case V_SgGreaterOrEqualOp:
    case V_SgNotEqualOp: //TODO Do we want to allow this?
      break;
    default:
      return false;
  }
  // Normalize the increment expression
  // -------------------------------------
  SgExpression* incr = loop->get_increment();
  ROSE_ASSERT(incr != NULL);
  switch (incr->variantT()) {
    case V_SgPlusPlusOp: //i++ is normalized to i+=1
      {
        // check if the variables match
        SgVarRefExp* incr_var = isSgVarRefExp(SkipCasting(isSgPlusPlusOp(incr)->get_operand()));
        if (incr_var == NULL) return false;
        if ( incr_var->get_symbol() != var_symbol)
          return false;
        replaceExpression(incr,
            buildPlusAssignOp(isSgExpression(deepCopy(incr_var)),buildIntVal(1)));
        break;
      }
    case V_SgMinusMinusOp: //i-- is normalized to i+=-1
      {
        // check if the variables match
        SgVarRefExp* incr_var = isSgVarRefExp(SkipCasting(isSgMinusMinusOp(incr)->get_operand()));
        if (incr_var == NULL) return false;
        if ( incr_var->get_symbol() != var_symbol)
          return false;
        replaceExpression(incr,
            buildPlusAssignOp(isSgExpression(deepCopy(incr_var)), buildIntVal(-1)));
        break;
      }
    case V_SgMinusAssignOp: // i-= s is normalized to i+= -s
      {
        SgVarRefExp* incr_var = isSgVarRefExp(SkipCasting(isSgMinusAssignOp(incr)->get_lhs_operand()));
        SgExpression* rhs = isSgMinusAssignOp(incr)->get_rhs_operand();
        ROSE_ASSERT (rhs != NULL);
        if (incr_var == NULL) return false;
        if ( incr_var->get_symbol() != var_symbol)
          return false;
        replaceExpression(incr,
            buildPlusAssignOp(isSgExpression(deepCopy(incr_var)), buildMultiplyOp(buildIntVal(-1), copyExpression(rhs))));
        break;
      }
    case V_SgAssignOp:
    case V_SgPlusAssignOp:
      break;
    default:
      return false;
  }

  // Normalize the loop body: ensure there is a basic block
  SgBasicBlock* body = ensureBasicBlockAsBodyOfFor(loop);
  ROSE_ASSERT(body!=NULL);
   // Liao, 9/22/2009
   // folding entire loop may cause decreased accuracy for floating point operations
   // we only want to fold the loop controlling expressions
  if (foldConstant)
  {
    //constantFolding(loop->get_parent());
    constantFolding(loop->get_test());
    constantFolding(loop->get_increment());
  }

  return true;
}
//!Normalize a Fortran Do loop. Make the default increment expression (1) explicit
bool SageInterface::doLoopNormalization(SgFortranDo* loop)
{
  // TODO, normalize continue to enddo ?
  ROSE_ASSERT (loop != NULL);
  SgExpression* e_3 = loop->get_increment();
  if (isSgNullExpression(e_3))
  {
    SgIntVal* iv = buildIntVal(1);
    loop->set_increment(iv);
    iv->set_parent(loop);
    delete (e_3);
  }
  return true;
}

#if 0
bool SageInterface::loopUnrolling(SgForStatement* loop, size_t unrolling_factor)
{
  // normalize the loop first
  if (!forLoopNormalization(loop))
    return false; // input loop cannot be normalized to a canonical form
  // prepare Loop transformation environment
  SgFunctionDeclaration* func = getEnclosingFunctionDeclaration(loop);
  ROSE_ASSERT(func!=NULL);
  AstInterfaceImpl faImpl(func->get_definition()->get_body());
  AstInterface fa(&faImpl);
  ArrayAnnotation* annot = ArrayAnnotation::get_inst();
  ArrayInterface array_interface (*annot);
  array_interface.initialize(fa, AstNodePtrImpl(func->get_definition()));
  array_interface.observe(fa);
  LoopTransformInterface :: set_astInterface(fa);
  LoopTransformInterface :: set_arrayInterface(&array_interface);

  // invoke the unrolling defined in Qing's code
  // the traversal will skip the input node ptr, so we pass loop's parent ptr instead
  AstNodePtr result = AstNodePtrImpl(loop->get_parent()) ;

  LoopUnrolling lu(unrolling_factor);
  //LoopUnrolling lu(unrolling_factor,LoopUnrolling::COND_LEFTOVER);//works but not a good choice
  //if (lu.cmdline_configure()) // this will cause unrolling to be skipped if no -unroll is used in command line
  result = lu(lpTrans, result);
  return true;
}
#else

// a brand new serious implementation for loop unrolling, Liao, 6/25/2009
/* Handle left-over iterations if iteration_count%unrolling_factor != 0
 * Handle stride (step) >1
 * Assuming loop is normalized to [lb,ub,step], ub is inclusive (<=, >=)
 *
 *  iteration_count = (ub-lb+1)%step ==0?(ub-lb+1)/step: (ub-lb+1)/step+1
 *  fringe = iteration_count%unroll_factor==0 ? 0:unroll_factor*step;
 *    fringe ==0 if no leftover iterations
 *    otherwise adjust ub so the leftover iterations will put into the last fringe loop
 *  unrolled loop's header: for (i=lb;i<=ub - fringe; i+= step*unroll_factor)
 *  loop body: copy body n times from 0 to factor -1
 *     stmt(i+ 0*step); ...; stmt (i+ (factor-1)*step);
 *  fringe loop: the same as the original loop, except for no init statement
 *
 * e.g:
 * // unrolling 3 times for the following loop with stride !=1
 *       for (i=0; i<=9; i+=3)
 *       {
 *         a[i]=i;
 *       }
 * // it becomes
 *     // iteration count = 10%3=1 -> 10/3+1 = 4
 *     // fringe = 4%3 =1 --> 3*3
 *     // ub-fringe = 9-3*3
 *        for (i=0; i<=9-3*3; i+=3*3)
 *     {
 *       a[i+3*0]=i;
 *       a[i+3*1]=i;
 *       a[i+3*2]=i;
 *     }
 *     // i=9 is the leftover iteration
 *     for (; i<=9; i+=3)
 *     {
 *       a[i]=i;
 *     }
 *
 */
bool SageInterface::loopUnrolling(SgForStatement* target_loop, size_t unrolling_factor)
{
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
  //Handle 0 and 1, which means no unrolling at all
  if (unrolling_factor <= 1)
    return true;
  // normalize the target loop first
  if (!forLoopNormalization(target_loop));
  {// the return value is not reliable
    //    cerr<<"Error in SageInterface::loopUnrolling(): target loop cannot be normalized."<<endl;
    //    dumpInfo(target_loop);
    //    return false;
  }
  // grab the target loop's essential header information
  SgInitializedName* ivar = NULL;
  SgExpression* lb = NULL;
  SgExpression* ub = NULL;
  SgExpression* step = NULL;
  SgStatement* orig_body = NULL;
  if (!isCanonicalForLoop(target_loop, &ivar, &lb, &ub, &step, &orig_body))
  {
    cerr<<"Error in SageInterface::loopUnrolling(): target loop is not canonical."<<endl;
    dumpInfo(target_loop);
    return false;
  }
  ROSE_ASSERT(ivar&& lb && ub && step);
  ROSE_ASSERT(isSgBasicBlock(orig_body));

   // generate the fringe loop
   bool needFringe = true;
   SgForStatement* fringe_loop = deepCopy<SgForStatement>(target_loop);
   insertStatementAfter(target_loop,fringe_loop);
   removeStatement(fringe_loop->get_for_init_stmt());
   fringe_loop->set_for_init_stmt(NULL);

  // _lu_iter_count = (ub-lb+1)%step ==0?(ub-lb+1)/step: (ub-lb+1)/step+1;
  SgExpression* raw_range_exp =buildSubtractOp(buildAddOp(copyExpression(ub),buildIntVal(1)),
            copyExpression(lb));
  raw_range_exp->set_need_paren(true);
  SgExpression* range_d_step_exp = buildDivideOp(raw_range_exp,copyExpression(step));//(ub-lb+1)/step
  SgExpression* condition_1 = buildEqualityOp(buildModOp(copyExpression(raw_range_exp),copyExpression(step)),buildIntVal(0)); //(ub-lb+1)%step ==0

  SgExpression* iter_count_exp = buildConditionalExp(condition_1,range_d_step_exp, buildAddOp(copyExpression(range_d_step_exp),buildIntVal(1)));
  // fringe = iteration_count%unroll_factor==0 ? 0:unroll_factor*step
  SgExpression* condition_2 = buildEqualityOp(buildModOp(iter_count_exp, buildIntVal(unrolling_factor)), buildIntVal(0));
  SgExpression* initor = buildConditionalExp(condition_2, buildIntVal(0), buildMultiplyOp(buildIntVal(unrolling_factor),copyExpression(step)));

   SgScopeStatement* scope = target_loop->get_scope();
   ROSE_ASSERT(scope != NULL);
   string fringe_name = "_lu_fringe_"+ StringUtility::numberToString(++gensym_counter);
   SgVariableDeclaration* fringe_decl = buildVariableDeclaration(fringe_name, buildIntType(),buildAssignInitializer(initor), scope);
   insertStatementBefore(target_loop, fringe_decl);
   attachComment(fringe_decl, "iter_count = (ub-lb+1)%step ==0?(ub-lb+1)/step: (ub-lb+1)/step+1;");
   attachComment(fringe_decl, "fringe = iter_count%unroll_factor==0 ? 0:unroll_factor*step");

  // compile-time evaluate to see if initor is a constant of value 0
  // if so, the iteration count can be divided even by the unrolling factor
  // and no fringe loop is needed
  // WE have to fold on its parent node to get a possible constant since
  // constant folding only folds children nodes, not the current node to a constant
   ConstantFolding::constantFoldingOptimization(fringe_decl,false);
   SgInitializedName * ivarname = fringe_decl->get_variables().front();
   ROSE_ASSERT(ivarname != NULL);
   // points to a new address if constant folding happens
   SgAssignInitializer * init1 = isSgAssignInitializer(ivarname->get_initializer());
   if (init1)
    if (isSgIntVal(init1->get_operand_i()))
     if (isSgIntVal(init1->get_operand_i())->get_value() == 0)
       needFringe = false;

  // rewrite loop header ub --> ub -fringe; step --> step *unrolling_factor
   SgBinaryOp* ub_bin_op = isSgBinaryOp(ub->get_parent());
   ROSE_ASSERT(ub_bin_op);
   if (needFringe)
     ub_bin_op->set_rhs_operand(buildSubtractOp(copyExpression(ub),buildVarRefExp(fringe_name,scope)));
   else
   {
     ub_bin_op->set_rhs_operand(copyExpression(ub));
     removeStatement(fringe_decl);
   }

   SgBinaryOp* step_bin_op = isSgBinaryOp(step->get_parent());
   ROSE_ASSERT(step_bin_op != NULL);
   step_bin_op->set_rhs_operand(buildMultiplyOp(copyExpression(step),buildIntVal(unrolling_factor)));

   bool isPlus = false;
   if (isSgPlusAssignOp(step_bin_op))
     isPlus = true;
    else if (isSgMinusAssignOp(step_bin_op))
      isPlus = false;
    else
    {
      cerr<<"Error in SageInterface::loopUnrolling(): illegal incremental exp of a canonical loop"<<endl;
      dumpInfo(step_bin_op);
      ROSE_ASSERT(false);
    }

   // copy loop body factor -1 times, and replace reference to ivar  with ivar +/- step*[1 to factor-1]
   for (size_t i =1; i<unrolling_factor; i++)
   {
     SgBasicBlock* body = isSgBasicBlock(deepCopy(fringe_loop->get_loop_body())); // normalized loop has a BB body
     ROSE_ASSERT(body);
     std::vector<SgVarRefExp*> refs = querySubTree<SgVarRefExp> (body, V_SgVarRefExp);
     for (std::vector<SgVarRefExp*>::iterator iter = refs.begin(); iter !=refs.end(); iter++)
     {
       SgVarRefExp* refexp = *iter;
       if (refexp->get_symbol()==ivar->get_symbol_from_symbol_table())
       {
         // replace reference to ivar with ivar +/- step*i
         SgExpression* new_exp = NULL;
         //build replacement  expression for every appearance 
         if (isPlus) //ivar +/- step * i
         new_exp = buildAddOp(buildVarRefExp(ivar,scope),buildMultiplyOp(copyExpression(step),buildIntVal(i)));
         else
         new_exp = buildSubtractOp(buildVarRefExp(ivar,scope),buildMultiplyOp(copyExpression(step),buildIntVal(i)));

         // replace it with the right one
         replaceExpression(refexp, new_exp);
       }
     }
     // copy body to loop body, this should be a better choice
     // to avoid redefinition of variables after unrolling (new scope is introduced to avoid this)
     appendStatement(body,isSgBasicBlock(orig_body));
    // moveStatementsBetweenBlocks(body,isSgBasicBlock(orig_body));
   }

   // remove the fringe loop if not needed finally
   // it is used to buffering the original loop body before in either cases
   if (!needFringe)
     removeStatement(fringe_loop);

   // constant folding for the transformed AST
   ConstantFolding::constantFoldingOptimization(scope,false);
   //ConstantFolding::constantFoldingOptimization(getProject(),false);

#endif

  return true;
}
#endif

// Liao, 6/15/2009
//! A helper function to calculate n!
//! See also, Combinatorics::factorial(), which also checks for overflow.
static size_t myfactorial (size_t n)
{
  size_t result=1;
  for (size_t i=2; i<=n; i++)
    result*=i;
  return result;
}

#endif

#ifndef USE_ROSE

//! A helper function to return a permutation order for n elements based on a lexicographical order number.
//! See also, Combinatorics::permute(), which is faster but does not use strict lexicographic ordering.
std::vector<size_t> getPermutationOrder( size_t n, size_t lexicoOrder)
{
  size_t k = lexicoOrder;
  std::vector<size_t> s(n);
  // initialize the permutation vector
  for (size_t i=0; i<n; i++)
    s[i]=i;

  //compute (n- 1)!
  size_t factorial = myfactorial(n-1);
  //check if the number is not in the range of [0, n! - 1]
  if (k/n>=factorial)
  {
    printf("Error: in getPermutationOrder(), lexicoOrder is larger than n!-1\n");
    ROSE_ASSERT(false);
  }
  // Algorithm:
  //check each element of the array, excluding the right most one.
  //the goal is to find the right element for each s[j] from 0 to n-2
  // method: each position is associated a factorial number
  //    s[0] -> (n-1)!
  //    s[1] -> (n-2)! ...
  // the input number k is divided by the factorial at each position (6, 3, 2, 1 for size =4)
  //   so only big enough k can have non-zero value after division
  //   0 value means no change to the position for the current iteration
  // The non-zero value is further modular by the number of the right hand elements of the current element.
  //     (mode on 4, 3, 2 to get offset 1-2-3, 1-2, 1 from the current position 0, 1, 2)
  //  choose one of them to be moved to the current position,
  //  shift elements between the current and the moved element to the right direction for one position
  for (size_t j=0; j<n-1; j++)
  {
    //calculates the next cell from the cells left
    //(the cells in the range [j, s.length - 1])
    int tempj = (k/factorial) % (n - j);
    //Temporarily saves the value of the cell needed
    // to add to the permutation this time
    int temps = s[j+tempj];
    //shift all elements to "cover" the "missing" cell
    //shift them to the right
    for (size_t i=j+tempj; i>j; i--)
    {
      s[i] = s[i-1]; //shift the chain right
    }
    // put the chosen cell in the correct spot
    s[j]= temps;
    // updates the factorial
    factorial = factorial /(n-(j+1));
  }
#if 0
  for (size_t i = 0; i<n; i++)
    cout<<" "<<s[i];
  cout<<endl;
#endif
  return s;
}

//! Tile the n-level (starting from 1) of a perfectly nested loop nest using tiling size s
/* Translation
 Before:
  for (i = 0; i < 100; i++)
    for (j = 0; j < 100; j++)
      for (k = 0; k < 100; k++)
        c[i][j]= c[i][j]+a[i][k]*b[k][j];

  After tiling i loop nest's level 3 (k-loop) with size 5, it becomes

// added a new controlling loop at the outer most level
  int _lt_var_k;
  for (_lt_var_k = 0; _lt_var_k <= 99; _lt_var_k += 1 * 5) {
    for (i = 0; i < 100; i++)
      for (j = 0; j < 100; j++)
        // rewritten loop header , normalized also
        for (k = _lt_var_k; k <= (99 < (_lt_var_k + 5 - 1))?99 : (_lt_var_k + 5 - 1); k += 1) {
          c[i][j] = c[i][j] + a[i][k] * b[k][j];
        }
  }
// finally run constant folding

 */
bool SageInterface::loopTiling(SgForStatement* loopNest, size_t targetLevel, size_t tileSize)
{
  ROSE_ASSERT(loopNest != NULL);
  ROSE_ASSERT(targetLevel >0);
 // ROSE_ASSERT(tileSize>0);// 1 is allowed
 // skip tiling if tiling size is 0 (no tiling), we allow 0 to get a reference value for the original code being tuned
 // 1 (no need to tile)
  if (tileSize<=1)
    return true;
  // Locate the target loop at level n
  std::vector<SgForStatement* > loops= SageInterface::querySubTree<SgForStatement>(loopNest,V_SgForStatement);
  ROSE_ASSERT(loops.size()>=targetLevel);
  SgForStatement* target_loop = loops[targetLevel -1]; // adjust to numbering starting from 0

  // normalize the target loop first
  if (!forLoopNormalization(target_loop));
  {// the return value is not reliable
//    cerr<<"Error in SageInterface::loopTiling(): target loop cannot be normalized."<<endl;
//    dumpInfo(target_loop);
//    return false;
  }
   // grab the target loop's essential header information
   SgInitializedName* ivar = NULL;
   SgExpression* lb = NULL;
   SgExpression* ub = NULL;
   SgExpression* step = NULL;
   if (!isCanonicalForLoop(target_loop, &ivar, &lb, &ub, &step, NULL))
   {
     cerr<<"Error in SageInterface::loopTiling(): target loop is not canonical."<<endl;
     dumpInfo(target_loop);
     return false;
   }
   ROSE_ASSERT(ivar&& lb && ub && step);

  // Add a controlling loop around the top loop nest
  // Ensure the parent can hold more than one children
  SgLocatedNode* parent = NULL; //SageInterface::ensureBasicBlockAsParent(loopNest)
  if (isBodyStatement(loopNest)) // if it is a single body statement (Already a for statement, not a basic block)
   parent = makeSingleStatementBodyToBlock (loopNest);
  else
    parent = isSgLocatedNode(loopNest ->get_parent());

  ROSE_ASSERT(parent!= NULL);
     // Now we can prepend a controlling loop index variable: __lt_var_originalIndex
  string ivar2_name = "_lt_var_"+ivar->get_name().getString();
  SgScopeStatement* scope = loopNest->get_scope();
  SgVariableDeclaration* loop_index_decl = buildVariableDeclaration
  (ivar2_name, buildIntType(),NULL, scope);
  insertStatementBefore(loopNest, loop_index_decl);
   // init statement of the loop header, copy the lower bound
   SgStatement* init_stmt = buildAssignStatement(buildVarRefExp(ivar2_name,scope), copyExpression(lb));
   //two cases <= or >= for a normalized loop
   SgExprStatement* cond_stmt = NULL;
   SgExpression* orig_test = target_loop->get_test_expr();
   if (isSgBinaryOp(orig_test))
   {
     if (isSgLessOrEqualOp(orig_test))
       cond_stmt = buildExprStatement(buildLessOrEqualOp(buildVarRefExp(ivar2_name,scope),copyExpression(ub)));
     else if (isSgGreaterOrEqualOp(orig_test))
     {
       cond_stmt = buildExprStatement(buildGreaterOrEqualOp(buildVarRefExp(ivar2_name,scope),copyExpression(ub)));
       }
     else
     {
       cerr<<"Error: illegal condition operator for a canonical loop"<<endl;
       dumpInfo(orig_test);
       ROSE_ASSERT(false);
     }
   }
   else
   {
     cerr<<"Error: illegal condition expression for a canonical loop"<<endl;
     dumpInfo(orig_test);
     ROSE_ASSERT(false);
   }
   ROSE_ASSERT(cond_stmt != NULL);

   // build loop incremental  I
   // expression var+=up*tilesize or var-=upper * tilesize
   SgExpression* incr_exp = NULL;
   SgExpression* orig_incr_exp = target_loop->get_increment();
   if( isSgPlusAssignOp(orig_incr_exp))
   {
     incr_exp = buildPlusAssignOp(buildVarRefExp(ivar2_name,scope), buildMultiplyOp(copyExpression(step), buildIntVal(tileSize)));
   }
    else if (isSgMinusAssignOp(orig_incr_exp))
    {
      incr_exp = buildMinusAssignOp(buildVarRefExp(ivar2_name,scope), buildMultiplyOp(copyExpression(step), buildIntVal(tileSize)));
    }
    else
    {
      cerr<<"Error: illegal increment expression for a canonical loop"<<endl;
      dumpInfo(orig_incr_exp);
      ROSE_ASSERT(false);
    }
    SgForStatement* control_loop = buildForStatement(init_stmt, cond_stmt,incr_exp, buildBasicBlock());
  insertStatementBefore(loopNest, control_loop);
  // move loopNest into the control loop
  removeStatement(loopNest);
  appendStatement(loopNest,isSgBasicBlock(control_loop->get_loop_body()));

  // rewrite the lower (i=lb), upper bounds (i<=/>= ub) of the target loop
  SgAssignOp* assign_op  = isSgAssignOp(lb->get_parent());
  ROSE_ASSERT(assign_op);
  assign_op->set_rhs_operand(buildVarRefExp(ivar2_name,scope));
    // ub< var_i+tileSize-1? ub:var_i+tileSize-1
  SgBinaryOp* bin_op = isSgBinaryOp(ub->get_parent());
  ROSE_ASSERT(bin_op);
  SgExpression* ub2 = buildSubtractOp(buildAddOp(buildVarRefExp(ivar2_name,scope), buildIntVal(tileSize)), buildIntVal(1));
  SgExpression* test_exp = buildLessThanOp(copyExpression(ub),ub2);
  test_exp->set_need_paren(true);
  ub->set_need_paren(true);
  ub2->set_need_paren(true);
  SgConditionalExp * triple_exp = buildConditionalExp(test_exp,copyExpression(ub), copyExpression(ub2));
  bin_op->set_rhs_operand(triple_exp);
  // constant folding
  // folding entire loop may decrease the accuracy of floating point calculation
  // we fold loop control expressions only
  //constantFolding(control_loop->get_scope());
  constantFolding(control_loop->get_test());
  constantFolding(control_loop->get_increment());
  return true;
}

//! Interchange/Permutate a n-level perfectly-nested loop rooted at 'loop' using a lexicographical order number within [0,depth!)
bool SageInterface::loopInterchange(SgForStatement* loop, size_t depth, size_t lexicoOrder)
{
  if (lexicoOrder == 0) // allow 0 to mean no interchange at all
    return true;
  // parameter verification
  ROSE_ASSERT(loop != NULL);
  //must have at least two levels
  ROSE_ASSERT (depth >1);
  ROSE_ASSERT(lexicoOrder<myfactorial(depth));
  //TODO need to verify the input loop has n perfectly-nested children loops inside
  // save the loop nest's headers: init, test, and increment
  std::vector<SgForStatement* > loopNest = SageInterface::querySubTree<SgForStatement>(loop,V_SgForStatement);
  ROSE_ASSERT(loopNest.size()>=depth);
  std::vector<std::vector<SgNode*> > loopHeads;
  for (std::vector<SgForStatement* > ::iterator i = loopNest.begin(); i!= loopNest.end(); i++)
  {
    SgForStatement* cur_loop = *i;
    std::vector<SgNode*> head;
    head.push_back(cur_loop->get_for_init_stmt());
    head.push_back(cur_loop->get_test());
    head.push_back(cur_loop->get_increment());
    loopHeads.push_back(head);
  }

  // convert the lexicographical number to a permutation order array permutation[depth]
  std::vector<size_t> changedOrder = getPermutationOrder (depth, lexicoOrder);
  // rewrite the loop nest to reflect the permutation
  // set the header to the new header based on the permutation array
  for (size_t i=0; i<depth; i++)
  {
    // only rewrite if necessary
    if (i != changedOrder[i])
    {
      SgForStatement* cur_loop = loopNest[i];
      std::vector<SgNode*> newhead = loopHeads[changedOrder[i]];

      SgForInitStatement* init = isSgForInitStatement(newhead[0]);
      //ROSE_ASSERT(init != NULL) // could be NULL?
      ROSE_ASSERT(init != cur_loop->get_for_init_stmt());
      cur_loop->set_for_init_stmt(init);
      if (init)
      {
        init->set_parent(cur_loop);
        setSourcePositionForTransformation(init);
      }

      SgStatement* test = isSgStatement(newhead[1]);
      cur_loop->set_test(test);
      if (test)
      {
        test->set_parent(cur_loop);
        setSourcePositionForTransformation(test);
      }

      SgExpression* incr = isSgExpression(newhead[2]);
      cur_loop->set_increment(incr);
      if (incr)
      {
        incr->set_parent(cur_loop);
        setSourcePositionForTransformation(incr);
      }
    }
  }
  return true;
}

//!Return the loop index variable for a C/C++ for or Fortran Do loop
SgInitializedName* SageInterface::getLoopIndexVariable(SgNode* loop)
{
  ROSE_ASSERT(loop != NULL);
  SgInitializedName* ivarname=NULL;

  // Fortran case ------------------
  if (SgFortranDo * do_loop = isSgFortranDo(loop))
  {
    SgAssignOp* assign_op = isSgAssignOp (do_loop->get_initialization());
    ROSE_ASSERT (assign_op != NULL);
    SgVarRefExp* var = isSgVarRefExp(assign_op->get_lhs_operand());
    ROSE_ASSERT (var != NULL);
    ivarname = var->get_symbol()->get_declaration();
    ROSE_ASSERT (ivarname != NULL);
    return ivarname;
  }
  // C/C++ case ------------------------------
  SgForStatement* fs = isSgForStatement(loop);
  ROSE_ASSERT (fs != NULL);

  //Check initialization statement is something like i=xx;
  SgStatementPtrList & init = fs->get_init_stmt();
  if (init.size() !=1)
  {
    cerr<<"SageInterface::getLoopIndexVariable(), no or more than one initialization statements are encountered. Not supported yet "<<endl;
    //ROSE_ASSERT(false);
    return NULL; 
  }
  SgStatement* init1 = init.front();
  SgExpression* ivarast=NULL;

  bool isCase1=false, isCase2=false;
  //consider C99 style: for (int i=0;...)
  if (isSgVariableDeclaration(init1))
  {
    SgVariableDeclaration* decl = isSgVariableDeclaration(init1);
    ivarname = decl->get_variables().front();
    ROSE_ASSERT(ivarname != NULL);
    SgInitializer * initor = ivarname->get_initializer();
    if (isSgAssignInitializer(initor))
      isCase1 = true;
  }// other regular case: for (i=0;..)
  else if (isAssignmentStatement(init1, &ivarast))
  {
    SgVarRefExp* var = isSgVarRefExp(SkipCasting(ivarast));
    if (var)
    {
      ivarname = var->get_symbol()->get_declaration();
      isCase2 = true;
    }
  }
  else if (SgExprStatement* exp_stmt = isSgExprStatement(init1))
  { //case like: for (i = 1, len1 = 0, len2=0; i <= n; i++)
     // AST is: SgCommaOpExp -> SgAssignOp -> SgVarRefExp
    if (SgCommaOpExp* comma_exp = isSgCommaOpExp(exp_stmt->get_expression()))
    {
      SgCommaOpExp* leaf_exp = comma_exp;
      while (isSgCommaOpExp(leaf_exp->get_lhs_operand()))
        leaf_exp = isSgCommaOpExp(leaf_exp->get_lhs_operand());
      if (SgAssignOp* assign_op = isSgAssignOp(leaf_exp->get_lhs_operand()))
      {
        SgVarRefExp* var = isSgVarRefExp(assign_op->get_lhs_operand());
        if (var)
        {
          ivarname = var->get_symbol()->get_declaration();
        }
      }
    }
  }
  else
  {
    cerr<<"Warning: SageInterface::getLoopIndexVariable(). Unhandled init_stmt type of SgForStatement"<<endl;
    cerr<<"Init statement is :"<<init1->class_name() <<" " <<init1->unparseToString()<<endl;
    init1->get_file_info()->display("Debug");
    return NULL; 
    //ROSE_ASSERT (false);
  }
  // Cannot be both true
 // ROSE_ASSERT(!(isCase1&&isCase2));

  //Check loop index's type
  //ROSE_ASSERT(isStrictIntegerType(ivarname->get_type()));
  return ivarname;
}

//!Check if a SgInitializedName is used as a loop index within a AST subtree
//! This function will use a bottom-up traverse starting from the subtree to find all enclosing loops and check if ivar is used as an index for either of them.
bool SageInterface::isLoopIndexVariable(SgInitializedName* ivar, SgNode* subtree_root)
{
  ROSE_ASSERT (ivar != NULL);
  ROSE_ASSERT (subtree_root != NULL);
  bool result = false;
  SgScopeStatement * cur_loop = findEnclosingLoop (getEnclosingStatement(subtree_root));
  while (cur_loop)
  {
    SgInitializedName * i_index = getLoopIndexVariable (cur_loop);
    if (i_index == ivar)
    {
      result = true;
      break;
    }
    else
    { // findEnclosingLoop() is inclusive.
      cur_loop = findEnclosingLoop (getEnclosingStatement(cur_loop->get_parent()));
    }
  }
  return result;
}

//! Get Fortran Do loop's key features
bool SageInterface::isCanonicalDoLoop(SgFortranDo* loop,SgInitializedName** ivar/*=NULL*/, SgExpression** lb/*=NULL*/, SgExpression** ub/*=NULL*/, SgExpression** step/*=NULL*/, SgStatement** body/*=NULL*/, bool *hasIncrementalIterationSpace/*= NULL*/, bool* isInclusiveUpperBound/*=NULL*/)
{
  ROSE_ASSERT(loop != NULL);
  SgFortranDo* fs = isSgFortranDo(loop);
  if (fs == NULL)
    return false;
  // 1. Check initialization statement is something like i=xx;
  SgExpression * init = fs->get_initialization();
  if (init == NULL)
    return false;
  SgAssignOp* init_assign = isSgAssignOp (init);
  SgExpression *lbast=NULL, *ubast=NULL;
 // SgExpression* ivarast=NULL, *stepast=NULL;
  SgInitializedName* ivarname=NULL;

  bool isCase1=false;
  if (init_assign)
   {
     SgVarRefExp* var = isSgVarRefExp(init_assign->get_lhs_operand());
     if (var)
       ivarname = var->get_symbol()->get_declaration();
     lbast = init_assign->get_rhs_operand();
     if (ivarname && lbast )
       isCase1 = true;
   }
   // if not i=1
    if (!isCase1)
      return false;

  //Check loop index's type
  if (!SageInterface::isStrictIntegerType(ivarname->get_type()))
    return false;
#if 0
  //2. Check test expression i [<=, >=, <, > ,!=] bound
  SgBinaryOp* test = isSgBinaryOp(fs->get_test_expr());
  if (test == NULL)
    return false;
  switch (test->variantT()) {
    case V_SgLessOrEqualOp:
       if (isInclusiveUpperBound != NULL)
         *isInclusiveUpperBound = true;
       if (hasIncrementalIterationSpace != NULL)
         *hasIncrementalIterationSpace = true;
       break;
    case V_SgLessThanOp:
       if (isInclusiveUpperBound != NULL)
         *isInclusiveUpperBound = false;
       if (hasIncrementalIterationSpace != NULL)
         *hasIncrementalIterationSpace = true;
       break;
    case V_SgGreaterOrEqualOp:
       if (isInclusiveUpperBound != NULL)
         *isInclusiveUpperBound = true;
        if (hasIncrementalIterationSpace != NULL)
         *hasIncrementalIterationSpace = false;
      break;
    case V_SgGreaterThanOp:
       if (isInclusiveUpperBound != NULL)
         *isInclusiveUpperBound = false;
       if (hasIncrementalIterationSpace != NULL)
         *hasIncrementalIterationSpace = false;
      break;
//    case V_SgNotEqualOp: // Do we really want to allow this != operator ?
      break;
    default:
      return false;
  }
  // check the tested variable is the same as the loop index
  SgVarRefExp* testvar = isSgVarRefExp(SkipCasting(test->get_lhs_operand()));
  if (testvar == NULL)
    return false;
  if (testvar->get_symbol() != ivarname->get_symbol_from_symbol_table ())
    return false;
#endif
 //grab the upper bound
  ubast = loop->get_bound();
  // Fortran Do loops always have inclusive upper bound
  if (isInclusiveUpperBound != NULL)
     *isInclusiveUpperBound = true;
  //3. Check the increment expression
  SgExpression* incr = fs->get_increment();
  ROSE_ASSERT (incr != NULL);
  if (isSgNullExpression(incr))
  {
    cerr<<"Error:isCanonicalDoLoop() found NULL increment expression. Please call doLoopNormalization() first!"<<endl;
    ROSE_ASSERT (false);
  }
  if (hasIncrementalIterationSpace != NULL)
  {
     *hasIncrementalIterationSpace = true;
    // We can only tell a few cases
    if (SgIntVal* i_v = isSgIntVal(incr))
    {
      if (i_v->get_value()<0)
     *hasIncrementalIterationSpace = false;
    }
  }
#if 0
  SgVarRefExp* incr_var = NULL;
  switch (incr->variantT()) {
    case V_SgPlusAssignOp: //+=
    case V_SgMinusAssignOp://-=
      incr_var = isSgVarRefExp(SkipCasting(isSgBinaryOp(incr)->get_lhs_operand()));
      stepast = isSgBinaryOp(incr)->get_rhs_operand();
      break;
    case V_SgPlusPlusOp:   //++
    case V_SgMinusMinusOp:  //--
      incr_var = isSgVarRefExp(SkipCasting(isSgUnaryOp(incr)->get_operand()));
      stepast = buildIntVal(1); // will this dangling SgNode cause any problem?
      break;
    default:
      return false;
  }
  if (incr_var == NULL)
    return false;
  if (incr_var->get_symbol() != ivarname->get_symbol_from_symbol_table ())
    return false;
#endif
  // return loop information if requested
  if (ivar != NULL)
    *ivar = ivarname;
  if (lb != NULL)
    *lb = lbast;
  if (ub != NULL)
    *ub = ubast;
  if (step != NULL)
    *step = incr;
  if (body != NULL) {
    *body = fs->get_body();
  }
  return true;
}


//! Based on AstInterface::IsFortranLoop() and ASTtools::getLoopIndexVar()
//TODO check the loop index is not being written in the loop body
bool SageInterface::isCanonicalForLoop(SgNode* loop,SgInitializedName** ivar/*=NULL*/, SgExpression** lb/*=NULL*/, SgExpression** ub/*=NULL*/, SgExpression** step/*=NULL*/, SgStatement** body/*=NULL*/, bool *hasIncrementalIterationSpace/*= NULL*/, bool* isInclusiveUpperBound/*=NULL*/)
{
  ROSE_ASSERT(loop != NULL);
  SgForStatement* fs = isSgForStatement(loop);
  //SgFortranDo* fs2 = isSgFortranDo(loop);
  if (fs == NULL)
  {
   // if (fs2)
   //   return isCanonicalDoLoop (fs2, ivar, lb, ub, step, body, hasIncrementalIterationSpace, isInclusiveUpperBound);
   // else
      return false;
   }
  // 1. Check initialization statement is something like i=xx;
  SgStatementPtrList & init = fs->get_init_stmt();
  if (init.size() !=1)
    return false;
  SgStatement* init1 = init.front();
  SgExpression* ivarast=NULL, *lbast=NULL, *ubast=NULL, *stepast=NULL;
  SgInitializedName* ivarname=NULL;

  bool isCase1=false, isCase2=false;
  //consider C99 style: for (int i=0;...)
  if (isSgVariableDeclaration(init1))
   {
     SgVariableDeclaration* decl = isSgVariableDeclaration(init1);
     ivarname = decl->get_variables().front();
     ROSE_ASSERT(ivarname != NULL);
     SgInitializer * initor = ivarname->get_initializer();
     if (isSgAssignInitializer(initor))
     {
       lbast = isSgAssignInitializer(initor)->get_operand();
       isCase1 = true;
     }
   }// other regular case: for (i=0;..)
   else if (isAssignmentStatement(init1, &ivarast, &lbast))
   {
     SgVarRefExp* var = isSgVarRefExp(SkipCasting(ivarast));
     if (var)
     {
       ivarname = var->get_symbol()->get_declaration();
       isCase2 = true;
     }
   }
   // Cannot be both true
   ROSE_ASSERT(!(isCase1&&isCase2));
   // if not either case is true
    if (!(isCase1||isCase2))
      return false;

  //Check loop index's type
  if (!isStrictIntegerType(ivarname->get_type()))
    return false;

  //2. Check test expression i [<=, >=, <, > ,!=] bound
  SgBinaryOp* test = isSgBinaryOp(fs->get_test_expr());
  if (test == NULL)
    return false;
  switch (test->variantT()) {
    case V_SgLessOrEqualOp:
       if (isInclusiveUpperBound != NULL)
         *isInclusiveUpperBound = true;
       if (hasIncrementalIterationSpace != NULL)
         *hasIncrementalIterationSpace = true;
       break;
    case V_SgLessThanOp:
       if (isInclusiveUpperBound != NULL)
         *isInclusiveUpperBound = false;
       if (hasIncrementalIterationSpace != NULL)
         *hasIncrementalIterationSpace = true;
       break;
    case V_SgGreaterOrEqualOp:
       if (isInclusiveUpperBound != NULL)
         *isInclusiveUpperBound = true;
        if (hasIncrementalIterationSpace != NULL)
         *hasIncrementalIterationSpace = false;
      break;
    case V_SgGreaterThanOp:
       if (isInclusiveUpperBound != NULL)
         *isInclusiveUpperBound = false;
       if (hasIncrementalIterationSpace != NULL)
         *hasIncrementalIterationSpace = false;
      break;
//    case V_SgNotEqualOp: // Do we really want to allow this != operator ?
      break;
    default:
      return false;
  }
  // check the tested variable is the same as the loop index
  SgVarRefExp* testvar = isSgVarRefExp(SkipCasting(test->get_lhs_operand()));
  if (testvar == NULL)
    return false;
  if (testvar->get_symbol() != ivarname->get_symbol_from_symbol_table ())
    return false;
  //grab the upper bound
  ubast = test->get_rhs_operand();

  //3. Check the increment expression
  SgExpression* incr = fs->get_increment();
  SgVarRefExp* incr_var = NULL;
  switch (incr->variantT()) {
    case V_SgPlusAssignOp: //+=
    case V_SgMinusAssignOp://-=
      incr_var = isSgVarRefExp(SkipCasting(isSgBinaryOp(incr)->get_lhs_operand()));
      stepast = isSgBinaryOp(incr)->get_rhs_operand();
      break;
    case V_SgPlusPlusOp:   //++
    case V_SgMinusMinusOp:  //--
      incr_var = isSgVarRefExp(SkipCasting(isSgUnaryOp(incr)->get_operand()));
      stepast = buildIntVal(1); // will this dangling SgNode cause any problem?
      break;
    default:
      return false;
  }
  if (incr_var == NULL)
    return false;
  if (incr_var->get_symbol() != ivarname->get_symbol_from_symbol_table ())
    return false;

  // return loop information if requested
  if (ivar != NULL)
    *ivar = ivarname;
  if (lb != NULL)
    *lb = lbast;
  if (ub != NULL)
    *ub = ubast;
  if (step != NULL)
    *step = stepast;
  if (body != NULL) {
    *body = fs->get_loop_body();
  }
  return true;
}

//! Set the lower bound of a loop header
void SageInterface::setLoopLowerBound(SgNode* loop, SgExpression* lb)
{
  ROSE_ASSERT(loop != NULL);
  ROSE_ASSERT(lb != NULL);
  SgForStatement* forstmt = isSgForStatement(loop);
  SgFortranDo* dostmt = isSgFortranDo(loop);
  //  ROSE_ASSERT(forstmt!= NULL);

  if (forstmt != NULL)
  {
    // two cases: init_stmt is
    //       SgExprStatement (assignment) like i=0;
    //       SgVariableDeclaration int i =0 or
    Rose_STL_Container<SgNode* > testList = NodeQuery::querySubTree( *((forstmt->get_init_stmt()).begin()), V_SgAssignOp);
    if (testList.size()>0) // assignment statement
    {
      ROSE_ASSERT(testList.size()==1);// only handle the case of 1 statement, canonical form
      SgAssignOp * assignop = isSgAssignOp((*testList.begin()));
      ROSE_ASSERT(assignop);
      if( assignop->get_rhs_operand()->get_lvalue())
        lb->set_lvalue(true);
      assignop->set_rhs_operand(lb);
      lb->set_parent(assignop);
      //TODO what happens to the original rhs operand?
    }
    else // variable declaration case
    {
      // SgVariableDeclaration
      Rose_STL_Container<SgNode* > testList = NodeQuery::querySubTree( *((forstmt->get_init_stmt()).begin()),  V_SgAssignInitializer );
      ROSE_ASSERT(testList.size()==1);// only handle the case of 1 statement, canonical form
      SgAssignInitializer* init = isSgAssignInitializer((*testList.begin()));
      ROSE_ASSERT(init != NULL);
      init->set_operand(lb);
      lb->set_parent(init);
      //TODO what happens to the original rhs operand?
    }
  }
  else if (dostmt != NULL)
  {
    SgExpression* init = dostmt->get_initialization();
    ROSE_ASSERT (init != NULL);
    SgAssignOp * a_op = isSgAssignOp (init);
    ROSE_ASSERT (a_op!=NULL);
    a_op->set_rhs_operand(lb);
    lb->set_parent(a_op);
    //TODO delete the previous operand?
  }
  else
  {
    cerr<<"Error. SageInterface::setLoopLowerBound(), illegal loop type:"<< loop->class_name()<<endl;
    ROSE_ASSERT (false);
  }
}

//! Set the upper bound of a loop header,regardless the condition expression type.  for (i=lb; i op up, ...)
void SageInterface::setLoopUpperBound(SgNode* loop, SgExpression* ub)
{
  ROSE_ASSERT(loop != NULL);
  ROSE_ASSERT(ub != NULL);
  SgForStatement* forstmt = isSgForStatement(loop);
  //  ROSE_ASSERT(forstmt!= NULL);
  SgFortranDo* dostmt = isSgFortranDo(loop);
  if (forstmt != NULL)
  {
    // set upper bound expression
    SgBinaryOp * binop= isSgBinaryOp(isSgExprStatement(forstmt->get_test())->get_expression());
    ROSE_ASSERT(binop != NULL);
    binop->set_rhs_operand(ub);
    ub->set_parent(binop);
  }
  else if (dostmt != NULL)
  {
    dostmt->set_bound(ub);
    ub->set_parent(dostmt);
    //TODO delete the original bound expression
  }
  else
  {
    cerr<<"Error. SageInterface::setLoopUpperBound(), illegal loop type:"<< loop->class_name()<<endl;
    ROSE_ASSERT (false);
  }

}

//! Set the stride(step) of a loop 's incremental expression, regardless the expression types (i+=s; i= i+s, etc)
void SageInterface::setLoopStride(SgNode* loop, SgExpression* stride)
{
  ROSE_ASSERT(loop != NULL);
  ROSE_ASSERT(stride != NULL);
  SgForStatement* forstmt = isSgForStatement(loop);
  SgFortranDo * dostmt = isSgFortranDo (loop);
  // ROSE_ASSERT(forstmt!= NULL);
  if (dostmt != NULL)
  {
    dostmt->set_increment(stride);
    stride->set_parent(dostmt);
    //TODO delete original increment expression
  }
  else  if (forstmt != NULL)
  {
    // set stride expression
    // case 1: i++ change to i+=stride
    Rose_STL_Container<SgNode*> testList = NodeQuery::querySubTree( forstmt->get_increment(), V_SgPlusPlusOp);
    if (testList.size()>0)
    {
      ROSE_ASSERT(testList.size() == 1); // should have only one
      SgVarRefExp *loopvarexp = isSgVarRefExp(SageInterface::deepCopy
          (isSgPlusPlusOp( *testList.begin())->get_operand()));
      SgPlusAssignOp *plusassignop = buildPlusAssignOp(loopvarexp, stride);
      forstmt->set_increment(plusassignop);
    }

    // case 1.5: i-- also changed to i+=stride
    testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgMinusMinusOp);
    if (testList.size()>0)
    {
      ROSE_ASSERT(testList.size()==1);// should have only one
      SgVarRefExp *loopvarexp =isSgVarRefExp(SageInterface::deepCopy
          (isSgMinusMinusOp(*testList.begin())->get_operand()));
      SgPlusAssignOp *plusassignop = buildPlusAssignOp(loopvarexp, stride);
      forstmt->set_increment(plusassignop);
    }

    // case 2: i+=X
    testList = NodeQuery::querySubTree( forstmt->get_increment(), V_SgPlusAssignOp);
    if (testList.size()>0)
    {
      ROSE_ASSERT(testList.size()==1);// should have only one
      SgPlusAssignOp * assignop = isSgPlusAssignOp(*(testList.begin()));
      ROSE_ASSERT(assignop!=NULL);
      assignop->set_rhs_operand(stride);
    }

    // case 2.5: i-=X changed to i+=stride
    testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgMinusAssignOp);
    if (testList.size()>0)
    {
      ROSE_ASSERT(testList.size()==1);// should have only one
      SgVarRefExp *loopvarexp =isSgVarRefExp(SageInterface::deepCopy
          (isSgMinusAssignOp(*testList.begin())->get_lhs_operand()));
      SgExprStatement* exprstmt = isSgExprStatement((*testList.begin())->get_parent());
      ROSE_ASSERT(exprstmt !=NULL);
      SgPlusAssignOp *plusassignop = buildPlusAssignOp(loopvarexp, stride);
      exprstmt->set_expression(plusassignop);
    }

    // DQ (1/3/2007): I think this is a meaningless statement.
    testList.empty();
    // case 3: i=i + X or i =X +i  i
    // TODO; what if users use i*=,etc ??
    //      send out a warning: not canonical FOR/DO loop
    //      or do this in the real frontend. MUST conform to canonical form
    testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgAddOp);
    if (testList.size()>0)
    {
      ROSE_ASSERT(testList.size()==1);// should have only one ??
      // consider only the top first one
      SgAddOp * addop = isSgAddOp(*(testList.begin()));
      ROSE_ASSERT(addop!=NULL);
      string loopvar= (isSgVarRefExp(isSgAssignOp(addop->get_parent())->get_lhs_operand())->get_symbol()->get_name()).getString();
      if (isSgVarRefExp(addop->get_rhs_operand())!=NULL)
      {
        if ((isSgVarRefExp(addop->get_rhs_operand())->get_symbol()->get_name()).getString() ==loopvar)
          addop->set_lhs_operand(stride);
        else
          addop->set_rhs_operand(stride);
      }
      else
        addop->set_rhs_operand(stride);
    }

    // case 3.5: i=i - X
    testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgSubtractOp);
    if (testList.size()>0)
    {
      ROSE_ASSERT(testList.size()==1);// should have only one ??
      // consider only the top first one
      SgSubtractOp * subtractop = isSgSubtractOp(*(testList.begin()));
      ROSE_ASSERT(subtractop!=NULL);
      SgVarRefExp *loopvarexp =isSgVarRefExp(SageInterface::deepCopy
          (isSgSubtractOp(*testList.begin())->get_lhs_operand()));
      SgAssignOp *assignop = isSgAssignOp((*testList.begin())->get_parent());
      ROSE_ASSERT(assignop !=NULL);
      SgPlusAssignOp *plusassignop = buildPlusAssignOp(loopvarexp, stride);
      assignop->set_rhs_operand(plusassignop);
    }
  }
  else
  {
    cerr<<"Error. SageInterface::setLoopStride(), illegal loop type:"<< loop->class_name()<<endl;
    ROSE_ASSERT (false);

  }
}

//! Check if a SgNode _s is an assignment statement (any of =,+=,-=,&=,/=, ^=, etc)
//!
//! Return the left hand, right hand expressions and if the left hand variable is also being read. This code is from AstInterface::IsAssignment()
bool SageInterface::isAssignmentStatement(SgNode* s, SgExpression** lhs/*=NULL*/, SgExpression** rhs/*=NULL*/, bool* readlhs/*=NULL*/)
{
  SgExprStatement *n = isSgExprStatement(s);
  SgExpression *exp = (n != 0)? n->get_expression() : isSgExpression(s);
  if (exp != 0) {
    switch (exp->variantT()) {
      case V_SgPlusAssignOp:
      case V_SgMinusAssignOp:
      case V_SgAndAssignOp:
      case V_SgIorAssignOp:
      case V_SgMultAssignOp:
      case V_SgDivAssignOp:
      case V_SgModAssignOp:
      case V_SgXorAssignOp:
      case V_SgAssignOp:
        {
          SgBinaryOp* s2 = isSgBinaryOp(exp);
          if (lhs != 0)
            *lhs = s2->get_lhs_operand();
          if (rhs != 0) {
            SgExpression* init = s2->get_rhs_operand();
            if ( init->variantT() == V_SgAssignInitializer)
              init = isSgAssignInitializer(init)->get_operand();
            *rhs = init;
          }
          if (readlhs != 0)
            *readlhs = (exp->variantT() != V_SgAssignOp);
          return true;
        }
      default:
         return false;
    }
  }
  return false;
}


  void SageInterface::removeConsecutiveLabels(SgNode* top) {
   Rose_STL_Container<SgNode*> gotos = NodeQuery::querySubTree(top,V_SgGotoStatement);
   for (size_t i = 0; i < gotos.size(); ++i) {
     SgGotoStatement* gs = isSgGotoStatement(gotos[i]);
     SgLabelStatement* ls = gs->get_label();
     SgBasicBlock* lsParent = isSgBasicBlock(ls->get_parent());
     if (!lsParent) continue;
     SgStatementPtrList& bbStatements = lsParent->get_statements();
     size_t j = std::find(bbStatements.begin(), bbStatements.end(), ls)
  - bbStatements.begin();
     ROSE_ASSERT (j != bbStatements.size());     while (j <
  bbStatements.size() - 1 && isSgLabelStatement(bbStatements[j + 1])) {
     ++j;
     }
     gs->set_label(isSgLabelStatement(bbStatements[j]));
   }
  }

namespace SageInterface { // A few internal helper classes

class AndOpGenerator: public StatementGenerator
   {
     SgAndOp* op;

     public:
          AndOpGenerator(SgAndOp* op): op(op) {}

          virtual SgStatement* generate(SgExpression* lhs)
             {
               SgTreeCopy treeCopy;
               SgExpression* lhsCopy = isSgExpression(lhs->copy(treeCopy));
               ROSE_ASSERT (lhsCopy);
               SgIfStmt* tree =
                 SageBuilder::buildIfStmt(
                     SageBuilder::buildExprStatement(op->get_lhs_operand()),
                     SageBuilder::buildBasicBlock(
                       SageBuilder::buildAssignStatement(lhs, op->get_rhs_operand())),
                     SageBuilder::buildBasicBlock(
                       SageBuilder::buildAssignStatement(lhsCopy, SageBuilder::buildBoolValExp(false))));
               return tree;
             }
   };

class OrOpGenerator: public StatementGenerator
   {
     SgOrOp* op;

     public:
          OrOpGenerator(SgOrOp* op): op(op) {}

          virtual SgStatement* generate(SgExpression* lhs)
             {
               SgTreeCopy treeCopy;
               SgExpression* lhsCopy = isSgExpression(lhs->copy(treeCopy));
               ROSE_ASSERT (lhsCopy);
               SgIfStmt* tree =
                 SageBuilder::buildIfStmt(
                     SageBuilder::buildExprStatement(op->get_lhs_operand()),
                     SageBuilder::buildBasicBlock(
                       SageBuilder::buildAssignStatement(lhs, SageBuilder::buildBoolValExp(true))),
                     SageBuilder::buildBasicBlock(
                       SageBuilder::buildAssignStatement(lhsCopy, op->get_rhs_operand())));
               return tree;
             }
   };

class ConditionalExpGenerator: public StatementGenerator
   {
     SgConditionalExp* op;

     public:
          ConditionalExpGenerator(SgConditionalExp* op): op(op) {}

          virtual SgStatement* generate(SgExpression* lhs)
             {
               SgTreeCopy treeCopy;
               SgExpression* lhsCopy = isSgExpression(lhs->copy(treeCopy));
               ROSE_ASSERT (lhsCopy);
               SgIfStmt* tree =
                 SageBuilder::buildIfStmt(
                     SageBuilder::buildExprStatement(op->get_conditional_exp()),
                     SageBuilder::buildBasicBlock(
                       SageBuilder::buildAssignStatement(lhs, op->get_true_exp())),
                     SageBuilder::buildBasicBlock(
                       SageBuilder::buildAssignStatement(lhsCopy, op->get_false_exp())));
               return tree;
             }
   };

} // end of namespace for the helper classes

//! Merged from replaceExpressionWithStatement.C
SgAssignInitializer* SageInterface::splitExpression(SgExpression* from, string newName/* ="" */)
{
  ROSE_ASSERT(from != NULL);

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
  if (!SageInterface::isCopyConstructible(from->get_type())) {
    std::cerr << "Type " << from->get_type()->unparseToString() << " of expression " << from->unparseToString() << " is not copy constructible" << std::endl;
    ROSE_ASSERT (false);
  }

  assert (SageInterface::isCopyConstructible(from->get_type())); // How do we report errors?
  SgStatement* stmt = getStatementOfExpression(from);
  assert (stmt);
  if (!isSgForInitStatement(stmt->get_parent())) {
    //SageInterface::ensureBasicBlockAsParent(stmt);
    // no return value is accepted. Only the optional transformation matters
    if (isBodyStatement(stmt) && !isSgBasicBlock(stmt))
      makeSingleStatementBodyToBlock (stmt);
  }

  SgScopeStatement* parent = isSgScopeStatement(stmt->get_parent());
  // cout << "parent is a " << (parent ? parent->sage_class_name() : "NULL") << endl;
  if (!parent && isSgForInitStatement(stmt->get_parent()))
    parent = isSgScopeStatement(stmt->get_parent()->get_parent()->get_parent());
  assert (parent);
  // cout << "parent is a " << parent->sage_class_name() << endl;
  // cout << "parent is " << parent->unparseToString() << endl;
  // cout << "stmt is " << stmt->unparseToString() << endl;
  SgName varname = "rose_temp__";
  if (newName == "") {
    varname << ++SageInterface::gensym_counter;
  } else {
    varname = newName;
  }

  SgType* vartype = from->get_type();
  SgNode* fromparent = from->get_parent();
  vector<SgExpression*> ancestors;
  for (SgExpression *expr = from, *anc = isSgExpression(fromparent); anc != 0;
      expr = anc, anc = isSgExpression(anc->get_parent()))
  {
    if ((isSgAndOp(anc) && expr != isSgAndOp(anc)->get_lhs_operand()) ||
        (isSgOrOp(anc) && expr != isSgOrOp(anc)->get_lhs_operand()) ||
        (isSgConditionalExp(anc) && expr != isSgConditionalExp(anc)->get_conditional_exp()))
      ancestors.push_back(anc); // Closest first
  }
  // cout << "This expression to split has " << ancestors.size() << " ancestor(s)" << endl;
  for (vector<SgExpression*>::reverse_iterator ai = ancestors.rbegin(); ai != ancestors.rend(); ++ai)
  {
    StatementGenerator* gen;
    switch ((*ai)->variantT()) {
      case V_SgAndOp:
        gen = new AndOpGenerator(isSgAndOp(*ai)); break;
      case V_SgOrOp:
        gen = new OrOpGenerator(isSgOrOp(*ai)); break;
      case V_SgConditionalExp:
        gen = new ConditionalExpGenerator(isSgConditionalExp(*ai)); break;
      default: assert (!"Should not happen");
    }
    replaceExpressionWithStatement(*ai, gen);
    delete gen;
  } // for
  if (ancestors.size() != 0) {
    return splitExpression(from);
    // Need to recompute everything if there were ancestors
  }
  SgVariableDeclaration* vardecl = SageBuilder::buildVariableDeclaration(varname, vartype, NULL, parent);
  SgVariableSymbol* sym = SageInterface::getFirstVarSym(vardecl);
  ROSE_ASSERT (sym);
  SgInitializedName* initname = sym->get_declaration();
  ROSE_ASSERT (initname);
  SgVarRefExp* varref = SageBuilder::buildVarRefExp(sym);
  replaceExpressionWithExpression(from, varref);
  // std::cout << "Unparsed 3: " << fromparent->sage_class_name() << " --- " << fromparent->unparseToString() << endl;
  // cout << "From is a " << from->sage_class_name() << endl;
  SgAssignInitializer* ai = SageBuilder::buildAssignInitializer(from);
  initname->set_initializer(ai);
  ai->set_parent(initname);
  myStatementInsert(stmt, vardecl, true);
  // vardecl->set_parent(stmt->get_parent());
  // FixSgTree(vardecl);
  // FixSgTree(parent);
  return ai;

#else
  return NULL;
#endif

} //splitExpression()

  //! This generalizes the normal splitExpression to allow loop tests and
  void SageInterface::splitExpressionIntoBasicBlock(SgExpression* expr) {
   struct SplitStatementGenerator: public StatementGenerator {
     SgExpression* expr;
     virtual SgStatement* generate(SgExpression* answer) {
       using namespace SageBuilder;
       return buildBasicBlock(buildAssignStatement(answer, expr));
     }
   };
   SplitStatementGenerator gen;
   gen.expr = expr;
   replaceExpressionWithStatement(expr, &gen);
  }

  void SageInterface::removeLabeledGotos(SgNode* top) {
   Rose_STL_Container<SgNode*> gotos = NodeQuery::querySubTree(top,
  V_SgGotoStatement);
   map<SgLabelStatement*, SgLabelStatement*> labelsToReplace;   for
  (size_t i = 0; i < gotos.size(); ++i) {
     SgGotoStatement* gs = isSgGotoStatement(gotos[i]);
     SgBasicBlock* gsParent = isSgBasicBlock(gs->get_parent());
     if (!gsParent) continue;
     SgStatementPtrList& bbStatements = gsParent->get_statements();
     size_t j = std::find(bbStatements.begin(), bbStatements.end(), gs)
  - bbStatements.begin();
     ROSE_ASSERT (j != bbStatements.size());
     if (j == 0) continue;
     if (isSgLabelStatement(bbStatements[j - 1])) {
       labelsToReplace[isSgLabelStatement(bbStatements[j - 1])] =
  gs->get_label();
     }
   }
   for (size_t i = 0; i < gotos.size(); ++i) {
     SgGotoStatement* gs = isSgGotoStatement(gotos[i]);
     SgLabelStatement* oldLabel = gs->get_label();
     while (labelsToReplace.find(oldLabel) != labelsToReplace.end()) {
       oldLabel = labelsToReplace[oldLabel];
     }
     gs->set_label(oldLabel);
   }
  }

  bool SageInterface::isConstantTrue(SgExpression* e) {
  switch (e->variantT()) {
#ifdef _MSC_VER
  // DQ (11/28/2009): This fixes a warning in MSVC (likely p_value should be a "bool" instead of an "int").
    case V_SgBoolValExp: return (isSgBoolValExp(e)->get_value() != 0);
#else
    case V_SgBoolValExp: return (isSgBoolValExp(e)->get_value() == true);
#endif
    case V_SgIntVal: return isSgIntVal(e)->get_value() != 0;
    case V_SgCastExp: return isConstantTrue(isSgCastExp(e)->get_operand());
    case V_SgNotOp: return isConstantFalse(isSgNotOp(e)->get_operand());
    case V_SgAddressOfOp: return true;
    default: return false;
  }
  }

  bool SageInterface::isConstantFalse(SgExpression* e) {
    switch (e->variantT()) {
      case V_SgBoolValExp: return isSgBoolValExp(e)->get_value() == false;
      case V_SgIntVal: return isSgIntVal(e)->get_value() == 0;
      case V_SgCastExp: return
  isConstantFalse(isSgCastExp(e)->get_operand());
      case V_SgNotOp: return isConstantTrue(isSgNotOp(e)->get_operand());
      default: return false;
    }
  }

  bool SageInterface::isCallToParticularFunction(SgFunctionDeclaration* decl,
  SgExpression* e) {
    SgFunctionCallExp* fc = isSgFunctionCallExp(e);
    if (!fc) return false;
    SgFunctionRefExp* fr = isSgFunctionRefExp(fc->get_function());
    return fr->get_symbol()->get_declaration() == decl;
  }

  bool SageInterface::isCallToParticularFunction(const std::string& qualifiedName, size_t
  arity, SgExpression* e) {
    SgFunctionCallExp* fc = isSgFunctionCallExp(e);
    if (!fc) return false;
    SgFunctionRefExp* fr = isSgFunctionRefExp(fc->get_function());
    string name =
  fr->get_symbol()->get_declaration()->get_qualified_name().getString();
    return (name == qualifiedName &&
  fc->get_args()->get_expressions().size() == arity);
  }

  SgExpression* SageInterface::copyExpression(SgExpression* e)
  {
    ROSE_ASSERT( e != NULL);
    // We enforce that the source expression is fully attached to the AST
    // Too strict, the source expression can be just built and not attached.
    // Liao, 9/21/2009
//    ROSE_ASSERT( e->get_parent() != NULL);
    return deepCopy(e);
  }

  SgStatement* SageInterface::copyStatement(SgStatement* s)
  {
    return deepCopy(s);
  }

  //----------------- add into AST tree --------------------
  void SageInterface::appendExpression(SgExprListExp *expList, SgExpression* exp)
  {
    ROSE_ASSERT(expList);
    ROSE_ASSERT(exp);
    expList->append_expression(exp);
    exp->set_parent(expList);
  }

  void SageInterface::appendExpressionList(SgExprListExp *expList, const std::vector<SgExpression*>& exp)
  {
    for (size_t i = 0; i < exp.size(); ++i)
      appendExpression(expList, exp[i]);
  }

# if 0
  // DQ (11/25/2011): Moved to the header file so that it could be seen as a template function.

  // TODO consider the difference between C++ and Fortran
  // fixup the scope of arguments,no symbols for nondefining function declaration's arguments
template <class actualFunction>
void
// SageInterface::setParameterList(SgFunctionDeclaration * func,SgFunctionParameterList * paralist)
SageInterface::setParameterList(actualFunction * func,SgFunctionParameterList * paralist)
   {
  // DQ (11/25/2011): Modified this to be a templated function so that we can handle both 
  // SgFunctionDeclaration and SgTemplateFunctionDeclaration (and their associated member 
  // function derived classes).

     ROSE_ASSERT(func);
     ROSE_ASSERT(paralist);

  // Warn to users if a paralist is being shared
     if (paralist->get_parent() !=NULL)
        {
          cerr << "Waring! Setting a used SgFunctionParameterList to function: "
               << (func->get_name()).getString()<<endl
               << " Sharing parameter lists can corrupt symbol tables!"<<endl
               << " Please use deepCopy() to get an exclusive parameter list for each function declaration!"<<endl;
       // ROSE_ASSERT(false);
        }

  // Liao,2/5/2008  constructor of SgFunctionDeclaration will automatically generate SgFunctionParameterList, so be cautious when set new paralist!!
     if (func->get_parameterList() != NULL)
          if (func->get_parameterList() != paralist)
               delete func->get_parameterList();

     func->set_parameterList(paralist);
     paralist->set_parent(func);
   }
#endif

// static 
SgVariableSymbol* addArg(SgFunctionParameterList *paraList, SgInitializedName* initName, bool isPrepend)
   {
     ROSE_ASSERT(paraList != NULL);
     ROSE_ASSERT(initName != NULL);

     if (isPrepend == true)
          paraList->prepend_arg(initName);
       else
          paraList->append_arg(initName);

  // DQ (12/4/2011): If this is going to be set, make sure it will not be over written.
  // initName->set_parent(paraList);
     if (initName->get_parent() == NULL)
          initName->set_parent(paraList);

     ROSE_ASSERT(initName->get_parent() == paraList);

     SgFunctionDeclaration* func_decl= isSgFunctionDeclaration(paraList->get_parent());

  // DQ (12/4/2011): This will not be true for function parameter lists in SgTemplateFunctionDeclaration cases.
  // Also in typical use the SgFunctionDeclaration is not known yet so the parent is not set.
  // ROSE_ASSERT(paraList->get_parent() != NULL);
  // ROSE_ASSERT(func_decl != NULL);

     SgScopeStatement* scope = NULL;
     if (func_decl != NULL)
        {
          if ((func_decl->get_definingDeclaration()) == func_decl )
             {
            // defining function declaration, set scope and symbol table
               SgFunctionDefinition* func_def = func_decl->get_definition();
               ROSE_ASSERT(func_def);
               scope = func_def;
             }
            else
             {
            // nondefining declaration, set scope only, currently set to decl's scope, TODO
               scope = func_decl->get_scope();
             }
 
       // fix up declptr of the init name
          initName->set_declptr(func_decl);
        }

  // Liao 11/21/2012. Part of this function's work is to set scope for initName which is freshly created. So we should not assert
  // that initName already has a scope.
  //
  // DQ (12/4/2011): Added check...(fails for case of SgTypeEllipse).
  // ROSE_ASSERT(initName->get_scope() != NULL);
  //   ROSE_ASSERT(initName->get_scope() != NULL || isSgTypeEllipse(initName->get_type()) != NULL);

  // ROSE_ASSERT (scope); -- scope may not be set because the function declaration may not have been inserted anywhere

     initName->set_scope(scope);
     if (scope != NULL)
        {
          SgVariableSymbol* sym = isSgVariableSymbol(initName->get_symbol_from_symbol_table());
          if (sym == NULL)
             {
               sym = new SgVariableSymbol(initName);
               scope->insert_symbol(initName->get_name(), sym);
               sym->set_parent(scope->get_symbol_table());
             }
          return sym;
        }
       else
        {
          return NULL;
        }
   }

SgVariableSymbol* SageInterface::appendArg(SgFunctionParameterList *paraList, SgInitializedName* initName)
{
  return addArg(paraList,initName,false);
}

SgVariableSymbol* SageInterface::prependArg(SgFunctionParameterList *paraList, SgInitializedName* initName)
{
  return addArg(paraList,initName,true);
}

void SageInterface::setPragma(SgPragmaDeclaration* decl, SgPragma *pragma)
{
  ROSE_ASSERT(decl);
  ROSE_ASSERT(pragma);
  if (decl->get_pragma()!=NULL) delete (decl->get_pragma());
  decl->set_pragma(pragma);
  pragma->set_parent(decl);
}


//! SageInterface::appendStatement()
//TODO should we ensureBasicBlockAsScope(scope) ? like ensureBasicBlockAsParent(targetStmt);
//It might be well legal to append the first and only statement in a scope!
void SageInterface::appendStatement(SgStatement *stmt, SgScopeStatement* scope)
   {
  // DQ (4/3/2012): Simple globally visible function to call (used for debugging in ROSE).
     void testAstForUniqueNodes ( SgNode* node );

#if 0
     printf ("In SageInterface::appendStatement(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif

  // DQ (6/19/2012): Exit as a test...
  // ROSE_ASSERT(isSgClassDeclaration(stmt) == NULL);

     if (scope == NULL)
        {
#if 0
          printf ("   --- scope was not specified as input! \n");
#endif
          scope = SageBuilder::topScopeStack();
        }

     ROSE_ASSERT(stmt  != NULL);
     ROSE_ASSERT(scope != NULL);

#if 0
  // DQ (2/2/2010): This fails in the projects/OpenMP_Translator "make check" tests.
  // DQ (1/2/2010): Introducing test that are enforced at lower levels to catch errors as early as possible.
     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(stmt);
     if (declarationStatement != NULL)
        {
          ROSE_ASSERT(declarationStatement->get_firstNondefiningDeclaration() != NULL);
          ROSE_ASSERT(declarationStatement->get_definingDeclaration() != NULL);
        }
#endif

#if 0
    // This fix breaks other transformations.
    // It is better to do this explicitly as needed before calling appendStatement();
    // Liao 10/19/2010
    // In rare cases, we are moving the statement from its original scope to another scope
    // We have to remove it from its original scope before append it to the new scope
    SgNode* old_parent=  stmt->get_parent();
    if (old_parent)
       {
         removeStatement(stmt);
       }
#endif

#if 0
  // catch-all for statement fixup
  // Must fix it before insert it into the scope,
     fixStatement(stmt,scope);

  //-----------------------
  // append the statement finally
  // scope->append_statement (stmt);
     scope->insertStatementInScope(stmt,false);
     stmt->set_parent(scope); // needed?
#else
  // DQ (7/12/2012): Skip adding when this is a non-autonomous type declaration.
     bool skipAddingStatement = false;
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(stmt);
     if (classDeclaration != NULL)
        {
       // DQ (7/9/2012): We only skip the attachment of the class declaration to the scope if it is NOT and autonomous declaration.
#if 0
          if (classDeclaration->get_parent() != NULL)
             {
               printf ("Since the parent of this SgClassDeclaration is set, it must have been previously added to the AST: classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
             }
#endif

#if 1
       // DQ (6/9/2013): This is the original code...
          skipAddingStatement = (classDeclaration->get_isAutonomousDeclaration() == false);
#else
       // DQ (6/9/2013): We have no other way to detect if the SgClassDeclaration has previously been added to the AST (short of searching the AST directly).
       // This fails to add enough statements to the AST.
          skipAddingStatement = (classDeclaration->get_isAutonomousDeclaration() == false) || (classDeclaration->get_parent() != NULL);
#endif

       // DQ (6/26/2013): Don't just check for SgTemplateInstantiationDecl, but also for SgClassDeclaration.
       // DQ (6/9/2013): Check if this is a SgTemplateInstantiationDecl, since it might be appearing 
       // twice as a result of a template argument being instantiated and we only want to add it into 
       // the scope once.  This happens for test2013_198.C and I can't find a better solution.
       // if (isSgTemplateInstantiationDecl(classDeclaration) != NULL && scope->containsOnlyDeclarations() == true)
          if (classDeclaration != NULL && scope->containsOnlyDeclarations() == true)
             {
            // Check if this instnatiated template has already been added to the scope.

            // DQ (6/26/2013): This is a newer alternative to test for an existing statement in a scope.
            // const SgDeclarationStatementPtrList & declarationList = scope->getDeclarationList();
            // SgDeclarationStatementPtrList::const_iterator existingDeclaration = find(declarationList.begin(),declarationList.end(),classDeclaration);
            // if (existingDeclaration != declarationList.end())
               bool statementAlreadyExistsInScope = scope->statementExistsInScope(classDeclaration);
               if (statementAlreadyExistsInScope == true)
                  {
                    if (isSgTemplateInstantiationDecl(classDeclaration) != NULL)
                       {
#if 1
                         printf ("RARE ISSUE #1: In SageInterface::appendStatement(): This template instantiation has previously been added to the scope, so avoid doing so again (see test2013_198.C): classDeclaration = %p = %s scope = %p = %s \n",
                              classDeclaration,classDeclaration->class_name().c_str(),scope,scope->class_name().c_str());
#endif
                       }
#if 1
                      else
                       {
#if 1
                         printf ("RARE ISSUE #2: In SageInterface::appendStatement(): This statement has previously been added to the scope, so avoid doing so again (see rose.h): stmt = %p = %s scope = %p = %s \n",
                              stmt,stmt->class_name().c_str(),scope,scope->class_name().c_str());
#endif
                       }
#endif
                    skipAddingStatement = true;
                  }
             }
        }
       else
        {
          SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(stmt);
          if (enumDeclaration != NULL)
             {
            // DQ (7/12/2012): We only skip the attachment of the class declaration to the scope if it is NOT and autonomous declaration.
               skipAddingStatement = (enumDeclaration->get_isAutonomousDeclaration() == false);
             }
        }

#if 0
  // DQ (6/26/2013): This is an attempt to support better testing of possible redundant statements 
  // that would be inserted into the current scope. This is however a bit expensive so we are using
  // this as a way to also debug the new cases where this happens.
     bool statementAlreadyExistsInScope = scope->statementExistsInScope(stmt);
     if (skipAddingStatement == false && statementAlreadyExistsInScope == true)
        {
#if 1
          printf ("RARE ISSUE #2: In SageInterface::appendStatement(): This statement has previously been added to the scope, so avoid doing so again (see rose.h): stmt = %p = %s scope = %p = %s \n",
               stmt,stmt->class_name().c_str(),scope,scope->class_name().c_str());
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
          skipAddingStatement = true;
        }
#endif

#if 0
     printf ("   --- skipAddingStatement = %s \n",skipAddingStatement ? "true" : "false");
#endif

     if (skipAddingStatement == false)
        {
       // catch-all for statement fixup
       // Must fix it before insert it into the scope,
       // printf ("In appendStatementList(): Calling fixStatement() \n");
          fixStatement(stmt,scope);
       // printf ("DONE: In appendStatementList(): Calling fixStatement() \n");

       //-----------------------
       // append the statement finally
       // scope->append_statement (stmt);
#if 0
          printf ("   --- calling insertStatementInScope(): scope = %p = %s stmt = %p = %s \n",scope,scope->class_name().c_str(),stmt,stmt->class_name().c_str());
#endif
          scope->insertStatementInScope(stmt,false);

       // DQ (6/9/2013): Added comment only: This is needed because some declaration have to have the 
       // setting of there paremtn pointes delayed until now based on if they appear nested inside of 
       // other declarations (e.g. "typedef struct { int x; } y;").
          stmt->set_parent(scope); // needed?
        }
#endif

  // DQ (11/19/2012): If we are building the AST within the front-end then don't do this expensive 
  // fixup (we already set it properly in the AST construction within the frontend so we don't need 
  // this).  Also since this is only operating within a single scope it is likely too specific to C 
  // instead of addressing the details of C++ where functions can be placed in alternative scopes and 
  // use name qualification).
  // update the links after insertion!
  // if (isSgFunctionDeclaration(stmt))
     SourcePositionClassification scp = getSourcePositionClassificationMode();
     if ( (scp != e_sourcePositionFrontendConstruction) && (isSgFunctionDeclaration(stmt) != NULL) )
        {
          updateDefiningNondefiningLinks(isSgFunctionDeclaration(stmt),scope);
        }

#if 0
  // DQ (6/26/2013): Turn on this test for debugging ROSE compiling rose.h header file.
  // Note that this is a stronger AST subtree test and not the weaker test for a redundant 
  // statement in a single scope.
  // DQ (9/1/2012): this is a debugging mode that we need to more easily turn on and off.
  // DQ (4/3/2012): Added test to make sure that the pointers are unique.
     testAstForUniqueNodes(scope);
#else
  // printf ("In SageInterface::appendStatement(): Skipping test for unique statements in subtree \n");
#endif
   }


void
SageInterface::appendStatementList(const std::vector<SgStatement*>& stmts, SgScopeStatement* scope) 
   {
     for (size_t i = 0; i < stmts.size(); ++i)
        {
#if 0
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
          printf ("In appendStatementList(): stmts[i = %zu] = %p = %s \n",i,stmts[i],stmts[i]->class_name().c_str());
       // printf ("In appendStatementList(): stmts[i = %zu]->get_parent() = %p \n",i,stmts[i]->get_parent());
#endif
#endif
        appendStatement(stmts[i], scope); // Liao 5/15/2013, defer the logic of checking parent pointers to appendStatement()
#if 0        
          if (stmts[i]->get_parent() != NULL)
             {
#if 0
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("   --- In appendStatementList(): stmts[i = %zu] will be added to scope (because stmts[i]->get_parent() != NULL (= %p = %s) \n",i,stmts[i]->get_parent(),stmts[i]->get_parent()->class_name().c_str());
#endif
#endif
               appendStatement(stmts[i], scope);
             }
            else
             {
               printf ("   --- WARNING: In appendStatementList(): stmts[i = %zu] not added to scope (because stmts[i]->get_parent() == NULL) \n",i);
             }
#endif        
        }
   }

//!SageInterface::prependStatement()
void SageInterface::prependStatement(SgStatement *stmt, SgScopeStatement* scope)
   {
     ROSE_ASSERT (stmt != NULL);
     if (scope == NULL)
          scope = SageBuilder::topScopeStack();
     ROSE_ASSERT(scope != NULL);
  // TODO handle side effect like SageBuilder::appendStatement() does

  // Must fix it before insert it into the scope,
  // otherwise assertions in insertStatementInScope() would fail
     fixStatement(stmt,scope);

     scope->insertStatementInScope(stmt,true);
     stmt->set_parent(scope); // needed?

  // DQ (11/19/2012): If we are building the AST within the front-end then don't do this expensive 
  // fixup (we already set it properly in the AST construction within the frontend so we don't need 
  // this).  Also since this is only operating within a single scope it is likely too specific to C 
  // instead of addessing the details of C++ where functions can be placed in alternative scopes and 
  // use name qualification).
  // update the links after insertion!
  // if (isSgFunctionDeclaration(stmt))
     SourcePositionClassification scp = getSourcePositionClassificationMode();
     if ( (scp != e_sourcePositionFrontendConstruction) && (isSgFunctionDeclaration(stmt) != NULL) )
        {
          updateDefiningNondefiningLinks(isSgFunctionDeclaration(stmt),scope);
        }

   } // prependStatement()

void SageInterface::prependStatementList(const std::vector<SgStatement*>& stmts, SgScopeStatement* scope)
   {
     for (size_t i = stmts.size(); i > 0; --i)
        {
          prependStatement(stmts[i - 1], scope);
        }
   }

  //! Check if a scope statement has a simple children statement list (SgStatementPtrList)
  //! so insert additional statements under the scope is straightforward and unambiguous .
  //! for example, SgBasicBlock has a simple statement list while IfStmt does not.
bool  SageInterface::hasSimpleChildrenList (SgScopeStatement* scope)
{
  bool rt = false;
  ROSE_ASSERT (scope != NULL);
  switch (scope->variantT())
  {
    case V_SgBasicBlock:
    case V_SgClassDefinition:
    case V_SgFunctionDefinition:
    case V_SgGlobal:
    case V_SgNamespaceDefinitionStatement: //?
      rt = true;
      break;

     case V_SgAssociateStatement :
     case V_SgBlockDataStatement :
     case V_SgCatchOptionStmt:
     case V_SgDoWhileStmt:
     case V_SgForAllStatement:
     case V_SgForStatement:
     case V_SgFortranDo:
     case V_SgIfStmt:
     case V_SgSwitchStatement:
     case V_SgUpcForAllStatement:
     case V_SgWhileStmt:
      rt = false;
      break;

    default:
      cout<<"unrecognized or unhandled scope type for SageInterface::hasSimpleChildrenList() "<<endl;
    break;
  }
  return rt;
}

  //TODO handle more side effect like SageBuilder::append_statement() does
  //Merge myStatementInsert()
  // insert  SageInterface::insertStatement()
void SageInterface::insertStatement(SgStatement *targetStmt, SgStatement* newStmt, bool insertBefore, bool autoMovePreprocessingInfo /*= true */)
   {
     ROSE_ASSERT(targetStmt &&newStmt);
     ROSE_ASSERT(targetStmt != newStmt); // should not share statement nodes!
     SgNode* parent = targetStmt->get_parent();
     if (parent==NULL)
        {
          cerr<<"Empty parent pointer for target statement. May be caused by the wrong order of target and new statements in insertStatement(targetStmt, newStmt)"<<endl;
          ROSE_ASSERT(parent);
        }

#if 0
     printf ("In SageInterface::insertStatement(): insert newStmt = %p = %s before/after targetStmt = %p = %s \n",newStmt,newStmt->class_name().c_str(),targetStmt,targetStmt->class_name().c_str());
#endif

  // Liao 3/2/2012. The semantics of ensureBasicBlockAsParent() are messy. input targetStmt may be
  // returned as it is if it is already a basic block as a body of if/while/catch/ etc.
  // We now have single statement true/false body for IfStmt etc
  // However, IfStmt::insert_child() is ambiguous and not implemented
  // So we make SgBasicBlock out of the single statement and
  // essentially call SgBasicBlock::insert_child() instead.
  // TODO: add test cases for If, variable, variable/struct inside if, etc
    // parent = ensureBasicBlockAsParent(targetStmt);

  // must get the new scope after ensureBasicBlockAsParent ()
     SgScopeStatement* scope= targetStmt->get_scope();
     ROSE_ASSERT(scope);

     newStmt->set_parent(targetStmt->get_parent());
     fixStatement(newStmt,scope);

  // DQ (9/16/2010): Added assertion that appears to be required to be true.
  // However, if this is required to be true then what about statements in
  // SgStatementExpression IR nodes?
     ROSE_ASSERT(isSgStatement(parent) != NULL);

  // DQ (9/16/2010): Added support to move comments and CPP directives marked to
  // appear before the statement to be attached to the inserted statement (and marked
  // to appear before that statement).
     ROSE_ASSERT(targetStmt != NULL);
     AttachedPreprocessingInfoType* comments = targetStmt->getAttachedPreprocessingInfo();

   //TODO refactor this portion of code into a separate function
  // DQ (9/17/2010): Trying to eliminate failing case in OpenMP projects/OpenMP_Translator/tests/npb2.3-omp-c/LU/lu.c
  // I think that special rules apply to inserting a SgBasicBlock so disable comment reloation when inserting a SgBasicBlock.
  // if (comments != NULL && newStmt->getAttachedPreprocessingInfo() == NULL)
  // if (comments != NULL)
   if (autoMovePreprocessingInfo) // Do this only if automatically handling of preprocessing information is request by users
    {
     if (comments != NULL && isSgBasicBlock(newStmt) == NULL)
        {
          vector<int> captureList;
#if 0
          printf ("Found attached comments (at %p = %s, inserting %p = %s insertBefore = %s): comments->size() = %zu \n",
               targetStmt,targetStmt->class_name().c_str(),newStmt,newStmt->class_name().c_str(),insertBefore ? "true" : "false",comments->size());
#endif
       // DQ (9/17/2010): Assert that the new statement being inserted has no attached comments or CPP directives.
          if (newStmt->getAttachedPreprocessingInfo() != NULL && newStmt->getAttachedPreprocessingInfo()->empty() == false)
             {
            // If the inserted statment has attached comments or CPP directives then this is gets a little
            // bit more comple and we don't support that at present.
               printf ("Warning: at present statements being inserted should not have attached comments of CPP directives (could be a problem, but comment relocation is not disabled). \n");
             }
       // DQ (9/17/2010): commented out because it fails test in projects/OpenMP_Translator/for_firstprivate.c
          ROSE_ASSERT((newStmt->getAttachedPreprocessingInfo() == NULL) || (newStmt->getAttachedPreprocessingInfo() != NULL && newStmt->getAttachedPreprocessingInfo()->empty() == false));

          int commentIndex = 0;
          AttachedPreprocessingInfoType::iterator i;
          for (i = comments->begin(); i != comments->end(); i++)
             {
               ROSE_ASSERT ( (*i) != NULL );
#if 0
               printf ("          Attached Comment (relativePosition=%s): %s\n",
                    ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                    (*i)->getString().c_str());
               printf ("Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
               (*i)->get_file_info()->display("comment/directive location");
#endif
               PreprocessingInfo::RelativePositionType relativePosition = (insertBefore == true) ? PreprocessingInfo::before : PreprocessingInfo::after;
               if ((*i)->getRelativePosition() == relativePosition)
                  {
                 // accumulate into list
                    captureList.push_back(commentIndex);
                  }

               commentIndex++;
             }

       // printf ("captureList.size() = %zu \n",captureList.size());
          if (captureList.empty() == false)
             {
            // Remove these comments and/or CPP directives and put them into the previous statement (marked to be output after the statement).
            // SgStatement* surroundingStatement = (insertBefore == true) ? getPreviousStatement(targetStmt) : getNextStatement(targetStmt);
            // SgStatement* surroundingStatement = (insertBefore == true) ? newStmt : newStmt;
               SgStatement* surroundingStatement = newStmt;
               ROSE_ASSERT(surroundingStatement != targetStmt);
               ROSE_ASSERT(surroundingStatement != NULL);
#if 0
               if (surroundingStatement == NULL)
                  {
                 // printf ("Warning: the surrounding statement for insertBefore = %s is NULL (so use the newStmt) \n",insertBefore ? "true" : "false");
                    surroundingStatement = (insertBefore == true) ? newStmt : newStmt;
                  }
#endif
            // Now add the entries from the captureList to the surroundingStatement and remove them from the targetStmt.
            // printf ("This is a valid surrounding statement = %s for insertBefore = %s \n",surroundingStatement->class_name().c_str(),insertBefore ? "true" : "false");
               vector<int>::iterator j = captureList.begin();
               while (j != captureList.end())
                  {
                 // Add the captured comments to the new statement. Likely we need to make sure that the order is preserved.
                 // printf ("Attaching comments to newStmt = %p = %s \n",newStmt,newStmt->class_name().c_str());
                    newStmt->addToAttachedPreprocessingInfo((*comments)[*j]);

                 // Remove them from the targetStmt. (set them to NULL and then remove them in a separate step).
                 // printf ("Removing entry from comments list on targetStmt = %p = %s \n",targetStmt,targetStmt->class_name().c_str());
                    (*comments)[*j] = NULL;

                    j++;
                  }

            // Now remove each NULL entries in the comments vector.
            // Because of iterator invalidation we must reset the iterators after each call to erase (I think).
               for (size_t n = 0; n < captureList.size(); n++)
                  {
                    AttachedPreprocessingInfoType::iterator k = comments->begin();
                    while (k != comments->end())
                       {
                      // Only modify the list once per iteration over the captureList
                         if (*k == NULL)
                            {
                              comments->erase(k);
                                                          break;
                            }
                             k++;
                       }
                  }
             }
        }
       else
        {
       // printf ("No attached comments (at %p of type: %s): \n",targetStmt,targetStmt->class_name().c_str());
       // DQ (9/17/2010): Trying to eliminate failing case in OpenMP projects/OpenMP_Translator/tests/npb2.3-omp-c/LU/lu.c
       // I think that special rules apply to inserting a SgBasicBlock so disable comment relocation when inserting a SgBasicBlock.
          if (comments != NULL)
             {
               printf ("Warning: special rules appear to apply to the insertion of a SgBasicBlock which has attached comments and/or CPP directives (comment relocation disabled). \n");
             }
        }
     } // end if autoMovePreprocessingInfo


     if (isSgIfStmt(parent))
        {
          if (isSgIfStmt(parent)->get_conditional()==targetStmt)
               insertStatement(isSgStatement(parent),newStmt,insertBefore);
            else
               if (isSgIfStmt(parent)->get_true_body()==targetStmt)
                  {
                    // Liao 3/2/2012
                    // We have some choices:
                    // 1) if the targeStmt is a basic block, we can append/prepend the new stmt
                    // within the targetStmt. But this is not the exact semantics of insertStatment. It will break the outliner.
                    // Since the targetStmt will have new content inside of it, which is not the semantics of
                    // inserting anything before/or after it.
                    // 2) always insert a padding basic block between parent and targetStmt
                    //   and we can legally insert before/after the target statement within the
                    //   padding basic block.
                    //TODO: this insertion of padding basic block should ideally go into some AST normalization phase
                    // so the transformation function (insertStatement) only does what it means to do, no more and no less.
                    SgBasicBlock* newparent = buildBasicBlock (targetStmt);
                    isSgIfStmt(parent)->set_true_body(newparent);
                    newparent->set_parent(parent);
                    insertStatement(targetStmt, newStmt,insertBefore);
                  }
                 else
                    if (isSgIfStmt(parent)->get_false_body()==targetStmt)
                    {

                      // ensureBasicBlockAsParent(targetStmt);
                      SgBasicBlock* newparent = buildBasicBlock (targetStmt);
                      isSgIfStmt(parent)->set_false_body(newparent);
                      newparent->set_parent(parent);
                      insertStatement(targetStmt, newStmt,insertBefore);
                      //insertStatement(isSgStatement(parent),newStmt,insertBefore);
                    }
        }
       else
          if (isSgWhileStmt(parent))
             {
               if (isSgWhileStmt(parent)->get_condition()==targetStmt)
                    insertStatement(isSgStatement(parent),newStmt,insertBefore);
                 else
                    if (isSgWhileStmt(parent)->get_body()==targetStmt)
                    {
                      SgBasicBlock* newparent = buildBasicBlock (targetStmt);
                      isSgWhileStmt(parent)->set_body(newparent);
                      newparent->set_parent(parent);
                      insertStatement(targetStmt, newStmt,insertBefore);
                      // ensureBasicBlockAsParent(targetStmt);
                     // insertStatement(isSgStatement(parent),newStmt,insertBefore);
                    }
             }
            else
               if (isSgDoWhileStmt(parent))
                  {
                    if (isSgDoWhileStmt(parent)->get_condition()==targetStmt)
                         insertStatement(isSgStatement(parent),newStmt,insertBefore);
                      else
                         if (isSgDoWhileStmt(parent)->get_body()==targetStmt)
                         {

                           SgBasicBlock* newparent = buildBasicBlock (targetStmt);
                           isSgDoWhileStmt(parent)->set_body(newparent);
                           newparent->set_parent(parent);
                           insertStatement(targetStmt, newStmt,insertBefore);
                           // ensureBasicBlockAsParent(targetStmt);
                           //   insertStatement(isSgStatement(parent),newStmt,insertBefore);
                         }
                  }
                 else
                    if (isSgForStatement(parent))
                       {
                         if (isSgForStatement(parent)->get_loop_body()==targetStmt)
                         {
                           SgBasicBlock* newparent = buildBasicBlock (targetStmt);
                           isSgForStatement(parent)->set_loop_body(newparent);
                           newparent->set_parent(parent);
                           insertStatement(targetStmt, newStmt,insertBefore);
                           // ensureBasicBlockAsParent(targetStmt);
                           //   insertStatement(isSgStatement(parent),newStmt,insertBefore);
                         }
                           else
                              if (isSgForStatement(parent)->get_test()==targetStmt)
                                 {
                                   insertStatement(isSgStatement(parent),newStmt,insertBefore);
                                 }
                       }
                      else // \pp (2/24/2011) added support for UpcForAll
                        if (SgUpcForAllStatement* p = isSgUpcForAllStatement(parent))
                        {
                          const bool stmt_present = (  p->get_loop_body() == targetStmt
                                                    || p->get_test() == targetStmt
                                                    );

                          // \pp \todo what if !stmt_present
                          ROSE_ASSERT(stmt_present);
                          insertStatement(p, newStmt, insertBefore);
                        }
                       else if (SgOmpBodyStatement * p = isSgOmpBodyStatement (parent))
                       {
                         SgBasicBlock* newparent = buildBasicBlock (targetStmt);
                         p->set_body(newparent);
                         newparent->set_parent(parent);
                         insertStatement(targetStmt, newStmt,insertBefore);
                       }
                        else
                           isSgStatement(parent)->insert_statement(targetStmt,newStmt,insertBefore);

  // DQ (11/19/2012): If we are building the AST within the front-end then don't do this expensive 
  // fixup (we already set it properly in the AST construction within the frontend so we don't need 
  // this).  Also since this is only operating within a single scope it is likely too specific to C 
  // instead of addessing the details of C++ where functions can be placed in alternative scopes and 
  // use name qualification).
  // update the links after insertion!
  // if (isSgFunctionDeclaration(newStmt))
     SourcePositionClassification scp = getSourcePositionClassificationMode();
     if ( (scp != e_sourcePositionFrontendConstruction) && (isSgFunctionDeclaration(newStmt) != NULL) )
        {
          updateDefiningNondefiningLinks(isSgFunctionDeclaration(newStmt),scope);
        }
   }

  void SageInterface::insertStatementList(SgStatement *targetStmt, const std::vector<SgStatement*>& newStmts, bool insertBefore) {
    if (insertBefore) {
      for (size_t i = 0; i < newStmts.size(); ++i) {
        insertStatementBefore(targetStmt, newStmts[i]);
      }
    } else {
      for (size_t i = newStmts.size(); i > 0; --i) {
        insertStatementAfter(targetStmt, newStmts[i - 1]);
      }
    }
  }

  void SageInterface::insertStatementAfter(SgStatement *targetStmt, SgStatement* newStmt, bool autoMovePreprocessingInfo /*= true*/)
  {
    insertStatement(targetStmt,newStmt,false, autoMovePreprocessingInfo);
  }

  void SageInterface::insertStatementListAfter(SgStatement *targetStmt, const std::vector<SgStatement*>& newStmts)
  {
    insertStatementList(targetStmt,newStmts,false);
  }

  //! Insert a statement after the last declaration within a scope. The statement will be prepended to the scope if there is no declaration statement found
  void SageInterface::insertStatementAfterLastDeclaration(SgStatement* stmt, SgScopeStatement* scope)
  {
    ROSE_ASSERT (stmt != NULL);
    ROSE_ASSERT (scope != NULL);
    // Insert to be the declaration after current declaration sequence, if any
    SgStatement* l_stmt = findLastDeclarationStatement (scope);
    if (l_stmt)
      insertStatementAfter(l_stmt,stmt);
    else
      prependStatement(stmt, scope);
  }

  //! Insert a list of statements after the last declaration within a scope. The statement will be prepended to the scope if there is no declaration statement found
  void SageInterface::insertStatementAfterLastDeclaration(std::vector<SgStatement*> stmt_list, SgScopeStatement* scope)
  {
    ROSE_ASSERT (scope != NULL);
    vector <SgStatement* >::iterator iter;
    SgStatement* prev_stmt = NULL;
    for (iter= stmt_list.begin(); iter != stmt_list.end(); iter++)
    {
      if (iter == stmt_list.begin())
      {
        insertStatementAfterLastDeclaration (*iter, scope);
      }
      else
      {
        ROSE_ASSERT (prev_stmt != NULL);
        insertStatementAfter (prev_stmt, *iter);
      }
      prev_stmt = *iter;
    }
  }

void SageInterface::insertStatementBeforeFirstNonDeclaration(SgStatement *newStmt, SgScopeStatement *scope,
                                                             bool movePreprocessingInfo)
{
    ROSE_ASSERT(newStmt!=NULL);
    ROSE_ASSERT(scope!=NULL);
    BOOST_FOREACH (SgStatement *targetStmt, scope->generateStatementList()) {
        if (!isSgDeclarationStatement(targetStmt)) {
            insertStatementBefore(targetStmt, newStmt, movePreprocessingInfo);
            return;
        }
    }
    appendStatement(newStmt, scope);
}

void SageInterface::insertStatementListBeforeFirstNonDeclaration(const std::vector<SgStatement*> &newStmts,
                                                                 SgScopeStatement *scope)
{
    ROSE_ASSERT(scope!=NULL);
    BOOST_FOREACH (SgStatement *targetStmt, scope->generateStatementList()) {
        if (!isSgDeclarationStatement(targetStmt)) {
            insertStatementListBefore(targetStmt, newStmts);
            return;
        }
    }
    appendStatementList(newStmts, scope);
}

  void SageInterface::insertStatementBefore(SgStatement *targetStmt, SgStatement* newStmt, bool autoMovePreprocessingInfo /*= true */)
  {
    insertStatement(targetStmt,newStmt,true, autoMovePreprocessingInfo);
  }

  void SageInterface::insertStatementListBefore(SgStatement *targetStmt, const std::vector<SgStatement*>& newStmts)
  {
    insertStatementList(targetStmt,newStmts,true);
  }

  //a wrapper for set_expression(), set_operand(), set_operand_exp() etc
  // special concern for lvalue, parent,
  // todo: warning overwritting existing operands
  void SageInterface::setOperand(SgExpression* target, SgExpression* operand)
  {
    ROSE_ASSERT(target);
    ROSE_ASSERT(operand);
    ROSE_ASSERT(target!=operand);
    switch (target->variantT())
    {
      case V_SgActualArgumentExpression:
        isSgActualArgumentExpression(target)->set_expression(operand);
        break;
      case V_SgAsmOp:
        isSgAsmOp(target)->set_expression(operand);
        break;
      case V_SgSizeOfOp:
        isSgSizeOfOp(target)->set_operand_expr(operand);
        break;
      case V_SgTypeIdOp:
        isSgTypeIdOp(target)->set_operand_expr(operand);
        break;
      case V_SgVarArgOp:
        isSgVarArgOp(target)->set_operand_expr(operand);
        break;
      case V_SgVarArgStartOneOperandOp:
        isSgVarArgStartOneOperandOp(target)->set_operand_expr(operand);
        break;
      default:
        if (isSgUnaryOp(target)!=NULL) isSgUnaryOp(target)->set_operand_i(operand);
        else
          {
            cout<<"SageInterface::setOperand(): unhandled case for target expression of type "
                <<target->class_name()<<endl;
            ROSE_ASSERT(false);
          }
    }// end switch
    operand->set_parent(target);
    markLhsValues(target);
  }

  // binary and SgVarArgCopyOp, SgVarArgStartOp
  void SageInterface::setLhsOperand(SgExpression* target, SgExpression* lhs)
  {
    ROSE_ASSERT(target);
    ROSE_ASSERT(lhs);
    ROSE_ASSERT(target!=lhs);
    bool hasrhs = false;

    SgVarArgCopyOp* varargcopy = isSgVarArgCopyOp(target);
    SgVarArgStartOp* varargstart = isSgVarArgStartOp(target);
    SgBinaryOp* binary = isSgBinaryOp(target);

    if (varargcopy!=NULL)
    {
      varargcopy->set_lhs_operand(lhs);
       if( varargcopy->get_rhs_operand()!=NULL) hasrhs= true;
    }
    else if(varargstart!=NULL)
    {
      varargstart->set_lhs_operand(lhs);
      if( varargstart->get_rhs_operand()!=NULL) hasrhs= true;
    }
    else if(binary!=NULL)
    {
      binary->set_lhs_operand(lhs);
      if( binary->get_rhs_operand()!=NULL) hasrhs= true;
    }
    else
    {
      cout<<"SageInterface::setLhsOperand(): unhandled case for target expression of type "
                <<target->class_name()<<endl;
      ROSE_ASSERT(false);
    }
    lhs->set_parent(target);
// only when both lhs and rhs are available, can we set lvalue
// there is assertion(rhs!=NULL) in markLhsValues()
   if (hasrhs)
      markLhsValues(target);
  }

  void SageInterface::setRhsOperand(SgExpression* target, SgExpression* rhs)
  {
    ROSE_ASSERT(target);
    ROSE_ASSERT(rhs);
    ROSE_ASSERT(target!=rhs);
    bool haslhs = false;

    SgVarArgCopyOp* varargcopy = isSgVarArgCopyOp(target);
    SgVarArgStartOp* varargstart = isSgVarArgStartOp(target);
    SgBinaryOp* binary = isSgBinaryOp(target);

    if (varargcopy!=NULL)
    {
       varargcopy->set_rhs_operand(rhs);
       if( varargcopy->get_lhs_operand()!=NULL) haslhs= true;
    }
    else if(varargstart!=NULL)
    {
      varargstart->set_rhs_operand(rhs);
      if( varargstart->get_lhs_operand()!=NULL) haslhs= true;
    }
    else if(binary!=NULL)
    {
      binary->set_rhs_operand(rhs);
      if( binary->get_lhs_operand()!=NULL) haslhs= true;
    }
    else
    {
      cout<<"SageInterface::setRhsOperand(): unhandled case for target expression of type "
                <<target->class_name()<<endl;
      ROSE_ASSERT(false);
    }
    rhs->set_parent(target);
// only when both lhs and rhs are available, can we set lvalue
   if (haslhs)
      markLhsValues(target);
  }

// DQ (1/25/2010): Added to simplify handling of directories (e.g. for code generation).
void SageInterface::moveToSubdirectory ( std::string directoryName, SgFile* file )
   {
  // This support makes use of the new SgDirectory IR node.  It causes the unparser to
  // generate a subdirectory and unparse the file into the subdirectory.  It works
  // by internally calling the system function "system()" to call "mkdir directoryName"
  // and then chdir()" to change the current directory.  These steps are handled by the
  // unparser.

  // This function just does the transformation to insert a SgDirectory IR node between
  // the referenced SgFile and it's project (fixing up parents and file lists etc.).

  // Add a directory and unparse the code (to the new directory)
     SgDirectory* directory = new SgDirectory(directoryName);

     SgFileList* parentFileList = isSgFileList(file->get_parent());
     ROSE_ASSERT(parentFileList != NULL);
     directory->set_parent(file->get_parent());

     SgProject* project           = NULL;
     SgDirectory* parentDirectory = isSgDirectory(parentFileList->get_parent());

     if (parentDirectory != NULL)
        {
       // Add a directory to the list in the SgDirectory node.
          parentDirectory->get_directoryList()->get_listOfDirectories().push_back(directory);

       // Erase the reference to the file in the project's file list.
       // parentDirectory->get_fileList().erase(find(parentDirectory->get_fileList().begin(),parentDirectory->get_fileList().end(),file));
        }
       else
        {
          project = isSgProject(parentFileList->get_parent());
          ROSE_ASSERT(project != NULL);

       // Add a directory to the list in the SgProject node.
          project->get_directoryList()->get_listOfDirectories().push_back(directory);

       // Erase the reference to the file in the project's file list.
       // project->get_fileList().erase(find(project->get_fileList().begin(),project->get_fileList().end(),file));
        }

  // Put the file into the new directory.
     directory->get_fileList()->get_listOfFiles().push_back(file);

  // Erase the reference to the file in the project's file list.
     parentFileList->get_listOfFiles().erase(find(parentFileList->get_listOfFiles().begin(),parentFileList->get_listOfFiles().end(),file));

     file->set_parent(directory);
}


//------------------------- AST repair----------------------------
//----------------------------------------------------------------
void SageInterface::fixStructDeclaration(SgClassDeclaration* structDecl, SgScopeStatement* scope)
   {
     ROSE_ASSERT(structDecl != NULL);
     ROSE_ASSERT(scope != NULL);
     SgClassDeclaration* nondefdecl = isSgClassDeclaration(structDecl->get_firstNondefiningDeclaration());
     ROSE_ASSERT(nondefdecl != NULL);

  // ROSE_ASSERT(structDecl->get_definingDeclaration() != NULL);
     SgClassDeclaration* defdecl = isSgClassDeclaration(structDecl->get_definingDeclaration());
  // ROSE_ASSERT(defdecl != NULL);

  // Liao, 9/2/2009
  // fixup missing scope when bottomup AST building is used
     if (structDecl->get_scope() == NULL)
          structDecl->set_scope(scope);
     if (nondefdecl->get_scope() == NULL)
          nondefdecl->set_scope(scope);

#if 0
     if (structDecl->get_parent() == NULL)
          structDecl->set_parent(scope);
     if (nondefdecl->get_parent() == NULL)
          nondefdecl->set_parent(scope);
#else
  // printf ("*** WARNING: In SageInterface::fixStructDeclaration(): Commented out the setting of the parent (of input class declaration and the nondefining declaration) to be the same as the scope (set only if NULL) \n");

  // DQ (7/21/2012): Can we assert this here? NO!
  // ROSE_ASSERT(structDecl->get_parent() == NULL);
  // ROSE_ASSERT(nondefdecl->get_parent() == NULL);
#endif

     SgName name = structDecl->get_name();

  // This is rare case (translation error) when scope->lookup_class_symbol(name) will find something
  // but nondefdecl->get_symbol_from_symbol_table() returns NULL
  // But symbols are associated with nondefining declarations whenever possible
  // and AST consistent check will check the nondefining declarations first
  // Liao, 9/2/2009
  // SgClassSymbol* mysymbol = scope->lookup_class_symbol(name);
     SgClassSymbol* mysymbol = isSgClassSymbol(nondefdecl->get_symbol_from_symbol_table());

  // DQ (3/14/2014): This is false for a copy of a class declaration being inserted into the AST.
  // DQ (9/4/2012): I want to assert this for the new EDG/ROSE connection code (at least).
  // ROSE_ASSERT(mysymbol != NULL);

     if (mysymbol == NULL)
        {
          printf ("Note: SageInterface::fixStructDeclaration(): structDecl = %p nondefdecl = %p (mysymbol == NULL) \n",structDecl,nondefdecl);

       // DQ (12/3/2011): This will be an error for C++ if the scope of the statment is different from the scope where it is located structurally...
       // DQ (12/4/2011): Only generate symbols and set the scope if this is the correct scope.
          ROSE_ASSERT(structDecl->get_scope() != NULL);

          if (scope == structDecl->get_scope())
             {
               mysymbol = new SgClassSymbol(nondefdecl);
               ROSE_ASSERT(mysymbol);

            // I need to check the rest of these functions.
               printf ("############## DANGER:DANGER:DANGER ################\n");

               printf ("In SageInterface::fixStructDeclaration(): Adding class symbol to scope = %p = %s \n",scope,scope->class_name().c_str());
               scope->insert_symbol(name, mysymbol);

            // ROSE_ASSERT(defdecl != NULL);
               if (defdecl)
                    defdecl->set_scope(scope);
               nondefdecl->set_scope(scope);

               printf ("*** WARNING: In SageInterface::fixStructDeclaration(): (mysymbol == NULL) Commented out the setting of the parent to be the same as the scope \n");
#if 0
               if (defdecl)
                    defdecl->set_parent(scope);
               nondefdecl->set_parent(scope);
#endif
             }
            else
             {
               printf ("In SageInterface::fixStructDeclaration(): (mysymbol == NULL) Skipped building an associated symbol! \n");
             }
        }

  // DQ (9/4/2012): I want to assert this for the new EDG/ROSE connection code (at least).
     ROSE_ASSERT(nondefdecl->get_type() != NULL);

  // DQ (9/4/2012): This is a sign that the pointer to the type was deleted.
     ROSE_ASSERT(nondefdecl->get_type()->variantT() != V_SgNode);

  // DQ (9/4/2012): This should be a SgClassType IR node.
     ROSE_ASSERT(isSgClassType(nondefdecl->get_type()) != NULL);

  // fixup SgClassType, which is associated with the first non-defining declaration only
  // and the other declarations share it.
     if (nondefdecl->get_type() == NULL)
        {
          nondefdecl->set_type(SgClassType::createType(nondefdecl));
        }
     ROSE_ASSERT (nondefdecl->get_type() != NULL);

  // DQ (9/4/2012): If defDecl != NULL, I want to assert this for the new EDG/ROSE connection code (at least).
     if (defdecl != NULL)
        {
       // DQ (9/4/2012): This is a sign that the pointer to the type was deleted.
          ROSE_ASSERT(defdecl->get_type()->variantT() != V_SgNode);

       // DQ (9/4/2012): This should be a SgClassType IR node.
          ROSE_ASSERT(isSgClassType(defdecl->get_type()) != NULL);
        }

  // ROSE_ASSERT(defdecl != NULL);
     if (defdecl != NULL)
        {
       // DQ (9/4/2012): If defDecl != NULL, I want to assert this for the new EDG/ROSE connection code (at least).
          ROSE_ASSERT(defdecl->get_type() != NULL);
          if (defdecl->get_type() != nondefdecl->get_type())
             {
               printf ("ERROR: defdecl->get_type() != nondefdecl->get_type(): what are these: \n");
               printf ("   defdecl->get_type()    = %p = %s \n",defdecl   ->get_type(),defdecl   ->get_type()->class_name().c_str());
               SgNamedType* namedType_definingDecl = isSgNamedType(defdecl->get_type());
               if (namedType_definingDecl != NULL)
                  {
                    printf ("namedType_definingDecl->get_declaration() = %p = %s \n",namedType_definingDecl->get_declaration(),namedType_definingDecl->get_declaration()->class_name().c_str());
                  }
               printf ("   nondefdecl->get_type() = %p = %s \n",nondefdecl->get_type(),nondefdecl->get_type()->class_name().c_str());
               SgNamedType* namedType_nondefiningDecl = isSgNamedType(nondefdecl->get_type());
               if (namedType_nondefiningDecl != NULL)
                  {
                    printf ("namedType_nondefiningDecl->get_declaration() = %p = %s \n",namedType_nondefiningDecl->get_declaration(),namedType_nondefiningDecl->get_declaration()->class_name().c_str());
                  }
             }
          ROSE_ASSERT(defdecl->get_type() == nondefdecl->get_type());

          if (defdecl->get_type() != nondefdecl->get_type())
             {
               if (defdecl->get_type())
                  {
                    printf ("WARNING: In SageInterface::fixStructDeclaration(): skipped calling delete on (defdecl = %p = %s) defdecl->get_type() = %p = %s \n",defdecl,defdecl->class_name().c_str(),defdecl->get_type(),defdecl->get_type()->class_name().c_str());
                 // delete defdecl->get_type();
                  }
               defdecl->set_type(nondefdecl->get_type());
             }
          ROSE_ASSERT (defdecl->get_type() != NULL);
          ROSE_ASSERT (defdecl->get_type() == nondefdecl->get_type());
        }
   }


void SageInterface::fixClassDeclaration(SgClassDeclaration* classDecl, SgScopeStatement* scope)
  {
    fixStructDeclaration(classDecl,scope);
  }


void SageInterface::fixNamespaceDeclaration(SgNamespaceDeclarationStatement* structDecl, SgScopeStatement* scope)
   {
     ROSE_ASSERT(structDecl);
     ROSE_ASSERT(scope);
     SgNamespaceDeclarationStatement* nondefdecl = isSgNamespaceDeclarationStatement(structDecl->get_firstNondefiningDeclaration());
     ROSE_ASSERT(nondefdecl);
  // Liao, 9/2/2009
  // fixup missing scope when bottomup AST building is used
     if (structDecl->get_parent() == NULL)
          structDecl->set_parent(scope);
     if (nondefdecl->get_parent() == NULL)
          nondefdecl->set_parent(scope);

  // tps : (09/03/2009) Namespace should not have a scope
    /*
    if (structDecl->get_scope() == NULL)
      structDecl->set_scope(scope);
    if (nondefdecl->get_scope() == NULL)
      nondefdecl->set_scope(scope);
    */

     SgName name= structDecl->get_name();
  // SgNamespaceSymbol* mysymbol = scope->lookup_namespace_symbol(name);
     SgNamespaceSymbol* mysymbol = isSgNamespaceSymbol(nondefdecl->get_symbol_from_symbol_table());
     if (mysymbol==NULL)
        {
       // DQ (12/4/2011): This code is modified to try to only insert the symbol into the correct scope.  It used to
       // just insert the symbol into whatever scope structureally held the declaration (not good enough for C++).
          if (scope == structDecl->get_scope())
             {
               mysymbol = new SgNamespaceSymbol(name,nondefdecl);
               ROSE_ASSERT(mysymbol);

               printf ("In SageInterface::fixNamespaceDeclaration(): inserting namespace symbol into scope = %p = %s \n",scope,scope->class_name().c_str());
               scope->insert_symbol(name, mysymbol);

               SgNamespaceDeclarationStatement* defdecl = isSgNamespaceDeclarationStatement(structDecl->get_definingDeclaration());
               ROSE_ASSERT(defdecl);
               defdecl->set_scope(scope);
               nondefdecl->set_scope(scope);

               defdecl->set_parent(scope);
               nondefdecl->set_parent(scope);
             }
        }
   }

void SageInterface::fixVariableDeclaration(SgVariableDeclaration* varDecl, SgScopeStatement* scope)
   {
     ROSE_ASSERT(varDecl != NULL);
     ROSE_ASSERT(scope   != NULL);

     SgInitializedNamePtrList namelist = varDecl->get_variables();

  // printf ("In SageInterface::fixVariableDeclaration(): Is this a recursive call! \n");

#if 0
     printf ("In SageInterface::fixVariableDeclaration(): varDecl = %p scope = %p = %s \n",varDecl,scope,scope->class_name().c_str());
#endif

#if 0
  // DQ (11/19/2011): This is dangerous code since for C++ the declarations can have different scopes.
  // But it is required to terminate some programs (now fixed by avoinding self reference, also put
  // assertion into symbol table handling to detect such infinite loops.

  // avoid duplicated work
  // CH (2010/7/28): The following test may have a bug. Its scope may be not NULL but different from
  // the scope passed in.
  // if (namelist.size()>0) if (namelist[0]->get_scope()!=NULL) return;
  // if (namelist.size() > 0) if (namelist[0]->get_scope() == scope) return;

     printf ("In SageInterface::fixVariableDeclaration(): This is dangerous code since for C++ the declarations can have different scopes \n");
     if (namelist.size() > 0)
        {
#if 1
       // DQ (11/19/2011): This at least makes this safer.
          bool exitEarly = true;
          SgInitializedNamePtrList::iterator j;
          for (j = namelist.begin(); j != namelist.end(); j++)
             {
               exitEarly = exitEarly && ((*j)->get_scope() == scope);
             }

          if (exitEarly == true)
             {
               printf ("Exit early from SageInterface::fixVariableDeclaration() \n");
               return;
             }
#else
          if (namelist[0]->get_scope() == scope)
             {
               printf ("Premature return from SageInterface::fixVariableDeclaration() \n");
               return;
             }
#endif
        }
#endif

     ROSE_ASSERT(namelist.size() > 0);

     SgInitializedNamePtrList::iterator i;
     for (i = namelist.begin(); i != namelist.end(); i++)
        {
          SgInitializedName *initName = *i;
          ROSE_ASSERT(initName != NULL);

       // DQ (11/19/2011): When used with C++, the variable may already have an associated scope 
       // and be using name qualification, so might not be associated with the current scope.
          SgScopeStatement* requiredScope = scope;
          SgScopeStatement* preAssociatedScope = initName->get_scope();
       // printf ("In SageInterface::fixVariableDeclaration() preAssociatedScope = %p \n",preAssociatedScope);

          if (preAssociatedScope != NULL)
             {
#if 0
               printf ("In SageInterface::fixVariableDeclaration(): Note that this variable already has an associated scope! preAssociatedScope = %p = %s (but will be reset below) \n",preAssociatedScope,preAssociatedScope->class_name().c_str());
#endif
            // ROSE_ASSERT(preAssociatedScope == scope);
               requiredScope = preAssociatedScope;
             }

          SgName name = initName->get_name();

       // DQ (11/19/2011): C++ can have a different scope than that of the current scope.
       // initName->set_scope(scope);
          initName->set_scope(requiredScope);

       // optional?
       // DQ (7/9/2012): Don't set this since it will be set later (see test2012_107.C) (LATER) Putting this back (mark declarations to not be output as compiler generated -- and not to be output).
       // DQ (7/12/2012): This is not correct for C++, so don't set it here (unless we use the current scope instead of scope).
       // Yes, let's set it to the current top of the scope stack.  This might be a problem if the scope stack is not being used...
       // varDecl->set_parent(scope);
        if (topScopeStack() != NULL) 
        {
          varDecl->set_parent(topScopeStack());
          ROSE_ASSERT(varDecl->get_parent() != NULL);
        }

       // DQ (11/19/2011): C++ can have a different scope than that of the current scope.
       // symbol table
       // ROSE_ASSERT(scope != NULL);
       // SgVariableSymbol* varSymbol = scope->lookup_variable_symbol(name);
          ROSE_ASSERT(requiredScope != NULL);
          SgVariableSymbol* varSymbol = requiredScope->lookup_variable_symbol(name);

          if (varSymbol == NULL)
             {
            // DQ (12/4/2011): This code is modified to try to only insert the symbol into the correct scope.  It used to
            // just insert the symbol into whatever scope structureally held the declaration (not good enough for C++).
               if (scope == initName->get_scope())
                  {
                    varSymbol = new SgVariableSymbol(initName);
                    ROSE_ASSERT(varSymbol);
#if 1
                 // DQ (5/16/2013): We now want to use the SgScopeStatement::insert_symbol() functions since we put 
                 // the complexity of handling namespaces into the implementation of that function.
                    scope->insert_symbol(name, varSymbol);
#else
                 // DQ (5/9/2013): If this is a namespace scope then we need so handle that case there there can be many since it is reentrant.
                    SgScopeStatement* associatedScope = NULL;
                    SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(scope);
                    if (namespaceDefinition != NULL)
                       {
                         associatedScope = namespaceDefinition->get_global_definition();
#error "DEAD CODE!"
                         printf ("WARNING: We should check the scope of the variable as well! initName->get_scope() = %p = %s associatedScope = %p \n",
                              initName->get_scope(),initName->get_scope()->class_name().c_str(),associatedScope);
                       }
                      else
                       {
                         associatedScope = scope;
                       }
                    ROSE_ASSERT(associatedScope != NULL);
                    associatedScope->insert_symbol(name, varSymbol);
#endif
                  }
             }
            else
             {
            // TODO consider prepend() and insert(), prev_decl_time is position dependent.
            // cout<<"sageInterface.C:5130 debug: found a previous var declaration!!...."<<endl;
               SgInitializedName* prev_decl = varSymbol->get_declaration();
               ROSE_ASSERT(prev_decl);

            // DQ (11/19/2011): Don't let prev_decl_item point be a self reference.
            // initName->set_prev_decl_item(prev_decl);
               if (initName != prev_decl)
                    initName->set_prev_decl_item(prev_decl);

               ROSE_ASSERT(initName->get_prev_decl_item() != initName);
#if 0
            // DQ (1/25/2014): We need to make sure that the variable is not initialzed twice.
            // The selection of where to do the initialization is however important.
            // ROSE_ASSERT(initName->get_prev_decl_item() != NULL);
               if (initName->get_prev_decl_item() != NULL)
                  {
                 // Check if get_prev_decl_item() is marked extern, and if so don't let it be marked with an initializer.
                 // We might also want to check is this is in a class, marked const, etc.
                    if (initName->get_prev_decl_item()->get_initializer() != NULL && initName->get_initializer() != NULL)
                       {
#if 1
                         printf ("In SageInterface::fixVariableDeclaration(): (initName->get_prev_decl_item()->get_init() != NULL): variable initialized twice! \n");
#endif
                         ROSE_ASSERT(initName->get_prev_decl_item()->get_initializer() != initName->get_initializer());

                      // DQ (1/25/2014): If the first variable was initialized, then reset the second one to NULL.
                         initName->set_initializer(NULL);
#if 1
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }
#endif
               
             } //end if
        } //end for

  // Liao 12/8/2010
  // For Fortran, a common statement may refer to a variable which is declared later.
  // In this case, a fake symbol is used for that variable reference.
  // But we have to replace the fake one with the real one once the variable declaration is inserted into AST
     if (SageInterface::is_Fortran_language() == true)
        {
          fixVariableReferences(scope);
        }
   }

int SageInterface::fixVariableReferences(SgNode* root)
{
  ROSE_ASSERT(root);
  int counter=0;
  Rose_STL_Container<SgNode*> varList;

  SgVarRefExp* varRef=NULL;
  Rose_STL_Container<SgNode*> reflist = NodeQuery::querySubTree(root, V_SgVarRefExp);
  for (Rose_STL_Container<SgNode*>::iterator i=reflist.begin();i!=reflist.end();i++)
  {
    varRef= isSgVarRefExp(*i);
    ROSE_ASSERT(varRef->get_symbol());
    SgInitializedName* initname= varRef->get_symbol()->get_declaration();

    ROSE_ASSERT (initname != NULL);
    if (initname->get_type()==SgTypeUnknown::createType())
      //    if ((initname->get_scope()==NULL) && (initname->get_type()==SgTypeUnknown::createType()))
    {
      SgName varName=initname->get_name();
      SgSymbol* realSymbol = NULL;

#if 1
      // CH (5/7/2010): Before searching SgVarRefExp objects, we should first deal with class/structure
      // members. Or else, it is possible that we assign the wrong symbol to those members if there is another
      // variable with the same name in parent scopes. Those members include normal member referenced using . or ->
      // operators, and static members using :: operators.
      //
      if (SgArrowExp* arrowExp = isSgArrowExp(varRef->get_parent()))
      {
        if (varRef == arrowExp->get_rhs_operand_i())
        {
            // make sure the lhs operand has been fixed
            counter += fixVariableReferences(arrowExp->get_lhs_operand_i());
            SgType* lhs_type = arrowExp->get_lhs_operand_i()->get_type() ;
            lhs_type = lhs_type->stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_TYPEDEF_TYPE);
            SgPointerType* ptrType = isSgPointerType(lhs_type);
            ROSE_ASSERT(ptrType);
            SgClassType* clsType = isSgClassType(ptrType->get_base_type()-> stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_TYPEDEF_TYPE));
            ROSE_ASSERT(clsType);
            SgClassDeclaration* decl = isSgClassDeclaration(clsType->get_declaration());
            decl = isSgClassDeclaration(decl->get_definingDeclaration());
            ROSE_ASSERT(decl);

         // DQ (8/16/2013): We want to lookup variable symbols not just any symbol.
         // realSymbol = lookupSymbolInParentScopes(varName, decl->get_definition());
            realSymbol = lookupVariableSymbolInParentScopes(varName, decl->get_definition());
        }
        else
        {
         // DQ (8/16/2013): We want to lookup variable symbols not just any symbol.
         // realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));
            realSymbol = lookupVariableSymbolInParentScopes(varName, getScope(varRef));
        }
      }
      else if (SgDotExp* dotExp = isSgDotExp(varRef->get_parent()))
      {
        if (varRef == dotExp->get_rhs_operand_i())
        {
            // make sure the lhs operand has been fixed
            counter += fixVariableReferences(dotExp->get_lhs_operand_i());

            SgType* lhs_type = dotExp->get_lhs_operand_i()->get_type() ;
            lhs_type = lhs_type->stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_TYPEDEF_TYPE);
            SgClassType* clsType = isSgClassType(lhs_type);
            ROSE_ASSERT(clsType);
            SgClassDeclaration* decl = isSgClassDeclaration(clsType->get_declaration());
            decl = isSgClassDeclaration(decl->get_definingDeclaration());
            ROSE_ASSERT(decl);

         // DQ (8/16/2013): We want to lookup variable symbols not just any symbol.
         // realSymbol = lookupSymbolInParentScopes(varName, decl->get_definition());
            realSymbol = lookupVariableSymbolInParentScopes(varName, decl->get_definition());
        }
        else
         // DQ (8/16/2013): We want to lookup variable symbols not just any symbol.
         // realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));
            realSymbol = lookupVariableSymbolInParentScopes(varName, getScope(varRef));
      }
      else
#endif
      {
         // DQ (8/16/2013): We want to lookup variable symbols not just any symbol.
         // realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));
            realSymbol = lookupVariableSymbolInParentScopes(varName, getScope(varRef));
      }

      // should find a real symbol at this final fixing stage!
      // This function can be called any time, not just final fixing stage
      if (realSymbol==NULL)
      {
        //cerr<<"Error: cannot find a symbol for "<<varName.getString()<<endl;
        //ROSE_ASSERT(realSymbol);
      }
      else {
        // release placeholder initname and symbol
        ROSE_ASSERT(realSymbol!=(varRef->get_symbol()));
#if 0
        // CH (5/12/2010):
        // To delete a symbol node, first check if there is any node in memory
        // pool which points to this symbol node. Only if no such node exists,
        // this symbol together with its initialized name can be deleted.
        //
        bool toDelete = true;

        SgSymbol* symbolToDelete = varRef->get_symbol();
        varRef->set_symbol(isSgVariableSymbol(realSymbol));
        counter ++;

        if (varList.empty())
        {
            VariantVector vv(V_SgVarRefExp);
            varList = NodeQuery::queryMemoryPool(vv);
        }

        for (Rose_STL_Container<SgNode*>::iterator i = varList.begin();
                i != varList.end(); ++i)
        {
            if (SgVarRefExp* var = isSgVarRefExp(*i))
            {
                if (var->get_symbol() == symbolToDelete)
                {
                    toDelete = false;
                    break;
                }
            }
        }
        if (toDelete)
        {
            delete initname; // TODO deleteTree(), release File_Info nodes etc.
            delete symbolToDelete;
        }

#else

        // CH (2010/7/26): We cannot delete those initname and symbol here, since there may be other variable references
        // which point to them. We will delay this clear just before AstTests.
#if 0
        delete initname; // TODO deleteTree(), release File_Info nodes etc.
        delete (varRef->get_symbol());
#endif

        //std::cout << "Fixed variable reference: " << realSymbol->get_name().str() << std::endl;
        varRef->set_symbol(isSgVariableSymbol(realSymbol));
        counter ++;
#endif
      }
    }
  } // end for
  // Liao 2/1/2013: delete unused initname and symbol, considering possible use by the current subtree from root node
  clearUnusedVariableSymbols(root); 
  return counter;
}

void SageInterface::clearUnusedVariableSymbols(SgNode* root /*= NULL */)
{
    Rose_STL_Container<SgNode*> symbolList;
    VariantVector sym_vv(V_SgVariableSymbol);
    symbolList = NodeQuery::queryMemoryPool(sym_vv);

    Rose_STL_Container<SgNode*> varList;
    VariantVector var_vv(V_SgVarRefExp);
    //varList = NodeQuery::queryMemoryPool(var_vv);
    if (root != NULL)
    {
      varList = NodeQuery::querySubTree(root, V_SgVarRefExp);
    }

    for (Rose_STL_Container<SgNode*>::iterator i = symbolList.begin();
            i != symbolList.end(); ++i)
    {
        SgVariableSymbol* symbolToDelete = isSgVariableSymbol(*i);
        ROSE_ASSERT(symbolToDelete);
        if (symbolToDelete->get_declaration()->get_type() != SgTypeUnknown::createType())
            continue;
        // symbol with a declaration of SgTypeUnknown will be deleted
        bool toDelete = true;

        if (root != NULL) // if root is specified. We further check if the symbol is referenced by any nodes of the tree rooted at "root"
        {
          for (Rose_STL_Container<SgNode*>::iterator j = varList.begin();
              j != varList.end(); ++j)
          {
            SgVarRefExp* var = isSgVarRefExp(*j);
            ROSE_ASSERT(var);

            if (var->get_symbol() == symbolToDelete)
            {
              toDelete = false;
              break;
            }
          }
        }

        if (toDelete)
        {
#if 0
            std::cout << "Symbol " << symbolToDelete->get_name().str() << ' ' << symbolToDelete <<
               ' ' << symbolToDelete->get_declaration() <<  " is deleted." << std::endl;
#endif
            delete symbolToDelete->get_declaration();
            delete symbolToDelete;
        }
    }
}


//! fixup symbol table for SgLableStatement. Used Internally when the label is built without knowing its target scope. Both parameters cannot be NULL.
/*
 * label statement has special scope: the closest function definition , not SgBasicBlock or others!
 */
void SageInterface::fixLabelStatement(SgLabelStatement* stmt, SgScopeStatement* scope)
   {
     SgLabelStatement* label_stmt = isSgLabelStatement(stmt);
     ROSE_ASSERT(label_stmt);
     SgName name = label_stmt->get_label();

     SgScopeStatement* label_scope = getEnclosingFunctionDefinition(scope,true);
     if (label_scope) //Should we assert this instead? No for bottom up AST building
        {
          label_stmt->set_scope(label_scope);
          SgLabelSymbol* lsymbol = label_scope->lookup_label_symbol(name);

          if (lsymbol != NULL)
             {
            // DQ (12/4/2011): This is the correct handling for SgLabelStatement (always in the function scope).
               lsymbol= new SgLabelSymbol(label_stmt);
               ROSE_ASSERT(lsymbol);
               label_scope->insert_symbol(lsymbol->get_name(), lsymbol);
             }
        }
   }


//! Set a numerical label for a Fortran statement. The statement should have a enclosing function definition already. SgLabelSymbol and SgLabelR
//efExp are created transparently as needed.
void SageInterface::setFortranNumericLabel(SgStatement* stmt, int label_value)
   {
     ROSE_ASSERT (stmt != NULL);
     ROSE_ASSERT (label_value >0 && label_value <=99999); //five digits for Fortran label

     SgScopeStatement* label_scope = getEnclosingFunctionDefinition(stmt);
     ROSE_ASSERT (label_scope != NULL);
     SgName label_name(StringUtility::numberToString(label_value));
     SgLabelSymbol * symbol = label_scope->lookup_label_symbol (label_name);
     if (symbol == NULL)
        {
       // DQ (12/4/2011): This is the correct handling for SgLabelStatement (always in the function scope, same as C and C++).
       // DQ (2/2/2011): We want to call the old constructor (we now have another constructor that takes a SgInitializedName pointer).
       // symbol = new SgLabelSymbol(NULL);
          symbol = new SgLabelSymbol((SgLabelStatement*) NULL);
          ROSE_ASSERT(symbol != NULL);
          symbol->set_fortran_statement(stmt);
          symbol->set_numeric_label_value(label_value);
          label_scope->insert_symbol(label_name,symbol);
        }
       else
        {
          cerr<<"Error. SageInterface::setFortranNumericLabel() tries to set a duplicated label value!"<<endl;
          ROSE_ASSERT (false);
        }

  // SgLabelRefExp
     SgLabelRefExp* ref_exp = buildLabelRefExp(symbol);
     stmt->set_numeric_label(ref_exp);
     ref_exp->set_parent(stmt);
   }


//! Suggest next usable (non-conflicting) numeric label value for a Fortran function definition scope
int  SageInterface::suggestNextNumericLabel(SgFunctionDefinition* func_def)
{
  int result =10;
  ROSE_ASSERT (func_def != NULL);
  ROSE_ASSERT (SageInterface::is_Fortran_language()== true);
  std::set<SgNode*> symbols = func_def->get_symbol_table()->get_symbols();

  // find the max label value, +10 to be the suggested next label value
  std::set<SgNode*>::iterator iter ;
  for (iter=symbols.begin(); iter !=symbols.end(); iter++)
  {
    SgLabelSymbol * l_symbol = isSgLabelSymbol(*iter);
    if (l_symbol)
    {
      int cur_val = l_symbol->get_numeric_label_value();
      if (result <=cur_val)
        result = cur_val +10;
    }
  }

  ROSE_ASSERT (result <= 99999); // max 5 digits for F77 label
  return result;
}

//! fixup symbol table for SgFunctionDeclaration (and template instantiations, member functions, and member function template instantiations). Used Internally when the function is built without knowing its target scope. Both parameters cannot be NULL.
/*
 * function declarations can have a scope that is different from their structural location (e.g. member functions declared outside of the defining class declaration.
 */
void SageInterface::fixFunctionDeclaration(SgFunctionDeclaration* stmt, SgScopeStatement* scope)
   {
 // DQ (3/5/2012): Added test.
    ROSE_ASSERT(scope != NULL);

 // fix function type table's parent edge
 // Liao 5/4/2010
    SgFunctionTypeTable * fTable = SgNode::get_globalFunctionTypeTable();
    ROSE_ASSERT(fTable != NULL);

    if (fTable->get_parent() == NULL)
       {
      // DQ (3/5/2012): This is a problem for test2012_13.C (test code taken from test2004_42.C).
      // fTable->set_parent(getGlobalScope(scope));
#if 0
         printf ("WARNING: Skip setting the scope of the SgFunctionTypeTable scope = %p = %s \n",scope,scope->class_name().c_str());
#endif
         fTable->set_parent(getGlobalScope(scope));
       }

#if 0
     printf ("In SageInterface::fixStatement(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif

  // Liao 4/23/2010,  Fix function symbol
  // This could happen when users copy a function, then rename it (func->set_name()), and finally insert it to a scope
     SgFunctionDeclaration               * func         = isSgFunctionDeclaration(stmt);
     SgMemberFunctionDeclaration         * mfunc        = isSgMemberFunctionDeclaration(stmt);
     SgTemplateFunctionDeclaration       * tfunc        = isSgTemplateFunctionDeclaration(stmt);
     SgTemplateMemberFunctionDeclaration * tmfunc       = isSgTemplateMemberFunctionDeclaration(stmt);
     SgProcedureHeaderStatement          * procfunc     = isSgProcedureHeaderStatement(stmt);

     if (tmfunc != NULL)
       assert(tmfunc->variantT() == V_SgTemplateMemberFunctionDeclaration);
     else if (mfunc != NULL)
       assert(mfunc->variantT() == V_SgMemberFunctionDeclaration || mfunc->variantT() == V_SgTemplateInstantiationMemberFunctionDecl);
     else if (tfunc != NULL)
       assert(tfunc->variantT() == V_SgTemplateFunctionDeclaration);
     else if (procfunc != NULL)
        assert(procfunc->variantT() == V_SgProcedureHeaderStatement);
     else if (func != NULL)
       assert(func->variantT() == V_SgFunctionDeclaration || func->variantT() == V_SgTemplateInstantiationFunctionDecl);
     else assert(false);

#if 0
     printf ("In SageInterface::fixStatement(): scope = %p = %s \n",scope,scope->class_name().c_str());
     printf ("In SageInterface::fixStatement(): stmt->get_scope() = %p \n",stmt->get_scope());
#endif

  // DQ (12/3/2011): This is a scary piece of code, but I think it is OK now!
  // It is an error to put the symbol for a function into the current scope if the function's scope 
  // is explicitly set to be different.  So this should be allowed only if the function's scope is 
  // not explicitly set, or if the scopes match.  This is an example of something different for C++
  // than for C or other simpler languages.
  // If the scope of the function is not set, or if it matches the current scope then allow this step.
     if (stmt->get_scope() == NULL || scope == stmt->get_scope())
        {
#if 0
          printf ("Looking up the function symbol using name = %s and type = %p = %s \n",func->get_name().str(),func->get_type(),func->get_type()->class_name().c_str());
#endif
#if 0
          printf ("[SageInterface::fixFunctionDeclaration] Lookup Function func = %p, name = %s, type = %p, scope = %p\n", func, func->get_name().getString().c_str(), func->get_type(), scope);
#endif
          SgFunctionSymbol* func_symbol = NULL;

       // DQ (7/31/2013): Fixing API to use functions that now require template parameters and template specialization arguments.
       // In this case these are unavailable from this point.
          if (tmfunc != NULL)
          {
            SgTemplateParameterPtrList & templateParameterList = tmfunc->get_templateParameters();
         // func_symbol = scope->lookup_template_member_function_symbol (func->get_name(), func->get_type());
         // func_symbol = scope->lookup_template_member_function_symbol (func->get_name(), func->get_type(),NULL);
            func_symbol = scope->lookup_template_member_function_symbol (func->get_name(), func->get_type(),&templateParameterList);
          }
          else if (mfunc != NULL)
          {
            SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(mfunc);
            SgTemplateArgumentPtrList* templateArgumentList = (templateInstantiationMemberFunctionDecl != NULL) ? &(templateInstantiationMemberFunctionDecl->get_templateArguments()) : NULL;
         // func_symbol = scope->lookup_nontemplate_member_function_symbol (func->get_name(), func->get_type(),NULL);
            func_symbol = scope->lookup_nontemplate_member_function_symbol (func->get_name(), func->get_type(),templateArgumentList);
          }
          else if (tfunc != NULL)
          {
            SgTemplateParameterPtrList & templateParameterList = tfunc->get_templateParameters();
#if 0
            printf ("In SageInterface::fixStatement(): templateParameterList.size() = %zu \n",templateParameterList.size());
#endif
         // func_symbol = scope->lookup_template_function_symbol (func->get_name(), func->get_type());
         // func_symbol = scope->lookup_template_function_symbol (func->get_name(), func->get_type(),NULL);
            func_symbol = scope->lookup_template_function_symbol (func->get_name(), func->get_type(),&templateParameterList);
          }
          else if (procfunc != NULL)
          {
#if 1
            printf ("In SageInterface::fixStatement(): procfunc->get_name() = %s calling lookup_function_symbol() \n",procfunc->get_name().str());
#endif
            func_symbol = scope->lookup_function_symbol (procfunc->get_name(), procfunc->get_type());
            assert(func_symbol != NULL);
          }
          else if (func != NULL)
          {
#if 0
            printf ("In SageInterface::fixStatement(): func->get_name() = %s calling lookup_function_symbol() \n",func->get_name().str());
#endif
            SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDecl = isSgTemplateInstantiationFunctionDecl(func);
            SgTemplateArgumentPtrList* templateArgumentList = (templateInstantiationFunctionDecl != NULL) ? &(templateInstantiationFunctionDecl->get_templateArguments()) : NULL;
         // func_symbol = scope->lookup_function_symbol (func->get_name(), func->get_type(),NULL);
            func_symbol = scope->lookup_function_symbol (func->get_name(), func->get_type(),templateArgumentList);

         // DQ (8/23/2013): Adding support for when the symbol is not present.  This can happen when building a new function as a copy of an existing
         // function the symantics of the copy is that it will not add the symbol (since it does not know the scope). So this function is the first
         // opportunity to fixup the function to have a symbol in the scope's symbol table.
            if (func_symbol == NULL)
               {
              // scope->print_symboltable("In SageInterface::fixStatement()");
                 func_symbol = new SgFunctionSymbol(func);
                 scope->insert_symbol(func->get_name(), func_symbol);
               }
          }
          else 
          {
            assert(false);
          }
#if 0
          printf("[SageInterface::fixFunctionDeclaration]     -> func = %p, mfunc = %p, tmfunc = %p\n", func, mfunc, tmfunc);
#endif
#if 0
          printf ("In SageInterface::fixStatement(): func_symbol = %p \n",func_symbol);
#endif

          assert(func_symbol != NULL);
        }
   }

//! fixup symbol table for SgFunctionDeclaration (and template instantiations, member functions, and member function template instantiations). Used Internally when the function is built without knowing its target scope. Both parameters cannot be NULL.
/*
 * function declarations can have a scope that is different from their structural location (e.g. member functions declared outside of the defining class declaration.
 */
void SageInterface::fixTemplateDeclaration(SgTemplateDeclaration* stmt, SgScopeStatement* scope)
   {
  // DQ (12/4/2011): This function has not been implemented yet.  It will assert fail if it is required.
     printf ("Need to handle SgTemplateDeclaration IR nodes as well...(implement later) \n");
  // ROSE_ASSERT(false);
   }


//! A wrapper containing fixes (fixVariableDeclaration(),fixStructDeclaration(), fixLabelStatement(), etc) for all kinds statements.
void SageInterface::fixStatement(SgStatement* stmt, SgScopeStatement* scope)
   {
  // fix symbol table
     if (isSgVariableDeclaration(stmt))
        {
          fixVariableDeclaration(isSgVariableDeclaration(stmt), scope);
        }
       else if (isStructDeclaration(stmt))
        {
       // DQ (1/2/2010): Enforce some rules as early as possible.
       // fixStructDeclaration(isSgClassDeclaration(stmt),scope);
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(stmt);
          ROSE_ASSERT(classDeclaration != NULL);
#if 0
          ROSE_ASSERT(classDeclaration->get_firstNondefiningDeclaration() != NULL);
          ROSE_ASSERT(classDeclaration->get_definingDeclaration() != NULL);
#endif
          fixStructDeclaration(classDeclaration,scope);
        }
       else if (isSgClassDeclaration(stmt))
        {
          fixClassDeclaration(isSgClassDeclaration(stmt),scope);
        }
      else if (isSgLabelStatement(stmt))
        {
          fixLabelStatement(isSgLabelStatement(stmt),scope);
        }
       else if (isSgFunctionDeclaration(stmt))
        {
#if 1
          fixFunctionDeclaration(isSgFunctionDeclaration(stmt),scope);
#else
       // fix function type table's parent edge
       // Liao 5/4/2010
          SgFunctionTypeTable * fTable = SgNode::get_globalFunctionTypeTable();
          ROSE_ASSERT(fTable);
          if (fTable->get_parent() == NULL)
               fTable->set_parent(getGlobalScope(scope));

       // Liao 4/23/2010,  Fix function symbol
       // This could happen when users copy a function, then rename it (func->set_name()), and finally insert it to a scope
          SgFunctionDeclaration*       func        = isSgFunctionDeclaration(stmt);
          SgMemberFunctionDeclaration* mfunc       = isSgMemberFunctionDeclaration(stmt);

          printf ("In SageInterface::fixStatement(): scope = %p = %s \n",scope,scope->class_name().c_str());
          printf ("In SageInterface::fixStatement(): stmt->get_scope() = %p \n",stmt->get_scope());

       // DQ (12/3/2011): This is a scary piece of code, but I think it is OK now!
       // It is an error to put the symbol for a function into the current scope if the function's scope 
       // is explicitly set to be different.  So this should be allowed only if the function's scope is 
       // not explicitly set, or if the scopes match.  This is an example of something different for C++
       // than for C or other simpler languages.
       // If the scope of the function is not set, or if it matches the current scope then allow this step.
          if (stmt->get_scope() == NULL || scope == stmt->get_scope())
             {
#if 0
               printf ("Looking up the function symbol using name = %s and type = %p = %s \n",func->get_name().str(),func->get_type(),func->get_type()->class_name().c_str());
#endif
               SgFunctionSymbol*            func_symbol = scope->lookup_function_symbol (func->get_name(), func->get_type());

               printf ("In SageInterface::fixStatement(): func_symbol = %p \n",func_symbol);
               if (func_symbol == NULL)
                  {
                 // DQ (12/3/2011): Added support for C++ member functions.
                 // func_symbol = new SgFunctionSymbol (func);
                    if (mfunc != NULL)
                       {
                         func_symbol = new SgMemberFunctionSymbol (func);
                       }
                      else
                       {
                         func_symbol = new SgFunctionSymbol (func);
                       }
                    ROSE_ASSERT (func_symbol != NULL);

                    scope->insert_symbol(func->get_name(), func_symbol);
                  }
                 else
                  {
                    printf ("In SageInterface::fixStatement(): found a valid function so no need to insert new symbol \n");
                  }
             }
#if 0
       // Fix local symbol, a symbol directly refer to this function declaration
       // This could happen when a non-defining func decl is copied, the corresonding symbol will point to the original source func
       // symbolTable->find(this) used inside get_symbol_from_symbol_table()  won't find the copied decl 
          SgSymbol* local_symbol = func ->get_symbol_from_symbol_table();
          if (local_symbol == NULL) // 
             {
               if (func->get_definingDeclaration() == NULL) // prototype function
                  {
                    SgFunctionDeclaration * src_func = func_symbol->get_declaration();
                    if (func != src_func )
                       {
                         ROSE_ASSERT (src_func->get_firstNondefiningDeclaration () == src_func);
                         func->set_firstNondefiningDeclaration (func_symbol->get_declaration());
                       }
                  }
             }
#endif
#endif
        }
       else if (isSgTemplateDeclaration(stmt) != NULL)
        {
       // DQ (12/3/2011): Added new case for SgTemplateDeclaration (adding template declarations to the AST).
          fixTemplateDeclaration(isSgTemplateDeclaration(stmt),scope);
        }

#if 0
  // DQ (12/4/2011): This WAS not the correct behavior for C++ since declarations can appear structureally in different 
  // scopes than where the are positioned (e.g. member functions defined outside of there associated class). 
  // This this code is very dangerous.

  // fix scope pointer for statements explicitly storing scope pointer
     switch (stmt->variantT())
        {
       // The case of SgLabelStatement should maybe be included.
          case V_SgEnumDeclaration:
          case V_SgTemplateDeclaration:
          case V_SgTypedefDeclaration:
          case V_SgFunctionDeclaration:
          case V_SgMemberFunctionDeclaration:
          case V_SgTemplateInstantiationFunctionDecl:
             {
            // DQ (12/4/2011): We can't just set the scope this simily (except in C).  In C++ the scope should have 
            // already been set or we can let it default to the current scope where it si located structurally.
            // stmt->set_scope(scope);
                if ( (stmt->hasExplicitScope() == true) && (stmt->get_scope() == NULL) )
                  {
                    stmt->set_scope(scope);
                  }
               break;
             }

          default:
             {
            // debugging support...
               printf ("In SageInterface::fixStatement(): switch default case used (likely OK): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
               ROSE_ASSERT(stmt->hasExplicitScope() == false);
#if 0
               printf ("switch case not handled properly: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
               ROSE_ASSERT(false);
#endif
               break;
             }
        }
#else
  // If the scoep has to be set and it has not yet been set, then set it directly.
     if ( (stmt->hasExplicitScope() == true) && (stmt->get_scope() == NULL) )
        {
          stmt->set_scope(scope);
        }
#endif
   }


/*! Liao, 7/3/2008
 * Update a list of function declarations inside a scope according to a newly introduced one
 *
 * Algorithm:
 * iterate declaration list for the same functions
 * func is defining:
 *       set_defining for all
 * func is nondefining:
 *       is first ?  set_first_nondefining for all
 *       not the first ? set first nondefining for itself only
 */
void SageInterface::updateDefiningNondefiningLinks(SgFunctionDeclaration* func, SgScopeStatement* scope)
   {
  // DQ (11/19/2012): Note that this appears to be an expensive function presently taking 22.5% of the total time 
  // to process the tests/CompilerTests/Cxx_tests/rosePerformance.C file.  So this is a performance problem.

     ROSE_ASSERT(func != NULL && scope != NULL);

     SgStatementPtrList stmtList, sameFuncList;

  // SgFunctionDeclaration* first_nondef = NULL;
  // Some annoying part of scope
     if (scope->containsOnlyDeclarations())
        {
          SgDeclarationStatementPtrList declList = scope->getDeclarationList();
          SgDeclarationStatementPtrList::iterator i;
          for (i=declList.begin();i!=declList.end();i++)
               stmtList.push_back(*i);
        }
       else
        {
          stmtList = scope->getStatementList();
        }

     SgFunctionDeclaration* firstNondefiningFunctionDeclaration = isSgFunctionDeclaration(func->get_firstNondefiningDeclaration());
     if (firstNondefiningFunctionDeclaration != NULL)
        {
       // If there exists a non-NULL reference to a firstNondefiningFunctionDeclaration 
       // then use it (unless we want to handle where it might be set wrong).
#if 0
          printf ("In SageInterface::updateDefiningNondefiningLinks(): func = %p Found a valid pointer to a firstNondefiningFunctionDeclaration = %p \n",func,firstNondefiningFunctionDeclaration);
#endif
        }

  // DQ (3/12/2012): Added assertion
  // ROSE_ASSERT(scope == func->get_firstNondefiningDeclaration()->get_scope());
     ROSE_ASSERT(func->get_firstNondefiningDeclaration()->get_scope() != NULL);

  // DQ (5/25/2013): This test fails for the astInterface test: buildStructDeclaration.C, since this is a new test, make it a warning for now.
     if (func->get_firstNondefiningDeclaration()->get_scope()->lookup_function_symbol(func->get_name(),func->get_type()) == NULL)
        {
          printf ("WARNING: symbol for func->get_firstNondefiningDeclaration() = %p = %s = %s is not present in the scope = %p = %s associated with the firstNondefiningDeclaration \n",
               func->get_firstNondefiningDeclaration(),func->get_firstNondefiningDeclaration()->class_name().c_str(),func->get_name().str(),
               func->get_firstNondefiningDeclaration()->get_scope(),func->get_firstNondefiningDeclaration()->get_scope()->class_name().c_str());
        }
  // ROSE_ASSERT(func->get_firstNondefiningDeclaration()->get_scope()->lookup_function_symbol(func->get_name(),func->get_type()) != NULL);

#if 0
  // It would be better to find the first non-defining declaration via the symbol.
     SgSymbol* functionSymbol = scope->lookup_function_symbol(func->get_name(),func->get_type());
     if (functionSymbol != NULL)
        {
          printf ("In SageInterface::updateDefiningNondefiningLinks(): func = %p Found a valid symbol = %p \n",func,functionSymbol);
        }
       else
        {
          printf ("In SageInterface::updateDefiningNondefiningLinks(): func = %p functionSymbol == NULL \n",func);
        }
#endif

  // Find the same function declaration list, including func itself
     SgStatementPtrList::iterator j;
     for (j = stmtList.begin(); j != stmtList.end(); j++)
        {
          SgFunctionDeclaration* func_decl = isSgFunctionDeclaration(*j);
          if (func_decl != NULL)
             {
            // DQ (11/19/2012): This call to the isSameFunction() function is taking a total of 22.1% 
            // of the total execution time of the tests/CompilerTests/Cxx_tests/rosePerformance.C file.
               if (isSameFunction(func_decl, func))
                  {
                 // Assume all defining functions have definingdeclaration links set properly already!!
                 // if ((first_nondef == NULL) && (func_decl->get_definingDeclaration() == NULL))
                 //      first_nondef = func_decl;
                    sameFuncList.push_back(func_decl);
                  }
             }
        }

     if (func->get_definingDeclaration()==func)
        {
          for (j = sameFuncList.begin(); j != sameFuncList.end(); j++)
               isSgFunctionDeclaration(*j)->set_definingDeclaration(func);
        }
       else
        {
       // DQ (3/9/2012): Added assertion to avoid empty list that would be an error in both cases below.
          ROSE_ASSERT(sameFuncList.empty() == false);

          if (func == isSgFunctionDeclaration(*(sameFuncList.begin()))) // is first_nondefining declaration
             {
               for (j = sameFuncList.begin(); j != sameFuncList.end(); j++)
                  {
                    SgFunctionDeclaration* func_decl = isSgFunctionDeclaration(*j);
#if 0
                    printf ("In SageInterface::updateDefiningNondefiningLinks(): (case 1) Testing j = %p set_firstNondefiningDeclaration(%p) \n",*j,func);
#endif
                 // DQ (3/9/2012): Avoid setting the function to be it's own firstNondefiningDeclaration.
                 // isSgFunctionDeclaration(*j)->set_firstNondefiningDeclaration(func);
                 // if (*j != func)
                    if (func_decl != func)
                       {
                      // DQ (11/18/2013): Modified to only set if not already set (see buildIfStmt.C in tests/nonsmoke/functional/roseTests/astInterface_tests).
                      // isSgFunctionDeclaration(*j)->set_firstNondefiningDeclaration(func);
                         if (func_decl->get_firstNondefiningDeclaration() == NULL)
                            {
#if 0
                              printf ("In SageInterface::updateDefiningNondefiningLinks(): (case 1) Calling j = %p set_firstNondefiningDeclaration(%p) \n",*j,func);
#endif
                              func_decl->set_firstNondefiningDeclaration(func);
                            }
                       }
                  }
             }
            else // is a following nondefining declaration, grab any other's first nondefining link then
             {
#if 0
               printf ("In SageInterface::updateDefiningNondefiningLinks(): (case 2) Testing func = %p set_firstNondefiningDeclaration(%p) \n",func,isSgFunctionDeclaration(*(sameFuncList.begin()))->get_firstNondefiningDeclaration());
#endif
            // DQ (11/18/2013): Modified to only set if not already set (see buildIfStmt.C in tests/nonsmoke/functional/roseTests/astInterface_tests).
            // func->set_firstNondefiningDeclaration(isSgFunctionDeclaration(*(sameFuncList.begin()))->get_firstNondefiningDeclaration());
               if (func->get_firstNondefiningDeclaration() == NULL)
                  {
#if 0
                    printf ("In SageInterface::updateDefiningNondefiningLinks(): (case 2) Calling func = %p set_firstNondefiningDeclaration(%p) \n",func,isSgFunctionDeclaration(*(sameFuncList.begin()))->get_firstNondefiningDeclaration());
#endif
                    func->set_firstNondefiningDeclaration(isSgFunctionDeclaration(*(sameFuncList.begin()))->get_firstNondefiningDeclaration());
                  }
             }
        }
   }


//---------------------------------------------------------------
PreprocessingInfo* SageInterface::attachComment(
           SgLocatedNode* target, const string& content,
           PreprocessingInfo::RelativePositionType  position /*=PreprocessingInfo::before*/,
           PreprocessingInfo::DirectiveType dtype /* PreprocessingInfo::CpreprocessorUnknownDeclaration */)
   {
     ROSE_ASSERT(target); //dangling comment is not allowed

     PreprocessingInfo* result = NULL;
     PreprocessingInfo::DirectiveType mytype=dtype;
     string comment;

  // DQ (5/5/2010): infer comment type from target's language
     if (mytype == PreprocessingInfo::CpreprocessorUnknownDeclaration)
        {
       // This is a rather expensive way to detect the language type (chases pointers back to the SgFile object).
          if (is_C_language() || is_C99_language())
             {
               mytype = PreprocessingInfo::C_StyleComment;
            // comment = "/* "+ content + " */";
             }
            else
             {
               if (is_Cxx_language() || is_Java_language())
                  {
                    mytype = PreprocessingInfo::CplusplusStyleComment;
                 // comment = "// "+ content;
                  }
                 else  // TODO :What about Fortran?
                  {
                    if (is_Fortran_language() || is_CAF_language()) //FMZ:3/23/2009
                       {
                         mytype = PreprocessingInfo::F90StyleComment;
                      // comment = "// "+ content;
                       }
                      else  // TODO :What about Fortran?
                       {
                         cout<<"Un-handled programming languages when building source comments.. "<<endl;
                         ROSE_ASSERT(false);
                       }
                  }
             }
        }

  // Once the langauge type is set (discovered automatically or more directly specified by the user).
     bool resetPositionInfo = false;
     switch (mytype)
        {
          case PreprocessingInfo::C_StyleComment:        comment = "/* " + content + " */"; break;
          case PreprocessingInfo::CplusplusStyleComment: comment = "// " + content;         break;
          case PreprocessingInfo::FortranStyleComment:   comment = "      C " + content;    break;
          case PreprocessingInfo::F90StyleComment:   comment = "!" + content;    break;
          case PreprocessingInfo::CpreprocessorLineDeclaration:
               comment = "#myline " + content;
               mytype = PreprocessingInfo::CplusplusStyleComment;
               resetPositionInfo = true;
               break;

          default:
             {
               printf ("Error: default in switch reached in SageInterface::attachComment() PreprocessingInfo::DirectiveType == %d \n",mytype);
               ROSE_ASSERT(false);
             }
        }

     result = new PreprocessingInfo (mytype,comment, "transformation-generated", 0, 0, 0, position);

  // If this is a Cpp Line declaration then we have to set the position to match the statement.
  // if (mytype == PreprocessingInfo::CpreprocessorLineDeclaration)
     if (resetPositionInfo == true)
        {
       // Call the Sg_File_Info::operator=() member function.
          *(result->get_file_info()) = *(target->get_file_info());
        }

     ROSE_ASSERT(result);
     target->addToAttachedPreprocessingInfo(result);
     return result;
   }

PreprocessingInfo* SageInterface::insertHeader(const string& filename, PreprocessingInfo::RelativePositionType position /*=after*/, bool isSystemHeader /*=false*/, SgScopeStatement* scope /*=NULL*/)
  {
    bool successful = false;
    if (scope == NULL)
        scope = SageBuilder::topScopeStack();
    ROSE_ASSERT(scope);
    SgGlobal* globalScope = getGlobalScope(scope);
    ROSE_ASSERT(globalScope);

    PreprocessingInfo* result=NULL;
    string content;
    if (isSystemHeader)
      content = "#include <" + filename + "> \n";
    else
      content = "#include \"" + filename + "\" \n";

    SgDeclarationStatementPtrList & stmtList = globalScope->get_declarations ();
    if (stmtList.size()>0) // the source file is not empty
     {
      for (SgDeclarationStatementPtrList::iterator j = stmtList.begin ();
           j != stmtList.end (); j++)
      {
            //must have this judgement, otherwise wrong file will be modified!
            //It could also be the transformation generated statements with #include attached
        if ( ((*j)->get_file_info ())->isSameFile(globalScope->get_file_info ())||
              ((*j)->get_file_info ())->isTransformation()
           )
         {
           result = new PreprocessingInfo(PreprocessingInfo::CpreprocessorIncludeDeclaration,
                                          content, "Transformation generated",0, 0, 0, PreprocessingInfo::before);
           ROSE_ASSERT(result);
           (*j)->addToAttachedPreprocessingInfo(result,position);
           successful = true;
           break;
         }
      }
     }
    else // empty file, attach it after SgGlobal,TODO it is not working for unknown reason!!
     {
       cerr<<"SageInterface::insertHeader() Empty file is found!"<<endl;
       cerr<<"#include xxx is  preprocessing information which has to be attached  to some other  located node (a statement for example)"<<endl;
       cerr<<"You may have to insert some statement first before inserting a header"<<endl;
       ROSE_ASSERT(false);
       result = new PreprocessingInfo(PreprocessingInfo::CpreprocessorIncludeDeclaration,
                content, "Transformation generated",0, 0, 0, PreprocessingInfo::after);
       ROSE_ASSERT(result);
       globalScope->addToAttachedPreprocessingInfo(result,position);
       successful = true;
    }
    // must be inserted once somehow
    ROSE_ASSERT(successful==true);
    return result;
  }


//! Attach an arbitrary string to a located node. A workaround to insert irregular statements or vendor-specific attributes. We abuse CpreprocessorDefineDeclaration for this purpose.
PreprocessingInfo* 
SageInterface::attachArbitraryText(SgLocatedNode* target, const std::string & text, PreprocessingInfo::RelativePositionType position /*=PreprocessingInfo::before*/)
   {
  // DQ (1/13/2014): This function needs a better mechanism than attaching text to the AST unparser as a CPP directive.

     ROSE_ASSERT(target != NULL); //dangling #define xxx is not allowed in the ROSE AST
     PreprocessingInfo* result = NULL;

  // DQ (1/13/2014): It is a mistake to attach arbitrary test to the AST as a #define 
  // (since we evaluate all #define CPP declarations to be a self-referential macro).
  // For now I will make it a #if CPP declaration, since these are not evaluated internally.
  // PreprocessingInfo::DirectiveType mytype = PreprocessingInfo::CpreprocessorDefineDeclaration;
     PreprocessingInfo::DirectiveType mytype = PreprocessingInfo::CpreprocessorIfDeclaration;

  // DQ (1/13/2014): Output a warning so that this can be fixed whereever it is used.
     printf ("Warning: attachArbitraryText(): attaching arbitrary text to the AST as a #if declaration: text = %s \n",text.c_str());

     result = new PreprocessingInfo (mytype,text, "transformation-generated", 0, 0, 0, position);
     ROSE_ASSERT(result);

     target->addToAttachedPreprocessingInfo(result);

     return result;
   }


//!Check if a target node has MacroCall attached, if yes, replace them with expanded strings
// TODO This is a dirty fix since the ideal solution would be having a preprocessed pragma text generated by the compiler. String matching and replacing is never safe.
void SageInterface::replaceMacroCallsWithExpandedStrings(SgPragmaDeclaration* target)
{
  // This is part of Wave support in ROSE.
// #if CAN_NOT_COMPILE_WITH_ROSE != true
// #if CAN_NOT_COMPILE_WITH_ROSE == 0
#ifndef USE_ROSE
  ROSE_ASSERT(target != NULL);
  AttachedPreprocessingInfoType *info=  target->getAttachedPreprocessingInfo ();
  if (info == NULL) return;
  AttachedPreprocessingInfoType::iterator j;
  for (j = info->begin (); j != info->end (); j++)
  {
    if ((*j)->getTypeOfDirective()==PreprocessingInfo::CMacroCall)
    {
      std::ostringstream os;
      token_container tc = (*j)->get_macro_call()->expanded_macro;
      token_container::const_iterator iter;
      for (iter=tc.begin(); iter!=tc.end(); iter++)
        os<<(*iter).get_value();
      //cout<<"Found a macro call: "<<(*j)->getString()<<
      //"\nexpanding it to: "<<os.str()<<endl;
      string pragmaText = target->get_pragma()->get_pragma();
      string targetString = (*j)->getString();
      string replacement = os.str();
      // repeat until not found
      size_t pos1 = pragmaText.find(targetString);
      while (pos1 != string::npos)
      {
        pragmaText.replace(pos1, targetString.size(), replacement);
        pos1 = pragmaText.find(targetString);
      }
       delete target->get_pragma();
       target->set_pragma(buildPragma(pragmaText));
    } // end if
  } // end for
#endif
}

//! If the given statement contains any break statements in its body, add a
//! new label below the statement and change the breaks into gotos to that
//! new label.
void SageInterface::changeBreakStatementsToGotos(SgStatement* loopOrSwitch) {
  using namespace SageBuilder;
  SgStatement* body = NULL;
  if (isSgWhileStmt(loopOrSwitch) || isSgDoWhileStmt(loopOrSwitch) ||
      isSgForStatement(loopOrSwitch)) {
    body = SageInterface::getLoopBody(isSgScopeStatement(loopOrSwitch));
  } else if (isSgSwitchStatement(loopOrSwitch)) {
    body = isSgSwitchStatement(loopOrSwitch)->get_body();
  }
  ROSE_ASSERT (body);
  std::vector<SgBreakStmt*> breaks = SageInterface::findBreakStmts(body);
  if (!breaks.empty()) {
    static int breakLabelCounter = 0;
    SgLabelStatement* breakLabel =
      buildLabelStatement("breakLabel" +
StringUtility::numberToString(++breakLabelCounter),
                          buildBasicBlock(),
                          isSgScopeStatement(loopOrSwitch->get_parent()));
    insertStatement(loopOrSwitch, breakLabel, false);
    for (size_t j = 0; j < breaks.size(); ++j) {
      SgGotoStatement* newGoto = buildGotoStatement(breakLabel);

  isSgStatement(breaks[j]->get_parent())->replace_statement(breaks[j],
  newGoto);
        newGoto->set_parent(breaks[j]->get_parent());
      }
    }
  }

  bool SageInterface::isStructDeclaration(SgNode* node)
  {
    ROSE_ASSERT(node!=NULL);
    SgClassDeclaration *decl = isSgClassDeclaration(node);
    if (decl==NULL)
      return false;
    else
       return (decl->get_class_type() == SgClassDeclaration::e_struct)? true:false;
  }

  bool SageInterface::isUnionDeclaration(SgNode* node)
  {
    ROSE_ASSERT(node!=NULL);
    SgClassDeclaration *decl = isSgClassDeclaration(node);
    if (decl==NULL)
      return false;
    else
       return (decl->get_class_type() == SgClassDeclaration::e_union)? true:false;
  }


void  SageInterface::movePreprocessingInfo (SgStatement* stmt_src,  SgStatement* stmt_dst, PreprocessingInfo::RelativePositionType src_position/* =PreprocessingInfo::undef */,
                            PreprocessingInfo::RelativePositionType dst_position/* =PreprocessingInfo::undef */, bool usePrepend /*= false */)
{
  ROSE_ASSERT(stmt_src != NULL);
  ROSE_ASSERT(stmt_dst != NULL);
  AttachedPreprocessingInfoType* infoList=stmt_src->getAttachedPreprocessingInfo();
  AttachedPreprocessingInfoType* infoToRemoveList = new AttachedPreprocessingInfoType();

  if (infoList == NULL) return;
  for (Rose_STL_Container<PreprocessingInfo*>::iterator i= (*infoList).begin();
      i!=(*infoList).end();i++)
  {
    PreprocessingInfo * info=dynamic_cast<PreprocessingInfo *> (*i);
    ROSE_ASSERT(info != NULL);

    if ( (info->getTypeOfDirective()==PreprocessingInfo::CpreprocessorIncludeDeclaration )||
        (info->getTypeOfDirective()==PreprocessingInfo::CpreprocessorDefineDeclaration )||
        (info->getTypeOfDirective()==PreprocessingInfo::CpreprocessorUndefDeclaration)||
        (info->getTypeOfDirective()==PreprocessingInfo::CpreprocessorIfdefDeclaration )||
        (info->getTypeOfDirective()==PreprocessingInfo::CpreprocessorIfndefDeclaration )||
        (info->getTypeOfDirective()==PreprocessingInfo::CpreprocessorIfDeclaration )||
        (info->getTypeOfDirective()==PreprocessingInfo::CpreprocessorElseDeclaration )||
        (info->getTypeOfDirective()==PreprocessingInfo::CpreprocessorElifDeclaration )||
        (info->getTypeOfDirective()==PreprocessingInfo::C_StyleComment)||
        (info->getTypeOfDirective()==PreprocessingInfo::CpreprocessorEndifDeclaration )
       )
    {
      // move all source preprocessing info if the desired source type is not specified or matching
      // a specified desired source type
      if ( src_position == PreprocessingInfo::undef || info->getRelativePosition()==src_position)
      {
        if (usePrepend)
          // addToAttachedPreprocessingInfo() is poorly designed, the last parameter is used
          // to indicate appending or prepending by reusing the type of relative position.
          // this is very confusing for users
          stmt_dst->addToAttachedPreprocessingInfo(info,PreprocessingInfo::before);
        else
          stmt_dst->addToAttachedPreprocessingInfo(info,PreprocessingInfo::after);

        (*infoToRemoveList).push_back(*i);
      }
#if 0
      else if (info->getRelativePosition()==src_position)
      {

        if (usePrepend)
        {
        }
        else
          stmt_dst->addToAttachedPreprocessingInfo(info,PreprocessingInfo::after);

        (*infoToRemoveList).push_back(*i);
      } // if src_position
#endif
      // adjust dst position if needed
      if (dst_position != PreprocessingInfo::undef)
        info->setRelativePosition(dst_position);
    } // end if
  }// end for

  // Remove the element from the list of comments at the current astNode
  AttachedPreprocessingInfoType::iterator j;
  for (j = (*infoToRemoveList).begin(); j != (*infoToRemoveList).end(); j++)
    infoList->erase( find(infoList->begin(),infoList->end(),*j) );

}

//----------------------------
// Sometimes, the preprocessing info attached to a declaration has to be
// moved 'up' if another declaration is inserted before it.
// This is a workaround for the broken LowLevelRewrite::insert() and the private
// LowLevelRewrite::reassociatePreprocessorDeclarations()
//
// input:
//     *stmt_dst: the new inserted declaration
//     *stmt_src: the existing declaration with preprocessing information
// tasks:
//     judge if stmt_src has propressingInfo with headers, ifdef, etc..
//     add them into stmt_dst
//     delete them from stmt_dst
// More general usage: move preprocessingInfo of stmt_src to stmt_dst, should used before any
//           LoweLevel::remove(stmt_src)
void SageInterface::moveUpPreprocessingInfo(SgStatement * stmt_dst, SgStatement * stmt_src,
      PreprocessingInfo::RelativePositionType src_position/*=PreprocessingInfo::undef*/,
      PreprocessingInfo::RelativePositionType dst_position/*=PreprocessingInfo::undef*/,
      bool usePrepend /*= false */)
{
  movePreprocessingInfo (stmt_src, stmt_dst, src_position, dst_position, usePrepend);
} // moveUpPreprocessingInfo()



/*!
 *  \brief Returns 'true' iff the given info object is valid and has
 *  the specified position.
 */
static bool isNotRelPos (const PreprocessingInfo* info,
             PreprocessingInfo::RelativePositionType pos)
{
  return info && (info->getRelativePosition () != pos);
}

/*!
 *  \brief Returns 'true' iff the given info object is valid and does
 *  not have the specified position.
 */
static bool isRelPos (const PreprocessingInfo* info,
             PreprocessingInfo::RelativePositionType pos)
{
  return info && !isNotRelPos (info, pos);
}


//!Cut preprocessing information from a source node and save it into a buffer. Used in combination of pastePreprocessingInfo(). The cut-paste operaation is similar to moveUpPreprocessingInfo() but it is more flexible in that the destination node only need to be known until the paste action.
void SageInterface::cutPreprocessingInfo (SgLocatedNode* src_node, PreprocessingInfo::RelativePositionType pos, AttachedPreprocessingInfoType& save_buf)
{
  ROSE_ASSERT(src_node != NULL);

  AttachedPreprocessingInfoType* info = src_node->get_attachedPreprocessingInfoPtr();
  if (info)
  {
    remove_copy_if (info->begin (), info->end (),
        back_inserter (save_buf),
        bind2nd (ptr_fun (isNotRelPos), pos));

    // DQ (9/26/2007): Commented out as part of move from std::list to std::vector
    // info->remove_if (bind2nd (ptr_fun (isRelPos), pos));
    // Liao (10/3/2007), implement list::remove_if for vector, which lacks sth. like erase_if
    AttachedPreprocessingInfoType::iterator new_end =
      remove_if(info->begin(),info->end(),bind2nd(ptr_fun (isRelPos), pos));
    info->erase(new_end, info->end());
  }
}

static AttachedPreprocessingInfoType *
createInfoList (SgLocatedNode* s)
{
  ROSE_ASSERT (s);
  AttachedPreprocessingInfoType* info_list = s->get_attachedPreprocessingInfoPtr ();
  if (!info_list)
  {
    info_list = new AttachedPreprocessingInfoType;
    ROSE_ASSERT (info_list);
    s->set_attachedPreprocessingInfoPtr (info_list);
  }

  // Guarantee a non-NULL pointer.
  ROSE_ASSERT (info_list);
  return info_list;
}

//!Cut preprocessing information from a source node and save it into a buffer. Used in combination of pastePreprocessingInfo()
void SageInterface::pastePreprocessingInfo (SgLocatedNode* dst_node, PreprocessingInfo::RelativePositionType pos, AttachedPreprocessingInfoType& save_buf)
{
  if (save_buf.size()==0) return;
  // if front
  AttachedPreprocessingInfoType* info = createInfoList (dst_node);
  ROSE_ASSERT (info);

  // DQ (9/26/2007): Commented out as part of move from std::list to std::vector
  // printf ("Commented out front_inserter() as part of move from std::list to std::vector \n");
  // copy (save_buf.rbegin (), save_buf.rend (), front_inserter (*info));

  // Liao (10/3/2007), vectors can only be appended at the rear
  if (pos==PreprocessingInfo::before)
  {
    for(AttachedPreprocessingInfoType::reverse_iterator i=save_buf.rbegin();i!=save_buf.rend();i++)
      info->insert(info->begin(),*i);
  }
  // if back
  else if (pos==PreprocessingInfo::after)
    copy (save_buf.begin (), save_buf.end (), back_inserter (*info));
  else if (pos==PreprocessingInfo::inside)
  {
    copy (save_buf.begin (), save_buf.end (), back_inserter (*info));
    cerr<<"SageInterface::pastePreprocessingInfo() pos==PreprocessingInfo::inside is not supported."<<endl;
    save_buf[0]->display("ttt");
  }
}

void SageInterface::dumpPreprocInfo (SgLocatedNode* locatedNode)
{
  ROSE_ASSERT(locatedNode != NULL);
  AttachedPreprocessingInfoType *comments =
    locatedNode->getAttachedPreprocessingInfo ();

  if (comments != NULL)
  {
    printf ("-----------------------------------------------\n");
    printf ("Found an IR node (at %p of type: %s) in file %s \n",
        locatedNode, locatedNode->class_name ().c_str (),
        (locatedNode->get_file_info ()->get_filenameString ()).c_str ());
    int counter = 0;
    AttachedPreprocessingInfoType::iterator i;
    for (i = comments->begin (); i != comments->end (); i++)
    {
      printf
        ("with attached preprocessingInfo numbering #%d :------------- \nclassification= %s:\nString format:%s\n",
         counter++,
         PreprocessingInfo::directiveTypeName ((*i)->getTypeOfDirective ()).
         c_str (), (*i)->getString ().c_str ());
      if ((*i)->getRelativePosition () == PreprocessingInfo::inside)
        printf ("relative position is: inside\n");
      else
        printf ("relative position is: %s\n", \
            ((*i)->getRelativePosition () == PreprocessingInfo::before) ? "before" : "after");
    }
  }
  else
  {
    printf ("No attached preprocessing info. (at %p of type: %s): \n", locatedNode,
        locatedNode->sage_class_name ());
  }
}

/// \brief             helper function that ensures that a certain child node
///                    of a statement is a basic block.
/// \tparam ParentNode the type of the statement
/// \param stmt        the parent statement
/// \param setter      the member function to retrieve the child node
/// \param getter      the member function to set the child node
/// \return            the pointer stmt.getter() if it is a basic block
///                    a new basic block containing stmt.getter() otherewise
/// \details           if stmt.getter() is not a basic block,
///                    ensureBasicBlock_aux also sets
///                    stmt.setter(new basic block)
template <class ParentNode>
static
SgBasicBlock* ensureBasicBlock_aux( ParentNode& stmt,
                                    SgStatement* (ParentNode::*getter) () const,
                                    void (ParentNode::*setter) (SgStatement*)
                                  )
{
  SgStatement* const body_stmt = (stmt.*getter)();
  SgBasicBlock*      basicblock = isSgBasicBlock(body_stmt);

  if (basicblock == NULL) {
    basicblock = SageBuilder::buildBasicBlock(body_stmt);
    (stmt.*setter)(basicblock);
    basicblock->set_parent(&stmt);
  }

  ROSE_ASSERT (basicblock != NULL);
  return basicblock;
}

SgBasicBlock* SageInterface::ensureBasicBlockAsBodyOfFor(SgForStatement* fs)
{
  SgStatement* b = fs->get_loop_body();
  if (!isSgBasicBlock(b)) {
    b = SageBuilder::buildBasicBlock(b);
    fs->set_loop_body(b);
    b->set_parent(fs);
  }
  ROSE_ASSERT (isSgBasicBlock(b));
  return isSgBasicBlock(b);
}

SgBasicBlock* SageInterface::ensureBasicBlockAsBodyOfCaseOption(SgCaseOptionStmt* cs)
{
  SgStatement* b = cs->get_body();
  if (!isSgBasicBlock(b)) {
    b = SageBuilder::buildBasicBlock(b);
    cs->set_body(b);
    b->set_parent(cs);
  }
  ROSE_ASSERT (isSgBasicBlock(b));
  return isSgBasicBlock(b);
}

SgBasicBlock* SageInterface::ensureBasicBlockAsBodyOfDefaultOption(SgDefaultOptionStmt * cs)
{
  SgStatement* b = cs->get_body();
  if (!isSgBasicBlock(b)) {
    b = SageBuilder::buildBasicBlock(b);
    cs->set_body(b);
    b->set_parent(cs);
  }
  ROSE_ASSERT (isSgBasicBlock(b));
  return isSgBasicBlock(b);
}

SgBasicBlock* SageInterface::ensureBasicBlockAsBodyOfUpcForAll(SgUpcForAllStatement* fs)
{
  ROSE_ASSERT (fs != NULL);

  return ensureBasicBlock_aux(*fs, &SgUpcForAllStatement::get_loop_body, &SgUpcForAllStatement::set_loop_body);
}

  SgBasicBlock* SageInterface::ensureBasicBlockAsBodyOfWhile(SgWhileStmt* fs) {
    SgStatement* b = fs->get_body();
    if (!isSgBasicBlock(b)) {
      b = SageBuilder::buildBasicBlock(b);
      fs->set_body(b);
      b->set_parent(fs);
    }
    ROSE_ASSERT (isSgBasicBlock(b));
    return isSgBasicBlock(b);
  }

  SgBasicBlock* SageInterface::ensureBasicBlockAsBodyOfDoWhile(SgDoWhileStmt* fs) {
    SgStatement* b = fs->get_body();
    if (!isSgBasicBlock(b)) {
      b = SageBuilder::buildBasicBlock(b);
      fs->set_body(b);
      b->set_parent(fs);
    }
    ROSE_ASSERT (isSgBasicBlock(b));
    return isSgBasicBlock(b);
  }

  SgBasicBlock* SageInterface::ensureBasicBlockAsBodyOfSwitch(SgSwitchStatement* fs) {
    SgStatement* b = fs->get_body();
    if (!isSgBasicBlock(b)) {
      b = SageBuilder::buildBasicBlock(b);
      fs->set_body(b);
      b->set_parent(fs);
    }
    ROSE_ASSERT (isSgBasicBlock(b));
    return isSgBasicBlock(b);
  }

  SgBasicBlock* SageInterface::ensureBasicBlockAsTrueBodyOfIf(SgIfStmt* fs) {
    SgStatement* b = fs->get_true_body();
    if (!isSgBasicBlock(b)) {
      b = SageBuilder::buildBasicBlock(b);
      fs->set_true_body(b);
      b->set_parent(fs);
    }
    ROSE_ASSERT (isSgBasicBlock(b));
    return isSgBasicBlock(b);
  }

  SgBasicBlock* SageInterface::ensureBasicBlockAsFalseBodyOfIf(SgIfStmt* fs , bool createEmptyBody /* = true*/) {
    SgStatement* b = fs->get_false_body();
    // if no false body at all AND no-create-empty-body
    if (!createEmptyBody && (b == NULL || isSgNullStatement(b)))
      return NULL;
    if (!isSgBasicBlock(b)) {
      b = SageBuilder::buildBasicBlock(b); // This works if b is NULL as well (producing an empty block)
      fs->set_false_body(b);
      b->set_parent(fs);
    }
    ROSE_ASSERT (isSgBasicBlock(b));
    return isSgBasicBlock(b);
  }

  SgBasicBlock* SageInterface::ensureBasicBlockAsBodyOfCatch(SgCatchOptionStmt* fs) {
    SgStatement* b = fs->get_body();
    if (!isSgBasicBlock(b)) {
      b = SageBuilder::buildBasicBlock(b);
      fs->set_body(b);
      b->set_parent(fs);
    }
    ROSE_ASSERT (isSgBasicBlock(b));
    return isSgBasicBlock(b);
  }

SgBasicBlock* SageInterface::ensureBasicBlockAsBodyOfOmpBodyStmt(SgOmpBodyStatement* fs)
{
  SgStatement* b = fs->get_body();
  if (!isSgBasicBlock(b)) {
    b = SageBuilder::buildBasicBlock(b);
    fs->set_body(b);
    b->set_parent(fs);
  }
  ROSE_ASSERT (isSgBasicBlock(b));
  return isSgBasicBlock(b);
}

bool SageInterface::isBodyStatement (SgStatement* s)
{
  bool rt = false;
  ROSE_ASSERT(s);
  SgLocatedNode* p = isSgLocatedNode(s->get_parent());
  ROSE_ASSERT(p);

  switch (p->variantT())
  {
    case V_SgForStatement:
      {
        if (isSgForStatement(p)->get_loop_body() == s)
          rt = true;
        break;
      }
    case V_SgUpcForAllStatement: // PP
      {
        SgUpcForAllStatement& upcforall = *isSgUpcForAllStatement(p);
        if (upcforall.get_loop_body() == s)
          rt = true;
        break;
      }
    case V_SgWhileStmt:
      {
        if (isSgWhileStmt(p)->get_body() == s)
          rt = true;
        break;
      }
    case V_SgDoWhileStmt:
      {
        if (isSgDoWhileStmt(p)->get_body() == s)
          rt = true;
        break;
      }
    case V_SgSwitchStatement:
      {
        if (isSgSwitchStatement(p)->get_body() == s)
          rt = true;
        break;
      }
    case V_SgCaseOptionStmt:
      {
          if (isSgCaseOptionStmt(p)->get_body() == s)
              rt = true;
          break;
      }
    case V_SgDefaultOptionStmt:
      {
          if (isSgDefaultOptionStmt(p)->get_body() == s)
              rt = true;
          break;
      }
    case V_SgCatchOptionStmt:
      {
        if (isSgCatchOptionStmt(p)->get_body() == s)
          rt = true;
        break;
      }
    case V_SgIfStmt:
      {
        if (isSgIfStmt(p)->get_true_body() == s)
          rt = true;
        else if (isSgIfStmt(p)->get_false_body() == s)
          rt = true;
        break;
      }
    default:
      {
        if (isSgOmpBodyStatement(p))
          rt = true;
        break;
      }
  }
  return rt;
}

//! Make a single statement body to be a basic block. Its parent is if, while, catch, or upc_forall
//etc.
SgBasicBlock * SageInterface::makeSingleStatementBodyToBlock(SgStatement* singleStmt)
{
  ROSE_ASSERT (singleStmt != NULL); // not NULL
  ROSE_ASSERT (isSgBasicBlock(singleStmt) == NULL); //not a block
  ROSE_ASSERT (isBodyStatement(singleStmt) == true); // is a body statement

  SgBasicBlock* rt = NULL;



  SgStatement* s = singleStmt;
  SgLocatedNode* p = isSgLocatedNode(s->get_parent());
  ROSE_ASSERT(p);
  switch (p->variantT())
  {
    case V_SgForStatement:
      {
        if (isSgForStatement(p)->get_loop_body() == s)
          rt = ensureBasicBlockAsBodyOfFor(isSgForStatement(p));
        break;
      }
    case V_SgUpcForAllStatement: // PP
      {
        SgUpcForAllStatement& upcforall = *isSgUpcForAllStatement(p);

        if (upcforall.get_loop_body() == s)
          rt = ensureBasicBlockAsBodyOfUpcForAll(&upcforall);
        break;
      }
    case V_SgWhileStmt:
      {
        if (isSgWhileStmt(p)->get_body() == s)
          rt = ensureBasicBlockAsBodyOfWhile(isSgWhileStmt(p));
        break;
      }
    case V_SgDoWhileStmt:
      {
        if (isSgDoWhileStmt(p)->get_body() == s)
          rt = ensureBasicBlockAsBodyOfDoWhile(isSgDoWhileStmt(p));
        break;
      }
    case V_SgSwitchStatement:
      {
        if (isSgSwitchStatement(p)->get_body() == s)
          rt = ensureBasicBlockAsBodyOfSwitch(isSgSwitchStatement(p));
        break;
      }
    case V_SgCaseOptionStmt:
      {
          if (isSgCaseOptionStmt(p)->get_body() == s)
            rt = ensureBasicBlockAsBodyOfCaseOption(isSgCaseOptionStmt(p));
          break;
      }
    case V_SgDefaultOptionStmt:
      {
          if (isSgDefaultOptionStmt(p)->get_body() == s)
            rt = ensureBasicBlockAsBodyOfDefaultOption(isSgDefaultOptionStmt(p));
          break;
      }          
    case V_SgCatchOptionStmt:
      {
        if (isSgCatchOptionStmt(p)->get_body() == s)
          rt = ensureBasicBlockAsBodyOfCatch(isSgCatchOptionStmt(p));
        break;
      }
    case V_SgIfStmt:
      {
        if (isSgIfStmt(p)->get_true_body() == s)
          rt = ensureBasicBlockAsTrueBodyOfIf(isSgIfStmt(p));
        else if (isSgIfStmt(p)->get_false_body() == s)
          rt = ensureBasicBlockAsFalseBodyOfIf(isSgIfStmt(p));
        break;
      }
    default:
      {
        if (isSgOmpBodyStatement(p))
        {
          rt = ensureBasicBlockAsBodyOfOmpBodyStmt(isSgOmpBodyStatement(p));
        }
        break;
      }
  }
  ROSE_ASSERT (rt != NULL); // the input statement has been confirmed to be a body statement, it must have being processed to be a basic block at this point.
  return rt;
}

#if 0
SgLocatedNode* SageInterface::ensureBasicBlockAsParent(SgStatement* s)
{
        ROSE_ASSERT(s);

        //Vulov: The parent of a statement is not necessarily a statement. It could be SgStatementExpression
        SgLocatedNode* p = isSgLocatedNode(s->get_parent());
        ROSE_ASSERT(p);
        switch (p->variantT())
        {
                case V_SgBasicBlock: return isSgBasicBlock(p);
                case V_SgForStatement:
                {
                        if (isSgForStatement(p)->get_loop_body() == s)
                                return ensureBasicBlockAsBodyOfFor(isSgForStatement(p));
                        else if (isSgForStatement(p)->get_test() == s)
                        {
                        }
                        else if (isSgForStatement(p)->get_for_init_stmt() == s)
                        {
                        }
                        else ROSE_ASSERT(false);
                        break;
                }
                case V_SgUpcForAllStatement: // PP
                {
                  SgUpcForAllStatement& upcforall = *isSgUpcForAllStatement(p);

                  if (upcforall.get_loop_body() == s)
                    return ensureBasicBlockAsBodyOfUpcForAll(&upcforall);

                  ROSE_ASSERT(  (s == upcforall.get_for_init_stmt())
                      || (s == upcforall.get_test())
                      );
                  break;
                }
                case V_SgWhileStmt:
                {
                        if (isSgWhileStmt(p)->get_body() == s)
                                return ensureBasicBlockAsBodyOfWhile(isSgWhileStmt(p));
                        else if (isSgWhileStmt(p)->get_condition() == s)
                        {
                        }
                        else ROSE_ASSERT(false);
                        break;
                }
                case V_SgDoWhileStmt:
                {
                        if (isSgDoWhileStmt(p)->get_body() == s)
                                return ensureBasicBlockAsBodyOfDoWhile(isSgDoWhileStmt(p));
                        else if (isSgDoWhileStmt(p)->get_condition() == s)
                        {
                        }
                        else ROSE_ASSERT(false);
                        break;
                }
                case V_SgSwitchStatement:
                {
                        if (isSgSwitchStatement(p)->get_body() == s)
                                return ensureBasicBlockAsBodyOfSwitch(isSgSwitchStatement(p));
                        else if (isSgSwitchStatement(p)->get_item_selector() == s)
                        {
                        }
                        else ROSE_ASSERT(false);
                        break;
                }
                case V_SgCatchOptionStmt:
                {
                        if (isSgCatchOptionStmt(p)->get_body() == s)
                                return ensureBasicBlockAsBodyOfCatch(isSgCatchOptionStmt(p));
                        else if (isSgCatchOptionStmt(p)->get_condition() == s)
                        {
                        }
                        else ROSE_ASSERT(false);
                        break;
                }
                case V_SgIfStmt:
                {
                        if (isSgIfStmt(p)->get_true_body() == s)
                                return ensureBasicBlockAsTrueBodyOfIf(isSgIfStmt(p));
                        else if (isSgIfStmt(p)->get_false_body() == s)
                                return ensureBasicBlockAsFalseBodyOfIf(isSgIfStmt(p));
                        else if (isSgIfStmt(p)->get_conditional() == s)
                        {
                        }
                        else ROSE_ASSERT(false);
                        break;
                }
                default:
                {
                        if (isSgOmpBodyStatement(p))
                        {
                                return ensureBasicBlockAsBodyOfOmpBodyStmt(isSgOmpBodyStatement(p));
                        }
                        else
                                // Liao, 7/3/2008 We allow other conditions to fall through,
                                // they are legal parents with list of statements as children.
                                //cerr << "Unhandled parent block:"<< p->class_name() << endl;
                                // ROSE_ASSERT (!"Bad parent in ensureBasicBlockAsParent");
                                break;
                }
        }
        return p;
}
#endif
  void SageInterface::changeAllLoopBodiesToBlocks(SgNode* top) {
    cerr<<"Warning: SageInterface::changeAllLoopBodiesToBlocks() is being replaced by SageInterface::changeAllBodiesToBlocks()."<<endl;
    cerr<<"Please use SageInterface::changeAllBodiesToBlocks() if you can."<<endl;
        changeAllBodiesToBlocks(top) ;
  }

  void SageInterface::changeAllBodiesToBlocks(SgNode* top, bool createEmptyBody /*= true*/ ) {
    class Visitor: public AstSimpleProcessing {
      public: 
      bool allowEmptyBody; 
      Visitor (bool flag):allowEmptyBody(flag) {}
      virtual void visit(SgNode* n) {
        switch (n->variantT()) {
          case V_SgForStatement: {
            ensureBasicBlockAsBodyOfFor(isSgForStatement(n));
            break;
          }
          case V_SgWhileStmt: {
            ensureBasicBlockAsBodyOfWhile(isSgWhileStmt(n));
            break;
          }
          case V_SgDoWhileStmt: {
            ensureBasicBlockAsBodyOfDoWhile(isSgDoWhileStmt(n));
            break;
          }
          case V_SgSwitchStatement: {
            ensureBasicBlockAsBodyOfSwitch(isSgSwitchStatement(n));
            break;
          }
          case V_SgIfStmt: {
            ensureBasicBlockAsTrueBodyOfIf(isSgIfStmt(n));
            ensureBasicBlockAsFalseBodyOfIf(isSgIfStmt(n), allowEmptyBody);
            break;
          }
          case V_SgCatchOptionStmt: {
            ensureBasicBlockAsBodyOfCatch(isSgCatchOptionStmt(n));
            break;
          }
          case V_SgUpcForAllStatement: {
            ensureBasicBlockAsBodyOfUpcForAll(isSgUpcForAllStatement(n));
            break;
          }

          default:
            {
              if (isSgOmpBodyStatement(n))
                ensureBasicBlockAsBodyOfOmpBodyStmt(isSgOmpBodyStatement(n));
              break;
            }
        }
      }
    };
    Visitor(createEmptyBody).traverse(top, postorder);
  }


// Replace a given expression with a list of statements produced by a
// generator.  The generator, when given a variable as input, must produce
// some code which leaves its result in the given variable.  The output
// from the generator is then inserted into the original program in such a
// way that whenever the expression had previously been evaluated, the
// statements produced by the generator are run instead and their result is
// used in place of the expression.
// Assumptions: not currently traversing from or the statement it is in
void
SageInterface::replaceExpressionWithStatement(SgExpression* from, StatementGenerator* to)
   {
  // DQ (3/11/2006): The problem here is that the test expression for a "for loop" (SgForStmt)
  // is assumed to be a SgExpression.  This was changed in Sage III as part of a bugfix and so
  // the original assumptions upon which this function was based are not incorrect, hence the bug!
  // Note that a number of cases were changed when this fix was made to SageIII (see documentation
  // for SgScopeStatement).

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
     SgStatement*           enclosingStatement      = getStatementOfExpression(from);
     SgExprStatement*       exprStatement           = isSgExprStatement(enclosingStatement);

     SgForStatement*        forStatement            = isSgForStatement(enclosingStatement);
     SgReturnStmt*          returnStatement         = isSgReturnStmt(enclosingStatement);
     SgVariableDeclaration* varDeclarationStatement = isSgVariableDeclaration(enclosingStatement);


  // DQ (3/11/2006): Bugfix for special cases of conditional that are either SgStatement or SgExpression IR nodes.

     ROSE_ASSERT (exprStatement || forStatement || returnStatement || varDeclarationStatement);

     if (varDeclarationStatement)
        {
          replaceSubexpressionWithStatement(from, to);
        }
       else
        {
          SgExpression* root = getRootOfExpression(from);
          ROSE_ASSERT (root);
       // printf ("root = %p \n",root);
             {
               if (forStatement && forStatement->get_increment() == root)
                  {
                 // printf ("Convert step of for statement \n");
                 // Convert step of for statement
                 // for (init; test; e) body; (where e contains from) becomes
                 // for (init; test; ) {
                 //   body (with "continue" changed to "goto label");
                 //   label: e;
                 // }
                 // std::cout << "Converting for step" << std::endl;
                    SgExprStatement* incrStmt = SageBuilder::buildExprStatement(forStatement->get_increment());
                    forStatement->get_increment()->set_parent(incrStmt);

                    SageInterface::addStepToLoopBody(forStatement, incrStmt);
                    SgNullExpression* ne = buildNullExpression();
                    forStatement->set_increment(ne);
                    ne->set_parent(forStatement);
                    replaceSubexpressionWithStatement(from, to);
                  }
                 else
                  {
                    SgStatement* enclosingStmtParent = isSgStatement(enclosingStatement->get_parent());
                    assert (enclosingStmtParent);
                    SgWhileStmt* whileStatement = isSgWhileStmt(enclosingStmtParent);
                    SgDoWhileStmt* doWhileStatement = isSgDoWhileStmt(enclosingStmtParent);
                    SgIfStmt* ifStatement = isSgIfStmt(enclosingStmtParent);
                    SgSwitchStatement* switchStatement = isSgSwitchStatement(enclosingStmtParent);
                    SgForStatement* enclosingForStatement = isSgForStatement(enclosingStmtParent);
                  if (enclosingForStatement && enclosingForStatement->get_test() == exprStatement)
                     {
                    // printf ("Found the test in the for loop \n");
                    // ROSE_ASSERT(false);

                    // Convert test of for statement:
                    // for (init; e; step) body; (where e contains from) becomes
                    // for (init; true; step) {
                    //   bool temp;
                    //   temp = e;
                    //   if (!temp) break;
                    //   body;
                    // }
                    // in which "temp = e;" is rewritten further
                    // std::cout << "Converting for test" << std::endl;
                       pushTestIntoBody(enclosingForStatement);
                       replaceSubexpressionWithStatement(from, to);
                     }
                  else if (whileStatement && whileStatement->get_condition() == exprStatement)
                     {
                    // printf ("Convert while statements \n");
                    // Convert while statement:
                    // while (e) body; (where e contains from) becomes
                    // while (true) {
                    //   bool temp;
                    //   temp = e;
                    //   if (!temp) break;
                    //   body;
                    // }
                    // in which "temp = e;" is rewritten further
                    // std::cout << "Converting while test" << std::endl;
                       pushTestIntoBody(whileStatement);
                    // FixSgTree(whileStatement);
                       replaceSubexpressionWithStatement(from, to);
                     }
                  else if (doWhileStatement && doWhileStatement->get_condition() == exprStatement)
                    {
         // printf ("Convert do-while statements \n");
                   // Convert do-while statement:
                   // do body; while (e); (where e contains from) becomes
                   // {bool temp = true;
                   //  do {
                   //    body (with "continue" changed to "goto label";
                   //    label:
                   //    temp = e;} while (temp);}
                   // in which "temp = e;" is rewritten further
                   // std::cout << "Converting do-while test" << std::endl;
                      SgBasicBlock* new_statement = SageBuilder::buildBasicBlock();
                   // printf ("Building IR node #14: new SgBasicBlock = %p \n",new_statement);
                      assert (doWhileStatement->get_parent());
                      new_statement->set_parent(doWhileStatement->get_parent());
                      myStatementInsert(doWhileStatement, new_statement, false);
                      SageInterface::myRemoveStatement(doWhileStatement);
                      SgName varname = "rose__temp"; // Does not need to be unique, but must not be used in user code anywhere
                      SgAssignInitializer* assignInitializer = buildAssignInitializer(
                          buildBoolValExp(true));
                      //SageInterface::getBoolType(doWhileStatement));
                      SgVariableDeclaration* new_decl = buildVariableDeclaration(
                        varname, buildBoolType(), assignInitializer, new_statement);

                      SgInitializedName* initname = new_decl->get_variables().back();
                      initname->set_scope(new_statement);

         // DQ (12/14/2006): set the parent of the SgAssignInitializer to the variable (SgInitializedName).
            assignInitializer->set_parent(initname);

                      SgVariableSymbol* varsym = new SgVariableSymbol(initname);
                      new_statement->insert_symbol(varname, varsym);
                      varsym->set_parent(new_statement->get_symbol_table());
                      SageInterface::appendStatement(new_decl, new_statement);
                      SageInterface::appendStatement(doWhileStatement, new_statement);
                      assert (varsym);
                      SgCastExp* castExp1 = buildCastExp(root,buildBoolType());
                      SgVarRefExp* vr = buildVarRefExp(varsym);
                      vr->set_lvalue(true);

                      SgExprStatement* temp_setup = SageBuilder::buildAssignStatement(vr, castExp1);

                      SageInterface::addStepToLoopBody(doWhileStatement, temp_setup);
                      SgVarRefExp* varsymVr = buildVarRefExp(varsym);

                      SgExprStatement* condStmt = SageBuilder::buildExprStatement(varsymVr);
                      varsymVr->set_parent(condStmt);
                      doWhileStatement->set_condition(condStmt);
                      condStmt->set_parent(doWhileStatement);
                      replaceSubexpressionWithStatement(from, to);
                    }
                  else if (ifStatement && ifStatement->get_conditional() == exprStatement)
                    {
                      SgBasicBlock* new_statement = SageBuilder::buildBasicBlock();
                   // printf ("Building IR node #15: new SgBasicBlock = %p \n",new_statement);
                      assert (ifStatement->get_parent());
                      new_statement->set_parent(ifStatement->get_parent());
                      myStatementInsert(ifStatement, new_statement, false);
                      SageInterface::myRemoveStatement(ifStatement);
                      SgName varname = "rose__temp"; // Does not need to be unique, but must not be used in user code anywhere
                      SgBoolValExp* trueVal = buildBoolValExp(true);

                      SgAssignInitializer* ai = buildAssignInitializer(trueVal);

                      SgVariableDeclaration* new_decl = buildVariableDeclaration(varname,
                         buildBoolType(), ai,new_statement);
                      SgInitializedName* initname = new_decl->get_variables().back();
                      ai->set_parent(initname);
                      initname->set_scope(new_statement);
                      SgVariableSymbol* varsym = new SgVariableSymbol(initname);
                      new_statement->insert_symbol(varname, varsym);
                      varsym->set_parent(new_statement->get_symbol_table());
                      SageInterface::appendStatement(new_decl, new_statement);
                      ifStatement->set_parent(new_statement);
                      assert (varsym);

                                        SgCastExp* castExp2 = SageBuilder::buildCastExp(root, SageInterface::getBoolType(ifStatement));
                      SgVarRefExp* vr = buildVarRefExp(varsym);
                      vr->set_lvalue(true);
                      SgExprStatement* temp_setup = SageBuilder::buildAssignStatement(vr, castExp2 );
                      SageInterface::appendStatement(temp_setup, new_statement);
                      SageInterface::appendStatement(ifStatement, new_statement);
                      SgVarRefExp* vr2 = SageBuilder::buildVarRefExp(varsym);
                      SgExprStatement* es = SageBuilder::buildExprStatement(vr2);
                      ifStatement->set_conditional(es);
                      es->set_parent(ifStatement);
                      replaceSubexpressionWithStatement(from, to);
                    }
                  else if (switchStatement && switchStatement->get_item_selector() == exprStatement)
                    {
                      SgExpression* switchCond = exprStatement->get_expression();
                      ROSE_ASSERT (switchCond);
                      SgBasicBlock* new_statement = SageBuilder::buildBasicBlock();
                   // printf ("Building IR node #15: new SgBasicBlock = %p \n",new_statement);
                      assert (switchStatement->get_parent());
                      new_statement->set_parent(switchStatement->get_parent());
                      myStatementInsert(switchStatement, new_statement, false);
                      SageInterface::myRemoveStatement(switchStatement);
                      SgName varname = "rose__temp"; // Does not need to be unique, but must not be used in user code anywhere
                      switchCond->set_parent(NULL);
                      SgVariableDeclaration* new_decl = SageBuilder::buildVariableDeclaration(varname, switchCond->get_type(), SageBuilder::buildAssignInitializer(switchCond), new_statement);
                      SgVariableSymbol* varsym = SageInterface::getFirstVarSym(new_decl);
                      SageInterface::appendStatement(new_decl, new_statement);
                      switchStatement->set_parent(new_statement);
                      assert (varsym);


                      SageInterface::appendStatement(switchStatement, new_statement);
                      SgVarRefExp* vr2 = SageBuilder::buildVarRefExp(varsym);
                      SgExprStatement* es = SageBuilder::buildExprStatement(vr2);
                      switchStatement->set_item_selector(es);
                      es->set_parent(switchStatement);
                      replaceSubexpressionWithStatement(from, to);
                    }
                  else
                    {
                      // printf ("Handles expression and return statements \n");
                      // Handles expression and return statements
                      // std::cout << "Converting other statement" << std::endl;
                      replaceSubexpressionWithStatement(from, to);
                    }
                  }
             }
        }

#endif

  // printf ("Leaving replaceExpressionWithStatement(from,to) \n");
   }

//! Replace a given expression with a list of statements produced by a
//! generator.  The generator, when given a variable as input, must produce
//! some code which leaves its result in the given variable.  The output
//! from the generator is then inserted into the original program in such a
//! way that whenever the expression had previously been evaluated, the
//! statements produced by the generator are run instead and their result is
//! used in place of the expression.
//! Assumptions: not currently traversing from or the statement it is in

// Similar to replaceExpressionWithStatement, but with more restrictions.
// Assumptions: from is not within the test of a loop or if
//              not currently traversing from or the statement it is in
void SageInterface::replaceSubexpressionWithStatement(SgExpression* from, StatementGenerator* to)
   {

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
     SgStatement* stmt = getStatementOfExpression(from);

     if (isSgExprStatement(stmt))
        {
          SgExpression* top = getRootOfExpression(from);


          if (top == from)
             {
               SgStatement* generated = to->generate(0);
               isSgStatement(stmt->get_parent())->replace_statement(stmt, generated);
               generated->set_parent(stmt->get_parent());
               return;
             }
            else
             {
               if (isSgAssignOp(top) && isSgAssignOp(top)->get_rhs_operand() == from)
                  {
                    SgAssignOp* t = isSgAssignOp(top);
                    SgStatement* generated = to->generate(t->get_lhs_operand());
                    isSgStatement(stmt->get_parent())->replace_statement(stmt, generated);
                    generated->set_parent(stmt->get_parent());
                    return;
                  }
                 else
                  {
                 // printf ("In replaceSubexpressionWithStatement(): Statement not generated \n");
                  }
             }
        }

  // cout << "1: " << getStatementOfExpression(from)->unparseToString() << endl;
     SgAssignInitializer* init = splitExpression(from);
  // cout << "2: " << getStatementOfExpression(from)->unparseToString() << endl;
     convertInitializerIntoAssignment(init);
  // cout << "3: " << getStatementOfExpression(from)->unparseToString() << endl;
  // cout << "3a: " << getStatementOfExpression(from)->get_parent()->unparseToString() << endl;
     SgExprStatement* new_stmt = isSgExprStatement(getStatementOfExpression(from));
     assert (new_stmt != NULL); // Should now have this form because of conversion
     replaceAssignmentStmtWithStatement(new_stmt, to);

  // printf ("In replaceSubexpressionWithStatement: new_stmt = %p = %s \n",new_stmt,new_stmt->class_name().c_str());
  // cout << "4: " << getStatementOfExpression(from)->get_parent()->unparseToString() << endl;

#endif
   }


  // Liao, 6/27/2008
  //Tasks
  //  find all return statements
  //  rewrite it to  temp = expression; return temp; if expression is not a single value.
  //  insert s right before 'return xxx;'
  int SageInterface::instrumentEndOfFunction(SgFunctionDeclaration * func, SgStatement* s)
  {
    int result = 0;
    ROSE_ASSERT(func&&s);
  //  vector<SgReturnStmt* > stmts = findReturnStmts(func);
    Rose_STL_Container <SgNode* > stmts = NodeQuery::querySubTree(func, V_SgReturnStmt);
    //vector<SgReturnStmt*>::iterator i;
    Rose_STL_Container<SgNode*>::iterator i;
    for (i=stmts.begin();i!=stmts.end();i++)
    {
      SgReturnStmt* cur_stmt = isSgReturnStmt(*i);
      ROSE_ASSERT(cur_stmt);
      SgExpression * exp = cur_stmt->get_expression();
   // TV (05/03/2011) Catch the case "return ;" where exp is NULL
      bool needRewrite = (exp != NULL) && !(isSgValueExp(exp));
      if (needRewrite)
      {
        splitExpression(exp);
      }
       // avoid reusing the statement
      if (result>=1 )
         s = copyStatement(s);
       insertStatementBefore(cur_stmt,s);
      result ++;
    } // for
    if (stmts.size()==0 ) // a function without any return at all,
    {
      SgBasicBlock * body = func->get_definition()->get_body();
      if (body== NULL)
       {
         cout<<"In instrumentEndOfFunction(), found an empty function body.! "<<endl;
         ROSE_ASSERT(false);
       }
      appendStatement(s,body);
      result ++;
    }
    return result;
  } // instrumentEndOfFunction

  bool SageInterface::isStatic(SgDeclarationStatement* stmt)
  {
    ROSE_ASSERT(stmt);
    return ((stmt->get_declarationModifier()).get_storageModifier()).isStatic();
  } // isStatic()

  //! Set a declaration as static
  void SageInterface::setStatic(SgDeclarationStatement* stmt)
  {
    ROSE_ASSERT(stmt);
    return ((stmt->get_declarationModifier()).get_storageModifier()).setStatic();
  }

  bool SageInterface::isExtern(SgDeclarationStatement* stmt)
  {
    ROSE_ASSERT(stmt);
    return ((stmt->get_declarationModifier()).get_storageModifier()).isExtern();
  } // isExtern()


  //! Set a declaration as extern
  void SageInterface::setExtern(SgDeclarationStatement* stmt)
  {
    ROSE_ASSERT(stmt);
    return ((stmt->get_declarationModifier()).get_storageModifier()).setExtern();
  }


  unsigned long long SageInterface::getIntegerConstantValue(SgValueExp* expr) {
    switch (expr->variantT()) {
      case V_SgCharVal: return (long long)(isSgCharVal(expr)->get_value());
      case V_SgUnsignedCharVal: return isSgUnsignedCharVal(expr)->get_value();
      case V_SgShortVal: return (long long)(isSgShortVal(expr)->get_value());
      case V_SgUnsignedShortVal: return isSgUnsignedShortVal(expr)->get_value();
      case V_SgIntVal: return (long long)(isSgIntVal(expr)->get_value());
      case V_SgUnsignedIntVal: return isSgUnsignedIntVal(expr)->get_value();
      case V_SgLongIntVal: return (long long)(isSgLongIntVal(expr)->get_value());
      case V_SgUnsignedLongVal: return isSgUnsignedLongVal(expr)->get_value();
      case V_SgLongLongIntVal: return isSgLongLongIntVal(expr)->get_value();
      case V_SgUnsignedLongLongIntVal: return isSgUnsignedLongLongIntVal(expr)->get_value();
      default: ROSE_ASSERT (!"Bad kind in getIntegerConstantValue");
    }

    ROSE_ASSERT (!"Bad kind return in getIntegerConstantValue");
        return 0;
  }


// tps : 28 Oct 2008 - support for finding the main interpretation
// rpm : 18 Sep 2009 - rewritten to support multiple files per interpretation
/** Returns the "main" interpretation. "Main" is defined as the first interpretation that points to a header of the supplied
 *  file. If the supplied file has more than one header then the interpretation must point to this file's PE header. */
SgAsmInterpretation *
SageInterface::getMainInterpretation(SgAsmGenericFile *file)
{
    SgBinaryComposite *binary = getEnclosingNode<SgBinaryComposite>(file);
    ROSE_ASSERT(binary!=NULL);

    /* Find the only header or the PE header of this file */
    SgAsmGenericHeader *requisite_header = NULL; /*the returned interpretation must point to this header*/
    const SgAsmGenericHeaderPtrList &headers = file->get_headers()->get_headers();
    if (1==headers.size()) {
        requisite_header = headers[0];
    } else {
        for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
            if (isSgAsmPEFileHeader(*hi)) {
                requisite_header = isSgAsmPEFileHeader(*hi);
                break;
            }
        }
    }
    ROSE_ASSERT(requisite_header!=NULL);

    /* Find an interpretation that points to this header */
    const SgAsmInterpretationPtrList &interps = binary->get_interpretations()->get_interpretations();
    for (SgAsmInterpretationPtrList::const_iterator ii=interps.begin(); ii!=interps.end(); ++ii) {
        const SgAsmGenericHeaderPtrList &headers = (*ii)->get_headers()->get_headers();
        for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
            if ((*hi)==requisite_header)
                return *ii;
        }
    }

    ROSE_ASSERT(!"no appropriate interpretation");
    return NULL;
}

class CollectDependentDeclarationsCopyType : public SgCopyHelp
   {
     public:
       // This is the simpliest possible version of a deep copy SgCopyHelp::copyAst() member function.
       // Note that the root of the does not have its file info set like its children.
          virtual SgNode *copyAst(const SgNode *n)
             {
#if 1
            // DQ (2/26/2009): This defines a simple concept of "deep" copy. It forms a more testable building block, I hope.
               SgNode* copy = n->copy(*this);
#else
            // DQ (2/26/2009): I am giving up for now on this more elegant approach, in favor of something I can tests and debug!
            // This defines a more complex concept of mostly "deep" copy, except for defining function declarations
            // which are converted to non-defining declarations.  However, this level of complexity is difficult to
            // support and debug, so we are switching to a simpler approach of just using the "deep" copy as a building
            // block and then using a second pass to transform defining declarations to be non-defining declarations
            // were required.  This should be easier to test and debug, I hope.
               SgNode* copy = NULL;
               const SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);

            // For function declarations we don't want to do deep copies on defining declarations
            // since that would violate the One-time Definition Rule (ODR).
            // Note that this is only important for nested function in a declaration being copied,
            // since the SgCopyHelp class is not used at the top level of the AST copy mechanism.
               if (functionDeclaration != NULL)
                  {
                    printf ("In CollectDependentDeclarationsCopyType: functionDeclaration = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),SageInterface::get_name(functionDeclaration).c_str());
                    printf ("In CollectDependentDeclarationsCopyType: Copy mechanism appied to SgFunctionDeclaration functionDeclaration->get_firstNondefiningDeclaration() = %p \n",functionDeclaration->get_firstNondefiningDeclaration());
                    if (functionDeclaration->get_firstNondefiningDeclaration() != NULL)
                       {
                         printf ("Exiting before getting here \n");
                         ROSE_ASSERT(false);

                      // Make a copy
                         copy = functionDeclaration->get_firstNondefiningDeclaration()->copy(*this);
                       }
                      else
                       {
                      // Build a function prototype, but what scope should be used?
                         ROSE_ASSERT(functionDeclaration->get_scope() != NULL);
                         const SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(functionDeclaration);
                         if (memberFunctionDeclaration != NULL)
                            {
                              copy = SageBuilder::buildNondefiningMemberFunctionDeclaration(memberFunctionDeclaration,memberFunctionDeclaration->get_scope());
                            }
                           else
                            {
                              copy = SageBuilder::buildNondefiningFunctionDeclaration(functionDeclaration,functionDeclaration->get_scope());
                            }

                         SgFunctionDeclaration* copy_functionDeclaration = isSgFunctionDeclaration(copy);

                      // In the case of a member function in a class there in no non-defining declaration (see moreTest4.cpp).
                      // non-member function can also sometimes not have a non-defining declaration.
                         if (functionDeclaration->get_firstNondefiningDeclaration() != NULL)
                            {
                              ROSE_ASSERT(TransformationSupport::getSourceFile(functionDeclaration) == TransformationSupport::getSourceFile(functionDeclaration->get_firstNondefiningDeclaration()));
                              ROSE_ASSERT(TransformationSupport::getSourceFile(functionDeclaration->get_scope()) == TransformationSupport::getSourceFile(functionDeclaration->get_firstNondefiningDeclaration()));
                            }

                         ROSE_ASSERT(copy_functionDeclaration != NULL);
                         ROSE_ASSERT(copy_functionDeclaration->get_firstNondefiningDeclaration() != NULL);
                         ROSE_ASSERT(TransformationSupport::getSourceFile(copy_functionDeclaration) == TransformationSupport::getSourceFile(copy_functionDeclaration->get_firstNondefiningDeclaration()));
                         ROSE_ASSERT(TransformationSupport::getSourceFile(copy_functionDeclaration->get_scope()) == TransformationSupport::getSourceFile(copy_functionDeclaration->get_firstNondefiningDeclaration()));
                       }
#if 0
                    printf ("DONE: Copy mechanism appied to SgFunctionDeclaration \n");
                    ROSE_ASSERT(false);
#endif
                  }
                 else
                  {
                    copy = n->copy(*this);
                  }
#endif

            // Also mark this as a transformation and to be output in unparsing (so it will be output by the code generator).
               Sg_File_Info* fileInfo = copy->get_file_info();
               if (fileInfo != NULL)
                  {
                 // Must make this for output (checked in unparser), marking as a transformation is not checked
                    fileInfo->setOutputInCodeGeneration();
                    fileInfo->setTransformation();
                  }

               return copy;
             }

   } collectDependentDeclarationsCopyType;


class CollectCppDirectivesTraversal : public SgSimpleProcessing
   {
  // This traversal collects the includes at the top of a file.

     public:
          vector<PreprocessingInfo*> cppDirectiveList;

          void visit(SgNode *astNode);
   };


void
CollectCppDirectivesTraversal::visit(SgNode *astNode)
   {
     SgLocatedNode* locatedNode = isSgLocatedNode(astNode);
     if (locatedNode != NULL)
        {
          AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();

          if (comments != NULL)
             {
#if 0
               printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
#endif
               AttachedPreprocessingInfoType::iterator i;
               for (i = comments->begin(); i != comments->end(); i++)
                  {
                    ROSE_ASSERT ( (*i) != NULL );
#if 0
                    printf ("          Attached Comment (relativePosition=%s): %s\n",
                         ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                         (*i)->getString().c_str());
                    printf ("Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
                    (*i)->get_file_info()->display("comment/directive location");
#endif

                 // We only save the #include directives, but likely we should be collecting ALL directives to
                 // avoid the collection of #include that is meant to be hidden in an #if 0 ... #endif pairing.
                 // if ((*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration)
                       {
                      // This is an include directive.
                         cppDirectiveList.push_back(*i);
#if 0
                         printf ("          Attached include directive (relativePosition=%s): %s\n",
                              ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                              (*i)->getString().c_str());
#endif
                       }
                  }
             }
            else
             {
#if 0
               printf ("No attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
#endif
             }
        }
   }


// This is part of a mechanism to collect directives from code that is to be outlined.
// However if we collect include directives we likely should really be collecting ALL
// directives (since the #include could be inside of an #if 0 ... #endif pairing.
// This level of detail will be addressed later (in an iterative approach).
vector<PreprocessingInfo*>
collectCppDirectives ( SgSourceFile* file )
   {
  // This function is used to collect include directives from the whole file.
     CollectCppDirectivesTraversal t;
     t.traverse(file,preorder);

     return t.cppDirectiveList;
   }


vector<PreprocessingInfo*>
collectCppDirectives ( SgLocatedNode* n )
   {
  // This function is used to collect include directives from specific dependent declarations.
     CollectCppDirectivesTraversal t;
     t.traverse(n,preorder);

     return t.cppDirectiveList;
   }

// Debugging support.
void
outputPreprocessingInfoList ( const vector<PreprocessingInfo*> & l )
   {
  // This function support debugging the generated directive lists.

     vector<PreprocessingInfo*>::const_iterator i = l.begin();
     while ( i != l.end() )
        {
          printf ("          Attached include directive (relativePosition=%s): %s\n",
             ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
             (*i)->getString().c_str());
          i++;
        }
   }



SgDeclarationStatement*
getAssociatedDeclaration( SgScopeStatement* scope )
   {
  //TODO This should become a member of SgScopeStatement

     SgDeclarationStatement* declaration = NULL;
     switch(scope->variantT())
        {
          case V_SgNamespaceDefinitionStatement:
             {
               SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(scope);
               declaration = namespaceDefinition->get_namespaceDeclaration();
               break;
             }

          case V_SgClassDefinition:
          case V_SgTemplateInstantiationDefn: // Liao, 5/12/2009
             {
               SgClassDefinition* classDefinition = isSgClassDefinition(scope);
               declaration = classDefinition->get_declaration();
               break;
             }

          default:
             {
               printf ("Error: default reached in getAssociatedDeclaration(): scope = %p = %s \n",scope,scope->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

  // There may be some scopes that don't have an associated declaration.
     ROSE_ASSERT(declaration != NULL);

     return declaration;
   }


class CollectDependentDeclarationsTraversal : public SgSimpleProcessing
   {
     public:
       // Accumulate a list of copies of associated declarations referenced in the AST subtree
       // (usually of the outlined functions) to insert in the separate file to support outlining.
          vector<SgDeclarationStatement*> declarationList;

       // Save the list of associated symbols of dependent declarations identified so that we can
       // support their replacement in the new AST.
          vector<SgSymbol*> symbolList;

       // Keep track of declarations already copied so that we don't copy them more than once and try to insert them.
          set<SgDeclarationStatement*> alreadySavedDeclarations;

       // Required visit function for the AST traversal
          void visit(SgNode *astNode);
     private:
       void addDeclaration(SgDeclarationStatement* decl);
   };

SgDeclarationStatement*
getGlobalScopeDeclaration( SgDeclarationStatement* inputDeclaration )
   {
  // DQ (2/16/2009): Basically if a class is used from a namespace (or any outer scope) and we
  // outline the reference to the class, we have to declare not the class but the outer scope
  // (which will have the class included).

     SgDeclarationStatement* returnDeclaration = inputDeclaration;

  // I think that we have to copy the outer scope if the declaration's scope is not SgGlobal.
     SgScopeStatement* scope = inputDeclaration->get_scope();
     ROSE_ASSERT(scope != NULL);

  // printf ("inputDeclaration->get_scope() = %p = %s \n",scope,scope->class_name().c_str());

  // If the input declaration is not in global scope then find the parent declaration that is in global scope!
     SgGlobal* globalScope = isSgGlobal(scope);
     if (globalScope == NULL)
        {
       // Traverse back to the global scope to include outer declarations which contain the "declaration"
       // printf ("Traverse back to the global scope to include outer declarations \n");

          SgScopeStatement* parentScope = scope;
          SgDeclarationStatement* associatedDeclaration = returnDeclaration;
          ROSE_ASSERT(parentScope != NULL);
          while (globalScope == NULL)
             {
               associatedDeclaration = getAssociatedDeclaration(parentScope);
               ROSE_ASSERT(associatedDeclaration != NULL);

               parentScope = parentScope->get_scope();
               globalScope = isSgGlobal(parentScope);
             }

          returnDeclaration = associatedDeclaration;
        }

     return returnDeclaration;
   }


// Debugging support.
void
outputDeclarationList ( const vector<SgDeclarationStatement*> & l )
   {
  // This function support debugging the generated declarations.

     int counter = 0;
     vector<SgDeclarationStatement*>::const_iterator i = l.begin();
     while ( i != l.end() )
        {
          printf ("In outputDeclarationList(): list[%d] = %p = %s = %s \n",counter++,*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());
          i++;
        }
   }

void CollectDependentDeclarationsTraversal::addDeclaration(SgDeclarationStatement* declaration)
{
  // If there was a declaration found then handle it.
     if (declaration != NULL)
        {
       // Reset the defining declaration in case there is an outer declaration that is more important
       // to consider the dependent declaration (e.g. a class in a namespace).  In general this will
       // find the associated outer declaration in the global scope.
          SgDeclarationStatement* dependentDeclaration = getGlobalScopeDeclaration(declaration);

       // This declaration is in global scope so we just copy the declaration
       // For namespace declarations: they may have the save name but they have to be saved separated.
          if (alreadySavedDeclarations.find(dependentDeclaration) == alreadySavedDeclarations.end())
             {
#if 0
               printf ("In CollectDependentDeclarationsTraversal::visit(): selected dependentDeclaration = %p = %s = %s \n",
                    dependentDeclaration,dependentDeclaration->class_name().c_str(),SageInterface::get_name(dependentDeclaration).c_str());
#endif
            // DQ (2/22/2009): Semantics change for this function, just save the original declaration, not a copy of it.
               declarationList.push_back(dependentDeclaration);

            // Record this as a copied declaration
               alreadySavedDeclarations.insert(dependentDeclaration);
#if 0
               printf ("In CollectDependentDeclarationsTraversal::visit(): astNode = %p = %s = %s \n",astNode,astNode->class_name().c_str(),SageInterface::get_name(astNode).c_str());
               printf ("############### ADDING dependentDeclaration = %p = %s to alreadySavedDeclarations set (size = %zu) \n",
                    dependentDeclaration,dependentDeclaration->class_name().c_str(),alreadySavedDeclarations.size());
#endif
            // DQ (2/21/2009): Added assertions (will be inforced in SageInterface::appendStatementWithDependentDeclaration()).
            // ROSE_ASSERT(copy_definingDeclaration->get_firstNondefiningDeclaration() != NULL);
             }
            else
             {
#if 0
               printf ("In CollectDependentDeclarationsTraversal::visit(): astNode = %p = %s = %s \n",astNode,astNode->class_name().c_str(),SageInterface::get_name(astNode).c_str());
               printf ("############### EXISTING dependentDeclaration = %p = %s found in alreadySavedDeclarations set (size = %zu) \n",
                    dependentDeclaration,dependentDeclaration->class_name().c_str(),alreadySavedDeclarations.size());
#endif
             }
        }
}

//! Collect all typedef declarations used by an input type and its base types
static std::vector<SgTypedefDeclaration*> collectTypedefDeclarations(SgType* type)
{
  ROSE_ASSERT(type != NULL);
  std::vector<SgTypedefDeclaration*> result;
  SgType* currentType = type;

  SgModifierType*  modType     = NULL;
  SgPointerType*   pointType   = NULL;
  SgReferenceType* refType     = NULL;
  SgArrayType*     arrayType   = NULL;
  SgTypedefType*   typedefType = NULL;

  while (true)
  {
    modType = isSgModifierType(currentType);
    if(modType)
    {
      currentType = modType->get_base_type();
    }
    else
    {
      refType = isSgReferenceType(currentType);
      if(refType)
      {
        currentType = refType->get_base_type();
      }
      else
      {
         pointType = isSgPointerType(currentType);
        if ( pointType)
        {
          currentType = pointType->get_base_type();
        }
        else
        {
          arrayType = isSgArrayType(currentType);
          if  (arrayType)
          {
            currentType = arrayType->get_base_type();
          }
          else
          {
            typedefType = isSgTypedefType(currentType);
            if (typedefType)
            {
              currentType = typedefType->get_base_type();
              SgTypedefDeclaration* tdecl = isSgTypedefDeclaration(typedefType->get_declaration());
              // have to try to get the defining declaration for a defining typedef declaration
              // otherwise AST traversal will not visit the non-defining one for a defining typedef declaration
              // sortSgNodeListBasedOnAppearanceOrderInSource() won't work properly
              SgTypedefDeclaration* decl = isSgTypedefDeclaration(tdecl->get_definingDeclaration());
              if (decl ==NULL)
                decl = tdecl;
              result.push_back(decl);
            }
            else
            {
              // Exit the while(true){} loop!
              break;
            }
          }
        }
      }
    }
  }
#if 0
  // debug here
  if (result.size()>0)
  {
    cout<<"------------Found a chain of typedef decls: count="<<result.size()<<endl;
    for (vector <SgTypedefDeclaration*>::const_iterator iter = result.begin();
          iter!=result.end(); iter ++)
      cout<<(*iter)->unparseToString()<<endl;
  }
#endif
  return result;
}

//! visitor function for each node to collect non-builtin types' declarations
void
CollectDependentDeclarationsTraversal::visit(SgNode *astNode)
   {
  // Statements that can cause us to have declaration dependences:
  //    1) variable declarations (through their types)
  //    2) function calls
  //    3) typedefs (through their base types)
  //    4) static member functions (through their class)
  //    5) static data members (through their class)
  //    6) namespaces
  //    7) #include<> CPP directives.

  // DQ (2/22/2009): Changing the semantics for this function,
  // just save the original declaration, not a copy of it.

#if 0
  // Debugging support.
     Sg_File_Info* fileInfo = astNode->get_file_info();
     if (fileInfo != NULL && fileInfo->isFrontendSpecific() == false && (isSgStatement(astNode) != NULL) )
        {
          printf ("\n\nIn CollectDependentDeclarationsTraversal::visit(): astNode = %p = %s = %s \n",astNode,astNode->class_name().c_str(),SageInterface::get_name(astNode).c_str());
       // fileInfo->display("In CollectDependentDeclarationsTraversal::visit()");
          int counter = 0;
          printf ("alreadySavedDeclarations.size() = %zu \n",alreadySavedDeclarations.size());
          for (set<SgDeclarationStatement*>::iterator i = alreadySavedDeclarations.begin(); i != alreadySavedDeclarations.end(); i++)
             {
               printf ("alreadySavedDeclarations %d: %p = %s \n",counter++,*i,(*i)->class_name().c_str());
             }
        }
#endif

  // The following conditionals set this variable
     SgDeclarationStatement* declaration = NULL;

  // 1) ------------------------------------------------------------------
  // Collect the declarations associated with referenced types in variable declarations (or any types associated with SgInitializedName IR nodes)
     SgInitializedName* initializedname = isSgInitializedName(astNode);
     if (initializedname != NULL)
     {
       SgType* type = initializedname->get_type();

       // handle all dependent typedef declarations, if any
       std::vector <SgTypedefDeclaration*> typedefVec = collectTypedefDeclarations(type);
        for (std::vector <SgTypedefDeclaration*>::const_iterator iter =typedefVec.begin();
                iter != typedefVec.end(); iter++)
       {
         SgTypedefDeclaration* typedef_decl = *iter;
         addDeclaration(typedef_decl);
         symbolList.push_back(typedef_decl->get_symbol_from_symbol_table());
       }

      // handle base type:
       SgType* strippedType = type;
       // We now can to strip typedefs since they are already handled by collectTypedefDeclarations()
       // this also reach to the defining body of a defining typedef declaration
       // and treat it as an independent declarations,
       // the assumption here is that a defining typedef declaration will only has its
       // nondefining declaration copied to avoid redefining of the struct.
       // This is also a workaround for an AST copy bug: defining body gets lost after copying
       // a defining typedef declaration.
       // Liao, 5/8/2009
       //
       // e.g. typedef struct hypre_BoxArray_struct
       // {
       //          int alloc_size;
       // } hypre_BoxArray;
       //
       // struct hypre_BoxArray_struct will be treated as a strippedType and its declaration
       // will be inserted.
       //
       strippedType = type->stripType();
       SgNamedType* namedType = isSgNamedType(strippedType);
       if (namedType != NULL)
       {
         // Note that since this was obtained via the types and types are shared, this is the non-defining
         // declaration in original program (not the separate file is this is to support outlining into a
         // separate file.
         SgDeclarationStatement* named_decl = namedType->get_declaration();
         // the case of class declaration, including struct, union
         SgClassDeclaration* classDeclaration = isSgClassDeclaration(named_decl);
         if (classDeclaration != NULL)
         {
           // printf ("Found class declaration: classDeclaration = %p \n",classDeclaration);
           declaration = classDeclaration->get_definingDeclaration();
           ROSE_ASSERT(declaration != NULL);
           addDeclaration(declaration);

           // Note that since types are shared in the AST, the declaration for a named type may be (is)
           // associated with the class declaration in the original file. However, we want to associated
           // class declaration in the current file, but since the AST copy mechanism work top-down, this
           // mapping form the declaration in the original file to the new declaration in the copied AST
           // is available in the SgCopyHelp map of copied IR nodes.
           // DQ (3/3/2009): Added support for symbol references to be saved (symbols in the original file).
           // these symbols will be mapped to their new symbols.
           ROSE_ASSERT(classDeclaration->hasAssociatedSymbol() == true);
           SgSymbol* classSymbol = classDeclaration->get_symbol_from_symbol_table();
           ROSE_ASSERT(classSymbol != NULL);

           // printf ("Saving classSymbol = %p \n",classSymbol);
           symbolList.push_back(classSymbol);
         }

         // handle Enum types
         SgEnumDeclaration* enum_decl = isSgEnumDeclaration(named_decl);
         if (enum_decl != NULL)
         {
           declaration = enum_decl->get_definingDeclaration();
           ROSE_ASSERT(declaration != NULL);
           addDeclaration(declaration);
           ROSE_ASSERT(enum_decl->hasAssociatedSymbol() == true);
           // Liao 12/14/2012. use search_for_symbol_from_symbol_table() instead to avoid the defining/nondefing decl issue
           //SgSymbol* esymbol = enum_decl->get_symbol_from_symbol_table();
           SgSymbol* esymbol = enum_decl->search_for_symbol_from_symbol_table();
           ROSE_ASSERT(esymbol!= NULL);
           symbolList.push_back(esymbol);
         }
       } // end if namedType
#if 0
       printf ("Found reference to type = %p = %s strippedType = %p = %s \n",type,type->class_name().c_str(),strippedType,strippedType->class_name().c_str());
#endif
     }// end if (initializedname)

  // 2) ------------------------------------------------------------------
  // Collect declarations associated with function calls.
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
     if (functionCallExp != NULL)
        {
          declaration = functionCallExp->getAssociatedFunctionDeclaration();
          //ROSE_ASSERT(declaration != NULL);
          // We allow a function pointer to have no specific declaration associated.
          if (declaration != NULL)
            addDeclaration(declaration);

       // DQ (3/2/2009): Added support for symbol references to be saved (this can be a SgFunctionSymbol or a SgMemberFunctionSymbol).
          SgSymbol* functionSymbol = functionCallExp->getAssociatedFunctionSymbol();
          //ROSE_ASSERT(functionSymbol != NULL);

       // printf ("Saving functionSymbol = %p \n",functionSymbol);
          if (functionSymbol)
            symbolList.push_back(functionSymbol);
        }

    // 3) ------------------------------------------------------------------
    // Collect enumerate declarations associated with SgEnumVal
    SgEnumVal * eval = isSgEnumVal(astNode);
    if (eval != NULL)
    {
      declaration = eval->get_declaration();
      ROSE_ASSERT(declaration != NULL);
      addDeclaration(declaration);
      SgSymbol* symbol = declaration->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table();
      ROSE_ASSERT(symbol != NULL);
      symbolList.push_back(symbol);
    }
//       addDeclaration(declaration); // do it in different cases individually
   }

static std::map<const SgStatement*, bool> visitedDeclMap; // avoid infinite recursion


//! Collect dependent type declarations and corresponding symbols used by a declaration statement with defining body.
// Used to separate a function to a new source file and add necessary type declarations into the new file.
// NOTICE: each call to this function has to have call visitedDeclMap.clear() first!!
static void
getDependentDeclarations (SgStatement* stmt, vector<SgDeclarationStatement*> & declarationList, vector<SgSymbol*> & symbolList )
{
  // This function returns a list of the dependent declaration for any input statement.
  // Dependent declaration are functions called, types referenced in variable declarations, etc.
#if 0
  printf ("\n\n********************************************************** \n");
  printf (" Inside of getDependentDeclarations(stmt = %p = %s) \n",stmt,stmt->class_name().c_str());
  printf ("********************************************************** \n");
#endif
 visitedDeclMap[stmt]= true;
  CollectDependentDeclarationsTraversal t;
  t.traverse(stmt,preorder);
#if 0
     declarationList = t.declarationList;
     symbolList      = t.symbolList;
#else
  // Merge to the parent level list
  copy(t.declarationList.begin(),t.declarationList.end(), back_inserter(declarationList));
  copy(t.symbolList.begin(),t.symbolList.end(), back_inserter(symbolList));
    // make their elements unique
  sort (declarationList.begin(), declarationList.end());
  vector<SgDeclarationStatement*>::iterator new_end = unique(declarationList.begin(), declarationList.end());
  declarationList.erase(new_end, declarationList.end());

  sort (symbolList.begin(), symbolList.end());
  vector<SgSymbol*>::iterator end2 = unique(symbolList.begin(), symbolList.end());
  symbolList.erase(end2, symbolList.end());


  // Liao, 5/7/2009 recursively call itself to get dependent declarations' dependent declarations
  for (vector<SgDeclarationStatement*>::const_iterator iter = t.declarationList.begin();
      iter !=t.declarationList.end(); iter++)
  {
    SgDeclarationStatement* decl = *iter;
    SgType* base_type = NULL;
    SgStatement* body_stmt= NULL;

    // grab base type for a declaration
    // For class declaration: grab their
    if (isSgClassDeclaration(decl))
    {
      base_type =  isSgClassDeclaration(decl)->get_type();
    } else
    if (isSgTypedefDeclaration(decl))
    {

      // we don't want to strip of nested typedef declarations
      base_type = isSgTypedefDeclaration(decl)->get_base_type()->stripType(SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_MODIFIER_TYPE);
    }

    //TODO variable declaration, function declaration: parameter list types,
    // multiple base_type then

    // is the base type associated with a defining body?
    // TODO enum type
      if (isSgClassType(base_type))
      {
        SgClassDeclaration* class_decl = isSgClassDeclaration(isSgClassType(base_type)->get_declaration()->get_definingDeclaration());
        if (class_decl!=NULL)
        {
          body_stmt = class_decl->get_definition();
        }
      }
    // recursively collect dependent declarations for the body stmt
    if ((body_stmt!=NULL) &&(!visitedDeclMap[body_stmt]))
      { // avoid infinite recursion
        getDependentDeclarations(body_stmt, declarationList, symbolList);
      }
    }
#endif
} // end void getDependentDeclarations()


// Reorder a list of declaration statements based on their appearance order in source files
// This is essential to insert their copies into a new file in a right order
// Liao, 5/7/2009
vector<SgDeclarationStatement*>
SageInterface::sortSgNodeListBasedOnAppearanceOrderInSource(const vector<SgDeclarationStatement*>& nodevec)
{
  vector<SgDeclarationStatement*> sortedNode;

  if (nodevec.size()==0 )
    return sortedNode;
  // no need to sort if there is only 1 element
  if (nodevec.size() ==1)
    return nodevec;

  SgProject* project = SageInterface::getProject();
  Rose_STL_Container<SgNode*> queryResult = NodeQuery::querySubTree(project,V_SgDeclarationStatement);
  for (Rose_STL_Container<SgNode*>::const_iterator iter = queryResult.begin();
      iter!= queryResult.end(); iter++)
  {
    //    cerr<<"Trying to match:"<<(*iter)<<" "<<(*iter)->class_name() <<" "<<(*iter)->unparseToString()<<endl;
    SgNode* cur_node = *iter;
    SgDeclarationStatement* cur_stmt =  isSgDeclarationStatement(cur_node);
    ROSE_ASSERT(cur_stmt!=NULL);
    // Liao 12/14/2012. It is possible nodevec contains a first non-defining function declaration since the function is called in the outlined function
    // This is true even if the AST only has a defining function declaration.
    //
    // But that first non-defining function declaration is not traversable in AST due to it is hidden.
    // The solution here is to for each defining function decl traversed, convert it to the first nondefining one to do the match.
    SgFunctionDeclaration * func_decl = isSgFunctionDeclaration (cur_stmt);
    if (func_decl)
    {
      if (func_decl->get_definingDeclaration() == func_decl )
      {
        cur_stmt = func_decl->get_firstNondefiningDeclaration();
        ROSE_ASSERT (cur_stmt != func_decl);
      }
    }
    vector<SgDeclarationStatement*>::const_iterator i = find (nodevec.begin(), nodevec.end(), cur_stmt);
    if (i!=nodevec.end())
    {
      // It is possible we already visited a real prototype func decl before, now we see a prototype converted from a defining declaration. 
      // We have to make sure only one copy is inserted.
      vector<SgDeclarationStatement*>::const_iterator j = find (sortedNode.begin(), sortedNode.end(), *i); 
      if (j == sortedNode.end())
        sortedNode.push_back(*i);
    }
  }

  if (nodevec.size() != sortedNode.size())
  {
    cerr<<"Fatal error in sortSgNodeListBasedOnAppearanceOrderInSource(): nodevec.size() != sortedNode.size()"<<endl;
    cerr<<"nodevec() have "<< nodevec.size()<<" elements. They are:"<<endl;
    for (vector<SgDeclarationStatement*>::const_iterator iter = nodevec.begin(); iter != nodevec.end(); iter++)
    {
      cerr<<(*iter)<<" "<<(*iter)->class_name() <<" "<<(*iter)->unparseToString()<<endl;
    }
    cerr<<"sortedNode() have " << sortedNode.size() <<" elements. They are:"<<endl;
    for (vector<SgDeclarationStatement*>::const_iterator iter = sortedNode.begin(); iter != sortedNode.end(); iter++)
    {
      cerr<<(*iter)<<" "<<(*iter)->class_name() <<" "<<(*iter)->unparseToString()<<endl;
    }

    ROSE_ASSERT(nodevec.size() == sortedNode.size());
  }
  return sortedNode;
}

//! Please call this instead of calling getDependentDeclarations ( SgStatement* stmt, vector<SgDeclarationStatement*> & declarationList, vector<SgSymbol*> & symbolList )
// This function clears a history map transparently and return a sorted list of dependent declarations
std::vector<SgDeclarationStatement*>
SageInterface::getDependentDeclarations ( SgStatement* stmt )
   {
  // This function returns a list of the dependent declaration for any input statement.
  // Dependent declaration are functions called, types referenced in variable declarations, etc.
#if 0
     printf ("\n\n********************************************************** \n");
     printf (" Inside of getDependentDeclarations(stmt = %p = %s) \n",stmt,stmt->class_name().c_str());
     printf ("********************************************************** \n");

     CollectDependentDeclarationsTraversal t;
     t.traverse(stmt,preorder);

     return t.declarationList;
#else
     // share a single implementation for recursive lookup for dependent declaration
   visitedDeclMap.clear();
   vector<SgDeclarationStatement*> declarationList;
   vector<SgSymbol*> symbolList;
   getDependentDeclarations(stmt, declarationList, symbolList);
   declarationList = sortSgNodeListBasedOnAppearanceOrderInSource(declarationList);
#if 0
     printf ("\n\n ********************************************************** \n");
     cout<<"Found dependent decl: count="<<declarationList.size()<<endl;
     for ( vector<SgDeclarationStatement*>::const_iterator iter = declarationList.begin();
          iter != declarationList.end(); iter++)
     {
       cout<<"\t"<<(*iter)->class_name()<<" at line "<<(*iter)->get_file_info()->get_line()<<endl;
       if ((*iter)->variantT()== V_SgFunctionDeclaration)
         cout<<"func name is:"<<isSgFunctionDeclaration(*iter)->get_name().getString()<<endl;
       //<<(*iter)->unparseToString()<<endl;  // unparseToString() won't work on outlined function
     }
     printf ("\n ********************************************************** \n");
#endif

   return declarationList;

#endif
   }


bool
SageInterface::isPrefixOperatorName( const SgName & functionName )
   {
     bool returnValue = false;

#if 0
     printf ("In SageInterface::isPrefixOperatorName(): functionName = %s (might have to check the return type to distinguish the deref operator from the multiply operator) \n",functionName.str());
#endif

     if (functionName.is_null() == false)
        {
          if ( functionName == "operator++" || functionName == "operator--" || functionName == "operator&" || 
               functionName == "operator!"  || functionName == "operator*"  || functionName == "operator+" ||
               functionName == "operator-"  || functionName == "operator+" )
             {
               returnValue = true;
             }
        }

     return returnValue;
   }


// DQ (4/13/2013): We need these to support the unparing of operators defined by operator syntax or member function names.
//! Is an overloaded operator a prefix operator (e.g. address operator X * operator&(), dereference operator X & operator*(), unary plus operator X & operator+(), etc.
bool
SageInterface::isPrefixOperator( SgExpression* exp )
   {
  // DQ (4/21/2013): Reimplemented this function to support more of the prefix operators.
  // Also we now support when they are defined as member functions and non-member functions.

     bool returnValue = false;

     SgFunctionRefExp* functionRefExp             = isSgFunctionRefExp(exp);
     SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(exp);
  // ROSE_ASSERT(memberFunctionRefExp != NULL);

     SgName functionName;
     size_t numberOfOperands = 0;

     if (memberFunctionRefExp != NULL)
        {
          ROSE_ASSERT(functionRefExp == NULL);
          SgMemberFunctionDeclaration* memberFunctionDeclaration = memberFunctionRefExp->getAssociatedMemberFunctionDeclaration();
          if (memberFunctionDeclaration != NULL)
             {
               functionName = memberFunctionDeclaration->get_name();
               numberOfOperands = memberFunctionDeclaration->get_args().size();
             }
        }
       else
        {
       // This could be "friend bool operator!(const X & x);"
          if (functionRefExp != NULL)
             {
               SgFunctionDeclaration* functionDeclaration = functionRefExp->getAssociatedFunctionDeclaration();
               if (functionDeclaration != NULL)
                  {
                    functionName     = functionDeclaration->get_name();
                    numberOfOperands = functionDeclaration->get_args().size();
                  }
             }
            else
             {
            // Note clear if this should be an error.
               printf ("In SageInterface::isPrefixOperator(): unknown case of exp = %p = %s \n",exp,exp->class_name().c_str());
             }
        }

#if 0
     printf ("In SageInterface::isPrefixOperator(): functionName = %s numberOfOperands = %zu (might have to check the return type to distinguish the deref operator from the multiply operator) \n",functionName.str(),numberOfOperands);
#endif

     if (isPrefixOperatorName(functionName) == true)
        {
          if (memberFunctionRefExp != NULL)
             {
            // This case is for member functions.
               ROSE_ASSERT(functionRefExp == NULL);
               if (numberOfOperands == 0)
                  {
                 // This is the C++ signature for the operator++() prefix operator.
                    returnValue = true;
                  }
                 else
                  {
                 // This is the C++ signature for the operator++() postfix operator.
                    returnValue = false;
                  }
             }
            else
             {
            // This case is for non-member functions.
               ROSE_ASSERT(functionRefExp != NULL);
               ROSE_ASSERT(memberFunctionRefExp == NULL);
               if (numberOfOperands == 1)
                  {
                 // This is the C++ signature for the operator++() prefix operator.
                    returnValue = true;
                  }
                 else
                  {
                 // This is the C++ signature for the operator++() postfix operator.
                    ROSE_ASSERT(numberOfOperands == 2);
                    returnValue = false;
                  }
             }
        }

#if 0
     printf ("Leaving SageInterface::isPrefixOperator(): returnValue = %s \n",returnValue ? "true" : "false");
#endif

     return returnValue;
   }


//! Is an overloaded operator a postfix operator. (e.g. ).
bool
SageInterface::isPostfixOperator( SgExpression* exp )
   {
     return ( (isPrefixOperator(exp) == false) && (isIndexOperator(exp) == false) );
   }


//! Is an overloaded operator an index operator (also refereded to as call or subscript operators). (e.g. X & operator()() or X & operator[]()).
bool
SageInterface::isIndexOperator( SgExpression* exp )
   {
     bool returnValue = false;
     SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(exp);
     ROSE_ASSERT(memberFunctionRefExp != NULL);

     SgMemberFunctionDeclaration* memberFunctionDeclaration = memberFunctionRefExp->getAssociatedMemberFunctionDeclaration();
     if (memberFunctionDeclaration != NULL)
        {
          SgName functionName = memberFunctionDeclaration->get_name();
          if ( (functionName == "operator[]") && (isSgType(memberFunctionDeclaration->get_type()) != NULL) )
             {
               returnValue = true;
             }
            else
             {
               if ( (functionName == "operator()") && (isSgType(memberFunctionDeclaration->get_type()) != NULL) )
                  {
                    returnValue = true;
                  }
                 else
                  {
                    returnValue = false;
                  }
             }
        }
      
     return returnValue;
   }




//! Generate copies for a list of declarations and insert them into a different targetScope.
vector<SgDeclarationStatement*>
generateCopiesOfDependentDeclarations (const  vector<SgDeclarationStatement*>& dependentDeclarations, SgScopeStatement* targetScope)
//generateCopiesOfDependentDeclarations ( SgStatement* stmt, SgScopeStatement* targetScope )
   {
  // Liao suggests adding the target scope to the parameter list so that the constructed function
  // built using SageBuilder::buildNondefiningFunctionDeclaration() can be built to be in the
  // correct scope as soon as possible.

  // This function uses the getDependentDeclarations() function to get the dependent declarations
  // and then returns a list of copies of each of them.

#if 0
     printf ("\n\n********************************************************** \n");
     printf (" Inside of generateCopiesOfDependentDeclarations(stmt = %p = %s) \n",stmt,stmt->class_name().c_str());
     printf ("********************************************************** \n");
#endif

     vector<SgDeclarationStatement*> copiesOfDependentDeclarations;
     // avoiding call getDependentDeclarations() twice
//     vector<SgDeclarationStatement*> dependentDeclarations = getDependentDeclarations(stmt);

#if 0
     printf ("Output the dependentDeclarations: dependentDeclarations.size() = %zu \n",dependentDeclarations.size());
     outputDeclarationList(dependentDeclarations);
#endif
#if 0
     printf ("************************************************ \n");
     printf ("*** Make all copies of dependentDeclarations *** \n");
     printf ("************************************************ \n");
#endif

  // Should have a valid scope, so that symbol tables can be expected to be setup.
     ROSE_ASSERT(targetScope != NULL);

  // DQ (2/25/2009): Initially this is always global scope so test this for now, at least.
     ROSE_ASSERT(isSgGlobal(targetScope) != NULL);

     for (vector<SgDeclarationStatement*>::const_iterator i = dependentDeclarations.begin(); i != dependentDeclarations.end(); i++)
        {
#if 0
          printf ("Copying declaration = %p = %s = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());
#endif
       // SgNode* copy_node = (*i)->copy(collectDependentDeclarationsCopyType);

          SgNode* copy_node = NULL;
          const SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);

       // For function declarations we don't want to do deep copies on defining declarations
       // since that would violate the One-time Definition Rule (ODR).
          if (functionDeclaration != NULL)
             {
#if 1
               // the target scope may already have a declaration for this function.
               // This happens since SageInterface::appendStatementWithDependentDeclaration() is called in the end of outlining
               // and the original enclosing class of the outlined target has been changed already (replaced target with a call to OUT_xxx())
               // Also, getDependentDeclarations() recursively searches for declarations within the dependent class and hits OUT_xxx()
               // Liao, 5/8/2009

               printf ("WARNING: In SageInterface -- generateCopiesOfDependentDeclarations(): I think this is the wrong lookup symbol function that is being used here! \n");

            // DQ (8/16/2013): I think this is the wrong symbol lookup function to be using here, but the API is fixed.
            // if (targetScope->lookup_symbol(functionDeclaration->get_name()) !=NULL)
               if (targetScope->lookup_symbol(functionDeclaration->get_name(),NULL,NULL) !=NULL)
                    continue;
#endif
#if 0
               printf ("In generateCopiesOfDependentDeclarations(): Copy mechanism appied to SgFunctionDeclaration functionDeclaration->get_firstNondefiningDeclaration() = %p \n",functionDeclaration->get_firstNondefiningDeclaration());

               printf ("functionDeclaration                                    = %p \n",functionDeclaration);
               printf ("functionDeclaration->get_definingDeclaration()         = %p \n",functionDeclaration->get_definingDeclaration());
               printf ("functionDeclaration->get_firstNondefiningDeclaration() = %p \n",functionDeclaration->get_firstNondefiningDeclaration());
               printf ("functionDeclaration->get_scope()                       = %p \n",functionDeclaration->get_scope());
               printf ("targetScope                                            = %p \n",targetScope);
#endif
            // FIXME: This conditional could be replace by a single case using the SageBuilder::buildNondefiningFunctionDeclaration()
            // But the idea started that only defining function declarations would be a special case to all declarations being copied.

            // Make sure this is not a defining declaration
            // if (functionDeclaration->get_firstNondefiningDeclaration() != NULL)
            // if (functionDeclaration->get_firstNondefiningDeclaration() != NULL && functionDeclaration->get_definingDeclaration() != functionDeclaration)
               if (functionDeclaration->get_definingDeclaration() != functionDeclaration)
                  { //functionDeclaration is a non-defining declaration
                    ROSE_ASSERT(functionDeclaration->get_firstNondefiningDeclaration() != NULL);

                 // Make a copy of the non-defining declaration
                    copy_node = functionDeclaration->get_firstNondefiningDeclaration()->copy(collectDependentDeclarationsCopyType);

                    SgFunctionDeclaration* copy_nondefiningDeclaration = isSgFunctionDeclaration(copy_node);
                    copy_nondefiningDeclaration->set_firstNondefiningDeclaration(copy_nondefiningDeclaration);
                    //ROSE_ASSERT(copy_nondefiningDeclaration->get_definingDeclaration() == NULL); // TODO: Liao 12/14/2012. this needs more investigation. It fails on tests/nonsmoke/functional/roseTests/astOutliningTests/moreTest2.cpp. We need a special version of AST copy (collectDependentDeclarationsCopyType) to do just shallow copy of function prototypes

                 // DQ (2/25/2009): Added assertion.
                    ROSE_ASSERT(copy_nondefiningDeclaration->get_scope() == functionDeclaration->get_scope());
                    ROSE_ASSERT(copy_nondefiningDeclaration->get_firstNondefiningDeclaration() != NULL);

                 // Set the scope now that we know it (might be the same as the parent which will be set when the copy is inserted into the AST).
                    copy_nondefiningDeclaration->set_scope(targetScope);

                 // Need to fixup the symbol table to have a symbol for the copied function (this is fixed up later).
                 // ROSE_ASSERT(copy_nondefiningDeclaration->get_symbol_from_symbol_table() != NULL);
                  }
                 else  // functionDeclaration is a defining declaration
                  {
                 // Build a function prototype, but what scope should be used?
                    ROSE_ASSERT(functionDeclaration->get_scope() != NULL);

                 // FIXME: The scope passed to SageBuilder::buildNondefiningFunctionDeclaration() should be NULL
                 // copy_node = SageBuilder::buildNondefiningFunctionDeclaration(functionDeclaration,functionDeclaration->get_scope());
                 // copy_node = SageBuilder::buildNondefiningFunctionDeclaration(functionDeclaration,NULL);
                    copy_node = SageBuilder::buildNondefiningFunctionDeclaration(functionDeclaration,targetScope);

                    SgDeclarationStatement* copy_definingDeclaration = isSgDeclarationStatement(copy_node);

                 // Since copy_definingDeclaration is build as a non-defining declaration we can set it to be the firstNondefiningDeclaration (for the separate file).
                    copy_definingDeclaration->set_firstNondefiningDeclaration(copy_definingDeclaration);

                 // This causes a cross file reference which we might want to disallow in the future.
                 // I think that this might cause the AST post-processing to reset some fields (e.g. scope to be incorect as well).
                 // copy_definingDeclaration->set_definingDeclaration(functionDeclaration->get_definingDeclaration());

                 // DQ (2/25/2009): Added assertion.
                 // ROSE_ASSERT(copy_definingDeclaration->get_scope() == functionDeclaration->get_scope());
                 // ROSE_ASSERT(copy_definingDeclaration->get_scope() == NULL);
                    ROSE_ASSERT(copy_definingDeclaration->get_scope() == targetScope);

                 // If this is for a separate file then the scopes should not match.
                    ROSE_ASSERT(functionDeclaration->get_scope() != targetScope);

                    ROSE_ASSERT(copy_definingDeclaration->get_firstNondefiningDeclaration() != NULL);

                 // This is setup in the SageBuilder::buildNondefiningFunctionDeclaration() function,
                 // so resetting up the symbol table will be skipped later.
                    ROSE_ASSERT(copy_definingDeclaration->get_symbol_from_symbol_table() != NULL);
                  }

               SgFunctionDeclaration* copy_functionDeclaration = isSgFunctionDeclaration(copy_node);
               ROSE_ASSERT(copy_functionDeclaration != NULL);
               //Liao, 5/19/2009
               //FixupTemplateDeclarations::visit() has this assertion
               //patch up endOfConstruct: TODO should do this in copy()
               SgFunctionParameterList * functionParameterList = copy_functionDeclaration->get_parameterList();
               functionParameterList->set_endOfConstruct(functionParameterList->get_startOfConstruct());
               ROSE_ASSERT(functionParameterList->get_startOfConstruct()->isSameFile(functionParameterList->get_endOfConstruct()) == true);

            // Set the scope to NULL, since it AST copy just preserves the scope to be that of functionDeclaration->get_scope()
            // copy_functionDeclaration->set_scope(NULL);

            // ROSE_ASSERT(isSgGlobal(copy_functionDeclaration->get_scope()) != NULL);
            // ROSE_ASSERT(TransformationSupport::getSourceFile(copy_functionDeclaration) == NULL);

            // Parents are not set and the test of SgSourceFile is structural (based on parent pointers).
            // ROSE_ASSERT(TransformationSupport::getSourceFile(copy_functionDeclaration) != NULL);

            // printf ("copy_functionDeclaration reported to be in file = %s \n",TransformationSupport::getSourceFile(copy_functionDeclaration)->getFileName().c_str());
            // ROSE_ASSERT(copy_functionDeclaration->get_scope() == NULL);
               ROSE_ASSERT(copy_functionDeclaration->get_scope() == targetScope);

            // The original function declaration in the original source file could have not had a function
            // prototype, and so it is allowable to have firstNondefiningDeclaration() == NULL.
            // ROSE_ASSERT(functionDeclaration->get_firstNondefiningDeclaration() != NULL);
               if (functionDeclaration->get_firstNondefiningDeclaration() != NULL)
                  {
                    ROSE_ASSERT(TransformationSupport::getSourceFile(functionDeclaration) == TransformationSupport::getSourceFile(functionDeclaration->get_firstNondefiningDeclaration()));
                    ROSE_ASSERT(TransformationSupport::getSourceFile(functionDeclaration->get_scope()) == TransformationSupport::getSourceFile(functionDeclaration->get_firstNondefiningDeclaration()));
                  }

               ROSE_ASSERT(copy_functionDeclaration != NULL);
               ROSE_ASSERT(copy_functionDeclaration->get_firstNondefiningDeclaration() != NULL);

            // Since the scopes are not properly set (functions have not been inserted) TransformationSupport::getSourceFile() will just be NULL.
            // ROSE_ASSERT(TransformationSupport::getSourceFile(copy_functionDeclaration) == TransformationSupport::getSourceFile(copy_functionDeclaration->get_firstNondefiningDeclaration()));

            // The scope has not been set yet, so we can't test that the files are the same.
            // ROSE_ASSERT(TransformationSupport::getSourceFile(copy_functionDeclaration->get_scope()) == TransformationSupport::getSourceFile(copy_functionDeclaration->get_firstNondefiningDeclaration()));
#if 0
               printf ("In generateCopiesOfDependentDeclarations(): DONE: Copy mechanism appied to SgFunctionDeclaration \n");
               ROSE_ASSERT(false);
#endif
             }
            else
             {
#if 1     // We only copy the non-defining declaration of a defining typedef declaration
          // since its defining body will be treated as a separate declaration and inserted to the new file.
          // This is also a workaround for an AST copy bug: losing defining body of a defining typedef declaration after copying.
                 SgTypedefDeclaration* tdecl = isSgTypedefDeclaration(*i);
                 if (tdecl)
                 {
#if 0
                   if (tdecl->get_definingDeclaration() == tdecl)
                     cout<<"Copying a defining typedef declaration:"<<tdecl->unparseToString()<<endl;
                   else
                     cout<<"Copying a non-defining typedef declaration:"<<tdecl->unparseToString()<<endl;
#endif
                   copy_node = SageInterface::deepCopy(tdecl->get_firstNondefiningDeclaration());
                   SgTypedefDeclaration* tdecl_copy = isSgTypedefDeclaration(copy_node);
                   tdecl_copy->set_typedefBaseTypeContainsDefiningDeclaration (false); // explicit indicate this does not contain defining base type, Liao 12/14/2012
                 }
                  else
#endif
                   copy_node = (*i)->copy(collectDependentDeclarationsCopyType);

            // Set the scope now that we know it (might be the same as the parent which will be set when the copy is inserted into the AST).
               SgDeclarationStatement* copy_declaration = isSgDeclarationStatement(copy_node);
               ROSE_ASSERT(copy_declaration != NULL);

            // DQ (3/2/2009): This will be done later, but it is set above, so set it in this branch to be consistant.
               if (copy_declaration->hasExplicitScope() == true)
                    copy_declaration->set_scope(targetScope);
             }

       // SgNode* copy_node = (*i)->copy(collectDependentDeclarationsCopyType);
       // Build a function prototype, but what scope should be used?
       // ROSE_ASSERT(functionDeclaration->get_scope() != NULL);
       // copy = SageBuilder::buildNondefiningFunctionDeclaration(functionDeclaration,functionDeclaration->get_scope());

#if 0
          printf ("DONE: Copying declaration: original = %p = %s = %s to copy = %p = %s = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str(),copy_node,copy_node->class_name().c_str(),SageInterface::get_name(copy_node).c_str());
#endif

          ROSE_ASSERT(copy_node != NULL);
          ROSE_ASSERT(copy_node->get_file_info() != NULL);

       // Note that the root of the does not have its file info set like its children.
          copy_node->get_file_info()->setTransformation();
          copy_node->get_file_info()->setOutputInCodeGeneration();
       // copy_node->get_file_info()->display("CollectDependentDeclarationsTraversal::visit()");

          SgDeclarationStatement* copy_definingDeclaration = isSgDeclarationStatement(copy_node);
          ROSE_ASSERT(copy_definingDeclaration != NULL);

          ROSE_ASSERT( *i != NULL );
          ROSE_ASSERT(copy_definingDeclaration != NULL);

       // DQ (2/26/2009): get_scope() will require a valid parent for some cases (see moreTest4.cpp).
          if (copy_definingDeclaration->get_parent() == NULL)
             {
               copy_definingDeclaration->set_parent(targetScope);
             }
          ROSE_ASSERT(copy_definingDeclaration->get_parent() != NULL);

#if 0
          printf ("DONE: Copying declaration: original scope = %p copied to scope = %p \n",(*i)->get_scope(),copy_definingDeclaration->get_scope());
#endif

       // DQ (2/22/2009): I think we need to set these explicitly (it will be reset later).
          copy_definingDeclaration->set_parent(NULL);

       // Now that we pass in the scope explicitly we can set the scope (above), so don't reset to NULL!
       // copy_definingDeclaration->set_scope(NULL);

       // DQ (2/20/2009): Added assertion.
          ROSE_ASSERT(copy_definingDeclaration->get_parent() == NULL);

#if 0
       // DQ (2/20/2009): Added assertion.
       // ROSE_ASSERT(copy_definingDeclaration->get_definingDeclaration() != NULL);
          if (copy_definingDeclaration->get_firstNondefiningDeclaration() == NULL)
             {
               printf ("copy_definingDeclaration = %p \n",copy_definingDeclaration);
               printf ("copy_definingDeclaration->get_firstNondefiningDeclaration() == NULL \n");
               printf ("copy_definingDeclaration->get_definingDeclaration() = %p \n",copy_definingDeclaration->get_definingDeclaration());
             }
#endif

       // DQ (2/21/2009): Commented out as a test.
          if ((*i)->get_firstNondefiningDeclaration() != NULL)
             {
               ROSE_ASSERT(copy_definingDeclaration->get_firstNondefiningDeclaration() != NULL);
             }

       // DQ (2/20/2009): Added asertion.
       // ROSE_ASSERT(copy_definingDeclaration->get_parent() != NULL);

          copiesOfDependentDeclarations.push_back(copy_definingDeclaration);
        }

#if 0
     printf ("****************************************************** \n");
     printf ("*** DONE: Make all copies of dependentDeclarations *** \n");
     printf ("****************************************************** \n");
     printf ("copiesOfDependentDeclarations.size() = %zu \n",copiesOfDependentDeclarations.size());
#endif

  // The mapping of copies to original declarations should be 1-to-1.
  // Liao, not true anymore for getDependentDeclarations() using recursion: a depending class's body is searched for dependents also.
  // The class body  might have a call to an outlined function, which already has a prototype in the target scope and needs no redundant copy
     ROSE_ASSERT(copiesOfDependentDeclarations.size() <= dependentDeclarations.size());

     return copiesOfDependentDeclarations;
   }


bool
declarationContainsDependentDeclarations( SgDeclarationStatement* decl, vector<SgDeclarationStatement*> & dependentDeclarationList )
   {
  // DQ (2/16/2009): This function finds the declarations in "decl" and checks if they match against those in dependentDeclarationList
     bool returnValue = false;

#if 0
     printf ("\n\n********************************************************** \n");
     printf (" Inside of declarationContainsDependentDeclarations(decl = %p = %s) \n",decl,decl->class_name().c_str());
     printf ("********************************************************** \n");
#endif

     vector<SgDeclarationStatement*> locallyDependentDeclarationList = SageInterface::getDependentDeclarations(decl);

  // printf ("In declarationContainsDependentDeclarations(): locallyDependentDeclarationList: \n");
  // outputPreprocessingInfoList(locallyDependentDeclarationList);

     for (size_t i = 0; i < locallyDependentDeclarationList.size(); i++)
        {
          SgDeclarationStatement* d = locallyDependentDeclarationList[i];
#if 0
          printf ("locallyDependentDeclarationList[%zu] = %p = %s = %s \n",i,d,d->class_name().c_str(),SageInterface::get_name(d).c_str());
#endif
          vector<SgDeclarationStatement*>::iterator j = find(dependentDeclarationList.begin(),dependentDeclarationList.end(),d);
          if (j != dependentDeclarationList.end())
             {
            // These identified declaration must be output as members of the class when it is output in the separate header file.
            // infact ODR may require that the whole class be output! Actually I think we don't have any chioce here!
#if 0
               printf ("Found a dependent declaration buried in the class definition: locallyDependentDeclarationList[%zu] = %p = %s = %s \n",i,d,d->class_name().c_str(),SageInterface::get_name(d).c_str());
#endif
               returnValue = true;
             }
        }

#if 0
     printf ("**************************************************************************** \n");
     printf (" LEAVING: Inside of declarationContainsDependentDeclarations(decl = %p = %s) returnValue = %s \n",decl,decl->class_name().c_str(),returnValue ? "true" : "false");
     printf ("**************************************************************************** \n");
#endif

     return returnValue;
   }
//! Insert an expression (new_exp )before another expression (anchor_exp) has possible side effects, with minimum changes to the original semantics. This is achieved by using a comma operator: (new_exp, anchor_exp). The comma operator is returned.
SgCommaOpExp * SageInterface::insertBeforeUsingCommaOp (SgExpression* new_exp, SgExpression* anchor_exp)
{
  ROSE_ASSERT (new_exp != NULL);
  ROSE_ASSERT (anchor_exp != NULL);
  ROSE_ASSERT (new_exp != anchor_exp);

  SgNode* parent = anchor_exp->get_parent();
  ROSE_ASSERT (parent != NULL);

  //TODO use deep copy may be a better way, avoid reusing the original anchor_exp
  SgCommaOpExp * result = buildCommaOpExp(new_exp, NULL);
  ROSE_ASSERT (result != NULL);
  replaceExpression (anchor_exp, result, true);

  result->set_rhs_operand(anchor_exp);
  anchor_exp->set_parent(result);
  return result ;
}


//! Insert an expression (new_exp ) after another expression (anchor_exp) has possible side effects, with minimum changes to the original semantics. This is done by using two comma operators:  type T1; ... ((T1 = anchor_exp, new_exp),T1) )... , where T1 is a temp variable saving the possible side effect of anchor_exp. The top level comma op exp is returned. The reference to T1 in T1 = anchor_exp is saved in temp_ref.
SgCommaOpExp * SageInterface::insertAfterUsingCommaOp (SgExpression* new_exp, SgExpression* anchor_exp, SgStatement** temp_decl /* = NULL */, SgVarRefExp** temp_ref /* = NULL */)
{
  ROSE_ASSERT (new_exp != NULL);
  ROSE_ASSERT (anchor_exp != NULL);
  ROSE_ASSERT (new_exp != anchor_exp);

  SgNode* parent = anchor_exp->get_parent();
  ROSE_ASSERT (parent != NULL);

  // insert TYPE T1; right before the enclosing statement of anchor_exp
  SgType * t = anchor_exp ->get_type();
  ROSE_ASSERT (t != NULL);
  SgStatement * enclosing_stmt = getEnclosingStatement(anchor_exp);
  ROSE_ASSERT (enclosing_stmt != NULL);

  gensym_counter ++;
  string temp_name = "_t_"+ StringUtility::numberToString(gensym_counter);
  SgVariableDeclaration* t_decl = buildVariableDeclaration(temp_name, t, NULL, enclosing_stmt->get_scope());
  insertStatementBefore (enclosing_stmt, t_decl);
  SgVariableSymbol * temp_sym = getFirstVarSym (t_decl);
  ROSE_ASSERT (temp_sym != NULL);
  if (temp_decl)
    *temp_decl = t_decl;

  // build ((T1 = anchor_exp, new_exp),T1) )
  SgVarRefExp * first_ref = buildVarRefExp(temp_sym);
  if (temp_ref)
    * temp_ref = first_ref;
  SgCommaOpExp * result = buildCommaOpExp ( buildCommaOpExp (buildAssignOp ( first_ref, deepCopy(anchor_exp)), new_exp) , buildVarRefExp(temp_sym));
  replaceExpression (anchor_exp, result, false);

  return result;
}

void
SageInterface::addMessageStatement( SgStatement* stmt, string message )
   {
  // Put out a message in the separate file to lable the dependent CPP directives.
  // --- PreprocessingInfo(DirectiveType, const std::string & inputString, const std::string & filenameString, int line_no, int col_no, int nol, RelativePositionType relPos );
  // SgSourceFile* separateSourceFile = TransformationSupport::getSourceFile(scope);
     string fileName = "separate-outlined-file";
     PreprocessingInfo* messageToUser = new PreprocessingInfo(PreprocessingInfo::C_StyleComment,message,fileName,0,0,1,PreprocessingInfo::before);
  // requiredDirectivesList.push_back(messageToUser);
     stmt->addToAttachedPreprocessingInfo(messageToUser,PreprocessingInfo::before);
   }


// DQ (2/6/2009): Added function to support outlining into separate file.
// Append a function 'decl' into a 'scope', including any referenced declarations required if the scope is within a compiler generated file. All referenced declarations, including those from headers, are inserted if excludeHeaderFiles is set to true (the new file will not have any headers)
void
SageInterface::appendStatementWithDependentDeclaration( SgDeclarationStatement* decl, SgGlobal* scope, SgStatement* original_statement, bool excludeHeaderFiles)
   {
  // New function to support outlining of functions into separate files (with their required declarations).

#if 0
     printf ("***** In SageInterface::appendStatementWithDependentDeclaration(): decl                                    = %p \n",decl);
     printf ("***** In SageInterface::appendStatementWithDependentDeclaration(): decl->get_parent()                      = %p \n",decl->get_parent());
     printf ("***** In SageInterface::appendStatementWithDependentDeclaration(): decl->get_scope()                       = %p \n",decl->get_scope());
     printf ("***** In SageInterface::appendStatementWithDependentDeclaration(): original_statement                      = %p \n",original_statement);
     printf ("***** In SageInterface::appendStatementWithDependentDeclaration(): file (decl)                             = %s \n",TransformationSupport::getSourceFile(decl)->getFileName().c_str());
     printf ("***** In SageInterface::appendStatementWithDependentDeclaration(): decl->get_firstNondefiningDeclaration() = %p \n",decl->get_firstNondefiningDeclaration());
     if (decl->get_firstNondefiningDeclaration() != NULL)
          printf ("***** In SageInterface::appendStatementWithDependentDeclaration(): file (first non-defining)               = %s \n",TransformationSupport::getSourceFile(decl->get_firstNondefiningDeclaration())->getFileName().c_str());
     printf ("***** In SageInterface::appendStatementWithDependentDeclaration(): decl->get_definingDeclaration()         = %p \n",decl->get_definingDeclaration());
     if (decl->get_definingDeclaration() != NULL)
          printf ("***** In SageInterface::appendStatementWithDependentDeclaration(): file (first non-defining)               = %s \n",TransformationSupport::getSourceFile(decl->get_definingDeclaration())->getFileName().c_str());
#endif

#if 0
  // This is the most basic form of what code is required, but now enough for the general cases.
  // Also a copy is required to avoid sharing a statment in two places in the AST (part of what
  // is required for AST consistancy).
     scope->append_declaration (decl);
     decl->set_scope (scope);
     decl->set_parent (scope);
#endif

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
  // Make sure that the input declaration (decl" is consistent in it's representation across more
  // than one file (only a significant test when outlining to a separate file; which is what this
  // function supports).
     ROSE_ASSERT(decl->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(TransformationSupport::getSourceFile(decl) == TransformationSupport::getSourceFile(decl->get_firstNondefiningDeclaration()));
     ROSE_ASSERT(TransformationSupport::getSourceFile(decl->get_scope()) == TransformationSupport::getSourceFile(decl->get_firstNondefiningDeclaration()));

  // DQ (2/6/2009): I need to write this function to support the
  // insertion of the function into the specified scope.  If the
  // file associated with the scope is marked as compiler generated
  // (or as a transformation) then the declarations referenced in the
  // function must be copied as well (those not in include files)
  // and the include files must be copies also. If the SgFile
  // is not compiler generated (or a transformation) then we just
  // append the function to the scope (trivial case).

  // This code will be based on the work in:
  //   developersScratchSpace/Dan/translator_tests/reverseTraversal.C

  // To test this run: "rm moreTest2.o ; make moreTest2.o"
  // in directory: tests/nonsmoke/functional/roseTests/astOutliningTests

  // ***** Also move different loop IR nodes into a common base class *****

  // SgSourceFile* separateSourceFile = TransformationSupport::getSourceFile(scope);

  // DQ (3/2/2009): This now calls a newer function which returns a list of declarations and a list of symbols.
  // The declarations are sometimes outer declarations of nested references to dependent declaration in inner
  // scopes (see moreTest3.cpp).  The Symbol list are the symbols in the old AST that will be mapped to newer
  // symbols generated in the copied AST.
  // Collect the declaration that the input declaration depends upon.
    vector<SgDeclarationStatement*> dependentDeclarationList_inOriginalFile;

    dependentDeclarationList_inOriginalFile = getDependentDeclarations(decl);

  // Generate the copies of all the dependent statements
//     printf ("Fixme: this currently causes the getDependentDeclarations(decl) function to be called twice \n");
//     vector<SgDeclarationStatement*> dependentDeclarationList = generateCopiesOfDependentDeclarations(decl,scope);
     vector<SgDeclarationStatement*> dependentDeclarationList = generateCopiesOfDependentDeclarations(dependentDeclarationList_inOriginalFile,scope);
     ROSE_ASSERT(dependentDeclarationList.size() <= dependentDeclarationList_inOriginalFile.size());

  // Make sure that the input declaration (decl" is consistent in it's representation across more
  // than one file (only a significant test when outlining to a separate file; which is what this
  // function supports).
     ROSE_ASSERT(decl->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(TransformationSupport::getSourceFile(decl) == TransformationSupport::getSourceFile(decl->get_firstNondefiningDeclaration()));
     ROSE_ASSERT(TransformationSupport::getSourceFile(decl->get_scope()) == TransformationSupport::getSourceFile(decl->get_firstNondefiningDeclaration()));

  // This is used to fixup the AST by resetting references to IR nodes (leveraged from AST merge).
     int replacementHashTableSize = 1001;
     std::map<SgNode*, SgNode*> replacementMap(replacementHashTableSize);

  // DQ (3/2/2009): Now use the collectDependentDeclarationsCopyType object to generate the mapping
  // from the symbols in the old AST to the new symbols in the new AST (generated as part of the AST
  // copy mechanism).
     SgCopyHelp::copiedNodeMapType copyNodeMap = collectDependentDeclarationsCopyType.get_copiedNodeMap();
     SgCopyHelp::copiedNodeMapType::iterator copyNodeMapItrator = copyNodeMap.begin();
     while (copyNodeMapItrator != copyNodeMap.end())
        {
       // Loop over all the nodes generated in the AST copy mechanism (accumulated result over multiple copies).
       // const SgNode* first  = const_cast<SgNode*>(copyNodeMapItrator->first);
       // SgNode* first  = copyNodeMapItrator->first;
          SgNode* first  = const_cast<SgNode*>(copyNodeMapItrator->first);
          SgNode* second = copyNodeMapItrator->second;
#if 0
          printf ("copyNodeMapItrator.first = %p = %s second = %p = %s \n",first,first->class_name().c_str(),second,second->class_name().c_str());
#endif
       // Add the SgGlobal referenece to the replacementMap
          if (isSgSymbol(first) != NULL)
             {
#if 0
               printf ("Adding to replacementMap (first = %p = %s = %s , second = %p \n",first,first->class_name().c_str(),SageInterface::get_name(first).c_str(),second);
#endif
               replacementMap.insert(pair<SgNode*,SgNode*>(first,second));
             }

          copyNodeMapItrator++;
        }

#if 0
     printf ("Exiting after test of new functionality \n");
     ROSE_ASSERT(false);
#endif

  // DQ (2/22/2009): We need all the declarations! (moreTest3.cpp demonstrates this, since it drops
  // the "#define SIMPLE 1" which causes it to be treated a "0" (causing errors in the generated code).
     SgSourceFile* sourceFile = TransformationSupport::getSourceFile(original_statement);
     vector<PreprocessingInfo*> requiredDirectivesList = collectCppDirectives(sourceFile);

     SgFunctionDeclaration* outlinedFunctionDeclaration = isSgFunctionDeclaration(decl);
     ROSE_ASSERT(outlinedFunctionDeclaration != NULL);
     SgGlobal* originalFileGlobalScope = TransformationSupport::getGlobalScope(original_statement);
     ROSE_ASSERT(originalFileGlobalScope != NULL);

     printf ("WARNING: In SageInterface::appendStatementWithDependentDeclaration(): I think this is the wrong lookup symbol function that is being used here! \n");

  // DQ (8/16/2013): I think this is the wrong symbol lookup function to be using here, but the API is fixed.
  // SgFunctionSymbol* outlinedFunctionSymbolFromOriginalFile = isSgFunctionSymbol(originalFileGlobalScope->lookup_symbol(outlinedFunctionDeclaration->get_name()));
     SgFunctionSymbol* outlinedFunctionSymbolFromOriginalFile = isSgFunctionSymbol(originalFileGlobalScope->lookup_symbol(outlinedFunctionDeclaration->get_name(),NULL,NULL));

  // SgSymbol* outlinedFunctionSymbolFromOutlinedFile = scope->lookup_symbol(outlinedFunctionDeclaration->get_name());
     ROSE_ASSERT(decl->get_firstNondefiningDeclaration() != NULL);
     SgFunctionSymbol* outlinedFunctionSymbolFromOutlinedFile = isSgFunctionSymbol(decl->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table());
#if 0
     printf ("outlinedFunctionSymbolFromOriginalFile = %p outlinedFunctionSymbolFromOutlinedFile = %p \n",outlinedFunctionSymbolFromOriginalFile,outlinedFunctionSymbolFromOutlinedFile);

     printf ("TransformationSupport::getSourceFile(decl)->getFileName()                                    = %s \n",TransformationSupport::getSourceFile(decl)->getFileName().c_str());
     printf ("TransformationSupport::getSourceFile(decl->get_firstNondefiningDeclaration())->getFileName() = %s \n",TransformationSupport::getSourceFile(decl->get_firstNondefiningDeclaration())->getFileName().c_str());
     printf ("TransformationSupport::getSourceFile(original_statement)->getFileName()                      = %s \n",TransformationSupport::getSourceFile(original_statement)->getFileName().c_str());
#endif

     ROSE_ASSERT(outlinedFunctionSymbolFromOriginalFile != NULL);
     ROSE_ASSERT(outlinedFunctionSymbolFromOutlinedFile != NULL);

     ROSE_ASSERT(outlinedFunctionSymbolFromOriginalFile != outlinedFunctionSymbolFromOutlinedFile);

  // Add the SgFunctionSymbol for the outlined function to the replacement list so that references
  // to the original symbol (in the original file) within the outlined code will be replaced in the
  // AST for the outlined code copied to the separated outlined file.
  // replacementMap.insert(pair<SgNode*,SgNode*>(outlinedFunctionSymbolFromOutlinedFile,outlinedFunctionSymbolFromOriginalFile));
     replacementMap.insert(pair<SgNode*,SgNode*>(outlinedFunctionSymbolFromOriginalFile,outlinedFunctionSymbolFromOutlinedFile));

  // Add the SgGlobal referenece to the replacementMap
     replacementMap.insert(pair<SgNode*,SgNode*>(originalFileGlobalScope,scope));

  // Add the non-defining declarations of the outlined function to the replacementMap
     SgFunctionDeclaration* outlinedNondefiningFunctionDeclarationFromOriginalFile = isSgFunctionDeclaration(outlinedFunctionSymbolFromOriginalFile->get_declaration());
     SgFunctionDeclaration* outlinedNondefiningFunctionDeclarationFromOutlinedFile = isSgFunctionDeclaration(decl->get_firstNondefiningDeclaration());
     ROSE_ASSERT(outlinedNondefiningFunctionDeclarationFromOriginalFile != NULL);
     ROSE_ASSERT(outlinedNondefiningFunctionDeclarationFromOutlinedFile != NULL);
     replacementMap.insert(pair<SgNode*,SgNode*>(outlinedNondefiningFunctionDeclarationFromOriginalFile,outlinedNondefiningFunctionDeclarationFromOutlinedFile));


  // list<SgDeclarationStatement>::iterator i = declarationList.begin();
  // while (i != declarationList.end())
  // for (list<SgDeclarationStatement>::iterator i = declarationList.begin(); i != declarationList.end(); i++)

//      cout<<"\n*******************************************\n"<<endl;
//      cout<<"Inserting dependent decls: count="<<dependentDeclarationList.size()<<endl;
     for (size_t i = 0; i < dependentDeclarationList.size(); i++)
        {
          SgDeclarationStatement* d                   = dependentDeclarationList[i]; // copies of dependent declarations
          SgDeclarationStatement* originalDeclaration = dependentDeclarationList_inOriginalFile[i];
#if 0
          printf ("declarationList[%zu] = %p = %s = %s \n",i,d,d->class_name().c_str(),SageInterface::get_name(d).c_str());
          printf ("originalDeclaration = %p \n",originalDeclaration);

          d->get_file_info()->display("SageInterface::appendStatementWithDependentDeclaration()");
#endif
       // DQ (2/20/2009): Added assertions (fails for moreTest2.cpp)
       // ROSE_ASSERT(d->get_firstNondefiningDeclaration() != NULL);

       // This is not defined for SgNamespaceDeclarationStatement (so it is OK if the copy is NULL)
       // ROSE_ASSERT(d->get_definingDeclaration() != NULL);

       // DQ (2/20/2009): Added assertion.
          ROSE_ASSERT(d->get_parent() == NULL);

       // scope->append_declaration(d);
       // scope->insert_statement (decl, d, /* bool inFront= */ true);
          ROSE_ASSERT(decl->get_scope() == scope);
          ROSE_ASSERT(find(scope->getDeclarationList().begin(),scope->getDeclarationList().end(),decl) != scope->getDeclarationList().end());
          scope->insert_statement (decl, d, /* bool inFront= */ true);
          d->set_parent (scope);

//debug here
//     cout<<d->class_name()<<endl;
       // "d" appears to loose the fact that it is a SgNamespaceDeclarationStatement (because it is not explicitly stored for this case!).
       // d->set_scope (scope);
          if (d->hasExplicitScope() == true)
               d->set_scope (scope);

       // Also set the scope and parent for the firstNondefiningDeclaration (also built via AST Copy)
       // Show this detail to Liao and now to see it in the AST.
          if (d->get_firstNondefiningDeclaration() != NULL)
             {
               d->get_firstNondefiningDeclaration()->set_parent (scope);
               if (d->hasExplicitScope() == true)
                    d->get_firstNondefiningDeclaration()->set_scope (scope);

               ROSE_ASSERT(d->get_firstNondefiningDeclaration()->get_parent() != NULL);
               ROSE_ASSERT(d->get_firstNondefiningDeclaration()->get_parent() == scope);
             }

          if (d->get_definingDeclaration() != NULL)
             {
               d->get_definingDeclaration()->set_parent(scope);

               ROSE_ASSERT(d->get_definingDeclaration()->get_parent() != NULL);
               ROSE_ASSERT(d->get_definingDeclaration()->get_parent() == scope);
               if (d->hasExplicitScope() == true)
                  {
                    d->get_definingDeclaration()->set_scope (scope);
                    ROSE_ASSERT(d->get_definingDeclaration()->get_scope() == scope);
                  }
             }

       // Make sure that internal references are to the same file (else the symbol table information will not be consistent).
          ROSE_ASSERT(d->get_firstNondefiningDeclaration() != NULL);
          ROSE_ASSERT(TransformationSupport::getSourceFile(d) == TransformationSupport::getSourceFile(d->get_firstNondefiningDeclaration()));
          ROSE_ASSERT(TransformationSupport::getSourceFile(d->get_scope()) == TransformationSupport::getSourceFile(d->get_firstNondefiningDeclaration()));

#if 0
          printf ("Add the required symbol information to the symbol table: scope = %p = %s \n",scope,scope->class_name().c_str());
#endif

       // For whatever type of declaration we add to the global scope in the new separate
       // file we have to add the required symbol information to the symbol table.
          switch(d->variantT())
             {
               case V_SgClassDeclaration:
                  {
                 // This is not called, since this function is used to insert functions!
                    ROSE_ASSERT(d->get_firstNondefiningDeclaration() != NULL);
                    SgClassDeclaration* classDeclaration = NULL;
                    if ( declarationContainsDependentDeclarations(d,dependentDeclarationList) == true )
                       {
                      // If we need to reference declaration in the class then we need the same defining
                      // declaration as was in the other file where we outlined the original function from!
                         printf ("Warning: This class contains dependent declarations (not implemented) \n");
                      // ROSE_ASSERT(false);
                         classDeclaration = isSgClassDeclaration(d);
                         ROSE_ASSERT(classDeclaration != NULL);
                       }
                      else
                       {
                         classDeclaration = isSgClassDeclaration(d->get_firstNondefiningDeclaration());
                         ROSE_ASSERT(classDeclaration != NULL);
                       }

                    ROSE_ASSERT(classDeclaration != NULL);

                 // printf ("$$$$$$$$$$$$$$$$$$$$$  Building a SgClassSymbol %s $$$$$$$$$$$$$$$$$$$$$ \n",classDeclaration->get_name().str());
                    SgClassSymbol* classSymbol = new SgClassSymbol(classDeclaration);
                    ROSE_ASSERT(classSymbol != NULL);
                    scope->insert_symbol(classDeclaration->get_name(),classSymbol);
                 // ROSE_ASSERT(classDeclaration->get_symbol_from_symbol_table() != NULL);

                 // printf ("Case of SgClassDeclaration implemented but not being tested \n");
                 // ROSE_ASSERT(false);

                    SgSymbol* symbolInOutlinedFile = classDeclaration->get_symbol_from_symbol_table();
                 // printf ("$$$$$$$$$$$$$$ originalDeclaration = %p = %s \n",originalDeclaration,originalDeclaration->class_name().c_str());
                    ROSE_ASSERT(originalDeclaration != NULL);
                    ROSE_ASSERT(originalDeclaration->get_firstNondefiningDeclaration() != NULL);
                    SgSymbol* symbolInOriginalFile = originalDeclaration->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table();
                 // printf ("$$$$$$$$$$$$$$ case V_SgClassDeclaration: symbolInOriginalFile = %p symbolInOutlinedFile = %p \n",symbolInOriginalFile,symbolInOutlinedFile);

                    ROSE_ASSERT(symbolInOriginalFile != NULL);
                    ROSE_ASSERT(symbolInOutlinedFile != NULL);
                    ROSE_ASSERT(symbolInOriginalFile != symbolInOutlinedFile);

                    replacementMap.insert(pair<SgNode*,SgNode*>(symbolInOutlinedFile,symbolInOriginalFile));
                    break;
                  }

               case V_SgFunctionDeclaration:
                  {
                 // ROSE_ASSERT(d->get_firstNondefiningDeclaration() == NULL);
                    SgFunctionDeclaration* copiedFunctionDeclaration = isSgFunctionDeclaration(d);
                    ROSE_ASSERT(copiedFunctionDeclaration != NULL);
#if 0
                    printf ("$$$$$$$$$$$$$$$$$$$$$  Building a SgFunctionSymbol %s $$$$$$$$$$$$$$$$$$$$$ \n",copiedFunctionDeclaration->get_name().str());
                    printf ("functionDeclaration                                    = %p \n",copiedFunctionDeclaration);
                    printf ("functionDeclaration->get_definingDeclaration()         = %p \n",copiedFunctionDeclaration->get_definingDeclaration());
                    printf ("functionDeclaration->get_firstNondefiningDeclaration() = %p \n",copiedFunctionDeclaration->get_firstNondefiningDeclaration());
                    printf ("lokup symbol in scope =                                = %p \n",scope);
#endif

                    printf ("WARNING: In SageInterface::appendStatementWithDependentDeclaration(): I think this is the wrong lookup symbol function that is being used here! \n");

                 // DQ (8/16/2013): I think this is the wrong symbol lookup function to be using here, but the API is fixed.
                 // if (scope->lookup_symbol(copiedFunctionDeclaration->get_name()) == NULL)
                    if (scope->lookup_symbol(copiedFunctionDeclaration->get_name(),NULL,NULL) == NULL)
                       {
                         SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(copiedFunctionDeclaration);
                         ROSE_ASSERT(functionSymbol != NULL);
                      // printf ("copiedFunctionDeclaration = %p Inserting functionSymbol = %p with name = %s \n",copiedFunctionDeclaration,functionSymbol,copiedFunctionDeclaration->get_name().str());
                         scope->insert_symbol(copiedFunctionDeclaration->get_name(),functionSymbol);

                      // DQ (8/16/2013): I think this is the wrong symbol lookup function to be using here, but the API is fixed.
                      // ROSE_ASSERT(scope->lookup_symbol(copiedFunctionDeclaration->get_name()) == functionSymbol);
                         ROSE_ASSERT(scope->lookup_symbol(copiedFunctionDeclaration->get_name(),NULL,NULL) == functionSymbol);

                         ROSE_ASSERT(copiedFunctionDeclaration->get_symbol_from_symbol_table() != NULL);
                         copiedFunctionDeclaration->set_scope(scope);
                       }
                    ROSE_ASSERT(copiedFunctionDeclaration->get_symbol_from_symbol_table() != NULL);

                 // printf ("decl = %p = %s original_statement = %p = %s \n",decl,decl->class_name().c_str(),original_statement,original_statement->class_name().c_str());
                    printf ("decl = %p = %s originalDeclaration = %p = %s \n",decl,decl->class_name().c_str(),originalDeclaration,originalDeclaration->class_name().c_str());

                 // SgFunctionDeclaration* originalFunctionDeclaration = isSgFunctionDeclaration(decl);
                    SgFunctionDeclaration* originalFunctionDeclaration = isSgFunctionDeclaration(originalDeclaration);
                    ROSE_ASSERT(originalFunctionDeclaration != NULL);

                    SgSymbol* symbolInOutlinedFile = originalFunctionDeclaration->get_symbol_from_symbol_table();
                    SgSymbol* symbolInOriginalFile = copiedFunctionDeclaration->get_symbol_from_symbol_table();
#if 0
                    printf ("symbolInOriginalFile = %p symbolInOutlinedFile = %p \n",symbolInOriginalFile,symbolInOutlinedFile);
#endif
                    ROSE_ASSERT(symbolInOriginalFile != NULL);
                    ROSE_ASSERT(symbolInOutlinedFile != NULL);
                    ROSE_ASSERT(symbolInOriginalFile != symbolInOutlinedFile);

                 // Build up the replacementMap
                 // replacementMap.insert(pair<SgNode*,SgNode*>(node,duplicateNodeFromOriginalAST));
                 // replacementMap.insert(pair<SgNode*,SgNode*>(symbolInOriginalFile,symbolInOutlinedFile));
                    replacementMap.insert(pair<SgNode*,SgNode*>(symbolInOutlinedFile,symbolInOriginalFile));

#if 0
                 // Not sure if this is a problem (at this point)
                    SgFunctionType* functionType = functionDeclaration->get_type();
                    ROSE_ASSERT(functionType != NULL);
                    string functionTypeName = functionType->get_mangled();
                    printf ("Testing the function type: functionTypeName = %s \n",functionTypeName.c_str());
                    if (SgNode::get_globalFunctionTypeTable()->lookup_function_type(functionTypeName) == NULL)
                       {
                         printf ("Adding the function type: functionTypeName = %s \n",functionTypeName.c_str());
                         SgNode::get_globalFunctionTypeTable()->insert_function_type(functionTypeName,functionType);
                       }
#endif
                 // printf ("Case of SgFunctionDeclaration implemented but not being tested \n");
                 // ROSE_ASSERT(false);

                    break;
                  }

               case V_SgMemberFunctionDeclaration:
                  {
                 // ROSE_ASSERT(d->get_firstNondefiningDeclaration() == NULL);
                    SgMemberFunctionDeclaration* copiedMemberFunctionDeclaration = isSgMemberFunctionDeclaration(d);
                    ROSE_ASSERT(copiedMemberFunctionDeclaration != NULL);

                    printf ("Sorry, support for dependent member function declarations not implemented! \n");
                    ROSE_ASSERT(false);

                    break;
                  }

                case V_SgTemplateInstantiationDecl:
                  {
                    printf ("Sorry, not implemented: case SgTemplateInstantiationDecl not handled as dependent declaration \n");
                    d->get_file_info()->display("Sorry, not implemented: case SgTemplateInstantiationDecl not handled as dependent declaration");

                    printf ("Case of SgTemplateInstantiationDecl not implemented. \n");
                    ROSE_ASSERT(false);
                    break;
                  }

                case V_SgNamespaceDeclarationStatement:
                  {
                 // printf ("Sorry, not implemented: case SgNamespaceDeclarationStatement not handled as dependent declaration \n");
                 // d->get_file_info()->display("Sorry, not implemented: case SgNamespaceDeclarationStatement not handled as dependent declaration");

                    ROSE_ASSERT(d->get_firstNondefiningDeclaration() != NULL);
                    SgNamespaceDeclarationStatement* namespaceDeclaration = NULL;
                    if ( declarationContainsDependentDeclarations(d,dependentDeclarationList) == true )
                       {
                      // If we need to reference declaration in the class then we need the same defining
                      // declaration as was in the other file where we outlined the original function from!
                         printf ("Warning: This namespace contains dependent declarations (not supported) \n");
                      // ROSE_ASSERT(false);
                         namespaceDeclaration = isSgNamespaceDeclarationStatement(d);
                         ROSE_ASSERT(namespaceDeclaration != NULL);
                       }
                      else
                       {
                         namespaceDeclaration = isSgNamespaceDeclarationStatement(d->get_firstNondefiningDeclaration());
                         ROSE_ASSERT(namespaceDeclaration != NULL);
                       }

                    ROSE_ASSERT(namespaceDeclaration != NULL);

                 // DQ (2/22/2009): This should not already be present! If it is present then we don't want to add it (for now detect if it is present).
                    ROSE_ASSERT(scope->lookup_namespace_symbol(namespaceDeclaration->get_name()) == NULL);

                    SgNamespaceSymbol* namespaceSymbol = new SgNamespaceSymbol(namespaceDeclaration->get_name(),namespaceDeclaration);
                    ROSE_ASSERT(namespaceSymbol != NULL);
                    scope->insert_symbol(namespaceDeclaration->get_name(),namespaceSymbol);

                    break;
                  }
                // Liao, 5/7/2009 handle more types of declarations
                case V_SgTypedefDeclaration:
                  {
                    // symbol is associated with the first non-defining declaration
                    SgTypedefDeclaration* typedef_decl = isSgTypedefDeclaration(isSgTypedefDeclaration(d)->get_firstNondefiningDeclaration());
                    ROSE_ASSERT(typedef_decl);
                    // 1 make a symbol in the target scope
                    SgTypedefSymbol * tsymbol = new SgTypedefSymbol(typedef_decl);
                    scope->insert_symbol(typedef_decl->get_name(), tsymbol);

                    SgSymbol* symbolInOutlinedFile = typedef_decl->get_symbol_from_symbol_table();
                    ROSE_ASSERT(symbolInOutlinedFile != NULL);
                    // 2 build a map between old and new symbol
                    ROSE_ASSERT(originalDeclaration != NULL);
                    // symbol is associated with the first non-defining declaration
                    SgSymbol* symbolInOriginalFile = originalDeclaration->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table();
                    ROSE_ASSERT(symbolInOriginalFile != NULL);

                    ROSE_ASSERT(symbolInOriginalFile != symbolInOutlinedFile);
                    replacementMap.insert(pair<SgNode*,SgNode*>(symbolInOutlinedFile,symbolInOriginalFile));
                    break;
                  }
               case V_SgEnumDeclaration:
                  {
                    SgEnumDeclaration * decl = isSgEnumDeclaration(isSgEnumDeclaration(d)->get_firstNondefiningDeclaration());
                    ROSE_ASSERT(decl);
                    SgEnumSymbol * symbol = new SgEnumSymbol(decl);
                    ROSE_ASSERT(symbol);
                    scope->insert_symbol(decl->get_name(), symbol);
                    SgSymbol* symbolInOutlinedFile = decl->get_symbol_from_symbol_table();
                    ROSE_ASSERT(symbolInOutlinedFile != NULL);

                    ROSE_ASSERT(originalDeclaration != NULL);
                    SgSymbol* symbolInOriginalFile = originalDeclaration->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table();
                    ROSE_ASSERT(symbolInOriginalFile != NULL);
                    ROSE_ASSERT(symbolInOriginalFile != symbolInOutlinedFile);
                    replacementMap.insert(pair<SgNode*,SgNode*>(symbolInOutlinedFile,symbolInOriginalFile));
                    break;
                  }
               default:
                  {
                    printf ("default case in SageInterface::appendStatementWithDependentDeclaration() (handling dependentDeclarationList) d = %p = %s \n",d,d->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }


       // Collect include directives that are already attached to this dependent declaration.
          vector<PreprocessingInfo*> cppDirectivesAlreadyAttachedToDependentDeclarations = collectCppDirectives(d);

#if 0
          printf ("directives BEFORE excluding those already present in dependent declarations \n");
          outputPreprocessingInfoList(requiredDirectivesList);

          printf ("directives already attached to dependent declarations \n");
          outputPreprocessingInfoList(cppDirectivesAlreadyAttachedToDependentDeclarations);
#endif

       // Remove these include directives from the requiredDirectivesList (to prevent redundant output in the generated file)
          vector<PreprocessingInfo*>::iterator j = cppDirectivesAlreadyAttachedToDependentDeclarations.begin();
          while ( j != cppDirectivesAlreadyAttachedToDependentDeclarations.end() )
             {
            // Remove this directive from the requiredDirectivesList (to avoid having them output redundently).
               vector<PreprocessingInfo*>::iterator entry = find(requiredDirectivesList.begin(),requiredDirectivesList.end(),*j);
               ROSE_ASSERT(entry != requiredDirectivesList.end());

               requiredDirectivesList.erase(entry);

               j++;
             }

#if 0
          printf ("directives AFTER excluding those already present in dependent declarations \n");
          outputPreprocessingInfoList(requiredDirectivesList);
#endif
        }

  // Add a message to the top of the outlined function that has been added
     addMessageStatement(decl,"/* OUTLINED FUNCTION */");

  // Insert the dependent declarations ahead of the input "decl".
     SgStatement* firstStatmentInFile = NULL;
     if (dependentDeclarationList.empty() == true)
        {
          firstStatmentInFile = decl;
        }
       else
        {
          firstStatmentInFile = dependentDeclarationList[0];
        }

     ROSE_ASSERT(firstStatmentInFile != NULL);

  // Add a message to the top of the dependent declarations that have been added
     addMessageStatement(firstStatmentInFile,"/* REQUIRED DEPENDENT DECLARATIONS */");

  // DQ (3/6/2009): Added support to permit exclusion of "#include<header.h>" files since they can make it
  // much more difficult for external tools. Later we will check if there are remaining unsatisfied dependent
  // declarations (which must be in the header file) so we can automate this step.
     if (excludeHeaderFiles == false)
        {
       // Include all the "#include<header.h>" cpp directives obtained from the original file.
          vector<PreprocessingInfo*>::reverse_iterator j = requiredDirectivesList.rbegin();
          while ( j != requiredDirectivesList.rend() )
             {
               firstStatmentInFile->addToAttachedPreprocessingInfo(*j,PreprocessingInfo::before);
               j++;
             }
        }

  // Add a message to the top of the CPP directives that have been added
     addMessageStatement(firstStatmentInFile,"/* REQUIRED CPP DIRECTIVES */");

  // ****************************************************************************
  // ****************  Fixup AST to Reset References To IR nodes  ***************
  // ****************************************************************************
  // This traversal of the replacement map modified the AST to reset pointers to subtrees that will be shared.
  // The whole AST is traversed (using the memory pool traversal) and the data member pointers to IR nodes that
  // are found in the replacement map are used to lookup the replacement values that are used to reset the
  // pointers in the AST. As the replacement is computed the pointer values that are marked in the replacement
  // list for update are added to the intermediateDeleteSet.

     SgSourceFile* outlinedFile = TransformationSupport::getSourceFile(scope);
     ROSE_ASSERT(outlinedFile != NULL);

  // This is required because the AST merge interface records a delete list
  // (these nodes are not deleted for the outlining support).
     set<SgNode*>  intermediateDeleteSet;

  // This replacement will be done over the entire file (parts of it are redundant with what has already
  // been done by the AST copy (so this step need not do as much and may be reduced to just operating
  // on the outlined function, I think).
#if 0
     printf ("\n\n************************************************************\n");
     printf ("Calling fixupSubtreeTraversal() \n");
#endif

     fixupSubtreeTraversal(outlinedFile,replacementMap,intermediateDeleteSet);

#if 0
     printf ("Calling fixupSubtreeTraversal(): DONE \n");
     printf ("************************************************************\n\n");

     printf ("\n\n After replacementMapTraversal(): intermediateDeleteSet: \n");
     displaySet(intermediateDeleteSet,"After fixupTraversal");
#endif

  // Repeated test from above
     ROSE_ASSERT(dependentDeclarationList.size() <= dependentDeclarationList_inOriginalFile.size());

// endif for ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
#endif

#if 0
  // The replacementMap should include the symbols associated with the dependentDeclarationList
  // and the outlined function (so dependentDeclarationList.size() + 1).
     printf ("replacementMap.size() = %zu dependentDeclarationList.size() = %zu \n",replacementMap.size(),dependentDeclarationList.size());
  // ROSE_ASSERT(replacementMap.size() == dependentDeclarationList.size() + 1);
#endif

#if 0
     printf ("Exiting as a test \n");
     ROSE_ASSERT(false);
#endif
   }



#if 0
// This function is not implemented our used, but it might be when the final code is refactored.

// rose_hash::unordered_map<SgNode*, SgNode*, hash_nodeptr>
void
SageInterface::supplementReplacementSymbolMap ( rose_hash::unordered_map<SgNode*, SgNode*, hash_nodeptr> & inputReplacementMap )
   {
  // Prior to the call to fixupSubtreeTraversal(), we have to build the "replacementMap".  The "replacementMap"
  // contains the IR node address references into the old AST (what was copied) which need to be replaced in the
  // new AST (the copy).  In general the old AST is the original file from which outlining was done and the new
  // AST is the separate file where the outlined code has been placed with its dependent declarations.
  // This function adds to the previously assembled "replacementMap"


   }
#endif


void
SageInterface::deleteAST ( SgNode* n )
   {
//Tan, August/25/2010:       //Re-implement DeleteAST function

        //Use MemoryPoolTraversal to count the number of references to a certain symbol
        //This class defines the visitors for the MemoryPoolTraversal

        class ClassicVisitor : public ROSE_VisitorPattern
        {
                private:
                int SgVariableSymbol_count;
                int SgFunctionSymbol_count;
                int SgClassDeclaration_count;
                int SgTypedefSymbol_count;
                int SgMemFuncSymbol_count;
                int SgTemplateSymbol_count;
                int SgEnumFieldSymbol_count;

                SgVariableSymbol* SgVariableSymbolPtr;
                SgFunctionSymbol* SgFunctionSymbolPtr;
                SgClassSymbol * SgClassSymbolPtr;
                SgTypedefSymbol * SgTypedefPtr;
                SgEnumFieldSymbol * SgEnumFieldSymbolPtr;
                SgMemberFunctionSymbol * SgMemFuncSymbolPtr;
                SgTemplateSymbol * SgTemplateSymbolPtr;
                SgClassDeclaration * class_defining;
                SgTemplateDeclaration * template_defining;
                SgMemberFunctionDeclaration * memFunc;
                SgTypedefDeclaration * typedef_defining;
                SgFunctionDeclaration * function_decl;
                SgTemplateInstantiationDecl * templateInstantiate_defining;

                public:
                ClassicVisitor(SgVariableSymbol* symbol){
                        SgVariableSymbol_count = 0;
                        SgVariableSymbolPtr = symbol;
                        SgFunctionSymbolPtr =NULL;
                        SgClassSymbolPtr =NULL;
                        SgTypedefPtr = NULL;
                        SgMemFuncSymbolPtr =NULL;
                        class_defining = NULL;
                        memFunc =NULL;
                        typedef_defining =NULL;
                        function_decl = NULL;
                        SgTemplateSymbolPtr = NULL;
                        template_defining = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                        templateInstantiate_defining =NULL;
                }

                ClassicVisitor(SgFunctionSymbol* symbol){
                        SgFunctionSymbol_count = 0;
                        SgFunctionSymbolPtr = symbol;

                     // DQ (5/2/2013): Added to fix test2013_141.C.
                        SgMemFuncSymbol_count =0;

                        SgVariableSymbolPtr = NULL;
                        SgClassSymbolPtr =NULL;
                        SgTypedefPtr = NULL;
                        SgMemFuncSymbolPtr =NULL;
                        class_defining = NULL;
                        memFunc =NULL;
                        typedef_defining =NULL;
                        function_decl = NULL;
                        SgTemplateSymbolPtr = NULL;
                        template_defining = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                        templateInstantiate_defining =NULL;
                }

                ClassicVisitor(SgClassSymbol* symbol){
                        SgClassDeclaration_count = 0;
                        SgClassSymbolPtr = symbol;
                        SgFunctionSymbolPtr = NULL;
                        SgVariableSymbolPtr = NULL;
                        SgTypedefPtr = NULL;
                        SgMemFuncSymbolPtr =NULL;
                        class_defining = NULL;
                        memFunc =NULL;
                        typedef_defining =NULL;
                        function_decl = NULL;
                        SgTemplateSymbolPtr = NULL;
                        template_defining = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                        templateInstantiate_defining =NULL;
                }

                ClassicVisitor(SgTypedefSymbol* symbol){
                        SgTypedefSymbol_count =0;
                        SgTypedefPtr = symbol;
                        SgClassSymbolPtr = NULL;
                        SgFunctionSymbolPtr = NULL;
                        SgVariableSymbolPtr = NULL;
                        SgMemFuncSymbolPtr =NULL;
                        class_defining = NULL;
                        memFunc =NULL;
                        typedef_defining =NULL;
                        function_decl = NULL;
                        SgTemplateSymbolPtr = NULL;
                        template_defining = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                        templateInstantiate_defining =NULL;
                }

                ClassicVisitor(SgMemberFunctionSymbol* symbol){
                        SgMemFuncSymbolPtr = symbol;
                        SgMemFuncSymbol_count =0;
                        SgTypedefPtr = NULL;
                        SgClassSymbolPtr = NULL;
                        SgFunctionSymbolPtr = NULL;
                        SgVariableSymbolPtr = NULL;
                        class_defining = NULL;
                        memFunc =NULL;
                        typedef_defining =NULL;
                        function_decl = NULL;
                        SgTemplateSymbolPtr = NULL;
                        template_defining = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                        templateInstantiate_defining =NULL;
                }

                ClassicVisitor(SgTemplateSymbol* symbol){
                        SgTemplateSymbolPtr = symbol;
                        SgTemplateSymbol_count =0;
                        SgMemFuncSymbolPtr = NULL;
                        SgTypedefPtr = NULL;
                        SgClassSymbolPtr = NULL;
                        SgFunctionSymbolPtr = NULL;
                        SgVariableSymbolPtr = NULL;
                        class_defining = NULL;
                        memFunc =NULL;
                        typedef_defining =NULL;
                        function_decl = NULL;
                        template_defining = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                        templateInstantiate_defining =NULL;
                }

                ClassicVisitor(SgEnumFieldSymbol* symbol){
                        SgEnumFieldSymbolPtr = symbol;
                        SgEnumFieldSymbol_count =0;
                        SgTemplateSymbolPtr = NULL;
                        SgMemFuncSymbolPtr = NULL;
                        SgTypedefPtr = NULL;
                        SgClassSymbolPtr = NULL;
                        SgFunctionSymbolPtr = NULL;
                        SgVariableSymbolPtr = NULL;
                        class_defining = NULL;
                        memFunc =NULL;
                        typedef_defining =NULL;
                        function_decl = NULL;
                        template_defining = NULL;
                        templateInstantiate_defining =NULL;
                }


                ClassicVisitor(SgClassDeclaration* node){
                        class_defining = node;
                        SgMemFuncSymbolPtr = NULL;
                        SgTypedefPtr = NULL;
                        SgClassSymbolPtr = NULL;
                        SgFunctionSymbolPtr = NULL;
                        SgVariableSymbolPtr = NULL;
                        memFunc =NULL;
                        typedef_defining =NULL;
                        function_decl = NULL;
                        SgTemplateSymbolPtr = NULL;
                        template_defining = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                        templateInstantiate_defining =NULL;
                }

                ClassicVisitor(SgTemplateDeclaration* node){
                        template_defining = node;
                        class_defining = NULL;
                        SgMemFuncSymbolPtr = NULL;
                        SgTypedefPtr = NULL;
                        SgClassSymbolPtr = NULL;
                        SgFunctionSymbolPtr = NULL;
                        SgVariableSymbolPtr = NULL;
                        memFunc =NULL;
                        typedef_defining =NULL;
                        function_decl = NULL;
                        SgTemplateSymbolPtr = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                        templateInstantiate_defining =NULL;
                }
                ClassicVisitor(SgFunctionDeclaration* node){
                        function_decl =node;
                        class_defining = NULL;
                        SgMemFuncSymbolPtr = NULL;
                        SgTypedefPtr = NULL;
                        SgClassSymbolPtr = NULL;
                        SgFunctionSymbolPtr = NULL;
                        SgVariableSymbolPtr = NULL;
                        memFunc =NULL;
                        typedef_defining =NULL;
                        SgTemplateSymbolPtr = NULL;
                        template_defining = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                        templateInstantiate_defining =NULL;
                }

                ClassicVisitor(SgMemberFunctionDeclaration* node){
                        memFunc = node;
                        function_decl =NULL;
                        class_defining = NULL;
                        SgMemFuncSymbolPtr = NULL;
                        SgTypedefPtr = NULL;
                        SgClassSymbolPtr = NULL;
                        SgFunctionSymbolPtr = NULL;
                        SgVariableSymbolPtr = NULL;
                        typedef_defining =NULL;
                        SgTemplateSymbolPtr = NULL;
                        template_defining = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                        templateInstantiate_defining =NULL;
                }

                ClassicVisitor(SgTypedefDeclaration* node){
                        typedef_defining = node;
                        memFunc = NULL;
                        function_decl =NULL;
                        class_defining = NULL;
                        SgMemFuncSymbolPtr = NULL;
                        SgTypedefPtr = NULL;
                        SgClassSymbolPtr = NULL;
                        SgFunctionSymbolPtr = NULL;
                        SgVariableSymbolPtr = NULL;
                        SgTemplateSymbolPtr = NULL;
                        template_defining = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                        templateInstantiate_defining =NULL;
                }

                ClassicVisitor(SgTemplateInstantiationDecl* node){
                        templateInstantiate_defining =node;
                        typedef_defining = NULL;
                        memFunc = NULL;
                        function_decl =NULL;
                        class_defining = NULL;
                        SgMemFuncSymbolPtr = NULL;
                        SgTypedefPtr = NULL;
                        SgClassSymbolPtr = NULL;
                        SgFunctionSymbolPtr = NULL;
                        SgVariableSymbolPtr = NULL;
                        SgTemplateSymbolPtr = NULL;
                        template_defining = NULL;
                        SgEnumFieldSymbolPtr = NULL;
                }


        // SgVariableSymbol and SgEnumFieldSymbol
                void visit(SgInitializedName* node)
                   {
                     if(SgVariableSymbolPtr !=NULL)
                        {
                          if(node->get_scope()!=NULL)
                             {
                            // DQ (5/21/2013): We want to restrict access to the symbol table.
                               if(node->get_scope()->get_symbol_table()!=NULL)
                                  {
                                    SgSymbol* s = node->get_symbol_from_symbol_table();
                                    if (isSgVariableSymbol(s) == SgVariableSymbolPtr) SgVariableSymbol_count++;
                                  }
                             }
                        }

                     if(SgEnumFieldSymbolPtr !=NULL)
                        {
                          if(node->get_scope()!=NULL)
                             {
                            // DQ (5/21/2013): We want to restrict access to the symbol table.
                               if(node->get_scope()->get_symbol_table()!=NULL)
                                  {
                                    SgSymbol* s = node->get_symbol_from_symbol_table();
                                    if (isSgEnumFieldSymbol(s) == SgEnumFieldSymbolPtr) SgEnumFieldSymbol_count++;
                                  }
                             }
                        }
                   }

                void visit(SgVarRefExp* node)
                {
                        if(SgVariableSymbolPtr !=NULL){
                                SgVariableSymbol* s = node->get_symbol();
                                if (s == SgVariableSymbolPtr) SgVariableSymbol_count++;
                        }
                }

                int get_num_variable_pointers(){return SgVariableSymbol_count;}

                int get_num_EnumField_pointers(){return SgEnumFieldSymbol_count;}


        // SgFunctionSymbol
                void visit(SgFunctionDeclaration* node)         {
                        if(SgFunctionSymbolPtr !=NULL){
                                if(node->get_scope()!=NULL){
                                   // DQ (5/21/2013): We want to restrict access to the symbol table.
                                      if(node->get_scope()->get_symbol_table()!=NULL)
                                        {
                                                SgSymbol* s = ((SgFunctionDeclaration*)node)->get_symbol_from_symbol_table();
                                                if ((SgFunctionSymbol *)s == SgFunctionSymbolPtr) SgFunctionSymbol_count++;
                                        }
                                }
                        }
#if 0
                        if(function_decl!=NULL){
                                if(node->get_symbol_from_symbol_table() == NULL){
                                        SgDeclarationStatement * define = ((SgDeclarationStatement*)node)->get_definingDeclaration();
                                        SgDeclarationStatement * first_nondefine = ((SgDeclarationStatement*)node)->get_firstNondefiningDeclaration();
                                        if(node!=function_decl && (define==function_decl || first_nondefine==function_decl)) delete node;
                                }
                        }
#endif
                }

                void visit(SgFunctionRefExp* node)
                {
#if 0
                        printf ("In visit(SgFunctionRefExp* node): SgFunctionSymbolPtr = %p \n",SgFunctionSymbolPtr);
#endif
                        if (SgFunctionSymbolPtr !=NULL)
                        {
                                SgFunctionSymbol* s = node->get_symbol_i();
                                if (isSgFunctionSymbol(s) == SgFunctionSymbolPtr) 
                                {
                                  SgFunctionSymbol_count++;
#if 0
                                  printf ("Increment SgFunctionSymbol_count = %d \n",SgFunctionSymbol_count);
#endif
                                }
                        }
                }

             // DQ (5/2/2013): Added support for SgMemberFunctionRefExp which is not derived from SgFunctionRefExp.
                void visit(SgMemberFunctionRefExp* node)
                {
#if 0
                        printf ("In visit(SgMemberFunctionRefExp* node): SgFunctionSymbolPtr = %p \n",SgFunctionSymbolPtr);
#endif
                        if (SgFunctionSymbolPtr !=NULL)
                        {
                                SgFunctionSymbol* s = node->get_symbol_i();
                                if (isSgFunctionSymbol(s) == SgFunctionSymbolPtr) 
                                {
                                  SgFunctionSymbol_count++;
#if 0
                                  printf ("Increment SgFunctionSymbol_count = %d \n",SgFunctionSymbol_count);
#endif
                                }
                        }
                }

                void visit(SgUserDefinedBinaryOp* node)
                {
                        if (SgFunctionSymbolPtr !=NULL){
                                SgFunctionSymbol* s = node->get_symbol();
                                if (isSgFunctionSymbol(s) == SgFunctionSymbolPtr) SgFunctionSymbol_count++;
                        }
                }

                int get_num_Function_pointers(){return SgFunctionSymbol_count;}

        // SgClassSymbol
                void visit(SgClassDeclaration* node)
                {
                        if(SgClassSymbolPtr !=NULL){
                                if(node->get_scope()!=NULL){
                                     if(node->get_scope()->get_symbol_table()!=NULL)
                                        {
                                                SgSymbol* s = node->get_symbol_from_symbol_table();
                                                if (isSgClassSymbol(s) == SgClassSymbolPtr) SgClassDeclaration_count++;
                                        }
                                }
                        }

                        if(class_defining!=NULL) {
                                if(node->get_symbol_from_symbol_table() == NULL){
                                        SgDeclarationStatement * class_decl = ((SgDeclarationStatement*)node)->get_firstNondefiningDeclaration();
                                        SgDeclarationStatement * class_decl1 = ((SgDeclarationStatement*)node)->get_definingDeclaration();
                                        if((class_decl==class_defining||class_decl1==class_defining) && node!=class_defining )
                                                delete node;
                                }
                        }
                }

                void visit(SgTemplateInstantiationDecl* node)
                {
                        if(SgClassSymbolPtr !=NULL){
                                if(node->get_scope()!=NULL){
                                     if(node->get_scope()->get_symbol_table()!=NULL)
                                        {
                                                SgSymbol* s = node->get_symbol_from_symbol_table();
                                                if (isSgClassSymbol(s) == SgClassSymbolPtr) SgClassDeclaration_count++;
                                        }
                                }
                        }

                        if(templateInstantiate_defining!=NULL) {
                                if(node->get_scope()!=NULL){
                                     if(node->get_scope()->get_symbol_table()!=NULL)
                                        {
                                                if(node->get_symbol_from_symbol_table() == NULL){
                                                        SgDeclarationStatement * template_decl = ((SgDeclarationStatement*)node)->get_firstNondefiningDeclaration();
                                                        SgDeclarationStatement * template_decl1 = ((SgDeclarationStatement*)node)->get_definingDeclaration();
                                                        if((template_decl==templateInstantiate_defining||template_decl1==templateInstantiate_defining) && node!=templateInstantiate_defining){
                                                                /*vector<SgTemplateArgument*> tempargs=  ((SgTemplateInstantiationDecl*)node)->get_templateArguments();
                                                                foreach (SgTemplateArgument* element, tempargs){
                                                                        SgTemplateArgument* temparg = isSgTemplateArgument(element);
                                                                        if(temparg){
                                                                                delete temparg;
                                                                        }
                                                                        printf("SgTemplateArg in Memory Pool traversal\n");
                                                                }*/
                                                                delete node;
                                                                //printf("SgTemplateInstantiationDecl in Memory Pool traversal\n");
                                                        }
                                                }
                                        }
                                }
                        }
                }

                void visit(SgThisExp* node)
                {
                        if (SgClassSymbolPtr !=NULL){
                                SgSymbol* s = node->get_class_symbol();
                                if (s == SgClassSymbolPtr) SgClassDeclaration_count++;
                        }
                }

                void visit(SgClassNameRefExp* node)
                {
                        if (SgClassSymbolPtr !=NULL){
                                SgSymbol* s = node->get_symbol();
                                if (isSgClassSymbol(s) == SgClassSymbolPtr) SgClassDeclaration_count++;
                        }
                }


                int get_num_Class_pointers(){return SgClassDeclaration_count;}


        // SgMemberFunctionSymbol
                void visit(SgCtorInitializerList* node)
                {
                        if(memFunc !=NULL){
                                SgMemberFunctionDeclaration * func= (SgMemberFunctionDeclaration*) (node->get_parent());
                                if(func == memFunc){
                                        delete node;
                                }
                        }
                }


                void visit(SgMemberFunctionDeclaration* node)
                {
                        if (SgMemFuncSymbolPtr !=NULL){
                                if(node->get_scope()!=NULL){
                                     if(node->get_scope()->get_symbol_table()!=NULL)
                                        {
                                                SgSymbol* symbol = ((SgMemberFunctionDeclaration*)node)->get_symbol_from_symbol_table();
                                                if(symbol == SgMemFuncSymbolPtr){
                                                        SgMemFuncSymbol_count++;
                                                }
                                        }
                                }
                        }
                }

                void visit(SgTemplateInstantiationMemberFunctionDecl* node)
                {
                        if (SgMemFuncSymbolPtr !=NULL){
                                if(node->get_scope()!=NULL){
                                     if(node->get_scope()->get_symbol_table()!=NULL)
                                        {
                                                SgSymbol* symbol = ((SgTemplateInstantiationMemberFunctionDecl*)node)->get_symbol_from_symbol_table();
                                                if(symbol == SgMemFuncSymbolPtr){
                                                        SgMemFuncSymbol_count++;
                                                }
                                        }
                                }
                        }
                }



                int get_num_memFunc_pointers(){return SgMemFuncSymbol_count;}


        // SgTypedefSymbol
                void visit(SgTypedefDeclaration* node)
                {
                        if(SgTypedefPtr!=NULL){
                                if(node->get_scope()!=NULL){
                                     if(node->get_scope()->get_symbol_table()!=NULL)
                                        {
                                                SgSymbol* s = ((SgTypedefDeclaration*)node)->get_symbol_from_symbol_table();
                                                if ((SgTypedefSymbol *)s == SgTypedefPtr) SgTypedefSymbol_count++;
                                        }
                                }
                        }
                        if(typedef_defining!=NULL){
                                SgDeclarationStatement * typedef_define = ((SgDeclarationStatement*)node)->get_definingDeclaration();
                                if(typedef_define == typedef_defining && node != typedef_defining ) {
                                        delete node;
                                }
                        }
                }

                int get_num_Typedef_pointers(){return SgTypedefSymbol_count;}



                void visit(SgTemplateDeclaration* node)
                {
                        if (SgTemplateSymbolPtr !=NULL){
                                if(node->get_scope()!=NULL){
                                     if(node->get_scope()->get_symbol_table()!=NULL)
                                        {
                                                SgSymbol* symbol = ((SgTemplateDeclaration*)node)->get_symbol_from_symbol_table();
                                                if(symbol == SgTemplateSymbolPtr){
                                                        SgTemplateSymbol_count++;
                                                }
                                        }
                                }
                        }

                        if(template_defining !=NULL) {
                                if(node->get_scope()!=NULL){
                                     if(node->get_scope()->get_symbol_table()!=NULL)
                                        {
                                                if(node->get_symbol_from_symbol_table() == NULL){
                                                        SgDeclarationStatement * template_decl = ((SgDeclarationStatement*)node)->get_firstNondefiningDeclaration();
                                                        SgDeclarationStatement * template_decl1 = ((SgDeclarationStatement*)node)->get_definingDeclaration();
                                                        if((template_decl==template_defining||template_decl1==template_defining) && node!=template_defining) {
                                                                delete node;

                                                        }
                                                }
                                        }
                                }
                        }
                }

                int get_num_Template_pointers(){return SgTemplateSymbol_count;}

        };


        //Tan August,25,2010 //Traverse AST in post order, delete nodes and their symbols if it's safe to do so
        class DeleteAST : public SgSimpleProcessing,  ROSE_VisitTraversal
                {
                        public:

                        void visit (SgNode* node)
                        {
                        //These nodes are manually deleted because they cannot be visited by the traversal
                                /*////////////////////////////////////////////////
                                /remove SgVariableDefinition, SgVariableSymbol and SgEnumFieldSymbol
                                /////////////////////////////////////////////////*/
#if 0
                                printf ("In DeleteAST::visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif
#if 0
                             // DQ (3/2/2014): I think this might be a problem...
                             // DQ (3/1/2014): check for a SgScopeStatement and delete the associated local type table.
                                if (isSgScopeStatement(node) !=NULL)
                                   {
                                     SgScopeStatement* scope = isSgScopeStatement(node);
#if 1
                                     printf ("Deleting the scopes type table: scope->get_type_table() = %p \n",scope->get_type_table());
#endif
                                     delete scope->get_type_table();
                                   }
#endif
#if 0
                             // DQ (3/2/2014): I think this might be a problem...
                             // DQ (3/1/2014): check for a SgScopeStatement and delete the associated local type table.
                                if (isSgTypeTable(node) !=NULL)
                                   {
                                     SgTypeTable* typeTable = isSgTypeTable(node);
#if 1
                                     printf ("Deleting the type table (SgSymbolTable): typeTable->get_type_table() = %p \n",typeTable->get_type_table());
#endif
                                     delete typeTable->get_type_table();
                                   }
#endif
                                if(isSgInitializedName(node) !=NULL){
                                        //remove SgVariableDefinition
                                        SgDeclarationStatement* var_def;
                                        var_def =  ((SgInitializedName *)node)->get_definition();
                                        if(isSgVariableDefinition(var_def) !=NULL){
                                                delete var_def;
                                                //printf("A SgVariableDefinition was deleted\n");
                                        }


                                        //remove SgVariableSymbol
                                        if(isSgInitializedName(node)->get_scope()!=NULL){
                                             if(isSgInitializedName(node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgInitializedName *)node)->get_symbol_from_symbol_table();
                                                        if(isSgVariableSymbol(symbol) !=NULL){
                                                                ClassicVisitor visitor((SgVariableSymbol*)symbol);
                                                                traverseMemoryPoolVisitorPattern(visitor);
                                                                if(visitor.get_num_variable_pointers()==1){ //only one reference to this symbol => safe to delete
                                                                ((SgInitializedName*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                        delete symbol;
                                                                        //printf("A SgVariableSymbol was deleted\n");
                                                                }
                                                        }

                                                        if(isSgEnumFieldSymbol(symbol) !=NULL){
                                                                ClassicVisitor visitor((SgEnumFieldSymbol*)symbol);
                                                                traverseMemoryPoolVisitorPattern(visitor);
                                                                if(visitor.get_num_EnumField_pointers()==1){
                                                                        ((SgInitializedName*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                        delete symbol;
                                                                        //printf("A SgEnumFieldSymbol was deleted\n");
                                                                }
                                                        }

                                                }
                                        }
                                }

                                if(isSgVarRefExp(node) !=NULL){
                                                SgVariableSymbol *symbol = ((SgVarRefExp*)node)->get_symbol();
                                                ClassicVisitor visitor(symbol);
                                                traverseMemoryPoolVisitorPattern(visitor);
                                                if(visitor.get_num_variable_pointers()==1){ //only one reference to this symbol => safe to delete
                                                        //((SgSymbol*)symbol)->get_scope()->get_symbol_table()->remove(symbol);
                                                        delete symbol;
                                                        //printf("A SgVariableSymbol was deleted\n");
                                                }
                                }

                                /*////////////////////////////////////////////////
                                /remove SgFunctionSymbol
                                /////////////////////////////////////////////////*/

                                if(isSgFunctionDeclaration(node) && isSgMemberFunctionDeclaration(node)==NULL){
                                        if(isSgFunctionDeclaration(node)->get_scope()!=NULL){
                                             if(isSgFunctionDeclaration(node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgFunctionDeclaration*)node)->get_symbol_from_symbol_table();
                                                        ClassicVisitor visitor((SgFunctionSymbol *)symbol);
                                                        traverseMemoryPoolVisitorPattern(visitor);
                                                        if(visitor.get_num_Function_pointers()==1){ //only one reference to this FunctionSymbol => safe to delete
                                                                ((SgFunctionDeclaration*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                delete symbol;
                                                                //printf("A SgFunctionSymbol was deleted\n");
                                                        }
                                                        ClassicVisitor visitor1((SgFunctionDeclaration *)node);
                                                        traverseMemoryPoolVisitorPattern(visitor1);
                                                }
                                        }
                                }


                                if(isSgFunctionRefExp(node) !=NULL)
                                {
#if 0
                                   SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(node);
                                   ROSE_ASSERT(functionRefExp->get_symbol_i() != NULL);
                                   printf ("In DeleteAST::visit(): functionRefExp->get_symbol_i() = %p = %s \n",functionRefExp->get_symbol_i(),functionRefExp->get_symbol_i()->class_name().c_str());
#endif
                                                SgFunctionSymbol *symbol = ((SgFunctionRefExp*)node)->get_symbol_i();
                                                ClassicVisitor visitor(symbol);
                                                traverseMemoryPoolVisitorPattern(visitor);
                                                if(visitor.get_num_Function_pointers()==1)
                                                {
                                               // only one reference to this FunctionSymbol => safe to delete
                                                        //((SgSymbol*)symbol)->get_scope()->get_symbol_table()->remove(symbol);
                                                        delete symbol;
                                                        //printf("A SgFunctionSymbol was deleted\n");
                                                }

                                }

                                if(isSgUserDefinedBinaryOp(node) !=NULL){
                                        SgFunctionSymbol *symbol = ((SgUserDefinedBinaryOp*)node)->get_symbol();
                                        ClassicVisitor visitor(symbol);
                                        traverseMemoryPoolVisitorPattern(visitor);
                                        if(visitor.get_num_Function_pointers()==1){ //only one reference to this FunctionSymbol => safe to delete
                                                ((SgSymbol*)symbol)->get_scope()->get_symbol_table()->remove(symbol);
                                                delete symbol;
                                                //printf("A SgFunctionSymbol was deleted\n");
                                        }
                                }

                                /*////////////////////////////////////////////////
                                /remove SgTypedefSymbol
                                /////////////////////////////////////////////////*/

                                if(isSgTypedefDeclaration(node) !=NULL){
                                        if(((SgTypedefDeclaration*)node)->get_scope()!=NULL){
                                             if(((SgTypedefDeclaration*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgTypedefDeclaration*)node)->get_symbol_from_symbol_table();
                                                        if(isSgTypedefSymbol(symbol)){
                                                                ClassicVisitor visitor((SgTypedefSymbol*) symbol);
                                                                traverseMemoryPoolVisitorPattern(visitor);
                                                                if(visitor.get_num_Typedef_pointers()==1){ //only one reference to this SgTypedefSymbol  => safe to delete
                                                                        ((SgTypedefDeclaration*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                        delete symbol;
                                                                        //printf("A SgTypedefSymbol was deleted\n");
                                                                }
                                                        }
                                                }
                                        }

                                        if(node == isSgTypedefDeclaration(node)->get_definingDeclaration()){
                                                ClassicVisitor visitor1((SgTypedefDeclaration*) node);
                                                traverseMemoryPoolVisitorPattern(visitor1);
                                        }
                                }

                                /*////////////////////////////////////////////////
                                /remove SgNamespaceDeclarationSymbol
                                /////////////////////////////////////////////////*/

                                if(isSgNamespaceDeclarationStatement(node) !=NULL){
                                        if(((SgNamespaceDeclarationStatement*)node)->get_scope()!=NULL){
                                             if(((SgNamespaceDeclarationStatement*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgNamespaceDeclarationStatement*)node)->get_symbol_from_symbol_table();
                                                        if(isSgNamespaceSymbol(symbol)){
                                                                ((SgNamespaceDeclarationStatement*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                delete symbol;
                                                                //printf("A SgNamespaceSymbol was deleted\n");
                                                        }
                                                }
                                        }
                                }


                                if(isSgNamespaceAliasDeclarationStatement(node) !=NULL){
                                        if(((SgNamespaceAliasDeclarationStatement*)node)->get_scope()!=NULL){
                                             if(((SgNamespaceAliasDeclarationStatement*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgNamespaceAliasDeclarationStatement*)node)->get_symbol_from_symbol_table();
                                                        if(isSgNamespaceSymbol(symbol)){
                                                                ((SgNamespaceAliasDeclarationStatement*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                delete symbol;
                                                                //printf("A SgNamespaceSymbol was deleted\n");
                                                        }
                                                }
                                        }
                                }


                                /*////////////////////////////////////////////////
                                /remove SgLabelSymbol
                                /////////////////////////////////////////////////*/

                                if(isSgLabelStatement(node) !=NULL){
                                        if(((SgLabelStatement*)node)->get_scope()!=NULL){
                                             if(((SgLabelStatement*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgLabelStatement*)node)->get_symbol_from_symbol_table();
                                                        if(isSgLabelSymbol(symbol)){
                                                                ((SgLabelStatement*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                delete symbol;
                                                                //printf("A SgLabelSymbol was deleted\n");
                                                        }
                                                }
                                        }
                                }

                                if(isSgLabelRefExp(node) !=NULL){
                                        SgLabelSymbol* symbol = ((SgLabelRefExp*)node)->get_symbol();
                                        ((SgSymbol*)symbol)->get_scope()->get_symbol_table()->remove(symbol);
                                        delete symbol;
                                        //printf("A SgLabelSymbol was deleted\n");
                                }


                                /*////////////////////////////////////////////////
                                /remove SgEnumSymbol
                                /////////////////////////////////////////////////*/

                                if(isSgEnumDeclaration(node) !=NULL){
                                        if(((SgEnumDeclaration*)node)->get_scope()!=NULL){
                                             if(((SgEnumDeclaration*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgEnumDeclaration*)node)->get_symbol_from_symbol_table();
                                                        if(isSgEnumSymbol(symbol) !=NULL){
                                                                ((SgSymbol*)symbol)->get_scope()->get_symbol_table()->remove(symbol);
                                                                delete symbol;
                                                                //printf("A SgEnumSymbol was deleted\n");
                                                        }
                                                }
                                        }
                                        SgEnumType* type= ((SgEnumDeclaration*)node)->get_type();
                                        if(type !=NULL){
                                                        delete type;
                                                        //printf("A SgEnumType was deleted\n");
                                        }
                                }


                                /*////////////////////////////////////////////////
                                /remove SgClassSymbol
                                /////////////////////////////////////////////////*/

                                if(isSgClassDeclaration(node) !=NULL && isSgTemplateInstantiationDecl(node) ==NULL){
                                        if(((SgClassDeclaration*)node)->get_scope()!=NULL){
                                             if(((SgClassDeclaration*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgClassDeclaration*)node)->get_symbol_from_symbol_table();
                                                        if(isSgClassSymbol(symbol) !=NULL){
                                                                ClassicVisitor visitor((SgClassSymbol*)symbol);
                                                                traverseMemoryPoolVisitorPattern(visitor);
                                                                if(visitor.get_num_Class_pointers()==1){ //only one reference to this symbol => safe to delete
                                                                        ((SgClassDeclaration*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                        delete symbol;
                                                                        //printf("A SgClassSymbol was deleted\n");
                                                                }
                                                        }
                                                }
                                        }

                                        ClassicVisitor visitor((SgClassDeclaration*) node );
                                        traverseMemoryPoolVisitorPattern(visitor);

                                        SgClassType* type= ((SgClassDeclaration*)node)->get_type();
                                        if(type !=NULL){
                                                delete type;
                                                //printf("A SgClassType was deleted\n");
                                        }
                                }

                                if(isSgThisExp(node) !=NULL){
                                        SgSymbol* symbol = ((SgThisExp*)node)->get_class_symbol();
                                        ClassicVisitor visitor((SgClassSymbol*)symbol);
                                        traverseMemoryPoolVisitorPattern(visitor);
                                        if(visitor.get_num_Class_pointers()==1){ //only one reference to this symbol => safe to delete
                                                ((SgSymbol*)symbol)->get_scope()->get_symbol_table()->remove(symbol);
                                                delete symbol;
                                                //printf("A SgClassSymbol was deleted\n");
                                        }

                                }

                                if(isSgClassNameRefExp(node) !=NULL){
                                        SgSymbol* symbol = ((SgClassNameRefExp*)node)->get_symbol();
                                        if(isSgClassSymbol(symbol) !=NULL)
                                        {
                                                ClassicVisitor visitor((SgClassSymbol*)symbol);
                                                traverseMemoryPoolVisitorPattern(visitor);
                                                if(visitor.get_num_Class_pointers()==1){ //only one reference to this symbol => safe to delete
                                                        ((SgSymbol*)symbol)->get_scope()->get_symbol_table()->remove(symbol);
                                                        delete symbol;
                                                        //printf("A SgClassSymbol was deleted\n");
                                                }
                                        }
                                }

                                /*////////////////////////////////////////////////
                                /remove SgMemberFunctionSymbol
                                /////////////////////////////////////////////////*/


                                if(isSgMemberFunctionDeclaration(node) !=NULL){
                                        if(((SgMemberFunctionDeclaration*)node)->get_scope()!=NULL){
                                             if(((SgMemberFunctionDeclaration*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgMemberFunctionDeclaration*)node)->get_symbol_from_symbol_table();
                                                        if(isSgMemberFunctionSymbol(symbol)){
                                                                ClassicVisitor visitor((SgMemberFunctionSymbol*)symbol);
                                                                traverseMemoryPoolVisitorPattern(visitor);
                                                                if(visitor.get_num_memFunc_pointers()==1){
                                                                        ((SgMemberFunctionDeclaration*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                        delete symbol;
                                                                        //printf("A SgMemberFunctionSymbol was deleted\n");
                                                                }
                                                        }
                                                }
                                        }
                                        ClassicVisitor visitor((SgMemberFunctionDeclaration*) node);
                                        traverseMemoryPoolVisitorPattern(visitor);

                                }
//Tan: I have no idea why the codes below cannot work. Perhaps it conflicts with some prior works
#if 0
                                if(isSgMemberFunctionRefExp(node) !=NULL){
                                        SgMemberFunctionSymbol* symbol = ((SgMemberFunctionRefExp*)node)->get_symbol_i();
                                        ClassicVisitor visitor(symbol);
                                        traverseMemoryPoolVisitorPattern(visitor);
                                        if(visitor.get_num_memFunc_pointers()==1){ //only one reference to this symbol => safe to delete
                                                ((SgSymbol*)symbol)->get_scope()->get_symbol_table()->remove(symbol);
                                                delete symbol;
                                                //printf("A SgClassSymbol was deleted\n");
                                        }

                                }

                                if(isSgFunctionType(node) !=NULL){
                                        SgSymbol* symbol = ((SgFunctionType*)node)->get_symbol_from_symbol_table();
                                        if(isSgFunctionTypeSymbol(symbol)){
                                                ((SgSymbol*)symbol)->get_scope()->get_symbol_table()->remove(symbol);
                                                delete symbol;
                                                //printf("A SgFunctionTypeSymbol was deleted\n");
                                        }
                                }
#endif

                                /*////////////////////////////////////////////////
                                /remove SgInterfaceSymbol and SgModuleSymbol
                                /////////////////////////////////////////////////*/

                                if(isSgInterfaceStatement(node) !=NULL){
                                        if(((SgDeclarationStatement*)node)->get_scope()!=NULL){
                                             if(((SgDeclarationStatement*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgDeclarationStatement*)node)->get_symbol_from_symbol_table();
                                                        if(isSgInterfaceSymbol(symbol)){
                                                                ((SgDeclarationStatement*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                delete symbol;
                                                                //printf("A SgInterfaceSymbol was deleted\n");
                                                        }
                                                }
                                        }

                                }


                                if(isSgModuleStatement(node) !=NULL){
                                        if(((SgClassDeclaration*)node)->get_scope()!=NULL){
                                             if(((SgClassDeclaration*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgClassDeclaration*)node)->get_symbol_from_symbol_table();
                                                        if(isSgModuleSymbol(symbol)){
                                                                ((SgClassDeclaration*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                delete symbol;
                                                                //printf("A SgModuleSymbol was deleted\n");
                                                        }
                                                }
                                        }

                                }


//Tan: I got stuck in deleting the SgTemplateArgument
#if 0
                                if(isSgTemplateInstantiationMemberFunctionDecl(node) !=NULL){
                                        if(((SgTemplateInstantiationMemberFunctionDecl*)node)->get_scope()!=NULL){
                                             if(((SgTemplateInstantiationMemberFunctionDecl*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgTemplateInstantiationMemberFunctionDecl*)node)->get_symbol_from_symbol_table();
                                                        if(isSgMemberFunctionSymbol(symbol)){
                                                                ClassicVisitor visitor((SgMemberFunctionSymbol*)symbol);
                                                                traverseMemoryPoolVisitorPattern(visitor);
                                                                if(visitor.get_num_memFunc_pointers()==1){
                                                                        ((SgMemberFunctionDeclaration*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                        delete symbol;
                                                                        //printf("A SgMemberFunctionSymbol was deleted\n");
                                                                }
                                                        }
                                                }
                                        }
                                        ClassicVisitor visitor((SgMemberFunctionDeclaration*) node);
                                        traverseMemoryPoolVisitorPattern(visitor);
                                }

                                if(isSgTemplateDeclaration(node) !=NULL){
                                        if(((SgTemplateDeclaration*)node)->get_scope()!=NULL){
                                             if(((SgTemplateDeclaration*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgTemplateDeclaration*)node)->get_symbol_from_symbol_table();
                                                        ClassicVisitor visitor((SgTemplateSymbol*)symbol);
                                                        traverseMemoryPoolVisitorPattern(visitor);
                                                        if(visitor.get_num_Template_pointers()==1){
                                                                        ((SgTemplateDeclaration*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                        delete symbol;
                                                                        printf("A SgTemplateSymbol was deleted\n");
                                                        }
                                                }
                                        }
                                        //if(isSgTemplateDeclaration(node) == ((SgTemplateDeclaration*)node)->get_firstNondefiningDeclaration()){
                                                ClassicVisitor visitor1((SgTemplateDeclaration*) node );
                                                traverseMemoryPoolVisitorPattern(visitor1);
                                        //}

                                }

                                if(isSgInterfaceStatement(node) !=NULL){
                                        if(((SgDeclarationStatement*)node)->get_scope()!=NULL){
                                             if(((SgDeclarationStatement*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgDeclarationStatement*)node)->get_symbol_from_symbol_table();
                                                        if(isSgInterfaceSymbol(symbol)){
                                                                ((SgDeclarationStatement*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                delete symbol;
                                                                //printf("A SgInterfaceSymbol was deleted\n");
                                                        }
                                                }
                                        }

                                }


                                if(isSgModuleStatement(node) !=NULL){
                                        if(((SgClassDeclaration*)node)->get_scope()!=NULL){
                                             if(((SgClassDeclaration*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgClassDeclaration*)node)->get_symbol_from_symbol_table();
                                                        if(isSgModuleSymbol(symbol)){
                                                                ((SgClassDeclaration*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                delete symbol;
                                                                //printf("A SgModuleSymbol was deleted\n");
                                                        }
                                                }
                                        }

                                }

                                if(isSgTemplateInstantiationDecl(node) !=NULL){
                                        if(((SgTemplateInstantiationDecl*)node)->get_scope()!=NULL){
                                             if(((SgTemplateInstantiationDecl*)node)->get_scope()->get_symbol_table()!=NULL)
                                                {
                                                        SgSymbol* symbol = ((SgTemplateInstantiationDecl*)node)->get_symbol_from_symbol_table();
                                                        if(isSgClassSymbol(symbol)){
                                                                ClassicVisitor visitor((SgClassSymbol*)symbol);
                                                                traverseMemoryPoolVisitorPattern(visitor);
                                                                if(visitor.get_num_Class_pointers()==1){
                                                                        ((SgClassDeclaration*)node)->get_scope()->get_symbol_table()->remove(symbol);
                                                                        delete symbol;
                                                                }
                                                        }
                                                }
                                        }
                                        SgClassType* type= ((SgClassDeclaration*)node)->get_type();
                                        if(type !=NULL){
                                                delete type;
                                                //printf("A SgClassType was deleted\n");
                                        }

                                        vector<SgTemplateArgument*> tempargs=  ((SgTemplateInstantiationDecl*)node)->get_templateArguments();
                                        foreach (SgTemplateArgument* element, tempargs){
                                                SgTemplateArgument* temparg = isSgTemplateArgument(element);
                                                if(temparg){
                                                        delete temparg;
                                                }
                                                printf("SgTemplateArg in normal traversal\n");
                                        }
                                        printf("SgTemplateInstantiationDecl in normal traversal\n");

                                        ClassicVisitor visitor((SgTemplateInstantiationDecl*) node);
                                        traverseMemoryPoolVisitorPattern(visitor);
                                }

#endif
#if 0
                        printf ("Deleting node = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
#endif
                     // Normal nodes  will be removed in a post-order way
                        delete node;
#if 0
                        printf ("After delete node: node = %p = %s \n",node,node->class_name().c_str());
#endif
                        }
                };


          DeleteAST deleteTree;

          // Deletion must happen in post-order to avoid traversal of (visiting) deleted IR nodes
          deleteTree.traverse(n,postorder);

#if 0
     printf ("Leaving SageInterface::deleteAST(): n = %p = %s \n",n,n->class_name().c_str());
#endif
   }




#ifndef USE_ROSE
// DQ (9/25/2011):  The deleteAST() function will not remove original expression trees behind constant folded expressions.
// These exist in the AST within the internal construction of the AST until they are simplified in the AST post-processing.
// In the post-processing either:
//    1) the constant folded values are kept and the original expression trees deleted (optional, controled by default parameter to function "frontend()", OR
//    2) the constant folded values are replaced by the original expression trees, and the constant folded values are deleted (default).
// Either way, after the AST post-processing the AST is simplified.  Until then the expression trees can contain constant
// folded values and the values will have a pointer to the original expression tree.  Before (9/16/2011) the original
// tree would also sometimes (not uniformally) be traversed as part of the AST.  This was confusing (to people and
// to numerous forms of analysis), so this is being fixed to be uniform (using either of the methods defined above).
// However, the fact that until post-processing the AST has this complexity, and that the AST traversal does not
// traverse the original expression trees (now uniform); means that we need a special delete function for subtrees
// that are not use post-processed.  This is the special purpose function that we need.
//
// NOTE: This function is called from the SgArrayType::createType() member function and in the constant folding AST post-processing.
//
void SageInterface::deleteExpressionTreeWithOriginalExpressionSubtrees(SgNode* root)
   {
     struct Visitor: public AstSimpleProcessing
        {
          virtual void visit(SgNode* n)
             {
               SgExpression* expression = isSgExpression(n);
               if (expression != NULL)
                  {
                    Visitor().traverse(expression->get_originalExpressionTree(), postorder);
                  }

               delete (n);
             }
        };

     Visitor().traverse(root, postorder);
   }
#endif


void
SageInterface::moveStatementsBetweenBlocks ( SgBasicBlock* sourceBlock, SgBasicBlock* targetBlock )
   {
  // This function moves statements from one block to another (used by the outliner).
  // printf ("***** Moving statements from sourceBlock %p to targetBlock %p ***** \n",sourceBlock,targetBlock);
    ROSE_ASSERT (sourceBlock && targetBlock);
    if (sourceBlock == targetBlock)
    {
      cerr<<"warning: SageInterface::moveStatementsBetweenBlocks() is skipped, "<<endl;
      cerr<<"         since program is trying to move statements from and to the identical basic block. "<<endl;
      return;
    }

     SgStatementPtrList & srcStmts = sourceBlock->get_statements();

//     cout<<"debug SageInterface::moveStatementsBetweenBlocks() number of stmts = "<< srcStmts.size() <<endl;
     for (SgStatementPtrList::iterator i = srcStmts.begin(); i != srcStmts.end(); i++)
        {
          // append statement to the target block
          targetBlock->append_statement(*i);

       // Make sure that the parents are set.
          ROSE_ASSERT((*i)->get_parent() == targetBlock);
          if ((*i)->hasExplicitScope())
             {
            // DQ (3/4/2009): This fails the test in ROSE/tutorial/outliner/inputCode_OutlineNonLocalJumps.cc
            // I am unclear if this is a reasonable constraint, it passes all tests but this one!
               if ((*i)->get_scope() != targetBlock)
                  {
                    if (SgFunctionDeclaration* func = isSgFunctionDeclaration(*i))
                    { // A call to a undeclared function will introduce a hidden func prototype declaration in the enclosing scope .
                      // The func declaration should be moved along with the call site.
                      // The scope should be set to the new block also
                      // Liao 1/14/2011
                      if (func->get_firstNondefiningDeclaration() == func);
                        func->set_scope(targetBlock);
                    }
                    else
                    {
                      //(*i)->set_scope(targetBlock);
                      printf ("Warning: test failing (*i)->get_scope() == targetBlock in SageInterface::moveStatementsBetweenBlocks() \n");
                      cerr<<"  "<<(*i)->class_name()<<endl;
                    }
                  }
               //ROSE_ASSERT((*i)->get_scope() == targetBlock);
             }

          SgDeclarationStatement* declaration = isSgDeclarationStatement(*i);
          if (declaration != NULL)
             {
            // Need to reset the scope from sourceBlock to targetBlock.
               switch(declaration->variantT())
                  {
                 // There will be other cases to handle, but likely not all declaration will be possible to support.

                    case V_SgVariableDeclaration:
                       {
                      // Reset the scopes on any SgInitializedName objects.
                         SgVariableDeclaration* varDecl = isSgVariableDeclaration(declaration);
                         SgInitializedNamePtrList & l = varDecl->get_variables();
                         for (SgInitializedNamePtrList::iterator i = l.begin(); i != l.end(); i++)
                            {
                           // reset the scope, but make sure it was set to sourceBlock to make sure.
                           // This might be an issue for extern variable declaration that have a scope
                           // in a separate namespace of a static class member defined external to
                           // its class, etc. I don't want to worry about those cases right now.
                              ROSE_ASSERT((*i)->get_scope() == sourceBlock);

                              (*i)->set_scope(targetBlock);
                            }
                         break;
                       }
                     case V_SgFunctionDeclaration: // Liao 1/15/2009, I don't think there is any extra things to do here
                       {
                         SgFunctionDeclaration * funcDecl = isSgFunctionDeclaration(declaration);
                         ROSE_ASSERT (funcDecl);
                         //cout<<"found a function declaration to be moved ..."<<endl;
                       }
                     break;
                     case V_SgFortranIncludeLine:
                     case V_SgAttributeSpecificationStatement:
                       break;
                    default:
                       {
                         printf ("Moving this declaration = %p = %s = %s between blocks is not yet supported \n",declaration,declaration->class_name().c_str(),get_name(declaration).c_str());
                         declaration->get_file_info()->display("file info");
                         ROSE_ASSERT(false);
                       }
                }
             } // end if
        } // end for

  // Remove the statements in the sourceBlock
     srcStmts.clear();
     ROSE_ASSERT(srcStmts.empty() == true);
     ROSE_ASSERT(sourceBlock->get_statements().empty() == true);

  // Move the symbol table
     ROSE_ASSERT(sourceBlock->get_symbol_table() != NULL);
     targetBlock->set_symbol_table(sourceBlock->get_symbol_table());

     ROSE_ASSERT(sourceBlock != NULL);
     ROSE_ASSERT(targetBlock != NULL);
     ROSE_ASSERT(targetBlock->get_symbol_table() != NULL);
     ROSE_ASSERT(sourceBlock->get_symbol_table() != NULL);
     targetBlock->get_symbol_table()->set_parent(targetBlock);

     ROSE_ASSERT(sourceBlock->get_symbol_table() != NULL);
     sourceBlock->set_symbol_table(NULL);

  // DQ (9/23/2011): Reset with a valid symbol table.
     sourceBlock->set_symbol_table(new SgSymbolTable());
     sourceBlock->get_symbol_table()->set_parent(sourceBlock);

     // Liao 2/4/2009
     // Finally , move preprocessing information attached inside the source block to the target block
     // Outliner uses this function to move a code block to the outlined function.
     // This will ensure that a trailing #endif (which is attached inside the source block) will be moved
     // to the target block to match #if (which is attached
     // before some statement moved to the target block)
     moveUpPreprocessingInfo (targetBlock, sourceBlock, PreprocessingInfo::inside);
   }


//! Variable references can be introduced by SgVarRef, SgPntrArrRefExp, SgInitializedName, SgMemberFunctionRef etc. This function will convert them all to  a top level SgInitializedName.
//TODO consult  AstInterface::IsVarRef() for more cases
SgInitializedName* SageInterface::convertRefToInitializedName(SgNode* current)
{
  SgInitializedName* name = NULL;
  SgExpression* nameExp = NULL;
  ROSE_ASSERT(current != NULL);
  if (isSgInitializedName(current))
  {
    name = isSgInitializedName(current);
  }
  else if (isSgPntrArrRefExp(current))
  {
    bool suc=false;
    suc= SageInterface::isArrayReference(isSgExpression(current),&nameExp);
    ROSE_ASSERT(suc == true);
     // has to resolve this recursively
    return convertRefToInitializedName(nameExp);
  }
  else if (isSgVarRefExp(current))
  {
    SgNode* parent = current->get_parent();
    if (isSgDotExp(parent))
    {
       if (isSgDotExp(parent)->get_rhs_operand() == current)
        return convertRefToInitializedName(parent);
    }
    else if(isSgArrowExp(parent))
    {
      if (isSgArrowExp(parent)->get_rhs_operand() == current)
        return convertRefToInitializedName(parent);
    }
    name = isSgVarRefExp(current)->get_symbol()->get_declaration();
  }
  else if (isSgDotExp(current))
  {
    SgExpression* lhs = isSgDotExp(current)->get_lhs_operand();
    ROSE_ASSERT(lhs);
     // has to resolve this recursively
    return convertRefToInitializedName(lhs);
  }
   else if (isSgArrowExp(current))
  {
    SgExpression* lhs = isSgArrowExp(current)->get_lhs_operand();
    ROSE_ASSERT(lhs);
     // has to resolve this recursively
    return convertRefToInitializedName(lhs);
  } // The following expression types are usually introduced by left hand operands of DotExp, ArrowExp
  else if (isSgPointerDerefExp(current))
  {
    return convertRefToInitializedName(isSgPointerDerefExp(current)->get_operand());
  }
  else if (isSgCastExp(current))
  {
    return convertRefToInitializedName(isSgCastExp(current)->get_operand());
  }
 else
  {
    cerr<<"In SageInterface::convertRefToInitializedName(): unhandled reference type:"<<current->class_name()<<endl;
    ROSE_ASSERT(false);
  }
  ROSE_ASSERT(name != NULL);
  return name;
}

//! Obtain a matching SgNode from an abstract handle string
SgNode* SageInterface::getSgNodeFromAbstractHandleString(const std::string& input_string)
{
#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
  printf ("AbstractHandle support is disabled for ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT \n");
  ROSE_ASSERT(false);
#else
  AbstractHandle::abstract_handle * project_handle = buildAbstractHandle(getProject());

  // trim off the possible leading handle for project: "Project<numbering,1>::"
  size_t pos = input_string.find("SourceFile<");
  ROSE_ASSERT (pos != string::npos);
  string trimed_string = input_string.substr(pos);
  AbstractHandle::abstract_handle * handle = new AbstractHandle::abstract_handle(project_handle, trimed_string);
  if (handle)
  {
    if (handle->getNode()!=NULL)
    {
#ifdef _MSC_VER
     // DQ (11/28/2009): This is related to the use of covariant return types (I think).
SgNode* result = NULL; // (SgNode*)(handle->getNode()->getNode());
#pragma message ("WARNING: covariant return type for get_node() not supported in MSVC.")
                printf ("ERROR: covariant return type for get_node() not supported in MSVC. \n");
                ROSE_ASSERT(false);
#else
                SgNode* result = (SgNode*)(handle->getNode()->getNode());
#endif
      // deallocate memory, should not do this!!
      // May corrupt the internal std maps used in abstract handle namespace
      //delete handle->getNode();
      //delete handle;
      return result;
    }
  }
#endif

  return NULL;
}


//! Dump information about a SgNode for debugging
// unparseToString() is too strict for debugging purpose
//  we provide this instead.
void SageInterface::dumpInfo(SgNode* node, std::string desc/*=""*/)
{
  ROSE_ASSERT(node != NULL);
  cout<<desc<<endl;
  // base information for all SgNode:
  cout<<"///////////// begin of SageInterface::dumpInfo() ///////////////"<<endl;
  cout<<"--------------base info. for SgNode---------------"<<endl;
  cout<<node<<" "<<node->class_name()<<endl;
  SgLocatedNode* snode = isSgLocatedNode(node);
  if (snode)
  {
    // source file info. dump
    cout<<"--------------source location info. for SgNode---------------"<<endl;
    cout<<snode->get_file_info()->get_filename()
      << ":"<<snode->get_file_info()->get_line()<<"-"
      << snode->get_file_info()->get_col()<<endl;
    // preprocessing info dump
    AttachedPreprocessingInfoType *comments = snode->getAttachedPreprocessingInfo ();
    if (comments)
    {
      cout<<"--------------preprocessing info. for SgNode---------------"<<endl;
      AttachedPreprocessingInfoType::iterator i;
      cout<<"Total attached preprocessingInfo count="<<comments->size()<<endl;
      for (i = comments->begin (); i != comments->end (); i++)
      {
        PreprocessingInfo * pinfo = *i;
        pinfo->display("");
      }
    }
    cout<<"--------------name info. for SgNode---------------"<<endl;
    // print out namea for named nodes
    SgFunctionDeclaration * decl = isSgFunctionDeclaration(snode);
    if (decl)
      cout<<"\tqualified name="<<decl->get_qualified_name().getString()<<endl;
    SgVarRefExp * varRef =  isSgVarRefExp(snode);
    if (varRef)
      cout<<"\treferenced variable name= "<<varRef->get_symbol()->get_name().getString()<<endl;
  }
  SgInitializedName * iname = isSgInitializedName(snode);
  if (iname)
    cout<<"\tvariable name= "<<iname->get_qualified_name().getString()<<endl;

  cout<<endl;
  cout<<"///////////// end of SageInterface::dumpInfo() ///////////////"<<endl;
}

//! Collect all read and write references within stmt, which can be a function, a scope statement, or a single statement. Note that a reference can be both read and written, like i++
//! This is a wrapper function to Qing's side effect analysis from loop optimization
//! Liao, 2/26/2009
bool
SageInterface::collectReadWriteRefs(SgStatement* stmt, std::vector<SgNode*>& readRefs, std::vector<SgNode*>& writeRefs, bool useCachedDefUse)
{   // The type cannot be SgExpression since variable declarations have SgInitializedName as the reference, not SgVarRefExp.
  ROSE_ASSERT(stmt !=NULL);

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
  // We should allow accumulate the effects for multiple statements
  // ROSE_ASSERT(readRefs.size() == 0);
  // ROSE_ASSERT(writeRefs.size() == 0);

  // convert a request for a defining function declaration to its function body
  SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(stmt);
  if (funcDecl != NULL)
  {
    funcDecl= isSgFunctionDeclaration(funcDecl->get_definingDeclaration ());
    if (funcDecl == NULL)
    {
      cerr<<"In collectReadWriteRefs(): cannot proceed without a function body!"<<endl;
    }
    stmt = funcDecl->get_definition()->get_body();
  }

  // get function level information
  SgFunctionDefinition* funcDef = SageInterface::getEnclosingFunctionDefinition(stmt);
  ROSE_ASSERT(funcDef != NULL);
  SgBasicBlock* funcBody = funcDef->get_body();
  ROSE_ASSERT(funcBody!= NULL);

  // prepare Loop transformation environment
  AstInterfaceImpl faImpl(funcBody);
  AstInterface fa(&faImpl);
  ArrayAnnotation* annot = ArrayAnnotation::get_inst();
  if( useCachedDefUse ){
    ArrayInterface* array_interface = ArrayInterface::get_inst(*annot, fa, funcDef, AstNodePtrImpl(funcDef));
    LoopTransformInterface::set_arrayInfo(array_interface);
  } else {
    ArrayInterface array_interface(*annot);
    array_interface.initialize(fa, AstNodePtrImpl(funcDef));
    array_interface.observe(fa);
    LoopTransformInterface::set_arrayInfo(&array_interface);
  }
  LoopTransformInterface::set_astInterface(fa);

  // variables to store results
  DoublyLinkedListWrap<AstNodePtr> rRef1, wRef1;
  CollectDoublyLinkedList<AstNodePtr> crRef1(rRef1),cwRef1(wRef1);
  AstNodePtr s1 = AstNodePtrImpl(stmt);

  // Actual side effect analysis
  if (!AnalyzeStmtRefs(fa, s1, cwRef1, crRef1))
  {
//    cerr<<"error in side effect analysis!"<<endl;
    return false;
  }

  // transfer results into STL containers.
  for (DoublyLinkedEntryWrap<AstNodePtr>* p = rRef1.First(); p != 0; )
  {
    DoublyLinkedEntryWrap<AstNodePtr>* p1 = p;
    p = rRef1.Next(p);
    AstNodePtr cur = p1->GetEntry();
    SgNode* sgRef = AstNodePtrImpl(cur).get_ptr();
   ROSE_ASSERT(sgRef != NULL);
    readRefs.push_back(sgRef);
    //cout<<"read reference:"<<sgRef->unparseToString()<<" address "<<sgRef<<
    //    " sage type:"<< sgRef->class_name()<< endl;
  }

  for (DoublyLinkedEntryWrap<AstNodePtr>* p = wRef1.First(); p != 0; )
  {
    DoublyLinkedEntryWrap<AstNodePtr>* p1 = p;
    p = wRef1.Next(p);
    AstNodePtr cur = p1->GetEntry();
    SgNode* sgRef = AstNodePtrImpl(cur).get_ptr();
    ROSE_ASSERT(sgRef != NULL);
    writeRefs.push_back(sgRef);
  //  cout<<"write reference:"<<sgRef->unparseToString()<<" address "<<sgRef<<
  //      " sage type:"<< sgRef->class_name()<< endl;
  }

#endif

  return true;
}

//!Collect unique variables which are read or written within a statement. Note that a variable can be both read and written. The statement can be either of a function, a scope, or a single line statement.
bool SageInterface::collectReadWriteVariables(SgStatement* stmt, set<SgInitializedName*>& readVars, set<SgInitializedName*>& writeVars)
{
  ROSE_ASSERT(stmt != NULL);
  vector <SgNode* > readRefs, writeRefs;
  if (!collectReadWriteRefs(stmt, readRefs, writeRefs))
  {
    return false;
  }
  // process read references
  vector<SgNode*>::iterator iter = readRefs.begin();
  for (; iter!=readRefs.end();iter++)
  {
    SgNode* current = *iter;
    SgInitializedName* name = convertRefToInitializedName(current);
   // Only insert unique ones
#if 0   //
    vector <SgInitializedName*>::iterator iter2 = find (readVars.begin(), readVars.end(), name);
    if (iter2==readVars.end())
    {
      readVars.push_back(name);
    //  cout<<"inserting read SgInitializedName:"<<name->unparseToString()<<endl;
    }
#else
    // We use std::set to ensure uniqueness now
    readVars.insert(name);
#endif
  }
  // process write references
  vector<SgNode*>::iterator iterw = writeRefs.begin();
  for (; iterw!=writeRefs.end();iterw++)
  {
    SgNode* current = *iterw;
    SgInitializedName* name = convertRefToInitializedName(current);
   // Only insert unique ones
#if 0   //
    vector <SgInitializedName*>::iterator iter2 = find (writeVars.begin(), writeVars.end(), name);
    if (iter2==writeVars.end())
    {
      writeVars.push_back(name);
     // cout<<"inserting written SgInitializedName:"<<name->unparseToString()<<endl;
    }
#else
    // We use std::set to ensure uniqueness now
    writeVars.insert(name);
#endif
  }
  return true;
}

//!Collect read only variables within a statement. The statement can be either of a function, a scope, or a single line statement.
void SageInterface::collectReadOnlyVariables(SgStatement* stmt, std::set<SgInitializedName*>& readOnlyVars)
{
  ROSE_ASSERT(stmt != NULL);
  set<SgInitializedName*> readVars, writeVars;
   // Only collect read only variables if collectReadWriteVariables() succeeded.
  if (collectReadWriteVariables(stmt, readVars, writeVars))
  {
    // read only = read - write
    set_difference(readVars.begin(), readVars.end(),
        writeVars.begin(), writeVars.end(),
        std::inserter(readOnlyVars, readOnlyVars.begin()));
  }
}


//!Collect read only variable symbols within a statement. The statement can be either of a function, a scope, or a single line statement.
void SageInterface::collectReadOnlySymbols(SgStatement* stmt, std::set<SgVariableSymbol*>& readOnlySymbols)
{
  set<SgInitializedName*> temp;
  collectReadOnlyVariables(stmt, temp);

  for (set<SgInitializedName*>::const_iterator iter = temp.begin();
      iter!=temp.end(); iter++)
  {
    SgSymbol* symbol = (*iter)->get_symbol_from_symbol_table () ;
    ROSE_ASSERT(symbol != NULL );
    ROSE_ASSERT(isSgVariableSymbol(symbol));
    readOnlySymbols.insert(isSgVariableSymbol(symbol));
  }

}

//! Check if a variable reference is used by its address: including &a expression and foo(a) when type2 foo(Type& parameter) in C++
bool SageInterface::isUseByAddressVariableRef(SgVarRefExp* ref)
{
  bool result = false;
  ROSE_ASSERT(ref != NULL);
  ROSE_ASSERT(ref->get_parent() != NULL);
  // case 1: ref is used as an operator for & (SgAddressofOp)
  // TODO tolerate possible type casting operations in between ?
  if (isSgAddressOfOp(ref->get_parent()))
  {
    result = true;
  }
  // case 2. ref is used as a function call's parameter, and the parameter has reference type in C++
  else if ((SageInterface::is_Cxx_language())&&(isSgExprListExp(ref->get_parent())))
  {
    SgNode* grandparent = ref->get_parent()->get_parent();
    ROSE_ASSERT(grandparent);
    if (isSgFunctionCallExp(grandparent)) // Is used as a function call's parameter
    {
      // find which parameter ref is in SgExpressionPtrList
      int param_index =0;
      SgExpressionPtrList expList = isSgExprListExp(ref->get_parent())->get_expressions();
      Rose_STL_Container<SgExpression*>::const_iterator iter= expList.begin();
      for (; iter!=expList.end(); iter++)
      {
        if (*iter == ref)
          break;
        else
          param_index++;
      }
      // find the parameter type of the corresponding function declaration
      SgExpression* func_exp = isSgFunctionCallExp(grandparent)->get_function();
      ROSE_ASSERT (func_exp);
      SgFunctionRefExp * funcRef = isSgFunctionRefExp(func_exp);
      if (funcRef) // regular functions
      {
        SgFunctionDeclaration* funcDecl = isSgFunctionSymbol(funcRef->get_symbol())->get_declaration();
        SgInitializedNamePtrList nameList = funcDecl->get_args();
        //TODO tolerate typedef chains
        if (isSgReferenceType(nameList[param_index]->get_type()))
        {
          result = true;
        }
      } //It also could be a dot or arrow expression where its right hand operand is a SgMemberFunctionRefExp
      else if (isSgDotExp (func_exp) || isSgArrowExp(func_exp)) // ArrowExp TODO
      {
        SgBinaryOp* binOp = isSgBinaryOp(func_exp);
        ROSE_ASSERT (binOp);
        SgMemberFunctionRefExp* mfuncRef = isSgMemberFunctionRefExp(binOp->get_rhs_operand_i());
        ROSE_ASSERT (mfuncRef);
        SgMemberFunctionDeclaration * mfuncDecl = isSgMemberFunctionSymbol (mfuncRef->get_symbol())->get_declaration();
        ROSE_ASSERT (mfuncDecl);
        SgInitializedNamePtrList nameList = mfuncDecl->get_args();
        //TODO tolerate typedef chains
        if (isSgReferenceType(nameList[param_index]->get_type()))
        {
          result = true;
        }
      }

    }
  }
  return result;
}


//! Collect variable references involving use by address: including &a expression and foo(a) when foo(Type& parameter) in C++
void SageInterface::collectUseByAddressVariableRefs (const SgStatement* s, std::set<SgVarRefExp* >& varSetB)
{
  Rose_STL_Container <SgNode*> var_refs = NodeQuery::querySubTree (const_cast<SgStatement *> (s), V_SgVarRefExp);

  Rose_STL_Container<SgNode*>::iterator iter = var_refs.begin();
  for (; iter!=var_refs.end(); iter++)
  {
    SgVarRefExp* ref = isSgVarRefExp(*iter);
    ROSE_ASSERT(ref != NULL);
    ROSE_ASSERT(ref->get_parent() != NULL);
#if 0
    // case 1: ref is used as an operator for & (SgAddressofOp)
    // TODO tolerate possible type casting operations in between ?
    if (isSgAddressOfOp(ref->get_parent()))
    {
      varSetB.insert(ref);
    }
    // case 2. ref is used as a function call's parameter, and the parameter has reference type in C++
    else if ((SageInterface::is_Cxx_language())&&(isSgExprListExp(ref->get_parent())))
    {
      SgNode* grandparent = ref->get_parent()->get_parent();
      ROSE_ASSERT(grandparent);
      if (isSgFunctionCallExp(grandparent)) // Is used as a function call's parameter
      {
        // find which parameter ref is in SgExpressionPtrList
        int param_index =0;
        SgExpressionPtrList expList = isSgExprListExp(ref->get_parent())->get_expressions();
        Rose_STL_Container<SgExpression*>::const_iterator iter= expList.begin();
        for (; iter!=expList.end(); iter++)
        {
          if (*iter == ref)
            break;
          else
            param_index++;
        }
        // find the parameter type of the corresponding function declaration
        SgFunctionRefExp * funcRef = isSgFunctionRefExp(isSgFunctionCallExp(grandparent)->get_function());
        SgFunctionDeclaration* funcDecl = isSgFunctionSymbol(funcRef->get_symbol())->get_declaration();
        SgInitializedNamePtrList nameList = funcDecl->get_args();
        //TODO tolerate typedef chains
        if (isSgReferenceType(nameList[param_index]->get_type()))
        {
          varSetB.insert(ref);
        }
      }
    }
#endif
    if (isUseByAddressVariableRef(ref))
      varSetB.insert(ref);
  }
}

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
//!Call liveness analysis on an entire project
LivenessAnalysis * SageInterface::call_liveness_analysis(SgProject* project, bool debug/*=false*/)
{
  // We expect this function to be called multiple times but always return the result generated from the first execution.
  static LivenessAnalysis* liv = NULL; // one instance
  if (liv!=NULL)
    return liv;

  static DFAnalysis * defuse = NULL; // only one instance
  // Prepare def-use analysis
  if (defuse==NULL)
  {
    ROSE_ASSERT(project != NULL);
    defuse = new DefUseAnalysis(project);
  }

  ROSE_ASSERT(defuse != NULL);
  defuse->run(debug);

  if (debug)
    defuse->dfaToDOT();

  //Prepare variable liveness analysis
  liv = new LivenessAnalysis(debug,(DefUseAnalysis*)defuse);
  ROSE_ASSERT(liv != NULL);

  std::vector <FilteredCFGNode < IsDFAFilter > > dfaFunctions;
  NodeQuerySynthesizedAttributeType vars =
          NodeQuery::querySubTree(project, V_SgFunctionDefinition);
  NodeQuerySynthesizedAttributeType::const_iterator i;
  bool abortme=false;
     // run liveness analysis on each function body
  for (i= vars.begin(); i!=vars.end();++i)
  {
    SgFunctionDefinition* func = isSgFunctionDefinition(*i);
    if (debug)
    {
      std::string name = func->class_name();
      string funcName = func->get_declaration()->get_qualified_name().str();
      cout<< " .. running liveness analysis for function: " << funcName << endl;
    }
    FilteredCFGNode <IsDFAFilter> rem_source = liv->run(func,abortme);
    // propagate results to statement level
    liv->fixupStatementsINOUT(func);
    if (rem_source.getNode()!=NULL)
      dfaFunctions.push_back(rem_source);
    if (abortme)
      break;
  } // end for ()
  if(debug)
  {
    cout << "Writing out liveness analysis results into var.dot... " << endl;
    std::ofstream f2("var.dot");
    dfaToDot(f2, string("var"), dfaFunctions, (DefUseAnalysis*)defuse, liv);
    f2.close();
  }
  if (abortme) {
    cerr<<"Error: Liveness analysis is ABORTING ." << endl;
    ROSE_ASSERT(false);
  }
  return liv;
  //return !abortme;
}
#endif

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
//!Get liveIn and liveOut variables for a for loop
void SageInterface::getLiveVariables(LivenessAnalysis * liv, SgForStatement* loop, std::set<SgInitializedName*>& liveIns, std::set<SgInitializedName*> & liveOuts)
{
  ROSE_ASSERT(liv != NULL);
  ROSE_ASSERT(loop != NULL);
  SgForStatement *forstmt = loop;
  std::vector<SgInitializedName*> liveIns0, liveOuts0; // store the original one

  // Jeremiah's hidden constructor parameter value '2' to grab the node for forStmt's
  // Several CFG nodes are used for the same SgForStatement, only one of the is needed.
  // We have to check the full control flow graph to find all SgForStatement's nodes,
  // check the index numbers from 0 , find the one with two out edges (true, false)
  // The CFG node should have a caption like" <SgForStatement> @ 8: 1",
  // which means this is a CFG node for a for statement at source line 8, with an index 1.
  // For SgForStatement, there are 5 cfg nodes, 0 and 4 are for begin and end CFG nodes
  // 1: after init statement, 2: after test expression (the remaining one after filtering), 3: before increment
  CFGNode cfgnode(forstmt,2);
  FilteredCFGNode<IsDFAFilter> filternode= FilteredCFGNode<IsDFAFilter> (cfgnode);
  // This one does not return the one we want even its getNode returns the
  // right for statement
  //FilteredCFGNode<IsDFAFilter> filternode= FilteredCFGNode<IsDFAFilter> (forstmt->cfgForBeginning());
  ROSE_ASSERT(filternode.getNode()==forstmt);

  // Check out edges
  vector<FilteredCFGEdge < IsDFAFilter > > out_edges = filternode.outEdges();
  ROSE_ASSERT(out_edges.size()==2);
  vector<FilteredCFGEdge < IsDFAFilter > >::iterator iter= out_edges.begin();

  for (; iter!=out_edges.end();iter++)
  {
    FilteredCFGEdge < IsDFAFilter > edge= *iter;
    //SgForStatement should have two outgoing edges based on the loop condition
    // one true(going into the loop body) and one false (going out the loop)
    //x. Live-in (loop) = live-in (first-stmt-in-loop)
    if (edge.condition()==eckTrue)
    {
      SgNode* firstnode= edge.target().getNode();
      liveIns0 = liv->getIn(firstnode);
      // cout<<"Live-in variables for loop:"<<endl;
      for (std::vector<SgInitializedName*>::iterator iter = liveIns0.begin();
          iter!=liveIns0.end(); iter++)
      {
        // SgInitializedName* name = *iter;
        liveIns.insert(*iter);
        //           cout<< name->get_qualified_name().getString()<<endl;
      }
    }
    //x. live-out(loop) = live-in (first-stmt-after-loop)
    else if (edge.condition()==eckFalse)
    {
      SgNode* firstnode= edge.target().getNode();
      liveOuts0 = liv->getIn(firstnode);
      //  cout<<"Live-out variables for loop:"<<endl;
      for (std::vector<SgInitializedName*>::iterator iter = liveOuts0.begin();
          iter!=liveOuts0.end(); iter++)
      {
        // SgInitializedName* name = *iter;
        //  cout<< name->get_qualified_name().getString()<<endl;
        liveOuts.insert(*iter);
      }
    }
    else
    {
      cerr<<"Unexpected CFG out edge type for SgForStmt!"<<endl;
      ROSE_ASSERT(false);
    }
  } // end for (edges)

}
#endif

//!Recognize and collect reduction variables and operations within a C/C++ loop, following OpenMP 3.0 specification for allowed reduction variable types and operation types.
/* This code is refactored from project/autoParallelization/autoParSupport.C
  std::vector<SgInitializedName*>
  RecognizeReduction(SgNode *loop, OmpSupport::OmpAttribute* attribute, std::vector<SgInitializedName*>& candidateVars/)
  * Algorithms:
   *   for each scalar candidate which are both live-in and live-out for the loop body
   *   (We don't use liveness analysis here for simplicity)
   *    and which is not the loop invariant variable (loop index).
   *   Consider those with only 1 or 2 references
   *   1 reference
   *     the operation is one of x++, ++x, x--, --x, x binop= expr
   *   2 references belonging to the same operation
   *     operations: one of  x= x op expr,  x = expr op x (except for subtraction)
   * The reduction description from the OpenMP 3.0 specification.
   *  x is not referenced in exp
   *  expr has scalar type (no array, objects etc)
   *  x: scalar only, aggregate types (including arrays), pointer types and reference types may not appear in a reduction clause.
   *  op is not an overloaded operator, but +, *, -, &, ^ ,|, &&, ||
   *  binop is not an overloaded operator, but: +, *, -, &, ^ ,|
  */
void SageInterface::ReductionRecognition(SgForStatement* loop, std::set< std::pair <SgInitializedName*, VariantT> > & results)
{
  //x. Collect variable references of scalar types as candidates, excluding loop index
  SgInitializedName* loopindex;
  if (!(isCanonicalForLoop(loop, &loopindex)))
  {
    cerr<<"Skip reduction recognition for non-canonical for loop"<<endl;
    return;
  }
  std::set<SgInitializedName*> candidateVars; // scalar variables used within the loop
  //Store the references for each scalar variable
  std::map <SgInitializedName*, vector<SgVarRefExp* > > var_references;

  Rose_STL_Container<SgNode*> reflist = NodeQuery::querySubTree(loop, V_SgVarRefExp);
  SgStatement* lbody= loop->get_loop_body();
  ROSE_ASSERT(lbody != NULL);
  Rose_STL_Container<SgNode*>::iterator iter = reflist.begin();
  for (; iter!=reflist.end(); iter++)
  {
    SgVarRefExp* ref_exp = isSgVarRefExp(*iter);
    SgInitializedName* initname= ref_exp->get_symbol()->get_declaration();
    // candidates are of scalar types and are not the loop index variable
    // And also should be live-in:
    //        not declared locally (var_scope equal or lower than loop body )
    //        or redefined (captured by ref count)
    SgScopeStatement* var_scope = initname->get_scope();
    ROSE_ASSERT(var_scope != NULL);
    if ((isScalarType(initname->get_type())) &&(initname !=loopindex)
        && !(SageInterface::isAncestor(lbody, var_scope)||(lbody==var_scope)))
    {
      candidateVars.insert(initname);
      var_references[initname].push_back(ref_exp);
    }
  }
  //
  //Consider variables referenced at most twice
  std::set<SgInitializedName*>::iterator niter=candidateVars.begin();
  for (; niter!=candidateVars.end(); niter++)
  {
    SgInitializedName* initname = *niter;
    bool isReduction = false;
    VariantT optype;
    // referenced once only
    if (var_references[initname].size()==1)
    {
      if(getProject()->get_verbose()>1)
        cout<<"Debug: SageInterface::ReductionRecognition() A candidate used once:"<<initname->get_name().getString()<<endl;
      SgVarRefExp* ref_exp = *(var_references[initname].begin());
      SgStatement* stmt = SageInterface::getEnclosingStatement(ref_exp);
      if (isSgExprStatement(stmt))
      {
        SgExpression* exp = isSgExprStatement(stmt)->get_expression();
        SgExpression* binop = isSgBinaryOp(exp);
        if (isSgPlusPlusOp(exp)) // x++ or ++x
        { // Could have multiple reduction clause with different operators!!
          // So the variable list is associated with each kind of operator
          optype = V_SgPlusPlusOp;
          isReduction = true;
        }
        else if (isSgMinusMinusOp(exp)) // x-- or --x
        {
          optype = V_SgMinusMinusOp;
          isReduction = true;
        }
        else
        // x binop= expr where binop is one of + * - & ^ |
        // x must be on the left hand side
        if (binop!=NULL) {
          SgExpression* lhs= isSgBinaryOp(exp)->get_lhs_operand ();
          if (lhs==ref_exp)
          {
            switch (exp->variantT())
            {
              case V_SgPlusAssignOp:
                {
                  optype = V_SgPlusAssignOp;
                  isReduction = true;
                  break;
                }
              case V_SgMultAssignOp:
                {
                  optype = V_SgMultAssignOp;
                  isReduction = true;
                  break;
                }
              case V_SgMinusAssignOp:
                {
                  optype = V_SgMinusAssignOp;
                  isReduction = true;
                  break;
                }
              case V_SgAndAssignOp:
                {
                  optype = V_SgAndAssignOp;
                  isReduction = true;
                  break;
                }
              case V_SgXorAssignOp:
                {
                  optype = V_SgXorAssignOp;
                  isReduction = true;
                  break;
                }
              case V_SgIorAssignOp:
                {
                  optype = V_SgIorAssignOp;
                  isReduction = true;
                  break;
                }
              default:
                break;
            } // end
          }// end if on left side
        }
      }
    }
    // referenced twice within a same statement
    else if (var_references[initname].size()==2)
    {
      if(getProject()->get_verbose()>1)
        cout<<"Debug: A candidate used twice:"<<initname->get_name().getString()<<endl;
      SgVarRefExp* ref_exp1 = *(var_references[initname].begin());
      SgVarRefExp* ref_exp2 = *(++var_references[initname].begin());
      SgStatement* stmt = SageInterface::getEnclosingStatement(ref_exp1);
      SgStatement* stmt2 = SageInterface::getEnclosingStatement(ref_exp2);
      if (stmt != stmt2)
        continue;
      // must be assignment statement using
      //  x= x op expr,  x = expr op x (except for subtraction)
      // one reference on left hand, the other on the right hand of assignment expression
      // the right hand uses associative operators +, *, -, &, ^ ,|, &&, ||
      SgExprStatement* exp_stmt =  isSgExprStatement(stmt);
      if (exp_stmt && isSgAssignOp(exp_stmt->get_expression()))
      {
        SgExpression* assign_lhs=NULL, * assign_rhs =NULL;
        assign_lhs = isSgAssignOp(exp_stmt->get_expression())->get_lhs_operand();
        assign_rhs = isSgAssignOp(exp_stmt->get_expression())->get_rhs_operand();
        ROSE_ASSERT(assign_lhs && assign_rhs);
        // x must show up in both lhs and rhs in any order:
        //  e.g.: ref1 = ref2 op exp or ref2 = ref1 op exp
        if (((assign_lhs==ref_exp1)&&SageInterface::isAncestor(assign_rhs,ref_exp2))
            ||((assign_lhs==ref_exp2)&&SageInterface::isAncestor(assign_rhs,ref_exp1)))
        {
          // assignment's rhs must match the associative binary operations
          // +, *, -, &, ^ ,|, &&, ||
          SgBinaryOp * binop = isSgBinaryOp(assign_rhs);
          if (binop!=NULL){
            SgExpression* op_lhs = binop->get_lhs_operand();
            SgExpression* op_rhs = binop->get_rhs_operand();
            // double check that the binary expression has either ref1 or ref2 as one operand
            if( !((op_lhs==ref_exp1)||(op_lhs==ref_exp2))
                && !((op_rhs==ref_exp1)||(op_rhs==ref_exp2)))
              continue;
            bool isOnLeft = false; // true if it has form (refx op exp), instead (exp or refx)
            if ((op_lhs==ref_exp1)||   // TODO might have in between !!
                (op_lhs==ref_exp2))
              isOnLeft = true;
            switch (binop->variantT())
            {
              case V_SgAddOp:
                {
                  optype = V_SgAddOp;
                  isReduction = true;
                  break;
                }
              case V_SgMultiplyOp:
                {
                  optype = V_SgMultiplyOp;
                  isReduction = true;
                  break;
                }
              case V_SgSubtractOp: // special handle here!!
                {
                  optype = V_SgSubtractOp;
                  if (isOnLeft) // cannot allow (exp - x)a
                  {
                    isReduction = true;
                  }
                  break;
                }
              case V_SgBitAndOp:
                {
                  optype = V_SgBitAndOp;
                  isReduction = true;
                  break;
                }
              case V_SgBitXorOp:
                {
                  optype = V_SgBitXorOp;
                  isReduction = true;
                  break;
                }
              case V_SgBitOrOp:
                {
                  optype = V_SgBitOrOp;
                  isReduction = true;
                  break;
                }
              case V_SgAndOp:
                {
                  optype = V_SgAndOp;
                  isReduction = true;
                  break;
                }
              case V_SgOrOp:
                {
                  optype = V_SgOrOp;
                  isReduction = true;
                  break;
                }
              default:
                break;
            }
          } // end matching associative operations
        }
      } // end if assignop
    }// end referenced twice
    if (isReduction)
      results.insert(make_pair(initname,optype));
  }// end for ()
}

//! a wrapper for ConstantFolding::constantFoldingOptimization ()
void SageInterface::constantFolding(SgNode* r)
{
  ROSE_ASSERT(r!=NULL);
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
  ConstantFolding::constantFoldingOptimization(r,false);
#endif
}

//! Generate unique names for expressions and attach the names as persistent attributes
void SageInterface::annotateExpressionsWithUniqueNames (SgProject* project)
{
  class visitorTraversal:public AstSimpleProcessing
  {
    public:
      virtual void visit (SgNode * n)
      {
        SgExpression* exp = isSgExpression(n);
        if (exp)
        {
          string u_name = generateUniqueName(exp,false)+"-"+exp->class_name();
          AstAttribute * name_attribute = new UniqueNameAttribute(u_name);
          ROSE_ASSERT (name_attribute != NULL);
          exp->addNewAttribute("UniqueNameAttribute",name_attribute);
        }
      }
  };
  visitorTraversal exampleTraversal;
  //Sriram FIX: should traverse using the traverse function
  // exampleTraversal.traverseInputFiles(project,preorder);
  exampleTraversal.traverse(project, preorder);
}

#endif


  //
  // wrap free-standing function body in another function

  SgInitializedName& SageInterface::getFirstVariable(SgVariableDeclaration& vardecl)
  {
    ROSE_ASSERT(vardecl.get_variables().size());

    return *vardecl.get_variables().front();
  }

  /// \brief  clones a function parameter list @params and uses the function
  ///         definition @fundef as new scope
  /// \return a copy of a function parameter list
  /// \param  params the original list
  /// \param  fundef the function definition with which the new parameter list
  ///         will be associated (indirectly through the function declaration).
  ///         fundef can be NULL.
  static
  SgFunctionParameterList&
  cloneParameterList(const SgFunctionParameterList& params, SgFunctionDefinition* fundef = NULL)
  {
    namespace SB = SageBuilder;

    SgFunctionParameterList&          copy = *SB::buildFunctionParameterList();
    const SgInitializedNamePtrList&   orig_decls = params.get_args();

    std::transform( orig_decls.begin(), orig_decls.end(), sg::sage_inserter(copy), sg::InitNameCloner(copy, fundef) );

    return copy;
  }

  /// \brief swaps the "defining elements" of two function declarations
  static
  void swapDefiningElements(SgFunctionDeclaration& ll, SgFunctionDeclaration& rr)
  {
    // swap definitions
    sg::swap_child(ll, rr, &SgFunctionDeclaration::get_definition,    &SgFunctionDeclaration::set_definition);
    sg::swap_child(ll, rr, &SgFunctionDeclaration::get_parameterList, &SgFunctionDeclaration::set_parameterList);

    // \todo do we need to swap also exception spec, decorator_list, etc. ?
  }

  std::pair<SgStatement*, SgInitializedName*>
  SageInterface::wrapFunction(SgFunctionDeclaration& definingDeclaration, SgName newName)
  {
    namespace SB = SageBuilder;

    // handles freestanding functions only
    ROSE_ASSERT(typeid(SgFunctionDeclaration) == typeid(definingDeclaration));
    ROSE_ASSERT(definingDeclaration.get_definingDeclaration() == &definingDeclaration);

    // clone function parameter list
    SgFunctionParameterList&  param_list = cloneParameterList(*definingDeclaration.get_parameterList());

    // create new function definition/declaration in the same scope
    SgScopeStatement*         containing_scope = definingDeclaration.get_scope();
    SgType*                   result_type = definingDeclaration.get_type()->get_return_type();
    SgExprListExp*            decorators = deepCopy( definingDeclaration.get_decoratorList() );
    SgFunctionDeclaration*    wrapperfn = SB::buildDefiningFunctionDeclaration(newName, result_type, &param_list, containing_scope, decorators);
    SgFunctionDefinition*     wrapperdef = wrapperfn->get_definition();
    ROSE_ASSERT(wrapperdef);

    // copy the exception specification
    wrapperfn->set_exceptionSpecification(definingDeclaration.get_exceptionSpecification());

    // swap the original's function definition w/ the clone's function def
    //  and the original's func parameter list w/ the clone's parameters
    swapDefiningElements(definingDeclaration, *wrapperfn);

    // call original function from within the defining decl's body
    SgBasicBlock*             body = wrapperdef->get_body();
    SgExprListExp*            args = SB::buildExprListExp();
    SgInitializedNamePtrList& param_decls = param_list.get_args();

    std::transform( param_decls.begin(), param_decls.end(), sg::sage_inserter(*args), sg::VarRefBuilder(*wrapperdef) );

    SgFunctionCallExp*        callWrapped = SB::buildFunctionCallExp( newName, result_type, args, body );
    SgInitializedName*        resultName = NULL;
    SgStatement*              callStatement = NULL;

    // \todo skip legal qualifiers that could be on top of void
    if (!isSgTypeVoid(result_type))
    {
      // add call to original function and assign result to variable
      SgVariableDeclaration*  res = SB::buildVariableDeclaration( "res", result_type, SB::buildAssignInitializer(callWrapped), body );
      SgVarRefExp*            resref = SB::buildVarRefExp( res );

      appendStatement(res, body);

      // add return statement, returning result
      resultName    = &getFirstVariable(*res);
      callStatement = res;

      appendStatement(SB::buildReturnStmt(resref), body);
    }
    else
    {
      // add function call statement to original function
      callStatement = SB::buildExprStatement(callWrapped);
      appendStatement(callStatement, body);
    }

    ROSE_ASSERT(callStatement);

    // create non defining declaration
    SgExprListExp*            decorator_proto = deepCopy( decorators );
    SgFunctionDeclaration*    wrapperfn_proto = SB::buildNondefiningFunctionDeclaration(wrapperfn, containing_scope, decorator_proto);

    // add the new functions at the proper location of the surrounding scope
    insertStatementBefore(&definingDeclaration, wrapperfn_proto);
    insertStatementAfter (&definingDeclaration, wrapperfn);

    return std::make_pair(callStatement, resultName);
  }

  //
  // flatten C/C++ array dimensions
  namespace
  {
    struct VarrefBuilder
    {
      virtual SgVarRefExp* get() const = 0;

      static
      SgVarRefExp* build(const SgVarRefExp& n)
      {
        return SageInterface::deepCopy(&n);
      }

      static
      SgVarRefExp* build(SgInitializedName& n)
      {
        SgScopeStatement* scope = sg::ancestor<SgStatement>(n).get_scope();

        return SageBuilder::buildVarRefExp(&n, scope);
      }
    };

    template <class AstNode>
    struct VarrefCreator : VarrefBuilder
    {
      AstNode& origin;

      explicit
      VarrefCreator(AstNode& orig)
      : origin(orig)
      {}

      SgVarRefExp* get() const { return VarrefBuilder::build(origin); }
    };

    template <class AstNode>
    VarrefCreator<AstNode>
    varrefCreator(AstNode& n)
    {
      return VarrefCreator<AstNode>(n);
    }

    SgExpression* create_mulop(SgExpression* lhs, const SgExpression* rhs)
    {
      namespace SB = SageBuilder;
      namespace SI = SageInterface;

      // we own the lhs (intermediate result),
      // but we do not own the rhs (another top-level expression)
      return SB::buildMultiplyOp(lhs, SI::deepCopy(rhs));
    }

    std::pair<std::vector<SgExpression*>, SgType*>
    get_C_array_dimensions_aux(const SgArrayType& arr_type)
    {
      namespace SB = SageBuilder;
      namespace SI = SageInterface;

      const SgArrayType*         arrtype = &arr_type;
      std::vector<SgExpression*> indices;
      SgType*                    undertype = NULL;

      // \todo when get_index() does not return a nullptr anymore
      //       the condition can be removed
      if (arrtype->get_index() == NULL)
      {
        indices.push_back(SB::buildNullExpression());
        undertype = arrtype->get_base_type();
        arrtype = isSgArrayType(undertype);
      }

      while (arrtype)
      {
        SgExpression* indexexpr = arrtype->get_index();
        ROSE_ASSERT(indexexpr);

        indices.push_back(SI::deepCopy(indexexpr));
        undertype = arrtype->get_base_type();
        arrtype = isSgArrayType(undertype);
      }

      ROSE_ASSERT((!indices.empty()) && undertype);
      return std::make_pair(indices, undertype);
    }

    /// \param varrefBuilder generates an unowned varref expression on demand
    std::vector<SgExpression*>
    get_C_array_dimensions_aux(const SgArrayType& arrtype, const VarrefBuilder& varrefBuilder)
    {
      namespace SB = SageBuilder;

      std::pair<std::vector<SgExpression*>, SgType*> res = get_C_array_dimensions_aux(arrtype);
      const std::vector<SgExpression*>::iterator     first = res.first.begin();

      // if the first dimension was open, create the expression for it
      if (isSgNullExpression(*first))
      {
        // handle implicit first dimension for array initializers
        // for something like
        //      int p[][2][3] = {{{ 1, 2, 3 }, { 4, 5, 6 }}}
        //  we can calculate the first dimension as
        //      sizeof( p ) / ( sizeof( int ) * 2 * 3 )

        const std::vector<SgExpression*>::iterator aa = first+1;
        const std::vector<SgExpression*>::iterator zz = res.first.end();

        SgExpression* sz_undertype = SB::buildSizeOfOp(res.second);
        SgExpression* denominator  = std::accumulate(aa, zz, sz_undertype, create_mulop);
        SgSizeOfOp*   sz_var       = SB::buildSizeOfOp(varrefBuilder.get());
        SgExpression* sz           = SB::buildDivideOp(sz_var, denominator);

        std::swap(*first, sz);
        delete sz;
      }

      return res.first;
    }
  }

  std::vector<SgExpression*>
  SageInterface::get_C_array_dimensions(const SgArrayType& arrtype)
  {
    return get_C_array_dimensions_aux(arrtype).first;
  }

  std::vector<SgExpression*>
  SageInterface::get_C_array_dimensions(const SgArrayType& arrtype, const SgVarRefExp& varref)
  {
    return get_C_array_dimensions_aux(arrtype, varrefCreator(varref));
  }

  std::vector<SgExpression*>
  SageInterface::get_C_array_dimensions(const SgArrayType& arrtype, SgInitializedName& initname)
  {
    return get_C_array_dimensions_aux(arrtype, varrefCreator(initname));
  }


// DQ (1/23/2013): Added support for generated a set of source sequence entries.
class CollectSourceSequenceNumbers : public AstSimpleProcessing
   {
     public:
          set<unsigned int> sourceSequenceSet;

          void visit ( SgNode* astNode );
   };

// DQ (1/23/2013): Added support for generated a set of source sequence entries.
void
CollectSourceSequenceNumbers::visit ( SgNode* astNode )
   {
     Sg_File_Info* fileInfo = astNode->get_file_info();
     if (fileInfo != NULL)
        {
          unsigned int source_sequence_number = fileInfo->get_source_sequence_number();
#if 0
          printf ("In CollectSourceSequenceNumbers::visit(): source_sequence_number = %zu \n",source_sequence_number);
#endif
          sourceSequenceSet.insert(source_sequence_number);
        }
   }

// DQ (1/23/2013): Added support for generated a set of source sequence entries.
set<unsigned int>
SageInterface::collectSourceSequenceNumbers( SgNode* astNode )
  {
    CollectSourceSequenceNumbers traversal;

    traversal.traverse(astNode,preorder);

    return traversal.sourceSequenceSet;
    
  }

/*Winnie, loop collapse, collapse nested for loops into one large for loop
 *  return a SgExprListExp *, which will contain a list of SgVarRefExp * to variables newly created, inserted outside of the 
 *                            loop scope, and used inside the loop scope.
 *                            If the target_loop comes with omp target directive, these variables should be added in map in clause in 
 *                            transOmpCollpase(..) function in omp_lowering.cpp.
 *
 *  The input loop pointer is changed to the generated collapsed loop after the transformation.                              
 *
 * 
 *  Loop is normalized to [lb,ub,step], ub is inclusive (<=, >=)
 *  
 *  to collapse two level of loops:
 *  iteration_count_one= (ub1-lb1+1)%step1 ==0?(ub1-lb1+1)/step1: (ub1-lb1+1)/step1+1
 *  iteration_count_two= (ub2-lb2+1)%step2 ==0?(ub2-lb2+1)/step2: (ub2-lb2+1)/step2+1
 *  total_iteration_count = iteration_count_one * iteration_count_two
 *
 *  Decide incremental/decremental loop by checking operator of test statement(ub), <=/>=, this is done in isCanonicalForLoop()
 *
 * Example 1:
 * for (int i=lb2;i<ub2;i+=inc2)                 //incremental 
 *  {
 *      for (int j=lb1;j>ub1;i+=inc1)            //decremental
 *      {
 *                  for (int l=lb2;l<ub2;l+=inc2)        //incremental 
 *              {
 *               a[i][j][l]=i+j+l;      
 *              }
 *      }
 *  }
 *
 *==> translated output code ==>
 *  int i_nom_1_total_iters = (ub2 - 1 - lb2 + 1) % inc2 == 0?(ub2 - 1 - lb2 + 1) / inc2 : (ub2 - 1 - lb2 + 1) / inc2 + 1;
 *  int j_nom_2_total_iters = (lb1 - (ub1 + 1) + 1) % (inc1 * -1) == 0?(lb1 - (ub1 + 1) + 1) / (inc1 * -1) : (lb1 - (ub1 + 1) + 1) / (inc1 * -1) + 1;
 *  int l_nom_3_total_iters = (ub2 - 1 - lb2 + 1) % inc2 == 0?(ub2 - 1 - lb2 + 1) / inc2 : (ub2 - 1 - lb2 + 1) / inc2 + 1;
 *  int final_total_iters = 1 * i_nom_1_total_iters* j_nom_2_total_iters* l_nom_3_total_iters;
 *  int i_nom_1_interval = j_nom_2_total_iters * (l_nom_3_total_iters* 1);
 *  int j_nom_2_interval = l_nom_3_total_iters * 1;
 *  int l_nom_3_interval = 1;
 *
 *  for (int new_index = 0; new_index <= final_total_iters- 1; new_index += 1) {
 *    i_nom_1 = new_index / i_nom_1_interval* inc2 + lb2;
 *    int i_nom_1_remainder = new_index % i_nom_1_interval;
 *    j_nom_2 = -(i_nom_1_remainder / j_nom_2_interval* (inc1 * -1)) + lb1;
 *    l_nom_3 = i_nom_1_remainder % j_nom_2_interval* inc2 + lb2;
 *    a[i_nom_1][j_nom_2][l_nom_3] = i_nom_1 + j_nom_2 + l_nom_3;
 *  }
 *
 *  Example 2 with concrete numbers:
 *
 * // collapse the following two level of for loops:
 *       for (i=1; i<=9; i+=1)      //incremental for loop
 *       {
 *          for(j=10; j>=1; j+=-2)    //decremental for loop
 *         {
 *              a[i][j]=i+j;
 *         }
 *       }
 * // it becomes
 *     // total iteration count = ((9 - 1 + 1)/1) * ((10 - 1 + 1)/2) = 45
 *     // ub = 45
 *     // lb = 0
 *       
 *     int i_nom_1_total_iters = 9;
 *     int j_nom_1_total_iters = 5;      // 10 % (-2 * -1) == 0 ? 10 / (-2 * -1) : 10 /(-2 * -1) + 1;
 *     int final_total_iters = 45;       // i_nom_1_total_iters * j_nom_2_total_iters;
 *
 *     int i_nom_1_interval = 5;        
 * 
 *     for (z=0; z<=44; z+=1)
 *     {
 *       i_nom_1 = z / 5 + 1;
 *       j_nom_2 = -(z % 5 * 2) + 10;
 *       a[i_nom_1][j_nom_2]=i_nom_1 + j_nom_2;
 *     }
 *  // for the generated loop, a few compiler generated variables are introduced
 *     final_total_iters for the collapsed loop
 *     i_interval for each of the n-1 loops
 **
*/

#ifndef USE_ROSE
SgExprListExp * SageInterface::loopCollapsing(SgForStatement* loop, size_t collapsing_factor)
{
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
  //Handle 0 and 1, which means no collapsing at all
    if (collapsing_factor <= 1)
        return NULL;

    SgExprListExp * new_var_list = buildExprListExp();  //expression list contains all the SgVarRefExp * to variables that need to be added in the mapin clause

    /* 
     *step 1: grab the target loops' header information
     */
    SgForStatement *& target_loop = loop;

    // we use global scope to help generate unique variable name later
    // local scope-based unique names may cause conflicts if the declarations are moved around (in OpenMP target data promotion)
    SgGlobal* global_scope = getGlobalScope (loop); 
    ROSE_ASSERT (global_scope != NULL);
    SgInitializedName** ivar = new SgInitializedName*[collapsing_factor];
    SgExpression** lb = new SgExpression*[collapsing_factor];
    SgExpression** ub = new SgExpression*[collapsing_factor];
    SgExpression** step = new SgExpression*[collapsing_factor];
    SgStatement** orig_body = new SgStatement*[collapsing_factor]; 
    
    SgExpression** total_iters = new SgExpression*[collapsing_factor]; //Winnie, the real iteration counter in each loop level
    SgExpression** interval = new SgExpression*[collapsing_factor]; //Winnie, this will be used to calculate i_nom_1_remainder
    bool *isPlus = new bool[collapsing_factor]; //Winnie, a flag indicates incremental or decremental for loop


    //Winnie, get loops info first
    std::vector<SgForStatement* > loops= SageInterface::querySubTree<SgForStatement>(target_loop,V_SgForStatement);
    ROSE_ASSERT(loops.size()>=collapsing_factor);
 
    SgForStatement* temp_target_loop = NULL;
    SgExpression* temp_range_exp = NULL; //Raw iteration range
    SgExpression* temp_range_d_step_exp = NULL; //temp_range_exp / step[i]
    SgExpression* temp_condition_1 = NULL; //Check whether temp_range_exp % step[i] == 0
    SgExpression* temp_total_iter = NULL;
    SgExpression* ub_exp = buildIntVal(1); //Winnie, upbound

    /*
    *    get lb, ub, step information for each level of the loops
    *    ub_exp is the final iterantion range(starting from 0) after loop collapsing
    *    total_iters[i], = (ub[i] - lb[i] + 1)/step[i]  is the total iter num in each level of loop before loop collapsing   
    */

    SgStatement* parent =  isSgStatement(getScope(target_loop)->get_parent());        //Winnie, the scope that include target_loop
    ROSE_ASSERT(getScope(target_loop)->get_parent()!= NULL);
    
    SgScopeStatement* scope = isSgScopeStatement(parent);    //Winnie, the scope that include target_loop
    
    SgStatement* insert_target;
    while(scope == NULL)
    {
       parent = isSgStatement(parent->get_parent());
       scope = isSgScopeStatement(parent);
    }

    insert_target = findLastDeclarationStatement(scope);
    if(insert_target != NULL)
        insert_target = getNextStatement(insert_target);
    else
        insert_target = getFirstStatement(scope);

    ROSE_ASSERT(scope != NULL); 


    for(size_t i = 0; i < collapsing_factor; i ++)
    {     
        temp_target_loop = loops[i]; 

        // normalize the target loop first  // adjust to numbering starting from 0
        forLoopNormalization(temp_target_loop);  

        if (!isCanonicalForLoop(temp_target_loop, &ivar[i], &lb[i], &ub[i], &step[i], &orig_body[i], &isPlus[i]))
        {
            cerr<<"Error in SageInterface::loopCollapsing(): target loop is not canonical."<<endl;
            dumpInfo(target_loop);
            return false;
        }
        
        ROSE_ASSERT(ivar[i]&& lb[i] && ub[i] && step[i]);
   
      
//Winnie, (ub[i]-lb[i]+1)%step[i] ==0?(ub[i]-lb[i]+1)/step[i]: (ub[i]-lb[i]+1)/step[i]+1; (need ceiling) total number of iterations in this level (ub[i] - lb[i] + 1)/step[i]
        if(isPlus[i] == true)
            temp_range_exp = buildAddOp(buildSubtractOp(copyExpression(ub[i]), copyExpression(lb[i])), buildIntVal(1));
        else{
            temp_range_exp = buildAddOp(buildSubtractOp(copyExpression(lb[i]), copyExpression(ub[i])), buildIntVal(1));
            step[i] = buildMultiplyOp(step[i], buildIntVal(-1));
        }
        temp_range_d_step_exp = buildDivideOp(temp_range_exp,copyExpression(step[i]));//(ub[i]-lb[i]+1)/step[i]
        
        temp_condition_1 = buildEqualityOp(buildModOp(copyExpression(temp_range_exp),copyExpression(step[i])),buildIntVal(0)); //(ub[i]-lb[i]+1)%step[i] ==0

        temp_total_iter = buildConditionalExp(temp_condition_1,temp_range_d_step_exp, buildAddOp(copyExpression(temp_range_d_step_exp),buildIntVal(1)));

        //build variables to store iteration numbers in each loop, simplify the calculation of "final_total_iters"
        //insert the new variable (store real iteration number of each level of the loop) before the target loop
        string iter_var_name= "_total_iters";
        //iter_var_name = ivar[i]->get_name().getString() + iter_var_name + generateUniqueName(temp_total_iter, false);  
        iter_var_name = "__"+ivar[i]->get_name().getString() + iter_var_name+ generateUniqueVariableName (global_scope,"");
        SgVariableDeclaration* total_iter = buildVariableDeclaration(iter_var_name, buildIntType(), buildAssignInitializer(temp_total_iter, buildIntType()), scope);  
        insertStatementBefore(insert_target, total_iter);    
        constantFolding (total_iter);
        total_iters[i] = buildVarRefExp(iter_var_name, scope);
        ub_exp = buildMultiplyOp(ub_exp, total_iters[i]);    //Winnie, build up the final iteration range 
    }


    /*
    * step 2: build new variables (new_index, final_total_iters, remainders...) for the new loop
    */

    /*Winnie, build another variable to store final total iteration counter of the loop after collapsing*/
    //string final_iter_counter_name = "final_total_iters" + generateUniqueName(ub_exp, false);
    string final_iter_counter_name = "__final_total_iters" + generateUniqueVariableName(global_scope,"");
    SgVariableDeclaration * final_total_iter = buildVariableDeclaration(final_iter_counter_name, buildIntType(), buildAssignInitializer(copyExpression(ub_exp), buildIntType()), scope);
    insertStatementBefore(insert_target, final_total_iter);
    ub_exp = buildVarRefExp(final_iter_counter_name, scope);
    new_var_list->append_expression(isSgVarRefExp(ub_exp));
  
    /*Winnie, interval[i] will make the calculation of remainders simpler*/
    for(unsigned int i = 0; i < collapsing_factor; i++)
    {
        interval[i] = buildIntVal(1);
        for(unsigned int j = collapsing_factor - 1; j > i; j--)
        {
            interval[i] = buildMultiplyOp(total_iters[j], interval[i]); 
        }
        //string interval_name = ivar[i]->get_name().getString() + "_interval" + generateUniqueName(interval[i], false);
        string interval_name = "__"+ ivar[i]->get_name().getString() + "_interval" + generateUniqueVariableName (global_scope,"");
        SgVariableDeclaration* temp_interval = buildVariableDeclaration(interval_name, buildIntType(), buildAssignInitializer(copyExpression(interval[i]), buildIntType()), scope);
        insertStatementBefore(insert_target, temp_interval);
        interval[i] = buildVarRefExp(interval_name, scope);
        new_var_list->append_expression(isSgVarRefExp(interval[i]));
    }


   //Winnie, starting from here, we are dealing with variables inside loop, update scope
      scope = getScope(target_loop);

   //Winnie, init statement of the loop header, copy the lower bound, we are dealing with a range, the lower bound should always be "0"
    //Winnie, declare a brand new var as the new index
      string ivar_name = "__collapsed_index"+ generateUniqueVariableName (global_scope,"");
      SgVariableDeclaration* new_index_decl = buildVariableDeclaration(ivar_name, buildIntType(), NULL, insert_target->get_scope());
      SgVariableSymbol * collapsed_index_symbol = getFirstVarSym (new_index_decl);
      insertStatementBefore(insert_target, new_index_decl);
      SgVarRefExp * clps_index_ref = buildVarRefExp(collapsed_index_symbol);
      new_var_list->append_expression(clps_index_ref);

//      SgVariableDeclaration* init_stmt = buildVariableDeclaration(ivar_name, buildIntType(), buildAssignInitializer(buildIntVal(0), buildIntType()), scope);  
  
  
     SgBasicBlock* body = isSgBasicBlock(deepCopy(temp_target_loop->get_loop_body())); // normalized loop has a BB body
     ROSE_ASSERT(body);
     SgExpression* new_exp = NULL;
     SgExpression* remain_exp_temp = buildVarRefExp(ivar_name, scope);
     std::vector<SgStatement*> new_stmt_list; 
     
     SgExprStatement* assign_stmt = NULL;
     
     /*  Winnie
     *   express old iterator variables (i_norm, j_norm ...)  with new_index,  
     *   new_exp, create new expression for each of the iterators
     *   i_nom_1 = (_new_index / interval[0])*step[0] + lb[0] 
     *   i_nom_1_remain_value = (_new_index % interval[0])*step[0] + lb[0], create a new var to store remain value
     *   create a new var to store total_iters[i]
     */ 
     for(unsigned int i = 0; i < collapsing_factor - 1; i ++)  
     {  
         if(isPlus[i] == true)
             new_exp = buildAddOp(buildMultiplyOp(buildDivideOp(copyExpression(remain_exp_temp), copyExpression(interval[i])), step[i]), copyExpression(lb[i]));  //Winnie, (i_remain/interval[i])*step[i] + lb[i]
         else
             new_exp = buildAddOp(buildMinusOp(buildMultiplyOp(buildDivideOp(copyExpression(remain_exp_temp), copyExpression(interval[i])), step[i])), copyExpression(lb[i]));  //Winnie, -(i_remain/interval[i])*step[i] + lb[i], for decremental loop

         assign_stmt = buildAssignStatement(buildVarRefExp(ivar[i], scope), copyExpression(new_exp));   
         new_stmt_list.push_back(assign_stmt); 
         remain_exp_temp = buildModOp((remain_exp_temp), copyExpression(interval[i])); 

         if(i != collapsing_factor - 2){ //Winnie, if this is the second last level of loop, no need to create new variable to hold the remain_value, or remove the original index variable declaration
             string remain_var_name= "_remainder";
             remain_var_name = "__"+ ivar[i]->get_name().getString() + remain_var_name;  
             SgVariableDeclaration* loop_index_decl = buildVariableDeclaration(remain_var_name, buildIntType(), buildAssignInitializer(remain_exp_temp, buildIntType()), scope);  
             remain_exp_temp = buildVarRefExp(remain_var_name, scope);
             new_stmt_list.push_back(loop_index_decl); 
         }
         new_exp = NULL;
     }

//Winnie, the inner most loop, iter
    if(isPlus[collapsing_factor - 1] == true)
        assign_stmt = buildAssignStatement(buildVarRefExp(ivar[collapsing_factor - 1], scope), buildAddOp(buildMultiplyOp(remain_exp_temp, step[collapsing_factor - 1]), lb[collapsing_factor - 1]));  
    else
        assign_stmt = buildAssignStatement(buildVarRefExp(ivar[collapsing_factor - 1], scope), buildAddOp(buildMinusOp(buildMultiplyOp(remain_exp_temp, step[collapsing_factor - 1])), lb[collapsing_factor - 1]));   
     new_stmt_list.push_back(assign_stmt);
     prependStatementList(new_stmt_list, body);

    /*
    * step 3: build the new loop, new step is always 1, disregard value of step[i]
    */
    SgExpression* incr_exp = buildPlusAssignOp(buildVarRefExp(ivar_name, scope), buildIntVal(1)); 

    //Winnie, build the new conditional expression/ub
    SgExprStatement* cond_stmt = NULL;
    ub_exp = buildSubtractOp(ub_exp, buildIntVal(1));
    cond_stmt = buildExprStatement(buildLessOrEqualOp(buildVarRefExp(ivar_name,scope),copyExpression(ub_exp)));
    ROSE_ASSERT(cond_stmt != NULL);

    SgForStatement* new_loop = buildForStatement(buildAssignStatement(clps_index_ref, buildIntVal(0)), cond_stmt,incr_exp, body);  //Winnie, add in the new block!
    new_loop->set_parent(scope);  //TODO: what's the correct parent?

    replaceStatement(target_loop, new_loop);

    target_loop = new_loop; //Winnie, so that transOmpLoop() can work on the collapsed loop   
   // constant folding for the transformed AST
   ConstantFolding::constantFoldingOptimization(scope->get_parent(),false);   //Winnie, "scope" is the scope that contains new_loop, this is the scope where we insert some new variables to store interation count and intervals

    delete [] ivar;
    delete [] lb;
    delete [] ub;
    delete [] step;
    delete [] orig_body;
    delete [] total_iters;
    delete [] interval;
    delete [] isPlus;

    #endif

    return new_var_list;
}



bool
SageInterface::isStructurallyEquivalentAST( SgNode* tree1, SgNode* tree2 )
   {
  // DQ (3/4/2014): Added support for testing two trees for equivalents using the AST iterators.

#ifndef USE_CMAKE
  // DQ (3/8/2014): Make this conditionally compiled based on when CMake is not used because the libraries are not configured yet.

  // This is AST container for the ROSE AST that will provide an iterator.
  // We want two iterators (one for the copy of the snippet and one for the 
  // original snippet so that we can query the original snippet's AST 
  // as we process each IR node of the AST for the copy of the snippet.
  // Only the copy of the snippet is inserted into the target AST.
     RoseAst ast_of_copy(tree1);
     RoseAst ast_of_original(tree2);

  // printf ("ast_of_copy.size() = %zu \n",ast_of_copy.size());

  // Build the iterators so that we can increment thorugh both ASTs one IR node at a time.
     RoseAst::iterator i_copy     = ast_of_copy.begin();
     RoseAst::iterator i_original = ast_of_original.begin();

  // Iterate of the copy of the snippet's AST.
     while (i_copy != ast_of_copy.end())
        {
#if 0
          printf ("*i_copy = %p = %s \n",*i_copy,(*i_copy)->class_name().c_str());
          printf ("*i_original = %p = %s \n",*i_original,(*i_original)->class_name().c_str());
#endif
       // DQ (2/28/2014): This is a problem for some of the test codes (TEST   store/load heap string [test7a] and [test7a])
       // ROSE_ASSERT((*i_copy)->variantT() == (*i_original)->variantT());
          if ((*i_copy)->variantT() != (*i_original)->variantT())
             {
#if 0
               printf ("ERROR: return from SageInterface::isStructurallyEquivalentAST(): (*i_copy)->variantT() != (*i_original)->variantT() \n");
#endif
#if 0
               printf ("Making this an error! \n");
               ROSE_ASSERT(false);
#endif
               return false;
             }

          i_copy++;

       // Verify that we have not reached the end of the ast for the original (both the 
       // copy and the original are the same structurally, and thus the same size).
          ROSE_ASSERT(i_original != ast_of_original.end());
          i_original++;
        }

  // We have reached the end of both ASTs.
     ROSE_ASSERT(i_copy == ast_of_copy.end() && i_original == ast_of_original.end());
#endif

     return true;
   }


#endif

//------------------------------------------------------------------------------
#ifdef ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
//------------------------------------------------------------------------------

/**
 * Create a temporary directory if it does not yet exist and return its name.
 */
string SageInterface::getTempDirectory(SgProject *project) {
    jstring temp_directory = (jstring) Rose::Frontend::Java::Ecj::currentEnvironment -> CallObjectMethod(::currentJavaTraversalClass, Rose::Frontend::Java::Ecj::getTempDirectoryMethod);

    const char *utf8 = Rose::Frontend::Java::Ecj::currentEnvironment -> GetStringUTFChars(temp_directory, NULL);
    ROSE_ASSERT(utf8);
    string directory_name = utf8;
    Rose::Frontend::Java::Ecj::currentEnvironment -> ReleaseStringUTFChars(temp_directory, utf8);

    list<string> sourcepath = project -> get_Java_sourcepath();
    sourcepath.push_back(directory_name); // push it in the back because it should have lower priority
    project -> set_Java_sourcepath(sourcepath);

    return directory_name;
}


/**
 * Use the system command to remove a temporary directory and all its containing files.
 */
void SageInterface::destroyTempDirectory(string directory_name) {
    string command = string("rm -fr ") + directory_name;
    int status = system(command.c_str());
    ROSE_ASSERT(status == 0);
}


/**
 * Invoke JavaRose to translate a given file and put the resulting AST in the global space of the project.
 */
SgFile* SageInterface::processFile(SgProject *project, string filename, bool unparse /* = false */) {
    //
    // Set up the new source file for processing "a la Rose".
    //
    project -> get_sourceFileNameList().push_back(filename);
    Rose_STL_Container<std::string> arg_list = project -> get_originalCommandLineArgumentList();
    arg_list.push_back(filename);
    Rose_STL_Container<string> fileList = CommandlineProcessing::generateSourceFilenames(arg_list, false);
    CommandlineProcessing::removeAllFileNamesExcept(arg_list, fileList, filename);
    int error_code = 0; // need this because determineFileType takes a reference "error_code" argument.
    SgFile *file = determineFileType(arg_list, error_code, project);
    SgSourceFile *sourcefile = isSgSourceFile(file);
    ROSE_ASSERT(sourcefile);
    sourcefile -> set_parent(project);

    //
    // Insert the file into the list of files in the project.
    //
    project -> get_fileList_ptr() -> get_listOfFiles().push_back(sourcefile);
    ROSE_ASSERT(sourcefile == isSgSourceFile((*project)[filename]));

    sourcefile -> build_Java_AST(arg_list, project -> get_originalCommandLineArgumentList());

    if (! unparse) { // if we are not supposed to unparse this file, 
        project -> get_fileList_ptr() -> get_listOfFiles().pop_back(); // remove it from the list of files in the project
        ROSE_ASSERT(sourcefile != isSgSourceFile((*project)[filename]));
    }

    return file;
}


/**
 * Using the package_name, create a file with a package statement, translate it in order to load the package
 * into the project.
 */
string SageInterface::preprocessPackage(SgProject *project, string package_name) {
    string command = "package " + package_name + ";";

    //
    // Call the Java side to create an input file with the relevant package statement; translate the file and return the file name.
    //
    jstring temp_file = (jstring) Rose::Frontend::Java::Ecj::currentEnvironment -> CallObjectMethod(Rose::Frontend::Java::Ecj::currentJavaTraversalClass,
                                                                                                    Rose::Frontend::Java::Ecj::createTempFileMethod,
                                                                                                    Rose::Frontend::Java::Ecj::currentEnvironment -> NewStringUTF(command.c_str()));

    const char *utf8 = Rose::Frontend::Java::Ecj::currentEnvironment -> GetStringUTFChars(temp_file, NULL);
    ROSE_ASSERT(utf8);
    string filename = (string) utf8;
    Rose::Frontend::Java::Ecj::currentEnvironment -> ReleaseStringUTFChars(temp_file, utf8);

    processFile(project, filename); // translate the file 

    return package_name;
}


/**
 * Using the import_string parameter, create a file with the relevant import statement; translate the file and
 * add its AST to the project.
 */
string SageInterface::preprocessImport(SgProject *project, string import_string) {
    string command = "import " + import_string + ";";

    //
    // Call the Java side to create an input file with the relevant import statement.
    //
    jstring temp_file = (jstring) Rose::Frontend::Java::Ecj::currentEnvironment -> CallObjectMethod(Rose::Frontend::Java::Ecj::currentJavaTraversalClass,
                                                                                                    Rose::Frontend::Java::Ecj::createTempFileMethod,
                                                                                                    Rose::Frontend::Java::Ecj::currentEnvironment -> NewStringUTF(command.c_str()));

    const char *utf8 = Rose::Frontend::Java::Ecj::currentEnvironment -> GetStringUTFChars(temp_file, NULL);
    ROSE_ASSERT(utf8);
    string filename = (string) utf8;
    Rose::Frontend::Java::Ecj::currentEnvironment -> ReleaseStringUTFChars(temp_file, utf8);

    processFile(project, filename); // translate the file 

    return import_string;
}


/**
 * Using the file_content string, create a file with the content in question; build its AST and
 * add it to the project.
 */
SgFile* SageInterface::preprocessCompilationUnit(SgProject *project, string file_name, string file_content, bool unparse /* true */) {
    //
    // Call the Java side to create an input file with the relevant import statement.
    //
    jstring temp_file = (jstring) Rose::Frontend::Java::Ecj::currentEnvironment -> CallObjectMethod(Rose::Frontend::Java::Ecj::currentJavaTraversalClass,
                                                                                                    Rose::Frontend::Java::Ecj::createTempNamedFileMethod,
                                                                                                    Rose::Frontend::Java::Ecj::currentEnvironment -> NewStringUTF(file_name.c_str()),
                                                                                                    Rose::Frontend::Java::Ecj::currentEnvironment -> NewStringUTF(file_content.c_str()));

    const char *utf8 = Rose::Frontend::Java::Ecj::currentEnvironment -> GetStringUTFChars(temp_file, NULL);
    ROSE_ASSERT(utf8);
    string filename = (string) utf8;
    Rose::Frontend::Java::Ecj::currentEnvironment -> ReleaseStringUTFChars(temp_file, utf8);

    return processFile(project, filename, unparse); // translate the file and unparse it, if requested (unparse=true is the default).
}


/**
 * Look for a qualified package name in the given scope and return its package definition.
 */
SgClassDefinition *SageInterface::findJavaPackage(SgScopeStatement *scope, string package_name) {
    ROSE_ASSERT(scope);
    SgClassDefinition *package_definition = NULL;
    for (int index = 0, length = package_name.size(); index < length; index++) {
        int n;
        for (n = index; n < length; n++) {
            if (package_name[n] == '.') {
                break;
            }
        }
        string name = package_name.substr(index, n - index);

        SgClassSymbol *package_symbol = scope -> lookup_class_symbol(name);
        if (package_symbol == NULL) { // package not found?
            return NULL;
        }

        SgJavaPackageDeclaration *package_declaration = isSgJavaPackageDeclaration(package_symbol -> get_declaration() -> get_definingDeclaration());
        ROSE_ASSERT(package_declaration);
        package_definition = package_declaration -> get_definition();
        ROSE_ASSERT(package_definition);
        scope = package_definition;

        index = n;
    }

    return package_definition;
}


/**
 * Process a qualified package name, if needed, and return its package definition.
 */
SgClassDefinition *SageInterface::findOrInsertJavaPackage(SgProject *project, string package_name, bool create_directory /* = false */) {
    SgGlobal *global_scope = project -> get_globalScopeAcrossFiles();
    SgClassDefinition *package_definition = findJavaPackage(global_scope, package_name);
    if (package_definition == NULL) { // try again after loading the package
        preprocessPackage(project, package_name);

        //
        // If requested, Create the directory associated with this package_name.
        //
        if (create_directory) {
            Rose::Frontend::Java::Ecj::currentEnvironment -> CallObjectMethod(Rose::Frontend::Java::Ecj::currentJavaTraversalClass,
                                                                              Rose::Frontend::Java::Ecj::createTempNamedDirectoryMethod,
                                                                              Rose::Frontend::Java::Ecj::currentEnvironment -> NewStringUTF(package_name.c_str()));
        }

        package_definition = findJavaPackage(global_scope, package_name);
    }

    return package_definition;
}

/**
 * If the class_name already exists in the scope, return it. Otherwise, import it.
 */
SgClassDeclaration *SageInterface::findOrImportJavaClass(SgProject *project, SgClassDefinition *package_definition, string class_name) {
    ROSE_ASSERT(package_definition);
    SgClassSymbol *class_symbol = package_definition -> lookup_class_symbol(class_name);
    SgClassDeclaration *class_declaration = (class_symbol == NULL
                                                  ? NULL
                                                  : isSgClassDeclaration(class_symbol -> get_declaration() -> get_definingDeclaration()));
    if ((! class_declaration) || (! class_declaration -> attributeExists("complete"))) { // class not available in the scope
        string qualified_name = package_definition -> get_qualified_name().getString() + "." + class_name;
        preprocessImport(project, qualified_name);
        class_symbol = package_definition -> lookup_class_symbol(class_name);
    }

    class_declaration = (class_symbol == NULL 
                                       ? NULL
                                       : isSgClassDeclaration(class_symbol -> get_declaration() -> get_definingDeclaration()));

    return class_declaration;
}


/**
 * If the class_name already exists in the scope, return it. Otherwise, import it.
 */
SgClassDeclaration *SageInterface::findOrImportJavaClass(SgProject *project, string package_name, string class_name) {
    return findOrImportJavaClass(project, findOrInsertJavaPackage(project, package_name, false), class_name);
}


/**
 * If the class_name already exists in the scope, return it. Otherwise, import it.
 */
SgClassDeclaration *SageInterface::findOrImportJavaClass(SgProject *project, SgClassType *class_type) {
    SgClassDeclaration *class_declaration = isSgClassDeclaration(class_type -> get_declaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *scope = isSgClassDefinition(class_declaration -> get_scope());
    while (scope && (! isSgJavaPackageDeclaration(scope -> get_parent()))) {
        class_declaration = isSgClassDeclaration(scope -> get_parent());
        ROSE_ASSERT(class_declaration);
        scope = isSgClassDefinition(class_declaration -> get_scope());
    }

    if (! scope) { // a local type !!!
        return NULL;
    }

    string class_name = class_declaration -> get_name().getString();
    return findOrImportJavaClass(project, scope, class_name);
}


/**
 * Find a main method in a Java class.
 */
SgMemberFunctionDeclaration *SageInterface::findJavaMain(SgClassDefinition *class_definition) {
    ROSE_ASSERT(class_definition);
    SgFunctionParameterTypeList *type_list = SageBuilder::buildFunctionParameterTypeList();
    ROSE_ASSERT(type_list);
    SgArrayType *string_array_type = getUniqueJavaArrayType(Rose::Frontend::Java::StringClassType, 1);
    ROSE_ASSERT(string_array_type);
    type_list -> append_argument(string_array_type);
    SgFunctionType *member_function_type = SageBuilder::buildMemberFunctionType(SgTypeVoid::createType(), type_list, class_definition, 0); // mfunc_specifier);
    SgFunctionSymbol *method_symbol = class_definition -> lookup_function_symbol("main", member_function_type);
    delete type_list;
    return (method_symbol == NULL ? NULL : isSgMemberFunctionDeclaration(method_symbol -> get_declaration()));
}


/**
 * Find a main method in a Java class.
 */
SgMemberFunctionDeclaration *SageInterface::findJavaMain(SgClassType *class_type) {
    SgClassDeclaration *class_declaration = isSgClassDeclaration(class_type -> get_declaration() -> get_definingDeclaration());
    ROSE_ASSERT(class_declaration);
    SgClassDefinition *class_definition = class_declaration -> get_definition();
    return findJavaMain(class_definition);
}

//------------------------------------------------------------------------------
#endif // ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
//------------------------------------------------------------------------------



//! Move a declaration to a scope which is the closest to the declaration's use places
/*  On the request from Jeff Keasler, we provide this transformation:
 *  For a declaration, find the innermost scope we can move it into, without breaking the code's original semantics
 *  For a single use place, move to the innermost scope.
 *  For the case of multiple uses, we may need to duplicate the declarations and move to two scopes if there is no variable reuse in between, 
 *    otherwise, we move the declaration into the innermost common scope of the multiple uses. 
 *  
 *  Data structure: we maintain a scope tree, in which each node is 
 *     a scope 1) defining or 2) use the variable, or 3) is live in between .
 *  Several implementation choices for storing the tree
 *  1) The scope tree reuses the AST. AST attribute is used to store extra information  
 *  2) A dedicated scope tree independent from AST. 
 *  3) Storing individual scope chains (paths) in the tree
 *     hard to maintain consistency if we trim paths, hard to debug.
 *
 *  For efficiency, we save only a single scope node  if there are multiple uses in the same scope. 
 *  Also for two use scopes with enclosing relationship, we only store the outer scope in the scope tree and trim the rest. 
 *
 *  Algorithm:
 *    Save the scope of the declaration int DS
 *    Step 1: create a scope tree first, with trimming 
 *    Pre-order traversal to find all references to the declaration
 *    For each reference place
 *    {
 *       back track all its scopes until we reach DS
 *       if DS == US, nothing can be done to move the declaration, stop. 
 *       all scopes tracked are formed into a scope chain (a vector: US .. DS).
 *
 *       create a scope node of use type, called US.
 *       For all scopes in between, create nodes named intermediate scope or IS. 
 *       Add Ds-IS1-IS2..-US (reverse order of the scope chain) into the scope tree, consider consolidating overlapped scope chains;
 *          1) if an intermediate scope is equal to a use scope of another scope chain; stop add the rest of this chain. 
 *          2) if we are adding a use scope into the scope tree, but the same scope is already added by a intermediate scope from anther chain
 *             we mark the  existing scope node as a use scope, and remove its children from the scope tree.
 *    }
 *
 *    Step 2: find the scopes to move the declaration into
 *    find the innermost scope containing all paths to leaves: innerscope: single parent, multiple children
 *    count the number of children of innerscope: 
 *      if this is only one leaf: move the declaration to the innermost scope
 *      if there are two scopes: 
 *          not liveout for the variable in question?  duplicate the declaration and move to each scope chain. 
 *          if yes liveout in between two scopes.  no duplication, move the declaration to innerscope
 *        
 *  //TODO optimize efficiency for multiple declarations
 * //TODO move to a separated source file or even namespace
 * By Liao, 9/3/2014
*/

enum ScopeType {s_decl, s_intermediate, s_use};
class Scope_Node {
  public: 
    Scope_Node (SgScopeStatement* s, ScopeType t):scope(s), s_type(t) {};
    //TODO: protect members behind access functions
    // scope information
    SgScopeStatement* scope; 
    ScopeType s_type;
    int depth; // the depth in the tree (path or chain) starting from 0

    // for tree information
    Scope_Node* parent; // point to the parent scope in the chain
    std::vector < Scope_Node* > children ; // point to children scopes 
  
    //print the subtree rooted at this node to a dot file named as filename.dot
    void printToDot(std::string filename);
    std::string prettyPrint();
    //! Depth-first traverse to get all nodes within the current subtree
    void traverse_node(std::vector<Scope_Node* >& allnodes);
    //! Delete all children, grandchildren, etc.
    void deep_delete_children();
    //! Starting from the root, find the first node which has more than one children.
    // This is useful to identify the innermost common scope of all leaf scopes.
    Scope_Node * findFirstBranchNode();
    //~Scope_Node () {  deep_delete_children(); }

   private: 
      //! Recursive traverse the current subtree and write dot file information
      void traverse_write (Scope_Node* n, std::ofstream & dotfile);
      std::string getLineNumberStr(){ int lineno = scope->get_file_info()->get_line(); return StringUtility::numberToString(lineno); } ;
      // Dot graph Node Id: unique memory address, prepend with "n_".
      std::string getDotNodeId() { return "n_"+StringUtility::numberToString(scope); };
      std::string getScopeTypeStr() 
      { string rt;  
        if (s_type == s_decl) rt = "s_decl"; 
        else if (s_type == s_intermediate) rt = "s_intermediate";
        else if (s_type == s_use) rt = "s_use";
        else ROSE_ASSERT (false);
        return rt; }
  };

// Topdown traverse a tree to find the first node with multiple children
// Intuitively, the innermost common scope for a variable.
// However, we have to adjust a few special cases: 
// For example: if-stmt case
//  A variable is used in both true and false body.
 // Naive analysis will find if-stmt is the inner-most common scope.
//  But we cannot really move a declaration to the if-stmt scope (condition)
Scope_Node*  Scope_Node::findFirstBranchNode()
{
  Scope_Node* first_branch_node = this; 
  // three cases: 0 children, 1 children, >1 children
  while (first_branch_node->children.size()==1)
    first_branch_node = first_branch_node->children[0];

  // Adjust for if-stmt: special adjustment
  // switch stmt needs not to be adjusted since there is a middle scope as the innermost scope
  // if a variable is used in multiple case: scopes. 
   if (isSgIfStmt (first_branch_node->scope))
      first_branch_node = first_branch_node->parent;
  // now the node must has either 0 or >1 children.
  return first_branch_node; 
}
  
//TODO make this a member of Scope_Node.  
std::map <SgScopeStatement* , Scope_Node*>  ScopeTreeMap; // quick query if a scope is in the scope tree, temp variable shared by all trees.
void Scope_Node::deep_delete_children()
{
  std::vector <Scope_Node*> allnodes;
  // TODO better way is to use width-first traversal and delete from the bottom to the top
  this->traverse_node (allnodes);
  // allnodes[0] is the root node itself, we keep it.
  // reverse order of preorder to delete things?
  //for (size_t i =1; i<allnodes.size(); i++) 
  for (size_t i =allnodes.size()-1 ; i>0; i--) 
  {
    Scope_Node* child = allnodes[i];
    // mark the associated scope as not in the scope tree
    ScopeTreeMap[child->scope] = NULL;
    // TODO: currently a workaround for multiple delete due to recursive call issue. 
    if (child != NULL)
    {
      delete child;
      allnodes[i] = NULL;
    }
  }
  // no children for the current node
  children.clear(); 
}

void Scope_Node::traverse_node(std::vector<Scope_Node* >& allnodes)
{
  // action on current node, save to the vector
  ROSE_ASSERT (this != NULL); // we should not push NULL pointers
  allnodes.push_back(this);
  std::vector < Scope_Node* > children = this->children;
  for (size_t i=0; i<children.size(); i++) 
  {
     Scope_Node* child = children[i];
     if (child != NULL) // we may delete child nodes and have NULL pointers
       child->traverse_node(allnodes);
  }
}

std::string Scope_Node::prettyPrint()
{
  int lineno = scope->get_file_info()->get_line();
  return StringUtility::numberToString(scope)+"@"+StringUtility::numberToString(lineno);
}

void Scope_Node::printToDot (std::string filename)
{
  string full_filename = filename + ".dot";
  ofstream dotfile (full_filename.c_str());
  dotfile <<"digraph scopetree{"<<endl;
  traverse_write (this, dotfile);
  dotfile <<"}"<<endl;
}

void Scope_Node::traverse_write(Scope_Node* n,std::ofstream & dotfile)
{
  std::vector < Scope_Node* > children = n->children;
  // must output both node and edge lines: here is the node
  dotfile<<n->getDotNodeId()<<"[label=\""<< StringUtility::numberToString(n->scope)
         <<"\n"<<n->scope->class_name()
         <<"\nLine="<<n->getLineNumberStr()
         <<" Depth="<<StringUtility::numberToString(n->depth)
         <<"\nType="<<n->getScopeTypeStr()<<"\"];"<<endl;
  for (size_t i=0; i<children.size(); i++)
  {
    // Here is the edge
    dotfile<<n->getDotNodeId()<<"->"<<children[i]->getDotNodeId()<<";"<<endl;
    traverse_write (children[i], dotfile);
  }
}
 
 
//! A helper function to skip some scopes, such as while stmt scope: special adjustment.
// used for a variable showing up in condition expression of some statement. 
// We return a grand parent scope for those variables. 
/*
 *We don't try to merge a variable decl into the conditional of a while statement
 * The reason is that often the declaration has an initializer , which must be preserved.
 * The conditional contain the use of the declared variable, which usually cannot be merged with the declaration.
 * e.g.  int yy = 10; while (yy< 100) { yy ++; ... }
 * Another tricky thing is even though yy shows up in both condition and while-body, 
 * the scope tree will only show while-stmt as the single scope node since the body scope is shadowed. 
 * Note: the scope of the condition is considered to be the while-stmt.
 * TODO in a rare case like   int yy = 10; while (yy) { } we can merge the declaration and condition together.
 * */
static SgScopeStatement * getAdjustedScope(SgNode* n)
{
  ROSE_ASSERT (n!= NULL); 
  SgScopeStatement* result =  SageInterface::getScope (n);
  if (isSgWhileStmt (result) || isSgIfStmt (result) || isSgDoWhileStmt (result) || isSgSwitchStatement(result) )
    result = SageInterface::getEnclosingScope(result, false);

  // TODO: can recursive while-stmt scope happen?
  ROSE_ASSERT  (isSgWhileStmt (result) == NULL);
  ROSE_ASSERT  (isSgIfStmt (result) == NULL);
  return result; 
}
//! Generate a scope tree for a declaration: the tree is trimmed. 
//  To trim the tree , the inner scopes using the variable are removed if there is a use scope which enclosing the inner scopes. 
//  Return the tree, can not be a NULL pointer. At least we return a node for the scope of the declaration
//  Also return the leaf scopes represented by the tree
//  TODO: make it a SageInterface function ?
Scope_Node* generateScopeTree(SgDeclarationStatement* decl, bool debug = false)//std::map <SgScopeStatement*, Scope_Node*>& ScopeTreeMap)
{
  ScopeTreeMap.clear();
//  bool debug = true; // debugging flag
  ROSE_ASSERT (decl != NULL);
  SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl);
  ROSE_ASSERT (var_decl != NULL);
  SgScopeStatement * decl_scope = decl->get_scope();
  ROSE_ASSERT (decl_scope != NULL);

  SgVariableSymbol* var_sym = isSgVariableSymbol(SageInterface::getFirstVarSym(var_decl));
  ROSE_ASSERT (var_sym != NULL);

  // the root of the scope tree
  Scope_Node* scope_tree =new Scope_Node (decl_scope, s_decl);
  scope_tree->depth = 0;
  scope_tree->parent= NULL;
  ScopeTreeMap[decl_scope] = scope_tree; 

  // Step 1. Find all variable references to the declared variable.
  // Note: querySubTree uses pre-order traversal. 
  // This is essential since we want to preserve the order of the scopes within a parent scope.
  // The order of found variable references reflects the order of their scopes in AST. 
  //TODO: optimize for multiple declarations, avoid redundant query
  //   We can batch-generate scope trees for all declarations within a function
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(decl_scope, V_SgVarRefExp);
  std::vector  <SgVarRefExp*> var_refs; 
  bool usedInSameScope = false; // if the declared variable is also used within the same scope
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgVarRefExp *vRef = isSgVarRefExp(*i);
    if (vRef->get_symbol() == var_sym )
    {
      //if (SageInterface::getScope(vRef) == decl_scope) 
      if (getAdjustedScope(vRef) == decl_scope) 
      {
        usedInSameScope = true; 
        break;
      } // same scope
      var_refs.push_back(vRef);
    } // match symbol
  }
  // If there is a single variable use which happens within the declaration scope,
  // we immediately return.
  if (usedInSameScope) 
  { 
    if (debug)
      cout<<"Found a declaration with a varRefExp in the same scope, return a single node scope tree."<<endl;
    return scope_tree; 
  }

  // Step 2: For each reference, generate a scope chain, stored in a temp stack. 
  // Each scope can be either where the variable is declared, used, or a scope in between (intermediate)
  /*
   *    For each reference place
   *    {
   *       back track all its scopes until we reach DS
   *       if DS == US, nothing can be done to move the declaration, stop. 
   *       all scopes tracked are formed into a scope chain (a vector: US .. DS).
   *
   *       create a scope node of use type, called US.
   *       For all scopes in between, create nodes named intermediate scope or IS. 
   *       Add Ds-IS1-IS2..-US (reverse order of the scope chain) into the scope tree, consider consolidating overlapped scope chains;
   *          1) if an intermediate scope is equal to a use scope of another scope chain; stop add the rest of this chain. 
   *          2) if we are adding a use scope into the scope tree, but the same scope is already added by a intermediate scope from anther chain
   *             we mark the  existing scope node as a use scope, and remove its children from the scope tree.
   *             This is hard to do if we store paths explicitly (we have to change all other paths containing the scope being added) 
   *    }
   */
  // There may be multiple variable references within a same scope, 
  // we use a set to record processed scope in order to avoid repetitively considering the same use scopes.
  std::set<SgScopeStatement*> processedUseScopes;
  // For each variable reference, we backtrack its scopes, and add the scope chain into the scope tree of this variable.
  for (size_t i =0; i< var_refs.size(); i++)
  {
    std::stack <SgScopeStatement*> temp_scope_stack;
    SgVarRefExp *vRef = var_refs[i];
    //SgScopeStatement* var_scope = SageInterface::getScope (vRef);
    SgScopeStatement* var_scope = getAdjustedScope(vRef);
    SgScopeStatement * current_scope = var_scope;
    ROSE_ASSERT (current_scope != decl_scope); // we should have excluded this situation already
    temp_scope_stack.push (current_scope) ;  // push the very first bottom scope
    do {
      // this won't work since getScope () will return the input scope as it is!!
      //current_scope = getScope (current_scope);
      current_scope = current_scope->get_scope(); // get parent scope and push
      temp_scope_stack.push (current_scope) ; 
    }
    while (current_scope != decl_scope) ;
    // exit condition is current_scope == decl_scope, as a result  
    // at this point , the declaration scope is already pushed into the stack  

    if (debug)
    {
      cout<<"scope stack size="<<temp_scope_stack.size()<<endl;
    }

    // Step 3: add nodes into the scope tree from the stack.
    //if the current use scope is not yet considered
    // add nodes into the scope tree, avoid duplicated add the scope node containing multiple var references.
    if (processedUseScopes.find(var_scope) == processedUseScopes.end())
    {
      // add each scope into the scope tree
      Scope_Node* current_parent = scope_tree;
      int depth_counter = 0;
      while (!temp_scope_stack.empty())
      { // TODO: verify that the scope tree preserves the original order of children scopes.
        current_scope = temp_scope_stack.top();
        Scope_Node * current_node = NULL; 
        ScopeType s_t; 
        if (current_scope == var_scope) 
          s_t = s_use;
        else 
          s_t = s_intermediate; 
        // avoid add duplicated node into the tree, the first one, root node, is duplicate.
        if (ScopeTreeMap[current_scope] == NULL )
        {
          current_node = new Scope_Node(current_scope, s_t);
          (current_parent->children).push_back(current_node);
          current_node->parent = current_parent;
          current_node->depth = depth_counter;

          ScopeTreeMap[current_scope] = current_node;
        }
        else
        {
          //TODO: optimize 3: we only push scopes which are not yet in the scope tree in to the temp_scope_stack for a new variable use!!
          // This will save overhead of pushing duplicated scopes into stack and later adding them into the scope tree!!
          // Downside: need to calculate depth separately, not using the stack depth. 
          //
          //handle possible overlapped paths, we only want to store the shortest path reaching a s_use node.
          // Further path to deeper use of a variable does not need to be considered. 
          // This is called trimmed path or trimmed tree. 
          //
          // Three conditions
          //1. Reuse the existing scope node, continue next scope in the stack
          //  e.g. the root node and the intermediate nodes shared by multiple scope chains
          // no special handling at all here. 

          //2. Reuse the existing scope node, stop going further
          //  e.g. the existing scope node is already a leaf node (s_use type)
          //  There is no need to go deeper for the current scope chain.
          Scope_Node* existing_node = ScopeTreeMap[current_scope]; 
          if (existing_node->s_type == s_use)
          {
            break; // jump out the while loop for the stack. 
          }  
          //3. Modify the existing scope node's type to be decl, stop g
          //  e.g We are reaching the leaf node of the current scope chain (s_use type)
          //  The existing scope node is s_intermediate. 
          //  we should delete the existing subtree and add the new leaf node
          //  To simplify the implementation, we change type of the existing node to be s_use type and delete all its children nodes. 
          else if (s_t == s_use) 
          {
            ROSE_ASSERT (existing_node->s_type == s_intermediate);
            existing_node->s_type = s_use; 
            existing_node->deep_delete_children(); 
          }  
        } 
        temp_scope_stack.pop();
        // must use the one in the tree, not necessary current_node from the stack
        current_parent = ScopeTreeMap[current_scope]; 
        //         if (current_node != ScopeTreeMap[current_scope])
        //            delete current_node; // delete redundant Scope Node.
        depth_counter++;
      } // end while pop scope stack  

      // mark the current leaf scope as processed.
      processedUseScopes.insert (var_scope);
    } // end if not processed var scope  
  } // end of adding a scope stack into the  scope tree

  //------------- debug the scope tree---------
  if (debug)
  {
    SgFunctionDeclaration* func_decl = SageInterface::getEnclosingFunctionDeclaration (decl);
    std::string func_name = (func_decl->get_name()).getString();
    int decl_lineno = decl->get_file_info()->get_line();
    std::string filename = "scope_tree_"+func_name+"_"+(var_sym->get_name()).getString()+ "_at_"+ StringUtility::numberToString(decl_lineno);
    // report the number of nodes in the scope tree
    std::vector <Scope_Node*> allnodes; 
    scope_tree->traverse_node (allnodes);
    cout<<"Scope tree node count:"<<allnodes.size()<<endl;
    // write the tree into a dot file
    scope_tree->printToDot(filename);
  }
  return scope_tree; 
}  

//! Move a variable declaration from its original scope to a new scope, assuming original scope != target_scope
void SageInterface::moveVariableDeclaration(SgVariableDeclaration* decl, SgScopeStatement* target_scope)
{
  ROSE_ASSERT (decl!= NULL);
  ROSE_ASSERT (target_scope != NULL);
  ROSE_ASSERT (target_scope != decl->get_scope());

  // Move the declaration 
  //TODO: consider another way: copy the declaration, insert the copy, replace varRefExp, and remove (delete) the original declaration 
  SageInterface::removeStatement(decl); 

  switch (target_scope->variantT())
  {
    case V_SgBasicBlock:
      {
        SageInterface::prependStatement (decl, target_scope);
        break;
      }
    case V_SgForStatement: 
      {
        // we move int i; to be for (int i=0; ...);
        SgForStatement* stmt = isSgForStatement (target_scope);
        SgStatementPtrList& stmt_list = stmt->get_init_stmt();
        // Try to match a pattern like for (i=0; ...) here
        // assuming there is only one assignment like i=0
        // We don't yet handle more complex cases
        if (stmt_list.size() !=1)
        {
          cerr<<"Error in moveVariableDeclaration(): only single init statement is handled for SgForStatement now."<<endl;
          ROSE_ASSERT (stmt_list.size() ==1);
        }
        SgExprStatement* exp_stmt = isSgExprStatement(stmt_list[0]);
        ROSE_ASSERT (exp_stmt != NULL);
        SgAssignOp* assign_op = isSgAssignOp(exp_stmt->get_expression());
        ROSE_ASSERT (assign_op != NULL);

        // remove the existing i=0; preserve its right hand operand
        SgExpression * rhs = SageInterface::copyExpression(assign_op->get_rhs_operand());
        stmt_list.clear();
        SageInterface::deepDelete (exp_stmt);

        // modify the decl's rhs to be the new one
        SgInitializedName * init_name = SageInterface::getFirstInitializedName (decl);
        SgAssignInitializer * initor = SageBuilder::buildAssignInitializer (rhs);
        if (init_name->get_initptr() != NULL)
          SageInterface::deepDelete (init_name->get_initptr());
        init_name->set_initptr(initor);
        initor->set_parent(init_name);

        stmt_list.insert (stmt_list.begin(),  decl );
        break;
      }

    default:
      {
        cerr<<"Error. Unhandled target scope type:"<<target_scope->class_name()<<endl;
        ROSE_ASSERT  (false);
      }
  }

#if 1 
  //make sure the symbol is moved also since prependStatement() (in fact fixVariableDeclaration()) does not handle this detail.
  SgVariableSymbol * sym = SageInterface::getFirstVarSym(decl);
  SgScopeStatement* orig_scope = sym->get_scope();
  if (orig_scope != target_scope)
  {
    // SageInterface::fixVariableDeclaration() cannot switch the scope for init name.
    // it somehow always reuses previously associated scope.
    SgInitializedName* init_name = SageInterface::getFirstInitializedName (decl);
    init_name->set_scope(target_scope);
    SgName sname = sym->get_name();
    orig_scope->remove_symbol(sym);
    target_scope->insert_symbol(sname, sym);
  }
  // This is difficult since C++ variables have namespaces
  // Details are in SageInterface::fixVariableDeclaration()
  ROSE_ASSERT (target_scope->symbol_exists(sym));
#endif     

}

//! Replace all variable references to an old symbol in a scope to being references to a new symbol.
// Essentially replace variable a with b. 
void SageInterface::replaceVariableReferences(SgVariableSymbol* old_sym, SgVariableSymbol* new_sym, SgScopeStatement * scope )
{
  ROSE_ASSERT  (old_sym != NULL);
  ROSE_ASSERT  (new_sym != NULL);
  ROSE_ASSERT (old_sym != new_sym);
  ROSE_ASSERT  (scope != NULL);

  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(scope, V_SgVarRefExp);
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgVarRefExp *vRef = isSgVarRefExp(*i);
    if (vRef->get_symbol() == old_sym)
      vRef->set_symbol(new_sym);
  }
}

// Move a single declaration into multiple scopes. 
// For each target scope:
// 1. Copy the decl to be local decl , 
// 2. Insert into the target_scopes, 
// 3. Replace variable references to the new copies
// Finally, erase the original decl 
//
// if the target scope is a For loop && the variable is index variable,  merge the decl to be for( int i=.., ...).
void moveVariableDeclaration(SgVariableDeclaration* decl, std::vector <SgScopeStatement*> scopes)
{
  ROSE_ASSERT (decl!= NULL);
  ROSE_ASSERT (scopes.size() != 0);
  SgInitializedName* i_name = SageInterface::getFirstInitializedName (decl);
  ROSE_ASSERT (i_name != NULL);
  SgVariableSymbol * sym = SageInterface::getFirstVarSym(decl);
  ROSE_ASSERT (sym != NULL);
  SgScopeStatement* orig_scope = sym->get_scope();

  // when we adjust first branch node  (if-stmt with both true and false body )in the scope tree, we may backtrack to the decl's scope 
  // We don't move anything in this case.
  if ((scopes.size()==1) && (scopes[0] == decl->get_scope()))
  {
     return;
  }

  for (size_t i = 0; i< scopes.size(); i++)
  {
    SgScopeStatement* target_scope = scopes[i]; 
    ROSE_ASSERT (target_scope != decl->get_scope());

    SgScopeStatement* adjusted_scope = target_scope; 
    SgVariableDeclaration * decl_copy = SageInterface::deepCopy(decl);

    //bool skip = false; // in some rare case, we skip a target scope, no move to that scope (like while-stmt)
    //This won't work. The move must happen to all scopes or not at all, or dangling variable use without a declaration.
    //We must skip scopes when generating scope tree, not wait until now.

    switch (target_scope->variantT())
    {
      case V_SgBasicBlock:
        {
          SageInterface::prependStatement (decl_copy, adjusted_scope);
          break;
        }
      case V_SgForStatement:
        {
          SgForStatement* stmt = isSgForStatement (target_scope);
          ROSE_ASSERT (stmt != NULL);
          // target scope is a for loop and the declaration declares its index variable.
          if (i_name == SageInterface::getLoopIndexVariable (stmt))
          {
            if (i_name == NULL)
            {
              cerr<<"Warning: in moveVariableDeclaration(): targe_scope is a for loop with unrecognized index variable. Skipping it ..."<<endl;
              break;
            }
            // we move int i; to be for (int i=0; ...);
            SgStatementPtrList& stmt_list = stmt->get_init_stmt();
            // Try to match a pattern like for (i=0; ...) here
            // assuming there is only one assignment like i=0
            // We don't yet handle more complex cases
            if (stmt_list.size() !=1)
            {
              cerr<<"Error in moveVariableDeclaration(): only single init statement is handled for SgForStatement now."<<endl;
              ROSE_ASSERT (stmt_list.size() ==1);
            }
            SgExprStatement* exp_stmt = isSgExprStatement(stmt_list[0]);
            ROSE_ASSERT (exp_stmt != NULL);
            SgAssignOp* assign_op = isSgAssignOp(exp_stmt->get_expression());
            // TODO: it can be SgCommanOpExp
            if (isSgCommaOpExp (exp_stmt->get_expression()) )
            {
               cerr<<"Error in moveVariableDeclaration(), multiple expressions in for-condition is not supported now. "<<endl;
               ROSE_ASSERT (assign_op != NULL);
            } 
            ROSE_ASSERT (assign_op != NULL);

            // remove the existing i=0; preserve its right hand operand
            SgExpression * rhs = SageInterface::copyExpression(assign_op->get_rhs_operand());
            stmt_list.clear();
            SageInterface::deepDelete (exp_stmt);

            // modify the decl's rhs to be the new one
            SgInitializedName * copy_name = SageInterface::getFirstInitializedName (decl_copy);
            SgAssignInitializer * initor = SageBuilder::buildAssignInitializer (rhs);
            if (copy_name->get_initptr() != NULL)
              SageInterface::deepDelete (copy_name->get_initptr());
            copy_name->set_initptr(initor);
            initor->set_parent(copy_name);
            // insert the merged decl into the list, TODO preserve the order in the list
            stmt_list.insert (stmt_list.begin(),  decl_copy);
            decl_copy->set_parent(stmt->get_for_init_stmt());
            ROSE_ASSERT (decl_copy->get_parent() != NULL); 
          } //
          else 
          {
            SgBasicBlock* loop_body = SageInterface::ensureBasicBlockAsBodyOfFor (stmt);
            adjusted_scope = loop_body;
            SageInterface::prependStatement (decl_copy, adjusted_scope);
          }
          break;
        }
      /*
       *We don't try to merge a variable decl into the conditional of a while statement
       * The reason is that often the declaration has an initializer , which must be preserved.
       * The conditional contain the use of the declared variable, which cannot be merged with the declaration.
       * e.g.  int yy = 10; while (yy< 100) { yy ++; ... }
       * Another tricky thing is even though yy shows up in both condition and while-body, 
       * the scope tree will only show while-stmt as the single scope node since the body scope is shadowed. 
       * Note: the scope of the condition is considered to be the while-stmt.
       * */  
      case V_SgWhileStmt: 
          cerr<<"while statement @ line"<< target_scope->get_file_info()->get_line()<< " should not show up in scope tree"<<endl;
          ROSE_ASSERT (false);
          break;
      default:
        {
          cerr<<"Error. Unhandled target scope type:"<<target_scope->class_name()
            << " when moving decl at:"<< decl->get_file_info()->get_line()
            << " for variable "<< sym->get_name()
            << " to scope at:"<< target_scope->get_file_info()->get_line()<<endl;
          ROSE_ASSERT  (false);
        }
    }
#if 1 
    // check what is exactly copied:
    // Symbol is not copies. It is shared instead
    SgVariableSymbol * new_sym = SageInterface::getFirstVarSym (decl_copy);

    // init name is copied, but its scope is not changed!
    // but the symbol cannot be find by calling init_name->get_symbol_from_symbol_table ()
    SgInitializedName* init_name_copy = SageInterface::getFirstInitializedName (decl_copy);
    init_name_copy->set_scope (adjusted_scope);

    //ROSE_ASSERT (false);
    if (orig_scope != adjusted_scope)
    {
      // SageInterface::fixVariableDeclaration() cannot switch the scope for init name.
      // it somehow always reuses previously associated scope.
      ROSE_ASSERT (i_name != init_name_copy);
      // we have to manually copy the symbol and insert it
      SgName sname = sym->get_name();
      adjusted_scope->insert_symbol(sname, new SgVariableSymbol (init_name_copy));
    }
    new_sym = SageInterface::getFirstVarSym (decl_copy);
    ROSE_ASSERT (sym!=new_sym);
    // This is difficult since C++ variables have namespaces
    // Details are in SageInterface::fixVariableDeclaration()
    ROSE_ASSERT (adjusted_scope->symbol_exists(new_sym));
#endif     

#if 1 
    // replace variable references
    SageInterface::replaceVariableReferences  (sym, new_sym, adjusted_scope);
#endif 
  } //end for all scopes

  // remove the original declaration , must use false to turn of auto-relocate comments
  // TODO: investigate why default behavior adds two extra comments in the end of the code
  SageInterface::removeStatement(decl, false);
  //TODO deepDelete is problematic
  //SageInterface::deepDelete(decl);  // symbol is not deleted?
  //orig_scope->remove_symbol(sym);
  //delete i_name;
}

//! Check if a variable (symbol) is live in for a scope
// TODO A real solution is to check liveness analysis result here. However, the analysis is not ready for production use.
// The current workaround is to use a syntactic analysis: 
//
//    The opposite of dead:  !dead, not first defined
//    check if the first reference (in a pre-order traversal) to the variable is a definition (write access) only, exclude a reference which is both read and write, like a+=1; 
//    first defined means the variable is the lhs of a= rhs_exp; and it does not show up on rhs_exp; 
static bool isLiveIn(SgVariableSymbol* var_sym, SgScopeStatement* scope)
{
  SgVarRefExp * first_ref = NULL; 
  Rose_STL_Container <SgNode*> testList = NodeQuery::querySubTree (scope, V_SgVarRefExp);
  for (size_t i=0; i< testList.size(); i++)
  {
    SgVarRefExp * current_exp = isSgVarRefExp (testList[i]);
    ROSE_ASSERT (current_exp != NULL);
    if (current_exp->get_symbol() == var_sym)
    {
      first_ref = current_exp; 
      break;
    }
  } // end for search

  // No reference at all?  Not liveIn
  if (first_ref == NULL) return false; 

  // Now go through common cases for finding definitions. 
  // 1. We don't care about declarations since the variables will be different from var_sym!
  // 2. For unary operations: ++ or --,  they must read the variables first then change the values
  // 3. Binary operations: only SgAssignOp does not read lhs oprand (define only )
  // 4. Function call parameters: addressOf(a): cannot move, is live

  // if a = rhs_exp;  and a does not show up in rhs_exp; 
  // then it is not live in (return false).
// TODO : handle casting
// TODO: handle rare case like (t=i)=8; 
  SgNode* parent = first_ref ->get_parent();
  ROSE_ASSERT (parent != NULL);
  if ( (isSgAssignOp(parent) && isSgAssignOp(parent)->get_lhs_operand()==first_ref) )
  {
    // check if it shows up on the rhs
    bool onRhs = false; 
    Rose_STL_Container <SgNode*> rhs_vars = NodeQuery::querySubTree (isSgAssignOp(parent)->get_rhs_operand(), V_SgVarRefExp);
    for (size_t i=0; i< rhs_vars.size(); i++)
    {
      SgVarRefExp * current_var = isSgVarRefExp (rhs_vars[i]);
      ROSE_ASSERT (current_var != NULL);
      if (current_var->get_symbol() == var_sym)
      {
        onRhs = true;
        break;
      }
    } // end for search
    if (!onRhs ) return false; 
  }

  //All other cases, we conservatively assume the variable is live in for the scope
  return true; 
}

bool SageInterface::moveDeclarationToInnermostScope(SgDeclarationStatement* declaration, bool debug = false)
{
  SgVariableDeclaration * decl = isSgVariableDeclaration(declaration);
  ROSE_ASSERT (decl != NULL);
  // Step 1: generate a scope tree for the declaration
  // -----------------------------------------------------
  Scope_Node* scope_tree = generateScopeTree (decl, debug);

  // single node scope tree, nowhere to move into. 
  if ((scope_tree->children).size() == 0 )
  {
    scope_tree->deep_delete_children ();
    delete scope_tree;
    return false; 
  }

  // for a scope tree with two or more nodes  
  Scope_Node* first_branch_node = scope_tree->findFirstBranchNode();

  // the scopes to be moved into
  std::vector <SgScopeStatement *> target_scopes; 
  // Step 2: simplest case, only a single use place
  // -----------------------------------------------------
  // the first branch node is also the bottom node
  if ((first_branch_node->children).size() ==0)
  {
    SgScopeStatement* bottom_scope = first_branch_node->scope;
    target_scopes.push_back(bottom_scope);
  } // end the single decl-use path case
  else 
  { 
    //Step 3: multiple scopes
    // -----------------------------------------------------
    // there are multiple (0 to n - 1 )child scopes in which the variable is used. 
    // if for all scope 1, 2, .., n-1
    //  the variable is defined before being used (not live)
    //  Then we can move the variable into each child scope
    // Conversely, if any of scope has liveIn () for the declared variable, we cannot move
    bool moveToMultipleScopes= true ; 

    for (size_t i =1; i< (first_branch_node->children).size(); i++)
    {
      SgVariableSymbol * var_sym = SageInterface::getFirstVarSym (decl); 
      ROSE_ASSERT (var_sym != NULL);
      SgScopeStatement * current_child_scope = (first_branch_node->children[i])->scope;
      ROSE_ASSERT (current_child_scope != NULL); 
      if (isLiveIn (var_sym, current_child_scope))
        moveToMultipleScopes = false;
    }  // end for all scopes

    if (moveToMultipleScopes)
    {
      if (debug)
        cout<<"Found a movable declaration for multiple child scopes"<<endl;
      for (size_t i =0; i< (first_branch_node->children).size(); i++)
      {
        // we try to get the bottom for each branch, not just the upper scope
        // This is good for the case like: "if () { for (i=0;..) {}}" and if-stmt's scope is a child of the first branch scope node
        // TODO: one branch may fork multiple branches. Should we move further down on each grandchildren branch?
        //       Not really, we then find the common inner most scope of that branch. just simply move decl there!
        // Another thought: A better fix: we collect all leaf nodes of the scope tree! It has nothing to do with the first branch node!
        //       this won't work. First branch node still matters. 
        Scope_Node* current_child_scope = first_branch_node->children[i];     
        Scope_Node* bottom_node = current_child_scope -> findFirstBranchNode ();
        SgScopeStatement * bottom_scope = bottom_node->scope;
        ROSE_ASSERT (bottom_scope!= NULL);
        target_scopes.push_back (bottom_scope);
      }
    }
    else // we still can move it to the innermost common scope
    {
      SgScopeStatement* bottom_scope = first_branch_node->scope;
      if (decl->get_scope() != bottom_scope)
      {
        target_scopes.push_back(bottom_scope);
      }
    } // end else
  } // end else multiple scopes

  if (target_scopes.size()>0)
  {
    moveVariableDeclaration (decl, target_scopes);
    scope_tree->deep_delete_children ();
    delete scope_tree;
    return true;
  }
  else 
  {
    scope_tree->deep_delete_children ();
    delete scope_tree;
    return false;  
  }
}


class SimpleExpressionEvaluator2: public AstBottomUpProcessing <struct SageInterface::const_numeric_expr_t> {
 public:
   SimpleExpressionEvaluator2() {
   }

 
 struct SageInterface::const_numeric_expr_t getValueExpressionValue(SgValueExp *valExp) {
   
    struct SageInterface::const_numeric_expr_t subtreeVal;
    subtreeVal.isIntOnly_ = true;

   if (isSgIntVal(valExp)) {
     subtreeVal.value_ = isSgIntVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgLongIntVal(valExp)) {
     subtreeVal.value_ = isSgLongIntVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgLongLongIntVal(valExp)) {
     subtreeVal.value_ = isSgLongLongIntVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgShortVal(valExp)) {
     subtreeVal.value_ = isSgShortVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgUnsignedIntVal(valExp)) {
     subtreeVal.value_ = isSgUnsignedIntVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgUnsignedLongVal(valExp)) {
     subtreeVal.value_ = isSgUnsignedLongVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgUnsignedLongLongIntVal(valExp)) {
     subtreeVal.value_ = isSgUnsignedLongLongIntVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgUnsignedShortVal(valExp)) {
     subtreeVal.value_ = isSgUnsignedShortVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgDoubleVal(valExp)) {
     subtreeVal.value_ = isSgDoubleVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
     subtreeVal.isIntOnly_ = false;
   } else if (isSgFloatVal(valExp)){
     subtreeVal.value_ = isSgFloatVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
     subtreeVal.isIntOnly_ = false;
   } else if (isSgLongDoubleVal(valExp)){
     subtreeVal.value_ = isSgDoubleVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
     subtreeVal.isIntOnly_ = false;
   }
   return subtreeVal;
 }

 
 struct SageInterface::const_numeric_expr_t evaluateVariableReference(SgVarRefExp *vRef) {
   if (isSgModifierType(vRef->get_type()) == NULL) {
     struct SageInterface::const_numeric_expr_t val;
     val.value_ = -1;
     val.hasValue_ = false;
     return val;
   }
   if (isSgModifierType(vRef->get_type())->get_typeModifier().get_constVolatileModifier().isConst()) {
     // We know that the var value is const, so get the initialized name and evaluate it
     SgVariableSymbol *sym = vRef->get_symbol();
     SgInitializedName *iName = sym->get_declaration();
     SgInitializer *ini = iName->get_initializer();
                                                                                 
     if (isSgAssignInitializer(ini)) {
       SgAssignInitializer *initializer = isSgAssignInitializer(ini);
       SgExpression *rhs = initializer->get_operand();
       SimpleExpressionEvaluator2 variableEval;
                                                                                                                
       return variableEval.traverse(rhs);
     }
   }
   struct SageInterface::const_numeric_expr_t val;
   val.hasValue_ = false;
   val.value_ = -1;
   return val;
 }

 
 struct SageInterface::const_numeric_expr_t evaluateSynthesizedAttribute(SgNode *node, SynthesizedAttributesList synList) {
   if (isSgExpression(node)) {
     if (isSgValueExp(node)) {
       return this->getValueExpressionValue(isSgValueExp(node));
     }
                                                                                                                                                 
     if (isSgVarRefExp(node)) {
      //      std::cout << "Hit variable reference expression!" << std::endl;
       return evaluateVariableReference(isSgVarRefExp(node));
     }
     // Early break out for assign initializer // other possibility?
     if (isSgAssignInitializer(node)) {
       if(synList.at(0).hasValue_){
         return synList.at(0);
       } else { 
         struct SageInterface::const_numeric_expr_t val;
         val.value_ = -1;
         val.hasValue_ = false;
         return val;
       }
     }
     struct SageInterface::const_numeric_expr_t evaluatedValue;
     evaluatedValue.hasValue_ = false;
     evaluatedValue.value_ = -1;
     evaluatedValue.isIntOnly_ = false;
     if(isSgBinaryOp(node)){
       if((synList[0].isIntOnly_ == true) && (synList[1].isIntOnly_ == true)){
         evaluatedValue.isIntOnly_ = true;
       }
       // JP The += -= /= operator don't make sense, since we restrict ourselves to const marked variables.
       if(synList[0].hasValue_ && synList[1].hasValue_){
         if (isSgAddOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_ + synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgSubtractOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_  - synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgMultiplyOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_  * synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgDivideOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_  / synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgModOp(node)) {
           assert(synList.size() == 2);
           if (synList[0].isIntOnly_ && synList[1].isIntOnly_){
             long a = synList[0].value_;
             long b = synList[1].value_;
             evaluatedValue.value_ = a % b;
             evaluatedValue.isIntOnly_ = true;
           } else {
             std::cerr << "Wrong arguments to modulo operator." << std::endl;
           }
           evaluatedValue.hasValue_ = true;
         }
       } else {
         std::cerr << "Expression is not evaluatable" << std::endl;
         evaluatedValue.hasValue_ = false;
         evaluatedValue.value_ = -1;
         return evaluatedValue;
       }
     } else if(isSgUnaryOp(node)){
       evaluatedValue.isIntOnly_ = synList[0].isIntOnly_;
       if(synList[0].hasValue_ == true){
         if(isSgMinusOp(node)){
           evaluatedValue.value_ = (-1) * synList[0].value_;
           evaluatedValue.hasValue_ = true;
         } else if (isSgUnaryAddOp(node)){
           evaluatedValue.value_ = + synList[0].value_;
           evaluatedValue.hasValue_ = true;
         } else if (isSgCastExp(node)){
           evaluatedValue.hasValue_ = false;
           evaluatedValue.value_ = synList[0].value_;
         }
       } else {
         std::cerr << "Expression is not evaluatable" << std::endl;
         evaluatedValue.hasValue_ = false;
         evaluatedValue.value_ = -1;
         return evaluatedValue;
       }
     }
//     evaluatedValue.hasValue_ = true;
     return evaluatedValue;
   }
   struct SageInterface::const_numeric_expr_t evaluatedValue;
   evaluatedValue.hasValue_ = false;
   evaluatedValue.value_ = -1;
   return evaluatedValue;
 }
};

struct SageInterface::const_numeric_expr_t 
SageInterface::evaluateConstNumericExpression(SgExpression *expr){
  SimpleExpressionEvaluator2 eval;
  return eval.traverse(expr);
}


/////
class SimpleExpressionEvaluator: public AstBottomUpProcessing <struct SageInterface::const_int_expr_t> {
 public:
   SimpleExpressionEvaluator() {
   }

 struct SageInterface::const_int_expr_t getValueExpressionValue(SgValueExp *valExp) {
   struct SageInterface::const_int_expr_t subtreeVal_i;
   subtreeVal_i.hasValue_ = false;

   if (isSgIntVal(valExp)) {
     subtreeVal_i.value_ = isSgIntVal(valExp)->get_value();
     subtreeVal_i.hasValue_ = true;
   } else if (isSgLongIntVal(valExp)) {
     subtreeVal_i.value_ = isSgLongIntVal(valExp)->get_value();
     subtreeVal_i.hasValue_ = true;
   } else if (isSgLongLongIntVal(valExp)) {
     subtreeVal_i.value_ = isSgLongLongIntVal(valExp)->get_value();
     subtreeVal_i.hasValue_ = true;
   } else if (isSgShortVal(valExp)) {
     subtreeVal_i.value_ = isSgShortVal(valExp)->get_value();
     subtreeVal_i.hasValue_ = true;
   } else if (isSgUnsignedIntVal(valExp)) {
     subtreeVal_i.value_ = isSgUnsignedIntVal(valExp)->get_value();
     subtreeVal_i.hasValue_ = true;
   } else if (isSgUnsignedLongVal(valExp)) {
     subtreeVal_i.value_ = isSgUnsignedLongVal(valExp)->get_value();
     subtreeVal_i.hasValue_ = true;
   } else if (isSgUnsignedLongLongIntVal(valExp)) {
     subtreeVal_i.value_ = isSgUnsignedLongLongIntVal(valExp)->get_value();
     subtreeVal_i.hasValue_ = true;
   } else if (isSgUnsignedShortVal(valExp)) {
     subtreeVal_i.value_ = isSgUnsignedShortVal(valExp)->get_value();
     subtreeVal_i.hasValue_ = true;
   } 
   return subtreeVal_i;
 }

 struct SageInterface::const_int_expr_t evaluateVariableReference(SgVarRefExp *vRef) {
   if (isSgModifierType(vRef->get_type()) == NULL) {
     struct SageInterface::const_int_expr_t val;
     val.value_ = -1;
     val.hasValue_ = false;
     return val;
   }
   if (isSgModifierType(vRef->get_type())->get_typeModifier().get_constVolatileModifier().isConst()) {
     // We know that the var value is const, so get the initialized name and evaluate it
     SgVariableSymbol *sym = vRef->get_symbol();
     SgInitializedName *iName = sym->get_declaration();
     SgInitializer *ini = iName->get_initializer();
                                                                                 
     if (isSgAssignInitializer(ini)) {
       SgAssignInitializer *initializer = isSgAssignInitializer(ini);
       SgExpression *rhs = initializer->get_operand();
       SimpleExpressionEvaluator variableEval;
                                                                                                                
       return variableEval.traverse(rhs);
     }
   }
   struct SageInterface::const_int_expr_t val;
   val.hasValue_ = false;
   val.value_ = -1;
   return val;
 }

 struct SageInterface::const_int_expr_t evaluateSynthesizedAttribute(SgNode *node, SynthesizedAttributesList synList) {
   if (isSgExpression(node)) {
     if (isSgValueExp(node)) {
       return this->getValueExpressionValue(isSgValueExp(node));
     }
                                                                                                                                                 
     if (isSgVarRefExp(node)) {
      //      std::cout << "Hit variable reference expression!" << std::endl;
       return evaluateVariableReference(isSgVarRefExp(node));
     }
     // Early break out for assign initializer // other possibility?
     if (isSgAssignInitializer(node)) {
       if(synList.at(0).hasValue_){
         return synList.at(0);
       } else { 
         struct SageInterface::const_int_expr_t val;
         val.value_ = -1;
         val.hasValue_ = false;
         return val;
       }
     }
     struct SageInterface::const_int_expr_t evaluatedValue;
     evaluatedValue.hasValue_ = false;
     evaluatedValue.value_ = -1;
#if 0
    if(synList.size() != 2){
      for(SynthesizedAttributesList::iterator it = synList.begin(); it != synList.end(); ++it){
        std::cout << "Node: " << node->unparseToString() << "\n" << (*it).value_ << std::endl;
        std::cout << "Parent: " << node->get_parent()->unparseToString() << std::endl;
        std::cout << "Parent, Parent: " << node->get_parent()->get_parent()->unparseToString() << std::endl;
      }
    }
#endif
     for (SynthesizedAttributesList::iterator it = synList.begin(); it != synList.end(); ++it) {
       if((*it).hasValue_){
         if (isSgAddOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_ + synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgSubtractOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_  - synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgMultiplyOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_  * synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgDivideOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_  / synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgModOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_  % synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         }
       } else {
         std::cerr << "Expression is not evaluatable" << std::endl;
         evaluatedValue.hasValue_ = false;
         evaluatedValue.value_ = -1;
         return evaluatedValue;
       }
     }
     evaluatedValue.hasValue_ = true;
     return evaluatedValue;
   }
   struct SageInterface::const_int_expr_t evaluatedValue;
   evaluatedValue.hasValue_ = false;
   evaluatedValue.value_ = -1;
   return evaluatedValue;
 }
};

struct SageInterface::const_int_expr_t 
SageInterface::evaluateConstIntegerExpression(SgExpression *expr){
  SimpleExpressionEvaluator eval;
  return eval.traverse(expr);
}

bool
SageInterface::checkTypesAreEqual(SgType *typeA, SgType *typeB){

  class TypeEquivalenceChecker {
    public:
     TypeEquivalenceChecker(bool profile, bool useSemanticEquivalence)
       : profile_(profile), useSemanticEquivalence_(useSemanticEquivalence),
         namedType_(0), pointerType_(0), arrayType_(0), functionType_(0) 
     {
     }

     SgNode * getBasetypeIfApplicable(SgNode *t){
       SgNode * node = t;
       if (isSgTypedefType(t)) {
//    std::cout << "This is a typedef nodeT1. We strip everything away and compare the hidden types." << std::endl;
         node = isSgTypedefType(t)->stripType(SgType::STRIP_TYPEDEF_TYPE);
     }
     if(useSemanticEquivalence_){
       if(isSgModifierType(t)){
         SgModifierType *modType = isSgModifierType(t);
         ROSE_ASSERT(modType != NULL);
         // We need to check for Volatile/Restrict types. These are modelled as ModifierTypes, but are equal (in some cases)
         // volatile seems to make no difference for basic (built in) types like int, bool etc. But it has an impact on types
         // like classes
         // restrict seems to have no impact on the type itself.
         if(SageInterface::isVolatileType(modType)){
          // handle volatile case
          std::cout << "Hit volatile type, stripping of modifier type" << std::endl;
          node = modType->get_base_type();
         }
      if(SageInterface::isRestrictType(modType)){
        // handle restrict case
        std::cout << "Hit restrict type, stripping of modifier type" << std::endl;
        node = modType->get_base_type();
      }
    }
  }
  ROSE_ASSERT(node != NULL);
  return node;
}

bool typesAreEqual(SgType *t1, SgType *t2) {
  bool equal = false;
  if(t1 == NULL || t2 == NULL){
    std::string wasNull;
    if(t1 == NULL){
      wasNull = "t1";
    } else {
      wasNull = "t2";
    }
    std::cerr << "ERROR: " << wasNull << " was NULL" << std::endl;
    return equal;
  }
  // if both pointers point to same location the types MUST be equal!
  if(t1 == t2){
//    std::cout << "Pointers are equal, returning true" << std::endl;
    return true;
  }
#ifndef USE_CMAKE
  RoseAst subT1(t1);
  RoseAst subT2(t2);

  for (RoseAst::iterator i = subT1.begin(), j = subT2.begin();
       i != subT1.end(), j != subT2.end(); ++i, ++j) {
    SgNode *nodeT1 = *i;
    SgNode *nodeT2 = *j;

//    std::cout << "nodeT1: " << nodeT1->class_name() << " nodeT2: " << nodeT2->class_name() << std::endl;
   nodeT1 = getBasetypeIfApplicable(nodeT1);
   nodeT2 = getBasetypeIfApplicable(nodeT2);

   if (nodeT1->variantT() == nodeT2->variantT()) {
//     std::cout << "variantT is the same" << std::endl;
      if(isSgModifierType(nodeT1)){
        // we need to check whether the modifier is the same or not
        SgTypeModifier modT1 = isSgModifierType(nodeT1)->get_typeModifier();
        SgTypeModifier modT2 = isSgModifierType(nodeT2)->get_typeModifier();
        if(modT1.get_constVolatileModifier().isConst() != modT2.get_constVolatileModifier().isConst()){
          return false;
        }
        if(modT1.get_constVolatileModifier().isVolatile() != modT2.get_constVolatileModifier().isVolatile()){
          return false;
        }
      } else if (isSgNamedType(nodeT1)) {      // Two different names -> Must be two different things
        if (profile_) {
          namedType_++;
        }
        i.skipChildrenOnForward();
        j.skipChildrenOnForward();
        SgNamedType *c1 = isSgNamedType(nodeT1);
        SgNamedType *c2 = isSgNamedType(nodeT2);

//        std::cout << c1->get_qualified_name() << std::endl;
        // XXX A function to check whether a named type is anonymous or not would speed
        // up this check, since we could get rid of this string compare.
//        if (c1->get_qualified_name().getString().find("__anonymous_") != std::string::npos) {
        if(!c1->get_autonomous_declaration()){
          return false;
        }
        if (!c2->get_autonomous_declaration()){
          return false;
        }
        if (c1->get_qualified_name() == c2->get_qualified_name()) {
          return true;
        } else {
          return false;
        }

      } else if (isSgPointerType(nodeT1)) {
        if (profile_) {
          pointerType_++;
        }
        SgPointerType *t1 = isSgPointerType(nodeT1);
        SgPointerType *t2 = isSgPointerType(nodeT2);

        return typesAreEqual(t1->get_base_type(), t2->get_base_type());

      } else if(isSgReferenceType(nodeT1)){
        SgReferenceType *t1 = isSgReferenceType(nodeT1);
        SgReferenceType *t2 = isSgReferenceType(nodeT2);

        return typesAreEqual(t1->get_base_type(), t2->get_base_type());
      } else if (isSgArrayType(nodeT1)) {
        if (profile_) {
          arrayType_++;
        }
        SgArrayType *a1 = isSgArrayType(nodeT1);
        SgArrayType *a2 = isSgArrayType(nodeT2);

        bool arrayBaseIsEqual = typesAreEqual(a1->get_base_type(), a2->get_base_type());

        SageInterface::const_int_expr_t t1Index = SageInterface::evaluateConstIntegerExpression(a1->get_index());
        SageInterface::const_int_expr_t t2Index = SageInterface::evaluateConstIntegerExpression(a2->get_index());
        bool arrayIndexExpressionIsEquivalent = false;
        if(t1Index.hasValue_ && t2Index.hasValue_){
          if(t1Index.value_ == t2Index.value_){
            arrayIndexExpressionIsEquivalent = true;
          }
        }
        bool arraysAreEqual = (arrayBaseIsEqual && arrayIndexExpressionIsEquivalent);
        return arraysAreEqual;
      } else if (isSgFunctionType(nodeT1)) {
        if(profile_) {
          functionType_++;
        }
        SgFunctionType *funcTypeA = isSgFunctionType(nodeT1);
        SgFunctionType *funcTypeB = isSgFunctionType(nodeT2);
//        std::cout << "Inside SgFunctionType" << std::endl;
//        assert(funcTypeA != funcTypeB);
        if(typesAreEqual(funcTypeA->get_return_type(), funcTypeB->get_return_type())) {
          // If functions don't have the same number of arguments, they are not type-equal
          if(funcTypeA->get_arguments().size() != funcTypeB->get_arguments().size()) {
            return false;
          }
          // This should always be the same as the if before...
          if(funcTypeA->get_argument_list()->get_arguments().size() != funcTypeB->get_argument_list()->get_arguments().size()){
            return false;
          }

          for(SgTypePtrList::const_iterator ii = funcTypeA->get_arguments().begin(),
              jj = funcTypeB->get_arguments().begin();
              ii != funcTypeA->get_arguments().end(),
              jj != funcTypeB->get_arguments().end();
              ++ii, ++jj) {
//            std::cout << (*ii)->class_name() << " " << (*jj)->class_name() << std::endl;
            // For all argument types check whether they are equal
            if(!typesAreEqual((*ii), (*jj))) {
              return false;
            }
          }
          return true;
        }
        return false;
      } else {
        // We don't have a named type, pointer type or array type, so they are equal
        // This is for the primitive type - case
        return true;
      }
    } else {
      // In this case the types are not equal, since its variantT is not equal.
      return false;
    }
  }
  // this should be unreachable code...
  return equal;
#else
  std::cerr << "This feature for now is available with autotools only!" << std::endl;
  ROSE_ASSERT(false);
  return false;
#endif
}

int getNamedTypeCount() {
  return namedType_;
}

int getPointerTypeCount() {
  return pointerType_;
}

int getArrayTypeCount() {
  return arrayType_;
}

int getFunctionTypeCount() {
  return functionType_;
}
    private:
//     SgNode * getBasetypeIfApplicable(SgNode *t);
     bool profile_, useSemanticEquivalence_;
     int namedType_, pointerType_, arrayType_, functionType_;
};

TypeEquivalenceChecker tec(false, false);
return tec.typesAreEqual(typeA, typeB);
}

