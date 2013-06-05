#include "rose.h"

#include "BinaryLoader.h"
#include "BinaryPointerDetection.h"
#include "YicesSolver.h"
#include "DwarfLineMapper.h"
#include "CloneDetectionProgress.h"
#include "CloneDetectionAnalysisFault.h"
#include "CloneDetectionOutputs.h"
#include "PartialSymbolicSemantics.h"
#include "x86InstructionSemantics.h"
using namespace BinaryAnalysis::InstructionSemantics;

// We'd like to not have to depend on a particular relational database management system, but ROSE doesn't have
// support for anything like ODBC.  The only other options are either to generate SQL output, which precludes being
// able to perform queries, or using sqlite3x.h, which is distributed as part of ROSE.  If you want to use a RDMS other
// than SQLite3, then dump SQL from the generated SQLite3 database and load it into whatever RDMS you want.
#include "sqlite3x.h"
using namespace sqlite3x; // its top-level class name start with "sqlite3_"

#ifdef ROSE_HAVE_GCRYPT_H
#include <gcrypt.h>
#endif

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>
#include <boost/algorithm/string/regex.hpp>

static std::string argv0; // base name of argv[0]

static void usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [FRONTEND_SWITCHES] SPECIMEN\n"
              <<"   --database=STR\n"
              <<"                     Specifies the name of the database into which results are placed.\n"
              <<"                     The default is \"clones.db\".\n"
              <<"   --follow-calls\n"
              <<"                     Normally, x86 CALL instructions are not followed, but rather consume\n"
              <<"                     the next input value as a return value.  This switch disables\n"
              <<"                     that special handling so that the CALL executes normally.  However,\n"
              <<"                     if the called address is invalid then the special handling still applies.\n"
              <<"   --function=ADDR\n"
              <<"                     Analyze only the specified function (provided it also satisfies\n"
              <<"                     all other selection criteria.  Normally all functions are considered.\n"
              <<"                     This switch may appear more than once to select multiple functions.\n"
              <<"   --link\n"
              <<"                     Perform dynamic linking and analyze the libraries along with the\n"
              <<"                     specified executable. The default is to not link.\n"
              <<"   --min-function-size=N\n"
              <<"                     Minimum size of functions to analyze. Any function containing fewer\n"
              <<"                     than N instructions is not processed. The default is to analyze all\n"
              <<"                     functions.\n"
              <<"   --max-insns=N\n"
              <<"                     Maximum number of instructions to simulate per function. The default\n"
              <<"                     is 5000.\n"
              <<"   --nfuzz=N[,START]\n"
              <<"                     Number of times to fuzz test each function (default 10) and the\n"
              <<"                     sequence number of the first test (default 0).\n"
              <<"   --ninputs=N1[,N2]\n"
              <<"                     Number of input values to supply each time a function is run. When\n"
              <<"                     N1 and N2 are both specified, then N1 is the number of pointers and\n"
              <<"                     N2 is the number of non-pointers.  When only N1 is specified it will\n"
              <<"                     indicate the number of pointers and the number of non-pointers. The\n"
              <<"                     default is 20 pointers and 100 non-pointers.  If a function\n"
              <<"                     requires more input values, then null/zero are supplied to it.\n"
              <<"   --permute-inputs=N\n"
              <<"                     Normally, each input group contains random non-pointer values.  If\n"
              <<"                     this switch is given with N greater than 1 then the first N non-pointer\n"
              <<"                     values of an input group is a permutation of the first N non-pointer\n"
              <<"                     values of an earlier input group.  For instance, if N is three then\n"
              <<"                     input groups will be generated in groups of six (6=3!).  The N\n"
              <<"                     specified here may be larger than the N1 value for the --ninputs\n"
              <<"                     switch, in which case some of the values are zero.\n"
              <<"   --[no-]pointers\n"
              <<"                     Turn pointer analysis on or off (the default is off).  When pointer\n"
              <<"                     analysis is turned on, each function is analyzed to find memory addresses\n"
              <<"                     that are used as code or data pointers.  This information is used when\n"
              <<"                     deciding whether to consume a pointer or non-pointer input.  When the\n"
              <<"                     analysis is turned off, only non-pointer inputs are used.\n"
              <<"   --progress\n"
              <<"                     Force a progress bar to be displayed even if the standard error stream\n"
              <<"                     is not a terminal and even if the verbosity is more than silent.\n"
              <<"   --verbose\n"
              <<"   --verbosity=(silent|laconic|effusive)\n"
              <<"                     How much diagnostics to show.  The default is silent.  The \"--verbose\"\n"
              <<"                     switch does the same thing as \"--verbosity=effusive\".\n";
    exit(exit_status);
}

enum Verbosity { SILENT, LACONIC, EFFUSIVE };

// Command-line switches
struct Switches {
    Switches()
        : dbname("clones.db"), firstfuzz(0), nfuzz(10), npointers(20), nnonpointers(100),
          min_funcsz(0), max_insns(5000), verbosity(SILENT), show_progress(false), link(false),
          pointer_analysis(false), follow_calls(false), permute_inputs(0) {}
    std::string dbname;                 /**< Name of database in which to store results. */
    size_t firstfuzz;                   /**< Sequence number for starting fuzz test. */
    size_t nfuzz;                       /**< Number of times to run each function, each time with a different input sequence. */
    size_t npointers, nnonpointers;     /**< Number of pointer and non-pointer values to supply to as inputs per fuzz test. */
    size_t min_funcsz;                  /**< Minimum function size measured in instructions; skip smaller functions. */
    size_t max_insns;                   /**< Maximum number of instrutions per fuzz test before giving up. */
    Verbosity verbosity;                /**< Produce lots of output?  Traces each instruction as it is simulated. */
    bool show_progress;                 /**< Force progress reports even if stderr is a non-terminal or --debug is specified. */
    bool link;                          /**< Perform dynamic linking. */
    std::set<rose_addr_t> functions;    /**< If non-empty, consider only these functions. */
    bool pointer_analysis;              /**< Perform pointer detection analysis?  Use pointer and non-pointer inputs? */
    bool follow_calls;                  /**< Follow CALL instructions if possible rather than consuming an input? */
    size_t permute_inputs;              /**< Number of non-pointer inputs to permute. */
};

/*******************************************************************************************************************************
 *                                      Miscellaneous functions
 *******************************************************************************************************************************/

static void die(std::string mesg="")
{
    std::cerr <<argv0 <<": " <<mesg;
    if (mesg.empty()) {
        std::cerr <<"unspecified failure\n";
        abort(); // for post mortem
    } else if (mesg.find_last_of('\n')==std::string::npos) {
        std::cerr <<"\n";
    }
    std::cerr <<"  See \"--help\" for basic usage information.\n";
    exit(1);
}

template<typename T>
static T
factorial(T n)
{
    T retval = 1;
    while (n>1) {
        T next = retval * n--;
        assert(next>retval); // overflow
        retval = next;
    }
    return retval;
}

// Permute a vector according to the specified permutation number. The permutation number should be between zero (inclusive)
// and the factorial of the values size (exclusive).  A permutation number of zero is a no-op; higher permutation numbers
// shuffle the values in repeatable ways.  Using swap rather that erase/insert is much faster than the standard Lehmer codes,
// but doesn't return permutations in lexicographic order.  This function can perform approx 9.6 million permutations per
// second on a vector of 12 64-bit integers on Robb's machine (computing all 12! permutations in about 50 seconds).
template<typename T>
static void
permute(std::vector<T> &values/*in,out*/, uint64_t pn, size_t sz=(size_t)(-1))
{
    if ((size_t)(-1)==sz)
        sz = values.size();
    assert(sz<=values.size());
    assert(pn<factorial(sz));
    for (size_t i=0; i<sz; ++i) {
        uint64_t radix = sz - i;
        uint64_t idx = pn % radix;
        std::swap(values[i+idx], values[i]);
        pn /= radix;
    }
}




/*****************************************************************************************************************************/








namespace CloneDetection {

typedef std::set<SgAsmFunction*> Functions;
typedef std::map<SgAsmFunction*, int> FunctionIdMap;
typedef BinaryAnalysis::FunctionCall::Graph CG;
typedef boost::graph_traits<CG>::vertex_descriptor CG_Vertex;

/*******************************************************************************************************************************
 *                                      Exceptions
 *******************************************************************************************************************************/

/** Exception thrown by this semantics domain. */
class Exception {
public:
    Exception(const std::string &mesg): mesg(mesg) {}
    std::string mesg;
};

/** Exceptions thrown by the analysis semantics to indicate some kind of fault. */
class FaultException: public Exception {
public:
    AnalysisFault::Fault fault;
    FaultException(AnalysisFault::Fault fault)
        : Exception(std::string("encountered ") + AnalysisFault::fault_name(fault)), fault(fault) {}
};

/*******************************************************************************************************************************
 *                                      Instruction Providor
 *******************************************************************************************************************************/

/** Efficient mapping from address to instruction. */
class InstructionProvidor {
protected:
    typedef std::map<rose_addr_t, SgAsmInstruction*> Addr2Insn;
    Addr2Insn addr2insn;
public:
    InstructionProvidor(const Functions &functions) {
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi)
            insert(*fi);
    }
    InstructionProvidor(SgNode *ast) {
        std::vector<SgAsmFunction*> functions = SageInterface::querySubTree<SgAsmFunction>(ast);
        for (size_t i=0; i<functions.size(); ++i)
            insert(functions[i]);
    }
    void insert(SgAsmFunction *func) {
        std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(func);
        for (size_t i=0; i<insns.size(); ++i)
            addr2insn[insns[i]->get_address()] = insns[i];
    }
    SgAsmInstruction *get_instruction(rose_addr_t addr) const {
        Addr2Insn::const_iterator found = addr2insn.find(addr);
        return found==addr2insn.end() ? NULL : found->second;
    }
};

typedef BinaryAnalysis::PointerAnalysis::PointerDetection<InstructionProvidor> PointerDetector;

/*******************************************************************************************************************************
 *                                      Address hasher
 *******************************************************************************************************************************/

/** Hashes a virtual address to a small integer. */
class AddressHasher {
public:
    AddressHasher() { init(0); }
    AddressHasher(unsigned seed) { init(seed); }

    void init(unsigned seed) {
        lcgN = seed;
        for (size_t i=0; i<256; ++i)
            tab[i] = random8();
    }
    
    uint8_t operator()(rose_addr_t addr, Verbosity verbosity) {
        uint8_t retval = 0;
        for (size_t i=0; i<4; ++i) {
            uint8_t byte = IntegerOps::shiftRightLogical2(addr, 8*i) & 0xff;
            retval = tab[(retval+byte) & 0xff];
        }
        if (verbosity>=EFFUSIVE)
            std::cerr <<"CloneDetection: initializing memory[" <<StringUtility::addrToString(addr) <<"]"
                      <<" = (uint8_t)" <<(unsigned)retval <<"\n";
        return retval;
    }
protected:
    uint8_t random8() {
        lcgN = 6364136223846793005ull * lcgN + 1442695040888963407ull;
        return IntegerOps::shiftRightLogical<64>(lcgN, 7) & 0xff;
    }
private:
    uint64_t lcgN;                      // previous value from linear congruential generator
    uint8_t tab[256];
};
    
/*******************************************************************************************************************************
 *                                      Input Group
 *******************************************************************************************************************************/

/** Initial values to supply for inputs.  These are defined in terms of integers which are then cast to the appropriate size
 *  when needed.  During fuzz testing, whenever the specimen reads from a register or memory location which has never been
 *  written, we consume the next value from this input object. When all values are consumed, this object begins to return only
 *  zero values. */
