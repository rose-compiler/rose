#include "sage3basic.h"
#include "rosePublicConfig.h"

#include "rose_getline.h"
#include "BinarySmtSolver.h"
#include "BinarySmtlibSolver.h"
#include "BinaryYicesSolver.h"
#include "BinaryZ3Solver.h"

#include <boost/format.hpp>
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
operator<<(std::ostream &o, const SmtSolver::Exception &e)
{
    return o <<"SMT solver: " <<e.what();
}

SmtSolver::Stats SmtSolver::classStats;
boost::mutex SmtSolver::classStatsMutex;

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
SmtSolver::requireLinkage(LinkMode need) {
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

void
SmtSolver::init(unsigned linkages) {
    linkage_ = bestLinkage(linkages);
}

// class method
SmtSolver::Stats
SmtSolver::classStatistics() 
{
    boost::lock_guard<boost::mutex> lock(classStatsMutex);
    return classStats;
}

// class method
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
    exprs.push_back(makeBooleanAnd(makeZerop(a8), makeZerop(c8), "Boolean conjunction"));
    exprs.push_back(makeEq(makeIte(makeZerop(a8), z8, b8), b8, "if-then-else"));
    exprs.push_back(makeBooleanOr(makeZerop(a8), makeZerop(c8), "Boolean disjunction"));

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

    // Run the solver
    BOOST_FOREACH (const E &expr, exprs) {
        mlog[TRACE] <<"test " <<*expr <<"\n";
        switch (satisfiable(expr)) {
            case SAT_NO:
                mlog[TRACE] <<"not satisfiable\n";
                break;
            case SAT_YES:
                mlog[TRACE] <<"satisfiable\n";
                break;
            case SAT_UNKNOWN:
                mlog[TRACE] <<"unknown\n";
                break;
        }
    }
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SmtSolver::Stats
SmtSolver::get_class_stats() {
    return classStatistics();
}

// class method
void
SmtSolver::resetClassStatistics()
{
    boost::lock_guard<boost::mutex> lock(classStatsMutex);
    classStats = Stats();
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
void
SmtSolver::reset_class_stats()
{
    resetClassStatistics();
}

SymbolicExpr::Ptr
SmtSolver::evidenceForAddress(uint64_t addr)
{
    return evidenceForName(StringUtility::addrToString(addr));
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SymbolicExpr::Ptr
SmtSolver::evidence_for_address(uint64_t addr) {
    return evidenceForAddress(addr);
}

SmtSolver::Satisfiable
SmtSolver::triviallySatisfiable(const std::vector<SymbolicExpr::Ptr> &exprs_)
{
    std::vector<SymbolicExpr::Ptr> exprs(exprs_.begin(), exprs_.end());
    for (size_t i=0; i<exprs.size(); ++i) {
        if (exprs[i]->isNumber()) {
            ASSERT_require(1==exprs[i]->nBits());
            if (0==exprs[i]->toInt())
                return SAT_NO;
            std::swap(exprs[i], exprs.back()); // order of exprs is not important
            exprs.resize(exprs.size()-1);
        }
    }
    return exprs.empty() ? SAT_YES : SAT_UNKNOWN;
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
SmtSolver::Satisfiable
SmtSolver::trivially_satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs) {
    return triviallySatisfiable(exprs);
}

SmtSolver::Satisfiable
SmtSolver::satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs)
{
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
    // tps (06/23/2010) : Does not work under Windows
    abort();
    Satisfiable retval;
    return retval;
#else

    requireLinkage(LM_EXECUTABLE);
    termNames_.clear();
    clearEvidence();

    Satisfiable retval = triviallySatisfiable(exprs);
    if (retval != SAT_UNKNOWN)
        return retval;

    // Keep track of how often we call the SMT solver.
    ++stats.ncalls;
    {
        boost::lock_guard<boost::mutex> lock(classStatsMutex);
        ++classStats.ncalls;
    }
    outputText_ = "";

    /* Generate the input file for the solver. */
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
            retval = SAT_YES;
            break;
        } else if (expr->name() == "unsat") {
            mlog[DEBUG] <<"not satisfied\n";
            retval = SAT_NO;
            break;
        }
    }

    if (SAT_YES==retval)
        parseEvidence();
#endif
    return retval;
}

std::string
SmtSolver::getErrorMessage(int exitStatus) {
    return 0 == exitStatus ? "" : "solver command had non-zero exit status";
}
        
SmtSolver::Satisfiable
SmtSolver::satisfiable(const SymbolicExpr::Ptr &tn)
{
    std::vector<SymbolicExpr::Ptr> exprs;
    exprs.push_back(tn);
    return satisfiable(exprs);
}

SmtSolver::Satisfiable
SmtSolver::satisfiable(std::vector<SymbolicExpr::Ptr> exprs, const SymbolicExpr::Ptr &expr)
{
    if (expr!=NULL)
        exprs.push_back(expr);
    return satisfiable(exprs);
}

// class method
SmtSolver::SExpr::Ptr
SmtSolver::SExpr::instance() {
    return Ptr(new SExpr(""));
}

// class method
SmtSolver::SExpr::Ptr
SmtSolver::SExpr::instance(const std::string &content) {
    return Ptr(new SExpr(content));
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
    } else if (!sexpr->name().empty()) {
        ASSERT_require(sexpr->children().size() == 0);
        o <<sexpr->name();
    } else {
        o <<"(";
        for (size_t i = 0; i < sexpr->children().size(); ++i) {
            if (i > 0)
                o <<" ";
            printSExpression(o, sexpr->children()[i]);
        }
        o <<")";
    }
}

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

} // namespace
} // namespace
