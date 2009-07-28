// This program uses the SATIrE framework to analyze programs and annotate
// them with comments in the form expected by AbsInt's aiT tool.
// The analyses are:
// - points-to analysis including resolution of function pointers
// - interval analysis
// The annotations are:
// - possible targets for function pointer calls
// - possible targets for pointer expressions
// - unreachable code
// The analyses are context-sensitive if the user asked for
// context-sensitivity; the program prints a friendly reminder if the user
// did not ask for context-sensitivity.

// TODO:
// - Allow the user to specify the entry point(s) of the analysis (this
//   should be a SATIrE feature, really).
// - Allow the user to specify a prefix for function names, for the case
//   where a compiler mangles function name "foo" into symbol "_foo".

#include <satire.h>

using namespace SATIrE;
using namespace SATIrE::Analyses;

enum where { before, after };

void addAisAnnotatorOptions(AnalyzerOptions *options);
void addAisAnnotations(Program *program);

int main(int argc, char **argv)
{
    AnalyzerOptions *options = extractOptions(argc, argv);

 // We augment (or override!) the command line options specified by the user
 // with a number of our own options. In particular, we force the points-to
 // and interval analyses to run, and we force output of source code. If no
 // context-sensitivity was requested, we inform the user.
    addAisAnnotatorOptions(options);

    Program *program = new Program(options);

 // The Program constructor called above conveniently ensured that our
 // analyzers have run; this is because analyses specified in the
 // command-line options are taken to be part of front-end processing of the
 // program. Thus, all we need to do now (this is the bulk of the work
 // written for this program) is to compute and add aiT annotations from the
 // analysis results.
    addAisAnnotations(program);

    outputProgramRepresentation(program, options);
}

static DataFlowAnalysis *globalIntervalAnalysis = NULL;

void addAisAnnotatorOptions(AnalyzerOptions *options)
{
    std::string programName
        = pathNameComponents(options->getProgramName()).back();
    if (options->getCallStringLength() == 0)
    {
        std::cout
            << programName << ": info: call string length is 0, "
                              "analysis is not context sensitive"
            << std::endl
            << programName << ": info: consider using --callstringlength=N "
                              "flag"
            << std::endl;
    }

    options->runPointsToAnalysisOn();
    options->resolveFuncPtrCallsOn();

 // Make sure there is an instance of the interval analysis in the analyzer
 // list of the AnalyzerOptions object.
    const std::vector<DataFlowAnalysis *> &analyzers
        = options->getDataFlowAnalyzers();
    std::vector<DataFlowAnalysis *>::const_iterator a;
    for (a = analyzers.begin(); a != analyzers.end(); ++a)
    {
        if ((*a)->identifier() == "interval")
        {
            globalIntervalAnalysis = *a;
            break;
        }
    }
    if (globalIntervalAnalysis == NULL)
    {
        DataFlowAnalysis *intervalAnalysis = makeProvidedAnalyzer("interval");
        options->appendDataFlowAnalysis(intervalAnalysis);
        globalIntervalAnalysis = intervalAnalysis;
    }

 // Don't add the usual data flow annotation comments. We only want the
 // aiT-specific annotation comments.
    options->analysisAnnotationOff();

    options->outputSourceOn();
    if (options->getOutputSourceFileName() == ""
     && options->getOutputFilePrefix() == "")
    {
        options->setOutputFilePrefix("ais_");
    }
}

void addContextAnnotations(Program *program);
void addUnreachabilityAnnotations(Program *program);
void addDataPointstoAnnotations(Program *program);
void addFunctionPointstoAnnotations(Program *program);

void addAisAnnotations(Program *program)
{
    addContextAnnotations(program);
    addUnreachabilityAnnotations(program);
    addDataPointstoAnnotations(program);
    addFunctionPointstoAnnotations(program);
}

void externalCallTargets(
        BasicBlock *bb, int procnum, PointsToAnalysis *pto, CFG *icfg,
        std::map<int, const std::list<SgFunctionSymbol *> *> &positionFuncMap);
