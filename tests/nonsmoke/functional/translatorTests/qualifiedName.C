/*
 * a strange bug related to missing the scope of the 2nd parameter of __builtin_prefetch()
 *  Liao
 *  8/26/2009
 */
#include "rose.h"
#include <iostream>
using namespace std;

int
main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);
  ROSE_ASSERT (project != NULL);
  printf("Running ROSE...");
  string filename = project->get_outputFileName () ;
  cout << "OUTPUTFILE : " << filename << endl;
  std::vector<SgFile*> files = project->get_fileList_ptr()->get_listOfFiles();
  std::vector<SgFile*>::const_iterator it = files.begin();
  for (;it!=files.end();++it) {
    SgFile* file = isSgFile(*it);
    ROSE_ASSERT(file);
    SgSourceFile* sfile = isSgSourceFile(file);
    if (sfile) {
      cout << "FILENAME : " << sfile->getFileName() << endl;
      SgGlobal* global = isSgGlobal(sfile->get_globalScope());
      std::vector< SgNode * > nodes=	sfile->get_traversalSuccessorContainer ();
      std::vector< SgNode * >::const_iterator it2=nodes.begin();
      for (;it2!=nodes.end();++it2) {
	SgNode* node= isSgNode(*it2);
	if (node) {
	  // right now this finds SgGlobal but it should find also
	  // SgVariableDeclaration and SgFunctionDeclaration
	  cout << "Found node : " << node->class_name() << endl;
	}
      }
    }
  }

  Rose_STL_Container<SgNode*> initNames = NodeQuery::querySubTree(project, V_SgInitializedName);
  for (Rose_STL_Container<SgNode*>::const_iterator i = initNames.begin(); i != initNames.end(); ++i) {
    SgInitializedName* iName = isSgInitializedName(*i);
    string name = iName->get_qualified_name().str();
  }

  return backend (project);
}
