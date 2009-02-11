// attachMetrics.cc -- Sample translator showing how to attach
// HPCToolkit data to the ROSE AST.

#include "rose.h"

#include <iostream>
#include <list>
#include <rosehpct/rosehpct.hh>

using namespace std;

// Prints the estimated flops/cycle at a located node.
static void printFlopRate (const SgNode* n)
{
  const SgLocatedNode* n_loc = isSgLocatedNode (n);
  if (n_loc
      && n_loc->attributeExists ("PAPI_TOT_CYC")
      && n_loc->attributeExists ("PAPI_FP_OPS"))
    {
      // Extract attributes.
      const RoseHPCT::MetricAttr* cycles_attr =
	dynamic_cast<const RoseHPCT::MetricAttr *> (n->getAttribute ("PAPI_TOT_CYC"));
      const RoseHPCT::MetricAttr* flops_attr =
	dynamic_cast<const RoseHPCT::MetricAttr *> (n->getAttribute ("PAPI_FP_OPS"));
      ROSE_ASSERT (cycles_attr && flops_attr);

      // Get the metric values, as doubles and strings.
      double cycles = cycles_attr->getValue ();
      string cycles_s = const_cast<RoseHPCT::MetricAttr *> (cycles_attr)->toString ();
      double flops = flops_attr->getValue ();
      string flops_s = const_cast<RoseHPCT::MetricAttr *> (flops_attr)->toString ();

      // Print node pointer/type, parent, estimated flop rate, and source position.
      const SgNode* n_par = n_loc->get_parent ();
      cout << (const void *)n_loc << ":<" << n_loc->class_name () << ">"
	   << " (Par=" << (const void *)n_par << ":<" << n_par->class_name () << ">)"
	   << " = (" << flops_s << " flops)"
	   << " / (" << cycles_s << " cy)"
	   << " ~= " << flops / cycles << " flops/cy" << endl
	   << "  [" << n_loc->get_startOfConstruct ()->get_raw_filename ()
	   << ':' << n_loc->get_startOfConstruct ()->get_raw_line () << "]"
	   << endl;
    }
}

int main (int argc, char* argv[])
{
  vector<string> argvList(argv, argv+argc);
  cerr << "[Loading HPCToolkit profiling data...]" << endl;
  RoseHPCT::ProgramTreeList_t profiles
    = RoseHPCT::loadHPCTProfiles (argvList);

  cerr << "[Building the ROSE AST...]" << endl;
  SgProject* proj = frontend (argvList);

  cerr << "[Attaching HPCToolkit metrics to the AST...]" << endl;
  RoseHPCT::attachMetrics (profiles, proj, proj->get_verbose () > 0);

  cerr << "[Estimating flop execution rates...]" << endl;
  //typedef list<SgNode *> NodeList_t;
  //Liao (10/1/2007): Commented out as part of move from std::list to std::vector
  typedef Rose_STL_Container<SgNode *> NodeList_t; 

  NodeList_t estmts = NodeQuery::querySubTree (proj, V_SgExprStatement);
  for_each (estmts.begin (), estmts.end (), printFlopRate);

  NodeList_t for_inits = NodeQuery::querySubTree (proj, V_SgForInitStatement);
  for_each (for_inits.begin (), for_inits.end (), printFlopRate);

  NodeList_t fors = NodeQuery::querySubTree (proj, V_SgForStatement);
  for_each (fors.begin (), fors.end (), printFlopRate);

  cerr << "[Dumping a PDF...]" << endl;
  generatePDF (*proj);

// DQ (1/2/2008): This output appears to have provided enough synchronization in 
// the output to allow -j32 to work. Since I can't debug the problem further for 
// now I will leave it.
  cerr << "[Calling backend...]" << endl;

  return backend (proj);
}
