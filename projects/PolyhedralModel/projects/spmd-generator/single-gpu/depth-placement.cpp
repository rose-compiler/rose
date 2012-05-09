
#include "single-gpu/depth-placement.hpp"
#include "common/spmd-tree.hpp"
#include "common/array-analysis.hpp"

#include <cassert>

#include "rose.h"

// Fix a g++ bug: some header (probably iostream) define 'used' to nothing
#undef used

#define DEBUG 1

bool DepthPlacement::isParallel(SPMD_Loop * loop) {
  ScopTree * scoptree = polydriver->getScopTree(loop);
  assert(scoptree->isLoop());
  SgNode * tmp_loop = (SgNode*)(((ScopLoop *)scoptree)->getOriginalLoop());
  assert(tmp_loop != NULL);
  SgForStatement * orig_loop = isSgForStatement(tmp_loop);
  assert(orig_loop != NULL);
  std::vector<PreprocessingInfo *> * preproc_info = orig_loop->get_attachedPreprocessingInfoPtr();
  if (preproc_info != NULL && preproc_info->size() > 0) {
    std::vector<PreprocessingInfo *>::iterator it;
    for (it = preproc_info->begin(); it != preproc_info->end(); it++) {
      if ((*it)->getString().find("// parfor") != std::string::npos) {
#if DEBUG
        std::cerr << "Loop " << loop << " is Parallel" << std::endl;
#endif
        return true;
      }
#if DEBUG
      std::cerr << "Loop " << loop << " have " << (*it)->getString() << std::endl;
#endif
    }
  }
#if DEBUG
  std::cerr << "Loop " << loop << " don't carry any preprocessing info: Not Parallel" << std::endl;
#endif
  return false;
}

void DepthPlacement::preplace(SPMD_Tree * tree, unsigned depth) {
  bool inserted;
  if (depth <= out)
    inserted = placement.insert(std::pair<SPMD_Tree *, ComputeSystem *>(tree, cores[0])).second;
  else
    inserted = placement.insert(std::pair<SPMD_Tree *, ComputeSystem *>(tree, gpus[0])).second;
  assert(inserted);

  std::vector<SPMD_Tree *>::iterator it;
  for (it = tree->getChildren().begin(); it != tree->getChildren().end(); it++) {
    if (dynamic_cast<SPMD_Loop *>(*it) != NULL)
      preplace(*it, depth+1);
    else if (dynamic_cast<SPMD_KernelCall *>(*it))
      assert(false); // Don't support pre-existing kernel
    else
      preplace(*it, depth);
  }
}

void DepthPlacement::generateKernels(
  SPMD_Tree * parent,
  std::vector<SPMD_Tree *> & branch,
  unsigned length,
  std::vector<SPMD_KernelCall *> & res,
  ArrayAnalysis & array_analysis
) {
  std::vector<SPMD_Tree *> & children = branch.back()->getChildren();
  std::vector<SPMD_Tree *>::iterator it;
  if (length == used) {
    SPMD_KernelCall * kernel_call = new SPMD_KernelCall(parent, branch.front(), branch.back());
    placement.insert(std::pair<SPMD_Tree *, ComputeSystem *>(kernel_call, placement[branch.front()]));
    array_analysis.update(kernel_call, branch);
    it = children.begin();
    while (it != children.end()) {
      kernel_call->appendChild(*it);
      it = children.erase(it);
    }
    res.push_back(kernel_call);
  }
  else {
    std::vector<SPMD_Tree *> tmp_children;
    it = children.begin();
    while (it != children.end() || tmp_children.size() > 0) {
      while (
              it != children.end() && 
              (dynamic_cast<SPMD_Loop *>(*it) == NULL || !isParallel((SPMD_Loop *)(*it))) &&
              dynamic_cast<SPMD_DomainRestriction *>(*it) == NULL
      ) {
        tmp_children.push_back(*it);
        it = children.erase(it);
      }
      bool use_tmp_children = false;
      if (tmp_children.size() > 0) {
        SPMD_KernelCall * kernel_call = new SPMD_KernelCall(parent, branch.front(), branch.back());
        placement.insert(std::pair<SPMD_Tree *, ComputeSystem *>(kernel_call, placement[branch.front()]));
        array_analysis.update(kernel_call, branch);
        for (std::vector<SPMD_Tree *>::iterator it_ = tmp_children.begin(); it_ != tmp_children.end(); it_++)
          kernel_call->appendChild(*it_);
        res.push_back(kernel_call);
        tmp_children.clear();
        use_tmp_children = true;
      }
      if (it != children.end()) {
        unsigned nbr_kernel = res.size();
        branch.push_back(*it);
        if (dynamic_cast<SPMD_Loop *>(*it) != NULL)
          generateKernels(parent, branch, length+1, res, array_analysis);
        else
          generateKernels(parent, branch, length, res, array_analysis);
        branch.erase(branch.end()-1);
        if (dynamic_cast<SPMD_DomainRestriction *>(*it) != NULL && nbr_kernel == res.size() - 1) {
          (*it)->getChildren().insert((*it)->getChildren().begin(), res.back()->getChildren().begin(), res.back()->getChildren().end());
          delete res.back();
          res.erase(res.end()-1);
          if (use_tmp_children) {
            tmp_children.insert(tmp_children.begin(), res.back()->getChildren().begin(), res.back()->getChildren().end());
            delete res.back();
            res.erase(res.end()-1);
          }
          tmp_children.push_back(*it);
        }
        it = children.erase(it);
      } 
    }
    assert(it == children.end());
    assert(tmp_children.size() == 0);
  }
}