class InputGroup {
public:
    enum Type { POINTER, NONPOINTER, UNKNOWN_TYPE };
    InputGroup(): next_integer_(0), next_pointer_(0) {}
    void add_integer(uint64_t i) { integers_.push_back(i); }
    void add_pointer(uint64_t p) { pointers_.push_back(p); }
    uint64_t next_integer() {
        uint64_t retval = next_integer_ < integers_.size() ? integers_[next_integer_] : 0;
        ++next_integer_; // increment even past the end so we know how many inputs were consumed
        return retval;
    }
    uint64_t next_pointer() {
        uint64_t retval = next_pointer_ < pointers_.size() ? pointers_[next_pointer_] : 0;
        ++next_pointer_; // increment even past the end so we know how many inputs were consumed
        return retval;
    }
    size_t integers_consumed() const { return next_integer_; }
    size_t pointers_consumed() const { return next_pointer_; }
    const std::vector<uint64_t> get_integers() const { return integers_; }
    const std::vector<uint64_t> get_pointers() const { return pointers_; }
    size_t num_inputs() const { return integers_consumed() + pointers_consumed(); }
    void reset() { next_integer_ = next_pointer_ = 0; }
    void clear() {
        reset();
        integers_.clear();
        pointers_.clear();
    }
    void shuffle() {
        for (size_t i=0; i<integers_.size(); ++i) {
            size_t j = rand() % integers_.size();
            std::swap(integers_[i], integers_[j]);
        }
        for (size_t i=0; i<pointers_.size(); ++i) {
            size_t j = rand() % pointers_.size();
            std::swap(pointers_[i], pointers_[j]);
        }
    }
    std::string toString() const {
        std::ostringstream ss;
        print(ss);
        return ss.str();
    }
    void print(std::ostream &o) const {
        o <<"non-pointer inputs (" <<integers_.size() <<" total):\n";
        for (size_t i=0; i<integers_.size(); ++i)
            o <<"  " <<integers_[i] <<(i==next_integer_?"\t<-- next input":"") <<"\n";
        if (next_integer_>=integers_.size())
            o <<"  all non-pointers have been consumed; returning zero\n";
        o <<"pointer inputs (" <<pointers_.size() <<" total):\n";
        for (size_t i=0; i<pointers_.size(); ++i)
            o <<"  " <<pointers_[i] <<(i==next_pointer_?"\t<-- next input":"") <<"\n";
        if (next_pointer_>=pointers_.size())
            o <<"  all pointers have been consumed; returning null\n";
    }
        
protected:
    std::vector<uint64_t> integers_;
    std::vector<uint64_t> pointers_;
    size_t next_integer_, next_pointer_;        // May increment past the end of its array
};

/*******************************************************************************************************************************
 *                                      Analysis Machine State
 *******************************************************************************************************************************/

/** Bits to track variable access. */
enum {
    NO_ACCESS=0,                        /**< Variable has been neither read nor written. */
    HAS_BEEN_READ=1,                    /**< Variable has been read. */
    HAS_BEEN_WRITTEN=2                  /**< Variable has been written. */ 
};

/** Semantic value to track read/write state of registers. The basic idea is that we have a separate register state object
 *  whose values are instances of this ReadWriteState type. We can use the same RegisterStateX86 template for the read/write
 *  state as we do for the real register state. */
template<size_t nBits>
struct ReadWriteState {
    unsigned state;                     /**< Bit vector containing HAS_BEEN_READ and/or HAS_BEEN_WRITTEN, or zero. */
    ReadWriteState(): state(NO_ACCESS) {}
};

/** One value stored in memory. */
struct MemoryValue {
    MemoryValue(): val(PartialSymbolicSemantics::ValueType<8>(0)), rw_state(NO_ACCESS) {}
    MemoryValue(const PartialSymbolicSemantics::ValueType<8> &val, unsigned rw_state): val(val), rw_state(rw_state) {}
    PartialSymbolicSemantics::ValueType<8> val;
    unsigned rw_state;
};

/** Analysis machine state. We override some of the memory operations. All values are concrete (we're using
 *  PartialSymbolicSemantics only for its constant-folding ability and because we don't yet have a specifically concrete
 *  semantics domain). */
template <template <size_t> class ValueType>
class State: public PartialSymbolicSemantics::State<ValueType> {
public:
    typedef std::map<uint32_t, MemoryValue> MemoryCells;        // memory cells indexed by address
    MemoryCells stack_cells;                                    // memory state for stack memory (accessed via SS register)
    MemoryCells data_cells;                                     // memory state for anything that non-stack (e.g., DS register)
    BaseSemantics::RegisterStateX86<ValueType> registers;
    BaseSemantics::RegisterStateX86<ReadWriteState> register_rw_state;
    OutputGroup output_group;                                  // output values filled in as we run a function

    // Write a single byte to memory. The rw_state are the HAS_BEEN_READ and/or HAS_BEEN_WRITTEN bits.
    void mem_write_byte(X86SegmentRegister sr, const ValueType<32> &addr, const ValueType<8> &value,
                        unsigned rw_state=HAS_BEEN_WRITTEN) {
        MemoryCells &cells = x86_segreg_ss==sr ? stack_cells : data_cells;
        cells[addr.known_value()] = MemoryValue(value, rw_state);
    }
        
    // Read a single byte from memory.  If the read operation cannot find an appropriate memory cell, then @p
    // uninitialized_read is set (it is not cleared in the counter case).
    ValueType<8> mem_read_byte(X86SegmentRegister sr, const ValueType<32> &addr, bool *uninitialized_read/*out*/) {
        MemoryCells &cells = x86_segreg_ss==sr ? stack_cells : data_cells;

        std::vector<ValueType<8> > found;
        typename MemoryCells::iterator ci = cells.find(addr.known_value());
        if (ci!=cells.end())
            found.push_back(ci->second.val);
        if (!found.empty())
            return found[rand()%found.size()];
        *uninitialized_read = true;
        return ValueType<8>(rand()%256);
    }
        
    // Returns true if two memory addresses can be equal.
    static bool may_alias(const ValueType<32> &addr1, const ValueType<32> &addr2) {
        return addr1.known_value()==addr2.known_value();
    }

    // Returns true if two memory address are equivalent.
    static bool must_alias(const ValueType<32> &addr1, const ValueType<32> &addr2) {
        return addr1.known_value()==addr2.known_value();
    }

    // Reset the analysis state by clearing all memory and by resetting the read/written status of all registers.
    void reset_for_analysis() {
        stack_cells.clear();
        data_cells.clear();
        registers.clear();
        register_rw_state.clear();
        output_group.clear();
    }

    // Return output values.  These are the interesting general-purpose registers to which a value has been written, and the
    // memory locations to which a value has been written.  The returned object can be deleted when no longer needed.  The EIP,
    // ESP, and EBP registers are not considered to be interesting.  Memory addresses that are less than or equal to the @p
    // stack_frame_top but larger than @p stack_frame_top - @p frame_size are not considered to be outputs (they are the
    // function's local variables). The @p stack_frame_top is usually the address of the function's return EIP, the address
    // that was pushed onto the stack by the CALL instruction.
    //
    // Even though we're operating in the concrete domain, it is possible for a register or memory location to contain a
    // non-concrete value.  This can happen if only a sub-part of the register was written (e.g., writing a concrete value to
    // AX will result in EAX still having a non-concrete value.
    OutputGroup get_outputs(uint32_t stack_frame_top, size_t frame_size, Verbosity verbosity) {
        OutputGroup outputs = this->output_group;

        // Function return value is EAX, but only if it has been written to and is concrete
        if (0 != (register_rw_state.gpr[x86_gpr_ax].state & HAS_BEEN_WRITTEN) && registers.gpr[x86_gpr_ax].is_known()) {
            if (verbosity>=EFFUSIVE)
                std::cerr <<"output for ax = " <<registers.gpr[x86_gpr_ax].known_value() <<"\n";
            outputs.values.push_back(registers.gpr[x86_gpr_ax].known_value());
        }

        // Add to the outputs the memory cells that are outside the local stack frame (estimated) and are concrete
        for (MemoryCells::iterator ci=stack_cells.begin(); ci!=stack_cells.end(); ++ci) {
            uint32_t addr = ci->first;
            MemoryValue &mval = ci->second;
            bool cell_in_frame = (addr <= stack_frame_top && addr > stack_frame_top-frame_size);
            if (0 != (mval.rw_state & HAS_BEEN_WRITTEN) && mval.val.is_known()) {
                if (verbosity>=EFFUSIVE)
                    std::cerr <<"output for stack address " <<StringUtility::addrToString(addr) <<": "
                              <<mval.val <<(cell_in_frame?" (IGNORED)":"") <<"\n";
                if (!cell_in_frame)
                    outputs.values.push_back(mval.val.known_value());
            }
        }

        // Add to the outputs the non-stack memory cells
        for (MemoryCells::iterator ci=data_cells.begin(); ci!=data_cells.end(); ++ci) {
            uint32_t addr = ci->first;
            MemoryValue &mval = ci->second;
            if (0 != (mval.rw_state & HAS_BEEN_WRITTEN) && mval.val.is_known()) {
                if (verbosity>=EFFUSIVE)
                    std::cerr <<"output for data address " <<addr <<": " <<mval.val <<"\n";
                outputs.values.push_back(mval.val.known_value());
            }
        }

        return outputs;
    }

    // Printing
    void print(std::ostream &o, unsigned domain_mask=0x07) const {
        BaseSemantics::SEMANTIC_NO_PRINT_HELPER *helper = NULL;
        this->registers.print(o, "   ", helper);
        for (size_t i=0; i<2; ++i) {
            size_t ncells=0, max_ncells=100;
            const MemoryCells &cells = 0==i ? stack_cells : data_cells;
            o <<"== Memory (" <<(0==i?"stack":"data") <<" segment) ==\n";
            for (typename MemoryCells::const_iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
                uint32_t addr = ci->first;
                const MemoryValue &mval = ci->second;
                if (++ncells>max_ncells) {
                    o <<"    skipping " <<cells.size()-(ncells-1) <<" more memory cells for brevity's sake...\n";
                    break;
                }
                o <<"         cell access:"
                  <<(0==(mval.rw_state & HAS_BEEN_READ)?"":" read")
                  <<(0==(mval.rw_state & HAS_BEEN_WRITTEN)?"":" written")
                  <<(0==(mval.rw_state & (HAS_BEEN_READ|HAS_BEEN_WRITTEN))?" none":"")
                  <<"\n"
                  <<"    address symbolic: " <<addr <<"\n";
                o <<"        value " <<mval.val <<"\n";
            }
        }
    }
        
    friend std::ostream& operator<<(std::ostream &o, const State &state) {
        state.print(o);
        return o;
    }
};

/*******************************************************************************************************************************
 *                                      Analysis Semantic Policy
 *******************************************************************************************************************************/

// Define the template portion of the CloneDetection::Policy so we don't have to repeat it over and over in the method
// defintions found in CloneDetectionTpl.h.  This also helps Emac's c++-mode auto indentation engine since it seems to
// get confused by complex multi-line templates.
#define CLONE_DETECTION_TEMPLATE template <                                                                                    \
    template <template <size_t> class ValueType> class State,                                                                  \
    template <size_t nBits> class ValueType                                                                                    \
>

CLONE_DETECTION_TEMPLATE
class Policy: public PartialSymbolicSemantics::Policy<State, ValueType> {
public:
    typedef          PartialSymbolicSemantics::Policy<State, ValueType> Super;

    State<ValueType> state;
    static const rose_addr_t INITIAL_STACK = 0x80000000;// Initial value for the EIP and EBP registers
    static const rose_addr_t FUNC_RET_ADDR = 4083;      // Special return address to mark end of analysis
    InputGroup *inputs;                                 // Input values to use when reading a never-before-written variable
    const PointerDetector *pointers;                    // Addresses of pointer variables, or null if not analyzed
    SgAsmInterpretation *interp;                        // Interpretation in which we're executing
    size_t ninsns;                                      // Number of instructions processed since last trigger() call
    AddressHasher address_hasher;                       // Hashes a virtual address
    const InstructionProvidor *insns;                   // Instruction cache
    Switches opt;                                       // Command-line switches

    Policy(const Switches &opt): inputs(NULL), pointers(NULL), ninsns(0), opt(opt) {}

    template <size_t nBits>
    ValueType<nBits>
    next_input_value(InputGroup::Type type) {
        // Instruction semantics API1 calls readRegister when initializing X86InstructionSemantics in order to obtain the
        // original EIP value, but we haven't yet set up an input group (nor would we want this initialization to consume
        // an input anyway).  So just return zero.
        if (!inputs)
            return ValueType<nBits>(0);

        uint64_t value = 0;
        size_t nvalues = 0;
        const char *type_name = NULL;
        switch (type) {
            case InputGroup::POINTER:
                value = inputs->next_pointer();
                nvalues = inputs->pointers_consumed();
                type_name = "pointer";
                break;
            case InputGroup::UNKNOWN_TYPE:
            case InputGroup::NONPOINTER:
                value = inputs->next_integer();
                nvalues = inputs->integers_consumed();
                type_name = "non-pointer";
                break;
        }

        ValueType<nBits> retval(value);
        if (opt.verbosity>=EFFUSIVE)
            std::cerr <<"CloneDetection: using " <<type_name <<" input #" <<nvalues <<": " <<retval <<"\n";
        return retval;
    }

    // Return output values. These include the return value, certain memory writes, function calls, system calls, etc.
    OutputGroup get_outputs() {
        return state.get_outputs(INITIAL_STACK, 8192, opt.verbosity);
    }
    
