
#include "sage3basic.h"

#include "fixupTraversal.h"

#if !defined(ROSE_PEDANTIC_FIXUP_CONSTANT_FOLDING)
#  define ROSE_PEDANTIC_FIXUP_CONSTANT_FOLDING 0
#endif

#if !defined(ROSE_DIAGNOSE_FIXUP_CONSTANT_FOLDING)
#  define ROSE_DIAGNOSE_FIXUP_CONSTANT_FOLDING 0
#endif

#if !defined(ROSE_GRAPHVIZ_EXPRESSION_TREES)
#  define ROSE_GRAPHVIZ_EXPRESSION_TREES ROSE_DIAGNOSE_FIXUP_CONSTANT_FOLDING
#endif

#if !defined(ROSE_WARN_ORPHAN_EXPRESSIONS)
#  define ROSE_WARN_ORPHAN_EXPRESSIONS ROSE_DIAGNOSE_FIXUP_CONSTANT_FOLDING
#endif

#if !defined(ROSE_WARN_DISCONNECTED_EXPRESSIONS)
#  define ROSE_WARN_DISCONNECTED_EXPRESSIONS ROSE_DIAGNOSE_FIXUP_CONSTANT_FOLDING
#endif

// ****************************************************************************
//   Supporting function used in both cases of the constant folding handling
// ****************************************************************************

static void deleteExpressionAndOriginalExpressionTree(SgNode * node) {
  // Skip node that have already been deleted
  if (node->get_freepointer() != AST_FileIO::IS_VALID_POINTER()) return;

  // If it is an expression delete the associated original expression tree
  SgExpression * exp = isSgExpression(node);
  if (exp != NULL && exp->get_originalExpressionTree() != NULL) {
    deleteExpressionAndOriginalExpressionTree(exp->get_originalExpressionTree());
  }

  // Traverse the AST and delete successors
  std::vector<SgNode*> successors = node->get_traversalSuccessorContainer();
  for (std::vector<SgNode*>::iterator n = successors.begin(); n != successors.end(); ++n) {
    if (*n != NULL ) {
      deleteExpressionAndOriginalExpressionTree(*n);
    }
  }
  
  delete node;
}

struct ReplacePointerInParent : public SimpleReferenceToPointerHandler {
  SgNode * old_ptr;
  SgNode * new_ptr;
  unsigned count;

  ReplacePointerInParent(SgNode * old_ptr_, SgNode * new_ptr_) :
    old_ptr(old_ptr_),
    new_ptr(new_ptr_),
    count(0)
  {}

  virtual void operator()(SgNode * & key, const SgName & debugStringName, bool is_traversed) {
    if (key == old_ptr) {
      ROSE_ASSERT(count == 0);
      key = new_ptr;
      count++;
    }
  }
  
  static void apply(SgNode * op, SgNode * np) {
    SgNode * parent = op->get_parent();
    ROSE_ASSERT(parent != NULL);

    ReplacePointerInParent r(op, np);
    parent->processDataMemberReferenceToPointers(&r);

    np->set_parent(parent);
  }
};

struct CollectExpressionTrees : public ROSE_VisitTraversal {
  // Root of an expression trees but *parent is not set*
  //    Triggers an ASSERTION if not empty
  std::set<SgExpression *> orphans;

  // Root of an expression tree that is part of the AST
  std::set<SgExpression *> roots;

  // Root of an original expression tree (not part of the AST)
  std::set<SgExpression *> originals;

  // Root of an expression tree not reacheable from the AST (only testing successors)
  std::set<SgExpression *> disconnected;

#if ROSE_GRAPHVIZ_EXPRESSION_TREES
  // Part of an expression tree but not root (only used for graph generation)
  std::set<SgExpression *> subtrees;
#endif

