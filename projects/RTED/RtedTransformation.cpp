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
void RtedTransformation::loadFunctionSymbols(SgSourceFile& n, SourceFileType sft)
{
   SgGlobal* globalScope = n.get_globalScope();
   ROSE_ASSERT(globalScope);

   symbols.initialize(*globalScope, sft);
}

/* -----------------------------------------------------------
 * Perform all transformations needed
 * -----------------------------------------------------------*/
void RtedTransformation::transform(SgProject* project)
{
   using rted::VariableTraversal;
   using rted::InheritedAttribute;

   ROSE_ASSERT(project);

   if (RTEDDEBUG) std::cout << "Running Transformation..." << std::endl;

   VariableTraversal                varTraversal(this);

   // Call the traversal starting at the project (root) node of the AST
   varTraversal.traverseInputFiles(project, InheritedAttribute());

   // tps: Traverse all classes that appear in header files and create copy in
   // source file within a namespace We need to know the sizeOf classes. To do
   // so we need to modify the class but do not want to do this in the header
   // file right now.

   insertNamespaceIntoSourceFile(project);
   executeTransformations();
}


/* -----------------------------------------------------------
 * Collects information needed for transformations
 * -----------------------------------------------------------*/

#endif

// vim:et sta ts=2 sw=2:
