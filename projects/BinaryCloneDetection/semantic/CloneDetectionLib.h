#ifndef RSIM_CloneDetection_H
#define RSIM_CloneDetection_H

#include "Disassembler.h"
#include "PartialSymbolicSemantics.h"
#include "x86InstructionSemantics.h"
#include "BinaryPointerDetection.h"
#include "SqlDatabase.h"
#include "LinearCongruentialGenerator.h"
#include "Combinatorics.h"
#include "Map.h"

#include "compute_signature_vector.h"

#include <stdint.h>
#include <vector>
#include <ostream>
#include <map>

namespace CloneDetection {

extern const char *schema; /**< Contents of Schema.sql file, initialized in CloneDetectionSchema.C */

using namespace rose::BinaryAnalysis::InstructionSemantics;

typedef std::set<SgAsmFunction*> Functions;
typedef std::map<SgAsmFunction*, int> FunctionIdMap;
typedef std::map<int, SgAsmFunction*> IdFunctionMap;
typedef std::map<rose_addr_t, int> AddressIdMap;
typedef Map<std::string, rose_addr_t> NameAddress;
typedef rose::BinaryAnalysis::FunctionCall::Graph CG;
typedef boost::graph_traits<CG>::vertex_descriptor CG_Vertex;
enum Verbosity { SILENT, LACONIC, EFFUSIVE };
enum FollowCalls { CALL_NONE, CALL_ALL, CALL_BUILTIN };
extern const rose_addr_t GOTPLT_VALUE; /**< Address of all dynamic functions that are not loaded. */



/*******************************************************************************************************************************
 *                                      Progress bars
 *******************************************************************************************************************************/

/** Show progress bar indicator on standard error. The progress bar's constructor should indicate the total number of times
 *  that the show() method is expected to be called.  If the standard error stream is a terminal (or force_output is set) then
 *  a progress bar is emitted at most once per RTP_INTERVAL seconds (default 1).  The cursor is positioned at the beginning
 *  of the progress bar's line each time the progress bar is printed. The destructor erases the progress bar. */
class Progress {
protected:
    size_t cur, total;
    time_t last_report;
    bool is_terminal, force, had_output;
    std::string mesg;
    enum { WIDTH=100, RPT_INTERVAL=1 };
    void init();
public:
    Progress(size_t total): cur(0), total(total), last_report(0), is_terminal(false), force(false), had_output(false) { init(); }
    ~Progress() { clear(); }

    /** Force the progress bar to be emitted even if standard error is not a terminal. */
    void force_output(bool b) { force = b; }

    /** Increment the progress. The bar is updated only if it's been at least RPT_INTERVAL seconds since the previous update
     *  or if @p update_now is true.
     *  @{ */
    void increment(bool update_now=false);
    Progress& operator++() { increment(); return *this; }
    Progress operator++(int) { Progress retval=*this; increment(); return retval; }
    /** @} */

    /** Immediately erase the progress bar from the screen by emitting white space. */
    void clear();

    /** Reset the progress counter back to zero. */
    void reset(size_t current=0, size_t total=(size_t)(-1));

    /** Show a message. The progress bar is updated only if it's been at least RPT_INTERVAL seconds since the previous
     *  update or if @p update_now is true.*/
    void message(const std::string&, bool update_now=true);

    /** Update the progress bar without incrementing. The progress bar is updated only if it's been at least RPT_INTERVAL
     *  seconds since the previous update or if @p update_now is true. */
    void update(bool update_now=true);

    /** Returns the current rendering of the progress line. */
    std::string line() const;

    /** Returns the current position. */
    size_t current() const { return cur; }
};


/*******************************************************************************************************************************
 *                                      Analysis faults
 *******************************************************************************************************************************/

/** Special output values for when something goes wrong. */
class AnalysisFault {
public:
    enum Fault {
        NONE        = 0,
        DISASSEMBLY = 911000001,     /**< Disassembly failed possibly due to bad address. */
        INSN_LIMIT  = 911000002,     /**< Maximum number of instructions executed. */
        HALT        = 911000003,     /**< x86 HLT instruction executed or "abort@plt" called. */
        INTERRUPT   = 911000004,     /**< x86 INT instruction executed. */
        SEMANTICS   = 911000005,     /**< Some fatal problem with instruction semantics, such as a not-handled instruction. */
        SMTSOLVER   = 911000006,     /**< Some fault in the SMT solver. */
        INPUT_LIMIT = 911000007,     /**< Too many input values consumed. */
        BAD_STACK   = 911000008,     /**< ESP is above the starting point. */
        // don't forget to fix 00-create-schema.C and the functions below
    };

    /** Return the short name of a fault ID. */
    static const char *fault_name(Fault fault) {
        switch (fault) {
            case NONE:          return "success";
            case DISASSEMBLY:   return "disassembly";
            case INSN_LIMIT:    return "insn limit";
            case HALT:          return "halt";
            case INTERRUPT:     return "interrupt";
            case SEMANTICS:     return "semantics";
            case SMTSOLVER:     return "SMT solver";
            case INPUT_LIMIT:   return "input limit";
            case BAD_STACK:     return "bad stack ptr";
            default:
                assert(!"fault not handled");
                abort();
        }
    }

    /** Return the description for a fault ID. */
    static const char *fault_desc(Fault fault) {
        switch (fault) {
            case NONE:          return "success";
            case DISASSEMBLY:   return "disassembly failed or bad instruction address";
            case INSN_LIMIT:    return "simulation instruction limit reached";
            case HALT:          return "x86 HLT instruction executed";
            case INTERRUPT:     return "interrupt or x86 INT instruction executed";
            case SEMANTICS:     return "instruction semantics error";
            case SMTSOLVER:     return "SMT solver error";
            case INPUT_LIMIT:   return "over-consumption of input values";
            case BAD_STACK:     return "stack pointer is above analysis starting point";
            default:
                assert(!"fault not handled");
                abort();
        }
    }
};


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

std::ostream& operator<<(std::ostream&, const Exception&);



/*******************************************************************************************************************************
 *                                      Large Table Output
 *******************************************************************************************************************************/

/** Base class for generating data for large tables.  The data is first written to a temporary file so as not to consume large
 *  amounts of core memory. The flush() operation then bulk loads the data into the database table and truncates the temporary
 *  file.
 *
 *  The Row class must have a serialize() method that writes the row's data to a specified std::ostream argument in a form
 *  suitable for bulk loading.  The format should be comma-separated values on a single line. Strings need to be escaped in the
 *  usual SQL manner.
 *
 *  The destructor does not save accumulated events.  The flush() method should be called first otherwise events accumulated
 *  since the last flush() will be lost. This behavior is consistent with SqlDatabase, where an explicit commit is necessary
 *  before destroying a transaction. */
template<class Row>
class WriteOnlyTable {
public:
    /** Construct a new, empty table in memory.  The table may exist in the database, but it is not loaded into memory. */
    WriteOnlyTable(const std::string &tablename): tablename(tablename), nrows(0) {}

    ~WriteOnlyTable() { close_backing(); }

    /** Discard data that is pending to be loaded into the database. */
    void clear() { close_backing(); }

    /** Returns true unless there is data pending to be loaded into the database. */
    bool empty() const { return size()==0; }

    /** Returns the number of rows pending to be loaded into the database. */
    size_t size() const { return nrows; }

    /** Accumulate a new row and mark is a pending to be loaded into the database by the next flush() operation. */
    void insert(const Row &row) {
        open_backing();
        row.serialize(f);
        ++nrows;
        if (f.fail())
            throw Exception(std::string("CloneDetection::WriteOnlyTable::insert: write error for table: "+tablename));
    }

    /** Copy pending data into the database. */
    void flush(const SqlDatabase::TransactionPtr &tx) {
        if (!empty()) {
            f.close();
            std::ifstream fin(filename);
            tx->bulk_load(tablename, fin);
            fin.close();
            unlink(filename);
            filename[0] = '\0';
            nrows = 0;
        }
    }

private:
    void open_backing() {
        if (!f.is_open()) {
            strcpy(filename, "/tmp/roseXXXXXX");
            int fd = mkstemp(filename);
            assert(-1!=fd);
            close(fd);
            f.open(filename);
        }
    }

    void close_backing() {
        if (f.is_open()) {
            f.close();
            unlink(filename);
            filename[0] = '\0';
            nrows = 0;
        }
    }


private:
    std::string tablename;
    size_t nrows;
    char filename[64];
    std::ofstream f;
};


/*******************************************************************************************************************************
 *                                      Test trace events
 *******************************************************************************************************************************/

/** Types of events that are emitted to the database during tracing. */
enum TracerEvent {
    // note: don't change the numbering, because these numbers appear in databases.
    // note: update the analysis to emit the event
    // note: update the --trace switch in 25-run-tests
    // note: update 00-create-schema to insert the new event into the semantic_fio_events table
    // note: update 90-list-function so the event is shown in the listing
    EV_NONE                 = 0x00000000,   /**< No event. */
    EV_REACHED              = 0x00000001,   /**< Basic block executed. */
    EV_BRANCHED             = 0x00000002,   /**< Branch taken. */
    EV_FAULT                = 0x00000004,   /**< Test failed; minor number is the fault ID */
    EV_CONSUME_INPUT        = 0x00000008,   /**< Consumed an input. Minor number is the queue. */
    EV_MEM_WRITE            = 0x00000010,   /**< Data written to non-stack memory. Value is address written. */
    EV_RETURNED             = 0x00000020,   /**< Forced immediate return of inner-most function. */
    // Masks
    CONTROL_FLOW            = 0x00000023,   /**< Control flow events. */
    ALL_EVENTS              = 0xffffffff    /**< All possible events. */
};

// See Schema.sql for semantic_fio_event
struct TracerRow {
    int func_id, igroup_id, minr;
    size_t pos;
    rose_addr_t addr;
    TracerEvent event;
    uint64_t value;

    TracerRow(int func_id, int igroup_id, size_t pos, rose_addr_t addr, TracerEvent event, int minr, uint64_t value)
        : func_id(func_id), igroup_id(igroup_id), minr(minr), pos(pos), addr(addr), event(event), value(value) {}
    void serialize(std::ostream &output) const { // output order must match the schema
        output <<func_id <<"," <<igroup_id <<"," <<pos <<"," <<addr <<"," <<event <<"," <<minr <<"," <<value <<"\n";
    }
};

class Tracer: public WriteOnlyTable<TracerRow> {
public:

    /** Construct a tracer not yet associated with any function or input group. The reset() method must be called before
     *  any events can be emitted. */
    Tracer(): WriteOnlyTable<TracerRow>("semantic_fio_trace"), func_id(-1), igroup_id(-1), event_mask(ALL_EVENTS), pos(0) {}

    /** Associate a tracer with a particular function and input group.  Accumulated events are not lost. */
    void current_test(int func_id, int igroup_id, unsigned events=ALL_EVENTS, size_t pos=0);

    /** Add an event to the stream.  The event is not actually inserted into the database until flush() is called. */
    void emit(rose_addr_t addr, TracerEvent event, uint64_t value=0, int minor=0);

private:
    int func_id, igroup_id;
    unsigned event_mask;
    size_t pos;
};



/*******************************************************************************************************************************
 *                                      File names table
 *******************************************************************************************************************************/

class FilesTable {
public:
    struct Row {
        Row(): in_db(false), id(-1) {}
        Row(int id, const std::string &name, const std::string &digest, const std::string &ast_digest, bool in_db)
            : in_db(in_db), id(id), name(name), digest(digest), ast_digest(ast_digest) {}
        bool in_db;
        int id;
        std::string name;
        std::string digest;             // SHA1 of this file if it is's stored in the semantic_binaries table
        std::string ast_digest;         // SHA1 hash of binary AST if it's stored in the semantic_binaries table
    };
    typedef std::map<int, Row> Rows;
    Rows rows;
    typedef std::map<std::string, int> NameIdx;
    NameIdx name_idx;
    int next_id;

    /** Constructor loads file information from the database. */
    FilesTable(const SqlDatabase::TransactionPtr &tx): next_id(0) { load(tx); }

    /** Reload information from the database. */
    void load(const SqlDatabase::TransactionPtr &tx);

    /** Save all unsaved files to the database. */
    void save(const SqlDatabase::TransactionPtr &tx);

