
#include "sage3basic.h"

#include "merge.h"

#include "fixupTraversal.h"

#include "AST_FILE_IO.h"
// Note that this is required to define the Sg_File_Info_XXX symbols (need for file I/O)
#include "Cxx_GrammarMemoryPoolSupport.h"

using namespace std;

namespace Rose {
namespace AST {

/////////////////////////////////
// Sharing Duplicate AST Nodes //
/////////////////////////////////

bool shareableIRnode (const SgNode * node) {
  if (isSgType(node)) return true;

  // Matching exact nodes kind that are shareable
  switch (node->variantT()) {
    // Declarations
    case V_SgFunctionDeclaration:
    case V_SgVariableDeclaration:
    case V_SgClassDeclaration:
    case V_SgTemplateInstantiationDecl:
    case V_SgPragmaDeclaration:
    case V_SgTemplateInstantiationDirectiveStatement:
    case V_SgTypedefDeclaration:
    case V_SgEnumDeclaration:
    case V_SgTemplateDeclaration:
    case V_SgUsingDeclarationStatement:
    case V_SgUsingDirectiveStatement:
    case V_SgMemberFunctionDeclaration:
    case V_SgTemplateInstantiationFunctionDecl:
    case V_SgTemplateInstantiationMemberFunctionDecl:
    // Symbols
    case V_SgClassSymbol:
    case V_SgEnumFieldSymbol:
    case V_SgEnumSymbol:
    case V_SgFunctionSymbol:
    case V_SgMemberFunctionSymbol:
    case V_SgLabelSymbol:
    case V_SgNamespaceSymbol:
    case V_SgTemplateSymbol:
    case V_SgTypedefSymbol:
    case V_SgVariableSymbol:
    // Others
    case V_SgTemplateArgument:
    case V_SgPragma:
    case V_SgInitializedName:
      return true;
    default:
      return false;
  }
}

struct NameBasedSharing : public ROSE_VisitTraversal {
  std::set<SgNode *> seen;
  std::map<std::string, std::vector<SgNode *> > name_to_nodes;
  std::map<SgNode *, std::string> nodes_to_name;

  void visit(SgNode * n) {
    if (!seen.insert(n).second) return;

    if (shareableIRnode(n)) {
      std::string name = SageInterface::generateUniqueName(n, false);
      name_to_nodes[name].push_back(n);
      nodes_to_name.insert(std::pair<SgNode *, std::string>(n, name));
    }
  }

  void apply() {
    traverseMemoryPool();

    std::map<SgNode*, SgNode*> replacements;
    for (std::map<std::string, std::vector<SgNode *> >::const_iterator it_map = name_to_nodes.begin(); it_map != name_to_nodes.end(); ++it_map) {
      std::vector<SgNode *> const & nodes = it_map->second;
      ROSE_ASSERT(nodes.size() > 0);
      std::vector<SgNode *>::const_iterator it_node = nodes.begin();
      SgNode * reference_node = *(it_node++);
      ROSE_ASSERT(reference_node != NULL);

      // Set reference_node as shared
      if (nodes.size() > 1) {
        if (reference_node->get_file_info() != NULL)
          reference_node->get_startOfConstruct()->setShared();
        if (reference_node->get_endOfConstruct() != NULL)
          reference_node->get_endOfConstruct()->setShared();
      }

      // Deal with the duplicates
      while (it_node != nodes.end()) {
        SgNode * duplicate_node = *(it_node++);
        ROSE_ASSERT(duplicate_node != NULL);
        replacements.insert(std::pair<SgNode*, SgNode*>(duplicate_node, reference_node));
      }
    }

    fixupTraversal(replacements);
  }
};

////////////////////
// Deleting Nodes //
////////////////////

struct DeleteDisconnectedNode : public ROSE_VisitTraversal {
  std::set<SgNode*> saves;
  std::vector<SgNode*> deletes;

  void visit (SgNode* node) {
//  if (isSgStorageModifier(node)) return; // FIXME idk why?
    if (
      saves.find(node) == saves.end()                                 || // part of the `saves` nodes
      ( isSg_File_Info(node) && isSgTypeDefault(node->get_parent()) )    // or signature of a Sg_File_Info object that is used as the location in a PreprocessingInfo object
    ) {
      deletes.push_back(node);
    }
  }

