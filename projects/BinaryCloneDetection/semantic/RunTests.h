#ifndef ROSE_CloneDetection_RunTests_H
#define ROSE_CloneDetection_RunTests_H

#include "rose.h"

#include "CloneDetectionLib.h"
#include "rose_getline.h"
#include "vectorCompression.h"

namespace CloneDetection {
namespace RunTests {

typedef CloneDetection::Policy<State, PartialSymbolicSemantics::ValueType> ClonePolicy;
typedef X86InstructionSemantics<ClonePolicy, PartialSymbolicSemantics::ValueType> CloneSemantics;
typedef std::set<std::string> NameSet;

enum PathSyntactic {PATH_SYNTACTIC_NONE, PATH_SYNTACTIC_ALL, PATH_SYNTACTIC_FUNCTION };

struct Switches {
    Switches()
        : verbosity(SILENT), progress(false), pointers(false), interactive(false), trace_events(0), dry_run(false),
          save_coverage(false), save_callgraph(false), save_consumed_inputs(false), nprocs(1),
          path_syntactic(PATH_SYNTACTIC_NONE) {
        checkpoint = 300 + LinearCongruentialGenerator()()%600;
    }
    Verbosity verbosity;                        // semantic policy has a separate verbosity
    bool progress;
    bool pointers;
    time_t checkpoint;
    bool interactive;
    unsigned trace_events;
    bool dry_run;
    std::string input_file_name;
    bool save_coverage;
    bool save_callgraph;
    bool save_consumed_inputs;
    PolicyParams params;                                // parameters controlling instruction semantics
    size_t nprocs;                                      // number of parallel processes to fork
    std::vector<std::string> signature_components;      /**< How should the signature vectors be computed */
    PathSyntactic path_syntactic;                       /**< How to compute path sensistive syntactic signature */
};

struct WorkItem {
    int specimen_id, func_id, igroup_id;
    WorkItem(): specimen_id(-1), func_id(-1), igroup_id(-1) {}
    WorkItem(int specimen_id, int func_id, int igroup_id): specimen_id(specimen_id), func_id(func_id), igroup_id(igroup_id) {}
    bool operator<(const WorkItem&) const;
    bool operator==(const WorkItem&) const;
};

std::ostream& operator<<(std::ostream&, const WorkItem&);

typedef std::vector<WorkItem> Work;
typedef std::vector<Work> MultiWork;
typedef std::map<SgAsmFunction*, PointerDetector*> PointerDetectors;

extern Switches opt;
extern std::string argv0;
extern int interrupted;

void usage(int exit_status);
int parse_commandline(int argc, char *argv[]);
Work load_work(const std::string &filename, FILE*);
CloneDetection::PointerDetector* detect_pointers(SgAsmFunction*, const CloneDetection::FunctionIdMap&);
void sig_handler(int signo);
void add_builtin_functions(NameSet&);
SgAsmGenericHeader* header_for_va(SgAsmInterpretation*, rose_addr_t va);
rose::BinaryAnalysis::Disassembler::AddressSet get_import_addresses(SgAsmInterpretation*, const NameSet &whitelist_names);
void overmap_dynlink_addresses(SgAsmInterpretation*, const InstructionProvidor&, FollowCalls, MemoryMap *ro_map /*in,out*/,
                               rose_addr_t special_value, const rose::BinaryAnalysis::Disassembler::AddressSet &whitelist_imports,
                               rose::BinaryAnalysis::Disassembler::AddressSet &whitelist /*out*/);
OutputGroup fuzz_test(SgAsmInterpretation *interp, SgAsmFunction *function, InputGroup &inputs, Tracer &tracer,
                      const InstructionProvidor &insns, MemoryMap *ro_map, const PointerDetector *pointers,
                      const AddressIdMap &entry2id, const rose::BinaryAnalysis::Disassembler::AddressSet &whitelist_exports,
                      FuncAnalyses &funcinfo, InsnCoverage &insn_coverage, DynamicCallGraph &dynamic_cg,
                      ConsumedInputs &consumed_inputs);
SqlDatabase::TransactionPtr checkpoint(const SqlDatabase::TransactionPtr &tx, OutputGroups &ogroups, Tracer &tracer,
                                       InsnCoverage &insn_coverage, DynamicCallGraph &dynamic_cg, ConsumedInputs &consumed_inputs,
                                       Progress *progress, size_t ntests_ran, int64_t cmd_id);
void runOneTest(SqlDatabase::TransactionPtr tx, const WorkItem &workItem, PointerDetectors &pointers, SgAsmFunction *func,
                const FunctionIdMap &function_ids, InsnCoverage &insn_coverage /*in,out*/, DynamicCallGraph &dynamic_cg /*in,out*/,
                Tracer &tracer /*in,out*/, ConsumedInputs &consumed_inputs /*in,out*/, SgAsmInterpretation *interp,
                const rose::BinaryAnalysis::Disassembler::AddressSet &whitelist_exports, int64_t cmd_id, InputGroup &igroup,
                FuncAnalyses funcinfo, const InstructionProvidor &insns, MemoryMap *ro_map, const AddressIdMap &entry2id,
                OutputGroups &ogroups /*in,out*/);

} // namespace
} // namespace

#endif
