#include "valueGraph.h"

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

	Backstroke::ValueGraph vg;
	vg.build(funcDef);
	vg.toDot("VG.dot");

	break;
  }

  return backend(project);
}