std::string callSiteRegisterName(Procedure *);
std::string contextCondition(ContextInformation::Context context, CFG *icfg);
void addAisComment(std::string what, enum where where, SgStatement *stmt);

void addContextAnnotations(Program *program)
{
    // Use "call site registers" where @function_N_foobar_callsite = M if
    // ICFG node M can call to function foobar (with procnum N).
    // For non-recursive programs, we don't need an arbitrarily deep stack;
    // a table recording whether each function is on the stack suffices. If
    // there are several permutations of call paths to a function -- for
    // example, function h is reachable through f->g->h and g->f->h -- then
    // we need additional numbering (but actually only if f and g may be
    // called from the same site, through some pointer -- that seems highly
    // unlikely). Let's go for this if there is not much recursion in the
    // case study code!
    // TODO: Identify recursion in a program, and report how much recursion
    // there is in the case study code. Although possibly (think more about
    // this!) simply overwriting a function's call site register might be
    // benign; no condition will match, so aiT will assume conservative
    // information, which should be fine.
    class ContextAnnotator: private IcfgTraversal
    {
    public:
        void run()
        {
            traverse(icfg);
        }

        ContextAnnotator(Program *program)
          : program(program), icfg(program->icfg),
            pto(icfg->contextSensitivePointsToAnalysis)
        {
        }

    private:
        Program *program;
        CFG *icfg;
        PointsToAnalysis *pto;
        std::map<SgStatement *, std::map<Procedure *, std::set<int> > >
            callSiteMap;

        void icfgVisit(SgNode *node)
        {
            if (isExternalCall(node))
            {
                SgStatement *stmt = icfg->labeledStatement(get_node_id());
                BasicBlock *bb = icfg->nodes[get_node_id()];
                int procnum = get_node_procnum();
                std::map<int, const std::list<SgFunctionSymbol *> *> map;
                externalCallTargets(bb, procnum, pto, icfg, map);
                std::map<int, const std::list<SgFunctionSymbol *> *>::iterator
                    pos;
                for (pos = map.begin(); pos != map.end(); ++pos)
                {
                 // pos->first is the context number; we will ignore it
                 // here. Strictly speaking, we would not need to add
                 // call site register annotations for calls that are
                 // infeasible in the current context; but on the other
                 // hand, it shouldn't hurt.
                    std::list<SgFunctionSymbol *>::const_iterator f;
                    for (f=pos->second->begin(); f!=pos->second->end(); ++f)
                    {
                        std::string func = (*f)->get_name().str();
                        recordCallSite(stmt, func, get_node_id());
                    }
                }
            }
            else if (FunctionCall *call = isFunctionCall(node))
            {
                SgStatement *stmt = icfg->labeledStatement(get_node_id());
                std::string func = call->get_funcname();
                recordCallSite(stmt, func, get_node_id());
            }
        }

        void atIcfgTraversalEnd()
        {
         // annotate the collected statements with the collected
         // information: set (and reset?) call site registers
            std::map<SgStatement *, std::map<Procedure *, std::set<int> > >
                :: iterator site;
            for (site = callSiteMap.begin(); site != callSiteMap.end(); ++site)
            {
                SgStatement *stmt = site->first;
                std::map<Procedure *, std::set<int> > &targets = site->second;
                std::map<Procedure *, std::set<int> >::iterator tgt;
                for (tgt = targets.begin(); tgt != targets.end(); ++tgt)
                {
                    std::stringstream annotation;
                    if (tgt->second.size() == 1)
                    {
                        int node = *tgt->second.begin();
                        annotation
                            << "instruction here is entered with "
                            << callSiteRegisterName(tgt->first)
                            << " = " << node << ";";
                    }
                    else if (tgt->second.size() > 1)
                    {
                        int min = *tgt->second.begin();
                        int max = *tgt->second.rbegin();
                        annotation
                            << "instruction here is entered with "
                            << callSiteRegisterName(tgt->first)
                            << " = " << min << ".." << max << ";";
                    }
                    addAisComment(annotation.str(), before, stmt);
                }
            }
        }

        void recordCallSite(SgStatement *stmt, std::string func, int node_id)
        {
            std::multimap<std::string, Procedure *>::iterator
                p,
                low = icfg->proc_map.lower_bound(func),
                high = icfg->proc_map.upper_bound(func);
            for (p = low; p != high; ++p)
            {
                callSiteMap[stmt][p->second].insert(node_id);
            }
        }
    };

    ContextAnnotator ca(program);
    ca.run();
}