    /** Add or remove an AST for this file. Returns the SHA1 digest for the AST, which also serves as the key in the
     *  semantic_binaries table. */
    std::string save_ast(const SqlDatabase::TransactionPtr&, int64_t cmd_id, int file_id, SgProject*);

    /** Load an AST from the database if it is saved there. Returns the SgProject or null. */
    SgProject *load_ast(const SqlDatabase::TransactionPtr&, int file_id);

    /** Add (or update) file content to the database. Returns the SHA1 digest for the file. */
    std::string add_content(const SqlDatabase::TransactionPtr&, int64_t cmd_id, int file_id);

    /** Print the files table. */
    void print(std::ostream&) const;

    void clear() {
        rows.clear();
        name_idx.clear();
        next_id = 0;
    }

    bool exists(const std::string &name) const;
    int insert(const std::string &name);
    int id(const std::string &name) const;
    std::string name(int id) const;

};

/*******************************************************************************************************************************
 *                                      Instruction coverage analysis
 *******************************************************************************************************************************/

    /** Information about an executed address. */
struct InsnCoverageRow {
    int func_id;                                /**< Function ID for test that was executed. */
    int igroup_id;                              /**< Input group ID for test that was executed. */
    rose_addr_t address;                        /**< Instruction address. */
    size_t pos;                                 /**< Sequence number for when this address was first executed by this test. */
    size_t nhits;                               /**< Number of times this address was executed. */
    size_t nhits_saved;                         /**< Part of "nhits" already flushed to the database (not a database column). */

    InsnCoverageRow(int func_id, int igroup_id, rose_addr_t address, size_t pos, size_t nhits)
        : func_id(func_id), igroup_id(igroup_id), address(address), pos(pos), nhits(nhits), nhits_saved(0) {}

    void serialize(std::ostream &stream) const { // output order must match the schema
        stream <<func_id <<"," <<igroup_id <<"," <<address <<"," <<pos <<"," <<nhits <<"\n";
    }
};

/** Instruction coverage information for a single test. A test is identified by function ID and input group ID. */
class InsnCoverage: public WriteOnlyTable<InsnCoverageRow> {
public:
    InsnCoverage()
        : WriteOnlyTable<InsnCoverageRow>("semantic_fio_coverage"), func_id(-1), igroup_id(-1) {}

    /** Set the current test information. */
    void current_test(int func_id, int igroup_id) {
        this->func_id = func_id;
        this->igroup_id = igroup_id;
    }

    /** Delete coverage info and reset to initial state. */
    void clear() {
        WriteOnlyTable<InsnCoverageRow>::clear();
        coverage.clear();
    }

    /** Mark instructions as having been executed. */
    void execute(SgAsmInstruction*);

    /** Returns true if this instruction coverage object contains no instructions. */
    bool empty() const { return coverage.empty(); }

    /** Number of unique addresses executed. */
    size_t nunique() const { return coverage.size(); }

    /** Total number of addresses executed. */
    size_t total_ninsns() const;

    /** Flush pending data to the database and clear this object. */
    void flush(const SqlDatabase::TransactionPtr&);

   /** Returns the coverage ratio for a function.  The return value is the number of unique instructions of the function that
     *  were executed divided by the total number of instructions in the function. This InsnCoverage object may contain
     *  instructions that belong to other functions also, and they are not counted. */
    double get_ratio(SgAsmFunction*, int func_id, int igroup_id) const;

    /** Get instructions covered by trace in the order in which they were encountered.
     */
    void get_instructions(std::vector<SgAsmInstruction*>& insns, SgAsmInterpretation* interp, SgAsmFunction* top = NULL);
protected:
    int func_id, igroup_id;

    typedef std::map<rose_addr_t, InsnCoverageRow> CoverageMap;
    CoverageMap coverage;                       // info about addresses that were executed
};

/*******************************************************************************************************************************
 *                                      Dynamic Function Call Graph
 *******************************************************************************************************************************/

/** Information about a single function call. */
struct DynamicCallGraphRow {
    int func_id;                                /**< ID of function that is being tested. */
    int igroup_id;                              /**< ID of the input group being tested. */
    int caller_id;                              /**< ID of function that is doing the calling. */
    int callee_id;                              /**< ID of the function that is being called. */
    size_t pos;                                 /**< Sequence numbering for ordering calls. */
    size_t ncalls;                              /**< Number of consecutive calls for this caller and callee pair. */

    DynamicCallGraphRow()
        : func_id(-1), igroup_id(-1), caller_id(-1), callee_id(-1), pos(0), ncalls(0) {}
    DynamicCallGraphRow(int func_id, int igroup_id, int caller_id, int callee_id, size_t pos)
        : func_id(func_id), igroup_id(igroup_id), caller_id(caller_id), callee_id(callee_id), pos(pos), ncalls(1) {}

    void serialize(std::ostream &stream) const { // must be in the same order as the schema
        stream <<func_id <<"," <<igroup_id <<"," <<caller_id <<"," <<callee_id <<"," <<pos <<"," <<ncalls <<"\n";
    }
};

/** Dynamic call graph information.  This class stores a call graph by storing edges between the caller and callee function
 * IDs.  The edges are ordered according to when the call occurred. Consecutive parallel edges are stored as a single edge with
 * an @p ncalls attribute larger than one. */
class DynamicCallGraph: public WriteOnlyTable<DynamicCallGraphRow> {
public:
    explicit DynamicCallGraph(bool keep_in_memory=false)
        : WriteOnlyTable<DynamicCallGraphRow>("semantic_fio_calls"),
          keep_in_memory(keep_in_memory), func_id(-1), igroup_id(-1) {}

    /** Set information for current test. */
    void current_test(int func_id, int igroup_id) {
        this->func_id = func_id;
        this->igroup_id = igroup_id;
        this->call_sequence = 0;
    }

    /** Set this call graph back to its empty state. */
    void clear() {
        WriteOnlyTable<DynamicCallGraphRow>::clear();
        last_call = DynamicCallGraphRow();
        rows.clear();
    }

    /** Append a new call to the list of calls.
     * @{ */
    void call(int caller_id, int callee_id);
    /** @} */

    /** Returns true if the graph contains no instructions. */
    bool empty() const {
        return last_call.func_id < 0 && WriteOnlyTable<DynamicCallGraphRow>::empty();
    }

    /** Returns the number of calls. Consecutive calls between a specific caller and callee are compressed into a single call
     *  structure whose ncalls member indicates the number of calls, and which are treated as a single call for the purpose of
     *  this function. */
    size_t size() const {
        return keep_in_memory ?
            rows.size() :
            WriteOnlyTable<DynamicCallGraphRow>::size() + (last_call.func_id>=0 ? 1 : 0);
    }

    /** Return info for the indicated call number. Consecutive calls from a specific caller to callee are treated as one call
     * for the purpose of this function; the @p ncalls member indicates how many consecutive calls actually occurred. The
     * information is only saved in memory if keep_in_memory is set to true during construction. */
    const DynamicCallGraphRow& operator[](size_t idx) const {
        assert(keep_in_memory);
        return rows[idx];
    }

    /** Flush all pending data to the database. */
    void flush(const SqlDatabase::TransactionPtr&);

protected:
    bool keep_in_memory;                // keep copies of rows in memory until clear() is called.
    std::vector<DynamicCallGraphRow> rows; // rows kept in memory only if keep_in_memory is set
    int func_id, igroup_id;             // identifies test; used to fill in the corresponding columns as rows are added
    DynamicCallGraphRow last_call;      // most recent call; this row hasn't been handed to the WriteOnlyTable yet
    size_t call_sequence;               // sequence number for ordering calls
};


/*******************************************************************************************************************************
 *                                      ConsumedInputs
 *******************************************************************************************************************************/

struct ConsumedInputsRow {
    int func_id;                                /**< ID of function that is being tested. */
    int igroup_id;                              /**< ID of the input group being tested. */
    int request_queue_id;                       /**< Input queue from which this input value was requested. */
    int actual_queue_id;                        /**< Input queue from which this value was obtained. */
    size_t pos;                                 /**< Relative position of this input w.r.t. other consumed inputs. */
    uint64_t value;                             /**< Value that was consumed. */

    ConsumedInputsRow(int func_id, int igroup_id, int request_queue_id, int actual_queue_id, size_t pos, uint64_t value)
        : func_id(func_id), igroup_id(igroup_id), request_queue_id(request_queue_id), actual_queue_id(actual_queue_id),
          pos(pos), value(value) {}

    void serialize(std::ostream &stream) const { // output must be same order as schema
        stream <<func_id <<"," <<igroup_id <<"," <<request_queue_id <<"," <<actual_queue_id <<"," <<pos <<"," <<value <<"\n";
    }
};

class ConsumedInputs: public WriteOnlyTable<ConsumedInputsRow> {
public:
    explicit ConsumedInputs(bool keep_in_memory=false)
        : WriteOnlyTable<ConsumedInputsRow>("semantic_fio_inputs"),
          keep_in_memory(keep_in_memory), func_id(-1), igroup_id(-1) {}

    /** Set information for current test. */
    void current_test(int func_id, int igroup_id) {
        this->func_id = func_id;
        this->igroup_id = igroup_id;
        this->pos = 0;
    }

    /** Drop all rows from memory and pending for write. Does not affect rows existing in the database. */
    void clear() {
        rows.clear();
        WriteOnlyTable<ConsumedInputsRow>::clear();
    }

    /** Number of rows in memory or pending to write to database. */
    size_t size() {
        return keep_in_memory ?
            rows.size() :
            WriteOnlyTable<ConsumedInputsRow>::size();
    }

    /** Return a particular row from memory. */
    const ConsumedInputsRow& operator[](size_t idx) {
        assert(keep_in_memory);
        return rows[idx];
    }

