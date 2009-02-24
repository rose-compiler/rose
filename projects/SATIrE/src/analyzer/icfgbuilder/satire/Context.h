// This file is part of SATIrE. It defines a data structure for access to
// PAG's interprocedural contexts, and the corresponding call strings.

#ifndef H_CONTEXT
#define H_CONTEXT

#if HAVE_PAG

class Context;

#include "cfg_support.h"

class CallSite
{
public:
    int procnum;    // the number of the calling procedure
    int node_id;    // the number of the call node
    int target_procnum; // the number of the called procedure

    std::string callerName;
    std::string targetName;

    CallSite(int procnum, int node_id, int target_procnum, CFG *icfg);
};

class Context
{
public:
    int procnum;
    int position;

    std::string procName;

 // This is the sequence of call sites, callers first. I.e., if f calls g,
 // and g calls h, then this will contain the call sites <f, g>, <g, h> in
 // this order.
    typedef std::vector<CallSite> CallString;
    CallString callstring;

    Context(int procnum, int position, CFG *icfg);

    bool isSpontaneous() const;
    std::string toString() const;
};

#endif

#endif