void addUnreachabilityAnnotations(Program *program)
{
    class UnreachabilityAnnotator: private IcfgTraversal
    {
    public:
        void run()
        {
            traverse(icfg);
        }

        UnreachabilityAnnotator(Program *program, DataFlowAnalysis *interval)
          : program(program), icfg(program->icfg), interval(interval)
        {
        }

    private:
        Program *program;
        CFG *icfg;
        DataFlowAnalysis *interval;

        void icfgVisit(SgNode *node)
        {
            if (isSgIfStmt(node))
            {
                BasicBlock *if_bb = icfg->nodes[get_node_id()];
                KFG_NODE_LIST succs = kfg_successors(icfg, if_bb);
                while (!kfg_node_list_is_empty(succs))
                {
                    BasicBlock *bb = (BasicBlock *) kfg_node_list_head(succs);
                    KFG_EDGE_TYPE e = kfg_edge_type(if_bb, bb);
                    SgStatement *icfgStmt = bb->statements[0];
                    if (!isIfJoin(icfgStmt)
                     && icfg->nodeHasCorrespondingAstStatement(bb->id))
                    {
                        SgStatement *astStmt = icfg->labeledStatement(bb->id);
                        annotateReachability(astStmt, bb);
                    }
                    succs = kfg_node_list_tail(succs);
                }
            }
        }

        void annotateReachability(SgStatement *stmt, BasicBlock *bb)
        {
            int arity = kfg_arity_id(bb->id);
            for (int position = 0; position < arity; position++)
            {
                bool reachable = true;
                bool queryResult
                    = interval->query("nodeIsReachable(int block_id, "
                                      "int context, bool *reachable)",
                                      bb->id, position, &reachable);
                if (!reachable)
                {
                    std::stringstream annotation;
                    annotation
                        << "snippet here is never executed";
                    std::string condition
                        = contextCondition(ContextInformation::Context(
                                    bb->procnum, position, icfg), icfg);
                    if (condition != "")
                        annotation << " if " << condition;
                    annotation
                        << ";";
                    addAisComment(annotation.str(), before, stmt);
                }
            }
        }
    };

    UnreachabilityAnnotator ua(program, globalIntervalAnalysis);
    ua.run();
}