void DepthPlacement::makeKernel(
  SPMD_Loop * first_loop,
  ArrayAnalysis & array_analysis
) {
  SPMD_Tree * parent = first_loop->getParent();

#if DEBUG
  std::cerr << "[DepthPlacement::makeKernel] Start: " << first_loop << std::endl;
#endif

  std::vector<SPMD_KernelCall *> kernel_calls;
  std::vector<SPMD_Tree *> branch;
  branch.push_back(first_loop);
  generateKernels(parent, branch, 1, kernel_calls, array_analysis);

#if DEBUG
  std::cerr << "[DepthPlacement::makeKernel] Remove the loops" << std::endl;
#endif

  // Remove the loops
  first_loop->deepDelete();

#if DEBUG
  std::cerr << "[DepthPlacement::makeKernel] Insert the kernel calls" << std::endl;
#endif

  // Find the insertion point in the parent node
  std::vector<SPMD_Tree *>::iterator it_child = parent->getChildren().begin();
  while (it_child != parent->getChildren().end()) {
    if (*it_child == first_loop) break;
    it_child++;
  }
  assert(it_child != parent->getChildren().end());

  // Insert the kernel calls sequentially
  *it_child = kernel_calls[0];
  if (kernel_calls.size() > 1) {
    std::vector<SPMD_KernelCall *>::iterator it_kernels;
    for (it_kernels = kernel_calls.begin()+1; it_kernels != kernel_calls.end(); it_kernels++) {
      it_child = parent->getChildren().insert(it_child+1, *it_kernels);
    }
  }
#if DEBUG
  std::cerr << "[DepthPlacement::makeKernel] Done: " << first_loop << std::endl;
#endif
}
 
DepthPlacement::DepthPlacement(ComputeNode * node, unsigned out_, unsigned used_) :
  PolyPlacement(node),
  out(out_),
  used(used_),
  gpus(),
  cores(),
  placement()
{
  { // List Cores
    ComputeSystem * cpu = node->getCPU();
    assert(cpu != NULL);
    Core * core = dynamic_cast<Core *>(cpu);
    MultiCore * multicore = dynamic_cast<MultiCore *>(cpu);
    if (core != NULL)
      cores.push_back(core);
    else {
      assert(multicore != NULL);
      assert(false);
    }
  }
  { // List GPUs
    unsigned id = 0;
    GPU * gpu = dynamic_cast<GPU *>(node->getAcceleratorByID(id));
    while (gpu != NULL) {
      gpus.push_back(gpu);
      id++;
      gpu = dynamic_cast<GPU *>(node->getAcceleratorByID(id));
    }
    assert(node->getAcceleratorByID(id) == NULL); // FIXME accept only system with only GPU accelerator
  }
  assert(cores.size() == 1 && gpus.size() == 1); // FIXME because right now it is single CPU and single GPU
}

DepthPlacement::~DepthPlacement() {}

