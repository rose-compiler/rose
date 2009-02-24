// This file is part of SATIrE. It defines a data structure for access to
// PAG's interprocedural contexts, and the corresponding call strings.

#include "Context.h"

#if HAVE_PAG

// PAG header
#include "mapping.h"

CallSite::CallSite(int procnum, int node_id, int target_procnum, CFG *icfg)
  : procnum(procnum), node_id(node_id), target_procnum(target_procnum),
    callerName(kfg_proc_name(icfg, procnum)),
    targetName(kfg_proc_name(icfg, target_procnum))
{
}


Context::Context(int procnum, int position, CFG *icfg)
  : procnum(procnum), position(position),
    procName(kfg_proc_name(icfg, procnum)), callstring()
{
    int *call_data;
    int length;

    if (sel_mapping == PAG_MAPPING_CALLSTRING_0)
    {
     // Nothing to do: All call strings are empty.
    }
    else if (sel_mapping == PAG_MAPPING_CALLSTRING_1
          || sel_mapping == PAG_MAPPING_CALLSTRING_N)
    {
        mapping_get_callstring(icfg, procnum, position, &call_data, &length);
        for (int i = 0; i + 1 < length; i += 2)
        {
         // call_data[i] is the call node, call_data[i+1] is the callee's
         // entry node
            int call_node_id = call_data[i];
            int caller_procnum = kfg_procnum(icfg, call_node_id);
            int target_procnum = kfg_procnum(icfg, call_data[i+1]);
            callstring.push_back(
                CallSite(caller_procnum, call_node_id, target_procnum, icfg));
        }
    }
    else
    {
        std::cerr
            << "*** SATIrE Context construction error: unsupported mapping '"
            << sel_mapping << "'" << std::endl
            << "*** only finite-length non-VIVU call strings are supported"
            << std::endl
            << "*** (maybe you should try the --no-compute-call-strings flag)"
            << std::endl;
        std::abort();
    }
}

bool
Context::isSpontaneous() const
{
    return callstring.empty();
}

std::string
Context::toString() const
{
    std::stringstream str;

    str << procName
        << "/" << procnum
        << "/" << position << ": ";

    if (callstring.empty())
    {
        str << "<spontaneous>";
    }
    else
    {
        CallString::const_iterator i = callstring.begin();
        CallString::const_iterator j = i + 1;
        while (j != callstring.end())
        {
            str << i->callerName << "/" << i->node_id << "  ->  ";
            if (i->target_procnum != j->procnum)
            {
                std::cerr
                    << "*** SATIrE callstring internal consistency error: "
                    << i->procnum << "->" << i->target_procnum << " / "
                    << j->procnum << "->" << j->target_procnum << std::endl;
                std::abort();
            }

            i = j;
            ++j;
        }
        if (i == callstring.begin())
        {
         // The call string contains exactly one call; print that one...
            str << i->callerName << "/" << i->node_id << "  ->  ";
        }
     // When we get here, j == callstring.end(); thus, i is the last
     // CallSite in the callstring (note that we ensured above that the
     // callstring is non-empty). The last procedure is printed specially.
        str << i->targetName;
    }

    return str.str();
}

#endif