void addDataPointstoAnnotations(Program *program)
{
    class DataPointstoAnnotator: private IcfgTraversal,
                                 private AstSimpleProcessing
    {
    public:
        void run()
        {
            IcfgTraversal::traverse(icfg);
        }

        DataPointstoAnnotator(Program *program)
          : program(program), icfg(program->icfg),
            pto(icfg->contextSensitivePointsToAnalysis),
            globalVarsyms(icfg->globals.begin(), icfg->globals.end()),
            currentContext(0, 0, icfg)
        {
        }

    private:
        Program *program;
        CFG *icfg;
        PointsToAnalysis *pto;

        std::set<ContextInformation::Context> contextsWithNonglobalDerefs;
        std::map<ContextInformation::Context,
                 std::set<SgVariableSymbol *> > contextDerefMap;
        std::set<SgVariableSymbol *> globalVarsyms;
        ContextInformation::Context currentContext;

        void icfgVisit(SgNode *node)
        {
            if (is_icfg_statement())
            {
                int arity = kfg_arity_id(get_node_id());
                for (int position = 0; position < arity; position++)
                {
                    currentContext = ContextInformation::Context(
                            get_node_procnum(), position, icfg);
                 // no need to traverse in contexts that we have already
                 // given up on
                    if (contextsWithNonglobalDerefs.find(currentContext) !=
                        contextsWithNonglobalDerefs.end())
                        continue;
                    else
                        AstSimpleProcessing::traverse(node, preorder);
                }
            }
        }

        void visit(SgNode *node)
        {
         // inside an AST fragment
            switch (node->variantT())
            {
            case V_SgPointerDerefExp:
                dereferenceChild(node, SgPointerDerefExp_operand_i);
                break;
            case V_SgPntrArrRefExp:
             // assume the pointer operand is on the lhs
                dereferenceChild(node, SgPntrArrRefExp_lhs_operand_i);
                break;
            case V_SgArrowExp:
                dereferenceChild(node, SgArrowExp_lhs_operand_i);
                break;
            }
        }

        void dereferenceChild(SgNode *node, size_t index)
        {
            SgExpression *expression
                = isSgExpression(node->get_traversalSuccessorByIndex(index));

         // only consider dereferences of pointers (not arrays)
            SgType *t = expression->get_type()->stripType(
                                            SgType::STRIP_MODIFIER_TYPE
                                          | SgType::STRIP_REFERENCE_TYPE
                                          | SgType::STRIP_TYPEDEF_TYPE);
            if (!isSgPointerType(t))
                return;

            PointsToAnalysis::Location *loc
                = pto->base_location(
                        pto->expressionLocation(expression, currentContext));
            const std::list<SgSymbol *> &symbols = pto->location_symbols(loc);
            std::list<SgSymbol *>::const_iterator s;
            for (s = symbols.begin(); s != symbols.end(); ++s)
            {
                if (SgVariableSymbol *varsym = isSgVariableSymbol(*s))
                {
                    if (globalVarsyms.find(varsym) != globalVarsyms.end())
                        contextDerefMap[currentContext].insert(varsym);
                    else
                        contextsWithNonglobalDerefs.insert(currentContext);
                }
            }
        }

        void atIcfgTraversalEnd()
        {
            std::map<ContextInformation::Context,
                     std::set<SgVariableSymbol *> >::const_iterator i;
            for (i = contextDerefMap.begin(); i != contextDerefMap.end(); ++i)
            {
                const ContextInformation::Context &context = i->first;
                const std::set<SgVariableSymbol *> &variables = i->second;
                if (contextsWithNonglobalDerefs.find(context) ==
                    contextsWithNonglobalDerefs.end())
                {
                    std::stringstream annotation;
                    annotation
                        << "accesses default \"" << context.procName
                        << "\" to ";
                    std::set<SgVariableSymbol *>::const_iterator v;
                    v = variables.begin();
                    while (v != variables.end())
                    {
                        SgVariableSymbol *sym = *v;
                        annotation << '"' << sym->get_name().str() << '"';
                        if (++v != variables.end())
                            annotation << ", ";
                    }
                    std::string condition = contextCondition(context, icfg);
                    if (condition != "")
                        annotation << " if " << condition;
                    annotation << ";";
                    Procedure *p = (*icfg->procedures)[context.procnum];
                    SgFunctionDeclaration *decl
                        = isSgFunctionDeclaration(
                                p->decl->get_definingDeclaration());
                    SgStatement *functionBody
                        = decl->get_definition()->get_body();
                    addAisComment(annotation.str(), before, functionBody);
                }
            }
        }
    };

    DataPointstoAnnotator dpa(program);
    dpa.run();
}