  // Recursively traverse all data members of AST nodes that are AST nodes themselves (not restricted to actual tree structure)
  void recursive_saves( SgNode* node) {
    // Stop on sinks and loops
    if (node == NULL || !saves.insert(node).second) return;

    std::vector<std::pair<SgNode*, std::string> > data_members = node->returnDataMemberPointers();
    for (std::vector<std::pair<SgNode*, std::string> >::iterator i = data_members.begin(); i != data_members.end(); ++i) {
      recursive_saves(i->first);
    }
  }

  // Prevent double deletes by traversing the various subtree in a top down fashion
  void ordered_delete() {
    std::map< SgNode *, std::vector<SgNode *> > descendants; // direct descendants
    for (std::vector<SgNode *>::const_iterator i = deletes.begin(); i != deletes.end(); ++i) {
      descendants.insert(std::pair< SgNode *, std::vector<SgNode *> >(*i, std::vector<SgNode *>()));
    }

    std::vector<SgNode *> roots;
    for (std::vector<SgNode *>::const_iterator i = deletes.begin(); i != deletes.end(); ++i) {
      SgNode * ni = *i;
      SgNode * pi = ni->get_parent();

      // If there is an entry for this node's parent then it is NOT a root (because previous loop init.)
      std::map< SgNode *, std::vector<SgNode *> >::iterator d = descendants.find(pi);
      if (d != descendants.end()) {
        d->second.push_back(ni);
      } else {
        roots.push_back(ni);
      }
    }

    // Delete starting from the roots
    while (roots.size() > 0) {
      std::vector<SgNode *> next;
      for (std::vector<SgNode *>::const_iterator i = roots.begin(); i != roots.end(); ++i) {
        SgNode * r = *i;

        // Only delete if it has not already been deleted
        if (r->get_freepointer() == AST_FileIO::IS_VALID_POINTER()) {
          delete r;
        }

        std::map< SgNode *, std::vector<SgNode *> >::const_iterator d = descendants.find(r);
        if (d != descendants.end()) {
          next.insert(next.end(), d->second.begin(), d->second.end());
        }
      }
      roots.clear();
      roots.insert(roots.end(), next.begin(), next.end());
    }
  }

  void apply(SgProject * project) {
    recursive_saves(project);
    recursive_saves(SgNode::get_globalFunctionTypeTable());
    recursive_saves(SgNode::get_globalTypeTable());

    traverseMemoryPool();

    ordered_delete();
  }
};

//////////////////////////////////////////
// Fixes defn and non-defn declarations //
//////////////////////////////////////////

#define DEBUG_MULTIPLE_DEFINING_DECLARATIONS 0

class PatchDeclStmt : public ROSE_VisitTraversal {
  private:
    std::map<std::string, std::vector<SgDeclarationStatement *> > declstmt_map;

  public:
    PatchDeclStmt() :
      declstmt_map()
    {}

    virtual ~PatchDeclStmt() {};

    void visit(SgNode * node) {
      SgDeclarationStatement * declstmt = isSgDeclarationStatement(node);
      if (
        declstmt != NULL &&
        !isSgPragmaDeclaration(declstmt) &&
        !isSgCtorInitializerList(declstmt) &&
        !isSgTemplateInstantiationDirectiveStatement(declstmt) &&
        !isSgUsingDeclarationStatement(declstmt)
      ) {
        std::string mangled_name = declstmt->get_mangled_name();
        declstmt_map[mangled_name].push_back(declstmt);
      }
    }