    // Sets up the machine state to start the analysis of one function.
    void reset(SgAsmInterpretation *interp, SgAsmFunction *func, InputGroup *inputs, const InstructionProvidor *insns,
               const PointerDetector *pointers/*=NULL*/) {
        inputs->reset();
        this->inputs = inputs;
        this->insns = insns;
        this->pointers = pointers;
        this->ninsns = 0;
        this->interp = interp;
        state.reset_for_analysis();
        address_hasher.init(inputs->next_integer());

        // Initialize some registers.  Obviously, the EIP register needs to be set, but we also set the ESP and EBP to known
        // (but arbitrary) values so we can detect when the function returns.  Be sure to use these same values in related
        // analyses (like pointer variable detection).
        rose_addr_t target_va = func->get_entry_va();
        ValueType<32> eip(target_va);
        this->writeRegister("eip", eip);
        ValueType<32> esp(INITIAL_STACK); // stack grows down
        this->writeRegister("esp", esp);
        ValueType<32> ebp(INITIAL_STACK);
        this->writeRegister("ebp", ebp);

        // Initialize callee-saved registers. The callee-saved registers interfere with the analysis because if the same
        // function is compiled two different ways, then it might use different numbers of callee-saved registers.  Since
        // callee-saved registers are pushed onto the stack without first initializing them, the push consumes an input.
        // Therefore, we must consistently initialize all possible callee-saved registers.  We are assuming cdecl calling
        // convention (i.e., GCC's default for C/C++).
        ValueType<32> rval(inputs->next_integer());
        this->writeRegister("ebx", rval);
        this->writeRegister("esi", rval);
        this->writeRegister("edi", rval);

        // Initialize some additional registers.  GCC optimization sometimes preserves a register's value as part of a code
        // path that's shared between points when the register has been initialized and when it hasn't.  Non-optimized code
        // (apparently) never does this.  Therefore, we need to also initialize registers used this way.
        this->writeRegister("eax", rval);
        this->writeRegister("ecx", rval);
        this->writeRegister("edx", rval);
    }
    
    void startInstruction(SgAsmInstruction *insn_) /*override*/ {
        if (++ninsns >= opt.max_insns)
            throw FaultException(AnalysisFault::INSN_LIMIT);
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(insn_);
        assert(insn!=NULL);
        if (opt.verbosity>=EFFUSIVE) {
            std::cerr <<"CloneDetection: " <<std::string(80, '-') <<"\n"
                      <<"CloneDetection: executing: " <<unparseInstructionWithAddress(insn) <<"\n";
        }

        // Make sure EIP is updated with the instruction's address (most policies assert this).
        this->writeRegister("eip", ValueType<32>(insn->get_address()));

        Super::startInstruction(insn_);
    }
        

    // Special handling for some instructions. Like CALL, which does not call the function but rather consumes an input value.
    void finishInstruction(SgAsmInstruction *insn) /*override*/ {
        SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(insn);
        assert(insn_x86!=NULL);

        // Special handling for function calls.  Optionally, instead of calling the function, we treat the function as
        // returning a newly consumed input value to the caller via EAX.  We make the following assumptions:
        //    * Function calls are via CALL instruction
        //    * The called function always returns
        //    * The called function's return value is in the EAX register
        //    * The caller cleans up any arguments that were passed via stack
        //    * The function's return value is a non-pointer type
        if (x86_call==insn_x86->get_kind()) {
            bool follow = opt.follow_calls;
            ValueType<32> callee_va = this->template readRegister<32>("eip");
            if (follow) {
                SgAsmInstruction *called_insn = insns->get_instruction(callee_va.known_value());
                SgAsmFunction *called_func = called_insn ? SageInterface::getEnclosingNode<SgAsmFunction>(called_insn) : NULL;
                if ((follow = called_insn!=NULL && called_func!=NULL)) {
                    std::string func_name = called_func->get_name();
                    if (func_name.size()>4 && 0==func_name.substr(func_name.size()-4).compare("@plt"))
                        follow = false;
                }
            }
            if (!follow) {
                if (opt.verbosity>=EFFUSIVE)
                    std::cerr <<"CloneDetection: special handling for function call (fall through and return via EAX)\n";
                state.output_group.callees_va.push_back(callee_va.known_value());
                ValueType<32> call_fallthrough_va = this->template number<32>(insn->get_address() + insn->get_size());
                this->writeRegister("eip", call_fallthrough_va);
                this->writeRegister("eax", next_input_value<32>(InputGroup::NONPOINTER));
                ValueType<32> esp = this->template readRegister<32>("esp");
                esp = this->add(esp, ValueType<32>(4));
                this->writeRegister("esp", esp);
            }
        }

        Super::finishInstruction(insn);
    }
    
    // Handle INT 0x80 instructions: save the system call number (from EAX) in the output group and set EAX to a random
    // value, thus consuming one input.
    void interrupt(uint8_t inum) /*override*/ {
        if (0x80==inum) {
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: special handling for system call (fall through and consume an input into EAX)\n";
            ValueType<32> syscall_num = this->template readRegister<32>("eax");
            state.output_group.syscalls.push_back(syscall_num.known_value());
            this->writeRegister("eax", next_input_value<32>(InputGroup::NONPOINTER));
        } else {
            Super::interrupt(inum);
            throw FaultException(AnalysisFault::INTERRUPT);
        }
    }

    // Handle the HLT instruction by throwing an exception.
    void hlt() {
        throw FaultException(AnalysisFault::HALT);
    }

    // Track memory access.
    template<size_t nBits>
    ValueType<nBits> readMemory(X86SegmentRegister sr, ValueType<32> a0, const ValueType<1> &cond) {
        // For RET instructions, when reading DWORD PTR ss:[INITIAL_STACK], do not consume an input, but rather
        // return FUNC_RET_ADDR.
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(this->get_insn());
        if (32==nBits && insn && x86_ret==insn->get_kind()) {
            rose_addr_t c_addr = a0.known_value();
            if (c_addr == this->INITIAL_STACK)
                return ValueType<nBits>(this->FUNC_RET_ADDR);
        }

        // Read a multi-byte value from memory in little-endian order.
        bool uninitialized_read = false; // set to true by any mem_read_byte() that has no data
        assert(8==nBits || 16==nBits || 32==nBits);
        ValueType<32> dword = this->concat(state.mem_read_byte(sr, a0, &uninitialized_read),
                                           ValueType<24>(0));
        if (nBits>=16) {
            ValueType<32> a1 = this->add(a0, ValueType<32>(1));
            dword = this->or_(dword, this->concat(ValueType<8>(0),
                                                  this->concat(state.mem_read_byte(sr, a1, &uninitialized_read),
                                                               ValueType<16>(0))));
        }
        if (nBits>=24) {
            ValueType<32> a2 = this->add(a0, ValueType<32>(2));
            dword = this->or_(dword, this->concat(ValueType<16>(0),
                                                  this->concat(state.mem_read_byte(sr, a2, &uninitialized_read),
                                                               ValueType<8>(0))));
        }
        if (nBits>=32) {
            ValueType<32> a3 = this->add(a0, ValueType<32>(3));
            dword = this->or_(dword, this->concat(ValueType<24>(0), state.mem_read_byte(sr, a3, &uninitialized_read)));
        }

        ValueType<nBits> retval = this->template extract<0, nBits>(dword);
        if (uninitialized_read) {
            // At least one of the bytes read did not previously exist, so we need to initialize these memory locations.
            // Sometimes we want memory to have a value that depends on the next input, and other times we want a value that
            // depends on the address.
            bool consume_input = false;
            if (this->interp!=NULL && this->interp->get_map()!=NULL) {
                consume_input = this->interp->get_map()->exists(a0.known_value());
            }
            if (consume_input) {
                // Return either a pointer or non-pointer value depending pointer detection analysis
                SymbolicSemantics::ValueType<32> a0_sym(a0.known_value());
                InputGroup::Type type = this->pointers!=NULL && this->pointers->is_pointer(a0_sym) ?
                                        InputGroup::POINTER : InputGroup::NONPOINTER;
                retval = next_input_value<nBits>(type);
            } else {
                // Return a value which is a function of the address (and one of the inputs that was used to initialize the
                // hash function).
                retval = ValueType<nBits>(address_hasher(a0.known_value(), opt.verbosity));
            }
            // Write the value back to memory so the same value is read next time.
            this->writeMemory<nBits>(sr, a0, retval, this->true_(), HAS_BEEN_READ);
        }

        return retval;
    }
        
    template<size_t nBits>
    void writeMemory(X86SegmentRegister sr, ValueType<32> a0, const ValueType<nBits> &data, const ValueType<1> &cond,
                     unsigned rw_state=HAS_BEEN_WRITTEN) {

        // Add the address/value pair to the memory state one byte at a time in little-endian order.
        assert(8==nBits || 16==nBits || 32==nBits);
        ValueType<8> b0 = this->template extract<0, 8>(data);
        state.mem_write_byte(sr, a0, b0, rw_state);
        if (nBits>=16) {
            ValueType<32> a1 = this->add(a0, ValueType<32>(1));
            ValueType<8> b1 = this->template extract<8, 16>(data);
            state.mem_write_byte(sr, a1, b1, rw_state);
        }
        if (nBits>=24) {
            ValueType<32> a2 = this->add(a0, ValueType<32>(2));
            ValueType<8> b2 = this->template extract<16, 24>(data);
            state.mem_write_byte(sr, a2, b2, rw_state);
        }
        if (nBits>=32) {
            ValueType<32> a3 = this->add(a0, ValueType<32>(3));
            ValueType<8> b3 = this->template extract<24, 32>(data);
            state.mem_write_byte(sr, a3, b3, rw_state);
        }
    }

    // Track register access
    template<size_t nBits>
    ValueType<nBits> readRegister(const char *regname) {
        const RegisterDescriptor &reg = this->findRegister(regname, nBits);
        return this->template readRegister<nBits>(reg);
    }