void addFunctionPointstoAnnotations(Program *program)
{
    class FunctionPointstoAnnotator: private IcfgTraversal
    {
    public:
        void run()
        {
            traverse(icfg);
        }

        FunctionPointstoAnnotator(Program *program)
          : program(program), icfg(program->icfg),
            pto(icfg->contextSensitivePointsToAnalysis)
        {
        }

    private:
        Program *program;
        CFG *icfg;
        PointsToAnalysis *pto;

        void icfgVisit(SgNode *node)
        {
            if (ExternalCall *call = isExternalCall(node))
            {
                BasicBlock *bb = icfg->nodes[get_node_id()];
                int procnum = get_node_procnum();
                std::map<int, const std::list<SgFunctionSymbol *> *> map;
                externalCallTargets(bb, procnum, pto, icfg, map);

                std::map<int, const std::list<SgFunctionSymbol *> *>::iterator
                    pos;
                for (pos = map.begin(); pos != map.end(); ++pos)
                {
                    std::stringstream annotation;
                    annotation << "instruction here + 1 call calls ";

                    std::list<SgFunctionSymbol *>::const_iterator f;
                    f = pos->second->begin();
                    while (f != pos->second->end())
                    {
                        SgFunctionSymbol *sym = *f;
                        annotation << '"' << sym->get_name().str() << '"';
                        if (++f != pos->second->end())
                            annotation << ", ";
                    }

                    std::string condition =
                        contextCondition(ContextInformation::Context(
                                    bb->procnum, pos->first, icfg), icfg);
                    if (condition != "")
                        annotation << " if " << condition;
                    annotation << ";";

                    SgStatement *stmt = icfg->labeledStatement(get_node_id());
                    addAisComment(annotation.str(), before, stmt);
                }
            }
        }
    };

    FunctionPointstoAnnotator fpa(program);
    fpa.run();
}

void externalCallTargets(
        BasicBlock *bb, int procnum, PointsToAnalysis *pto, CFG *icfg,
        std::map<int, const std::list<SgFunctionSymbol *> *> &positionFuncMap)
{
    SgExpression *callTarget = bb->call_target;
    if (callTarget == NULL)
    {
        std::cerr
            << "*** error: unknown call target for external call "
            << " node " << bb->id << " in procedure " << procnum
            << std::endl;
    }
    int arity = kfg_arity_id(bb->id);
    int position;
    for (position = 0; position < arity; position++)
    {
        ContextInformation::Context ctx(procnum, position, icfg);
        PointsToAnalysis::Location *targetLocation
            = pto->base_location(
                    pto->expressionLocation(callTarget, ctx));
        const std::list<SgFunctionSymbol *> &fs
            = pto->location_funcsymbols(targetLocation);
        if (fs.empty())
        {
            std::cerr
                << "*** error: no call targets for external call "
                << Ir::fragmentToString(callTarget)
                << " node " << bb->id << " in procedure " << procnum
                << std::endl;
            std::abort();
        }
        positionFuncMap[position] = &fs;
    }
}

std::string callSiteRegisterName(Procedure *p)
{
    std::stringstream name;
    name
        << "@routine_" << p->procnum << "_" << p->name
        << "_callsite";
    return name.str();
}

std::string contextCondition(ContextInformation::Context context, CFG *icfg)
{
    std::stringstream result;
    const ContextInformation::CallString &callString
        = icfg->contextInformation->contextCallString(context);
    std::vector<ContextInformation::CallSite>::const_iterator site;
    site = callString.callstring.begin();
    while(site != callString.callstring.end())
    {
        Procedure *proc = (*icfg->procedures)[site->target_procnum];
        result
            << callSiteRegisterName(proc) << " = "
            << site->node_id;

        if (++site != callString.callstring.end())
            result << " and ";
    }
    return result.str();
}

void addAisComment(std::string what, enum where where, SgStatement *stmt)
{
    PreprocessingInfo::RelativePositionType position
        = (where == before ? PreprocessingInfo::before
                           : PreprocessingInfo::after);
    Sg_File_Info *fi = stmt->get_startOfConstruct();
    std::string fileName = "compilerGenerated";
    int lineNo = 0;
    int colNo = 0;
    if (fi != NULL)
    {
        fileName = fi->get_filename();
        lineNo = fi->get_line();
        colNo = fi->get_col();
    }
    PreprocessingInfo *commentInfo
        = new PreprocessingInfo(PreprocessingInfo::C_StyleComment,
                                "/* ai: " + what + " */",
                                fileName, lineNo, colNo,
                                /* number of lines = */ 1, position);
    stmt->addToAttachedPreprocessingInfo(commentInfo);

#if LOG
    std::cout
        << "* " << fileName << ":" << lineNo << ":" << colNo << ": "
        << what
        << std::endl;
#endif
}