  void visit (SgNode * node) {
    SgExpression * expr = isSgExpression(node);
    if (expr != NULL) {
      SgNode * parent = expr->get_parent();
      if (parent == NULL) {
        orphans.insert(expr);
        return;
      }

      SgExpression * expr_parent = isSgExpression(parent);
      if (expr_parent == NULL) {
        roots.insert(expr);
        return;
      }

      if (expr_parent->get_originalExpressionTree() == expr) {
        originals.insert(expr);
        return;
      }

      std::vector<SgNode*> successors = expr_parent->get_traversalSuccessorContainer();
      if (std::find(successors.begin(), successors.end(), node) != successors.end()) {
#if ROSE_GRAPHVIZ_EXPRESSION_TREES
        subtrees.insert(expr);
#endif
        return;
      }

      SgTypeTraitBuiltinOperator * ttbo_parent = isSgTypeTraitBuiltinOperator(expr_parent);
      if (ttbo_parent != NULL) {
#if defined(__cpp_range_based_for) && __cpp_range_based_for >= 200907
        for (auto arg: ttbo_parent->get_args()) {
#else
        SgNodePtrList & args = ttbo_parent->get_args();
        for (SgNodePtrList::iterator it = args.begin(); it != args.end(); it++) {
          SgNode * arg = *it;
#endif
          if (arg == expr) {
#if ROSE_GRAPHVIZ_EXPRESSION_TREES
            subtrees.insert(expr);
#endif
            return;
          }
        }
      }

      disconnected.insert(expr);
    }
  }

#if ROSE_GRAPHVIZ_EXPRESSION_TREES
  void toDot(std::ostream & out) {
    out << "digraph expr_trees {" << std::endl;
    for (auto expr: orphans) {
      out << "  n_" << std::hex << expr << " [label=\"orphan\n" << std::hex << expr << "\n" << expr->class_name() << "\", color=red];" << std::endl;
    }
    for (auto expr: roots) {
      out << "  n_" << std::hex << expr << " [label=\"root\n" << std::hex << expr << "\n" << expr->class_name() << "\"];" << std::endl;
    }
    for (auto child: originals) {
      auto folded = child->get_parent();
      out << "  n_" << std::hex << child << " [label=\"original\n" << std::hex << child << "\n" << child->class_name() << "\"];" << std::endl;
      out << "  n_" << std::hex << folded << " -> n_" << std::hex << child << " [constraint=false, label=\"original\", color=green];" << std::endl;
    }
    for (auto child: disconnected) {
      auto parent = child->get_parent();
      out << "  n_" << std::hex << child << " [label=\"disconnected\n" << std::hex << child << "\n" << child->class_name() << "\"];" << std::endl;
      out << "  n_" << std::hex << parent << " -> n_" << std::hex << child << " [constraint=false, color=red];" << std::endl;
    }
    for (auto child: subtrees) {
      auto parent = child->get_parent();
      out << "  n_" << std::hex << child << " [label=\"subtree\n" << std::hex << child << "\n" << child->class_name() << "\"];" << std::endl;
      out << "  n_" << std::hex << parent << " -> n_" << std::hex << child << " [constraint=true, color=blue];" << std::endl;
    }
    out << "}" << std::endl;
  }
#endif
};

inline SgExpression * get_parent_if_folded_in(SgExpression * expr) {
  SgExpression * result = isSgExpression(expr->get_parent());
  if (result != NULL && result->get_originalExpressionTree() == expr) {
    return result;
  } else {
    return NULL;
  }
}

void removeConstantFoldedValue(SgProject * project) {
  CollectExpressionTrees cet;
  cet.traverseMemoryPool();

#if ROSE_GRAPHVIZ_EXPRESSION_TREES
  std::ofstream out("expr_trees.dot");
  cet.toDot(out);
#endif

#if ROSE_WARN_ORPHAN_EXPRESSIONS
  if (!cet.orphans.empty()) {
    std::cerr << "# Found " << std::dec << cet.orphans.size() << " orphaned expressions: parent is not set." << std::endl;
    for (auto expr: cet.orphans) {
      std::cerr << "#  - " << std::hex << expr << " ( " << expr->class_name() << " )" << std::endl;
    }
    std::cerr << "# These nodes and all their successors will be deleted from the AST." << std::endl << std::endl;
  }
#endif

#if ROSE_WARN_DISCONNECTED_EXPRESSIONS
  if (!cet.disconnected.empty()) {
    std::cerr << "# Found " << std::dec << cet.disconnected.size() << " disconnected expressions: parent is set but expression is not in its successors." << std::endl;
    for (auto expr: cet.disconnected) {
      std::cerr << "#  - " << std::hex << expr << " ( " << expr->class_name() << " )" << std::endl;
    }
    std::cerr << "# These nodes and all their successors will be deleted from the AST." << std::endl << std::endl;
  }
#endif

  std::map<SgNode *, SgNode *> replace_set;
  std::set<SgExpression *> delete_set;

  delete_set.insert(cet.disconnected.begin(), cet.disconnected.end());
  delete_set.insert(cet.orphans.begin(), cet.orphans.end());

  // We cut the chain as we process it starting from the latest element.
  // Some visited links could be seen as last element of a chain that has been cut by mistake.
  std::set<SgExpression *> seen_in_ot_chain; 

#if defined(__cpp_range_based_for) && __cpp_range_based_for >= 200907
  for (auto child: cet.originals) {
#else
  for (std::set<SgExpression *>::iterator it = cet.originals.begin(); it != cet.originals.end(); it++) {
    SgExpression * child = *it;
#endif
    // Check that it is the actual original tree of a *potential* chain of substitutions
    //    Note: I am not sure chain of substitutions occur with latest version of EDG
    if (child->get_originalExpressionTree() == NULL && seen_in_ot_chain.find(child) == seen_in_ot_chain.end()) {
      seen_in_ot_chain.insert(child);

      SgExpression * folded = get_parent_if_folded_in(child);
      ROSE_ASSERT(folded != NULL);
      ROSE_ASSERT(folded != child);

      // Detach the selected expression from the chain of substitutions
      //   `folded` is the direct parent of `child` in the chain
      folded->set_originalExpressionTree(NULL);
      seen_in_ot_chain.insert(folded);

      // Traverse the chain of substitutions to find its first element
      SgExpression * tmp = get_parent_if_folded_in(folded);
      while (tmp != NULL) {
        folded = tmp;
        seen_in_ot_chain.insert(folded);
        tmp = get_parent_if_folded_in(folded);
      }

      // Mark all intermediaries to be deleted
      if (folded->get_originalExpressionTree() != NULL) {
        delete_set.insert(folded->get_originalExpressionTree());
        folded->set_originalExpressionTree(NULL);
      }

      bool replace_folded_by_child = true;

      // Folded already marked for delete
      if (delete_set.find(folded) != delete_set.end()) {
        replace_folded_by_child = false; // Child will be deleted too
      }

      if (isSgEnumVal(folded)) {
        // TODO 1st issue: the initializer of a variable using enum-value (`X::enum_e e = X::none`):
        //        -> the value is in the original tree of the enum-value
        //        -> this replacement create a type error in C++
        // TODO 2nd issue: `enum { s = sizeof(struct X {  } };` definition of X would not be unparsed
        //        -> not an issue if struct is anonymous
        //        -> I cannot find a correct predicate would probably need to save more info in EDG
        replace_folded_by_child = false;
      }

      if (replace_folded_by_child) {
        child->set_parent(folded->get_parent()); // prevent replacement from creating self loop if child is direct descendant of folded
#if 0
        printf ("child = %p = %s \n",child,child->class_name().c_str());
#endif
#if 0
     // DQ (7/18/2020): Added support to permit Cxx11_tests/test2020_69.C to pass.
        SgLambdaExp* lambdaExp = isSgLambdaExp(child);
        if (lambdaExp == NULL)
           {
             replace_set[folded] = child;
             delete_set.insert(folded);
           }
#else
        replace_set[folded] = child;
        delete_set.insert(folded);
#endif
      } else {
        delete_set.insert(child);
      }
    }
  }

  Rose::AST::fixupTraversal(replace_set);

#if defined(__cpp_range_based_for) && __cpp_range_based_for >= 200907
  for (auto expr: delete_set) {
#else
  for (std::set<SgExpression *>::iterator it = delete_set.begin(); it != delete_set.end(); it++) {
    SgExpression * expr = *it;
#endif
    deleteExpressionAndOriginalExpressionTree(expr);
  }
}

struct RemoveOriginalExpressionTrees : public ROSE_VisitTraversal {
  void visit (SgNode* node) {
    ROSE_ASSERT(node != NULL);

    SgExpression * exp = isSgExpression(node);
#if 0
    printf ("In RemoveOriginalExpressionTrees::visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif

    if (exp != NULL) {
      SgExpression * oet = exp->get_originalExpressionTree();
      if (oet != NULL) {
#if 0
        printf ("In RemoveOriginalExpressionTrees::visit(): calling deleteExpressionAndOriginalExpressionTree()oet = %p = %s \n",oet,oet->class_name().c_str());
#endif
        exp->set_originalExpressionTree(NULL);
        deleteExpressionAndOriginalExpressionTree(oet);
      }
    }
  }
};

//! This removes the original expression tree from value expressions where it has been constant folded by EDG.
void resetConstantFoldedValues( SgNode* node ) {

#if 1
  if (!isSgProject(node) || !((SgProject *)node)->get_frontendConstantFolding()) {
    TimingPerformance timer1 ("Fixup Constant Folded Values (replace with original expression trees):");

#if 1
    removeConstantFoldedValue((SgProject *)node);
#else
    printf ("In resetConstantFoldedValues(): Skipping call to removeConstantFoldedValue() \n");
#endif

  } else {
    TimingPerformance timer1 ("Fixup Constant Folded Values (remove the original expression tree, leaving the constant folded values):");
    RemoveOriginalExpressionTrees astFixupTraversal;
    astFixupTraversal.traverseMemoryPool();
  }
#else
    printf ("In resetConstantFoldedValues(): Skipping body of resetConstantFoldedValues() \n");
#endif

}