void DepthPlacement::place(
  SPMD_Root * root,
  ArrayAnalysis & array_analysis,
  std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > & to_be_aliased
) {

#if DEBUG
    std::cerr << "[DepthPlacement::place] Start" << std::endl;
#endif

  { // init of the map 'to_be_aliased'
    std::vector<GPU *>::iterator it0;
    for (it0 = gpus.begin(); it0 != gpus.end(); it0++)
      to_be_aliased.insert(
        std::pair<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >(
          *it0,
          std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> >(std::set<ArrayPartition *>(), std::set<ArrayPartition *>())
        )
      );
    std::vector<Core *>::iterator it1;
    for (it1 = cores.begin(); it1 != cores.end(); it1++)
      to_be_aliased.insert(
        std::pair<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > >(
          *it1,
          std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> >(std::set<ArrayPartition *>(), std::set<ArrayPartition *>())
        ) 
      );
  }
  { // initial placement

#if DEBUG
    std::cerr << "[DepthPlacement::place] Proceed to initial placement" << std::endl;
#endif

    placement.insert(std::pair<SPMD_Tree *, ComputeSystem *>(root, cores[0]));

    std::vector<SPMD_Tree *>::iterator it;
    for (it = root->getChildren().begin(); it != root->getChildren().end(); it++) {
      if (dynamic_cast<SPMD_Loop *>(*it) != NULL)
        preplace(*it, 1);
      else if (dynamic_cast<SPMD_KernelCall *>(*it))
        assert(false); // Don't support pre-existing kernel FIXME at the first lvl could be nice for UPC -> UPC/OpenCL
      else
        preplace(*it, 0);
    }
  }
  { // create KernelCall nodes

#if DEBUG
    std::cerr << "[DepthPlacement::place] Identify first parallel loops" << std::endl;
#endif

    std::vector<SPMD_Loop *> first_loops;
    std::queue<SPMD_Tree *> queue;
    std::vector<SPMD_Tree *>::iterator it_child;
    std::map<SPMD_Tree *, ComputeSystem *>::iterator it;
    queue.push(root);
    while (!queue.empty()) {
      SPMD_Tree * tree = queue.front();
      queue.pop();
      SPMD_Loop * loop = dynamic_cast<SPMD_Loop *>(tree);
      if (loop != NULL) {
        it = placement.find(loop);
        assert(it != placement.end());
        if (dynamic_cast<GPU *>(it->second)) {
          if (isParallel(loop)) {
            first_loops.push_back(loop);
            continue;
          }
          else {
            assert(loop->getParent() != NULL);
            std::map<SPMD_Tree *, ComputeSystem *>::iterator it_ = placement.find(loop->getParent());
            assert(it_ != placement.end());
            placement[loop] = it_->second;
          }
        }
      }
      else if (tree != root) {
        assert(tree->getParent() != NULL);
        std::map<SPMD_Tree *, ComputeSystem *>::iterator it_ = placement.find(tree->getParent());
        assert(it_ != placement.end());
        placement[tree] = it_->second; // to correct if parent is a loop and have been change
      }
      for (it_child = tree->getChildren().begin(); it_child != tree->getChildren().end(); it_child++)
        queue.push(*it_child);
    }

#if DEBUG
    std::cerr << "[DepthPlacement::place] Produce kernels" << std::endl;
#endif

    std::vector<SPMD_Loop *>::iterator it_loop;
    for (it_loop = first_loops.begin(); it_loop != first_loops.end(); it_loop++)
      makeKernel(*it_loop, array_analysis);
  }

#if DEBUG
  std::cerr << "[DepthPlacement::place] Collect array partition to be aliased" << std::endl;
#endif

  collectAliasing(root, array_analysis, to_be_aliased);

#if DEBUG
  std::cerr << "[DepthPlacement::place] Generate Communication and Sync" << std::endl;
#endif

  generateCommSync(root, array_analysis);
  
#if DEBUG
  std::cerr << "[DepthPlacement::place] Done" << std::endl;
#endif
}

Domain * DepthPlacement::onSameComputeSystem(SPMD_Tree * t1, SPMD_Tree * t2) const {
  return NULL;
}

ComputeSystem * DepthPlacement::assigned(SPMD_Tree * tree) const {
  std::map<SPMD_Tree *, ComputeSystem *>::const_iterator it = placement.find(tree);
  assert(it != placement.end());
  return it->second;
}

