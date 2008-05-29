

#include "broadway.h"

using namespace std;

actionChanger::actionChanger(Analyzer * analyzer, Annotations * annotations)
  : Changer(Preorder, Subtree, true),
    _analyzer(analyzer->property_analyzer()),
    linker(analyzer->getLinker()),
    _annotations(annotations) { }


Node * actionChanger::at_threeAddr(threeAddrNode * the_3addr, Order ord)
{
  const procedures_map & procs = _annotations->procedures();

  // -- Figure out which procedure is being called

  procedureAnn * proc_anns = 0;
  callsiteEntry * callsite = 0;

  for (procedures_map_cp p = procs.begin();
       p != procs.end();
       ++p)
    {
      proc_anns = (*p).second;
      callsite = proc_anns->find_callsite(the_3addr);
      if (callsite)
        break;
    }

  if (callsite) {

    // -- If we found a library call, see if there is an applicable
    // action.

    actionAnn * action = proc_anns->find_applicable_action(the_3addr);

    // -- If there is one, generate the replacement code

    if (action) {
      stmtNode * repl = action->generate_replacement(callsite, _analyzer,
                                                     linker, _annotations);
      if (repl) {

        // -- Show the original code in a comment

        ostringstream ostr;
        ostr << " -- Action: ";
        if (action->condition())
          action->condition()->print(ostr);
        output_context oc(ostr);
        the_3addr->output(oc, 0);

        repl->comment() = ostr.str();

        if (Annotations::Show_actions) {
          cout << " -- Action: ";
          if (action->condition())
            action->condition()->print(cout);
          cout << " --------------------------------------------" << endl;

          output_context oc(cout);
          cout << "   Original code:" << endl;
          the_3addr->output(oc, 0);
          cout << endl;

          cout << "   Replacement code:" << endl;
          repl->output(oc, 0);
          cout << endl;
        }

        // -- Return the new code

        return repl;
      }
    }
  }

  return the_3addr;
}
