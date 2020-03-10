#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "BinarySmtSolver.h"

#include "rose_getline.h"
#include "BinarySmtlibSolver.h"
#include "BinaryYicesSolver.h"
#include "BinaryZ3Solver.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/tuple/tuple.hpp>
#include <fcntl.h> /*for O_RDWR, etc.*/
#include <Sawyer/FileSystem.h>
#include <Sawyer/LineVector.h>
#include <Sawyer/Stopwatch.h>

// Many of the expression-creating calls pass NO_SOLVER in order to not invoke the solver recursively.
#define NO_SOLVER SmtSolverPtr()

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility SmtSolver::mlog;

bool
CompareLeavesByName::operator()(const SymbolicExpr::LeafPtr &a, const SymbolicExpr::LeafPtr &b) const {
    if (!a || !b)
        return !a && b;                                 // null a is less than non-null b; other null combos return false
    ASSERT_require(a->isVariable2() || a->isMemoryExpr());
    ASSERT_require(b->isVariable2() || b->isMemoryExpr());
    if (a->isMemoryExpr() != b->isMemoryExpr())
        return !a->isMemoryExpr();                      // memory expressions come after other things
    return a->nameId() < b->nameId();
}

// class method
void
SmtSolver::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::SmtSolver");
        mlog.comment("invoking a satisfiability modulo theory solver");
    }
}

std::ostream&
operator<<(std::ostream &o, const SmtSolver::Exception &e) {
    return o <<"SMT solver: " <<e.what();
}

std::ostream&
operator<<(std::ostream &o, const SmtSolver::SExpr &e) {
    e.print(o);
    return o;
}

SmtSolver::Stats SmtSolver::classStats;
boost::mutex SmtSolver::classStatsMutex;

void
SmtSolver::init(unsigned linkages) {
    linkage_ = bestLinkage(linkages);
    stack_.push_back(std::vector<SymbolicExpr::Ptr>());

    if (linkage_ == LM_LIBRARY) {
        name_ = std::string(name_.empty()?"noname":name_) + "-lib";
    } else if (linkage_ == LM_EXECUTABLE) {
        name_ = std::string(name_.empty()?"noname":name_) + "-exe";
    }

    {
        boost::lock_guard<boost::mutex> lock(classStatsMutex);
        ++classStats.nSolversCreated;
    }
}

SmtSolver::~SmtSolver() {
    resetStatistics();
    boost::lock_guard<boost::mutex> lock(classStatsMutex);
    ++classStats.nSolversDestroyed;
}
    
void
SmtSolver::reset() {
    if (errorIfReset_)
        throw Exception("reset not allowed for this solver");
    stack_.clear();
    push();
    clearEvidence();
    latestMemoizationId_ = 0;
    // stats not cleared
}

void
SmtSolver::clearEvidence() {
    outputText_ = "";
    parsedOutput_.clear();
    termNames_.clear();
}

// class method
SmtSolver::Availability
SmtSolver::availability() {
    SmtSolver::Availability retval;
    retval.insert(std::make_pair(std::string("z3-lib"), (Z3Solver::availableLinkages() & LM_LIBRARY) != 0));
    retval.insert(std::make_pair(std::string("z3-exe"), (Z3Solver::availableLinkages() & LM_EXECUTABLE) != 0));
    retval.insert(std::make_pair(std::string("yices-lib"), (YicesSolver::availableLinkages() & LM_LIBRARY) != 0));
    retval.insert(std::make_pair(std::string("yices-exe"), (YicesSolver::availableLinkages() & LM_EXECUTABLE) != 0));
    return retval;
}

// class methd
SmtSolver::Ptr
SmtSolver::instance(const std::string &name) {
    if ("" == name || "none" == name)
        return Ptr();
    if ("best" == name)
        return bestAvailable();
    if ("z3-lib" == name)
        return Z3Solver::instance(LM_LIBRARY);
    if ("z3-exe" == name)
        return Z3Solver::instance(LM_EXECUTABLE);
    if ("yices-lib" == name)
        return YicesSolver::instance(LM_LIBRARY);
    if ("yices-exe" == name)
        return YicesSolver::instance(LM_EXECUTABLE);
    throw Exception("unrecognized SMT solver name \"" + StringUtility::cEscape(name) + "\"");
}

