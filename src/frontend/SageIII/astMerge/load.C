
#include "sage3basic.h"

#include "AST_FILE_IO.h"
// Note that this is required to define the Sg_File_Info_XXX symbols (need for file I/O)
#include "Cxx_GrammarMemoryPoolSupport.h"

using namespace std;

namespace Rose {
namespace AST {

static void mergeSymbolTable(SgSymbolTable * gst, SgSymbolTable * st) {
  SgSymbolTable::BaseHashType* iht = st->get_table();
  ROSE_ASSERT(iht != NULL);

  SgSymbolTable::hash_iterator i = iht->begin();
  while (i != iht->end()) {
    SgSymbol * symbol = isSgSymbol((*i).second);
    ROSE_ASSERT(symbol != NULL);

    if (!gst->exists(i->first)) {
      // This function in the local function type table is not in the global function type table, so add it.
      gst->insert(i->first, i->second);
    } else {
      // These are redundant symbols, but likely something in the AST points to them so be careful.
      // This function type is already in the global function type table, so there is nothing to do (later we can delete it to save space)
    }
    i++;
  }
}

static void mergeTypeSymbolTable(SgTypeTable * gtt, SgTypeTable * tt) {
  SgSymbolTable * st  = tt->get_type_table();
  SgSymbolTable * gst = gtt->get_type_table();

  mergeSymbolTable(gst, st);
}

static void mergeFunctionTypeSymbolTable(SgFunctionTypeTable * gftt, SgFunctionTypeTable * ftt) {
  SgSymbolTable * fst  = ftt->get_function_type_table();
  SgSymbolTable * gfst = gftt->get_function_type_table();

  mergeSymbolTable(gfst, fst);
}

static void mergeFileIDs(
  std::map<int, std::string> const & f2n,
  std::map<std::string, int> const & n2f,
  std::map<int, std::string> & gf2n,
  std::map<std::string, int> & gn2f,
  size_t start_node
) {
  std::map<int, int> idxmap;

  for (std::map<int, std::string>::const_iterator i = f2n.begin(); i != f2n.end(); i++) {
    if (gn2f.count(i->second) == 0) {
      int idx = (int)gn2f.size();

      gn2f[i->second] = idx;
      gf2n[idx] = i->second;
      idxmap[i->first] = idx;
    } else {
      idxmap[i->first] = gn2f[i->second];
    }
  }

  unsigned num_nodes = Sg_File_Info::numberOfNodes();
  for (unsigned long i = start_node; i < num_nodes; i++) {
    // Compute the postion of the indexed Sg_File_Info object in the memory pool.
    unsigned long positionInPool = i % Sg_File_Info_CLASS_ALLOCATION_POOL_SIZE ;
    unsigned long memoryBlock    = (i - positionInPool) / Sg_File_Info_CLASS_ALLOCATION_POOL_SIZE;

    Sg_File_Info * fileInfo = &(((Sg_File_Info*)(Sg_File_Info_Memory_Block_List[memoryBlock]))[positionInPool]);
    ROSE_ASSERT(fileInfo != NULL);

    int oldFileId = fileInfo->get_file_id();
    int newFileId = idxmap[oldFileId];

    if (oldFileId >= 0 && oldFileId != newFileId) {
      fileInfo->set_file_id(newFileId);
    }
  }
}

#define DEBUG__ROSE_AST_LOAD 0

#if DEBUG__ROSE_AST_LOAD
static void displayFileIDs(
  std::map<int, std::string> const & f2n,
  std::map<std::string, int> const & n2f
) {
  for (std::map<int, std::string>::const_iterator i = f2n.begin(); i != f2n.end(); i++) {
    auto match = n2f.find(i->second);
    ROSE_ASSERT(match != n2f.end());
    std::cout << " * " << i->first << " -> " << i->second << " -> " << match->second << std::endl;
  }
}
#endif

void load(SgProject * project, std::list<std::string> const & astfiles) {
#if DEBUG__ROSE_AST_LOAD
  printf("Rose::AST::load:\n");
  printf(" -- project = %p\n", project);
#endif
  size_t num_nodes = Sg_File_Info::numberOfNodes();

  AST_FILE_IO::startUp(project);
  AST_FILE_IO::resetValidAstAfterWriting();

  // Save shared (static) fields, TODO:
  //   - global scope accross project
  //   - name mangling caches?
  SgTypeTable *         gtt = SgNode::get_globalTypeTable();
#if DEBUG__ROSE_AST_LOAD
  printf(" -- gtt = %p\n", gtt);
#endif
  SgNode::set_globalTypeTable(NULL);

  SgFunctionTypeTable * gftt = SgNode::get_globalFunctionTypeTable();
#if DEBUG__ROSE_AST_LOAD
  printf(" -- gftt = %p\n", gftt);
#endif
  SgNode::set_globalFunctionTypeTable(NULL);

  std::map<int, std::string> gf2n = Sg_File_Info::get_fileidtoname_map();
  std::map<std::string, int> gn2f = Sg_File_Info::get_nametofileid_map();

  std::list<std::string>::const_iterator astfile = astfiles.begin();
  size_t cnt = 1;
  while (astfile != astfiles.end()) {
    // Note the postfix increment in the following two lines
    std::string astfile_ = *(astfile++);

    AST_FILE_IO::readASTFromFile(astfile_);
    AstData * ast = AST_FILE_IO::getAst(cnt++);

    // Check that the root of the read AST is valid
    SgProject * lproject = ast->getRootOfAst();
#if DEBUG__ROSE_AST_LOAD
    printf(" -- lproject = %p\n", lproject);
#endif
    ROSE_ASSERT(lproject->get_freepointer() == AST_FileIO::IS_VALID_POINTER());

    // Insert all files into main project
    std::vector<SgFile *> const & files = lproject->get_files();
    for (std::vector<SgFile *>::const_iterator it = files.begin(); it != files.end(); ++it) {
      project->get_fileList().push_back(*it);
      (*it)->set_parent(project->get_fileList_ptr());
      (*it)->set_skipfinalCompileStep(true); // FIXME SgProject::get_skipfinalCompileStep returns conjunction of SgFile::get_skipfinalCompileStep which default to false. It would then always be false.
      (*it)->set_skipfinalCompileStep(project->get_skipfinalCompileStep());
    }
    lproject->get_fileList_ptr()->get_listOfFiles().clear();

    // Load shared (static) fields from the AST being read
    AST_FILE_IO::setStaticDataOfAst(ast);

    // Merge static fields
    SgTypeTable * lgtt = SgNode::get_globalTypeTable();
#if DEBUG__ROSE_AST_LOAD
    printf(" -- lgtt = %p\n", lgtt);
#endif
    mergeTypeSymbolTable(gtt, lgtt);

    SgFunctionTypeTable * lgftt = SgNode::get_globalFunctionTypeTable();
#if DEBUG__ROSE_AST_LOAD
    printf(" -- lgftt = %p\n", lgftt);
#endif
    mergeFunctionTypeSymbolTable(gftt, lgftt);

#if DEBUG__ROSE_AST_LOAD
    std::cout << "local file-map:" << std::endl;
    displayFileIDs(Sg_File_Info::get_fileidtoname_map(), Sg_File_Info::get_nametofileid_map());
    std::cout << "global file-map:" << std::endl;
    displayFileIDs(gf2n, gn2f);
#endif

    mergeFileIDs(Sg_File_Info::get_fileidtoname_map(), Sg_File_Info::get_nametofileid_map(), gf2n, gn2f, num_nodes);

    // Static pointer must be set to NULL for AST I/O to load them 
    SgNode::set_globalTypeTable(NULL);
    SgNode::set_globalFunctionTypeTable(NULL);

    num_nodes = Sg_File_Info::numberOfNodes();
  }

  SgNode::set_globalTypeTable(gtt);
  SgNode::set_globalFunctionTypeTable(gftt);
  Sg_File_Info::set_fileidtoname_map(gf2n);
  Sg_File_Info::set_nametofileid_map(gn2f);

#if DEBUG__ROSE_AST_LOAD
  std::cout << "final file-map:" << std::endl;
  displayFileIDs(Sg_File_Info::get_fileidtoname_map(), Sg_File_Info::get_nametofileid_map());
#endif

  AST_FILE_IO::reset();

//generateWholeGraphOfAST("merged", NULL);
}

}
}

