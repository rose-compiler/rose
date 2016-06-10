// outlineIfs.cc: Calls Outliner directly to outline if statements.
#include <rose.h>
#include <iostream>
#include <set>
#include <list>

#include <Outliner.hh>

using namespace std;

// Traversal to gather all outlineable SgIfStmt nodes.
class CollectOutlineableIfs : public AstSimpleProcessing
{
public:
  // Container of list statements in ``outlineable'' order.
  typedef list<SgIfStmt *> IfList_t;

  // Call this routine to gather the outline targets.
  static void collect (SgProject* p, IfList_t& final)
  {
    CollectOutlineableIfs collector (final);
    collector.traverseInputFiles (p, postorder);
  }

  virtual void visit (SgNode* n)
  {
    SgIfStmt* s = isSgIfStmt (n);
    if (Outliner::isOutlineable (s))
      final_targets_.push_back (s);
  }

private:
  CollectOutlineableIfs (IfList_t& final) : final_targets_ (final) {}
  IfList_t& final_targets_; // Final list of outline targets.
};

//===================================================================
int main (int argc, char* argv[])
{
  // Initialize and check compatibility. See rose::initialize
  ROSE_INITIALIZE;

  SgProject* proj = frontend (argc, argv);
  ROSE_ASSERT (proj);

#if 1
  // Build a set of outlineable if statements.
  CollectOutlineableIfs::IfList_t ifs;
  CollectOutlineableIfs::collect (proj, ifs);

  // Outline them all.
  for (CollectOutlineableIfs::IfList_t::iterator i = ifs.begin ();
       i != ifs.end (); ++i)
    Outliner::outline (*i);
#else
  printf ("Skipping outlining due to recent move from std::list to std::vector in ROSE \n");
#endif

  // Unparse
  return backend (proj);
}
