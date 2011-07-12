#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <set>

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"
#include "RtedVisit.h"

using namespace SageInterface;
using namespace SageBuilder;

/* -----------------------------------------------------------
 * Run frontend and return project
 * -----------------------------------------------------------*/
SgProject*
RtedTransformation::parse(int argc, char** argv) {
   SgProject* project = frontend(argc, argv);
   ROSE_ASSERT(project);
   return project;
}

/* -----------------------------------------------------------
 * Load all function symbols that are needed for transformations
 * Basically we need to know which functions to insert
 * -----------------------------------------------------------*/
void RtedTransformation::loadFunctionSymbols(SgSourceFile& n)
{
   SgGlobal* globalScope = n.get_globalScope();
   ROSE_ASSERT(globalScope);

   symbols.initialize(*globalScope);

   ROSE_ASSERT(symbols.roseCreateArray);
   ROSE_ASSERT(symbols.roseAllocMem);
   ROSE_ASSERT(symbols.roseAccessArray);
   ROSE_ASSERT(symbols.roseCheckpoint);
   ROSE_ASSERT(symbols.roseFunctionCall);
   ROSE_ASSERT(symbols.roseAssertFunctionSignature);
   ROSE_ASSERT(symbols.roseConfirmFunctionSignature);
   ROSE_ASSERT(symbols.roseFreeMemory);
   ROSE_ASSERT(symbols.roseReallocateMemory);
   ROSE_ASSERT(symbols.roseConvertIntToString);
   ROSE_ASSERT(symbols.roseCreateVariable);
   ROSE_ASSERT(symbols.roseCreateObject);
   ROSE_ASSERT(symbols.roseInitVariable);
   ROSE_ASSERT(symbols.roseMovePointer);
   ROSE_ASSERT(symbols.roseAccessVariable);
   ROSE_ASSERT(symbols.roseEnterScope);
   ROSE_ASSERT(symbols.roseExitScope);
   ROSE_ASSERT(symbols.roseIOFunctionCall);
   ROSE_ASSERT(symbols.roseRegisterTypeCall);
   ROSE_ASSERT(symbols.roseCheckIfThisNULL);
   ROSE_ASSERT(symbols.roseAddr);
   ROSE_ASSERT(symbols.roseClose);

   if (withupc)
   {
     /* only present when compiling for UPC code */
     ROSE_ASSERT(symbols.roseAddrSh);
     ROSE_ASSERT(symbols.roseUpcExitWorkzone);
     ROSE_ASSERT(symbols.roseUpcEnterWorkzone);
     ROSE_ASSERT(symbols.roseUpcBeginExclusive);
     ROSE_ASSERT(symbols.roseUpcEndExclusive);
   }

   ROSE_ASSERT(symbols.roseUpcAllInitialize); // see comment in ParallelRTS.h

   ROSE_ASSERT(symbols.roseAllocKind);

   ROSE_ASSERT(symbols.roseTypeDesc);
   ROSE_ASSERT(symbols.roseAddressDesc);
   ROSE_ASSERT(symbols.roseSourceInfo);
   ROSE_ASSERT(symbols.size_t_member);
}

/* -----------------------------------------------------------
 * Perform all transformations needed
 * -----------------------------------------------------------*/
void RtedTransformation::transform(SgProject* project, std::set<std::string>& rtedfiles)
{
   using rted::VariableTraversal;
   using rted::InheritedAttribute;

   ROSE_ASSERT(project);

   if (RTEDDEBUG) std::cout << "Running Transformation..." << std::endl;

   this->rtedfiles = &rtedfiles;

   VariableTraversal                varTraversal(this);

   // Call the traversal starting at the project (root) node of the AST
   varTraversal.traverseInputFiles(project, InheritedAttribute());

   // tps: Traverse all classes that appear in header files and create copy in
   // source file within a namespace We need to know the sizeOf classes. To do
   // so we need to modify the class but do not want to do this in the header
   // file right now.
   // \pp \note can the loop be done as part of varTraversal, or seperated into
   //           another class? This way we could eliminate the dependency
   //           between Rtedtransformation and AstSimpleProcessing.

   insertNamespaceIntoSourceFile(project);

   executeTransformations();
}


/* -----------------------------------------------------------
 * Collects information needed for transformations
 * -----------------------------------------------------------*/

#endif

// vim:et sta ts=2 sw=2:
