#include "valueGraph.h"
#include <slicing/backstrokeCFG.h>

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH

int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  SgSourceFile* sourceFile = isSgSourceFile((*project)[0]);

  // Process all function definition bodies for static control flow graph generation
  Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
  for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
  {
    SgFunctionDefinition* funcDef = isSgFunctionDefinition(*i);
    ROSE_ASSERT (funcDef != NULL);

	if (!funcDef->get_file_info()->isSameFile(sourceFile))
		continue;

    Backstroke::BackstrokeCFG cfg(funcDef);
	cfg.toDot("CFG.dot");

    // Prepend includes to test files.
    SgGlobal* globalScope = SageInterface::getFirstGlobalScope(project);
	SageInterface::insertHeader("rctypes.h", PreprocessingInfo::after, false, globalScope);
    
	Backstroke::EventReverser reverser(funcDef);
	reverser.buildValueGraph();
//	//reverser.searchValueGraph();
//	reverser.shortestPath();
//	reverser.buildForwardAndReverseEvent();
//	reverser.getPath();

//	Backstroke::ValueGraph reverseVg;
//	// The following function makes a reverse CFG copy.
//	boost::transpose_graph(vg, reverseVg);

	reverser.valueGraphToDot("VG.dot");

	break;
  }

  return backend(project);
}