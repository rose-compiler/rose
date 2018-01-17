#include "sage3basic.h"
#include "rosePublicConfig.h"

#include "rose_getline.h"
#include "BinarySmtSolver.h"
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

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility SmtSolver::mlog;

// class method
void
SmtSolver::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::SmtSolver");
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
}

void
SmtSolver::reset() {
    stack_.clear();
    push();
    clearEvidence();
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
SmtSolver*
SmtSolver::instance(const std::string &name) {
    if ("" == name || "none" == name)
        return NULL;
    if ("best" == name)
        return bestAvailable();
    if ("z3-lib" == name)
        return new Z3Solver(LM_LIBRARY);
    if ("z3-exe" == name)
        return new Z3Solver(LM_EXECUTABLE);
    if ("yices-lib" == name)
        return new YicesSolver(LM_LIBRARY);
    if ("yices-exe" == name)
        return new YicesSolver(LM_EXECUTABLE);
    throw Exception("unrecognized SMT solver name \"" + StringUtility::cEscape(name) + "\"");
}

// class method
SmtSolver*
SmtSolver::bestAvailable() {
    // Binary APIs are faster, so prefer them
    if ((Z3Solver::availableLinkages() & LM_LIBRARY) != 0)
        return new Z3Solver(LM_LIBRARY);
    if ((YicesSolver::availableLinkages() & LM_LIBRARY) != 0)
        return new YicesSolver(LM_LIBRARY);

    // Next try text-based APIs
    if ((Z3Solver::availableLinkages() & LM_EXECUTABLE) != 0)
        return new Z3Solver(LM_EXECUTABLE);
    if ((YicesSolver::availableLinkages() & LM_EXECUTABLE) != 0)
        return new YicesSolver(LM_EXECUTABLE);

    return NULL;
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
    if (SAT_YES == retval)
        parseEvidence();
    return retval;
}

SmtSolver::Satisfiable
SmtSolver::satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs) {
    reset();
    insert(exprs);
    Satisfiable retval = check();
    if (SAT_YES == retval)
        parseEvidence();
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
    ASSERT_forbid(stack_.empty());
    stack_.pop_back();
    if (stack_.empty())
        push();
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
    std::vector<SymbolicExpr::Ptr> exprs = assertions();
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs) {
        if (expr->isNumber()) {
            ASSERT_require(1 == expr->nBits());
            if (expr->toInt() == 0)
                return SAT_NO;
        }
    }
    return exprs.empty() ? SAT_YES : SAT_UNKNOWN;
}