    template<size_t nBits>
    ValueType<nBits> readRegister(const RegisterDescriptor &reg) {
        ValueType<nBits> retval;
        switch (nBits) {
            case 1: {
                // Only FLAGS/EFLAGS bits have a size of one.  Other registers cannot be accessed at this granularity.
                if (reg.get_major()!=x86_regclass_flags)
                    throw Exception("bit access only valid for FLAGS/EFLAGS register");
                if (reg.get_minor()!=0 || reg.get_offset()>=state.registers.n_flags)
                    throw Exception("register not implemented in semantic policy");
                if (reg.get_nbits()!=1)
                    throw Exception("semantic policy supports only single-bit flags");
                bool never_accessed = 0 == state.register_rw_state.flag[reg.get_offset()].state;
                state.register_rw_state.flag[reg.get_offset()].state |= HAS_BEEN_READ;
                if (never_accessed)
                    state.registers.flag[reg.get_offset()] = next_input_value<1>(InputGroup::NONPOINTER);
                retval = this->template unsignedExtend<1, nBits>(state.registers.flag[reg.get_offset()]);
                break;
            }

            case 8: {
                // Only general-purpose registers can be accessed at a byte granularity, and we can access only the low-order
                // byte or the next higher byte.  For instance, "al" and "ah" registers.
                if (reg.get_major()!=x86_regclass_gpr)
                    throw Exception("byte access only valid for general purpose registers");
                if (reg.get_minor()>=state.registers.n_gprs)
                    throw Exception("register not implemented in semantic policy");
                assert(reg.get_nbits()==8); // we had better be asking for a one-byte register (e.g., "ah", not "ax")
                bool never_accessed = 0==state.register_rw_state.gpr[reg.get_minor()].state;
                state.register_rw_state.gpr[reg.get_minor()].state |= HAS_BEEN_READ;
                if (never_accessed)
                    state.registers.gpr[reg.get_minor()] = next_input_value<32>(InputGroup::NONPOINTER);
                switch (reg.get_offset()) {
                    case 0:
                        retval = this->template extract<0, nBits>(state.registers.gpr[reg.get_minor()]);
                        break;
                    case 8:
                        retval = this->template extract<8, 8+nBits>(state.registers.gpr[reg.get_minor()]);
                        break;
                    default:
                        throw Exception("invalid one-byte access offset");
                }
                break;
            }

            case 16: {
                if (reg.get_nbits()!=16)
                    throw Exception("invalid 2-byte register");
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for word granularity register access");
                switch (reg.get_major()) {
                    case x86_regclass_segment: {
                        if (reg.get_minor()>=state.registers.n_segregs)
                            throw Exception("register not implemented in semantic policy");
                        bool never_accessed = 0==state.register_rw_state.segreg[reg.get_minor()].state;
                        state.register_rw_state.segreg[reg.get_minor()].state |= HAS_BEEN_READ;
                        if (never_accessed)
                            state.registers.segreg[reg.get_minor()] = next_input_value<16>(InputGroup::NONPOINTER);
                        retval = this->template unsignedExtend<16, nBits>(state.registers.segreg[reg.get_minor()]);
                        break;
                    }
                    case x86_regclass_gpr: {
                        if (reg.get_minor()>=state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        bool never_accessed = 0==state.register_rw_state.gpr[reg.get_minor()].state;
                        state.register_rw_state.segreg[reg.get_minor()].state |= HAS_BEEN_READ;
                        if (never_accessed)
                            state.registers.gpr[reg.get_minor()] = next_input_value<32>(InputGroup::NONPOINTER);
                        retval = this->template extract<0, nBits>(state.registers.gpr[reg.get_minor()]);
                        break;
                    }

                    case x86_regclass_flags: {
                        if (reg.get_minor()!=0 || state.registers.n_flags<16)
                            throw Exception("register not implemented in semantic policy");
                        for (size_t i=0; i<16; ++i) {
                            bool never_accessed = 0==state.register_rw_state.flag[i].state;
                            state.register_rw_state.flag[i].state |= HAS_BEEN_READ;
                            if (never_accessed)
                                state.registers.flag[i] = next_input_value<1>(InputGroup::NONPOINTER);
                        }
                        retval = this->template unsignedExtend<16, nBits>(concat(state.registers.flag[0],
                                                                          concat(state.registers.flag[1],
                                                                          concat(state.registers.flag[2],
                                                                          concat(state.registers.flag[3],
                                                                          concat(state.registers.flag[4],
                                                                          concat(state.registers.flag[5],
                                                                          concat(state.registers.flag[6],
                                                                          concat(state.registers.flag[7],
                                                                          concat(state.registers.flag[8],
                                                                          concat(state.registers.flag[9],
                                                                          concat(state.registers.flag[10],
                                                                          concat(state.registers.flag[11],
                                                                          concat(state.registers.flag[12],
                                                                          concat(state.registers.flag[13],
                                                                          concat(state.registers.flag[14],
                                                                                 state.registers.flag[15]))))))))))))))));
                        break;
                    }
                    default:
                        throw Exception("word access not valid for this register type");
                }
                break;
            }

            case 32: {
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for double word granularity register access");
                switch (reg.get_major()) {
                    case x86_regclass_gpr: {
                        if (reg.get_minor()>=state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        bool never_accessed = 0==state.register_rw_state.gpr[reg.get_minor()].state;
                        state.register_rw_state.gpr[reg.get_minor()].state |= HAS_BEEN_READ;
                        if (never_accessed)
                            state.registers.gpr[reg.get_minor()] = next_input_value<32>(InputGroup::UNKNOWN_TYPE);
                        retval = this->template unsignedExtend<32, nBits>(state.registers.gpr[reg.get_minor()]);
                        break;
                    }
                    case x86_regclass_ip: {
                        if (reg.get_minor()!=0)
                            throw Exception("register not implemented in semantic policy");
                        bool never_accessed = 0==state.register_rw_state.ip.state;
                        state.register_rw_state.ip.state |= HAS_BEEN_READ;
                        if (never_accessed)
                            state.registers.ip = next_input_value<32>(InputGroup::POINTER);
                        retval = this->template unsignedExtend<32, nBits>(state.registers.ip);
                        break;
                    }
                    case x86_regclass_segment: {
                        if (reg.get_minor()>=state.registers.n_segregs || reg.get_nbits()!=16)
                            throw Exception("register not implemented in semantic policy");
                        bool never_accessed = 0==state.register_rw_state.segreg[reg.get_minor()].state;
                        state.register_rw_state.segreg[reg.get_minor()].state |= HAS_BEEN_READ;
                        if (never_accessed)
                            state.registers.segreg[reg.get_minor()] = next_input_value<16>(InputGroup::UNKNOWN_TYPE);
                        retval = this->template unsignedExtend<16, nBits>(state.registers.segreg[reg.get_minor()]);
                        break;
                    }
                    case x86_regclass_flags: {
                        if (reg.get_minor()!=0 || state.registers.n_flags<32)
                            throw Exception("register not implemented in semantic policy");
                        if (reg.get_nbits()!=32)
                            throw Exception("register is not 32 bits");
                        for (size_t i=0; i<32; ++i) {
                            bool never_accessed = 0==state.register_rw_state.flag[i].state;
                            state.register_rw_state.flag[i].state |= HAS_BEEN_READ;
                            if (never_accessed)
                                state.registers.flag[i] = next_input_value<1>(InputGroup::NONPOINTER);
                        }
                        retval = this->template unsignedExtend<32, nBits>(concat(state.registers.flag[0],
                                                                          concat(state.registers.flag[1],
                                                                          concat(state.registers.flag[2],
                                                                          concat(state.registers.flag[3],
                                                                          concat(state.registers.flag[4],
                                                                          concat(state.registers.flag[5],
                                                                          concat(state.registers.flag[6],
                                                                          concat(state.registers.flag[7],
                                                                          concat(state.registers.flag[8],
                                                                          concat(state.registers.flag[9],
                                                                          concat(state.registers.flag[10],
                                                                          concat(state.registers.flag[11],
                                                                          concat(state.registers.flag[12],
                                                                          concat(state.registers.flag[13],
                                                                          concat(state.registers.flag[14],
                                                                          concat(state.registers.flag[15],
                                                                          concat(state.registers.flag[16],
                                                                          concat(state.registers.flag[17],
                                                                          concat(state.registers.flag[18],
                                                                          concat(state.registers.flag[19],
                                                                          concat(state.registers.flag[20],
                                                                          concat(state.registers.flag[21],
                                                                          concat(state.registers.flag[22],
                                                                          concat(state.registers.flag[23],
                                                                          concat(state.registers.flag[24],
                                                                          concat(state.registers.flag[25],
                                                                          concat(state.registers.flag[26],
                                                                          concat(state.registers.flag[27],
                                                                          concat(state.registers.flag[28],
                                                                          concat(state.registers.flag[29],
                                                                          concat(state.registers.flag[30],
                                                                                 state.registers.flag[31]
                                                                                 ))))))))))))))))))))))))))))))));
                        break;
                    }
                    default:
                        throw Exception("double word access not valid for this register type");
                }
                break;
            }
            default:
                throw Exception("invalid register access width");
        }
        return retval;
    }

    template<size_t nBits>
    void writeRegister(const char *regname, const ValueType<nBits> &value) {
        const RegisterDescriptor &reg = this->findRegister(regname, nBits);
        this->template writeRegister(reg, value);
    }

    template<size_t nBits>
    void writeRegister(const RegisterDescriptor &reg, const ValueType<nBits> &value, unsigned update_access=HAS_BEEN_WRITTEN) {
        switch (nBits) {
            case 1: {
                // Only FLAGS/EFLAGS bits have a size of one.  Other registers cannot be accessed at this granularity.
                if (reg.get_major()!=x86_regclass_flags)
                    throw Exception("bit access only valid for FLAGS/EFLAGS register");
                if (reg.get_minor()!=0 || reg.get_offset()>=state.registers.n_flags)
                    throw Exception("register not implemented in semantic policy");
                if (reg.get_nbits()!=1)
                    throw Exception("semantic policy supports only single-bit flags");
                state.registers.flag[reg.get_offset()] = this->template unsignedExtend<nBits, 1>(value);
                state.register_rw_state.flag[reg.get_offset()].state |= update_access;
                break;
            }

            case 8: {
                // Only general purpose registers can be accessed at byte granularity, and only for offsets 0 and 8.
                if (reg.get_major()!=x86_regclass_gpr)
                    throw Exception("byte access only valid for general purpose registers.");
                if (reg.get_minor()>=state.registers.n_gprs)
                    throw Exception("register not implemented in semantic policy");
                assert(reg.get_nbits()==8); // we had better be asking for a one-byte register (e.g., "ah", not "ax")
                bool never_accessed = 0==state.register_rw_state.gpr[reg.get_minor()].state;
                state.register_rw_state.gpr[reg.get_minor()].state |= update_access;
                if (never_accessed)
                    state.registers.gpr[reg.get_minor()] = next_input_value<32>(InputGroup::NONPOINTER);
                switch (reg.get_offset()) {
                    case 0:
                        state.registers.gpr[reg.get_minor()] =
                            concat(this->template signExtend<nBits, 8>(value),
                                   this->template extract<8, 32>(state.registers.gpr[reg.get_minor()])); // no-op extend
                        break;
                    case 8:
                        state.registers.gpr[reg.get_minor()] =
                            concat(this->template extract<0, 8>(state.registers.gpr[reg.get_minor()]),
                                   concat(this->template unsignedExtend<nBits, 8>(value),
                                          this->template extract<16, 32>(state.registers.gpr[reg.get_minor()])));
                        break;
                    default:
                        throw Exception("invalid byte access offset");
                }
                break;
            }

            case 16: {
                if (reg.get_nbits()!=16)
                    throw Exception("invalid 2-byte register");
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for word granularity register access");
                switch (reg.get_major()) {
                    case x86_regclass_segment: {
                        if (reg.get_minor()>=state.registers.n_segregs)
                            throw Exception("register not implemented in semantic policy");
                        state.registers.segreg[reg.get_minor()] = this->template unsignedExtend<nBits, 16>(value);
                        state.register_rw_state.segreg[reg.get_minor()].state |= update_access;
                        break;
                    }
                    case x86_regclass_gpr: {
                        if (reg.get_minor()>=state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        bool never_accessed = 0==state.register_rw_state.gpr[reg.get_minor()].state;
                        state.register_rw_state.gpr[reg.get_minor()].state |= update_access;
                        if (never_accessed)
                            state.registers.gpr[reg.get_minor()] = next_input_value<32>(InputGroup::NONPOINTER);
                        state.registers.gpr[reg.get_minor()] =
                            concat(this->template unsignedExtend<nBits, 16>(value),
                                   this->template extract<16, 32>(state.registers.gpr[reg.get_minor()]));
                        break;
                    }
                    case x86_regclass_flags: {
                        if (reg.get_minor()!=0 || state.registers.n_flags<16)
                            throw Exception("register not implemented in semantic policy");
                        state.registers.flag[0]  = this->template extract<0,  1 >(value);
                        state.registers.flag[1]  = this->template extract<1,  2 >(value);
                        state.registers.flag[2]  = this->template extract<2,  3 >(value);
                        state.registers.flag[3]  = this->template extract<3,  4 >(value);
                        state.registers.flag[4]  = this->template extract<4,  5 >(value);
                        state.registers.flag[5]  = this->template extract<5,  6 >(value);
                        state.registers.flag[6]  = this->template extract<6,  7 >(value);
                        state.registers.flag[7]  = this->template extract<7,  8 >(value);
                        state.registers.flag[8]  = this->template extract<8,  9 >(value);
                        state.registers.flag[9]  = this->template extract<9,  10>(value);
                        state.registers.flag[10] = this->template extract<10, 11>(value);
                        state.registers.flag[11] = this->template extract<11, 12>(value);
                        state.registers.flag[12] = this->template extract<12, 13>(value);
                        state.registers.flag[13] = this->template extract<13, 14>(value);
                        state.registers.flag[14] = this->template extract<14, 15>(value);
                        state.registers.flag[15] = this->template extract<15, 16>(value);
                        for (size_t i=0; i<state.register_rw_state.n_flags; ++i)
                            state.register_rw_state.flag[i].state |= update_access;
                        break;
                    }
                    default:
                        throw Exception("word access not valid for this register type");
                }
                break;
            }

            case 32: {
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for double word granularity register access");
                switch (reg.get_major()) {
                    case x86_regclass_gpr: {
                        if (reg.get_minor()>=state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        state.registers.gpr[reg.get_minor()] = this->template signExtend<nBits, 32>(value);
                        state.register_rw_state.gpr[reg.get_minor()].state |= update_access;
                        break;
                    }
                    case x86_regclass_ip: {
                        if (reg.get_minor()!=0)
                            throw Exception("register not implemented in semantic policy");
                        state.registers.ip = this->template unsignedExtend<nBits, 32>(value);
                        state.register_rw_state.ip.state |= update_access;
                        break;
                    }
                    case x86_regclass_flags: {
                        if (reg.get_minor()!=0 || state.registers.n_flags<32)
                            throw Exception("register not implemented in semantic policy");
                        if (reg.get_nbits()!=32)
                            throw Exception("register is not 32 bits");
                        this->template writeRegister<16>("flags", this->template unsignedExtend<nBits, 16>(value));
                        state.registers.flag[16] = this->template extract<16, 17>(value);
                        state.registers.flag[17] = this->template extract<17, 18>(value);
                        state.registers.flag[18] = this->template extract<18, 19>(value);
                        state.registers.flag[19] = this->template extract<19, 20>(value);
                        state.registers.flag[20] = this->template extract<20, 21>(value);
                        state.registers.flag[21] = this->template extract<21, 22>(value);
                        state.registers.flag[22] = this->template extract<22, 23>(value);
                        state.registers.flag[23] = this->template extract<23, 24>(value);
                        state.registers.flag[24] = this->template extract<24, 25>(value);
                        state.registers.flag[25] = this->template extract<25, 26>(value);
                        state.registers.flag[26] = this->template extract<26, 27>(value);
                        state.registers.flag[27] = this->template extract<27, 28>(value);
                        state.registers.flag[28] = this->template extract<28, 29>(value);
                        state.registers.flag[29] = this->template extract<29, 30>(value);
                        state.registers.flag[30] = this->template extract<30, 31>(value);
                        state.registers.flag[31] = this->template extract<31, 32>(value);
                        for (size_t i=0; i<state.register_rw_state.n_flags; ++i)
                            state.register_rw_state.flag[i].state |= update_access;
                        break;
                    }
                    default:
                        throw Exception("double word access not valid for this register type");
                }
                break;
            }

            default:
                throw Exception("invalid register access width");
        }
    }
        

    // Print the state, including memory and register access flags
    void print(std::ostream &o, bool abbreviated=false) const {
        state.print(o, abbreviated?this->get_active_policies() : 0x07);
    }
    
    friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
        p.print(o);
        return o;
    }
};

/*******************************************************************************************************************************
 *                                      Clone Detection Analysis
 *******************************************************************************************************************************/


/** Main driving function for clone detection.  This is the class that chooses inputs, runs each function, and looks at the
 *  outputs to decide how to partition the functions.  It does this repeatedly in order to build a PartitionForest. The
 *  analyze() method is the main entry point. */
class Analysis {
protected:
    sqlite3_connection sqlite;          /**< Database in which to place results; may already exist */
    OutputGroups output_groups;         /**< Distinct groups of output values from fuzz tests. */
    Switches opt;                       /**< Analysis configuration switches from the command line. */
    FunctionIdMap func_ids;             /**< Mapping from SgAsmFunction* to the ID stored in the database. */

    Policy<State, PartialSymbolicSemantics::ValueType> policy;
    typedef X86InstructionSemantics<Policy<State, PartialSymbolicSemantics::ValueType>,
                                    PartialSymbolicSemantics::ValueType> Semantics;
    Semantics semantics;

public:
    Analysis(const Switches &opt): opt(opt), policy(opt), semantics(policy) {
        open_db(opt.dbname);
    }

    void open_db(const std::string &dbname) {
        sqlite.open(dbname.c_str());
        sqlite.busy_timeout(15*60*1000); // 15 minutes
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);

        // Execute the SQL that describes the schema.  We used to just pipe this into the sqlite3 command, but we had
        // problems when running in parallel.  It would be nice to be able to pass the whole string to executenonquery(),
        // but that seems to execute only the first statement in the string.  Therefore, we split the big, multi-line string
        // into statements and execute each individually.
        extern const char *schema; // contents of Schema.sql file
        std::string stmts(schema);
        boost::regex stmt_re("( ( '([^']|'')*'   )"     // string literal
                             "| ( --[^\n]*       )"     // comment
                             "| ( [^;]           )"     // other
                             ")+ ;", boost::regex::perl|boost::regex::mod_x);
        typedef boost::algorithm::find_iterator<std::string::iterator> Sfi; // string find iterator
        for (Sfi i=make_find_iterator(stmts, boost::algorithm::regex_finder(stmt_re)); i!=Sfi(); ++i) {
            std::string stmt = boost::copy_range<std::string>(*i);
            try {
                sqlite.executenonquery(stmt);
            } catch (const database_error &e) {
                std::cerr <<stmt <<"\n";
                throw;
            }
        }

        // Populate the semantic_faults table.
        struct FaultInserter {
            FaultInserter(sqlite3_connection &db, int id, const char *name, const char *desc) {
                sqlite3_command cmd1(db, "select count(*) from semantic_faults where id = ?");
                cmd1.bind(1, id);
                if (cmd1.executeint()==0) {
                    sqlite3_command cmd2(db, "insert into semantic_faults (id, name, description) values (?,?,?)");
                    cmd2.bind(1, id);
                    cmd2.bind(2, name);
                    cmd2.bind(3, desc);
                    cmd2.executenonquery();
                }
            }
        };
#define add_fault(DB, ID, DESC) FaultInserter(DB, ID, #ID, DESC)
        add_fault(sqlite, AnalysisFault::DISASSEMBLY, "disassembly failed");
        add_fault(sqlite, AnalysisFault::INSN_LIMIT,  "simulation instruction limit reached");
        add_fault(sqlite, AnalysisFault::HALT,        "x86 HLT instruction executed");
        add_fault(sqlite, AnalysisFault::INTERRUPT,   "interrupt or x86 INT instruction executed");
        add_fault(sqlite, AnalysisFault::SEMANTICS,   "instruction semantics error");
        add_fault(sqlite, AnalysisFault::SMTSOLVER,   "SMT solver error");
#undef  add_fault

        lock.commit();
    }

    // Allocate a page of memory in the analysis state
    rose_addr_t allocate_page() {
        static size_t ncalls = 0;
        static const uint32_t base = 0x40000000;
        static const uint32_t npages = 512;
        static const uint32_t page_size = 4096;
        return base + (ncalls++ % npages)*page_size;
    }
    
    // Get a list of functions to analyze.
    Functions find_functions(SgAsmInterpretation *interp, const std::set<rose_addr_t> &limited) {
        Functions retval;
        std::vector<SgAsmFunction*> allfuncs = SageInterface::querySubTree<SgAsmFunction>(interp);
        for (size_t i=0; i<allfuncs.size(); ++i) {
            size_t ninsns = SageInterface::querySubTree<SgAsmInstruction>(allfuncs[i]).size();
            if ((0==opt.min_funcsz || ninsns >= opt.min_funcsz) &&
                (limited.empty() || limited.find(allfuncs[i]->get_entry_va())!=limited.end()))
                retval.insert(allfuncs[i]);
        }
        return retval;
    }

    // Perform a pointer-detection analysis on the specified function. We'll need the results in order to determine whether a
    // function input should consume a pointer or a non-pointer from the input value set.
    PointerDetector* detect_pointers(SgAsmFunction *func) {
        if (!opt.pointer_analysis)
            return NULL;

        // Choose an SMT solver. This is completely optional.  Pointer detection still seems to work fairly well (and much,
        // much faster) without an SMT solver.
        SMTSolver *solver = NULL;
#if 0   // optional code
        if (YicesSolver::available_linkage())
            solver = new YicesSolver;
#endif
        InstructionProvidor insn_providor(func);
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection: " <<function_to_str(func) <<" pointer detection analysis\n";
        PointerDetector *pd = new PointerDetector(&insn_providor, solver);
        pd->initial_state().registers.gpr[x86_gpr_sp] = SymbolicSemantics::ValueType<32>(policy.INITIAL_STACK);
        pd->initial_state().registers.gpr[x86_gpr_bp] = SymbolicSemantics::ValueType<32>(policy.INITIAL_STACK);
        //pd.set_debug(stderr);
        try {
            pd->analyze(func);
        } catch (...) {
            // probably the instruction is not handled by the semantics used in the analysis.  For example, the
            // instruction might be a floating point instruction that isn't handled yet.
            std::cerr <<"CloneDetection: pointer analysis FAILED for " <<function_to_str(func) <<"\n";
        }
        if (opt.verbosity>=EFFUSIVE) {
            const PointerDetector::Pointers plist = pd->get_pointers();
            for (PointerDetector::Pointers::const_iterator pi=plist.begin(); pi!=plist.end(); ++pi) {
                std::cerr <<"    ";
                if (pi->type & BinaryAnalysis::PointerAnalysis::DATA_PTR)
                    std::cerr <<"data ";
                if (pi->type & BinaryAnalysis::PointerAnalysis::CODE_PTR)
                    std::cerr <<"code ";
                std::cerr <<"pointer at " <<pi->address <<"\n";
            }
        }
        return pd;
    }

    // Get the ID for a file, adding a new entry to the table if necessary.
    // Must aquire the write lock before calling this function.
    int get_file_id(const std::string &filename) {
        sqlite3_command cmd1(sqlite, "select coalesce(max(id),-1) from semantic_files where name = ?");
        sqlite3_command cmd2(sqlite,
                             "insert into semantic_files (id, name)"
                             "values ((select coalesce(max(id),-1)+1 from semantic_files), ?)");

        cmd1.bind(1, filename);
        int file_id = cmd1.executeint();
        if (file_id < 0) {
            cmd2.bind(1, filename);
            cmd2.executenonquery();
            file_id = cmd1.executeint();
            assert(file_id>=0);
        }
        return file_id;
    }

    static std::string
    digest_to_str(const unsigned char digest[20]) {
        std::string digest_str;
        for (size_t i=20; i>0; --i) {
            digest_str += "0123456789abcdef"[(digest[i-1] >> 4) & 0xf];
            digest_str += "0123456789abcdef"[digest[i-1] & 0xf];
        }
        return digest_str;
    }

    struct FuncStats {
        FuncStats(): file_id(-1), isize(0), dsize(0), size(0), ninsns(0) {}
        FuncStats(size_t isize, size_t dsize, size_t size, size_t ninsns, const std::string &digest)
            : file_id(-1), isize(isize), dsize(dsize), size(size), ninsns(ninsns), digest(digest) {}
        int file_id;
        size_t isize, dsize, size, ninsns;
        std::string digest;                             // SHA1 hash of function bytes in virtual address order if known
    };


    // Save each function to the database. Updates the mapping from function object to ID number.
    // We have to do this in a database-efficient manner without holding a transaction lock too long because we're
    // likely to be running in parallel.
    void save_functions(SgAsmInterpretation *interp, const Functions &functions) {
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection: saving function information and assembly listings...\n";

        typedef std::map<rose_addr_t/*entry_va*/, SgAsmFunction*> AddrFunc;
        typedef std::map<std::string/* sha1 */, SgAsmFunction*> Sha1Func;
        typedef std::map<SgAsmFunction*, FuncStats> Stats;

        struct InstructionSelector: SgAsmFunction::NodeSelector {
            virtual bool operator()(SgNode *node) { return isSgAsmInstruction(node)!=NULL; }
        } iselector;

        struct DataSelector: SgAsmFunction::NodeSelector {
            virtual bool operator()(SgNode *node) { return isSgAsmStaticData(node)!=NULL; }
        } dselector;

        // Pre-compute some function info that doesn't depend on the database.
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   calculating function information from AST"
                      <<" (" <<functions.size() <<" function" <<(1==functions.size()?"":"s") <<")\n";
        AddrFunc addrfunc;
        Sha1Func sha1func;
        Stats stats;
        assert(gcry_md_get_algo_dlen(GCRY_MD_SHA1)==20);
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            SgAsmFunction *func = *fi;
            ExtentMap e_insns, e_data, e_total;
            size_t ninsns = func->get_extent(&e_insns, NULL, NULL, &iselector);
            func->get_extent(&e_data,  NULL, NULL, &dselector);
            func->get_extent(&e_total);
            uint8_t digest[20];
            func->get_sha1(digest);
            std::string digest_str = digest_to_str(digest);
            stats[func] = FuncStats(e_insns.size(), e_data.size(), e_total.size(), ninsns, digest_str);
            addrfunc[func->get_entry_va()] = func;
            sha1func[digest_str] = func;
        }

        // Populate the semantic_files table (assuming our functions can come from multiple files)
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   populating file table from binary function info\n";
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi)
            stats[*fi].file_id = get_file_id(filename_for_function(*fi));
        lock.commit();

        // Scan the whole function table and keep only those entries that correspond to our functions.  Since our functions
        // might come from multiple files, there isn't an easy way to limit the query to only those files (we could use "in"
        // but this seems about as fast).
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   getting function IDs from the database\n";
        sqlite3_command cmd1(sqlite, "select id, entry_va, file_id, digest from semantic_functions");
        lock.begin(sqlite3_transaction::LOCK_IMMEDIATE);
        sqlite3_reader c1 = cmd1.executereader();
        while (c1.read()) {
            int function_id = c1.getint(0);
            rose_addr_t entry_va = c1.getint64(1);
            int file_id = c1.getint(2);
            std::string digest = c1.getstring(3);
            Sha1Func::iterator sha1_found = sha1func.find(digest);
            SgAsmFunction *func = NULL;
            if (sha1_found!=sha1func.end()) {
                func = sha1_found->second;
            } else {
                AddrFunc::iterator addr_found = addrfunc.find(entry_va);
                if (addr_found!=addrfunc.end()) {
                    func = addr_found->second;
                }
            }
            // Functions are the same if they have either the same address or message digest, and they come from the same file.
            if (func!=NULL && file_id==stats[func].file_id)
                func_ids[func] = function_id;
        }
        lock.commit();
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   " <<func_ids.size() <<" of our functions " <<(1==func_ids.size()?"is":"are")
                      <<" already in the database\n";

        // Figure out which functions are not in the database.  By time we get done with this loop some other process might
        // have added those functions, but we'll handle that later.
        Functions to_add;
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            SgAsmFunction *func = *fi;
            if (func_ids.find(func)==func_ids.end())
                to_add.insert(func);
        }