    /** Add an input to the list of rows pending to be written to the database. */
    void consumed(int request_queue_id, int actual_queue_id, uint64_t value) {
        assert(func_id>=0 && igroup_id>=0);
        ConsumedInputsRow row(func_id, igroup_id, request_queue_id, actual_queue_id, pos++, value);
        insert(row);
        if (keep_in_memory)
            rows.push_back(row);
    }

private:
    bool keep_in_memory;
    std::vector<ConsumedInputsRow> rows;
    int func_id, igroup_id;
    size_t pos;
};


/*******************************************************************************************************************************
 *                                      Per-function analyses
 *******************************************************************************************************************************/

// Results from various kinds of analyses run on functions
struct FuncAnalysis {
    FuncAnalysis(): ncalls(0), nretused(0), ntests(0), nvoids(0) {}
    size_t ncalls;                      // number of times this function was called
    size_t nretused;                    // number of ncalls where the caller read EAX after the call
    size_t ntests;                      // number of times this function was tested
    size_t nvoids;                      // number of ntests where this function didn't write to EAX
};

typedef std::map<int/*func_id*/, FuncAnalysis> FuncAnalyses;

/*******************************************************************************************************************************
 *                                      Output groups
 *******************************************************************************************************************************/

typedef std::map<int, uint64_t> IdVa;
typedef std::map<uint64_t, int> VaId;

// How to store values.  Define this to be OutputgroupValueVector, OutputGroupValueSet, or OutputGroupValueAddrSet. See their
// definitions below.  They all store values, just in different orders and cardinalities.
#define OUTPUTGROUP_VALUE_CONTAINER OutputGroupValueAddrSet

// Should we store call graph info?
#undef OUTPUTGROUP_SAVE_CALLGRAPH

// Should we store system call info?
#undef OUTPUTGROUP_SAVE_SYSCALLS

// Store every value (including duplicate values) in address order.
template<typename T>
class OutputGroupValueVector {
private:
    typedef std::vector<T> Values;
    Values values_;
public:
    void clear() {
        values_.clear();
    }
    void insert(T value, rose_addr_t where=0) {
        values_.push_back(value);
    }
    const std::vector<T>& get_vector() const { return values_; }
    bool operator<(const OutputGroupValueVector &other) const {
        if (values_.size() != other.values_.size())
            return values_.size() < other.values_.size();
        typedef std::pair<typename Values::const_iterator, typename Values::const_iterator> vi_pair;
        vi_pair vi = std::mismatch(values_.begin(), values_.end(), other.values_.begin());
        if (vi.first!=values_.end())
            return *(vi.first) < *(vi.second);
        return false;
    }
    bool operator==(const OutputGroupValueVector &other) const {
        return values_.size() == other.values_.size() && std::equal(values_.begin(), values_.end(), other.values_.begin());
    }
};

// Store set of unique values in ascending order of value
template<typename T>
class OutputGroupValueSet {
private:
    typedef std::set<T> Values;
    Values values_;
public:
    void clear() {
        values_.clear();
    }
    void insert(T value, rose_addr_t where=0) {
        values_.insert(value);
    }
    std::vector<T> get_vector() const {
        std::vector<T> retval(values_.begin(), values_.end());
        return retval;
    }
    bool operator<(const OutputGroupValueSet &other) const {
        if (values_.size() != other.values_.size())
            return values_.size() < other.values_.size();
        typedef std::pair<typename Values::const_iterator, typename Values::const_iterator> vi_pair;
        vi_pair vi = std::mismatch(values_.begin(), values_.end(), other.values_.begin());
        if (vi.first!=values_.end())
            return *(vi.first) < *(vi.second);
        return false;
    }
    bool operator==(const OutputGroupValueSet &other) const {
        return values_.size() == other.values_.size() && std::equal(values_.begin(), values_.end(), other.values_.begin());
    }
};

template<typename T>
struct OutputGroupValueAddrSetLessp {
    bool operator()(const std::pair<T, rose_addr_t> &a, const std::pair<T, rose_addr_t> &b) {
        return a.second < b.second;
    }
};

// Store set of unique values in ascending order of the value's minimum address
template<typename T>
class OutputGroupValueAddrSet {
private:
    typedef std::map<T, rose_addr_t> Values;
    Values values_;
public:
    void clear() {
        values_.clear();
    }
    void insert(T value, rose_addr_t where=0) {
        std::pair<typename Values::iterator, bool> found = values_.insert(std::make_pair(value, where));
        if (!found.second)
            found.first->second = std::min(found.first->second, where);
    }
    std::vector<T> get_vector() const {
        std::vector<std::pair<T, rose_addr_t> > tmp(values_.begin(), values_.end());
        std::sort(tmp.begin(), tmp.end(), OutputGroupValueAddrSetLessp<T>());
        std::vector<T> retval;
        retval.reserve(tmp.size());
        for (size_t i=0; i<tmp.size(); ++i)
            retval.push_back(tmp[i].first);
        return retval;
    }
    bool operator<(const OutputGroupValueAddrSet &other) const {
        if (values_.size() != other.values_.size())
            return values_.size() < other.values_.size();
        typename Values::const_iterator vi1 = values_.begin();
        typename Values::const_iterator vi2 = other.values_.begin();
        for (/*void*/; vi1!=values_.end(); ++vi1, ++vi2) {
            if (vi1->first!=vi2->first)
                return vi1->first < vi2->first;
        }
        return false;
    }
    bool operator==(const OutputGroupValueAddrSet &other) const {
        if (values_.size() != other.values_.size())
            return false;
        typename Values::const_iterator vi1 = values_.begin();
        typename Values::const_iterator vi2 = other.values_.begin();
        for (/*void*/; vi1!=values_.end(); ++vi1, ++vi2) {
            if (vi1->first!=vi2->first)
                return false;
        }
        return true;
    }
};



/** Collection of output values. The output values are gathered from the instruction semantics state after a specimen function
 *  is analyzed.  The outputs consist of those interesting registers that are marked as having been written to by the specimen
 *  function, and the memory values whose memory cells are marked as having been written to.  We omit status flags since they
 *  are not typically treated as function call results, and we omit the instruction pointer (EIP). */
class OutputGroup {
public:
    typedef uint32_t value_type;
    OutputGroup(): fault(AnalysisFault::NONE), ninsns(0), retval(0), has_retval(false) {}
    bool operator<(const OutputGroup &other) const;
    bool operator==(const OutputGroup &other) const;
    void clear();
    void print(std::ostream&, const std::string &title="", const std::string &prefix="") const;
    void print(RTS_Message*, const std::string &title="", const std::string &prefix="") const;
    friend std::ostream& operator<<(std::ostream &o, const OutputGroup &outputs) {
        outputs.print(o);
        return o;
    }
    void add_param(const std::string vtype, int pos, int64_t value); // used by OutputGroups

    /** Add another value to the output. */
    void insert_value(int64_t value, rose_addr_t va) {
        values.insert(value, va);
    }

    /** Return all the output values. */
    std::vector<value_type> get_values() const { return values.get_vector(); }

    /** Add a return value to this output group. */
    void insert_retval(uint64_t retval) {
        this->retval = retval;
        has_retval = true;
    }

    /** Remove the return value, if any. */
    void remove_retval() {
        this->retval = 0;
        has_retval = false;
    }

    /** Returns the return value and whether the return value is the default or explicitly set. */
    std::pair<bool, value_type> get_retval() const {
        return std::make_pair(has_retval, retval);
    }

    /** Accessor for the fault value.
     * @{ */
    AnalysisFault::Fault get_fault() const { return fault; }
    void set_fault(AnalysisFault::Fault &fault) { this->fault = fault; }
    /** @} */

    /** Add another function ID for a called function. */
    void insert_callee_id(int id) { callee_ids.push_back(id); }

    /** Return the list of all function call IDs. */
    const std::vector<int>& get_callee_ids() const { return callee_ids; }

    /** Add another system call to the list of system calls. */
    void insert_syscall(int syscall_number) { syscalls.push_back(syscall_number); }

    /** Return the list of all system calls in the order they occurred. */
    const std::vector<int>& get_syscalls() const { return syscalls; }

    /** Accessor for number of instructions executed.
     * @{ */
    void set_ninsns(size_t ninsns) { this->ninsns = ninsns; }
    size_t get_ninsns() const { return ninsns; }
    /** @} */

    /** Accessor for signature vectors
     * @{ */
    SignatureVector& get_signature_vector() { return this->signature_vector; }
    /** @} */

protected:
    OUTPUTGROUP_VALUE_CONTAINER<value_type> values;
    std::vector<int> callee_ids;                // IDs for called functions
    std::vector<int> syscalls;                  // system call numbers in the order they occur
    AnalysisFault::Fault fault;                 // reason for test failure (or FAULT_NONE)
    size_t ninsns;                              // number of instructions executed
    value_type retval;                          // return value from function
    bool has_retval;                            // true if 'retval' is initialized
    SignatureVector signature_vector;           // feature vector

};

// Used internally by OutputGroups so that we don't have to store OutputGroup objects in std::map. OutputGroup objects can be
// very large, causing std::map reallocation to be super slow.  Therefore, the map stores OutputGroupDesc objects which have
// an operator< that compares the pointed-to OutputGroup objects.
class OutputGroupDesc {
public:
    explicit OutputGroupDesc(const OutputGroup *ogroup): ptr(ogroup) { assert(ogroup!=NULL); }
    bool operator<(const OutputGroupDesc &other) const {
        assert(ptr!=NULL && other.ptr!=NULL);
        return *ptr < *other.ptr;
    }
private:
    const OutputGroup *ptr;
};

/** A collection of OutputGroup objects, each having an ID number.  ID numbers are 63-bit random numbers (non-negative 63-bit
 *  values) so that we don't have to synchronize between processes that are creating them. */
class OutputGroups {
public:
    /** Constructor creates an empty OutputGroups. */
    OutputGroups(): file(NULL) {}

    /** Constructor initializes this OutputGroup with the contents of the database. */
    explicit OutputGroups(const SqlDatabase::TransactionPtr &tx): file(NULL) { load(tx); }

    ~OutputGroups();

    /** Reload this output group with the contents from the database. */
    void load(const SqlDatabase::TransactionPtr&);

    /** Reload only the specified hashkey and return a pointer to it. Returns false if the object does not exist in the
     * database (and also removes it from memory if it existed there). */
    bool load(const SqlDatabase::TransactionPtr&, int64_t hashkey);

    /** Insert a new OutputGroup locally.  This does not update the database. If @p hashkey is not the default -1 then
     *  this function assumes that the hash key was obtained from the database and therefore we are adding an OutputGroup
     *  object that is already in the database; such an object will not be written back to the database by a save()
     *  operation. */
    int64_t insert(const OutputGroup&, int64_t hashkey=-1);

    /** Erase the specified output group according to its hash key. */
    void erase(int64_t hashkey);

    /** Find the hashkey for an existing OutputGroup. Returns -1 if the specified OutputGroup does not exist. */
    int64_t find(const OutputGroup&) const;

    /** Does the given hashkey exist? */
    bool exists(int64_t hashkey) const;

    /** Find the output group for the given hash key. Returns NULL if the hashkey does not exist. */
    const OutputGroup* lookup(int64_t hashkey) const;

    /** Save locally-inserted OutputGroup objects to the database. */
    void save(const SqlDatabase::TransactionPtr&);

    /** Generate another hash key. */
    int64_t generate_hashkey();