// class method
SmtSolver::Ptr
SmtSolver::bestAvailable() {
    // Binary APIs are faster, so prefer them
    if ((Z3Solver::availableLinkages() & LM_LIBRARY) != 0)
        return Z3Solver::instance(LM_LIBRARY);
    if ((YicesSolver::availableLinkages() & LM_LIBRARY) != 0)
        return YicesSolver::instance(LM_LIBRARY);

    // Next try text-based APIs
    if ((Z3Solver::availableLinkages() & LM_EXECUTABLE) != 0)
        return Z3Solver::instance(LM_EXECUTABLE);
    if ((YicesSolver::availableLinkages() & LM_EXECUTABLE) != 0)
        return YicesSolver::instance(LM_EXECUTABLE);

    return Ptr();
}

SmtSolver::LinkMode
SmtSolver::bestLinkage(unsigned linkages) {
    for (size_t i=0; i<8*sizeof(linkages); ++i) {
        unsigned bit = (1u << i);
        if ((linkages & bit) != 0)
            return (LinkMode)bit;
    }
    return LM_NONE;
}

void
SmtSolver::requireLinkage(LinkMode need) const {
    if ((linkage_ & need) == 0) {
        std::string mesg = name();
        if (mesg.empty())
            mesg = "unnamed";
        mesg += " solver";
        switch (need) {
            case LM_EXECUTABLE:
                mesg += " (executable mode)";
                break;
            case LM_LIBRARY:
                mesg += " (library mode)";
                break;
            default:
                mesg += " (unknown mode)";
                break;
        }
        mesg += " is not available";
        throw Exception(mesg);
    }
}

// class method
SmtSolver::Stats
SmtSolver::classStatistics() {
    boost::lock_guard<boost::mutex> lock(classStatsMutex);
    return classStats;
}

// class method
void
SmtSolver::resetClassStatistics() {
    boost::lock_guard<boost::mutex> lock(classStatsMutex);
    classStats = Stats();
}

void
SmtSolver::resetStatistics() {
    boost::lock_guard<boost::mutex> lock(classStatsMutex);
    classStats.ncalls += stats.ncalls;
    classStats.input_size += stats.input_size;
    classStats.output_size += stats.output_size;
    classStats.memoizationHits += stats.memoizationHits;
    classStats.prepareTime += stats.prepareTime;
    classStats.solveTime += stats.solveTime;
    classStats.evidenceTime += stats.evidenceTime;
    stats = Stats();
}

SmtSolver::Satisfiable
SmtSolver::triviallySatisfiable(const std::vector<SymbolicExpr::Ptr> &exprs) {
    reset();
    insert(exprs);
    return checkTrivial();
}

SmtSolver::Satisfiable
SmtSolver::satisfiable(const SymbolicExpr::Ptr &expr) {
    reset();
    insert(expr);
    Satisfiable retval = check();
    return retval;
}

SmtSolver::Satisfiable
SmtSolver::satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs) {
    reset();
    insert(exprs);
    Satisfiable retval = check();
    return retval;
}

void
SmtSolver::push() {
    clearEvidence();
    stack_.push_back(std::vector<SymbolicExpr::Ptr>());
}

void
SmtSolver::pop() {
    clearEvidence();
    if (stack_.size() == 1)
        throw Exception("tried to pop the initial level; use reset instead");
    ASSERT_require(stack_.size() > 1);                  // you should have clalled reset instead
    stack_.pop_back();
}

size_t
SmtSolver::nAssertions(size_t level) {
    ASSERT_require(level < stack_.size());
    return stack_[level].size();
}

size_t
SmtSolver::nAssertions() const {
    size_t retval = 0;
    BOOST_FOREACH (const std::vector<SymbolicExpr::Ptr> &level, stack_)
        retval += level.size();
    return retval;
}

std::vector<SymbolicExpr::Ptr>
SmtSolver::assertions() const {
    std::vector<SymbolicExpr::Ptr> retval;
    BOOST_FOREACH (const std::vector<SymbolicExpr::Ptr> &level, stack_)
        retval.insert(retval.end(), level.begin(), level.end());
    return retval;
}

std::vector<SymbolicExpr::Ptr>
SmtSolver::assertions(size_t level) const {
    ASSERT_require(level < stack_.size());
    return stack_[level];
}


void
SmtSolver::insert(const SymbolicExpr::Ptr &expr) {
    clearEvidence();
    ASSERT_not_null(expr);
    ASSERT_forbid(stack_.empty());
    stack_.back().push_back(expr);
}

void
SmtSolver::insert(const std::vector<SymbolicExpr::Ptr> &exprs) {
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        insert(expr);
}

SmtSolver::Satisfiable
SmtSolver::checkTrivial() {
    // Empty set of assertions is YES
    std::vector<SymbolicExpr::Ptr> exprs = assertions();
    if (exprs.empty())
        return SAT_YES;

    // If any assertion is a constant zero, then NO
    // If all assertions are non-zero integer constants, then YES
    bool allTrue = true;
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs) {
        if (expr->isIntegerConstant()) {
            ASSERT_require(1 == expr->nBits());
            if (expr->toUnsigned().get() == 0)
                return SAT_NO;
        } else {
            allTrue = false;
        }
    }
    if (allTrue)
        return SAT_YES;

    return SAT_UNKNOWN;
}