        // Add necessary functions to the semantic_functions table, but be careful because some other process might have added
        // them since we released the lock above.
        if (opt.verbosity>=LACONIC) {
            std::cerr <<"CloneDetection:   adding " <<to_add.size() <<" function ID" <<(1==to_add.size()?"":"s")
                      <<" to the database\n";
        }
        sqlite3_command cmd2(sqlite, "select coalesce(max(id),-1) from semantic_functions where entry_va=? and file_id=?");
        sqlite3_command cmd3(sqlite, "insert into semantic_functions"
                             // 1   2         3         4        5      6      7     8       9
                             " (id, entry_va, funcname, file_id, isize, dsize, size, ninsns, digest)"
                             " values (?,?,?,?,?,?,?,?,?)");
        lock.begin(sqlite3_transaction::LOCK_IMMEDIATE);
        int next_id = sqlite.executeint("select coalesce(max(id),-1)+1 from semantic_functions");
        Functions added;
        for (Functions::iterator fi=to_add.begin(); fi!=to_add.end(); ++fi) {
            SgAsmFunction *func = *fi;
            cmd2.bind(1, func->get_entry_va());
            cmd2.bind(2, stats[func].file_id);
            int func_id = cmd2.executeint();
            if (func_id<0) {
                func_ids[func] = next_id;
                cmd3.bind(1, next_id);
                cmd3.bind(2, func->get_entry_va());
                cmd3.bind(3, func->get_name());
                cmd3.bind(4, stats[func].file_id);
                const FuncStats &s = stats[func];
                cmd3.bind(5, s.isize);
                cmd3.bind(6, s.dsize);
                cmd3.bind(7, s.size);
                cmd3.bind(8, s.ninsns);
                cmd3.bind(9, s.digest);
                cmd3.executenonquery();
                added.insert(func);
                ++next_id;
            }
        }
        lock.commit();
        if (opt.verbosity>=LACONIC) {
            std::cerr <<"CloneDetection:   added " <<added.size() <<" function ID" <<(1==added.size()?"":"s")
                      <<" to the database\n";
        }

