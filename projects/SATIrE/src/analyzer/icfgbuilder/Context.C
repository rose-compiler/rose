// This file is part of SATIrE. It defines a data structure for access to
// PAG's interprocedural contexts, and the corresponding call strings.

// We need to separate a few entities:
// - Context: A PAG context. Consists of a procnum and a position.
// - CallSite: A PAG call site, sort of. Has a procnum, a call node within
//      that procedure, and the procnum of the target. Does not have the
//      position (context) of the target. Can there be more than one target
//      context? Does it matter? (No to both, I think right now.)
// - CallString: A sequence of call sites.
// - Parent context: One of the several (!) possible contexts in the calling
//      procedure. Has a call node id and a position; the call node id is
//      always the same, I think. The position matters. In essence, this is
//      a Context + the call node. Which is important to locate
//      call-specific stuff. But we can also recover the call node from the
//      call string!
// Important: Do not store the call string inside the context. We will have
// a number of (equal) context objects floating around, and having that many
// call strings is not necessary. Mappings we need:
// - (int, int) -> Context
// - Context -> CallString
// - Context -> ParentContexts
// - CallString -> sequence of CallSites
// - something for CallSite <-> Context ?


#include "Context.h"

#if HAVE_PAG

// PAG header
#include "mapping.h"

const std::vector<ContextInformation::Context> &
ContextInformation::allContexts() const
{
    return contexts;
}

const ContextInformation::CallString &
ContextInformation::contextCallString(
        const ContextInformation::Context &c) const
{
    return callstrings.find(c)->second;
}

bool
ContextInformation::isSpontaneousContext(
        const ContextInformation::Context &c) const
{
    return contextCallString(c).callstring.empty();
}

const std::vector<ContextInformation::Context> &
ContextInformation::parentContexts(
        const ContextInformation::Context &c) const
{
    return parents.find(c)->second;
}

const std::vector<ContextInformation::Context> &
ContextInformation::childContexts(int id, const Context &c) const
{
 // This find form is more complicated than [], but it appears to be
 // necessary to preserve constness.
    std::map<std::pair<int, Context>, std::vector<Context> >::const_iterator p;
    p = children.find(std::make_pair(id, c));
    if (p == children.end())
    {
        std::cerr
            << "panic! could not find ("
            << id << ", " << c.toString() << ") in children!"
            << std::endl;
        std::abort();
    }
    return p->second;
}

void
ContextInformation::print(std::ostream &stream) const
{
    std::vector<Context>::const_iterator c;
    for (c = contexts.begin(); c != contexts.end(); ++c)
    {
        stream << c->toString() << ": ";
        stream << contextCallString(*c).toString();
        stream << std::endl;
    }
    std::map<std::pair<int, Context>, std::vector<Context> >
        ::const_iterator chld;
    for (chld = children.begin(); chld != children.end(); ++chld)
    {
        stream
            << "node " << chld->first.first
            << " in context " << chld->first.second.toString()
            << " may call to:";
        const std::vector<Context> &cs = chld->second;
        for (c = cs.begin(); c != cs.end(); ++c)
            stream << " " << c->toString();
        stream << std::endl;
    }
}

PrologTerm *
ContextInformation::toPrologTerm() const
{
    PrologList *callStrings = new PrologList();
    std::reverse_iterator<std::vector<Context>::const_iterator> c;
    for (c = contexts.rbegin(); c != contexts.rend(); ++c)
    {
        PrologTerm *context = c->toPrologTerm();
        PrologTerm *callString = contextCallString(*c).toPrologTerm();
        PrologInfixOperator *colon = new PrologInfixOperator(":");
        colon->addSubterm(context);
        colon->addSubterm(callString);
        callStrings->addFirstElement(colon);
    }
    return callStrings;
}

void
ContextInformation::printContextGraph(std::ostream &stream) const
{
    stream << "digraph contextGraph {" << std::endl;
    std::vector<Context>::const_iterator c, p;
    for (c = contexts.begin(); c != contexts.end(); ++c)
    {
        const std::vector<Context> &parents = parentContexts(*c);
        for (p = parents.begin(); p != parents.end(); ++p)
        {
         // Note: In the graph, arrows are from parent to child, i.e., from
         // caller to callee -- as in a call graph. But conceptually, this
         // is sort of the wrong way round, since during analysis we are
         // more interested in going from callee to caller, i.e., up the
         // call chain. Still, somehow it looks more natural this way.
            stream
                << '"' << p->toString() << '"' << " -> "
                << '"' << c->toString() << '"' << ";" << std::endl;
        }
    }
    stream << "}" << std::endl;
}