std::vector<SymbolicExpr::Ptr>
SmtSolver::normalizeVariables(const std::vector<SymbolicExpr::Ptr> &exprs, SymbolicExpr::ExprExprHashMap &index /*out*/) {
    index.clear();
    size_t varCounter = 0;
    std::vector<SymbolicExpr::Ptr> retval;
    retval.reserve(exprs.size());
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        retval.push_back(expr->renameVariables(index /*in,out*/, varCounter /*in,out*/));
    return retval;
}

std::vector<SymbolicExpr::Ptr>
SmtSolver::undoNormalization(const std::vector<SymbolicExpr::Ptr> &exprs, const SymbolicExpr::ExprExprHashMap &norm) {
    SymbolicExpr::ExprExprHashMap denorm = norm.invert();
    std::vector<SymbolicExpr::Ptr> retval;
    retval.reserve(exprs.size());
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        retval.push_back(expr->substituteMultiple(denorm));
    return retval;
}

SmtSolver::Satisfiable
SmtSolver::check() {
    ++stats.ncalls;

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"assertions:\n";
        BOOST_FOREACH (const SymbolicExpr::Ptr &expr, assertions())
            mlog[DEBUG] <<"  " <<*expr <<"\n";
    }
    
    latestMemoizationId_ = 0;
    latestMemoizationRewrite_.clear();
    clearEvidence();
    bool wasTrivial = false;
    Satisfiable retval = checkTrivial();
    if (retval != SAT_UNKNOWN) {
        mlog[DEBUG] <<"trivial solution\n";
        wasTrivial = true;
    }
    
    // Have we seen this before?
    bool wasMemoized = false;
    SymbolicExpr::Hash h = 0;
    if (!wasTrivial && doMemoization_) {
        // Normalize the expressions by renumbering all variables. The renumbering is saved in the latestMemoizationRewrites_
        // data member so the mapping can be reversed when parsing evidence.
        std::vector<SymbolicExpr::Ptr> rewritten = normalizeVariables(assertions(), latestMemoizationRewrite_/*out*/);
        h = SymbolicExpr::hash(rewritten);
        Memoization::iterator found = memoization_.find(h);
        if (found != memoization_.end()) {
            retval = found->second;
            latestMemoizationId_ = h;
            ++stats.memoizationHits;
            mlog[DEBUG] <<"using memoized result\n";
            wasMemoized = true;
        }
    }
    
    // Do the real work
    if (!wasTrivial && !wasMemoized) {
        switch (linkage_) {
            case LM_EXECUTABLE:
                retval = checkExe();
                break;
            case LM_LIBRARY:
                retval = checkLib();
                break;
            case LM_NONE:
                throw Exception("no linkage for " + name_ + " solver");
            default:
                ASSERT_not_reachable("invalid solver linkage: " + boost::lexical_cast<std::string>(linkage_));
        }
    }
    
    if (mlog[DEBUG]) {
        switch (retval) {
            case SAT_NO:
                mlog[DEBUG] <<"  unsat\n";
                break;
            case SAT_YES:
                mlog[DEBUG] <<"  sat\n";
                break;
            case SAT_UNKNOWN:
                mlog[DEBUG] <<"  unknown\n";
                break;
        }
    }
    
    // Cache the result
    if (doMemoization_ && !wasTrivial && !wasMemoized) {
        memoization_[h] = retval;
        latestMemoizationId_ = h;
    }

    if (SAT_YES == retval && !wasTrivial)
        parseEvidence();
    return retval;
}

SmtSolver::Satisfiable
SmtSolver::checkLib() {
    requireLinkage(LM_LIBRARY);
    ASSERT_not_reachable("subclass does not implement checkLib");
}