        // For each function we added to the database, also add the assembly listing. We assume that if the function already
        // exists in the database then its assembly listing also exists, but this might not be true if the process is
        // interruped here.
        sqlite3_command cmd4(sqlite,
                             "insert into semantic_instructions"
                             // 1        2     3         4            5         6            7
                             " (address, size, assembly, function_id, position, src_file_id, src_line)"
                             " values (?,?,?,?,?,?,?)");
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   computing address-to-source mapping\n";
        SgBinaryComposite *binfile = SageInterface::getEnclosingNode<SgBinaryComposite>(interp);
        assert(binfile!=NULL);
        BinaryAnalysis::DwarfLineMapper dlm(binfile);
        dlm.fix_holes();
        AsmUnparser unparser;
        //unparser.staticDataDisassembler.init(thread->get_process()->get_disassembler()); //FIXME
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   saving assembly code for each function\n";
        for (Functions::iterator ai=added.begin(); ai!=added.end(); ++ai) {
            SgAsmFunction *func = *ai;
            int func_id = func_ids[func];
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"  unparsing function " <<func_id;
            std::string function_lst = unparser.to_string(func);
            std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(func);

            // much faster when using transactions, but don't hold lock too long; i.e., lock inside this loop, not outside
            sqlite3_transaction lock2(sqlite, sqlite3_transaction::LOCK_IMMEDIATE, sqlite3_transaction::DEST_COMMIT);

            for (size_t i=0; i<insns.size(); ++i) {
                BinaryAnalysis::DwarfLineMapper::SrcInfo loc = dlm.addr2src(insns[i]->get_address());
                int src_file_id = loc.file_id < 0 ? -1 : get_file_id(Sg_File_Info::getFilenameFromID(loc.file_id));
                cmd4.bind(1, insns[i]->get_address());
                cmd4.bind(2, insns[i]->get_size());
                cmd4.bind(3, unparseInstructionWithAddress(insns[i]));
                cmd4.bind(4, func_id);
                cmd4.bind(5, i);
                cmd4.bind(6, src_file_id);
                cmd4.bind(7, loc.line_num);
                cmd4.executenonquery();
                if (opt.verbosity>=EFFUSIVE)
                    std::cerr <<'.';
            }
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"\n";
        }
    }

    // Rewrite a call graph by removing dynamic linked function thunks.  If we were using ROSE's experimental Graph2 stuff
    // instead of the Boost Graph Library's (BGL) adjacency_list, we could modify the graph in place efficiently.  But with
    // adjacency_list it's more efficient to create a whole new graph. [Robb P. Matzke 2013-05-16]
    CG rewrite_cg(CG &src) {
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection:   rewriting call graph to eliminate dynamic-linking thunks\n";
        CG dst;
        typedef std::map<CG_Vertex, CG_Vertex> VertexMap;
        VertexMap vmap; // mapping from src to dst vertex
        boost::graph_traits<CG>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end)=vertices(src); vi!=vi_end; ++vi) {
            CG_Vertex f1 = *vi;
            SgAsmFunction *func1 = get(boost::vertex_name, src, f1);
            if (vmap.find(f1)==vmap.end() && 0!=(SgAsmFunction::FUNC_IMPORT & func1->get_reason()) && 1==out_degree(f1, src)) {
                CG_Vertex f2 = target(*(out_edges(*vi, src).first), src); // the function that f1 calls
                SgAsmFunction *func2 = get(boost::vertex_name, src, f2);
                // We found thunk F1 that calls function F2. We want to remove F1 from the returned graph and replace all edges
                // (X,F1) with (X,F2). Therefore, create two mappings in the vmap: F2->F2' and F1->F2'. Be careful because we
                // might have already added vertex F2' to the returned graph.
                VertexMap::iterator f2i = vmap.find(f2);
                CG_Vertex f2prime;
                if (f2i==vmap.end()) {
                    f2prime = add_vertex(dst);
                    put(boost::vertex_name, dst, f2prime, func2);
                    vmap[f2] = f2prime;
                } else {
                    f2prime = f2i->second;
                }
                vmap[f1] = f2prime;
                if (opt.verbosity>=EFFUSIVE) {
                    FunctionIdMap::const_iterator id1=func_ids.find(func1), id2=func_ids.find(func2);
                    std::cerr <<"CloneDetection:     thunk " <<function_to_str(func1)
                              <<" delegated to " <<function_to_str(func2) <<"\n";
                }
            } else {
                CG_Vertex f1prime = add_vertex(dst);
                vmap[f1] = f1prime;
                put(boost::vertex_name, dst, f1prime, func1);
            }
        }

        // Now add the edges
        boost::graph_traits<CG>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end)=edges(src); ei!=ei_end; ++ei) {
            CG_Vertex f1 = source(*ei, src);
            CG_Vertex f2 = target(*ei, src);
            VertexMap::iterator f1i = vmap.find(f1);
            VertexMap::iterator f2i = vmap.find(f2);
            assert(f1i!=vmap.end());
            assert(f2i!=vmap.end());
            CG_Vertex f1prime = f1i->second;
            CG_Vertex f2prime = f2i->second;
            add_edge(f1prime, f2prime, dst);
        }
        return dst;
    }
    
    // Save the function call graph.
    void save_cg(SgAsmInterpretation *interp) {
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection: saving call graph...\n";

        // Generate the full call graph, then rewrite the call graph so calls to thunks for dynamically-linked functions look
        // like they're calls directly to the dynamically linked function.
        CG cg1 = BinaryAnalysis::FunctionCall().build_cg_from_ast<CG>(interp);
        CG cg2 = rewrite_cg(cg1);

        // Filter out vertices (and their incident edges) if the vertex is a function which is not part of the database (has no
        // function ID).
        struct CGFilter: BinaryAnalysis::FunctionCall::VertexFilter {
            const FunctionIdMap &func_ids;
            CGFilter(const FunctionIdMap &func_ids): func_ids(func_ids) {}
            virtual bool operator()(BinaryAnalysis::FunctionCall*, SgAsmFunction *vertex) /*override*/ {
                return func_ids.find(vertex)!=func_ids.end();
            }
        } vertex_filter(func_ids);
        BinaryAnalysis::FunctionCall copier;
        copier.set_vertex_filter(&vertex_filter);
        CG cg = copier.copy<CG>(cg2);

        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);

        // Delete from the database those call graph edges which have a source or target vertex that is one of the functions
        // we're analyzing.  This would be much easier if we could guarantee that all the functions we're analyzing come from
        // the same file.
        sqlite.executenonquery("drop table if exists tmp_cg");
        sqlite.executenonquery("create temporary table tmp_cg (func_id integer)");
        sqlite3_command cmd1(sqlite, "insert into tmp_cg (func_id) values (?)");
        for (FunctionIdMap::const_iterator fi=func_ids.begin(); fi!=func_ids.end(); ++fi) {
            cmd1.bind(1, fi->second);
            cmd1.executenonquery();
        }
        sqlite.executenonquery("delete from semantic_cg where exists"
                               " (select * from tmp_cg where caller = func_id or callee = func_id)");

        // Add the new call graph into to the database (that's still locked)
        sqlite3_command cmd2(sqlite, "insert into semantic_cg (caller, callee) values (?,?)");
        boost::graph_traits<CG>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end)=edges(cg); ei!=ei_end; ++ei) {
            CG_Vertex caller_v = source(*ei, cg);
            CG_Vertex callee_v = target(*ei, cg);
            SgAsmFunction *caller = get(boost::vertex_name, cg, caller_v);
            SgAsmFunction *callee = get(boost::vertex_name, cg, callee_v);
            int caller_id = func_ids.at(caller);
            int callee_id = func_ids.at(callee);
            cmd2.bind(1, caller_id);
            cmd2.bind(2, callee_id);
            cmd2.executenonquery();
        }

        lock.commit();
    };

    // Returns true if the file looks like text
    bool is_text_file(FILE *f) {
        if (!f)
            return false;
        char buf[4096];
        fpos_t pos;
        if (fgetpos(f, &pos)<0)
            return false;
        size_t nread = fread(buf, 1, sizeof buf, f);
        if (0==nread)
            return false; // empty files are binary
        int status __attribute__((unused)) = fsetpos(f, &pos);
        assert(status>=0);
        for (size_t i=0; i<nread; ++i)
            if (!isascii(buf[i]))
                return false;
        return true;
    }

    // Suck source code into the database.  For any file that can be read and which does not have lines already saved
    // in the semantic_sources table, read each line of the file and store them in semantic_sources.
    void save_files() {
        if (opt.verbosity>=LACONIC)
            std::cerr <<"CloneDetection: saving source code listings for each function...\n";
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE, sqlite3_transaction::DEST_COMMIT);
        sqlite3_command cmd1(sqlite,
                             "select files.id, files.name"
                             " from semantic_files as files"
                             " left join semantic_sources as sources"
                             " on files.id = sources.file_id"
                             " where sources.file_id is null");
        sqlite3_command cmd2(sqlite, "select count(*) from semantic_sources where file_id = ?");
        sqlite3_command cmd3(sqlite, "insert into semantic_sources (file_id, linenum, line) values (?,?,?)");

        sqlite3_reader c1 = cmd1.executereader();
        while (c1.read()) {
            int file_id = c1.getint(0);
            std::string file_name = c1.getstring(1);
            FILE *f = fopen(file_name.c_str(), "r");
            if (is_text_file(f)) {
                cmd2.bind(1, file_id);
                if (cmd2.executeint()<=0) {
                    if (opt.verbosity>=EFFUSIVE)
                        std::cerr <<"  saving source code for " <<file_name;
                    char *line = NULL;
                    size_t linesz=0, line_num=0;
                    ssize_t nread;
                    while ((nread=getline(&line, &linesz, f))>0) {
                        while (nread>0 && isspace(line[nread-1]))
                            line[--nread] = '\0';
                        cmd3.bind(1, file_id);
                        cmd3.bind(2, ++line_num);
                        cmd3.bind(3, line);
                        cmd3.executenonquery();
                        if (opt.verbosity>=EFFUSIVE)
                            std::cerr <<".";
                    }
                    if (line)
                        free(line);
                }
                if (opt.verbosity>=EFFUSIVE)
                    std::cerr <<"\n";
            }
            if (f)
                fclose(f);
        }
    }

    // Choose input values for fuzz testing.  The input values come from the database if they exist there, otherwise they are
    // chosen and written to the database. The set will consist of some number of non-pointers and pointers.  The pointers are
    // chosen to be randomly null or non-null, but the non-null values are not random.  Pointer values are only used if pointer
    // detection analysis is performed (i.e., the "--pointers" switch).  The non-pointers are chosen randomly or are a
    // permutation of a previous group's non-pointer values, depending on the "--permute-inputs" switch.
    InputGroup choose_inputs(size_t inputgroup_id) {
        InputGroup inputs;

        // Hold write lock while we check for input values and either read or create them. Otherwise some other
        // process might concurrently decide that input values don't exist and we'll have two processes trying to create
        // the input values that might differ.
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);

        sqlite3_command cmd1(sqlite, "select vtype, val from semantic_inputvalues where id = ? order by pos");
        cmd1.bind(1, inputgroup_id);
        sqlite3_reader cursor = cmd1.executereader();
        while (cursor.read()) {
            std::string vtype = cursor.getstring(0);
            assert(vtype.size()>=1);
            if ('P'==vtype[0]) {
                rose_addr_t va = cursor.getint64(1);
                inputs.add_pointer(va);
            } else {
                assert('N'==vtype[0]);
                size_t val = cursor.getint64(1);
                inputs.add_integer(val);
            }
        }

        // If we didn't get any input values, then create some and add them to the database.
        if (inputs.get_integers().size() + inputs.get_pointers().size() == 0) {
            sqlite3_command cmd3(sqlite, "insert into semantic_inputvalues (id, vtype, pos, val) values (?,?,?,?)");
            size_t np = factorial(opt.permute_inputs); // number of possible permutations
            size_t pn = inputgroup_id % np; // non-pointer permutation number; zero means no permutation, but random values

            std::vector<uint64_t> nonpointers;
            if (0==pn) {
                static unsigned integer_modulus = 256;  // arbitrary;
                for (size_t i=0; i<opt.nnonpointers; ++i)
                    nonpointers.push_back(rand() % integer_modulus);
            } else {
                sqlite3_command cmd4(sqlite, "select val from semantic_inputvalues where id=? and vtype='N' order by pos");
                size_t base_group_id = (inputgroup_id / np) * np; // input group that serves as the base
                cmd4.bind(1, base_group_id);
                sqlite3_reader c4 = cmd4.executereader();
                while (c4.read())
                    nonpointers.push_back(c4.getint(0));
                if (opt.permute_inputs>nonpointers.size())
                    nonpointers.resize(opt.permute_inputs, 0);
                permute(nonpointers, pn, opt.permute_inputs);
            }
            for (size_t i=0; i<nonpointers.size(); ++i) {
                cmd3.bind(1, inputgroup_id);
                cmd3.bind(2, "N");
                cmd3.bind(3, i);
                cmd3.bind(4, nonpointers[i]);
                cmd3.executenonquery();
            }
                
            static unsigned nonnull_denom = 3;      // probability of a non-null pointer is 1/N
            for (size_t i=0; i<opt.npointers; ++i) {
                uint64_t val = rand()%nonnull_denom ? 0 : allocate_page();
                inputs.add_pointer(val);
                cmd3.bind(1, inputgroup_id);
                cmd3.bind(2, "P");
                cmd3.bind(3, i);
                cmd3.bind(4, val);
                cmd3.executenonquery();
            }
        }

        lock.commit();
        return inputs;
    }

    // Save output values into the database. Each fuzz test generates some number of output_values, all of which are collected
    // into a single container that we call an output group.  In order to cut down on the number of output groups in the
    // database, this function will reuse output groups that already exist.
    size_t save_outputs(OutputGroup &outputs, const VaId &func_va2id, const IdVa &func_id2va) {
        // We need a write lock for the duration, otherwise some other process might determine that an output set
        // doesn't exist and then try to create the same one we're about to create.
        sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE);

        sqlite3_command cmd1(sqlite, "select coalesce(max(id),-1)+1 from semantic_outputvalues"); // next group ID
        sqlite3_command cmd3(sqlite, "insert into semantic_outputvalues (id, pos, val, vtype) values (?,?,?,?)");

        // Load output groups from the database if they've changed since last time we loaded.  If we're the only process
        // creating output groups then we'll only load the output groups when we first start.  We're assuming that no process
        // is deleting output groups and that groups are numbered consecutively starting at zero.
        int next_id = cmd1.executeint();
        if ((int)output_groups.size() != next_id) {
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: loading output groups from the database\n";
            load_output_groups(sqlite, &func_id2va, output_groups);
        }

        // Remove non-analyzed functions from the output callee list
        for (size_t i=0; i<outputs.callees_va.size(); ++i) {
            if (func_va2id.find(outputs.callees_va[i])==func_va2id.end())
                outputs.callees_va[i] = 0;
        }
        outputs.callees_va.erase(std::remove(outputs.callees_va.begin(), outputs.callees_va.end(), 0), outputs.callees_va.end());

        // Find an existing output group that matches the given values
        for (size_t i=0; i<output_groups.size(); ++i) {
            if (output_groups[i] == outputs)
                return i;
        }

        // Save this output group
        output_groups.resize(next_id+1);
        output_groups[next_id] = outputs;
        for (size_t i=0; i<outputs.values.size(); ++i) {
            cmd3.bind(1, next_id);
            cmd3.bind(2, i);
            cmd3.bind(3, outputs.values[i]);
            cmd3.bind(4, "V");
            cmd3.executenonquery();
        }
        for (size_t i=0; i<outputs.callees_va.size(); ++i) {
            VaId::const_iterator found = func_va2id.find(outputs.callees_va[i]); // have entry_va, need to save function ID
            assert(found!=func_va2id.end());
            cmd3.bind(1, next_id);
            cmd3.bind(2, i);
            cmd3.bind(3, found->second);
            cmd3.bind(4, "C");
            cmd3.executenonquery();
        }
        for (size_t i=0; i<outputs.syscalls.size(); ++i) {
            cmd3.bind(1, next_id);
            cmd3.bind(2, i);
            cmd3.bind(3, outputs.syscalls[i]);
            cmd3.bind(4, "S");
            cmd3.executenonquery();
        }
        if (outputs.fault!=AnalysisFault::NONE) {
            cmd3.bind(1, next_id);
            cmd3.bind(2, 0);
            cmd3.bind(3, outputs.fault);
            cmd3.bind(4, "F");
            cmd3.executenonquery();
        }

        lock.commit();
        return next_id;
    }

    // Return the name of a file containing the specified function.
    std::string filename_for_function(SgAsmFunction *function, bool basename=true) {
        std::string retval;
        SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(function);
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
            size_t nmatch;
            (*hi)->get_section_by_va(function->get_entry_va(), false, &nmatch);
            if (nmatch>0) {
                SgAsmGenericFile *file = SageInterface::getEnclosingNode<SgAsmGenericFile>(*hi);
                if (file!=NULL && !file->get_name().empty()) {
                    retval = file->get_name();
                    break;
                }
            }
        }
        if (basename) {
            size_t slash = retval.rfind('/');
            if (slash!=std::string::npos)
                retval = retval.substr(slash+1);
        }
        return retval;
    }

    // Display string for function.  Includes function address, and in angle brackets, the database function ID if known, the
    // function name if known, and file name if known.
    std::string function_to_str(SgAsmFunction *function) {
        std::ostringstream ss;
        FunctionIdMap::const_iterator idi = func_ids.find(function);
        std::string func_name = function->get_name();
        std::string file_name = filename_for_function(function);

        ss <<StringUtility::addrToString(function->get_entry_va());

        bool printed = false;
        if (!func_name.empty()) {
            ss <<" <\"" <<func_name <<"\"";
            printed = true;
        }
        if (idi!=func_ids.end()) {
            ss <<(printed?" ":" <") <<"id=" <<idi->second;
            printed = true;
        }
        if (!file_name.empty()) {
            ss <<(printed?" ":" <") <<"in " <<file_name;
            printed = true;
        }
        if (printed)
            ss <<">";

        return ss.str();
    }
    
    // Analyze a single function by running it with the specified inputs and collecting its outputs. */
    OutputGroup fuzz_test(SgAsmInterpretation *interp, SgAsmFunction *function, InputGroup &inputs,
                          const InstructionProvidor &insns, const PointerDetector *pointers/*=NULL*/) {
        AnalysisFault::Fault fault = AnalysisFault::NONE;
        policy.reset(interp, function, &inputs, &insns, pointers);
        try {
            while (1) {
                if (!policy.state.registers.ip.is_known()) {
                    if (opt.verbosity>=EFFUSIVE)
                        std::cerr <<"CloneDetection: EIP value is not concrete\n";
                    fault = AnalysisFault::SEMANTICS;
                    break;
                }

                rose_addr_t insn_va = policy.state.registers.ip.known_value();
                if (policy.FUNC_RET_ADDR==insn_va) {
                    if (opt.verbosity>=EFFUSIVE)
                        std::cerr <<"CloneDetection: function returned\n";
                    fault = AnalysisFault::NONE;
                    break;
                }
                
                if (SgAsmx86Instruction *insn = isSgAsmx86Instruction(insns.get_instruction(insn_va))) {
                    semantics.processInstruction(insn);
                } else {
                    if (opt.verbosity>=EFFUSIVE)
                        std::cerr <<"CloneDetection: no instruction at " <<StringUtility::addrToString(insn_va) <<"\n";
                    fault = AnalysisFault::DISASSEMBLY;
                    break;
                }
            }
        } catch (const FaultException &e) {
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: analysis terminated by " <<AnalysisFault::fault_name(e.fault) <<"\n";
            fault = e.fault;
        } catch (const Exception &e) {
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: analysis terminated by semantic exception: " <<e.mesg <<"\n";
            fault = AnalysisFault::SEMANTICS;
        } catch (const BaseSemantics::Policy::Exception &e) {
            // Some exception in the policy, such as division by zero.
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: analysis terminated by FAULT_SEMANTICS: " <<e.mesg <<"\n";
            fault = AnalysisFault::SEMANTICS;
        } catch (const Semantics::Exception &e) { // X86InstructionSemantics<...>::Exception
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: analysis terminated by X86InstructionSemantics exception: " <<e.mesg <<"\n";
            fault = AnalysisFault::SEMANTICS;
        } catch (const SMTSolver::Exception &e) {
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: analysis terminated by SMT solver exception: " <<e.mesg <<"\n";
            fault = AnalysisFault::SMTSOLVER;
        }
        
        // Gather the function's outputs before restoring machine state.
        OutputGroup outputs = policy.get_outputs();
        outputs.fault = fault;
        return outputs;
    }

    // Detect functions that are semantically similar by running multiple iterations of partition_functions().
    void analyze(SgAsmInterpretation *interp) {
        std::cerr <<"CloneDetection: database=" <<opt.dbname
                  <<" fuzz=@" <<opt.firstfuzz <<"+" <<opt.nfuzz
                  <<" npointers=" <<opt.npointers <<" nnonpointers=" <<opt.nnonpointers
                  <<" max_insns=" <<opt.max_insns <<"\n";
        Functions functions = find_functions(interp, opt.functions);
        save_functions(interp, functions); // must be first because it initializes our func_ids data member
        save_files();
        save_cg(interp);
        InstructionProvidor insns(interp);

        // Mapping from function ID to function entry va and vice versa
        IdVa func_id2va;
        VaId func_va2id;
        for (FunctionIdMap::const_iterator fi=func_ids.begin(); fi!=func_ids.end(); ++fi) {
            func_id2va[fi->second] = fi->first->get_entry_va();
            func_va2id[fi->first->get_entry_va()] = fi->second;
        }

        typedef std::map<SgAsmFunction*, PointerDetector*> PointerDetectors;
        PointerDetectors pointers;
        sqlite3_command cmd1(sqlite, "insert into semantic_fio "
                             //1        2              3                  4                     5
                             "(func_id, inputgroup_id, pointers_consumed, nonpointers_consumed, instructions_executed,"
                             //6                   7                      8             9
                             " actual_outputgroup, effective_outputgroup, elapsed_time, cpu_time)"
                             " values (?,?,?,?,?,?,?,?,?)");
        sqlite3_command cmd2(sqlite, "select count(*) from semantic_fio where func_id=? and inputgroup_id=? limit 1");
        Progress progress(opt.nfuzz * functions.size());
        progress.force_output(opt.show_progress);
        for (size_t fuzz_number=opt.firstfuzz; fuzz_number<opt.firstfuzz+opt.nfuzz; ++fuzz_number) {
            InputGroup inputs = choose_inputs(fuzz_number);
            if (opt.verbosity>=LACONIC) {
                std::cerr <<"CloneDetection: " <<std::string(80, '#') <<"\n"
                          <<"CloneDetection: fuzz testing " <<functions.size() <<" function" <<(1==functions.size()?"":"s")
                          <<" with inputgroup " <<fuzz_number <<"\n"
                          <<"CloneDetection: using these input values:\n" <<inputs.toString();
            }
            for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
                if (opt.show_progress || opt.verbosity==SILENT)
                    progress.show();
                SgAsmFunction *func = *fi;
                inputs.reset();

                // Did we already test this function in a previous incarnation?
                cmd2.bind(1, func_ids[func]);
                cmd2.bind(2, fuzz_number);
                if (cmd2.executeint()>0) {
                    if (opt.verbosity>=LACONIC)
                        std::cerr <<"CloneDetection: " <<function_to_str(func) <<" fuzz test #" <<fuzz_number <<" SKIPPED\n";
                    continue;
                }

                // Get the results of pointer analysis.  We could have done this before any fuzz testing started, but by doing
                // it here we only need to do it for functions that are actually tested.
                PointerDetectors::iterator ip = pointers.find(func);
                if (ip==pointers.end())
                    ip = pointers.insert(std::make_pair(func, detect_pointers(func))).first;
                assert(ip!=pointers.end());

                // Run the test
                timeval start_time, stop_time;
                clock_t start_ticks = clock();
                gettimeofday(&start_time, NULL);
                if (opt.verbosity>=LACONIC)
                    std::cerr <<"CloneDetection: " <<function_to_str(func) <<" fuzz test #" <<fuzz_number <<"\n";
                OutputGroup outputs = fuzz_test(interp, func, inputs, insns, ip->second);
                gettimeofday(&stop_time, NULL);
                clock_t stop_ticks = clock();
                double elapsed_time = (stop_time.tv_sec - start_time.tv_sec) +
                                      ((double)stop_time.tv_usec - start_time.tv_usec) * 1e-6;

                // If clock_t is a 32-bit unsigned value then it will wrap around once every ~71.58 minutes. We expect clone
                // detection to take longer than that, so we need to be careful.
                double cpu_time = start_ticks <= stop_ticks ?
                                  (double)(stop_ticks-start_ticks) / CLOCKS_PER_SEC :
                                  (pow(2.0, 8*sizeof(clock_t)) - (start_ticks-stop_ticks)) / CLOCKS_PER_SEC;

                // Save the results. Some other process might have tested this function concurrently, in which case we'll
                // defer to the other process' results.
                if (opt.verbosity>=EFFUSIVE) {
                    std::cerr <<"CloneDetection: " <<function_to_str(func) <<" fuzz test #" <<fuzz_number <<" output values:\n";
                    outputs.print(std::cerr, "", "CloneDetection:  ");
                }
                size_t outputgroup_id = save_outputs(outputs, func_va2id, func_id2va);
                sqlite3_transaction lock(sqlite, sqlite3_transaction::LOCK_IMMEDIATE, sqlite3_transaction::DEST_COMMIT);
                if (cmd2.executeint()>0) {
                    std::cerr <<"CloneDetection: " <<function_to_str(func) <<" fuzz test #" <<fuzz_number <<" ALREADY PRESENT\n";
                } else {
                    cmd1.bind(1, func_ids[func]);
                    cmd1.bind(2, fuzz_number);
                    cmd1.bind(3, inputs.pointers_consumed());
                    cmd1.bind(4, inputs.integers_consumed());
                    cmd1.bind(5, policy.ninsns);
                    cmd1.bind(6, outputgroup_id);
                    cmd1.bind(7, outputgroup_id);
                    cmd1.bind(8, elapsed_time);
                    cmd1.bind(9, cpu_time);
                    cmd1.executenonquery();
                }
            }
        }
        progress.clear();
        std::cerr <<"CloneDetection: final results stored in " <<opt.dbname <<"\n";
    }
};

} // namespace