    /** Return the set of all output group hash keys. */
    std::vector<int64_t> hashkeys() const;

private:
    typedef std::map<int64_t/*hashkey*/, OutputGroup*> IdOutputMap;
    typedef std::map<OutputGroupDesc, int64_t/*hashkey*/> OutputIdMap;
    IdOutputMap ogroups;
    OutputIdMap ids;
    LinearCongruentialGenerator lcg; // to generate ID numbers
    std::string filename;
    FILE *file; // temp file for new rows
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
    InstructionProvidor() {}
    InstructionProvidor(const Functions &functions) {
        for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi)
            insert(*fi);
    }
    InstructionProvidor(const std::vector<SgAsmFunction*> &functions) {
        for (size_t i=0; i<functions.size(); ++i)
            insert(functions[i]);
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

typedef rose::BinaryAnalysis::PointerAnalysis::PointerDetection<InstructionProvidor> PointerDetector;

/*******************************************************************************************************************************
 *                                      Address hasher
 *******************************************************************************************************************************/

/** Hashes a virtual address to a random integer. */
class AddressHasher {
public:
    AddressHasher() { init(0, 255, 0); }
    AddressHasher(uint64_t minval, uint64_t maxval, int seed): lcg(0) { init(minval, maxval, seed); }

    void init(uint64_t minval, uint64_t maxval, int seed) {
        this->minval = std::min(minval, maxval);
        this->maxval = std::max(minval, maxval);
        lcg.reseed(seed);
        for (size_t i=0; i<256; ++i) {
            tab[i] = lcg() % 256;
            val[i] = lcg();
        }
    }

    uint64_t operator()(rose_addr_t addr) {
        uint64_t retval;
        uint8_t idx = 0;
        for (size_t i=0; i<4; ++i) {
            uint8_t byte = IntegerOps::shiftRightLogical2(addr, 8*i) & 0xff;
            idx = tab[(idx+byte) & 0xff];
            retval ^= val[idx];
        }
        return minval + retval % (maxval+1-minval);
    }
private:
    uint64_t minval, maxval;
    LinearCongruentialGenerator lcg;
    uint8_t tab[256];
    uint64_t val[256];
};

/*******************************************************************************************************************************
 *                                      Input Group
 *******************************************************************************************************************************/

enum InputQueueName {
    IQ_ARGUMENT= 0,                            // queue used for function arguments
    IQ_LOCAL,                                  // queue used for local variables
    IQ_GLOBAL,                                 // queue used for global variables
    IQ_FUNCTION,                               // queue used for return value for functions skipped over
    IQ_POINTER,                                // queue used for pointers if none of the above apply
    IQ_MEMHASH,                                // pseudo-queue used for memory values if none of the above apply
    IQ_INTEGER,                                // queue used if none of the above apply
    // must be last
    IQ_NONE,
    IQ_NQUEUES=IQ_NONE
};

/** Queue of values for input during testing. */
class InputQueue {
public:
    InputQueue(): nconsumed_(0), infinite_(false), pad_value_(0), redirect_(IQ_NONE) {}

    /** Reset the queue back to the beginning. */
    void reset() { nconsumed_=0; }

    /** Remove all values from the queue and set it to an initial state. */
    void clear() { *this=InputQueue(); }

    /** Returns true if the queue is an infinite sequence of values. */
    bool is_infinite() const { return infinite_; }

    /** Returns true if the queue contains a value at the specified position.*/
    bool has(size_t idx) { return infinite_ || idx<values_.size(); }

    /** Returns the finite size of the queue not counting any infinite padding. */
    size_t size() const { return values_.size(); }

    /** Returns a value or throws an INPUT_LIMIT FaultException. */
    uint64_t get(size_t idx) const;

    /** Consumes a value or throws an INPUT_LIMIT FaultException. */
    uint64_t next() { return get(nconsumed_++); }

    /** Returns the number of values consumed. */
    size_t nconsumed() const { return nconsumed_; }

    /** Appends N copies of a value to the queue. */
    void append(uint64_t val, size_t n=1) { if (!infinite_) values_.resize(values_.size()+n, val); }

    /** Appends values using a range of iterators. */
    template<typename Iterator>
    void append_range(Iterator from, Iterator to) {
        if (!infinite_)
            values_.insert(values_.end(), from, to);
    }

    /** Appends an infinite number copies of @p val to the queue. */
    void pad(uint64_t val) { if (!infinite_) { pad_value_=val; infinite_=true; }}

    /** Extend the explicit values by copying some of the infinite values. */
    std::vector<uint64_t>& extend(size_t n);

    /** Explicit values for an input group.
     * @{ */
    std::vector<uint64_t>& values() { return values_; }
    const std::vector<uint64_t>& values() const { return values_; }
    /** @} */

    /** Redirect property. This causes InputGroup to redirect one queue to another. Only one level of indirection
     *  is supported, which allows two queues to be swapped.
     * @{ */
    void redirect(InputQueueName qn) { redirect_=qn; }
    InputQueueName redirect() const { return redirect_; }
    /** @} */

    /** Load a row from the semantic_outputvalues table into this queue. */
    void load(int pos, uint64_t val);

private:
    size_t nconsumed_;                          // Number of values consumed
    std::vector<uint64_t> values_;              // List of explicit values
    bool infinite_;                             // List of explicit values followed by an infinite number of pad_values
    uint64_t pad_value_;                        // Value padded to infinity
    InputQueueName redirect_;                   // Use some other queue instead of this one (max 1 level of indirection)
};

/** Initial values to supply for inputs to tests.  An input group constists of a set of value queues.  The values are defined
 *  as unsigned 64-bit integers which are then cast to the appropriate size when needed. */
class InputGroup {
public:
    typedef std::vector<InputQueue> Queues;

    InputGroup()
        : queues_(IQ_NQUEUES), collection_id(-1), nconsumed_virtual_(IQ_NQUEUES, 0) {}
    InputGroup(const SqlDatabase::TransactionPtr &tx, int id)
        : queues_(IQ_NQUEUES), collection_id(-1), nconsumed_virtual_(IQ_NQUEUES, 0) { load(tx, id); }

    /** Return a name for one of the queues. */
    static std::string queue_name(InputQueueName q) {
        switch (q) {
            case IQ_ARGUMENT: return "argument";
            case IQ_LOCAL:    return "local";
            case IQ_GLOBAL:   return "global";
            case IQ_FUNCTION: return "function";
            case IQ_POINTER:  return "pointer";
            case IQ_MEMHASH:  return "memhash";
            case IQ_INTEGER:  return "integer";
            default: assert(!"fixme"); abort();
        }
    }

    /** Load a single input group from the database. Returns true if the input group exists in the database. */
    bool load(const SqlDatabase::TransactionPtr&, int id);

    /** Save this input group in the database.  It shouldn't already be in the database, or this will add a conflicting copy. */
    void save(const SqlDatabase::TransactionPtr&, int igroup_id, int64_t cmd_id);

    /** Obtain a reference to one of the queues.
     *  @{ */
    InputQueue& queue(InputQueueName qn) { return queues_[(int)qn]; }
    const InputQueue& queue(InputQueueName qn) const { return queues_[(int)qn]; }
    /** @} */

    /** Returns how many items were consumed across all queues. */
    size_t nconsumed() const;

    /** Returns how many values were virtually consumed from a queue.  This is the number of items that would have been
     *  consumed from this queue if requests to consume a value for this queue were not redirected to some other queue. E.g.,
     *  this measure how many arguments a function read even if those argument values ultimately came from the integer queue
     *  rather than the argument queue. */
    size_t nconsumed_virtual(InputQueueName qn) const { return nconsumed_virtual_[qn]; }

    /** Increment the value returned by nconsumed_virtual(). */
    void inc_nconsumed_virtual(InputQueueName qn) { ++nconsumed_virtual_[qn]; }

    /** Resets the queues to the beginning. Also resets the nconsumed_virtual counters. */
    void reset();

    /** Resets and emptiess all queues. */
    void clear();

    /** Collection to which input group is assigned. The @p dflt is usually the same as the input group ID. */
    int get_collection_id(int dflt=-1) const { return collection_id<0 ? dflt : collection_id; }
    void set_collection_id(int collection_id) { this->collection_id=collection_id; }

protected:
    Queues queues_;
    int collection_id;
    std::vector<size_t> nconsumed_virtual_;     // num values consumed per queue before redirects were applied
};

/****************************************************************************************************************************
 *                                      Semantic policy parameters
 ****************************************************************************************************************************/

// These parameters control the instruction semantics layer used by clone detection.  See also the Switches class in
// RunTests.h, which holds the higher-level parameters.
struct PolicyParams {
    PolicyParams()
        : timeout(5000), verbosity(SILENT), follow_calls(CALL_NONE), initial_stack(0x80000000),
          compute_coverage(false), compute_callgraph(false), top_callgraph(false), compute_consumed_inputs(false) {}

    size_t timeout;                     /**< Maximum number of instrutions per fuzz test before giving up. */
    Verbosity verbosity;                /**< Produce lots of output?  Traces each instruction as it is simulated. */
    FollowCalls follow_calls;           /**< Follow CALL instructions if possible rather than consuming an input? */
    rose_addr_t initial_stack;          /**< Initial values for ESP and EBP. */
    bool compute_coverage;              /**< Compute instruction coverage information? */
    bool compute_callgraph;             /**< Compute dynamic call graph information? */
    bool top_callgraph;                 /**< Store only function call edges emanating from the top stack frame? */
    bool compute_consumed_inputs;       /**< Insert consumed inputs into the ConsumedInputs object? */
};


/*******************************************************************************************************************************
 *                                      Analysis Machine State
 *******************************************************************************************************************************/

/** Bits to track variable access. */
enum {
    NO_ACCESS=0,                        /**< Variable has been neither read nor written. */
    HAS_BEEN_READ=1,                    /**< Variable has been read. */
    HAS_BEEN_WRITTEN=2,                 /**< Variable has been written. */
    HAS_BEEN_INITIALIZED=4,             /**< Variable has been initialized by the analysis before the test starts. */
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
    MemoryValue()
        : val(0), rw_state(NO_ACCESS), first_of_n(0), segreg(x86_segreg_none) {}
    MemoryValue(uint8_t val, unsigned rw_state, size_t first_of_n, X86SegmentRegister sr)
        : val(val), rw_state(rw_state), first_of_n(first_of_n), segreg(sr) {}
    uint8_t val;
    unsigned rw_state;
    size_t first_of_n;          // number of bytes written if this is the first byte of a multi-byte write
    X86SegmentRegister segreg;  // segment register last used to write to this memory
};

/** Analysis machine state. We override some of the memory operations. All values are concrete (we're using
 *  PartialSymbolicSemantics only for its constant-folding ability and because we don't yet have a specifically concrete
 *  semantics domain). */
template <template <size_t> class ValueType>
class State: public PartialSymbolicSemantics::State<ValueType> {
public:
    typedef std::map<uint32_t, MemoryValue> MemoryCells;        // memory cells indexed by address
    MemoryCells memory;                                         // memory state
    BaseSemantics::RegisterStateX86<ValueType> registers;
    BaseSemantics::RegisterStateX86<ReadWriteState> register_rw_state;
    OutputGroup output_group;                                  // output values filled in as we run a function

    // Write a single byte to memory. The rw_state are the HAS_BEEN_READ and/or HAS_BEEN_WRITTEN bits.
    void mem_write_byte(X86SegmentRegister sr, const ValueType<32> &addr, const ValueType<8> &value, size_t first_of_n,
                        unsigned rw_state=HAS_BEEN_WRITTEN) {
        memory[addr.known_value()] = MemoryValue(value.known_value(), rw_state, first_of_n, sr);
    }

    // Read a single byte from memory.  If the read operation cannot find an appropriate memory cell, then @p
    // uninitialized_read is set (it is not cleared in the counter case) and the return value is default constructed (for
    // PartialSymbolicSemantics it is a unique undefined value).
    ValueType<8> mem_read_byte(X86SegmentRegister sr, const ValueType<32> &addr, bool *uninitialized_read=NULL/*out*/) {
        typename MemoryCells::iterator ci = memory.find(addr.known_value());
        if (ci!=memory.end())
            return ValueType<8>(ci->second.val);
        if (uninitialized_read)
            *uninitialized_read = true;
        return ValueType<8>();
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
        memory.clear();
        registers.clear();
        register_rw_state.clear();
        output_group.clear();
    }

    // Determines whether the specified address is the location of an output value. If so, then this returns the number of
    // bytes in that output value, otherwise it return zero.  The output value might not be known yet since we only use the
    // last value written to an address (this might not be the last one).  The addr must be the first address of a multi-byte
    // write in order to be an output.
    size_t is_memory_output(rose_addr_t addr) {
        MemoryCells::const_iterator ci = memory.find(addr);
        return ci==memory.end() ? 0 : is_memory_output(addr, ci->second);
    }
    size_t is_memory_output(rose_addr_t addr, const MemoryValue &mval) {
        return 0!=(mval.rw_state & HAS_BEEN_WRITTEN) && x86_segreg_ss!=mval.segreg ? mval.first_of_n : 0;
    }

    // If the specified address looks like it points to a string in the memory map then return a hash of that string, otherwise
    // return the address.
    rose_addr_t hash_if_string(rose_addr_t va, const MemoryMap *map) {
        rose_addr_t retval = va;
        static const size_t limit = 4096; // arbitrary
        std::string str = map->readString(va, limit, isascii);
        if (str.size()>=5 && str.size()<4096) {
            std::vector<uint8_t> digest = Combinatorics::sha1_digest(str);
            assert(20==digest.size());
            retval = *(rose_addr_t*)&digest[0];
        }
        return retval;
    }

    // Return output values.  These are the interesting general-purpose registers to which a value has been written, and the
    // memory locations to which a value has been written.  The returned object can be deleted when no longer needed.
    OutputGroup get_outputs(Verbosity verbosity, const MemoryMap *map) {
        OutputGroup outputs = this->output_group;

        // Function return value is EAX, but only if it has been written to
        if (0 != (register_rw_state.gpr[x86_gpr_ax].state & HAS_BEEN_WRITTEN)) {
            rose_addr_t v1 = registers.gpr[x86_gpr_ax].known_value();
            rose_addr_t v2 = hash_if_string(v1, map);
            if (verbosity>=EFFUSIVE) {
                std::cerr <<"output for ax = " <<ValueType<32>(v2);
                if (v1!=v2)
                    std::cerr <<" (hash of string at " <<StringUtility::addrToString(v1) <<")";
                std::cerr <<"\n";
            }
            outputs.insert_retval(v2);
        }

        // Memory outputs
        for (MemoryCells::const_iterator ci=memory.begin(); ci!=memory.end(); ++ci) {
            rose_addr_t addr = ci->first;
            const MemoryValue &mval = ci->second;
            if (is_memory_output(addr, mval)) {
                assert(mval.first_of_n<=4);
                OutputGroup::value_type v1 = 0;
                MemoryCells::const_iterator ci2=ci;
                for (size_t i=0; i<mval.first_of_n && ci2!=memory.end(); ++i, ++ci2) {
                    if (ci2->first != addr+i || (i>0 && ci2->second.first_of_n!=0) || !is_memory_output(ci2->first, ci2->second))
                        break;
                    v1 |= IntegerOps::shiftLeft2<OutputGroup::value_type>(ci2->second.val, 8*i); // little endian
                }
                OutputGroup::value_type v2 = hash_if_string(v1, map);
                if (verbosity>=EFFUSIVE) {
                    int nbytes = v1==v2 ? (int)mval.first_of_n : 4;
                    char buf[32];
                    snprintf(buf, sizeof buf, "%0*"PRIx64, 2*nbytes, (uint64_t)v2);
                    std::cerr <<"output for mem[" <<StringUtility::addrToString(addr) <<"] = " <<buf;
                    if (v1!=v2)
                        std::cerr <<" (hash of string at " <<StringUtility::addrToString(v1) <<")";
                    std::cerr <<"\n";
                }
                outputs.insert_value(v2, addr);
            }
        }

        return outputs;
    }

    // Printing
    void print(std::ostream &o) const {
        BaseSemantics::Formatter fmt;
        fmt.set_line_prefix("    ");
        print(o, fmt);
    }
    void print(std::ostream &o, BaseSemantics::Formatter &fmt) const {
        this->registers.print(o, fmt);
        size_t ncells=0, max_ncells=500;
        o <<"== Memory ==\n";
        for (typename MemoryCells::const_iterator ci=memory.begin(); ci!=memory.end(); ++ci) {
            uint32_t addr = ci->first;
            const MemoryValue &mval = ci->second;
            if (++ncells>max_ncells) {
                o <<"    skipping " <<memory.size()-(ncells-1) <<" more memory cells for brevity's sake...\n";
                break;
            }
            o <<"    mem[" <<StringUtility::addrToString(addr) <<"] = " <<StringUtility::addrToString(mval.val, 8);
            if (mval.first_of_n) {
                o <<" 1/" <<mval.first_of_n;
            } else {
                o <<"    ";
            }
            o <<(0==(mval.rw_state & HAS_BEEN_READ)?"":" read")
              <<(0==(mval.rw_state & HAS_BEEN_WRITTEN)?"":" write")
              <<(0==(mval.rw_state & (HAS_BEEN_READ|HAS_BEEN_WRITTEN))?" init":"")
              <<"\n";
      }
    }

    friend std::ostream& operator<<(std::ostream &o, const State &state) {
        state.print(o);
        return o;
    }
};

/*******************************************************************************************************************************
 *                                      Stack frames
 *******************************************************************************************************************************/

/** Information about a function call. */
struct StackFrame {
    StackFrame(SgAsmFunction *func, rose_addr_t esp)
        : func(func), entry_esp(esp), monitoring_stdcall(true), stdcall_args_va(0), follow_calls(false), last_call(NULL) {}
    SgAsmFunction *func;                        // Function that is called
    rose_addr_t entry_esp;                      // ESP when this function was first entered
    bool monitoring_stdcall;                    // True for a little while when we enter a new function
    rose_addr_t stdcall_args_va;                // ESP where stdcall arguments will be pushed
    bool follow_calls;                          // Traverse into called functions
    SgAsmFunction *last_call;                   // Address of last called function, reset as soon as EAX is read/written
};

/** Information about all pending function calls.  This is the stack of functions that have not yet returned. */
class StackFrames {
    typedef std::vector<StackFrame> Stack;
    Stack stack_frames;
public:
    /** Clear the stack. */
    void clear() { stack_frames.clear(); }

    /** Returns true if the call stack is empty. Normally, the function being analyzed would be on the stack. */
    bool empty() const { return stack_frames.empty(); }

    /** Returns the number of stack frames on the call stack. */
    size_t size() const { return stack_frames.size(); }

    /** Returns the stack frame for the most recently called function that has not yet returned.
     * @{ */
    const StackFrame& top() const { return (*this)[0]; }
          StackFrame& top()       { return (*this)[0]; }
    /** @} */

    /** Returns the Nth frame from the top. The top frame is N=0.
     * @{ */
    const StackFrame& operator[](size_t n) const { assert(n<size()); return stack_frames[size()-(n+1)]; }
          StackFrame& operator[](size_t n)       { assert(n<size()); return stack_frames[size()-(n+1)]; }
    /** @} */

    /** Pop one frame from the stack.  The stack must not be empty. */
    void pop() {
        assert(!empty());
        SgAsmFunction *func = top().func;
        stack_frames.pop_back();
        if (!empty())
            top().last_call = func;
    }

    /** Pop frames from the stack until @p esp is valid for the top frame. */
    void pop_to(rose_addr_t esp) { while (!empty() && top().entry_esp<esp) pop(); }

    /** Push a new frame if @p insn is the entry instruction for a function. Returns true if a new frame is pushed. */
    bool push_if(SgAsmInstruction *insn, rose_addr_t esp) {
        SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(insn);
        if (func && insn->get_address()==func->get_entry_va()) {
            stack_frames.push_back(StackFrame(func, esp));
            return true;
        }
        return false;
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
    static const rose_addr_t FUNC_RET_ADDR = 4083;      // Special return address to mark end of analysis
    InputGroup *inputs;                                 // Input values to use when reading a never-before-written variable
    const PointerDetector *pointers;                    // Addresses of pointer variables, or null if not analyzed
    SgAsmInterpretation *interp;                        // Interpretation in which we're executing
    size_t ninsns;                                      // Number of instructions processed since last trigger() call
    AddressHasher address_hasher;                       // Hashes a virtual address
    bool address_hasher_initialized;                    // True if address_hasher was initialized
    const InstructionProvidor *insns;                   // Instruction cache
    PolicyParams params;                                // Parameters for controlling the policy
    AddressIdMap entry2id;                              // Map from function entry address to function ID
    SgAsmBlock *prev_bb;                                // Previously executed basic block
    Tracer &tracer;                                     // Responsible for emitting rows for semantic_fio_trace
    bool uses_stdcall;                                  // True if this executable uses the stdcall calling convention
    StackFrames stack_frames;                           // Stack frames ordered by decreasing entry_esp values
    rose::BinaryAnalysis::Disassembler::AddressSet whitelist_exports; // Dynamic funcs that can be called for CALL_BUILTIN
    FuncAnalyses &funcinfo;                             // Partial results of various kinds of function analyses
    InsnCoverage &insn_coverage;                        // Information about which instructions were executed
    DynamicCallGraph &dynamic_cg;                       // Information about function calls
    ConsumedInputs &consumed_inputs;                    // List of input values consumed

    Policy(const PolicyParams &params, const AddressIdMap &entry2id, Tracer &tracer, FuncAnalyses &funcinfo,
           InsnCoverage &insn_coverage, DynamicCallGraph &dynamic_cg, ConsumedInputs &consumed_inputs)
        : inputs(NULL), pointers(NULL), interp(NULL), ninsns(0), address_hasher(0, 255, 0),
          address_hasher_initialized(false), insns(0), params(params), entry2id(entry2id), prev_bb(NULL),
          tracer(tracer), uses_stdcall(false), funcinfo(funcinfo), insn_coverage(insn_coverage),
          dynamic_cg(dynamic_cg), consumed_inputs(consumed_inputs) {}

    // Consume and return an input value.  An argument is needed only when the memhash queue is being used.
    template <size_t nBits>
    ValueType<nBits>
    next_input_value(InputQueueName qn_orig, rose_addr_t addr=0) {
        // Instruction semantics API1 calls readRegister when initializing X86InstructionSemantics in order to obtain the
        // original EIP value, but we haven't yet set up an input group (nor would we want this initialization to consume
        // an input anyway).  So just return zero.
        if (!inputs)
            return ValueType<nBits>(0);

        // One level of indirection allowed
        InputQueueName qn2 = inputs->queue(qn_orig).redirect();
        InputQueueName qn = IQ_NONE==qn2 ? qn_orig : qn2;

        // Get next value, which might throw an AnalysisFault::INPUT_LIMIT FaultException
        ValueType<nBits> retval;
        if (IQ_MEMHASH==qn) {
            retval = ValueType<nBits>(address_hasher(addr));
            if (params.verbosity>=EFFUSIVE) {
                std::cerr <<"CloneDetection: using " <<InputGroup::queue_name(qn)
                          <<" addr " <<StringUtility::addrToString(addr) <<": " <<retval <<"\n";
            }
        } else {
            retval = ValueType<nBits>(inputs->queue(qn).next());
            size_t nvalues = inputs->queue(qn).nconsumed();
            if (params.verbosity>=EFFUSIVE) {
                std::cerr <<"CloneDetection: using " <<InputGroup::queue_name(qn)
                          <<" input #" <<nvalues <<": " <<retval <<"\n";
            }
        }
        inputs->inc_nconsumed_virtual(qn_orig); // before redirection
        tracer.emit(this->get_insn()->get_address(), EV_CONSUME_INPUT, retval.known_value(), (int)qn);
        if (params.compute_consumed_inputs)
            consumed_inputs.consumed((int)qn_orig, (int)qn, retval.known_value());
        return retval;
    }

    // Return output values. These include the return value, certain memory writes, function calls, system calls, etc.
    OutputGroup get_outputs() {
        return state.get_outputs(params.verbosity, interp->get_map());
    }

    // Sets up the machine state to start the analysis of one function.
    void reset(SgAsmInterpretation *interp, SgAsmFunction *func, InputGroup *inputs, const InstructionProvidor *insns,
               const PointerDetector *pointers/*=NULL*/,
               const rose::BinaryAnalysis::Disassembler::AddressSet &whitelist_exports) {
        inputs->reset();
        this->inputs = inputs;
        this->insns = insns;
        this->pointers = pointers;
        this->ninsns = 0;
        this->interp = interp;
        this->whitelist_exports = whitelist_exports;
        stack_frames.clear();
        state.reset_for_analysis();

        // Windows PE uses stdcall, where the callee cleans up its arguments that were pushed by the caller.
        this->uses_stdcall = false;
        const SgAsmGenericHeaderPtrList &hdrs = interp->get_headers()->get_headers();
        for (SgAsmGenericHeaderPtrList::const_iterator hi=hdrs.begin(); hi!=hdrs.end() && !uses_stdcall; ++hi)
            uses_stdcall = NULL!=isSgAsmPEFileHeader(*hi);

        // Initialize the address-to-value hasher for the memhash input "queue"
        InputQueue &memhash = inputs->queue(IQ_MEMHASH);
        if (memhash.has(0)) {
            uint64_t seed   = memhash.get(0);
            uint64_t minval = memhash.has(1) ? memhash.get(1) : 255;
            uint64_t maxval = memhash.has(2) ? memhash.get(2) : 0;
            address_hasher.init(minval, maxval, seed);
            address_hasher_initialized = true;
        } else {
            address_hasher_initialized = false;
        }

        // Initialize EIP. Anything part of the machine state initialized in this reset are marked as HAS_BEEN_INITIALIZE
        // as opposed to HAS_BEEN_WRITTEN.  This allows us to distinguish later between stuff we initialized as part of the
        // analysis vs. output that the test produced while it was running.
        rose_addr_t target_va = func->get_entry_va();
        ValueType<32> eip(target_va);
        state.registers.ip = eip;
        state.register_rw_state.ip.state = HAS_BEEN_INITIALIZED;

        // Initialize the stack and frame pointer. Push a special return address onto the top of the stack.  When the analysis
        // sees this value in the EIP register then we know the test has completed.
        this->writeMemory<32>(x86_segreg_ss, ValueType<32>(params.initial_stack-4), ValueType<32>(FUNC_RET_ADDR),
                              ValueType<1>(1), HAS_BEEN_INITIALIZED);
        ValueType<32> esp(params.initial_stack-4); // stack grows down
        state.registers.gpr[x86_gpr_sp] = esp;
        state.register_rw_state.gpr[x86_gpr_sp].state = HAS_BEEN_INITIALIZED;
        ValueType<32> ebp(params.initial_stack);
        state.registers.gpr[x86_gpr_bp] = ebp;
        state.register_rw_state.gpr[x86_gpr_bp].state = HAS_BEEN_INITIALIZED;

        // Initialize callee-saved registers. The callee-saved registers interfere with the analysis because if the same
        // function is compiled two different ways, then it might use different numbers of callee-saved registers.  Since
        // callee-saved registers are pushed onto the stack without first initializing them, the push consumes an input.
        // Therefore, we must consistently initialize all possible callee-saved registers.  We are assuming cdecl calling
        // convention (i.e., GCC's default for C/C++).  Registers are marked as HAS_BEEN_INITIALIZED so that an input isn't
        // consumed when they're read during the test.
        ValueType<32> rval(inputs->queue(IQ_INTEGER).has(0) ? inputs->queue(IQ_INTEGER).get(0) : 0);
        state.registers.gpr[x86_gpr_bx] = rval;
        state.register_rw_state.gpr[x86_gpr_bx].state = HAS_BEEN_INITIALIZED;
        state.registers.gpr[x86_gpr_si] = rval;
        state.register_rw_state.gpr[x86_gpr_si].state = HAS_BEEN_INITIALIZED;
        state.registers.gpr[x86_gpr_di] = rval;
        state.register_rw_state.gpr[x86_gpr_di].state = HAS_BEEN_INITIALIZED;
    }

    // Tries to figure out where the function call arguments start for functions called by this function.  This is important
    // when using the stdcall convention because we might need to pop arguments for function calls that we skip
    // over. Unfortunately, it doesn't work all that well -- it often sets the value too high because the demarcation is too
    // vague.
    void monitor_esp(SgAsmInstruction *insn, StackFrame &stack_frame) {
        if (!uses_stdcall) {
            stack_frame.monitoring_stdcall = false;
            return;
        }
        SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(insn);
        SgAsmBlock *bb = SageInterface::getEnclosingNode<SgAsmBlock>(insn);
        SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(bb);
        assert(func!=NULL);
        if (stack_frame.monitoring_stdcall) {
            bool do_monitor = false;
            if (bb->get_id()==func->get_entry_va()) {   // are we in the function's entry block?
                const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
                if (x86_push==insn_x86->get_kind() && 1==args.size() && isSgAsmRegisterReferenceExpression(args[0])) {
                    RegisterDescriptor reg = isSgAsmRegisterReferenceExpression(args[0])->get_descriptor();
                    do_monitor = reg.get_major()!=x86_regclass_gpr ||
                                 (reg.get_minor()!=x86_gpr_ax && reg.get_minor()!=x86_gpr_cx &&
                                  reg.get_minor()!=x86_gpr_dx); // pushing a callee-saved register
                } else if (x86_mov==insn_x86->get_kind() && 2==args.size() &&
                           isSgAsmRegisterReferenceExpression(args[0]) && isSgAsmRegisterReferenceExpression(args[1])) {
                    RegisterDescriptor dst_reg = isSgAsmRegisterReferenceExpression(args[0])->get_descriptor();
                    RegisterDescriptor src_reg = isSgAsmRegisterReferenceExpression(args[1])->get_descriptor();
                    do_monitor = dst_reg.get_major()==x86_regclass_gpr && dst_reg.get_minor()==x86_gpr_bp &&
                                 src_reg.get_major()==x86_regclass_gpr && src_reg.get_minor()==x86_gpr_sp; // mov ebp, esp
                } else if (args.size()>=1 && isSgAsmRegisterReferenceExpression(args[0])) {
                    RegisterDescriptor reg = isSgAsmRegisterReferenceExpression(args[0])->get_descriptor();
                    do_monitor = reg.get_major()==x86_regclass_gpr && reg.get_minor()==x86_gpr_sp; // e.g., sub esp, 0x44
                }
            }
            if (!do_monitor) {
                // this is the first instruction of this function that is no longer setting up the stack frame
                stack_frame.monitoring_stdcall = false;
                stack_frame.stdcall_args_va = state.registers.gpr[x86_gpr_sp].known_value();
                if (params.verbosity>=EFFUSIVE)
                    std::cerr <<"CloneDetection: stdcall args start at "
                              <<StringUtility::addrToString(stack_frame.stdcall_args_va) <<"\n";
            }
        }
    }

    // Skips over a function call by effectively executing a RET instruction.  The current EIP is already inside the function
    // being called and 'insn' is the instruction that caused the call.  The StackFrame for the callee has not yet been created
    // in the stack_frames data member.
    //
    // For caller-cleanup (like the cdecl convention) this is easy: all we need to do is simulate a RET.
    //
    // For calleee-cleanup (like the stdcall convention) this is a lot trickier and basically comes down to two cases.  If we
    // have the callee's code, we can find one of its return blocks and look at the argument for RET. If we don't have the
    // callee's code, we might be able to figure something out by looking at how the caller manipulates its stack.
    rose_addr_t skip_call(SgAsmInstruction *insn) {
        assert(!stack_frames.empty());
        StackFrame &caller_frame = stack_frames.top();
        rose_addr_t esp = state.registers.gpr[x86_gpr_sp].known_value(); // stack address of the return address
        assert(caller_frame.entry_esp>=esp);
        rose_addr_t ret_va = this->template readMemory<32>(x86_segreg_ss, ValueType<32>(esp), ValueType<1>(1)).known_value();
        if (!uses_stdcall) {
            state.registers.gpr[x86_gpr_sp] = ValueType<32>(esp+4);
        } else {
            rose_addr_t callee_va = state.registers.ip.known_value();
            SgAsmFunction *callee = SageInterface::getEnclosingNode<SgAsmFunction>(insns->get_instruction(callee_va));
            if (callee) {
                // We have the callee's code, so find one of its return blocks. (FIXME: we should cache this)
                struct T1: AstSimpleProcessing {
                    size_t nbytes;
                    T1(): nbytes(0) {}
                    void visit(SgNode *node) {
                        if (SgAsmX86Instruction *ret = isSgAsmX86Instruction(node)) {
                            const SgAsmExpressionPtrList &args = ret->get_operandList()->get_operands();
                            if (x86_ret==ret->get_kind() && 1==args.size() && isSgAsmIntegerValueExpression(args[0]))
                                nbytes = isSgAsmIntegerValueExpression(args[0])->get_absoluteValue();
                        }
                    }
                } t1;
                t1.traverse(callee, preorder);
                state.registers.gpr[x86_gpr_sp] = ValueType<32>(esp+4+t1.nbytes);
            } else if (0!=caller_frame.stdcall_args_va) {
                // We don't have the callee's code, but we've computed the stdcall arguments position
                state.registers.gpr[x86_gpr_sp] = ValueType<32>(caller_frame.stdcall_args_va);
            } else {
                // We have no idea how much to pop, so just pop the return address and don't try to pop any args.
                state.registers.gpr[x86_gpr_sp] = ValueType<32>(esp+4);
            }
        }
        state.registers.ip = ValueType<32>(ret_va);
        tracer.emit(insn->get_address(), EV_RETURNED);
        return ret_va;
    }

    // Return the word at the specified stack location as if the specimen had read the stack.  The argno is the 32-bit word
    // offset from the current ESP value.
    uint32_t stack(size_t argno) {
        rose_addr_t stack_va = readRegister<32>("esp").known_value() + 4 + 4*argno; // avoid return value
        ValueType<32> word = readMemory<32>(x86_segreg_ss, ValueType<32>(stack_va), this->true_());
        return word.known_value();
    }

    // Returns true if the instruction is of the form "jmp [ebx+OFFSET]"
    bool is_thunk_ebx(SgAsmX86Instruction *insn, rose_addr_t *offset_ptr/*out*/) {
        const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
        if (x86_jmp!=insn->get_kind() || 1!=args.size())
            return false;
        SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(args[0]);
        if (!mre)
            return false;
        SgAsmBinaryAdd *add = isSgAsmBinaryAdd(mre->get_address());
        if (!add)
            return false;
        SgAsmRegisterReferenceExpression *reg = isSgAsmRegisterReferenceExpression(add->get_lhs());
        if (!reg || reg->get_descriptor()!=RegisterDescriptor(x86_regclass_gpr, x86_gpr_bx, 0, 32))
            return false;
        SgAsmIntegerValueExpression *offset = isSgAsmIntegerValueExpression(add->get_rhs());
        if (!offset)
            return false;
        if (offset_ptr)
            *offset_ptr = offset->get_absoluteValue();
        return true;
    }

    SgAsmGenericHeader *header_for_va(rose_addr_t va) {
        const SgAsmGenericHeaderPtrList &hdrs = interp->get_headers()->get_headers();
        for (size_t i=0; i<hdrs.size(); ++i) {
            if (hdrs[i]->get_best_section_by_va(va, false))
                return hdrs[i];
        }
        return NULL;
    }

    // Called by instruction semantics before each instruction is executed
    void startInstruction(SgAsmInstruction *insn_) ROSE_OVERRIDE {
        if (ninsns++ >= params.timeout)
            throw FaultException(AnalysisFault::INSN_LIMIT);
        SgAsmX86Instruction *insn = isSgAsmX86Instruction(insn_);
        assert(insn!=NULL);
        SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(insn);
        assert(func!=NULL);
        if (params.compute_coverage)
            insn_coverage.execute(insn);

        // Adjust stack frames, popping stale frames and adding new ones
        rose_addr_t esp = state.registers.gpr[x86_gpr_sp].known_value();
        stack_frames.pop_to(esp);
        bool entered_function = stack_frames.push_if(insn, esp);
        if (stack_frames.empty())
            throw FaultException(AnalysisFault::BAD_STACK);
        StackFrame &stack_frame = stack_frames.top();
        if (entered_function) {
            AddressIdMap::const_iterator id_found = entry2id.find(insn->get_address());
            if (id_found!=entry2id.end() && 1!=stack_frames.size()) { // ignore top-level call from the test itself
                ++funcinfo[id_found->second].ncalls;
                // update dynamic callgraph info
                if (params.compute_callgraph && (!params.top_callgraph || 2==stack_frames.size())) {
                    AddressIdMap::const_iterator caller_id_found = entry2id.find(stack_frames[1].func->get_entry_va());
                    if (caller_id_found!=entry2id.end())
                        dynamic_cg.call(caller_id_found->second, id_found->second);
                }
            }
        }

        // Debugging
        if (params.verbosity>=EFFUSIVE) {
            std::string funcname = func->get_name();
            if (!funcname.empty())
                funcname = " <" + funcname + ">";
            std::cerr <<"CloneDetection: " <<std::string(80, '-') <<"\n"
                      <<"CloneDetection: in function " <<StringUtility::addrToString(func->get_entry_va()) <<funcname
                      <<" at level " <<stack_frames.size() <<"\n"
                      <<"CloneDetection: stack ptr: " <<StringUtility::addrToString(stack_frame.entry_esp)
                      <<" - " <<StringUtility::signedToHex2(stack_frame.entry_esp-esp, 32)
                      <<" = " <<StringUtility::addrToString(esp) <<"\n";
        }

        // Decide whether this function is allowed to call (via CALL, JMP, fall-through, etc) other functions.
        if (entered_function) {
            switch (params.follow_calls) {
                case CALL_ALL:
                    stack_frame.follow_calls = true;
                    break;
                case CALL_NONE:
                    stack_frame.follow_calls = false;
                    break;
                case CALL_BUILTIN:
                    // If the called function is whitelisted, then it should not call anything except other whitelisted
                    // functions; otherwise it can call other stuff.  The logic here works in conjunction with the logic
                    // in finishInstruction().
                    if (whitelist_exports.find(insn->get_address())!=whitelist_exports.end()) {
                        stack_frame.follow_calls = false;
                        if (params.verbosity>=EFFUSIVE) {
                            std::cerr <<"CloneDetection: turning off follow-calls for the duration of this"
                                      <<" white-listed function\n";
                        }
                    } else {
                        stack_frame.follow_calls = true;
                    }
                    break;
            }
        }

        // Special handling for thunks that use the form "jmp [ebx+OFFSET]". The ebx register needs to be loaded with the
        // address of the .got.plt before executing this function.
        rose_addr_t got_offset = 0;
        if (is_thunk_ebx(insn, &got_offset) &&
            0==(state.register_rw_state.gpr[x86_gpr_bx].state & (HAS_BEEN_WRITTEN|HAS_BEEN_READ))) {
            SgAsmGenericHeader *fhdr = header_for_va(insn->get_address());
            SgAsmGenericSection *gotplt = fhdr ? fhdr->get_section_by_name(".got.plt") : NULL;
            if (gotplt && gotplt->is_mapped() && got_offset+4<=gotplt->get_size()) {
                state.registers.gpr[x86_gpr_bx] = ValueType<32>(gotplt->get_mapped_actual_va());
                state.register_rw_state.gpr[x86_gpr_bx].state |= HAS_BEEN_INITIALIZED;
                if (params.verbosity>=EFFUSIVE) {
                    std::cerr <<"CloneDetection: special handling for thunk"
                              <<" at " <<StringUtility::addrToString(insn->get_address())
                              <<": set EBX = " <<StringUtility::addrToString(gotplt->get_mapped_actual_va()) <<"\n";
                }
            }
        }

        // Update some necessary things
        state.registers.ip = ValueType<32>(insn->get_address());
        monitor_esp(insn, stack_frame);
        if (params.verbosity>=EFFUSIVE)
            std::cerr <<"CloneDetection: executing: " <<unparseInstructionWithAddress(insn) <<"\n";
        SgAsmBlock *bb = SageInterface::getEnclosingNode<SgAsmBlock>(insn);
        if (bb!=prev_bb)
            tracer.emit(insn->get_address(), EV_REACHED);
        prev_bb = bb;

        Super::startInstruction(insn_);
    }

    // Called by instruction semantics after each instruction is executed. Stack frames are not updated until the next
    // call to startInstruction().
    void finishInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE {
        SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(insn);
        assert(insn_x86!=NULL);
        state.output_group.set_ninsns(1+state.output_group.get_ninsns());
        rose_addr_t next_eip = state.registers.ip.known_value();
        SgAsmInstruction *next_insn = insns->get_instruction(next_eip);
        SgAsmFunction *next_func = SageInterface::getEnclosingNode<SgAsmFunction>(next_insn);
        bool has_returned = false;

        // If the test branches (via CALL or JMP or other) to a dynamically-loaded function that has not been linked into the
        // address space, then we need to do something about it or we'll likely get a disassemble failure on the next
        // instruction (because the addresses in the .got.plt table are invalid).  Therefore, before we ran the test we
        // initialized the .got.plt with the GOTPLT_VALUE that we recognize here.  When we detect that the next instruction is
        // at the GOTPLT_VALUE address, we consume an input value and place it in EAX (the return value), then pop the return
        // address off the stack and load it into EIP.
        if (!has_returned && GOTPLT_VALUE==next_eip) {
            if (params.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: special handling for non-linked dynamic function\n";
            this->writeRegister("eax", next_input_value<32>(IQ_FUNCTION));
            if (params.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: consumed function input; EAX = " <<state.registers.gpr[x86_gpr_ax] <<"\n";
            next_eip = skip_call(insn);
            has_returned = true;
        }

        // If we're supposed to be skiping over calls and we've entered another function (which is not whitelisted), then
        // immediately return.  This implementation (rather than looking for CALL instructions) handles more cases since
        // functions are not always called via x86 CALL instruction -- sometimes the compiler will call using JMP, allowing the
        // "jumpee" function to inherit the stack frame (and in particular, the return address) of the jumper.  This also
        // avoids the problem where a CALL instruction is used to obtain the EIP value in position-independent code.
        const StackFrame &caller_frame = stack_frames.top(); // we haven't pushed the callee's frame yet (see startInstruction)
        if (!has_returned && !caller_frame.follow_calls && next_func && next_func->get_entry_va()==next_eip) {
            if (params.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: special handling for function call (fall through and return via EAX)\n";
#ifdef OUTPUTGROUP_SAVE_CALLGRAPH
            AddressIdMap::const_iterator found = entry2id.find(next_eip);
            if (found!=entry2id.end())
                state.output_group.callee_ids.push_back(found->second);
#endif
            next_eip = skip_call(insn);
            has_returned = true;
            this->writeRegister("eax", next_input_value<32>(IQ_FUNCTION));
        }

        // Special handling for indirect calls whose target is not a valid instruction.  Treat the call as if it were to a
        // black box function, skipping the call.
        if (!has_returned && x86_call==insn_x86->get_kind()) {
            const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
            if (args.size()==1 && NULL==isSgAsmValueExpression(args[0]) && NULL==next_insn) {
                if (params.verbosity>=EFFUSIVE)
                    std::cerr <<"CloneDetection: special handling for invalid indirect function call\n";
                next_eip = skip_call(insn);
                has_returned = true;
                this->writeRegister("eax", next_input_value<32>(IQ_FUNCTION));
            }
        }

        // The way monitor_esp() looks for the the stdcall_args_va value often doesn't work because the compiler doesn't always
        // work.  Therefore, we'll monitor "RET n" instructions and make adjustments.
        if (uses_stdcall && x86_ret==insn_x86->get_kind()) {
            const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
            if (args.size()==1 && isSgAsmIntegerValueExpression(args[0])) {
                rose_addr_t esp = state.registers.gpr[x86_gpr_sp].known_value(); // after the RET has executed
                for (size_t i=0; i<stack_frames.size(); ++i) {
                    StackFrame &sf = stack_frames[i];
                    if (sf.entry_esp>=esp && sf.stdcall_args_va!=esp) {
                        sf.stdcall_args_va = esp;
                        if (params.verbosity>=EFFUSIVE) {
                            std::cerr <<"CloneDetection: adjusted function "
                                      <<StringUtility::addrToString(sf.func->get_entry_va()) <<" stdcall args start to "
                                      <<StringUtility::addrToString(sf.stdcall_args_va) <<"\n";
                        }
                    }
                }
            }
        }

        // Emit an event if the next instruction to execute is not at the fall through address.
        rose_addr_t fall_through_va = insn->get_address() + insn->get_size();
        if (next_eip!=fall_through_va)
            tracer.emit(insn->get_address(), EV_BRANCHED, next_eip);
#if 0 /*DEBUGGING [Robb P. Matzke 2013-12-20]*/
        std::cerr <<state;
#endif

        Super::finishInstruction(insn);
    }

    // Handle INT 0x80 instructions: save the system call number (from EAX) in the output group and set EAX to a random
    // value, thus consuming one input.
    void interrupt(uint8_t inum) ROSE_OVERRIDE {
        if (0x80==inum) {
            if (params.verbosity>=EFFUSIVE)
                std::cerr <<"CloneDetection: special handling for system call (fall through and consume an input into EAX)\n";
#ifdef OUTPUTGROUP_SAVE_SYSCALLS
            ValueType<32> syscall_num = this->template readRegister<32>("eax");
            state.output_group.syscalls.push_back(syscall_num.known_value());
#endif
            this->writeRegister("eax", next_input_value<32>(IQ_FUNCTION));
        } else {
            Super::interrupt(inum);
            throw FaultException(AnalysisFault::INTERRUPT);
        }
    }

    // Handle the HLT instruction by throwing an exception.
    void hlt() {
        throw FaultException(AnalysisFault::HALT);
    }

    // Reads from memory without updating memory. Uninitialized bytes are read as undefined values. If any uninitialized bytes
    // are read then "uninitialized_read" is set if non-null (it is never cleared).
    template<size_t nBits>
    std::vector<ValueType<8> > readMemoryWithoutUpdate(X86SegmentRegister sr, ValueType<32> a0,
                                                       bool *uninitialized_read /*in,out*/) {
        assert(8==nBits || 16==nBits || 32==nBits);
        std::vector<ValueType<8> > retval;

        if (nBits>=0) {
            retval.push_back(state.mem_read_byte(sr, a0));
        }
        if (nBits>=16) {
            ValueType<32> a1 = this->add(a0, ValueType<32>(1));
            retval.push_back(state.mem_read_byte(sr, a1));
        }
        if (nBits>=24) {
            ValueType<32> a2 = this->add(a0, ValueType<32>(2));
            retval.push_back(state.mem_read_byte(sr, a2));
        }
        if (nBits>=32) {
            ValueType<32> a3 = this->add(a0, ValueType<32>(3));
            retval.push_back(state.mem_read_byte(sr, a3));
        }

        if (uninitialized_read) {
            for (size_t i=0; i<retval.size(); ++i) {
                if (!retval[i].is_known()) {
                    *uninitialized_read = true;
                    break;
                }
            }
        }

        return retval;
    }

    // Concatenate bytes in little-endian order.
    template<size_t nBits>
    ValueType<nBits> concatBytes(const std::vector<ValueType<8> > &bytes) {
        assert(nBits==8*bytes.size());
        if (8==nBits)
            return bytes[0];
        if (16==nBits)
            return this->concat(bytes[0], bytes[1]);
        if (24==nBits)
            return this->concat(this->concat(bytes[0], bytes[1]), bytes[2]);
        if (32==nBits)
            return this->concat(this->concat(bytes[0], bytes[1]),
                                this->concat(bytes[2], bytes[3]));
        assert(8==nBits || 16==nBits || 24==nBits || 32==nBits);
        abort();
    }

    // Track memory access.
    template<size_t nBits>
    ValueType<nBits> readMemory(X86SegmentRegister sr, ValueType<32> a0, const ValueType<1> &cond) {
        assert(8==nBits || 16==nBits || 32==nBits);
        bool uninitialized_read = false;
        std::vector<ValueType<8> > bytes = readMemoryWithoutUpdate<nBits>(sr, a0, &uninitialized_read);

        if (uninitialized_read) {
            // At least one of the bytes read did not previously exist, so consume an input value
            ValueType<nBits> ivalue;
            MemoryMap *map = this->interp ? this->interp->get_map() : NULL;
            rose_addr_t addr = a0.known_value();
            rose_addr_t ebp = state.registers.gpr[x86_gpr_bp].known_value();
            bool ebp_is_stack_frame = ebp>=params.initial_stack-16*4096 && ebp<params.initial_stack;
            if (ebp_is_stack_frame && addr>=ebp+8 && addr<ebp+8+40) {
                // This is probably an argument to the function, so consume an argument input value.
                ivalue = next_input_value<nBits>(IQ_ARGUMENT, addr);
            } else if (ebp_is_stack_frame && addr>=ebp-8192 && addr<ebp+8) {
                // This is probably a local stack variable
                ivalue = next_input_value<nBits>(IQ_LOCAL, addr);
            } else if (this->get_map() && this->get_map()->at(addr).limit(4).available()==4) {
                // Memory is read only, so we don't need to consume a value.
                int32_t buf=0;
                this->get_map()->at(addr).limit(4).read((uint8_t*)&buf);
                ivalue = ValueType<nBits>(buf);
            } else if (map!=NULL && map->at(addr).exists()) {
                // Memory mapped from a file, thus probably a global variable, function pointer, etc.
                ivalue = next_input_value<nBits>(IQ_GLOBAL, addr);
            } else if (this->pointers!=NULL && this->pointers->is_pointer(SymbolicSemantics::ValueType<32>(addr))) {
                // Pointer detection analysis says this address is a pointer
                ivalue = next_input_value<nBits>(IQ_POINTER, addr);
            } else if (address_hasher_initialized && map!=NULL && map->at(addr).exists()) {
                // Use memory that was already initialized with values
                ivalue = next_input_value<nBits>(IQ_MEMHASH, addr);
            } else {
                // Unknown classification
                ivalue = next_input_value<nBits>(IQ_INTEGER, addr);
            }

#if 1 /* [Robb P. Matzke 2014-01-02]: New behavior */
            // Write the value back to memory so the same value is read next time, but only write to the bytes that are not
            // already initialized.
            std::vector<size_t> first_of_n(bytes.size(), 0);
            {
                size_t nUnknown = 0;
                for (size_t i=bytes.size(); i>0; --i) {
                    if (!bytes[i-1].is_known()) {
                        ++nUnknown;
                    } else if (nUnknown>0) {
                        first_of_n[i] = nUnknown;
                        nUnknown = 0;
                    }
                }
                first_of_n[0] = nUnknown;
            }

            if (!bytes[0].is_known()) {
                bytes[0] = this->template extract<0, 8>(ivalue);
                state.mem_write_byte(sr, a0, bytes[0], first_of_n[0], HAS_BEEN_READ);
            }
            if (bytes.size()>=2 && !bytes[1].is_known()) {
                bytes[1] = this->template extract<8, 16>(ivalue);
                ValueType<32> a1 = this->add(a0, ValueType<32>(1));
                state.mem_write_byte(sr, a1, bytes[1], first_of_n[1], HAS_BEEN_READ);
            }
            if (bytes.size()>=3 && !bytes[2].is_known()) {
                bytes[2] = this->template extract<16, 24>(ivalue);
                ValueType<32> a2 = this->add(a0, ValueType<32>(2));
                state.mem_write_byte(sr, a2, bytes[2], first_of_n[2], HAS_BEEN_READ);
            }
            if (bytes.size()>=3 && !bytes[3].is_known()) {
                bytes[3] = this->template extract<24, 32>(ivalue);
                ValueType<32> a3 = this->add(a0, ValueType<32>(3));
                state.mem_write_byte(sr, a3, bytes[3], first_of_n[3], HAS_BEEN_READ);
            }
#else /* old behavior */
            // Write the new input value into memory, possibly overwriting some bytes that were already initialized.
            writeMemory(sr, a0, ivalue, cond, HAS_BEEN_READ);
#endif
        }

        ValueType<nBits> retval = concatBytes<nBits>(bytes);
        return retval;
    }

    template<size_t nBits>
    void writeMemory(X86SegmentRegister sr, ValueType<32> a0, ValueType<nBits> data, const ValueType<1> &cond,
                     unsigned rw_state=HAS_BEEN_WRITTEN) {
        // Some operations produce undefined values according to the x86 ISA specification. For example, certain flag bits are
        // sometimes unspecified, as is the result of certain kinds of shift operations when the shift amount is large. In
        // order to stay in the concrete domain, we always choose a value of zero when this happens.
        if (!data.is_known())
            data = ValueType<nBits>(0);

        // Add the address/value pair to the memory state one byte at a time in little-endian order.
        assert(8==nBits || 16==nBits || 32==nBits);
        size_t nbytes = nBits / 8;
        ValueType<8> b0 = this->template extract<0, 8>(data);
        state.mem_write_byte(sr, a0, b0, nbytes, rw_state);
        if (nBits>=16) {
            ValueType<32> a1 = this->add(a0, ValueType<32>(1));
            ValueType<8> b1 = this->template extract<8, 16>(data);
            state.mem_write_byte(sr, a1, b1, 0, rw_state);
        }
        if (nBits>=24) {
            ValueType<32> a2 = this->add(a0, ValueType<32>(2));
            ValueType<8> b2 = this->template extract<16, 24>(data);
            state.mem_write_byte(sr, a2, b2, 0, rw_state);
        }
        if (nBits>=32) {
            ValueType<32> a3 = this->add(a0, ValueType<32>(3));
            ValueType<8> b3 = this->template extract<24, 32>(data);
            state.mem_write_byte(sr, a3, b3, 0, rw_state);
        }
        if (state.is_memory_output(a0.known_value())) {
            if (params.verbosity>=EFFUSIVE) {
                std::cerr <<"CloneDetection: potential output value mem[" <<a0 <<"]=" <<data <<"\n";
                rose_addr_t v2 = state.hash_if_string(data.known_value(), interp->get_map());
                if (v2!=data.known_value())
                    std::cerr <<"CloneDetection: output value is a string pointer; hash="<<StringUtility::addrToString(v2)<<"\n";
            }
            tracer.emit(this->get_insn()->get_address(), EV_MEM_WRITE, a0.known_value(), data.known_value());
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
        // If we're reading the EAX register since the last return from a function call and before we write to EAX, then
        // that function must have returned a value.
        if (reg.get_major()==x86_regclass_gpr && reg.get_minor()==x86_gpr_ax && !stack_frames.empty()) {
            if (SgAsmFunction *last_call = stack_frames.top().last_call) {
                AddressIdMap::const_iterator found = entry2id.find(last_call->get_entry_va());
                if (found!=entry2id.end()) {
                    ++funcinfo[found->second].nretused;
                    if (params.verbosity>=EFFUSIVE) {
                        std::cerr <<"CloneDetection: function #" <<found->second
                                  <<" " <<StringUtility::addrToString(last_call->get_entry_va())
                                  <<" <" <<last_call->get_name() <<"> returns a value\n";
                    }
                }
                stack_frames.top().last_call = NULL; // end of function return value analysis
            }
        }

        // Now the real work of reading a register...
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
                    state.registers.flag[reg.get_offset()] = next_input_value<1>(IQ_INTEGER);
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
                    state.registers.gpr[reg.get_minor()] = next_input_value<32>(IQ_INTEGER);
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
                            state.registers.segreg[reg.get_minor()] = next_input_value<16>(IQ_INTEGER);
                        retval = this->template unsignedExtend<16, nBits>(state.registers.segreg[reg.get_minor()]);
                        break;
                    }
                    case x86_regclass_gpr: {
                        if (reg.get_minor()>=state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        bool never_accessed = 0==state.register_rw_state.gpr[reg.get_minor()].state;
                        state.register_rw_state.segreg[reg.get_minor()].state |= HAS_BEEN_READ;
                        if (never_accessed)
                            state.registers.gpr[reg.get_minor()] = next_input_value<32>(IQ_INTEGER);
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
                                state.registers.flag[i] = next_input_value<1>(IQ_INTEGER);
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
                        if (never_accessed) {
                            rose_addr_t fake_arg_addr = 0;
                            if (1==stack_frames.size()) {
                                // This function might be using a different calling convention where arguments are passed
                                // in registers.  So pretend we're reading an argument from the stack instead of a register.
                                switch (reg.get_minor()) {
                                    case x86_gpr_ax: fake_arg_addr = stack_frames.top().entry_esp + 4; break; // first arg
                                    case x86_gpr_dx: fake_arg_addr = stack_frames.top().entry_esp + 8; break; // second
                                    case x86_gpr_cx: fake_arg_addr = stack_frames.top().entry_esp + 0xc; break; // third
                                }
                            }
                            if (fake_arg_addr!=0) {
                                state.registers.gpr[reg.get_minor()] = next_input_value<32>(IQ_ARGUMENT, fake_arg_addr);
                            } else {
                                state.registers.gpr[reg.get_minor()] = next_input_value<32>(IQ_INTEGER);
                            }
                        }
                        retval = this->template unsignedExtend<32, nBits>(state.registers.gpr[reg.get_minor()]);
                        break;
                    }
                    case x86_regclass_ip: {
                        if (reg.get_minor()!=0)
                            throw Exception("register not implemented in semantic policy");
                        bool never_accessed = 0==state.register_rw_state.ip.state;
                        state.register_rw_state.ip.state |= HAS_BEEN_READ;
                        if (never_accessed)
                            state.registers.ip = next_input_value<32>(IQ_POINTER);
                        retval = this->template unsignedExtend<32, nBits>(state.registers.ip);
                        break;
                    }
                    case x86_regclass_segment: {
                        if (reg.get_minor()>=state.registers.n_segregs || reg.get_nbits()!=16)
                            throw Exception("register not implemented in semantic policy");
                        bool never_accessed = 0==state.register_rw_state.segreg[reg.get_minor()].state;
                        state.register_rw_state.segreg[reg.get_minor()].state |= HAS_BEEN_READ;
                        if (never_accessed)
                            state.registers.segreg[reg.get_minor()] = next_input_value<16>(IQ_INTEGER);
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
                                state.registers.flag[i] = next_input_value<1>(IQ_INTEGER);
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
    void writeRegister(const RegisterDescriptor &reg, ValueType<nBits> value, unsigned update_access=HAS_BEEN_WRITTEN) {
        // Some operations produce undefined values according to the x86 ISA specification. For example, certain flag bits are
        // sometimes unspecified, as is the result of certain kinds of shift operations when the shift amount is large. In
        // order to stay in the concrete domain, we always choose a value of zero when this happens.
        if (!value.is_known())
            value = ValueType<nBits>(0);

        if (reg.get_major()==x86_regclass_gpr && reg.get_minor()==x86_gpr_ax && !stack_frames.empty())
            stack_frames.top().last_call = NULL; // end of function return value analysis
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
                    state.registers.gpr[reg.get_minor()] = next_input_value<32>(IQ_INTEGER);
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
                            state.registers.gpr[reg.get_minor()] = next_input_value<32>(IQ_INTEGER);
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
    void print(std::ostream &o) const {
        BaseSemantics::Formatter fmt;
        print(o, fmt);
    }
    void print(std::ostream &o, BaseSemantics::Formatter &fmt) const {
        state.print(o, fmt);
    }

    friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
        p.print(o);
        return o;
    }
};

/** Open the specimen binary file and return its primary, most interesting interpretation. */
SgAsmInterpretation *open_specimen(const std::string &specimen_name, const std::string &argv0, bool do_link);

/** Open a specimen that was already added to the database. Parse the specimen using the same flags as when it was added. This
 * is the fallback method when an AST is not saved in the database. */
SgProject *open_specimen(const SqlDatabase::TransactionPtr&, FilesTable&, int specimen_id, const std::string &argv0);

/** Links exports with imports. The exports provided by @p exports_header are linked into the dynamic linking slots in the @p
 *  imports_header by modifying memory pointed to by the @p map. */
void link_builtins(SgAsmGenericHeader *imports_header, SgAsmGenericHeader *exports_header, MemoryMap *map);

/** Start the command by adding a new entry to the semantic_history table. Returns the hashkey ID for this command. */
int64_t start_command(const SqlDatabase::TransactionPtr&, int argc, char *argv[], const std::string &desc, time_t begin=0);

/** Called just before a command's final commit. The @p hashkey should be the value returned by start_command().
 *  The description can be updated if desired. */
void finish_command(const SqlDatabase::TransactionPtr&, int64_t hashkey, const std::string &desc="");

/** Return the name of the file that contains the specified header.  If basename is true then return only the base name, not
 *  any directory components. */
std::string filename_for_header(SgAsmGenericHeader*, bool basename=false);

/** Return the name of the file that contains the specified function.  If basename is true then return only the base name, not
 *  any directory components. */
std::string filename_for_function(SgAsmFunction*, bool basename=false);

/** Returns the functions that don't exist in the database. Of those function listed in @p functions, return those which
 *  are not present in the database.  The returned std::map's key is the ID number to be assigned to that function when it
 *  is eventually added to the database.  The internal representation of the FilesTable is updated with the names of
 *  the files that aren't yet in the database. */
IdFunctionMap missing_functions(const SqlDatabase::TransactionPtr&, CloneDetection::FilesTable&,
                                const std::vector<SgAsmFunction*> &functions);

/** Returns the functions that exist in the database.  Of those functions mentioned in @p functions, return those which
 *  are present in the database.  The returned std::map keys are the ID numbers for those functions. */
IdFunctionMap existing_functions(const SqlDatabase::TransactionPtr&, CloneDetection::FilesTable&,
                                 const std::vector<SgAsmFunction*> &functions);

/** Save binary data in the database. The data is saved under a hashkey which is the 20-byte (40 hexadecimal characters)
 *  SHA1 digest of the data.  The data is then split into chunks, encoded in base64, and saved one chunk per row in the
 *  semantic_binaries table. The 40-character hash key is returned.
 * @{ */
std::string save_binary_data(const SqlDatabase::TransactionPtr &tx, int64_t cmd_id, const std::string &filename);
std::string save_binary_data(const SqlDatabase::TransactionPtr &tx, int64_t cmd_id, const uint8_t *data, size_t nbytes);
/** @} */

/** Download binary data from the database.  The data is saved in the specified file, or a new file is created.  The name
 *  of the file is returned.  The file will be empty if the specified hash key is not present in the database. */
std::string load_binary_data(const SqlDatabase::TransactionPtr &tx, const std::string &hashkey, std::string filename="");

/** Save the binary representation of the AST into the database A 40-character hash key is returned which identifies the
 *  saved data. */
std::string save_ast(const SqlDatabase::TransactionPtr&, int64_t cmd_id);

/** Loads the specified AST from the database. Replaces any existing AST. */
SgProject *load_ast(const SqlDatabase::TransactionPtr&, const std::string &hashkey);

/** Identifying string for function.  Includes function address, and in angle brackets, the database function ID if known, the
 *  function name if known, and file name if known. */
std::string function_to_str(SgAsmFunction*, const FunctionIdMap&);

/** Returns the true if this function probably returns a value (rather than void).  If a function never writes to the EAX
 * register, then it has a zero probability of returning a value.  If a caller reads EAX after calling the function but before
 * writing to EAX, then the function has a probability of one that it returned a value.  This function computes the average of
 * all the return value analysis results, normalizes it to a probability [0,1], and compares it to the user-supplied
 * threshold.
 * @{ */
double function_returns_value(const SqlDatabase::TransactionPtr&, int func_id);
std::map<int/*func_id*/, double/*probability*/> function_returns_value(const SqlDatabase::TransactionPtr&);
bool function_returns_value_p(const SqlDatabase::TransactionPtr &tx, int func_id, double threshold=0.51);
/** @} */

} // namespace
#endif