SmtSolver::Satisfiable
SmtSolver::check() {
    clearEvidence();
    Satisfiable t = checkTrivial();
    if (t != SAT_UNKNOWN)
        return t;

    switch (linkage_) {
        case LM_EXECUTABLE:
            return checkExe();
        case LM_LIBRARY:
            return checkLib();
        case LM_NONE:
            throw Exception("no linkage for " + name_ + " solver");
        default:
            ASSERT_not_reachable("invalid solver linkage: " + boost::lexical_cast<std::string>(linkage_));
    }
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

    // Keep track of how often we call the SMT solver.
    ++stats.ncalls;
    {
        boost::lock_guard<boost::mutex> lock(classStatsMutex);
        ++classStats.ncalls;
    }
    outputText_ = "";

    /* Generate the input file for the solver. */
    std::vector<SymbolicExpr::Ptr> exprs = assertions();
    Sawyer::FileSystem::TemporaryFile tmpfile;
    Definitions defns;
    generateFile(tmpfile.stream(), exprs, &defns);
    tmpfile.stream().close();
    struct stat sb;
    int status __attribute__((unused)) = stat(tmpfile.name().string().c_str(), &sb);
    ASSERT_require(status>=0);
    stats.input_size += sb.st_size;
    {
        boost::lock_guard<boost::mutex> lock(classStatsMutex);
        classStats.input_size += sb.st_size;
    }

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
    Sawyer::Stopwatch timer;
    std::string cmd = getCommand(tmpfile.name().string());
    SAWYER_MESG(mlog[DEBUG]) <<"command: \"" <<StringUtility::cEscape(cmd) <<"\"\n";
    r.output = popen(cmd.c_str(), "r");
    if (!r.output)
        throw Exception("failed to run \"" + StringUtility::cEscape(cmd) + "\"");
    size_t lineAlloc = 0, lineNum = 0;
    ssize_t nread;
    mlog[DEBUG] <<"solver standard output:\n";
    while ((nread = rose_getline(&r.line, &lineAlloc, r.output)) >0 ) {
        mlog[DEBUG] <<(boost::format("%5u") % ++lineNum).str() <<": " <<r.line <<"\n";
        stats.output_size += nread;
        {
            boost::lock_guard<boost::mutex> lock(classStatsMutex);
            classStats.output_size += nread;
        }
        outputText_ += std::string(r.line);
    }
    status = pclose(r.output); r.output = NULL;
    mlog[DEBUG] <<"solver took " <<timer <<" seconds\n";
    mlog[DEBUG] <<"solver exit status = " <<status <<"\n";
    parsedOutput_ = parseSExpressions(outputText_);

    std::string errorMesg = getErrorMessage(status);
    if (!errorMesg.empty())
        throw Exception("solver command (\"" + StringUtility::cEscape(cmd) + "\") failed: \"" +
                        StringUtility::cEscape(errorMesg) + "\"");

    // Look for an expression that's just "sat" or "unsat"
    BOOST_FOREACH (const SExpr::Ptr &expr, parsedOutput_) {
        if (expr->name() == "sat") {
            mlog[DEBUG] <<"satisfied\n";
            return SAT_YES;
        } else if (expr->name() == "unsat") {
            mlog[DEBUG] <<"not satisfied\n";
            return SAT_NO;
        }
    }

    return SAT_UNKNOWN;
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
                return std::make_pair(std::string(input_.characters(offset), end - offset), end);
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
    E a1 = makeVariable(1, "a1");
    E a8 = makeVariable(8, "a8");
    E a32 = makeVariable(32, "a32");
    E a256 = makeVariable(256, "a256");

    E z8 = makeInteger(8, 0);
    E b1 = makeVariable(1, "b1");
    E b4 = makeInteger(4, 10);
    E b8 = makeInteger(8, 0xf0);
    E c8 = makeVariable(8);
    E z256 = makeInteger(256, 0xdeadbeef);

    E bfalse = makeBoolean(false);
    E btrue = makeBoolean(true);

    // Comparisons
    std::vector<E> exprs;
    exprs.push_back(makeZerop(a8, "zerop"));
    exprs.push_back(makeEq(a8, z8, "equal"));
    exprs.push_back(makeNe(a8, z8, "not equal"));
    exprs.push_back(makeLt(a8, z8, "unsigned less than"));
    exprs.push_back(makeLe(a8, z8, "unsigned less than or equal"));
    exprs.push_back(makeGt(a8, z8, "unsigned greater than"));
    exprs.push_back(makeGe(a8, z8, "unsigned greater than or equal"));
    exprs.push_back(makeSignedLt(a8, z8, "signed less than"));
    exprs.push_back(makeSignedLe(a8, z8, "signed less than or equal"));
    exprs.push_back(makeSignedGt(a8, z8, "signed greater than"));
    exprs.push_back(makeSignedGe(a8, z8, "signed greather than or equal"));

    // Boolean operations
    exprs.push_back(makeEq(makeIte(makeZerop(a8), z8, b8), b8, "if-then-else"));
    exprs.push_back(makeAnd(makeZerop(a8), makeZerop(c8), "Boolean conjunction"));
    exprs.push_back(makeOr(makeZerop(a8), makeZerop(c8), "Boolean disjunction"));
    exprs.push_back(makeXor(makeZerop(a8), makeZerop(c8), "Boolean exclusive disjunction"));

    // Bit operations
    exprs.push_back(makeZerop(makeAnd(a8, b8), "bit-wise conjunction"));
    exprs.push_back(makeZerop(makeAsr(makeInteger(2, 3), a8), "arithmetic shift right 3 bits"));
    exprs.push_back(makeZerop(makeOr(a8, b8), "bit-wise disjunction"));
    exprs.push_back(makeZerop(makeXor(a8, b8), "bit-wise exclusive disjunction"));
    exprs.push_back(makeZerop(makeConcat(a8, b8), "concatenation"));
    exprs.push_back(makeZerop(makeExtract(makeInteger(2, 3), makeInteger(4, 8), a8), "extract bits [3..7]"));
    exprs.push_back(makeZerop(makeInvert(a8), "bit-wise not"));
#if 0 // FIXME[Robb Matzke 2017-10-24]: not implemented yet
    exprs.push_back(makeZerop(makeLssb(a8), "least significant set bit"));
    exprs.push_back(makeZerop(makeMssb(a8), "most significant set bit"));
#endif
    exprs.push_back(makeZerop(makeRol(makeInteger(2, 3), a8), "rotate left three bits"));
    exprs.push_back(makeZerop(makeRor(makeInteger(2, 3), a8), "rotate right three bits"));
    exprs.push_back(makeZerop(makeSignExtend(makeInteger(6, 32), a8), "sign extend to 32 bits"));
    exprs.push_back(makeZerop(makeShl0(makeInteger(2, 3), a8), "shift left inserting three zeros"));
    exprs.push_back(makeZerop(makeShl1(makeInteger(2, 3), a8), "shift left inserting three ones"));
    exprs.push_back(makeZerop(makeShr0(makeInteger(2, 3), a8), "shift right inserting three zeros"));
    exprs.push_back(makeZerop(makeShr1(makeInteger(2, 3), a8), "shift right inserting three ones"));
    exprs.push_back(makeZerop(makeExtend(makeInteger(2, 3), a8), "truncate to three bits"));
    exprs.push_back(makeZerop(makeExtend(makeInteger(6, 32), a8), "extend to 32 bits"));

    // Arithmetic operations
    exprs.push_back(makeZerop(makeAdd(a8, b8), "addition"));
    exprs.push_back(makeZerop(makeNegate(a8), "negation"));
#if 0 // FIXME[Robb Matzke 2017-10-24]: not implemented yet
    exprs.push_back(makeZerop(makeSignedDiv(a8, b4), "signed ratio"));
    exprs.push_back(makeZerop(makeSignedMod(a8, b4), "signed remainder"));
#endif
    exprs.push_back(makeZerop(makeSignedMul(a8, b4), "signed multiply"));
    exprs.push_back(makeZerop(makeDiv(a8, b4), "unsigned ratio"));
    exprs.push_back(makeZerop(makeMod(a8, b4), "unsigned remainder"));
    exprs.push_back(makeZerop(makeMul(a8, b4), "unsigned multiply"));

    // Memory operations
    E mem = makeMemory(32, 8, "memory");
    E addr = makeInteger(32, 12345, "address");
    exprs.push_back(makeZerop(makeRead(mem, addr), "read from memory"));
    exprs.push_back(makeEq(makeRead(makeWrite(mem, addr, a8), addr), a8, "write to memory"));

    // Miscellaneous operations
    exprs.push_back(makeEq(makeSet(a8, b8, c8), b8, "set"));

    // Mixing 1-bit values used as bit vectors and Booleans should be allowed.
    exprs.push_back(makeAnd(makeAdd(a1, b1) /*bit-vector*/, makeZerop(b1) /*Boolean*/));

    // Some operations should work on bit vectors (tested above) or Booleans.  In ROSE, a Boolean is just a 1-bit vector, but
    // SMT solvers usually distinguish between 1-bit vector type and Boolean type and don't allow them to be mixed.
    exprs.push_back(makeEq(makeZerop(a1), b1));
    exprs.push_back(makeXor(makeZerop(a1), b1));
    exprs.push_back(makeNe(a1, makeZerop(b1)));
    exprs.push_back(makeIte(a1, makeZerop(a1), b1));

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
            if (boost::contains(e.what(), "not implemented")) {
                mlog[WARN] <<e.what() <<"\n";
            } else {
                throw;                                  // an error we don't expect
            }
        }
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
