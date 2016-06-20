// Example of an AST traversal that uses the Boost idiom of throwing
// an exception to exit the traversal early.

#include <rose.h>
#include <string>
#include <iostream>

using namespace std;

// Exception to indicate an early exit from a traversal at some node.
class StopEarly
{
public:
  StopEarly (const SgNode* n) : exit_node_ (n) {}
  StopEarly (const StopEarly& e) : exit_node_ (e.exit_node_) {}

  // Prints information about the exit node.
  void print (ostream& o) const
  {
    if (exit_node_) {
      o << '\t' << (const void *)exit_node_ << ":" << exit_node_->class_name () << endl;
      const SgLocatedNode* loc_n = isSgLocatedNode (exit_node_);
      if (loc_n) {
        const Sg_File_Info* info = loc_n->get_startOfConstruct ();
        ROSE_ASSERT (info);
        o << "\tAt " << info->get_filename () << ":" << info->get_line () << endl;
      }
    }
  }

private:
  const SgNode* exit_node_; // Node at early exit from traversal
};

// Preorder traversal to find the first SgVarRefExp of a particular name.
class VarRefFinderTraversal : public AstSimpleProcessing
{
public:
  // Initiate traversal to find 'target' in 'proj'.
  void find (SgProject* proj, const string& target)
  {
    target_ = target;
    traverseInputFiles (proj, preorder);
  }

  void visit (SgNode* node)
  {
    const SgVarRefExp* ref = isSgVarRefExp (node);
    if (ref) {
      const SgVariableSymbol* sym = ref->get_symbol ();
      ROSE_ASSERT (sym);
      cout << "Visiting SgVarRef '" << sym->get_name ().str () << "'" << endl;
      if (sym->get_name ().str () == target_) // Early exit at first match.
        throw StopEarly (ref);
    }
  }

private:
  string target_; // Symbol reference name to find.
};

int main (int argc, char* argv[])
{
  // Initialize and check compatibility. See rose::initialize
  ROSE_INITIALIZE;

  SgProject* proj = frontend (argc, argv);
  VarRefFinderTraversal finder;

  // Look for a reference to "__stop__".
  try {
    finder.find (proj, "__stop__");
    cout << "*** Reference to a symbol '__stop__' not found. ***" << endl;
  } catch (StopEarly& stop) {
    cout << "*** Reference to a symbol '__stop__' found. ***" << endl;
    stop.print (cout);
  }

  // Look for a reference to "__go__".
  try {
    finder.find (proj, "__go__");
    cout << "*** Reference to a symbol '__go__' not found. ***" << endl;
  } catch (StopEarly& go) {
    cout << "*** Reference to a symbol '__go__' found. ***" << endl;
    go.print (cout);
  }

  // Regardless of whether any StopEarly exception occurs, AST is usable.
  return backend (proj);
}

// eof
