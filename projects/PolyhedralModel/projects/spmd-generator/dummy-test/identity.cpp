
#include "polydriver/polyhedral-driver.hpp"
#include "polydriver/polyhedral-placement.hpp"
#include "common/spmd-generator.hpp"
#include "common/array-analysis.hpp"

void usage(char * name) {
  std::cout << "usage: " << name << " input.c" << std::endl;
}

void help(char * name) {
  usage(name);
  std::cout << "This application copy the SCoP contains in a kernel." << std::endl;
  std::cout << "Kernel function name need to have the prefix \"kernel_\" and contains pragma \"scop\" and \"endscop\"" << std::endl;
  std::cout << std::endl;
  std::cout << "Author: Tristan Vanderbruggen (vanderbruggentristan@gmail.com)" << std::endl;
}

class IdentityAlias : public ArrayAlias {
  public:
    IdentityAlias(ArrayPartition * original_array_) :
      ArrayAlias(original_array_)
    {}

    virtual ~IdentityAlias() {}

    virtual SgPntrArrRefExp * propagateArr(SgPntrArrRefExp * arr_ref) const { return arr_ref; }
    virtual SgVarRefExp * propagateVar(SgVarRefExp * var_ref) const { return var_ref; }
    virtual SgInitializedName * getInitName() const { return original_array->getOriginalVariable().getInitializedName(); }
};

class IdentityGenerator : public SPMD_Generator {
  protected:
    virtual SgSourceFile * buildKernelFile(std::string kernel_file_name) { return NULL; }
    virtual ArrayAlias * genAlias(ArrayPartition * array_partition, ComputeSystem * compute_system, bool read_and_write = true) {
      std::map<ComputeSystem *, std::map<ArrayPartition *, ArrayAlias *> >::iterator it_cs = array_aliases.find(compute_system);
      if (it_cs == array_aliases.end()) {
        it_cs = array_aliases.insert(
          std::pair<ComputeSystem *, std::map<ArrayPartition *, ArrayAlias *> >(compute_system, std::map<ArrayPartition *, ArrayAlias *>())
        ).first;
      }
      std::map<ArrayPartition *, ArrayAlias *>::iterator it_ap = it_cs->second.find(array_partition);
      if (it_ap == it_cs->second.end()) {
        it_ap = it_cs->second.insert(
          std::pair<ArrayPartition *, ArrayAlias *>(array_partition, new IdentityAlias(array_partition))
        ).first;
      }
      return it_ap->second;
    }
    virtual SgStatement * codeGeneration(SPMD_KernelCall * tree) { assert(false); return NULL; }
    virtual SgStatement * codeGeneration(SPMD_Comm * tree) { assert(false); return NULL; }
    virtual SgStatement * codeGeneration(SPMD_Sync * tree) { assert(false); return NULL; }
    virtual void insertInit(
      SPMD_Tree * root_tree,
      std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > & to_be_aliased,
      SgStatement * insert_init_after,
      std::string kernel_file_name
    ) {
      std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >::iterator it1;
      std::set<ArrayPartition *>::iterator it2;
      for (it1 = to_be_aliased.begin(); it1 != to_be_aliased.end(); it1++) {
        array_aliases.insert(
            std::pair<ComputeSystem *, std::map<ArrayPartition *, ArrayAlias *> >(it1->first, std::map<ArrayPartition *, ArrayAlias *>())
        );
        for (it2 = it1->second.first.begin(); it2 != it1->second.first.end(); it2++)
          genAlias(*it2, it1->first);
        for (it2 = it1->second.second.begin(); it2 != it1->second.second.end(); it2++)
          genAlias(*it2, it1->first);
      }
    }
    virtual void insertFinal(SPMD_Tree * root_tree, SgStatement * insert_final_after) {}
    virtual void generateKernel(SPMD_Tree * root_tree, SgSourceFile * kernel_file) {}

  public:
    IdentityGenerator(SPMD_Driver & driver_) :
      SPMD_Generator(driver_)
    {}
    virtual ~IdentityGenerator() {}
};

class IdentityPlacement : public PolyPlacement {
  public:
    IdentityPlacement() :
      PolyPlacement(NULL)
    {}

    virtual void place(
        SPMD_Root * root,
        ArrayAnalysis & array_analysis,
        std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > & to_be_aliased
    ) {
      to_be_aliased.insert(
          std::pair<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >(
              NULL,
              std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> >(std::set<ArrayPartition *>(), std::set<ArrayPartition *>())
          )
      );
    }
    virtual Domain * onSameComputeSystem(SPMD_Tree * t1, SPMD_Tree * t2) const { return NULL; }
    virtual ComputeSystem * assigned(SPMD_Tree * tree) const {
      return NULL;
    }
};

int main(int argc, char ** argv) {

  if (argc != 2) {
    if (argc == 2) {
      if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        help(argv[0]);
      else
        usage(argv[0]);
    }
    else
      usage(argv[0]);
    return 1;
  }

  char * args[3] = {argv[0], (char *)"-DPARAMETRIC", argv[1]};
  SgProject * project = frontend ( 3 , args );

  PolyDriver poly_driver(new IdentityPlacement());
  IdentityGenerator identity_generator(poly_driver);

  std::vector<SgNode*> func_decls = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
  std::vector<SgNode*>::iterator it_;
  for (it_ = func_decls.begin(); it_ != func_decls.end(); it_++) {
    SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(*it_);

    // Look for the kernel function
    std::string name = func_decl->get_name().getString();
    if (name.compare(0, 7, "kernel_") != 0) continue;
    SgFunctionDefinition * func_def = func_decl->get_definition();
    if (func_def == NULL) continue;
    SgBasicBlock * func_body = func_def->get_body();
    if (func_body == NULL) continue;

    SageBuilder::pushScopeStack(func_body);

    SgStatement * insert_init_after = NULL;
    {
      std::vector<SgPragmaDeclaration *> pragmas = SageInterface::querySubTree<SgPragmaDeclaration>(func_body);
      std::vector<SgPragmaDeclaration *>::iterator it;
      for (it = pragmas.begin(); it != pragmas.end(); it++) {
        if ((*it)->get_pragma()->get_pragma() == "scop")
          insert_init_after = SageInterface::getPreviousStatement(*it);
      }
    }
    ROSE_ASSERT(insert_init_after != NULL);

    SgStatement * first = NULL;
    SgStatement * last = NULL;
    std::vector<SgStatement *>::iterator it;
    bool proceed = false;
    for (it = func_body->get_statements().begin(); it != func_body->get_statements().end(); it++) {
      if (isSgPragmaDeclaration(*it))
        if (isSgPragmaDeclaration(*it)->get_pragma()->get_pragma() == "scop") {
          proceed = true;
          continue;
        }
        else if (isSgPragmaDeclaration(*it)->get_pragma()->get_pragma() == "endscop") {
          proceed = false;
          break;
        }
      if (proceed) {
        if (first == NULL) first = *it;
        last = *it;
      }
    }

    SgStatement * insert_final_after = *(func_body->get_statements().end() - 1);

    SgStatement * res = identity_generator.generate(insert_init_after, insert_final_after, first, last, NULL);

    SgStatement * current = first;
    while (current != last) {
      SgStatement * next = SageInterface::getNextStatement(current);
      SageInterface::removeStatement(current);
      current = next;
    }
    SageInterface::replaceStatement(last, res);

    SageBuilder::popScopeStack();

  }

  project->unparse();

  return 0;
}

