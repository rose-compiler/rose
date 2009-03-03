#include "rose.h"

#include <vector>
#include <qrose.h>
#include "BinQGui.h"
#include "BinQbatch.h"
#include "BinQinteractive.h"
#include <boost/program_options.hpp>
#include <iostream>
#include "boost/filesystem/operations.hpp" // includes boost/filesystem/path.hpp

#include <fstream>

using namespace qrs;
using namespace boost::program_options;
using namespace boost;
using namespace std;
using namespace boost::filesystem;



class DeleteAST2 : public SgSimpleProcessing {
public:
  //! Required traversal function
  void visit (SgNode* node) {
    if (node) {
      // tps , 2March 2009
      // This node can't be deleted for some reason?!
      if (!isSgAsmExecutableFileFormat(node)) {
      	delete node;
      }
    }
  }
};


void 
DeleteSgTree( SgNode* root) {
  VariantVector vv1 = V_SgNode;
  std::cout << "Number of nodes before: " << 
    NodeQuery::queryMemoryPool(vv1).size() << std::endl;

  DeleteAST2 deleteTree;
  deleteTree.traverse(root,postorder);

  vector<SgNode*> vec = NodeQuery::queryMemoryPool(vv1);
  std::cout << "Number of nodes after AST deletion: " << 
    vec.size() << std::endl;

  vector<SgNode*>::const_iterator it = vec.begin();
  for (;it!=vec.end();++it) {
    SgNode* node = *it;
    // tps :: the following nodes are not deleted with the 
    // AST traversal. We can only delete some of them -
    // but not all within the memory pool traversal
    cerr << "  Not deleted : " << node->class_name() ;
    if (!isSgAsmTypeByte(node) &&
	!isSgAsmTypeWord(node) &&
	!isSgAsmTypeDoubleWord(node) &&
	!isSgAsmTypeQuadWord(node) && 

	!isSgAsmType128bitFloat(node) && 
	!isSgAsmType80bitFloat(node) && 
	!isSgAsmTypeDoubleFloat(node) && 
	!isSgAsmTypeDoubleQuadWord(node) && 
	!isSgAsmTypeSingleFloat(node) && 
	!isSgAsmTypeVector(node) && 

	!isSgAsmGenericFile(node) &&
	!isSgAsmGenericHeader(node) &&
	!isSgAsmGenericSection(node) 
	) {
      cerr << "    .. deleting. " ;
      delete node;
    }
    cerr << endl;
  }

  std::cout << "Number of nodes after Memory pool deletion: " << 
    NodeQuery::queryMemoryPool(vv1).size() << std::endl;
  
}


int main(int argc, char** argv) {
  SgProject* project = frontend(argc,argv);
  DeleteSgTree(project);
  SgProject* project2 = frontend(argc,argv);
  DeleteSgTree(project2);
}