SmtSolver::Satisfiable
SmtSolver::checkExe() {
    requireLinkage(LM_EXECUTABLE);

    struct Resources {
        FILE *output;
        char *line;
        Resources()
            :output(NULL), line(NULL) {}
        ~Resources() {
            if (output != NULL)
                pclose(output);
            if (line)
                free(line);
        }
    } r;

#ifdef _MSC_VER
    abort();
    return SAT_UNKNOWN;
#else

    outputText_ = "";

    /* Generate the input file for the solver. */
    Sawyer::Stopwatch prepareTimer;
    std::vector<SymbolicExpr::Ptr> exprs = assertions();
    Sawyer::FileSystem::TemporaryFile tmpfile;
    Definitions defns;
    generateFile(tmpfile.stream(), exprs, &defns);
    tmpfile.stream().close();
    struct stat sb;
    int status __attribute__((unused)) = stat(tmpfile.name().string().c_str(), &sb);
    ASSERT_require(status>=0);
    stats.input_size += sb.st_size;
    stats.prepareTime += prepareTimer.stop();

    /* Show solver input */
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"solver input in " <<tmpfile.name() <<":\n";
        unsigned n = 0;
        std::ifstream f(tmpfile.name().string().c_str());
        while (!f.eof()) {
            std::string line;
            std::getline(f, line);
            mlog[DEBUG] <<(boost::format("%5u") % ++n).str() <<": " <<line <<"\n";
        }
    }

    // Run the solver and slurp up all its standard output
    Sawyer::Stopwatch solveTimer;
    std::string cmd = getCommand(tmpfile.name().string());
    SAWYER_MESG(mlog[DEBUG]) <<"command: \"" <<StringUtility::cEscape(cmd) <<"\"\n";
    r.output = popen(cmd.c_str(), "r");
    if (!r.output)
        throw Exception("failed to run \"" + StringUtility::cEscape(cmd) + "\"");
    size_t lineAlloc = 0, lineNum = 0;
    ssize_t nread;
    mlog[DEBUG] <<"solver standard output:\n";
    while ((nread = rose_getline(&r.line, &lineAlloc, r.output)) >0 ) {
        SAWYER_MESG(mlog[DEBUG]) <<(boost::format("%5u") % ++lineNum).str() <<": " <<r.line <<"\n";
        outputText_ += std::string(r.line);
    }
    status = pclose(r.output); r.output = NULL;
    stats.output_size += nread;
    stats.solveTime += solveTimer.stop();
    mlog[DEBUG] <<"solver took " <<solveTimer <<" seconds\n";
    mlog[DEBUG] <<"solver exit status = " <<status <<"\n";
    parsedOutput_ = parseSExpressions(outputText_);

    std::string errorMesg = getErrorMessage(status);
    if (!errorMesg.empty())
        throw Exception("solver command (\"" + StringUtility::cEscape(cmd) + "\") failed: \"" +
                        StringUtility::cEscape(errorMesg) + "\"");

    // Look for an expression that's just "sat" or "unsat"
    Satisfiable sat = SAT_UNKNOWN;
    BOOST_FOREACH (const SExpr::Ptr &expr, parsedOutput_) {
        if (expr->name() == "sat") {
            sat = SAT_YES;
        } else if (expr->name() == "unsat") {
            sat = SAT_NO;
        } else if (mlog[DEBUG]) {
            mlog[DEBUG] <<"solver output sexpr: " <<*expr <<"\n";
        }
    }

    return sat;
#endif
}

SymbolicExpr::Ptr
SmtSolver::evidenceForAddress(uint64_t addr)
{
    return evidenceForName(StringUtility::addrToString(addr));
}

