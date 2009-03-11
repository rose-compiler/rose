// This file is part of SATIrE. It defines a data structure for access to
// PAG's interprocedural contexts, and the corresponding call strings.

#ifndef H_CONTEXT
#define H_CONTEXT

#if HAVE_PAG

#include <vector>
#include <map>
#include <ostream>

#include "termite.h"

class CFG;

class ContextInformation
{
public:
    class Context;
    class CallSite;
    class CallString;

    const std::vector<Context> &allContexts() const;
    const CallString &contextCallString(const Context &c) const;
    bool isSpontaneousContext(const Context &c) const;
    const std::vector<Context> &parentContexts(const Context &c) const;

    void print(std::ostream &stream) const;
    PrologTerm *toPrologTerm() const;
    void printContextGraph(std::ostream &stream) const;

    ContextInformation(CFG *icfg);

private:
    CFG *icfg;
    std::vector<Context> contexts;
    std::map<Context, CallString> callstrings;
    std::map<Context, std::vector<Context> > parents;

    void computeParentInfo(const Context &c);
};

// Only include cfg_support.h here; this has to do with circular
// dependencies between this file and that one. Life is hard.
#include "cfg_support.h"

class ContextInformation::Context
{
public:
    int procnum;
    int position;

    std::string procName;
    std::string toString() const;
    PrologTerm *toPrologTerm() const;

    Context(int procnum, int position, CFG *icfg);
};

bool
operator<(const ContextInformation::Context &a,
          const ContextInformation::Context &b);

class ContextInformation::CallSite

{
public:
    int procnum;    // the number of the calling procedure
    int node_id;    // the number of the call node
    int target_procnum; // the number of the called procedure

    std::string callerName;
    std::string targetName;

    CallSite(int procnum, int node_id, int target_procnum, CFG *icfg);
};

class ContextInformation::CallString
{
public:
 // and g calls h, then this will contain the call sites <f, g>, <g, h> in
 // this order.
    std::vector<CallSite> callstring;

    CallString(int procnum, int position, CFG *icfg);
    std::string toString() const;
    PrologTerm *toPrologTerm() const;
};

#endif

#endif