    void apply() {
      std::map<std::string, std::vector<SgDeclarationStatement *> >::iterator it_declvect;
      for (it_declvect = declstmt_map.begin(); it_declvect != declstmt_map.end(); it_declvect++) {

        std::set<SgDeclarationStatement *> defdecls;
        std::vector<SgDeclarationStatement *>::iterator it_declstmt;
        for (it_declstmt = it_declvect->second.begin(); it_declstmt != it_declvect->second.end(); it_declstmt++) {
          SgDeclarationStatement * defdecl = (*it_declstmt)->get_definingDeclaration();
          if (defdecl != NULL) {
            defdecls.insert(defdecl);
          }
        }
        if (defdecls.size() > 0) {
          SgDeclarationStatement * defdecl = *(defdecls.begin());
#if DEBUG_MULTIPLE_DEFINING_DECLARATIONS
          if (defdecls.size() > 1) {
            mfprintf(mlog [ WARN ] )("Warning: Found %d defining declarations for declarations with mangled name: %s (%s)\n", defdecls.size(), it_declvect->first.c_str(), defdecl->class_name().c_str());
          }
#endif
          for (it_declstmt = it_declvect->second.begin(); it_declstmt != it_declvect->second.end(); it_declstmt++) {
            (*it_declstmt)->set_definingDeclaration(defdecl);
          }
        }
      }
    }
};

////////////////////////
// AST merge function //
////////////////////////

void merge(SgProject* project) {
  TimingPerformance timer ("AST merge:");

  NameBasedSharing nbs;
  nbs.apply();

  DeleteDisconnectedNode ddn;
  ddn.apply(project);

  PatchDeclStmt patch_declstmt;
  patch_declstmt.traverseMemoryPool();
  patch_declstmt.apply();

  if (SgProject::get_verbose() > 0) {
    int nodes_start = nbs.seen.size();
    int nodes_end = numberOfNodes();

    // Based on: http://en.wikipedia.org/wiki/Data_compression_ratio
    double percentageCompression  = ( nodes_end * 100.0 ) / nodes_start;
    double percentageSpaceSavings = 100.0 - percentageCompression;
    double mergeFactor            = ( ((double) nodes_start) / ((double) nodes_end) );

    double numberOfFiles  = project->numberOfFiles();
    double mergeEfficency = 0.0;
    if (numberOfFiles == 1) {
      mergeEfficency = 1.0 + percentageSpaceSavings;
    } else {
      double fileNumberMultiplier = numberOfFiles / (numberOfFiles - 1);
      mergeEfficency = percentageSpaceSavings * fileNumberMultiplier;
    }

    printf ("After AST delete removed %d nodes from %d to %d:\n", nodes_start - nodes_end, nodes_start, nodes_end);
    printf ("      %2.4lf percent compression\n", percentageCompression);
    printf ("      %2.4lf percent space savings\n", percentageSpaceSavings);
    printf ("      mergeEfficency = %2.4lf\n", mergeEfficency);
    printf ("      mergeFactor = %2.4lf\n", mergeFactor);
  }
}

//////////////////////////
// File merge functions //
//////////////////////////

void mergeSymbolTable(SgSymbolTable * gst, SgSymbolTable * st) {
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

void mergeTypeSymbolTable(SgTypeTable * gtt, SgTypeTable * tt) {
  SgSymbolTable * st  = tt->get_type_table();
  SgSymbolTable * gst = gtt->get_type_table();

  mergeSymbolTable(gst, st);
}

void mergeFunctionTypeSymbolTable(SgFunctionTypeTable * gftt, SgFunctionTypeTable * ftt) {
  SgSymbolTable * fst  = ftt->get_function_type_table();
  SgSymbolTable * gfst = gftt->get_function_type_table();

  mergeSymbolTable(gfst, fst);
}

void mergeFileIDs(
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

void load(SgProject * project, std::list<std::string> const & astfiles) {
  size_t num_nodes = Sg_File_Info::numberOfNodes();

  AST_FILE_IO::startUp(project);
  AST_FILE_IO::resetValidAstAfterWriting();

  // Save shared (static) fields, TODO:
  //   - global scope accross project
  //   - name mangling caches?
  SgTypeTable *         gtt = SgNode::get_globalTypeTable();
  SgFunctionTypeTable * gftt = SgNode::get_globalFunctionTypeTable();
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
    mergeTypeSymbolTable(gtt, lgtt);

    SgFunctionTypeTable * lgftt = SgNode::get_globalFunctionTypeTable();
    mergeFunctionTypeSymbolTable(gftt, lgftt);

    mergeFileIDs(Sg_File_Info::get_fileidtoname_map(), Sg_File_Info::get_nametofileid_map(), gf2n, gn2f, num_nodes);

    // Restore shared (static) fields
    SgNode::set_globalTypeTable(gtt);
    SgNode::set_globalFunctionTypeTable(gftt);
    Sg_File_Info::set_fileidtoname_map(gf2n);
    Sg_File_Info::set_nametofileid_map(gn2f);

    num_nodes = Sg_File_Info::numberOfNodes();
  }

  AST_FILE_IO::reset();

//generateWholeGraphOfAST("merged", NULL);
}

}
}