std::string
SmtSolver::getErrorMessage(int exitStatus) {
    return 0 == exitStatus ? "" : "solver command had non-zero exit status";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions for parsing S-Expressions from SMT lib text output.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
SmtSolver::SExpr::Ptr
SmtSolver::SExpr::instance(const Ptr &a, const Ptr &b, const Ptr &c, const Ptr &d) {
    SExpr *retval = new SExpr("");
    if (a) {
        retval->children_.push_back(a);
        if (b) {
            retval->children_.push_back(b);
            if (c) {
                retval->children_.push_back(c);
                if (d)
                    retval->children_.push_back(d);
            } else {
                ASSERT_require(NULL == d);
            }
        } else {
            ASSERT_require(NULL == c);
            ASSERT_require(NULL == d);
        }
    } else {
        ASSERT_require(NULL == b);
        ASSERT_require(NULL == c);
        ASSERT_require(NULL == d);
    }
    return Ptr(retval);
}

// class method
SmtSolver::SExpr::Ptr
SmtSolver::SExpr::instance(const std::string &content) {
    return Ptr(new SExpr(content));
}

// class method
SmtSolver::SExpr::Ptr
SmtSolver::SExpr::instance(size_t n) {
    return instance(boost::lexical_cast<std::string>(n));
}

void
SmtSolver::SExpr::append(const std::vector<Ptr> &exprs) {
    ASSERT_require(content_.empty());
    children_.insert(children_.end(), exprs.begin(), exprs.end());
}

void
SmtSolver::SExpr::print(std::ostream &o) const {
    if (!name().empty()) {
        ASSERT_require(children().size() == 0);
        o <<name();
    } else {
        o <<"(";
        for (size_t i = 0; i < children().size(); ++i) {
            if (i > 0)
                o <<" ";
            const SExpr::Ptr &child = children()[i];
            ASSERT_not_null(child);
            child->print(o);
        }
        o <<")";
    }
}

// The input stream of characters reinterpretted as a stream of tokens.  No look-ahead is necessary during parser, which makes
// this class even simpler than usual. And since S-Exprs are so simple, we don't even bother with a formal token type, instead
// returning std::string with the following values: "" means EOF; "(" and ")"; or anything else is a symbol.
class TokenStream {
private:
    Sawyer::Container::LineVector &input_;
    std::string current_;
    size_t nextTokenOffset_;

public:
    explicit TokenStream(Sawyer::Container::LineVector &input)
        : input_(input), nextTokenOffset_(0) {}

    std::string current() {
        if (current_.empty())
            boost::tie(current_, nextTokenOffset_) = scanTokenAt(nextTokenOffset_);
        return current_;
    }

    void shift() {
        current_ = "";
    }

    std::pair<size_t /*line*/, size_t /*col*/> location() {
        return input_.location(nextTokenOffset_);
    }

private:
    // Get token starting at specified position, after skipping white space, comments, etc. Returns token lexeme and offset to
    // first character after end of the token.
    std::pair<std::string, size_t> scanTokenAt(size_t offset) {
        while (true) {
            // Skip white space
            int ch = input_.character(offset);
            while (isspace(ch))
                ch = input_.character(++offset);
            if (EOF == ch)
                return std::make_pair(std::string(), offset);

            if ('(' == ch) {
                return std::make_pair(std::string("("), offset + 1);
            } else if (')' == ch) {
                return std::make_pair(std::string(")"), offset + 1);
            } else if (';' == ch) {
                // comment extending to end of line
                while (EOF != (ch = input_.character(++offset)) && '\n' != ch) /*void*/;
                ++offset;
            } else if ('"' == ch) {
                // string literal
                size_t end = offset + 1;
                while (EOF != (ch = input_.character(end)) && '"' != ch)
                    ++end; // FIXME[Robb Matzke 2017-10-31]: what escaping is legal?
                ASSERT_require('"'==ch);                // closing quote
                std::string s = std::string(input_.characters(offset)+1, (end-offset)-1); // w/out enclosing quotes
                ++end;                                  // advance past closing quote
                return std::make_pair(s, end);
            } else {
                // everthing else is assumed to be a symbol continuing until the next whitespace, paren, or comment
                size_t end = offset + 1;
                while (EOF != (ch = input_.character(end)) && !isspace(ch) && !strchr("();", ch))
                    ++end;
                const char *lexemeChars = input_.characters(offset);
                ASSERT_require(lexemeChars != NULL);
                return std::make_pair(std::string(lexemeChars, end - offset), end);
            }
        }
    }
};

static SmtSolver::SExpr::Ptr
parseSExpression(TokenStream &tokens) {
    ASSERT_require(tokens.current() != "");
    if (tokens.current() == "(") {
        tokens.shift();
        SmtSolver::SExpr::Ptr sexpr = SmtSolver::SExpr::instance();
        while (tokens.current() != ")") {
            if (tokens.current() == "")
                throw SmtSolver::ParseError(tokens.location(), "EOF before end of S-Expr");
            sexpr->children().push_back(parseSExpression(tokens));
        }
        ASSERT_require(tokens.current() == ")");
        tokens.shift();
        return sexpr;
    } else if (tokens.current() == ")") {
        throw SmtSolver::ParseError(tokens.location(), "an S-Expr cannot begin with ')'");
    } else {
        SmtSolver::SExpr::Ptr sexpr = SmtSolver::SExpr::instance(tokens.current());
        tokens.shift();
        return sexpr;
    }
}

std::vector<SmtSolver::SExpr::Ptr>
SmtSolver::parseSExpressions(const std::string &s) {
    std::vector<SExpr::Ptr> retval;
    Sawyer::Container::LineVector input(s.size(), s.c_str());
    TokenStream tokens(input);

    while (tokens.current() != "")
        retval.push_back(parseSExpression(tokens));
    return retval;
}

void
SmtSolver::printSExpression(std::ostream &o, const SExpr::Ptr &sexpr) {
    if (sexpr == NULL) {
        o <<"nil";
    } else {
        o <<*sexpr;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Unit tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SmtSolver::selfTest() {
    mlog[WHERE] <<"running self-tests\n";
    using namespace SymbolicExpr;
    typedef SymbolicExpr::Ptr E;

    // Make sure we can parse the S-Expr output from solvers
    std::vector<SExpr::Ptr> sexprs = parseSExpressions("x () (x) (x y) ((x) (y))");
    ASSERT_require(sexprs.size() == 5);

    ASSERT_require(sexprs[0]->children().size() == 0);
    ASSERT_require(sexprs[0]->name() == "x");

    ASSERT_require(sexprs[1]->children().size() == 0);
    ASSERT_require(sexprs[1]->name() == "");

    ASSERT_require(sexprs[2]->children().size() == 1);
    ASSERT_require(sexprs[2]->children()[0]->name() == "x");

    ASSERT_require(sexprs[3]->children().size() == 2);
    ASSERT_require(sexprs[3]->children()[0]->name() == "x");
    ASSERT_require(sexprs[3]->children()[1]->name() == "y");

    ASSERT_require(sexprs[4]->children().size() == 2);
    ASSERT_require(sexprs[4]->children()[0]->name() == "");
    ASSERT_require(sexprs[4]->children()[0]->children().size() == 1);
    ASSERT_require(sexprs[4]->children()[0]->children()[0]->name() == "x");
    ASSERT_require(sexprs[4]->children()[1]->name() == "");
    ASSERT_require(sexprs[4]->children()[1]->children().size() == 1);
    ASSERT_require(sexprs[4]->children()[1]->children()[0]->name() == "y");

    // Create some variables and constants
    E a1 = makeIntegerVariable(1, "a1");
    E a8 = makeIntegerVariable(8, "a8");
    E a32 = makeIntegerVariable(32, "a32");
    E a256 = makeIntegerVariable(256, "a256");

    E z8 = makeIntegerConstant(8, 0);
    E b1 = makeIntegerVariable(1, "b1");
    E b4 = makeIntegerConstant(4, 10);
    E b8 = makeIntegerConstant(8, 0xf0);
    E c8 = makeIntegerVariable(8);
    E z256 = makeIntegerConstant(256, 0xdeadbeef);

    E bfalse = makeBooleanConstant(false);
    E btrue = makeBooleanConstant(true);

    // Comparisons
    std::vector<E> exprs;
    exprs.push_back(makeZerop(a8, NO_SOLVER, "zerop"));
    exprs.push_back(makeEq(a8, z8, NO_SOLVER, "equal"));
    exprs.push_back(makeNe(a8, z8, NO_SOLVER, "not equal"));
    exprs.push_back(makeLt(a8, z8, NO_SOLVER, "unsigned less than"));
    exprs.push_back(makeLe(a8, z8, NO_SOLVER, "unsigned less than or equal"));
    exprs.push_back(makeGt(a8, z8, NO_SOLVER, "unsigned greater than"));
    exprs.push_back(makeGe(a8, z8, NO_SOLVER, "unsigned greater than or equal"));
    exprs.push_back(makeSignedLt(a8, z8, NO_SOLVER, "signed less than"));
    exprs.push_back(makeSignedLe(a8, z8, NO_SOLVER, "signed less than or equal"));
    exprs.push_back(makeSignedGt(a8, z8, NO_SOLVER, "signed greater than"));
    exprs.push_back(makeSignedGe(a8, z8, NO_SOLVER, "signed greather than or equal"));

    //  Wider than 64 bits
    exprs.push_back(makeEq(z256, a256, NO_SOLVER, "wide constant"));

    // Boolean operations
    exprs.push_back(makeEq(makeIte(makeZerop(a8), z8, b8), b8, NO_SOLVER, "if-then-else"));
    exprs.push_back(makeAnd(makeZerop(a8), makeZerop(c8), NO_SOLVER, "Boolean conjunction"));
    exprs.push_back(makeOr(makeZerop(a8), makeZerop(c8), NO_SOLVER, "Boolean disjunction"));
    exprs.push_back(makeXor(makeZerop(a8), makeZerop(c8), NO_SOLVER, "Boolean exclusive disjunction"));

    // Bit operations
    exprs.push_back(makeZerop(makeAnd(a8, b8), NO_SOLVER, "bit-wise conjunction"));
    exprs.push_back(makeZerop(makeAsr(makeIntegerConstant(2, 3), a8), NO_SOLVER, "arithmetic shift right 3 bits"));
    exprs.push_back(makeZerop(makeOr(a8, b8), NO_SOLVER, "bit-wise disjunction"));
    exprs.push_back(makeZerop(makeXor(a8, b8), NO_SOLVER, "bit-wise exclusive disjunction"));
    exprs.push_back(makeZerop(makeConcat(a8, b8), NO_SOLVER, "concatenation"));
    exprs.push_back(makeZerop(makeExtract(makeIntegerConstant(2, 3), makeIntegerConstant(4, 8), a8), NO_SOLVER,
                              "extract bits [3..7]"));
    exprs.push_back(makeZerop(makeInvert(a8), NO_SOLVER, "bit-wise not"));
#if 0 // FIXME[Robb Matzke 2017-10-24]: not implemented yet
    exprs.push_back(makeZerop(makeLssb(a8), NO_SOLVER, "least significant set bit"));
    exprs.push_back(makeZerop(makeMssb(a8), NO_SOLVER, "most significant set bit"));
#endif
    exprs.push_back(makeZerop(makeRol(makeIntegerConstant(2, 3), a8), NO_SOLVER, "rotate left three bits"));
    exprs.push_back(makeZerop(makeRor(makeIntegerConstant(2, 3), a8), NO_SOLVER, "rotate right three bits"));
    exprs.push_back(makeZerop(makeSignExtend(makeIntegerConstant(6, 32), a8), NO_SOLVER, "sign extend to 32 bits"));
    exprs.push_back(makeZerop(makeShl0(makeIntegerConstant(2, 3), a8), NO_SOLVER, "shift left inserting three zeros"));
    exprs.push_back(makeZerop(makeShl1(makeIntegerConstant(2, 3), a8), NO_SOLVER, "shift left inserting three ones"));
    exprs.push_back(makeZerop(makeShr0(makeIntegerConstant(2, 3), a8), NO_SOLVER, "shift right inserting three zeros"));
    exprs.push_back(makeZerop(makeShr1(makeIntegerConstant(2, 3), a8), NO_SOLVER, "shift right inserting three ones"));
    exprs.push_back(makeZerop(makeExtend(makeIntegerConstant(2, 3), a8), NO_SOLVER, "truncate to three bits"));
    exprs.push_back(makeZerop(makeExtend(makeIntegerConstant(6, 32), a8), NO_SOLVER, "extend to 32 bits"));

    // Arithmetic operations
    exprs.push_back(makeZerop(makeAdd(a8, b8), NO_SOLVER, "addition"));
    exprs.push_back(makeZerop(makeNegate(a8), NO_SOLVER, "negation"));
#if 0 // FIXME[Robb Matzke 2017-10-24]: not implemented yet
    exprs.push_back(makeZerop(makeSignedDiv(a8, b4), NO_SOLVER, "signed ratio"));
    exprs.push_back(makeZerop(makeSignedMod(a8, b4), NO_SOLVER, "signed remainder"));
#endif
    exprs.push_back(makeZerop(makeSignedMul(a8, b4), NO_SOLVER, "signed multiply"));
    exprs.push_back(makeZerop(makeDiv(a8, b4), NO_SOLVER, "unsigned ratio"));
    exprs.push_back(makeZerop(makeMod(a8, b4), NO_SOLVER, "unsigned remainder"));
    exprs.push_back(makeZerop(makeMul(a8, b4), NO_SOLVER, "unsigned multiply"));

    // Memory operations
    E mem = makeMemoryVariable(32, 8, "memory");
    E addr = makeIntegerConstant(32, 12345, "address");
    exprs.push_back(makeZerop(makeRead(mem, addr), NO_SOLVER, "read from memory"));
    exprs.push_back(makeEq(makeRead(makeWrite(mem, addr, a8), addr), a8, NO_SOLVER, "write to memory"));

    // Miscellaneous operations
    exprs.push_back(makeEq(makeSet(a8, b8, c8), b8, NO_SOLVER, "set"));

    // Mixing 1-bit values used as bit vectors and Booleans should be allowed.
    exprs.push_back(makeAnd(makeAdd(a1, b1) /*bit-vector*/, makeZerop(b1) /*Boolean*/));

    // Some operations should work on bit vectors (tested above) or Booleans.  In ROSE, a Boolean is just a 1-bit vector, but
    // SMT solvers usually distinguish between 1-bit vector type and Boolean type and don't allow them to be mixed.
    exprs.push_back(makeEq(makeZerop(a1), b1));
    exprs.push_back(makeXor(makeZerop(a1), b1));
    exprs.push_back(makeNe(a1, makeZerop(b1)));
    exprs.push_back(makeIte(a1, makeZerop(a1), b1));

    // Wide multiply
    exprs.push_back(makeEq(makeExtract(makeIntegerConstant(8, 0), makeIntegerConstant(8, 128),
                                       makeMul(makeIntegerVariable(128), makeIntegerConstant(128, 2))),
                           makeIntegerConstant(128, 16)));

    // Run the solver
    for (size_t i=0; i<exprs.size(); ++i) {
        const E &expr = exprs[i];
        mlog[TRACE] <<"expr [" <<i <<"] = " <<*expr <<"\n";
        try {
            switch (satisfiable(expr)) {
                case SAT_NO:
                    mlog[TRACE] <<"not satisfiable\n";
                    break;
                case SAT_YES:
                    mlog[TRACE] <<"satisfiable\n";

                    BOOST_FOREACH (const std::string &evidenceName, evidenceNames()) {
                        E evidence = evidenceForName(evidenceName);
                        ASSERT_always_not_null(evidence);
                        mlog[DEBUG] <<"  evidence: " <<evidenceName <<" = " <<*evidence <<"\n";
                    }
                    break;
                case SAT_UNKNOWN:
                    mlog[TRACE] <<"unknown\n";
                    break;
            }
        } catch (const Exception &e) {
            if (boost::contains(e.what(), "not implemented") ||
                boost::contains(e.what(), "z3 interface does not support")) {
                mlog[WARN] <<e.what() <<"\n";
            } else {
                throw;                                  // an error we don't expect
            }
        }
    }

    // Test that memoization works, including the ability to obtain the evidence afterward.
    if (memoization()) {
        E var1 = makeIntegerVariable(8);
        E var2 = makeIntegerVariable(8);
        ASSERT_always_forbid(var1->isEquivalentTo(var2));
        E expr1 = makeEq(var1, makeIntegerConstant(8, 123));
        E expr2 = makeEq(var2, makeIntegerConstant(8, 123));
        ASSERT_always_forbid(expr1->isEquivalentTo(expr2));

        Satisfiable sat = satisfiable(expr1);
        ASSERT_always_require(SAT_YES == sat);
        std::vector<std::string> evid1names = evidenceNames();
        ASSERT_always_require(evid1names.size() == 1);
        ASSERT_always_require(evid1names[0] == var1->isLeafNode()->toString());
        E evid1 = evidenceForName(evid1names[0]);
        ASSERT_always_not_null(evid1);
        ASSERT_always_require(evid1->isLeafNode());
        ASSERT_always_require(evid1->isLeafNode()->isIntegerConstant());
        ASSERT_always_require(evid1->isLeafNode()->toUnsigned().get() == 123);

        sat = satisfiable(expr2);
        ASSERT_always_require(SAT_YES == sat);
        std::vector<std::string> evid2names = evidenceNames();
        ASSERT_always_require(evid2names.size() == 1);
        ASSERT_always_require(evid2names[0] == var2->isLeafNode()->toString());
        E evid2 = evidenceForName(evid2names[0]);
        ASSERT_always_not_null(evid2);
        ASSERT_always_require(evid2->isLeafNode());
        ASSERT_always_require(evid2->isLeafNode()->isIntegerConstant());
        ASSERT_always_require(evid2->isLeafNode()->toUnsigned().get() == 123);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Deprecated functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// FIXME[Robb Matzke 2017-10-17]: deprecated
SymbolicExpr::Ptr
SmtSolver::evidence_for_variable(uint64_t varno) {
    return evidenceForVariable(varno);
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SymbolicExpr::Ptr
SmtSolver::evidence_for_variable(const SymbolicExpr::Ptr &var) {
    return evidenceForVariable(var);
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SymbolicExpr::Ptr
SmtSolver::evidence_for_name(const std::string &s) {
    return evidenceForName(s);
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
std::vector<std::string>
SmtSolver::evidence_names() {
    return evidenceNames();
}

// FIXME[Robb Matzke 2017-10-20]: deprecated
void
SmtSolver::clear_evidence() {
    return clearEvidence();
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
const SmtSolver::Stats&
SmtSolver::get_stats() const {
    return statistics();
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
void
SmtSolver::reset_stats() {
    resetStatistics();
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
void
SmtSolver::parse_evidence() {
    parseEvidence();
}

// FIXME[Robb Matzke 2017-10-20]: deprecated
void
SmtSolver::generate_file(std::ostream &o, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions *defns) {
    generateFile(o, exprs, defns);
}

// FIXME[Robb Matzke 2017-10-20]: deprecated
std::string
SmtSolver::get_command(const std::string &config_name) {
    return getCommand(config_name);
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SmtSolver::Stats
SmtSolver::get_class_stats() {
    return classStatistics();
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
void
SmtSolver::reset_class_stats()
{
    resetClassStatistics();
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SymbolicExpr::Ptr
SmtSolver::evidence_for_address(uint64_t addr) {
    return evidenceForAddress(addr);
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SmtSolver::Satisfiable
SmtSolver::trivially_satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs) {
    return triviallySatisfiable(exprs);
}

} // namespace
} // namespace

#endif
