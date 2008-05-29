
#include "broadway.h"
#include "bdwy_liveness.h"

using namespace std;

bdwyLivenessAnalyzer::bdwyLivenessAnalyzer()
  : livenessAnalyzer(),
    _library_stmt(0),
    _automatically_live(false)
{}

void bdwyLivenessAnalyzer::clear()
{
  livenessAnalyzer::clear();
}

void bdwyLivenessAnalyzer::record_defs(procLocation * libproc_location,
				       procedureAnn * library_procedure)
{
  // -- Any modification to an I/O variable is never dead

  if (library_procedure->allocates_memory() ||
      library_procedure->deallocates_memory() ||
      library_procedure->accesses_io()) {
    _automatically_live = true;
  }
  else {

    _automatically_live = false;

    // -- Record the defs in this library call

    const var_set & var_defs = library_procedure->defs();

    for (var_set_cp p = var_defs.begin();
	 p != var_defs.end();
	 ++p)
      {
	annVariable * var = (*p);

	// -- Get the current variable binding

	pointerValue & ptrs = var->get_binding(libproc_location);

	// -- Record the live variables

	collectDefs(ptrs);
      }
  }

  // -- Also, store the special library statement location

  _library_stmt = library_procedure->def_use_location(libproc_location);
}

void bdwyLivenessAnalyzer::at_threeAddr(stmtLocation * stmt,
				        threeAddrNode * threeaddr,
				        pointerValue & result)
{
  // -- Regular expression statement: test the defs to see if the uses that
  // they reach are live.

  bool live = false;

  if (determineLiveness() || _automatically_live) {

    setLive(stmt);

    live = true;

    if (_library_stmt) {

      // -- Important: For uses that occur inside the annotations, we need to
      // make sure that the synthetic statement location created for those
      // uses is also marked:

      setLive(_library_stmt);
    }
  }

  /*
  if (live)
    cout << "    ...Live" << endl;
  else
    cout << "    ...Dead" << endl;
  */

  _library_stmt = 0;
  _automatically_live = false;
}