/******************************************************************************************************************************/
int
main(int argc, char *argv[], char *envp[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

#ifdef ROSE_HAVE_GCRYPT_H
    if (!gcry_check_version(GCRYPT_VERSION)) {
        std::cerr <<argv0 <<": libgcrypt version mismatch\n";
        exit(1);
    }
#endif
    
    // Parse command-line switches that we recognize.
    Switches opt;
    for (int i=1; i<argc && '-'==argv[i][0]; /*void*/) {
        bool consume = false;
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h") || !strcmp(argv[i], "-?")) {
            usage(0);
        } else if (!strncmp(argv[i], "--database=", 11)) {
            opt.dbname = argv[i]+11;
            consume = true;
        } else if (!strcmp(argv[i], "--follow-calls")) {
            opt.follow_calls = consume = true;
        } else if (!strncmp(argv[i], "--function=", 11)) {
            char *rest;
            rose_addr_t va = strtoull(argv[i]+11, &rest, 0);
            if (rest==argv[i]+11 || *rest) {
                std::cerr <<argv0 <<": --function should specify a function entry address\n";
                exit(1);
            }
            opt.functions.insert(va);
        } else if (!strcmp(argv[i], "--link")) {
            opt.link = consume = true;
        } else if (!strncmp(argv[i], "--min-function-size=", 20)) {
            opt.min_funcsz = strtoul(argv[i]+20, NULL, 0);
            consume = true;
        } else if (!strncmp(argv[i], "--max-insns=", 12)) {
            opt.max_insns = strtoul(argv[i]+12, NULL, 0);
            consume = true;
        } else if (!strncmp(argv[i], "--nfuzz=", 8)) {
            char *rest;
            opt.nfuzz = strtoul(argv[i]+8, &rest, NULL);
            if (','==*rest)
                opt.firstfuzz = strtoul(rest+1, NULL, 0);
            consume = true;
        } else if (!strncmp(argv[i], "--ninputs=", 10)) {
            char *rest;
            opt.npointers = opt.nnonpointers = strtoul(argv[i]+10, &rest, 0);
            if (','==*rest)
                opt.nnonpointers = strtoul(rest+1, NULL, 0);
            consume = true;
        } else if (!strncmp(argv[i], "--permute-inputs=", 17)) {
            opt.permute_inputs = strtoul(argv[i]+17, NULL, 0);
            consume = true;
        } else if (!strcmp(argv[i], "--pointers")) {
            opt.pointer_analysis = true;
            consume = true;
        } else if (!strcmp(argv[i], "--no-pointers")) {
            opt.pointer_analysis = false;
            consume = true;
        } else if (!strcmp(argv[i], "--progress")) {
            opt.show_progress = true;
            consume = true;
        } else if (!strcmp(argv[i], "--verbose")) {
            opt.verbosity = EFFUSIVE;
            consume = true;
        } else if (!strncmp(argv[i], "--verbosity=", 12)) {
            size_t n = strtoul(argv[i]+12, NULL, 0);
            if (n>=EFFUSIVE || !strcmp(argv[i]+12, "effusive")) {
                opt.verbosity = EFFUSIVE;
            } else if (n>=LACONIC || !strcmp(argv[i]+12, "laconic")) {
                opt.verbosity = LACONIC;
            } else {
                opt.verbosity = SILENT;
            }
            consume = true;
        }
        
        if (consume) {
            memmove(argv+i, argv+i+1, (argc-i)*sizeof(argv[0])); // include terminating NULL at argv[argc]
            --argc;
        } else {
            ++i;
        }
    }

    // Parse the binary container (ELF, PE, etc) but do not disassemble yet.
    if (opt.verbosity >= LACONIC)
        std::cerr <<"CloneDetection: Parsing binary specimen...\n"
                  <<"CloneDetection:   parsing container\n";
    int argc2 = argc+1;
    char **argv2 = new char*[argc+2];
    argv2[0] = argv[0];
    argv2[1] = strdup("-rose:read_executable_file_format_only");
    for (int i=1; i<argc; ++i)
        argv2[i+1] = argv[i];
    argv[argc2] = NULL;
    SgProject *project = frontend(argc2, argv2);

    // Find the primary interpretation (e.g., the PE, not DOS, interpretation in PE files).
    if (opt.verbosity >= LACONIC)
        std::cerr <<"CloneDetection:   finding primary interpretation\n";
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    if (interps.empty())
        die("no binary specimen given");
    SgAsmInterpretation *interp = interps.back();

    // Get the shared libraries, map them, and apply relocation fixups. We have to do the mapping step even if we're not
    // linking with shared libraries, because that's what gets the various file sections lined up in memory for the
    // disassembler.
    if (opt.link && opt.verbosity >= LACONIC)
        std::cerr <<"CloneDetection:   loading shared libraries\n";
    if (BinaryLoader *loader = BinaryLoader::lookup(interp)) {
        try {
            loader = loader->clone(); // so our settings are private
            if (opt.link) {
                loader->add_directory("/lib32");
                loader->add_directory("/usr/lib32");
                loader->add_directory("/lib");
                loader->add_directory("/usr/lib");
                if (char *ld_library_path = getenv("LD_LIBRARY_PATH")) {
                    std::vector<std::string> paths;
                    StringUtility::splitStringIntoStrings(ld_library_path, ':', paths/*out*/);
                    loader->add_directories(paths);
                }
                loader->link(interp);
            }
            loader->remap(interp);
            BinaryLoader::FixupErrors fixup_errors;
            loader->fixup(interp, &fixup_errors);
            if (!fixup_errors.empty()) {
                std::cerr <<argv0 <<":     warning: " <<fixup_errors.size()
                          <<" relocation fixup error" <<(1==fixup_errors.size()?"":"s") <<" encountered\n";
            }
            if (SageInterface::querySubTree<SgAsmInterpretation>(project).size() != interps.size())
                std::cerr <<argv0 <<": warning: new interpretations created by the linker; mixed 32- and 64-bit libraries?\n";
        } catch (const BinaryLoader::Exception &e) {
            std::cerr <<argv0 <<": BinaryLoader error: " <<e.mesg <<"\n";
            exit(1);
        }
    } else {
        die("no suitable loader/linker found");
    }

    // Figure out what to disassemble.  If we did dynamic linking then we can mark the .got and .got.plt sections as read-only
    // because we've already filled them in with the addresses of the dynamically linked entities.  This will allow the
    // disassembler to know the successors for the indirect JMP instruction in the .plt section (the dynamic function thunks).
    assert(interp->get_map()!=NULL);
    MemoryMap map = *interp->get_map();
    if (opt.link) {
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
            SgAsmGenericSectionPtrList sections = (*hi)->get_sections_by_name(".got.plt");      // ELF
            SgAsmGenericSectionPtrList s2 = (*hi)->get_sections_by_name(".got");                // ELF
            SgAsmGenericSectionPtrList s3 = (*hi)->get_sections_by_name(".import");             // PE
            sections.insert(sections.end(), s2.begin(), s2.end());
            sections.insert(sections.end(), s3.begin(), s3.end());
            for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); si!=sections.end(); ++si) {
                if ((*si)->is_mapped()) {
                    Extent mapped_va((*si)->get_mapped_actual_va(), (*si)->get_mapped_size());
                    map.mprotect(mapped_va, MemoryMap::MM_PROT_READ, true/*relax*/);
                }
            }
        }
    }

    // Disassemble the executable
    if (opt.verbosity >= LACONIC)
        std::cerr <<"CloneDetection:   disassembling and partitioning\n";
    if (Disassembler *disassembler = Disassembler::lookup(interp)) {
        disassembler = disassembler->clone(); // so our settings are private
#if 1 // FIXME [Robb P. Matzke 2013-05-14]
        // We need to handle -rose:disassembler_search, -rose:partitioner_search, and -rose:partitioner_config
        // command-line switches.
#endif
        if (opt.verbosity >= EFFUSIVE) {
            std::cerr <<"CloneDetection:     memory map for disassembly:\n";
            map.print(std::cerr, "CloneDetection:       ");
        }
        Partitioner *partitioner = new Partitioner();
        SgAsmBlock *gblk = partitioner->partition(interp, disassembler, &map);
        interp->set_global_block(gblk);
        gblk->set_parent(interp);
    } else {
        die("unable to disassemble this specimen");
    }

    // Save listings and dumps to aid debugging
    if (opt.verbosity>=LACONIC)
        std::cerr <<"CloneDetection: saving dumps and listings to text files\n";
    backend(project);

    // Run the clone detection analysis
    CloneDetection::Analysis analysis(opt);
    analysis.analyze(interp);
    return 0;
}