ContextInformation::ContextInformation(CFG *icfg)
  : icfg(icfg)
{
    int procs = kfg_num_procs(icfg);
    for (int p = 0; p < procs; p++)
    {
        KFG_NODE entry = kfg_numproc(icfg, p);
        int arity = kfg_arity_id(kfg_get_id(icfg, entry));
        for (int pos = 0; pos < arity; pos++)
        {
            Context c = Context(p, pos, icfg);
            contexts.push_back(c);
            callstrings.insert(std::make_pair(c, CallString(p, pos, icfg)));
            computeParentInfo(c);
        }
    }
}

void
ContextInformation::computeParentInfo(const ContextInformation::Context &c)
{
    int entry_id = kfg_get_id(icfg, kfg_numproc(icfg, c.procnum));
    EPREDS e = _mapping_extpred[entry_id][c.position];
#if DEBUG
    std::cout
        << "context info ----------" << std::endl
        << c.toString() << std::endl
        << "e.length: " << e.length
        << std::endl;
#endif
    std::vector<Context> &parentContexts = parents[c];
    for (int i = 0; i < e.length; i++)
    {
        int call_id = e.node[i];
        int call_pos = e.pos[i];

        int caller_procnum = kfg_procnum(icfg, call_id);
        std::string caller_name = kfg_proc_name(icfg, caller_procnum);
#if DEBUG
        std::cout
            << "\tpossible predecessor: "
            << caller_name << "/" << call_id << "/" << call_pos
            << std::endl;
#endif
        Context callerContext(caller_procnum, call_pos, icfg);
        parentContexts.push_back(callerContext);
        children[std::make_pair(call_id, callerContext)].push_back(c);
    }
}


std::string
ContextInformation::Context::toString() const
{
    std::stringstream str;
    str << procName << "/" << procnum << "/" << position;
    return str.str();
}

PrologTerm *
ContextInformation::Context::toPrologTerm() const
{
    PrologCompTerm *t = new PrologCompTerm("name_procnum_pos");
    t->addSubterm(new PrologAtom(procName));
    t->addSubterm(new PrologInt(procnum));
    t->addSubterm(new PrologInt(position));
    return t;
}

ContextInformation::Context::Context(int procnum, int position, CFG *icfg)
  : procnum(procnum), position(position),
    procName(kfg_proc_name(icfg, procnum))
{
}

bool
operator<(const ContextInformation::Context &a,
          const ContextInformation::Context &b)
{
    return (a.procnum < b.procnum
            || (a.procnum == b.procnum && a.position < b.position));
}


ContextInformation::CallSite::CallSite(
        int procnum, int node_id, int target_procnum, CFG *icfg)
  : procnum(procnum), node_id(node_id), target_procnum(target_procnum),
    callerName(kfg_proc_name(icfg, procnum)),
    targetName(kfg_proc_name(icfg, target_procnum))
{
}

ContextInformation::CallString::CallString(
        int procnum, int position, CFG *icfg)
  : callstring()
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

std::string
ContextInformation::CallString::toString() const
{
    std::stringstream str;

    if (callstring.empty())
    {
        str << "<spontaneous>";
    }
    else
    {
        std::vector<CallSite>::const_iterator i = callstring.begin();
        std::vector<CallSite>::const_iterator prev = i;
        while (i != callstring.end())
        {
            str << i->callerName << "/" << i->node_id << "  ->  ";
            if (prev != i && prev->target_procnum != i->procnum)
            {
                std::cerr
                    << "*** SATIrE callstring internal consistency error: "
                    << prev->procnum << "->" << prev->target_procnum << " / "
                    << i->procnum << "->" << i->target_procnum << std::endl;
                std::abort();
            }
            prev = i;
            ++i;
        }
     // When we get here, i == callstring.end(); thus, prev is the last
     // CallSite in the callstring (note that we ensured above that the
     // callstring is non-empty). The last procedure is printed specially.
        str << prev->targetName;
    }

    return str.str();
}

PrologTerm *
ContextInformation::CallString::toPrologTerm() const
{
    PrologList *callString = new PrologList();

    if (!callstring.empty())
    {
        std::reverse_iterator<std::vector<CallSite>::const_iterator> i;
        i = callstring.rbegin();

        PrologCompTerm *target = new PrologCompTerm("target");
        target->addSubterm(new PrologAtom(i->targetName));
        callString->addFirstElement(target);

        while (i != callstring.rend())
        {
            PrologCompTerm *site = new PrologCompTerm("caller_callsite");
            site->addSubterm(new PrologAtom(i->callerName));
            site->addSubterm(new PrologInt(i->node_id));
            callString->addFirstElement(site);
            ++i;
        }
    }

    return callString;
}

#endif
