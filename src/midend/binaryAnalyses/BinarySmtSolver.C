#include "sage3basic.h"
#include "rosePublicConfig.h"

#include "rose_getline.h"
#include "BinarySmtSolver.h"
#include "BinarySmtlibSolver.h"
#include "BinaryYicesSolver.h"

#include <boost/format.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <fcntl.h> /*for O_RDWR, etc.*/
#include <Sawyer/FileSystem.h>
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
    using namespace SymbolicExpr;
    typedef SymbolicExpr::Ptr E;
    std::vector<E> exprs;

    E a1 = makeVariable(1, "1-bit variable");
    E a8 = makeVariable(8, "eight-bit variable");
    E a32 = makeVariable(32, "32-bit variable");
    E a256 = makeVariable(256, "256-bit variable");

    E z8 = makeInteger(8, 0, "8-bit zero");
    E b4 = makeInteger(4, 10, "4-bit 10");
    E b8 = makeInteger(8, 0xf0, "8-bit 0xf0");
    E c8 = makeVariable(8, "8-bit variable");
    E z256 = makeInteger(256, 0xdeadbeef, "256-bit 0xDeadBeef");

    E bfalse = makeBoolean(false, "Boolean false");
    E btrue = makeBoolean(true, "Boolean true");

    //---- Comparisons ----
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

    //----- Boolean operations -----
    exprs.push_back(makeBooleanAnd(makeZerop(a8), makeZerop(c8), "Boolean conjunction"));
    exprs.push_back(makeEq(makeIte(makeZerop(a8), z8, b8), b8, "if-then-else"));
    exprs.push_back(makeBooleanOr(makeZerop(a8), makeZerop(c8), "Boolean disjunction"));

    //----- Bit operations -----
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
    
    //----- Arithmetic operations -----
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

    //----- Memory operations -----
    E mem = makeMemory(32, 8, "memory");
    E addr = makeInteger(32, 12345, "address");
    exprs.push_back(makeZerop(makeRead(mem, addr), "read from memory"));
    exprs.push_back(makeEq(makeRead(makeWrite(mem, addr, a8), addr), a8, "write to memory"));

    //----- Miscellaneous operations -----
    exprs.push_back(makeEq(makeSet(a8, b8, c8), b8, "set"));

    generateFile(std::cout, exprs, NULL);
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
    bool got_satunsat_line = false;

#ifdef _MSC_VER
    // tps (06/23/2010) : Does not work under Windows
    abort();
    Satisfiable retval;
    return retval;
#else

    requireLinkage(LM_EXECUTABLE);
    clearEvidence();

    Satisfiable retval = triviallySatisfiable(exprs);
    if (retval!=SAT_UNKNOWN)
        return retval;

    // Keep track of how often we call the SMT solver.
    ++stats.ncalls;
    {
        boost::lock_guard<boost::mutex> lock(classStatsMutex);
        ++classStats.ncalls;
    }
    outputText = "";

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
        mlog[DEBUG] <<"SMT solver input in " <<tmpfile.name() <<"\n";
        size_t n=0;
        std::ifstream f(tmpfile.name().string().c_str());
        while (!f.eof()) {
            std::string line;
            std::getline(f, line);
            mlog[DEBUG] <<(boost::format("%5zu") % ++n).str() <<": " <<line <<"\n";
        }
    }

    /* Run the solver and read its output. The first line should be the word "sat" or "unsat" */
    {
        Sawyer::Stopwatch stopwatch;
        std::string cmd = getCommand(tmpfile.name().string());
        FILE *output = popen(cmd.c_str(), "r");
        ASSERT_not_null(output);
        char *line = NULL;
        size_t line_alloc = 0;
        ssize_t nread;
        while ((nread=rose_getline(&line, &line_alloc, output))>0) {
            stats.output_size += nread;
            {
                boost::lock_guard<boost::mutex> lock(classStatsMutex);
                classStats.output_size += nread;
            }
            if (!got_satunsat_line) {
                if (0==strncmp(line, "sat", 3) && isspace(line[3])) {
                    retval = SAT_YES;
                    got_satunsat_line = true;
                } else if (0==strncmp(line, "unsat", 5) && isspace(line[5])) {
                    retval = SAT_NO;
                    got_satunsat_line = true;
                } else {
                    std::cerr <<"SMT solver failed to say \"sat\" or \"unsat\"\n";
                    abort();
                }
            } else {
                outputText += std::string(line);
            }
        }
        if (line) free(line);
        int status = pclose(output);
        stopwatch.stop();
        if (mlog[DEBUG]) {
            mlog[DEBUG] <<"Running SMT solver=\"" <<cmd <<"; exit status="<<status <<"\n";
            mlog[DEBUG] <<"SMT Solver ran for " <<stopwatch <<" seconds\n";
            mlog[DEBUG] <<"SMT Solver reported: " <<(SAT_YES==retval ? "sat" : SAT_NO==retval ? "unsat" : "unknown") <<"\n";
            mlog[DEBUG] <<"SMT Solver output:\n" <<StringUtility::prefixLines(outputText, "     ");
        }
    }

    if (SAT_YES==retval)
        parseEvidence();
#endif
    return retval;
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
